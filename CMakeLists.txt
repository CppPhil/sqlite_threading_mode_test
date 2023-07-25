cmake_minimum_required(VERSION 3.15)

project(sqlite_testing_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(WIN32)
  add_definitions(-DUNICODE -D_UNICODE)
endif()

option(ENABLE_MUTEX "Enable FULL_MUTEX threading mode" OFF)

if (ENABLE_MUTEX)
  add_compile_definitions(USE_MUTEX=1)
else()
  add_compile_definitions(USE_MUTEX=0)
endif()

add_subdirectory(external/fmtlib)
add_subdirectory(external/GSL)
add_subdirectory(external/philslib)
add_subdirectory(sqlite)

set(APP_NAME sqlite_test_app)

add_executable(
  ${APP_NAME}
  include/as_string.hpp
  include/clean_function.hpp
  include/database_connection.hpp
  include/exception.hpp
  include/load_emails.hpp
  include/prepared_statement.hpp
  include/throw.hpp
  src/as_string.cpp
  src/clean_function.cpp
  src/database_connection.cpp
  src/exception.cpp
  src/load_emails.cpp
  src/main.cpp
  src/prepared_statement.cpp
)

target_include_directories(
  ${APP_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
  ${APP_NAME}
  PRIVATE
  fmt::fmt
  Microsoft.GSL::GSL
  CppPhil::philslib
  sqlite_lib
)

