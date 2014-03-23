## Try locate the environment variable where LLDB build exists
if ( NOT DEFINED ENV{LLVM_HOME} )
    message("--")
    message("**** NOTICE **** : Could not locate environment variable LLVM_HOME. Please set it to your local LLVM root folder")
    message("**** NOTICE **** : LLDB Debugger is disabled")
    message("--")
else()
    set( LLVM_HOME $ENV{LLVM_HOME} )
    message("-- LLVM_HOME is set to ${LLVM_HOME}")
    if ( UNIX AND NOT APPLE )
        set ( LLDB_LIB liblldb.so )
    else()
        set ( LLDB_LIB liblldb.dylib )
    endif()
    
    if ( NOT EXISTS ${LLVM_HOME}/build-release/lib/${LLDB_LIB} )
        ## We could not locate the binary
        message("**** NOTICE: Could not locate ${LLVM_HOME}/build-release/lib/${LLDB_LIB}")
        
    else()
        set(LLDB_LIB_PATH ${LLVM_HOME}/build-release/lib)
        set(LLDB_INCLUDE_PATH ${LLVM_HOME}/tools/lldb/include)
        message("-- LLDB_LIB_PATH is set to ${LLDB_LIB_PATH}")
        message("-- LLDB_INCLUDE_PATH is set to ${LLDB_INCLUDE_PATH}")
        
        include_directories(${LLDB_INCLUDE_PATH})
        link_directories(${LLDB_LIB_PATH})
        
        ## We are good to go - include this plugin
        CL_PLUGIN(LLDBDebugger "${LLDB_LIB}")
    endif()
endif()
