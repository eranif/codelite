if (UNIX)
    macro( FIND_LLDB_OFFICIAL )
        ## Locate the official packages
        find_library(LIBLLDB_T
                     NAMES liblldb.so liblldb.so.1
                     HINTS  
                     /usr/lib 
                     /usr/local/lib 
                     /usr/lib/llvm-3.8/lib
                     #/usr/lib/llvm-3.7/lib
                     /usr/lib/llvm-3.6/lib
                     /usr/lib/llvm-3.5/lib
                     /usr/lib/llvm-3.4/lib
                     /usr/lib/llvm-3.3/lib
                     # For fedora
                     /usr/lib/llvm
                     /usr/lib64/llvm
                     ${CMAKE_INSTALL_LIBDIR})

        find_path(LIBLLDB_INCLUDE_T NAMES lldb/API/SBDebugger.h
                  HINTS 
                  /usr/lib/llvm-3.8/include 
                  #/usr/lib/llvm-3.7/include 
                  /usr/lib/llvm-3.6/include 
                  /usr/lib/llvm-3.5/include 
                  /usr/lib/llvm-3.4/include 
                  /usr/lib/llvm-3.3/include 
                  /usr/include/llvm-3.8 
                  #/usr/include/llvm-3.7 
                  /usr/include/llvm-3.6 
                  /usr/include/llvm-3.5 
                  /usr/include/llvm-3.4 
                  /usr/include/llvm-3.3 
                  /usr/include/llvm
                  /usr/local/include
                  # For fedora
                  /usr/include
                  )

        if ( LIBLLDB_T STREQUAL "LIBLLDB_T-NOTFOUND" OR LIBLLDB_INCLUDE_T STREQUAL "LIBLLDB_INCLUDE_T-NOTFOUND" )
            set(LIBLLDB "LIBLLDB-NOTFOUND")
            set(LIBLLDB_INCLUDE "LIBLLDB_INCLUDE-NOTFOUND")
            set( LLDB_OFFICIAL_FOUND 0 )
        else()
            set(LIBLLDB ${LIBLLDB_T})
            set(LIBLLDB_INCLUDE ${LIBLLDB_INCLUDE_T})
            set( LLDB_OFFICIAL_FOUND 1 )
        endif()
    endmacro()
endif()
