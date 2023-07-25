#include <cstdio>
#include <cstring>

#include <sstream>

#include <gsl/util>

#include <pl/timer.hpp>

#include "as_string.hpp"
#include "exception.hpp"
#include "prepared_statement.hpp"
#include "throw.hpp"

namespace sqlite {
PreparedStatement::PreparedStatement(
    sqlite3*      db,
    sqlite3_stmt* statement,
    const char*   sqlQuery)
    : m_db{db}, m_statement{statement}, m_sqlQuery{sqlQuery}
{
}

PreparedStatement::~PreparedStatement()
{
    const int resultCode{sqlite3_finalize(m_statement)};

    if (resultCode != SQLITE_OK) {
        std::fprintf(
            stderr,
            "Failed to finalize prepared statement. resultCode: %s, error "
            "message: "
            "\"%s\"\n",
            asString(resultCode),
            sqlite3_errmsg(m_db));
    }
}

void PreparedStatement::bind(int placeholderIndex, double value)
{
    const int resultCode{
        sqlite3_bind_double(m_statement, placeholderIndex, value)};

    if (resultCode != SQLITE_OK) {
        SQLITE_THROW(
            Exception,
            resultCode,
            "Failed to bind double: \"{}\"",
            sqlite3_errmsg(m_db));
    }
}

void PreparedStatement::bind(int placeholderIndex, int value)
{
    const int resultCode{
        sqlite3_bind_int(m_statement, placeholderIndex, value)};

    if (resultCode != SQLITE_OK) {
        SQLITE_THROW(
            Exception,
            resultCode,
            "Failed to bind int: \"{}\"",
            sqlite3_errmsg(m_db));
    }
}

void PreparedStatement::bind(int placeholderIndex, sqlite3_int64 value)
{
    const int resultCode{
        sqlite3_bind_int64(m_statement, placeholderIndex, value)};

    if (resultCode != SQLITE_OK) {
        SQLITE_THROW(
            Exception,
            resultCode,
            "Failed to bind int64: \"{}\"",
            sqlite3_errmsg(m_db));
    }
}

void PreparedStatement::bind(int placeholderIndex, const char* text)
{
    const std::size_t length = std::strlen(text);
    const std::size_t bytes  = length + 1;
    const int         resultCode{sqlite3_bind_text(
        m_statement, placeholderIndex, text, bytes, SQLITE_TRANSIENT)};

    if (resultCode != SQLITE_OK) {
        SQLITE_THROW(
            Exception,
            resultCode,
            "Failed to bind text: \"{}\"",
            sqlite3_errmsg(m_db));
    }
}

static std::vector<PreparedStatement::Variant> extractRow(
    sqlite3_stmt* statement)
{
    using Variant = PreparedStatement::Variant;
    const int            columns{sqlite3_column_count(statement)};
    std::vector<Variant> row{};

    for (int i = 0; i < columns; ++i) {
        const int type{sqlite3_column_type(statement, i)};

        switch (type) {
        case SQLITE_INTEGER:
            row.push_back(Variant{sqlite3_column_int64(statement, i)});
            break;
        case SQLITE_FLOAT:
            row.push_back(Variant{sqlite3_column_double(statement, i)});
            break;
        case SQLITE_TEXT: {
            const unsigned char* const memory{
                sqlite3_column_text(statement, i)};
            const std::string string{reinterpret_cast<const char*>(memory)};
            row.push_back(Variant{string});
            break;
        }
        case SQLITE_BLOB:
            throw std::runtime_error{
                "PreparedStatement: BLOB is not handled yet."};
        case SQLITE_NULL:
            throw std::runtime_error{
                "PreparedStatement: NULL is not handled yet."};
        }
    }

    return row;
}

std::vector<std::vector<PreparedStatement::Variant>> PreparedStatement::run()
{
    std::vector<std::vector<Variant>> result{};
    int                               returnCode{};

    for (;;) {
        returnCode = sqlite3_step(m_statement);

        switch (returnCode) {
        case SQLITE_DONE:
            return result;
        case SQLITE_ROW: {
            const std::vector<Variant> row{extractRow(m_statement)};
            result.push_back(row);
            break;
        }
        default:
            SQLITE_THROW(
                Exception,
                returnCode,
                "sqlite3_step failed. Query: {}",
                m_sqlQuery);
        }
    }

    return result;
}

std::vector<std::vector<PreparedStatement::Variant>>
PreparedStatement::runProfiled()
{
    pl::timer timer{};
    auto      finalAction{gsl::finally([this, &timer] {
        const std::chrono::steady_clock::duration elapsedTime{
            timer.elapsed_time()};
        std::ostringstream oss{};
        oss << '"' << m_sqlQuery << "\" took "
            << std::chrono::duration_cast<std::chrono::microseconds>(
                   elapsedTime)
                   .count()
            << " microseconds.";
        std::printf("%s\n", oss.str().c_str());
    })};
    return run();
}

} // namespace sqlite
