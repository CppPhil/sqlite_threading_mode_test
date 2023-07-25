#pragma once
#include <cstddef>

#include <pl/current_function.hpp>

#include <fmt/format.h>

#include "clean_function.hpp"

#define SQLITE_THROW(ExceptionType, ResultCode, FormatString, ...)  \
    throw ExceptionType                                             \
    {                                                               \
#ExceptionType, static_cast < std::size_t>(__LINE__),       \
            ::sqlite::cleanFunction(PL_CURRENT_FUNCTION), __FILE__, \
            ResultCode, ::fmt::format(FormatString, __VA_ARGS__)    \
    }
