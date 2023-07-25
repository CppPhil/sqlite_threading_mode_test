#pragma once
#include <stdexcept>

#include <sqlite3.h>

#include "exception.hpp"
#include "prepared_statement.hpp"

namespace sqlite {
class DatabaseConnection {
public:
    DatabaseConnection(
        const char* filename,
        int         flags,
        const char* vfsModuleName);

    DatabaseConnection(const DatabaseConnection&) = delete;

    DatabaseConnection(DatabaseConnection&& other) noexcept;

    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    DatabaseConnection& operator=(DatabaseConnection&& other) noexcept;

    ~DatabaseConnection();

    PreparedStatement prepareStatement(const char* sqlStatement);

private:
    sqlite3* m_connection;
};
} // namespace sqlite
