#include <cstdio>
#include <cstring>

#include <utility>

#include "as_string.hpp"
#include "database_connection.hpp"
#include "throw.hpp"

namespace sqlite {
DatabaseConnection::DatabaseConnection(
    const char* filename,
    int         flags,
    const char* vfsModuleName)
    : m_connection{nullptr}
{
    const int resultCode{sqlite3_open_v2(
        /* filename */ filename,
        /* ppDb */ &m_connection,
        /* flags */ flags,
        /* zVfs */ vfsModuleName)};

    if (resultCode != SQLITE_OK) {
        SQLITE_THROW(
            Exception,
            resultCode,
            "Couldn't open SQLite database connection: \"{}\"",
            sqlite3_errmsg(m_connection));
    }

    // Enable write ahead logging
    sqlite::PreparedStatement stmt{
        prepareStatement("PRAGMA journal_mode=WAL;")};
    stmt.run();
}

DatabaseConnection::DatabaseConnection(DatabaseConnection&& other) noexcept
    : m_connection{other.m_connection}
{
    other.m_connection = nullptr;
}

DatabaseConnection& DatabaseConnection::operator=(
    DatabaseConnection&& other) noexcept
{
    std::swap(m_connection, other.m_connection);
    return *this;
}

DatabaseConnection::~DatabaseConnection()
{
    if (m_connection == nullptr) {
        return;
    }

    const int resultCode{sqlite3_close_v2(m_connection)};

    if (resultCode != SQLITE_OK) {
        std::fprintf(
            stderr,
            "Couldn't close SQLite database connection: \"%s\" (%s)\n",
            sqlite3_errmsg(m_connection),
            asString(resultCode));
    }
}

PreparedStatement DatabaseConnection::prepareStatement(const char* sqlStatement)
{
    const std::size_t stringLength{std::strlen(sqlStatement)};
    const std::size_t byteCount{stringLength + 1};
    sqlite3_stmt*     statement{nullptr};
    const int         resultCode{sqlite3_prepare_v2(
        /* db */ m_connection,
        /* zSql */ sqlStatement,
        /* nByte */ byteCount,
        /* ppStmt */ &statement,
        /* pzTail */ nullptr)};

    if (resultCode != SQLITE_OK) {
        SQLITE_THROW(
            Exception,
            resultCode,
            "Failed to prepare SQL statement! Statement: \"{}\"",
            sqlStatement);
    }

    return PreparedStatement{m_connection, statement, sqlStatement};
}
} // namespace sqlite
