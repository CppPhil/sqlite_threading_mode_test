#include <cstdio>

#include "sqlite3.h"

int main()
{ 
  const char* const libVersion{
    sqlite3_libversion()    
  };
  const char* const sourceId{
    sqlite3_sourceid()
  };
  const int versionNumber{
    sqlite3_libversion_number()
  };

  std::printf(
    "libVersion   : \"%s\"\n"
    "sourceId     : \"%s\"\n"
    "versionNumber: %d\n",
    libVersion, sourceId, versionNumber
  );
}
