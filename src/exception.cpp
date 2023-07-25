#include <ostream>
#include <utility>

#include "as_string.hpp"
#include "exception.hpp"

namespace sqlite {
std::ostream& operator<<(std::ostream& os, const Exception& exception)
{
    os << exception.type();
    os << '{';
    os << "\"line\": " << exception.line() << ", ";
    os << "\"function\": \"" << exception.function() << "\", ";
    os << "\"file\": \"" << exception.file() << "\", ";
    os << "\"resultCode\": " << asString(exception.resultCode()) << ", ";
    os << "\"message\": \"" << exception.message() << '"';
    os << '}';
    return os;
}

Exception::Exception(
    std::string exception,
    std::size_t line,
    std::string function,
    std::string file,
    int         resultCode,
    std::string message)
    : std::runtime_error{message}
    , m_exception{std::move(exception)}
    , m_line{line}
    , m_function{std::move(function)}
    , m_file{std::move(file)}
    , m_resultCode{resultCode}
    , m_message{std::move(message)}
{
}

const std::string& Exception::type() const
{
    return m_exception;
}

std::size_t Exception::line() const
{
    return m_line;
}

const std::string& Exception::function() const
{
    return m_function;
}

const std::string& Exception::file() const
{
    return m_file;
}

int Exception::resultCode() const
{
    return m_resultCode;
}

const std::string& Exception::message() const
{
    return m_message;
}
} // namespace sqlite
