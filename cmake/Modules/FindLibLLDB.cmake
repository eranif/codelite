if (UNIX)
    macro( FIND_LLDB_OFFICIAL )
        ## Locate the official packages
        find_library(LIBLLDB_T
                     NAMES liblldb.so liblldb.so.1
                     HINTS  
                     /usr/lib 
                     /usr/local/lib 
                     /usr/lib/llvm-20/lib
                     /usr/lib/llvm-19/lib
                     /usr/lib/llvm-18/lib
                     /usr/lib/llvm-17/lib
                     /usr/lib/llvm-16/lib
                     /usr/lib/llvm-15/lib
                     /usr/lib/llvm-14/lib
                     /usr/lib/llvm-13/lib
                     /usr/lib/llvm-12/lib
                     /usr/lib/llvm-11/lib
                     /usr/lib/llvm-10/lib
                     /usr/lib/llvm-9/lib
                     /usr/lib/llvm-8/lib
                     /usr/lib/llvm-7/lib
                     /usr/lib/llvm-6/lib
                     /usr/lib/llvm-20.0/lib
                     /usr/lib/llvm-19.0/lib
                     /usr/lib/llvm-18.0/lib
                     /usr/lib/llvm-17.0/lib
                     /usr/lib/llvm-16.0/lib
                     /usr/lib/llvm-15.0/lib
                     /usr/lib/llvm-14.0/lib
                     /usr/lib/llvm-13.0/lib
                     /usr/lib/llvm-12.0/lib
                     /usr/lib/llvm-11.0/lib
                     /usr/lib/llvm-10.0/lib
                     /usr/lib/llvm-9.0/lib
                     /usr/lib/llvm-8.0/lib
                     /usr/lib/llvm-7.0/lib
                     /usr/lib/llvm-6.0/lib
                     /usr/lib/llvm-5.0/lib
                     /usr/lib/llvm-4.2/lib
                     /usr/lib/llvm-4.1/lib
                     /usr/lib/llvm-4.0/lib
                     /usr/lib/llvm-3.9/lib
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
                  /usr/lib/llvm-20/include
                  /usr/lib/llvm-19/include
                  /usr/lib/llvm-18/include
                  /usr/lib/llvm-17/include
                  /usr/lib/llvm-16/include
                  /usr/lib/llvm-15/include
                  /usr/lib/llvm-14/include
                  /usr/lib/llvm-13/include
                  /usr/lib/llvm-12/include
                  /usr/lib/llvm-11/include
                  /usr/lib/llvm-10/include
                  /usr/lib/llvm-9/include
                  /usr/lib/llvm-8/include
                  /usr/lib/llvm-20.0/include
                  /usr/lib/llvm-19.0/include
                  /usr/lib/llvm-18.0/include
                  /usr/lib/llvm-17.0/include
                  /usr/lib/llvm-16.0/include
                  /usr/lib/llvm-15.0/include
                  /usr/lib/llvm-14.0/include
                  /usr/lib/llvm-13.0/include
                  /usr/lib/llvm-12.0/include
                  /usr/lib/llvm-11.0/include
                  /usr/lib/llvm-10.0/include
                  /usr/lib/llvm-9.0/include
                  /usr/lib/llvm-8.0/include
                  /usr/lib/llvm-7.0/include
                  /usr/lib/llvm-6.0/include
                  /usr/lib/llvm-5.0/include
                  /usr/lib/llvm-4.2/include 
                  /usr/lib/llvm-4.1/include 
                  /usr/lib/llvm-4.0/include 
                  /usr/lib/llvm-3.9/include 
                  /usr/lib/llvm-3.8/include 
                  #/usr/lib/llvm-3.7/include 
                  /usr/lib/llvm-3.6/include 
                  /usr/lib/llvm-3.5/include 
                  /usr/lib/llvm-3.4/include 
                  /usr/lib/llvm-3.3/include 
                  /usr/include/llvm-4.2 
                  /usr/include/llvm-4.1 
                  /usr/include/llvm-4.0 
                  /usr/include/llvm-3.9 
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
