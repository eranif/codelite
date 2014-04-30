macro( FIND_LLDB_OFFICIAL )

    ## Locate the official packages
    find_library(LIBLLDB_T
                 NAMES liblldb.so
                 HINTS  /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib)

    find_path(LIBLLDB_INCLUDE_T NAMES lldb/API/SBDebugger.h
              HINTS 
              /usr/lib/llvm-3.5/include 
              /usr/include/llvm-3.5 
              /usr/include/llvm
              /usr/local/include
              )

    if ( LIBLLDB_T STREQUAL "LIBLLDB_T-NOTFOUND" OR LIBLLDB_INCLUDE_T STREQUAL "LIBLLDB_INCLUDE_T-NOTFOUND" )
        set(LIBLLDB "LIBLLDB-NOTFOUND")
        set(LIBLLDB_INCLUDE "LIBLLDB_INCLUDE-NOTFOUND")
    else()
        set(LIBLLDB ${LIBLLDB_T})
        set(LIBLLDB_INCLUDE ${LIBLLDB_INCLUDE_T})
        set(LIBLLDB_INSTALL_NEEDED 0)
    endif()
endmacro()

macro ( FIND_LLDB_CUSTOM_BUILD)
    if ( NOT DEFINED ENV{LLVM_HOME} )
        set( LIBLLDB "LIBLLDB-NOTFOUND" )
        set( LIBLLDB_INCLUDE "LIBLLDB_INCLUDE-NOTFOUND" )
    else()
        set( LLVM_HOME $ENV{LLVM_HOME} )
        message("-- LLVM_HOME is set to ${LLVM_HOME}")
        if ( UNIX AND NOT APPLE )
            set ( LLDB_LIB liblldb.so )
        else()
            set ( LLDB_LIB liblldb.dylib )
        endif()
        set( LLDB_BUILD_DIRS "" )
        # define the search order of the folder
        if(DEBUG_BUILD MATCHES 1)
            message("-- Will search for LLBM Debug build first")
            LIST(APPEND LLDB_BUILD_DIRS "build-debug")
            LIST(APPEND LLDB_BUILD_DIRS "Debug")
            LIST(APPEND LLDB_BUILD_DIRS "build")
            LIST(APPEND LLDB_BUILD_DIRS "build-release")
            LIST(APPEND LLDB_BUILD_DIRS "Release")
        else ()
            message("-- Will search for LLBM Release build first")
            LIST(APPEND LLDB_BUILD_DIRS "build-release")
            LIST(APPEND LLDB_BUILD_DIRS "Release")
            LIST(APPEND LLDB_BUILD_DIRS "build")
            LIST(APPEND LLDB_BUILD_DIRS "build-debug")
            LIST(APPEND LLDB_BUILD_DIRS "Debug")
        endif()

        ## determine the build folder
        set( LLDB_BUILD_DIR "" )
        set ( BUILD_DIR_FOUND 0 )
        foreach(build_tmp_dir ${LLDB_BUILD_DIRS})
            if ( BUILD_DIR_FOUND MATCHES 0 ) 
                if ( EXISTS ${LLVM_HOME}/${build_tmp_dir} AND EXISTS ${LLVM_HOME}/${build_tmp_dir}/lib/${LLDB_LIB} )
                    set( LIBLLDB ${LLVM_HOME}/${build_tmp_dir}/lib/${LLDB_LIB} )
                    set( LIBLLDB_INCLUDE ${LLVM_HOME}/tools/lldb/include )
                    set( BUILD_DIR_FOUND 1 )
                    set(LIBLLDB_INSTALL_NEEDED 1)
                endif()
            endif()
        endforeach()
    endif()
endmacro ()

set( BUILD_LLDB 0 )
set( LIBLLDB "" )
set( LIBLLDB_INCLUDE "" )
set( LIBLLDB_INSTALL_NEEDED 0)
set( LLDB_OFFICIAL_FOUND 0)

if ( APPLE )
    set ( LIBLLDB ${CL_SRC_ROOT}/sdk/lldb/osx/lib/liblldb.3.5.0.dylib )
    set ( LIBLLDB_INCLUDE ${CL_SRC_ROOT}/sdk/lldb/osx/include )
    set ( BUILD_LLDB 1 )
    set ( LIBLLDB_INSTALL_NEEDED 1 )
    ## update search path to include C++11 headers
    include_directories(/usr/lib/c++/v1/)
else()
## could not locate an official package, try custom build
FIND_LLDB_CUSTOM_BUILD()

if ( LIBLLDB STREQUAL "LIBLLDB-NOTFOUND")
    ## locate the official package
    FIND_LLDB_OFFICIAL()

    if ( LIBLLDB STREQUAL "LIBLLDB-NOTFOUND" )
        message(" **** NOTICE: could not locate lldb devel package. LLDBDebugger is skipped")
    else()
        set( BUILD_LLDB 1 )
        set( LLDB_OFFICIAL_FOUND 1 ) ## this means that we need to add lldb-3.5-dev package to the list of dependencies
    endif()
else()
    set( BUILD_LLDB 1 )
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
        find_package(wxWidgets COMPONENTS std REQUIRED)
    else ( APPLE )
        find_package(wxWidgets COMPONENTS std aui propgrid stc richtext ribbon REQUIRED)
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

    # Add RPATH
    set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")

    ## By default, use the sources under the current folder
    FILE(GLOB PLUGIN_SRCS "*.cpp")

    # Define the output - shared library
    add_library(${PLUGIN_NAME} SHARED ${PLUGIN_SRCS})

    target_link_libraries(${PLUGIN_NAME} ${LIBLLDB})
    target_link_libraries(LLDBDebugger LLDBProtocol)

    # Codelite plugins doesn't use the "lib" prefix.
    set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
    target_link_libraries(${PLUGIN_NAME}
        ${LINKER_OPTIONS}
        ${PLUGIN_EXTRA_LIBS}
        ${wxWidgets_LIBRARIES}
        -L"${CL_LIBPATH}"
        -lLLDBProtocol
        -llibcodelite
        -lplugin
        -lwxsqlite3 
        -lsqlite3lib
    )

    # The plugin library is required
    add_dependencies(${PLUGIN_NAME} plugin)
    
    # Installation destination
    install(TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGINS_DIR})

    if ( LIBLLDB_INSTALL_NEEDED MATCHES 1 )
        set(LLDB_LIB_ABS ${LIBLLDB})
        if ( IS_SYMLINK ${LIBLLDB} )
            message( "-- ${LIBLLDB} is a symbolic link ")
            get_filename_component(LLDB_LIB_TMP ${LIBLLDB} REALPATH)
            set( LLDB_LIB_ABS ${LLDB_LIB_TMP})
        endif()

        message("-- Will install file ${LLDB_LIB_ABS}")
        install(FILES ${LLDB_LIB_ABS} DESTINATION ${PLUGINS_DIR} PERMISSIONS ${EXE_PERM})
    endif()

    add_subdirectory(LLDBProtocol)
    add_subdirectory(codelite-lldb)

    add_dependencies(codelite-lldb LLDBProtocol)
    add_dependencies(LLDBDebugger LLDBProtocol)
else()
    message(" **** NOTICE: Try installing lldb-3.5-dev or build lldb yourself and set up the environment variable LLVM_HOME")
endif()

