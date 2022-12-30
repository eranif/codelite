if(NOT APPLE)
    find_library(LIBHUNSPELL
                # Fedora doesn't create a hunspell->hunspell-1.3 symlink, so explicitly look for (the current) full name
                NAMES hunspell hunspell-1.3  hunspell-1.4 hunspell-1.5 hunspell-1.6 hunspell-1.7
                HINTS /usr/lib /usr/lib/x86_64-linux-gnu/ /usr/lib64/ ${BREW_PREFIX}/lib
                PATH_SUFFIXES lib)

    find_path(  HUNSPELL_H
                NAMES hunspell.h
                HINTS
                ENV HUNSPELL_DIR
                PATHS
                /usr/include /usr/local/include /usr/include/hunspell /usr/local/include/hunspell ${BREW_PREFIX}/include
                PATH_SUFFIXES include/hunspell include)
else()
    CL_OSX_FIND_BREW_LIB(libhunspell-1.7.0.dylib LIBHUNSPELL)
    CL_OSX_FIND_BREW_HEADER(hunspell/hunspell.h HUNSPELL_H)
endif()

if(LIBHUNSPELL STREQUAL "LIBHUNSPELL-NOTFOUND" OR HUNSPELL_H STREQUAL "HUNSPELL_H-NOTFOUND")
    message("    **")
    message("    ** NOTICE ** Could not locate libhunspell. SpellChecker plugin will not be build")
    message("    ** NOTICE ** Try installing libhunspell-dev package")
    message("    **")
else()
    if (UNIX AND NOT APPLE)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )
    endif()

    if (APPLE)
        add_definitions(-fPIC)
        include_directories(${BREW_PREFIX}/include)
    endif()


    # set the plugin name here
    set( PLUGIN_NAME "SpellCheck")
    message("-- Found libhunspell: ${LIBHUNSPELL}, ${HUNSPELL_H}")

    # Our project is called 'plugin' this is how it will be called in
    # visual studio, and in our makefiles.
    project( ${PLUGIN_NAME} )

    # It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.


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

    if ( USE_PCH AND NOT MINGW )
        add_definitions(-include "${CL_PCH_FILE}")
        add_definitions(-Winvalid-pch)
    endif()

    FILE(GLOB SRCS "*.cpp")

    # Define the output
    add_library(${PLUGIN_NAME} SHARED ${SRCS})

    target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)

    # Codelite plugins doesn't use the "lib" prefix.
    set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
    target_link_libraries(${PLUGIN_NAME}
                          ${LINKER_OPTIONS}
                          ${wxWidgets_LIBRARIES}
                          ${LIBHUNSPELL}
                          libcodelite
                          plugin
                          )

    # Installation destination
    CL_INSTALL_PLUGIN(${PLUGIN_NAME})
    CL_OSX_COPY_BREW_LIB(${LIBHUNSPELL} ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/)

    install(
      DIRECTORY ${CL_SRC_ROOT}/SpellChecker/dics
      DESTINATION ${CL_RESOURCES_DIR}
      USE_SOURCE_PERMISSIONS
      PATTERN ".svn" EXCLUDE
      PATTERN ".git" EXCLUDE
    )
endif()
