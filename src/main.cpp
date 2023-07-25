#include <cassert>
#include <cstdio>

#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <thread>

#include <gsl/util>

#include <pl/timer.hpp>

#include "database_connection.hpp"
#include "load_emails.hpp"

#define SQLITE_DATABASE_FILE_NAME "test_database.db"
#define SQLITE_VFS nullptr

namespace sqlite {
namespace {
constexpr int         repeatCount{500};
constexpr std::size_t threadCount{10};

sqlite::DatabaseConnection* getConnection()
{
#if USE_MUTEX
    static sqlite::DatabaseConnection connection{
        /* filename */ SQLITE_DATABASE_FILE_NAME,
        /* flags */ SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
            | SQLITE_OPEN_FULLMUTEX,
        /* vfsModuleName */ SQLITE_VFS};
    return &connection;
#else
    sqlite::DatabaseConnection* const connection{new sqlite::DatabaseConnection{
        /* filename */ SQLITE_DATABASE_FILE_NAME,
        /* flags */ SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
            | SQLITE_OPEN_NOMUTEX,
        /* vfsModuleName */ SQLITE_VFS}};
    return connection;
#endif
}

class ConnectionCloser {
public:
    explicit ConnectionCloser(sqlite::DatabaseConnection* conn) : m_conn{conn}
    {
    }

    ~ConnectionCloser()
    {
#if USE_MUTEX
        // no op
#else
        delete m_conn;
#endif
    }

private:
    sqlite::DatabaseConnection* const m_conn;
};

void readDataThreadFunction()
{
    try {
        sqlite::DatabaseConnection* const db{getConnection()};
        ConnectionCloser                  closer{db};
        sqlite::DatabaseConnection&       databaseConnection{*db};

        for (int i{0}; i < repeatCount; ++i) {
            sqlite::PreparedStatement statement{
                databaseConnection.prepareStatement(
                    "SELECT customer_id, first_name, last_name, email, phone, "
                    "address FROM "
                    "customer;")};
            const std::vector<std::vector<sqlite::PreparedStatement::Variant>>
                results{statement.run()};
            (void)results;
        }
    }
    catch (const sqlite::Exception& ex) {
        std::cerr << "std::thread: caught " << ex << '\n';
    }
    catch (const std::runtime_error& ex) {
        std::cerr << "std::thrtead: caught runtime_error: " << ex.what()
                  << '\n';
    }
}

bool stringEndsWith(const std::string& string, const std::string& other)
{
    return string.size() >= other.size()
           && strncmp(
                  string.c_str() + string.size() - other.size(),
                  other.c_str(),
                  other.size())
                  == 0;
}

bool isRootPath(const std::filesystem::path& path)
{
    std::filesystem::directory_iterator it{path};
    std::filesystem::directory_iterator end{};

    for (; it != end; ++it) {
        const std::filesystem::directory_entry& entry{*it};

        if (entry.is_regular_file()
            && stringEndsWith(entry.path().string(), "emails.txt")) {
            return true;
        }
    }

    return false;
}
} // anonymous namespace
} // namespace sqlite

int main()
{
    try {
        while (!sqlite::isRootPath(std::filesystem::current_path())) {
            std::filesystem::current_path(
                std::filesystem::current_path().parent_path());
        }

        std::vector<std::string> emails{sqlite::loadEmails()};

        if (std::filesystem::exists(SQLITE_DATABASE_FILE_NAME)) {
            std::remove(SQLITE_DATABASE_FILE_NAME);
        }

        sqlite::DatabaseConnection* const db{sqlite::getConnection()};
        sqlite::ConnectionCloser          closer{db};
        sqlite::PreparedStatement createTableStatement{db->prepareStatement(R"(
    CREATE TABLE customer (
      customer_id INTEGER PRIMARY KEY,
      first_name TEXT NOT NULL,
      last_name TEXT NOT NULL,
      email TEXT NOT NULL,
      phone TEXT,
      address TEXT
    );)")};
        createTableStatement.run();

        for (int i{0}; i < sqlite::repeatCount; ++i) {
            assert(!emails.empty() && "No more e-mails left.");
            sqlite::PreparedStatement insertStatement{db->prepareStatement(
                "INSERT INTO customer (first_name, last_name, email, phone, "
                "address) "
                "VALUES (?, ?, ?, ?, ?);")};
            insertStatement.bind(1, "John");
            insertStatement.bind(2, "Doe");
            insertStatement.bind(3, emails.back().c_str());
            insertStatement.bind(4, "+12345678");
            insertStatement.bind(5, "123 Main St");
            insertStatement.run();
            emails.pop_back();
        }

        pl::timer timer{};
        auto      timePrinter{gsl::finally([&timer] {
            const std::chrono::steady_clock::duration elapsedTime{
                timer.elapsed_time()};
            std::ostringstream oss{};
            oss << "The reading threads took a total of "
                << std::chrono::duration_cast<std::chrono::milliseconds>(
                       elapsedTime)
                       .count()
                << " milliseconds. ";
#if USE_MUTEX
            oss << "Threading mode: SQLITE_OPEN_FULLMUTEX";
#else
            oss << "Threading mode: SQLITE_OPEN_NOMUTEX";
#endif
            std::printf("%s\n", oss.str().c_str());
        })};

        std::vector<std::thread> threads{};
        auto                     threadJoiner{gsl::finally([&threads] {
            for (std::thread& thd : threads) {
                thd.join();
            }
        })};

        for (std::size_t i{0}; i < sqlite::threadCount; ++i) {
            threads.emplace_back(&sqlite::readDataThreadFunction);
        }
    }
    catch (const sqlite::Exception& ex) {
        std::cerr << "Main thread: caught " << ex << '\n';
    }
    catch (const std::runtime_error& ex) {
        std::cerr << "Main thread: caught runtime_error: " << ex.what() << '\n';
    }
}
