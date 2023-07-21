cmake_minimum_required(VERSION 3.15)

project(centipede_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(WIN32)
  add_definitions(-DUNICODE -D_UNICODE)
endif()

add_subdirectory(sqlite)

set(APP_NAME sqlite_test_app)

add_executable(
  ${APP_NAME}
  src/main.cpp
)

target_include_directories(
  ${APP_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
  ${APP_NAME}
  PRIVATE
  sqlite_lib
)

