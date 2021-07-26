#  Try to find Sqlite3
#  Once done this will define values if library is found
#    SQLITE3_FOUND - system has Sqlite3
#    SQLITE3_INCLUDE_DIR - the Sqlite3 include directory
#    SQLITE3_LIBRARY - Link these to use Sqlite3
#

if(NOT DEFINED SQLITE3_FOUND)
    find_library(SQLITE3_LIBRARY
                 NAMES libsqlite3.so
                 HINTS
                 /usr/local/lib
                 /usr/lib
                 /usr/lib/x86_64-linux-gnu
                 /usr/lib/i386-linux-gnu
                 )
                 
    find_path(SQLITE3_INCLUDE_DIR NAMES sqlite3.h
              HINTS 
              /usr/local/include
              /usr/include
              /usr/include/sqlite3
              )
    if (SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)
       set(SQLITE3_FOUND TRUE)
    endif()

    if(SQLITE3_FOUND)
      message("-- SQLITE3_INCLUDE_DIR: " ${SQLITE3_INCLUDE_DIR})
      message("-- SQLITE3_LIBRARY: " ${SQLITE3_LIBRARY})
    else()
      message(-- "Could not find system Sqlite3. Using bundled sqlite3")
    endif()
endif()
