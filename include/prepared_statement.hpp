#pragma once
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include <sqlite3.h>

#include "exception.hpp"

namespace sqlite {
class PreparedStatement {
public:
    using Variant = std::variant<sqlite_int64, double, std::string>;

    PreparedStatement(
        sqlite3*      db,
        sqlite3_stmt* statement,
        const char*   sqlQuery);

    ~PreparedStatement();

    void bind(int placeholderIndex, double value);

    void bind(int placeholderIndex, int value);

    void bind(int placeholderIndex, sqlite3_int64 value);

    void bind(int placeholderIndex, const char* text);

    std::vector<std::vector<Variant>> run();

    std::vector<std::vector<Variant>> runProfiled();

private:
    sqlite3*      m_db;
    sqlite3_stmt* m_statement;
    const char*   m_sqlQuery;
};
} // namespace sqlite
