if (UNIX)
    macro( FIND_LIBCLANG_OFFICIAL )
        ## Locate the official packages
        find_library(LIBCLANG_T
                     NAMES libclang.so
                     HINTS  
                     /usr/lib 
                     /usr/local/lib 
                     /usr/lib/llvm-3.5/lib
                     /usr/lib/llvm-3.6/lib
                     ${CMAKE_INSTALL_LIBDIR})

        find_path(LIBCLANG_INCLUDE_T NAMES clang-c/Index.h
                  HINTS 
                  /usr/lib/llvm-3.5/include 
                  /usr/include/llvm-3.5  
                  /usr/include/llvm-3.6 
                  /usr/include/llvm
                  /usr/local/include
                  )

        if ( LIBCLANG_T STREQUAL "LIBCLANG_T-NOTFOUND" OR LIBCLANG_INCLUDE_T STREQUAL "LIBCLANG_INCLUDE_T-NOTFOUND" )
            set(LIBCLANG "LIBCLANG-NOTFOUND")
            set(LIBCLANG_INCLUDE "LIBCLANG_INCLUDE-NOTFOUND")
        else()
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
                ## enabling LLDB with this version of clang will definitly cause a segfault
                set(ENABLE_LLDB 0)
                set(WITH_LLDB 0)
                message("-- Disabling LLDB support")
           else()
              add_definitions(-DHAS_LIBCLANG_BRIEFCOMMENTS=1)
              message("-- Found method clang_getCompletionBriefComment, enabling brief comments support")
       endif()
        endif()
    endmacro()
endif()
