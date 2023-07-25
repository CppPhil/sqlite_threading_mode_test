#pragma once
#include <cstddef>

#include <iosfwd>
#include <stdexcept>
#include <string>

#include <fmt/ostream.h>

namespace sqlite {
class Exception : public std::runtime_error {
public:
    friend std::ostream& operator<<(
        std::ostream&    os,
        const Exception& exception);

    Exception(
        std::string exception,
        std::size_t line,
        std::string function,
        std::string file,
        int         resultCode,
        std::string message);

    const std::string& type() const;

    std::size_t line() const;

    const std::string& function() const;

    const std::string& file() const;

    int resultCode() const;

    const std::string& message() const;

private:
    std::string m_exception;
    std::size_t m_line;
    std::string m_function;
    std::string m_file;
    int         m_resultCode;
    std::string m_message;
};
} // namespace sqlite
