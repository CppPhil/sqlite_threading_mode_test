add_library(
  sqlite_lib 
  STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/shell.c
  ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.c
  ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.h
  ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3ext.h
)

target_include_directories(
  sqlite_lib
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
