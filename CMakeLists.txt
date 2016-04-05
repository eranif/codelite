cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

#################################################################################################################################################################
##                                                                                                                                                              #
## codelite IDE cmake file                                                                                                                                      #
## Typical usage will be (build in release mode):                                                                                                               #
##                                                                                                                                                              #
## > mkdir build                                                                                                                                                #
## > cd build                                                                                                                                                   #
## > cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..                                                                                                    #
## > make -jN                                                                                                                                                   #
## > sudo make install                                                                                                                                          #
##                                                                                                                                                              #
## Optional command line arguments:                                                                                                                             #
##                                                                                                                                                              #
##      -DCMAKE_BUILD_TYPE=Release|Debug|DebugFull // Build release, debug + optimisation or debug without optimisation (for others see the Cmake docs)         #
##      -DPREFIX="<some-prefix>"                   // Installation prefix, default is set to /usr                                                               #
##      -DENABLE_CLANG=1|0                         // Build codelite with clang code completion support?, default is 1 (with clang)                             #
##      -DWITH_WXC=1|0                             // Build wxCrafter (sources are not part of codelite distribution) default is 0                              #
##      -DCOPY_WX_LIBS=1|0                         // Incorporate the wxWidgets libs into CodeLite so the binary doesn't depend on them. default is 0           #
##      -DPREVENT_WX_ASSERTS=1|0                   // Prevent those annoying wxASSERTS. In release builds the default is 1, in debug 0                          #
##      -DAUTOGEN_REVISION=1|0                     // Should cmake generate makefiles that auto generates the autoversion.cpp file - default is 1               #
##      -DWITH_PCH=1|0                             // Enable Pre Compiled Header?                                                                               #
##      -DGTK_USE_NATIVEBOOK=1|0                   // Under GTK, use the native notebook instead of wxAuiNoteook. Default is set to 0                           #
##      -DWITH_WXPATH=<fullpath>                   // Specify a particular wxWidgets build to use. The format must be /path/to/different_wx-config/directory/   #
##      -DMAKE_DEB=1|0                             // When set to 1, you can use make package to create .deb file for codelite                                  #
##      -DENABLE_SFTP=1|0                          // When set to 1 codelite is built with SFTP support. Default is build _with_ SFTP support                   #
##      -DENABLE_LLDB=1|0                          // When set to 0 codelite won't try to build or link to the lldb debugger. Default is 1 on Unix platforms    #
#################################################################################################################################################################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#############################################
## Defaults
#############################################
project( "CodeLite" )
if(APPLE)
  execute_process(COMMAND make type=release os=osx clean WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/PCH)
  execute_process(COMMAND make type=release os=osx WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/PCH)
endif()

## Generate compile_commands.json file for code completion
set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )
set( CL_PREFIX "/usr" )

if (CMAKE_CURRENT_LIST_DIR) # since cmake 2.8.3
    set( CL_SRC_ROOT ${CMAKE_CURRENT_LIST_DIR})
else()
    set( CL_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}) # which seems to be the same, at least in this situation
endif()

include(FindLibClang)
include(FindSqlite3)

if(NOT SQLITE3_FOUND)
    set(SQLITE3_INCLUDE_DIR ${CL_SRC_ROOT}/sqlite3)
    set(SQLITE3_LIBRARY "-lsqlite3lib")
endif()

set( USE_CLANG 1 )
set( IS_FREEBSD 0 )
set( BUILD_WXC 0 )
set( CL_COPY_WX_LIBS 0 )
set( WITH_SFTP 1 )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations") # Avoid very multiple warnings spam due to deprecated wx methods

if ( UNIX )
	# Configure CCache if available
	find_program(CCACHE_FOUND ccache)
	if (CCACHE_FOUND)
		set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
		set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
	endif(CCACHE_FOUND)
endif(UNIX)

if ( UNIX )
    execute_process(COMMAND pwd OUTPUT_VARIABLE BUILD_DIRECTORY OUTPUT_STRIP_TRAILING_WHITESPACE)
elseif(MINGW)
    set ( BUILD_DIRECTORY ${CMAKE_BINARY_DIR} )
else()
    set ( BUILD_DIRECTORY $ENV{CD} )
endif()

message( "-- BUILD_DIRECTORY is set to ${BUILD_DIRECTORY}")

set (OS_NAME "WIN")
if (UNIX)
  execute_process(COMMAND uname -s OUTPUT_VARIABLE OS_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
message("-- OS name ${OS_NAME}")

if ( APPLE )
  set(WX_COMPONENTS "std")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
else ( APPLE )
  set(WX_COMPONENTS "std aui propgrid stc ribbon")
endif()

if (WITH_WXPATH)
    set(ENV{PATH} ${WITH_WXPATH}:$ENV{PATH})
endif()
unset(WITH_WXPATH CACHE)

set( CL_WX_CONFIG wx-config )

if (UNIX OR MINGW)
    execute_process(COMMAND which ${CL_WX_CONFIG} OUTPUT_VARIABLE WX_TOOL OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (NOT WX_TOOL)
        message(FATAL_ERROR
"\nNo functional wx_config script was found in your PATH.\nIs the wxWidgets development package installed?"
             )
    else()
        execute_process(COMMAND sh ${WX_TOOL} --version OUTPUT_VARIABLE WX_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
        string(SUBSTRING "${WX_VERSION}" "0" "1" wxMAJOR_VERSION)
        string(SUBSTRING "${WX_VERSION}" "2" "1" wxMINOR_VERSION)
        string(SUBSTRING "${WX_VERSION}" "4" "1" wxRELEASE_NUMBER)
        if ( wxMAJOR_VERSION LESS 3 )
        message(FATAL_ERROR
"\nI'm afraid your wxWidgets version is too old.\nBuilding CodeLite requires at least wxWidgets-3.0.0"
             )
        endif()
        if (MINGW)
          execute_process(COMMAND sh ${WX_TOOL} --debug=no --rescomp OUTPUT_VARIABLE WX_RC_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
          string(REGEX REPLACE "windres" "" WX_RC_FLAGS ${WX_RC_FLAGS})
          set (CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} ${WX_RC_FLAGS}")
          add_definitions(-D__WXMSW__)
        endif (MINGW)
    endif()
    message("-- wx-config used is: ${WX_TOOL}")
    message("-- wxWidgets version is: ${WX_VERSION}")
    if (NOT APPLE AND NOT MINGW)
        # Is the wx we are using built on gtk2 or 3?
        execute_process(COMMAND ${WX_TOOL} --selected_config OUTPUT_VARIABLE WX_GTK_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
        string(SUBSTRING "${WX_GTK_VERSION}" "3" "1" GTK_VERSION)
        message("-- gtk version is: ${GTK_VERSION}")
    endif()
endif (UNIX OR MINGW)

########################################
## Override defaults with user input
########################################

set( CL_INSTALL_LIBDIR "lib" )
if ( PREFIX )
  set ( CL_PREFIX ${PREFIX} )
else()
    # If the caller hasn't set his own destination, install to a multi-arch lib dir if applicable
    if (CMAKE_VERSION VERSION_GREATER 2.8.7 AND ( UNIX AND NOT APPLE ))
        if (CMAKE_VERSION VERSION_GREATER 3.0.0)
            # Prior to this, afaict the GNUInstallDirs module worked whatever the prefix
            # Since, it looks at CMAKE_INSTALL_PREFIX which is /usr/local by default, and refuses to run the multiarch-setting code unless it's /usr/
            # So, partly to comply with the default documented above, & partly for packaging, explicitly set it to /usr
            set (CMAKE_INSTALL_PREFIX "/usr")
        endif()
        include (GNUInstallDirs) # defines CMAKE_INSTALL_LIBDIR to lib or lib64 or whatever. Available since cmake 2.8.8
        set( CL_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR} )
    endif()
endif(PREFIX)

## Try to link-to or build lldb?
if (UNIX)
    set(WITH_LLDB 1)
else()
    set(WITH_LLDB 0)
endif()
if(ENABLE_LLDB MATCHES 0)
    set(WITH_LLDB 0)
    message("-- Disabling lldb support")
endif ()
unset(ENABLE_LLDB CACHE)

#######################################
## Locate libssh
#######################################
## Enable SFTP support?
if(ENABLE_SFTP MATCHES 0)
    set( WITH_SFTP 0 )
endif (ENABLE_SFTP MATCHES 0)
unset(ENABLE_SFTP CACHE)

if ( WITH_SFTP )
    if (UNIX AND NOT APPLE OR MINGW)
        ## Linux
        if (MINGW)
            find_library(LIBSSH_LIB NAMES ssh PATH_SUFFIXES lib)
            find_path(LIBSSH_INCLUDE_DIR NAMES libssh.h PATH_SUFFIXES include/libssh)
        else()
            find_library(LIBSSH_LIB NAMES libssh.so HINTS /usr/local/lib /usr/lib ${CMAKE_INSTALL_LIBDIR})
            find_path(LIBSSH_INCLUDE_DIR NAMES libssh.h HINTS /usr/local/include /usr/include PATH_SUFFIXES libssh)
        endif()
        string(FIND ${LIBSSH_INCLUDE_DIR} "NOTFOUND" LIBSSH_NOT_FOUND_POS)
        if ( LIBSSH_NOT_FOUND_POS GREATER -1 )
            if (UNIX AND NOT APPLE )
                ## Linux / FreeBSD
                message("**** NOTICE: Install libssh-dev and try again")
            else (UNIX AND NOT APPLE )
                ## OSX
                message("**** NOTICE: Install libssh and try again (brew install libssh)")
            endif (UNIX AND NOT APPLE )
            message(FATAL_ERROR "-- Could not find libssh")
        endif( LIBSSH_NOT_FOUND_POS GREATER -1 )

    else ( UNIX AND NOT APPLE OR MINGW )
        ## OSX
        set( LIBSSH_INCLUDE_DIR ${CL_SRC_ROOT}/sdk/libssh/include)
        set( LIBSSH_LIB ${CL_SRC_ROOT}/sdk/libssh/lib/osx/libssh.a)
        include_directories(${LIBSSH_INCLUDE_DIR})
    endif ( UNIX AND NOT APPLE OR MINGW )
    message("-- LIBSSH_LIB is set to ${LIBSSH_LIB}")
endif ( WITH_SFTP )

## enable clang support?
if(ENABLE_CLANG MATCHES 0)
  set( USE_CLANG 0 )
endif (ENABLE_CLANG MATCHES 0)
unset(ENABLE_CLANG CACHE)

if (AUTOGEN_REVISION MATCHES 0)
    set ( MAKE_AUTOGEN_REVISION_STRING 0)
else (AUTOGEN_REVISION MATCHES 1)
    set ( MAKE_AUTOGEN_REVISION_STRING 1)
endif (AUTOGEN_REVISION MATCHES 0)
unset(AUTOGEN_REVISION CACHE)

## build wxCrafter?
if ( WITH_WXC )
    set(BUILD_WXC 1)
    if ( UNIX AND NOT APPLE )
        set(WX_COMPONENTS "std aui propgrid stc richtext ribbon")
    endif (UNIX AND NOT APPLE )
endif ( WITH_WXC )
unset(WITH_WXC CACHE)

## package the wx libs?
if (COPY_WX_LIBS MATCHES 1)
  set( CL_COPY_WX_LIBS 1 )
endif()
unset(COPY_WX_LIBS CACHE)

## Will set PLUGINS_DIR to the proper location on Linux / OSX
include(OSXInstall)

## Under OSX, create the skeleton bundle directory
OSX_MAKE_BUNDLE_DIRECTORY()

add_definitions(-DYY_NEVER_INTERACTIVE=1)
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

if (NOT MINGW)
    add_definitions(-DINSTALL_DIR=\"${CL_PREFIX}/share/codelite\")
    add_definitions(-DPLUGINS_DIR=\"${PLUGINS_DIR}\")
else()
    add_definitions(-DNDEBUG)
    add_definitions(-DUSE_POSIX_LAYOUT)
endif()

message("-- PLUGINS_DIR is set to ${PLUGINS_DIR}")

## Allow user to use wxAuiNotebook instead of the native notebook
if ( GTK_USE_NATIVEBOOK )
    add_definitions( -DGTK_USE_NATIVEBOOK=1 )
    message("-- Using wxNotebook")
    
else ( GTK_USE_NATIVEBOOK )
    add_definitions( -DGTK_USE_NATIVEBOOK=0 )
    message("-- Using wxAuiNotebook")
    
endif ( GTK_USE_NATIVEBOOK )
unset(GTK_USE_NATIVEBOOK CACHE)

#############################################
## Global optimizations
#############################################

if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES DebugFull)
    message("-- Building in ${CMAKE_BUILD_TYPE} mode")
    set( DEBUG_BUILD 1 )
    set( CL_PCH_FILE "${CL_SRC_ROOT}/PCH/precompiled_header_dbg.h")
    set( CL_PCH_TARGET "precompiled_header_dbg.h.gch")

    ## Set the libraries outout directory
    set( CL_BIN_DIR bin)
    set( CL_LIB_DIR lib)
    add_custom_target(distclean COMMAND cd ${CL_SRC_ROOT}/PCH && $(MAKE) type=debug clean )

    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIRECTORY}/${CL_LIB_DIR}" )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIRECTORY}/${CL_BIN_DIR}" )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIRECTORY}/${CL_LIB_DIR}" )
    set( CL_LIBPATH                     "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

    message("-- Executables will be written into ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    message("-- Shared Objects will be written into ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")        ## No optimization, debug info

    ## In debug, only add NDEBUG if the user says so
    if (PREVENT_WX_ASSERTS MATCHES 1)
    message("-- Adding -DNDEBUG to definitions")
        add_definitions(-DNDEBUG)
    endif()

else ()
    message("-- Building in Release mode")
    set ( DEBUG_BUILD 0 )
    set(CMAKE_INSTALL_DO_STRIP TRUE)
    message("-- CMAKE_INSTALL_DO_STRIP is " ${CMAKE_INSTALL_DO_STRIP})
    if(UNIX AND NOT APPLE)
        ## Avoid hardening-no-relro wrarning messages from lintian
        if(EXISTS "/usr/bin/dpkg-buildflags")
            execute_process(COMMAND /usr/bin/dpkg-buildflags --get CFLAGS OUTPUT_VARIABLE EXTRA_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
            execute_process(COMMAND /usr/bin/dpkg-buildflags --get CPPFLAGS OUTPUT_VARIABLE EXTRA_CPPFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
            execute_process(COMMAND /usr/bin/dpkg-buildflags --get CXXFLAGS OUTPUT_VARIABLE EXTRA_CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
            execute_process(COMMAND /usr/bin/dpkg-buildflags --get LDFLAGS OUTPUT_VARIABLE EXTRA_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXXFLAGS}")
            add_definitions(${EXTRA_CPPFLAGS})
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")
            set(LINKER_OPTIONS "${EXTRA_LDFLAGS}")
            list(APPEND LINKER_OPTIONS "-s") ## strip binaries
        endif()
    endif()
    
    set( CL_PCH_FILE "${CL_SRC_ROOT}/PCH/precompiled_header_release.h")
    set( CL_PCH_TARGET "precompiled_header_release.h.gch")
    add_custom_target(distclean COMMAND cd ${CL_SRC_ROOT}/PCH && $(MAKE) type=release clean )

    ## Set the libraries outout directory
    set( CL_BIN_DIR bin)
    set( CL_LIB_DIR lib)

    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIRECTORY}/${CL_LIB_DIR}" )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIRECTORY}/${CL_BIN_DIR}" )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIRECTORY}/${CL_LIB_DIR}" )
    set( CL_LIBPATH                     "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

    message("-- Executables will be written into ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    message("-- Shared Objects will be written into ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")        ## Optimize
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
    endif(CMAKE_COMPILER_IS_GNUCXX)

    ## In release, add NDEBUG unless explicitly told not to
    if (NOT PREVENT_WX_ASSERTS MATCHES 0)
    message("-- Adding -DNDEBUG to definitions")
        add_definitions(-DNDEBUG)
    endif()


endif()
unset(CMAKE_BUILD_TYPE CACHE)
unset(PREVENT_WX_ASSERTS CACHE)

#############################################
## Determine if 32 or 64 bit
#############################################

set(ARCH 32)
set(ARCH_NAME i386)

if ( UNIX AND NOT APPLE )
    execute_process(COMMAND /bin/sh -c "lsb_release -a|grep -i Codename | cut -d: -f2"    OUTPUT_VARIABLE OS_CODENAME OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    execute_process(COMMAND /bin/sh -c "lsb_release -a|grep -i Distributor | cut -d: -f2" OUTPUT_VARIABLE OS_DISTRO OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    execute_process(COMMAND /bin/sh -c "uname -m" OUTPUT_VARIABLE OS_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)

    ## replace any tab/space with nothing
    string(REPLACE " " "" DISTRO_CODENAME "${OS_DISTRO}-${OS_CODENAME}-${OS_ARCH}")
    string(REPLACE "\t" "" DISTRO_CODENAME "${DISTRO_CODENAME}")
    string(TOLOWER ${DISTRO_CODENAME} DISTRO_CODENAME)

    set(CPACK_SYSTEM_NAME "${DISTRO_CODENAME}")
    message("-- CPACK_SYSTEM_NAME ${CPACK_SYSTEM_NAME}")
endif ( UNIX AND NOT APPLE )

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH 64)
  set(ARCH_NAME x86_64)
endif()

message("-- ARCH ${ARCH}")
message("-- ARCH_NAME ${ARCH_NAME}")

##############################################
## CPack
##############################################
if ( MAKE_DEB )
    message("-- Generating deb target")
    if( ${ARCH} EQUAL 32 )
        message("-- CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
    else ()
        message("-- CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
    endif ()

    set(CPACK_GENERATOR                   "DEB")
    set(CPACK_PACKAGE_NAME                "CodeLite")
    set(CPACK_PACKAGE_VERSION             "9.1.6")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER   "Eran Ifrah <eran.ifrah@gmail.com>")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C/C++/PHP and Node.js IDE (Integrated Development Environment)")
    set(CPACK_DEBIAN_PACKAGE_SECTION      "devel")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY     "optional")
    set(CPACK_DEBIAN_PACKAGE_RECOMMENDS   "build-essential, git, subversion, gdb, xterm, gcc, g++")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS      "libc6, libgtk2.0-dev, libsqlite3-dev, libssh-dev, libedit-dev, libhunspell-dev, libclang-3.5-dev, clang-format-3.5, liblldb-3.5-dev")
    set(CPACK_PACKAGE_DESCRIPTION_FILE    "${CL_SRC_ROOT}/DESC")
    set(CPACK_STRIP_FILES TRUE)
    INCLUDE(CPack)
endif( MAKE_DEB )

############################################
## Clang support
############################################

if (USE_CLANG)

if (APPLE)
  set( CLANG_INCLUDE "${CL_SRC_ROOT}/sdk/clang/include" )
  set( CLANG_LIBRARY "-L${CL_SRC_ROOT}/sdk/clang/lib -lclang.3.7" )
  set( CLANG_BINARY "${CL_SRC_ROOT}/sdk/clang/lib/libclang.3.7.dylib")

elseif (MINGW)
  find_package(LLVM)
  if (LLVM_FOUND)
    find_package(Clang)
    if (CLANG_FOUND)
        set( CLANG_INCLUDE "${CLANG_INCLUDE_DIRS}" )
        set( CLANG_LIBRARY "${CLANG_LDFLAGS}")
        set( CLANG_BINARY "")
    endif ()
  endif ()
elseif (UNIX)
    FIND_LIBCLANG_OFFICIAL()
    if ( LIBCLANG STREQUAL "LIBCLANG-NOTFOUND" )
        set(USE_CLANG 0)
    else()
        set(CLANG_LIBRARY ${LIBCLANG})
        set(CLANG_INCLUDE ${LIBCLANG_INCLUDE})
        set( CLANG_BINARY ${CLANG_LIBRARY})
    endif()
endif()

if (USE_CLANG)
    message( "-- clang link line ${CLANG_LIBRARY} ")
    message( "-- clang include path ${CLANG_INCLUDE} ")
    message( "-- clang binary ${CLANG_BINARY}" )
    message( "-- CL_SRC_ROOT is set to => " ${CL_SRC_ROOT} )
    message( "-- PREFIX      is set to => " ${CL_PREFIX} )
    message( "-- PREFIX/LIB_DIR is set to => " ${CL_PREFIX}/${CL_INSTALL_LIBDIR} )
    message( "-- PLUGINS_DIR is set to => " ${PLUGINS_DIR} )
    message( "-- wx-config   is set to => " ${WX_TOOL} )
else()
    message( "-- *** NOTICE ***: clang code completion support is disabled" )
    message( "-- *** NOTICE ***: On Ubuntu / Debian you might want to install libclang-3.4-dev package")
endif()

else()
    message( "-- *** NOTICE ***: clang code completion support is disabled" )
endif(USE_CLANG)

############################################
## SFTP support
############################################
if (WITH_SFTP)

    ## Default is set to 1
    add_definitions(-DUSE_SFTP=1)
    message("-- USE_SFTP is set to 1")

else(WITH_SFTP)
    add_definitions(-DUSE_SFTP=0)
    message("-- USE_SFTP is set to 0")
    message( "-- *** NOTICE ***: SFTP support is disabled " )

endif(WITH_SFTP)

###########################################
## RPATH settings
###########################################

if (UNIX AND NOT APPLE)
    SET(CMAKE_INSTALL_RPATH ${PLUGINS_DIR})
    SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
    #SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif(UNIX AND NOT APPLE)

if ( WITH_PCH )
    set( USE_PCH 1 )
endif( WITH_PCH )
unset(WITH_PCH CACHE)

###########################################
## Add the folders, the order matters here
###########################################
if ( USE_PCH )
    add_subdirectory(PCH)
endif ( USE_PCH )

if ( APPLE )
    add_definitions( -mmacosx-version-min=10.8 )
endif()

if(APPLE)
    # On Linux, we use the distro's libsqlite3
    add_subdirectory(sqlite3)
endif()

add_subdirectory(sdk/wxsqlite3)
add_subdirectory(sdk/wxshapeframework)
add_subdirectory(sdk/databaselayer)
add_subdirectory(CodeLite)
add_subdirectory(Plugin)
add_subdirectory(abbreviation)

if ( NOT NO_CPP_PLUGINS )
    add_subdirectory(CallGraph)
    add_subdirectory(ContinuousBuild)
    add_subdirectory(Debugger)
    add_subdirectory(Gizmos)
    add_subdirectory(Outline)
    add_subdirectory(QmakePlugin)
    add_subdirectory(UnitTestCPP)
    add_subdirectory(cppchecker)
    add_subdirectory(cscope)
    add_subdirectory(wxformbuilder)
    
if ( UNIX AND NOT APPLE )
    ## Add valgrind plugin
    add_subdirectory(MemCheck)
endif ( UNIX AND NOT APPLE )

    if(IS_DIRECTORY "${CL_SRC_ROOT}/wxcrafter")
        add_subdirectory(wxcrafter)
    endif()

    if ( APPLE )
        message("-- Adding MacBundler...")
        add_subdirectory(MacBundler)
    endif ( APPLE )
endif ( NOT NO_CPP_PLUGINS )

add_subdirectory(Tweaks)
add_subdirectory(CodeFormatter)
add_subdirectory(Copyright)
add_subdirectory(DatabaseExplorer)
add_subdirectory(ExternalTools)
add_subdirectory(SnipWiz)
add_subdirectory(Subversion2)
add_subdirectory(ZoomNavigator)
add_subdirectory(git)
add_subdirectory(CMakePlugin)
add_subdirectory(CodeLiteDiff)
add_subdirectory(SpellChecker)

if (WITH_SFTP)
    add_subdirectory(SFTP)
endif(WITH_SFTP)

if (WITH_LLDB)
    add_subdirectory(LLDBDebugger)
endif()
add_subdirectory(codelitephp)
add_subdirectory(WordCompletion)
add_subdirectory(HelpPlugin)
## Executables
add_subdirectory(LiteEditor)
add_subdirectory(codelitegcc)
add_subdirectory(codelite_make)
add_subdirectory(codelite_terminal)
add_subdirectory(sdk/codelite_indexer)
add_subdirectory(sdk/codelite_cppcheck)
add_subdirectory(codelite_echo)
add_subdirectory(WebTools)

##
## Setup the proper dependencies
##
if(APPLE)
    if ( USE_PCH )
        add_dependencies(sqlite3lib ${CL_PCH_TARGET})
    endif ( USE_PCH )
    add_dependencies(wxsqlite3 sqlite3lib)
endif()

add_dependencies(databaselayersqlite wxsqlite3)
add_dependencies(wxshapeframework wxsqlite3)
add_dependencies(libcodelite wxshapeframework databaselayersqlite wxsqlite3)
add_dependencies(plugin libcodelite)
add_dependencies(codelite plugin)

## Include our custom plugin.cmake module
include(plugin)

## Scan for user plugins
CL_SCAN_FOR_PLUGINS()

# Don't unset this earlier: it would affect the plugins' CMakeLists.txts
unset(PREFIX CACHE)
