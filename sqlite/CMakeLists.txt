add_library(
  sqlite_lib 
  STATIC
  shell.c
  sqlite3.c
  sqlite3.h
  sqlite3ext.h
)

target_include_directories(
  sqlite_lib
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
