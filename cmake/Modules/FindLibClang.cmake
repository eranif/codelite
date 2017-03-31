if (UNIX)
    macro( FIND_LIBCLANG_OFFICIAL )
        find_library(LIBCLANG_T
                     NAMES libclang.so
                     HINTS  
                     /usr/lib 
                     /usr/local/lib 
                     /usr/lib/llvm-4.2/lib
                     /usr/lib/llvm-4.1/lib
                     /usr/lib/llvm-4.0/lib
                     /usr/lib/llvm-3.8/lib
                     /usr/lib/llvm-3.9/lib
                     /usr/lib/llvm-3.7/lib
                     /usr/lib/llvm-3.6/lib
                     /usr/lib/llvm-3.5/lib
                     /usr/lib/llvm-3.4/lib
                     /usr/lib/llvm-3.3/lib
                     # For fedora
                     /usr/lib/llvm
                     /usr/lib64/llvm
                     ${CMAKE_INSTALL_LIBDIR})

        find_path(LIBCLANG_INCLUDE_T NAMES clang-c/Index.h
                  HINTS 
                  /usr/lib/llvm-4.2/include 
                  /usr/lib/llvm-4.1/include 
                  /usr/lib/llvm-4.0/include 
                  /usr/lib/llvm-3.9/include 
                  /usr/lib/llvm-3.8/include 
                  /usr/lib/llvm-3.7/include 
                  /usr/lib/llvm-3.6/include 
                  /usr/lib/llvm-3.5/include 
                  /usr/lib/llvm-3.4/include 
                  /usr/lib/llvm-3.3/include 
                  /usr/include/llvm-4.2 
                  /usr/include/llvm-4.1 
                  /usr/include/llvm-4.0 
                  /usr/include/llvm-3.9 
                  /usr/include/llvm-3.8 
                  /usr/include/llvm-3.7 
                  /usr/include/llvm-3.6 
                  /usr/include/llvm-3.5  
                  /usr/include/llvm-3.4  
                  /usr/include/llvm-3.3  
                  /usr/include/llvm
                  /usr/local/include
                  # For fedora
                  /usr/include
                  )

        if ( LIBCLANG_T STREQUAL "LIBCLANG_T-NOTFOUND" OR LIBCLANG_INCLUDE_T STREQUAL "LIBCLANG_INCLUDE_T-NOTFOUND" )
            set(LIBCLANG "LIBCLANG-NOTFOUND")
            set(LIBCLANG_INCLUDE "LIBCLANG_INCLUDE-NOTFOUND")
            set( LIBCLANG_OFFICIAL_FOUND 0 )
        else()
            set( LIBCLANG_OFFICIAL_FOUND 1 )
            set(LIBCLANG ${LIBCLANG_T})
            set(LIBCLANG_INCLUDE ${LIBCLANG_INCLUDE_T})
            set(LIBCLANG_INSTALL_NEEDED 0)
            ## Check to see if we have a recent version of clang to include clang_getBriefComment
            execute_process(COMMAND /bin/grep clang_getCompletionBriefComment ${LIBCLANG_INCLUDE_T}/clang-c/Index.h 
                            OUTPUT_VARIABLE BRIEF_COMMENTS_OUTPUT 
                            OUTPUT_STRIP_TRAILING_WHITESPACE)
           message("-- Find libClang result: ${BRIEF_COMMENTS_OUTPUT}")
           if (BRIEF_COMMENTS_OUTPUT STREQUAL "")
                message("-- Could not find method clang_getCompletionBriefComment, disabling brief comments support")
                add_definitions(-DHAS_LIBCLANG_BRIEFCOMMENTS=0)
           else()
              add_definitions(-DHAS_LIBCLANG_BRIEFCOMMENTS=1)
              message("-- Found method clang_getCompletionBriefComment, enabling brief comments support")
       endif()
        endif()
    endmacro()
endif()
