if(UNIX AND NOT APPLE)
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
    
    if(NOT SQLITE3_FOUND)
        message(FATAL_ERROR " **** Could not find Sqlite3. Please install libsqlite3-dev package **** ")
    else()
        message("-- SQLITE3_INCLUDE_DIR: " ${SQLITE3_INCLUDE_DIR})
        message("-- SQLITE3_LIBRARY: " ${SQLITE3_LIBRARY})
    endif()
endif()
