# CodeLite plugin function.
# Usage:
#
# CL_PLUGIN( "MyPlugin" )

cmake_minimum_required(VERSION 3.0)
message("-- Including plugin.cmake module")

function( CL_PLUGIN PLUGIN_NAME )
    message("-- CL_SRC_ROOT is set to ${CL_SRC_ROOT}")
    message("-- USE_PCH is set to ${USE_PCH}")
    message("-- PLUGINS_DIR is set to ${PLUGINS_DIR}")
    
    set ( PLUGIN_EXTRA_LIBS ${ARGV1} )
    ##
    ## Include wxWidgets
    ##
    if ( NOT UNIX )
        ## Windows
        set( MINGW 1 )
        set( CMAKE_FIND_LIBRARY_PREFIXES lib)
        set( CMAKE_FIND_LIBRARY_SUFFIXES .a .dll.a)
        find_package(wxWidgets COMPONENTS std aui propgrid stc richtext ribbon REQUIRED)
        
    else( NOT UNIX )
        if ( APPLE )
            ## Under Apple, we only support monolithic build of wxWidgets
            find_package(wxWidgets COMPONENTS std REQUIRED)
        else ( APPLE )
            find_package(wxWidgets COMPONENTS std aui propgrid stc richtext ribbon REQUIRED)
        endif ( APPLE )
    endif( NOT UNIX )

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

    ## PreCompiled header support
    if ( USE_PCH )
        add_definitions(-include "${CL_PCH_FILE}")
        add_definitions(-Winvalid-pch)
    endif ( USE_PCH )
    
    ## By default, use the sources under the current folder
    FILE(GLOB_RECURSE PLUGIN_SRCS "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.c")
    
    # Define the output - shared library
    add_library(${PLUGIN_NAME} SHARED ${PLUGIN_SRCS})
    
    # Codelite plugins doesn't use the "lib" prefix.
    set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
    target_link_libraries(${PLUGIN_NAME}
                        ${LINKER_OPTIONS}
                        ${wxWidgets_LIBRARIES}
                        libcodelite
                        plugin
                        wxsqlite3 
                        sqlite3lib
                        )

    # Installation destination
    install(TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGINS_DIR})
endfunction()


# Scan the top level folders of the source tree, if a folder exists and it contains the a file 
# named "codelite-plugin.cmake" in it, automatically include it
function( CL_SCAN_FOR_PLUGINS )
    message("-- CMAKE_SOURCE_DIR is set to ${CMAKE_SOURCE_DIR}")
    file(GLOB files RELATIVE ${CMAKE_SOURCE_DIR} "*")
    foreach(dir ${files})
        if ( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${dir} ) 
            if ( EXISTS ${CMAKE_SOURCE_DIR}/${dir}/codelite-plugin.cmake )
                message("-- Found user plugin at ${CMAKE_SOURCE_DIR}/${dir}")
                include(${CMAKE_SOURCE_DIR}/${dir}/codelite-plugin.cmake)
            endif ()
        endif ()
    endforeach()
endfunction()
