#
# CodeLite plugin function.
# Usage:
#
# include(plugin)
# CL_PLUGIN( "MyPlugin" )
# 
# 

cmake_minimum_required(VERSION 2.8)

function( CL_PLUGIN PLUGIN_NAME )
    message("-- CL_SRC_ROOT is set to ${CL_SRC_ROOT}")
    message("-- USE_PCH is set to ${CL_SRC_ROOT}")
    message("-- PLUGINS_DIR is set to ${CL_SRC_ROOT}")
    ##
    ## Include wxWidgets
    ##
    if ( NOT UNIX )
        ## Windows
        set( MINGW 1 )
        set( CMAKE_FIND_LIBRARY_PREFIXES lib)
        set( CMAKE_FIND_LIBRARY_SUFFIXES .a)
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

    # Add RPATH
    set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")
    
    ## By default, use the sources under the current folder
    FILE(GLOB SRCS "*.cpp" "*.c")
    
endfunction()
