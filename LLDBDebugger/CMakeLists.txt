set( BUILD_LLDB 0 )
set( LIBLLDB "" )
set( LIBLLDB_INCLUDE "" )
set( LIBLLDB_INSTALL_NEEDED 0)
set( LLDB_OFFICIAL_FOUND 0)

include(FindLibLLDB)

if (UNIX AND NOT APPLE)
    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )
endif()

if ( APPLE )
    add_definitions(-fPIC)
endif()


if (WITH_LLDB MATCHES 1)
    if ( APPLE )
        set ( LIBLLDB ${CL_SRC_ROOT}/sdk/lldb/osx/lib/liblldb.dylib )
        set ( LIBLLDB_INCLUDE ${CL_SRC_ROOT}/sdk/lldb/osx/include )
        set ( BUILD_LLDB 1 )
        set ( LIBLLDB_INSTALL_NEEDED 1 )
        ## update search path to include C++11 headers
        include_directories(/usr/lib/c++/v1/)
    else()
        FIND_LLDB_OFFICIAL()
        if ( LIBLLDB STREQUAL "LIBLLDB-NOTFOUND" )
        ## could not locate an official package, try a custom build
            find_library(LIBEDIT_LIB NAMES libedit.so HINTS /usr/local/lib /usr/lib ${CMAKE_INSTALL_LIBDIR}) # Our liblldb.so depends on libedit.so
            if(LLDB_OFFICIAL_FOUND MATCHES 0)
                set ( BUILD_LLDB 0 )
            else()
                message("-- libedit.so is not installed. For lldb support, please install the libedit development package and try again.")
            endif()
        else()
            set ( BUILD_LLDB 1 )
        endif()
    endif()

    if ( BUILD_LLDB MATCHES 1 )
        message("-- LIBLLDB is set to ${LIBLLDB}")
        message("-- LIBLLDB_INCLUDE is set to ${LIBLLDB_INCLUDE}")
        include_directories(${LIBLLDB_INCLUDE})
        link_directories(${LLDB_LIB_PATH})

        ## lldb requires C++11
        add_definitions(-std=c++11)
        set(PLUGIN_NAME "LLDBDebugger")
        project(LLDBDebugger)

        if ( APPLE )
            ## Under Apple, we only support monolithic build of wxWidgets
            
        else ( APPLE )
            
        endif ( APPLE )

        # wxWidgets include (this will do all the magic to configure everything)
        include( "${wxWidgets_USE_FILE}" )

        # Include paths
        include_directories("${CL_SRC_ROOT}/Plugin"
                            "${CL_SRC_ROOT}/sdk/wxsqlite3/include" 
                            "${CL_SRC_ROOT}/CodeLite" 
                            "${CL_SRC_ROOT}/PCH" 
                            "${CL_SRC_ROOT}/Interfaces")
        ## Definitions
        add_definitions(-DWXUSINGDLL_WXSQLITE3)
        add_definitions(-DWXUSINGDLL_CL)
        add_definitions(-DWXUSINGDLL_SDK)

        ## By default, use the sources under the current folder
        FILE(GLOB PLUGIN_SRCS "*.cpp")

        # Define the output - shared library
        add_library(${PLUGIN_NAME} SHARED ${PLUGIN_SRCS})

        target_link_libraries(LLDBDebugger LLDBProtocol)

        # Codelite plugins doesn't use the "lib" prefix.
        set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
        target_link_libraries(${PLUGIN_NAME}
                              ${LINKER_OPTIONS}
                              ${wxWidgets_LIBRARIES}
                              ${LIBLLDB}
                              libcodelite
                              plugin
                              )
        # Installation destination
        CL_INSTALL_PLUGIN(${PLUGIN_NAME})

        add_subdirectory(LLDBProtocol)
        add_subdirectory(codelite-lldb)

    else()
        message(" **** NOTICE: lldb is not available. You could try installing the lldb-3.4-dev (or equivalent) package")
    endif()
endif(WITH_LLDB MATCHES 1)

