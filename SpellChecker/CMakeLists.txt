# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)

find_library(LIBHUNSPELL 
            NAMES hunspell
            PATH_SUFFIXES lib)

find_path(  HUNSPELL_H
            NAMES hunspell.h
            HINTS
              ENV HUNSPELL_DIR
            PATHS
              /usr/include /usr/local/include /usr/include/hunspell /usr/local/include/hunspell
            PATH_SUFFIXES include/hunspell include)

if ( (LIBHUNSPELL STREQUAL "LIBHUNSPELL-NOTFOUND" OR HUNSPELL_H STREQUAL "HUNSPELL_H-NOTFOUND") AND NOT APPLE )
    message("    **")
    message("    ** NOTICE ** Could not locate libhunspell. SpellChecker plugin will not be build")
    message("    ** NOTICE ** Try installing libhunspell-dev package")
    message("    **")
else ()
    if ( APPLE) 
        set(LIBHUNSPELL ${CL_SRC_ROOT}/sdk/hunspell/lib/osx/libhunspell-1.3.dylib)
        set(HUNSPELL_H ${CL_SRC_ROOT}/sdk/hunspell/include/)
    endif()
    
    # set the plugin name here
    set( PLUGIN_NAME "SpellCheck")
    message("-- Found libhunspell: ${LIBHUNSPELL}, ${HUNSPELL_H}")
    
    # Our project is called 'plugin' this is how it will be called in
    # visual studio, and in our makefiles. 
    project( ${PLUGIN_NAME} )

    # It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
    find_package(wxWidgets COMPONENTS ${WX_COMPONENTS} REQUIRED)

    # wxWidgets include (this will do all the magic to configure everything)
    include( "${wxWidgets_USE_FILE}" )

    # Include paths
    include_directories("${CL_SRC_ROOT}/SpellChecker" "${CL_SRC_ROOT}/Plugin" 
                        "${CL_SRC_ROOT}/sdk/wxsqlite3/include" 
                        "${CL_SRC_ROOT}/CodeLite" 
                        "${CL_SRC_ROOT}/PCH" 
                        "${CL_SRC_ROOT}/Interfaces"
                        "${HUNSPELL_H}"
                        )

    add_definitions(-DWXUSINGDLL_WXSQLITE3)
    add_definitions(-DWXUSINGDLL_CL)
    add_definitions(-DWXUSINGDLL_SDK)

    if ( USE_PCH )
        add_definitions(-include "${CL_PCH_FILE}")
        add_definitions(-Winvalid-pch)
    endif ( USE_PCH )

    # Add RPATH
    set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")

    FILE(GLOB SRCS "*.cpp")

    # Define the output
    add_library(${PLUGIN_NAME} SHARED ${SRCS})

    # Remove the "lib" prefix from the plugin name
    set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
    target_link_libraries(${PLUGIN_NAME} ${LIBHUNSPELL})
    target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES} -L"${CL_LIBPATH}" -lwxsqlite3 -lsqlite3lib -llibcodelite -lplugin)

    # Make sure that the plugin will not start build before 'plugin.so' is ready
    add_dependencies(${PLUGIN_NAME} plugin)

    install(TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGINS_DIR})
    install(
      DIRECTORY ${CL_SRC_ROOT}/SpellChecker/dics
      DESTINATION ${CL_PREFIX}/share/codelite
      USE_SOURCE_PERMISSIONS
      PATTERN ".svn" EXCLUDE
      PATTERN ".git" EXCLUDE
    )
endif() 
