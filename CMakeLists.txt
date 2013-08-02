cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

#######################################################################################################################################################################
##                                                                                                                                                                    #
## codelite IDE cmake file                                                                                                                                            #
## Typical usage will be (build in release mode):                                                                                                                     #
##                                                                                                                                                                    #
## > mkdir build                                                                                                                                                      #
## > cd build                                                                                                                                                         #
## > cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..                                                                                                          #
## > make -jN                                                                                                                                                         #
## > sudo make install                                                                                                                                                #
##                                                                                                                                                                    #
## Optional command line arguments:                                                                                                                                   #
##                                                                                                                                                                    #
##      -DCMAKE_BUILD_TYPE=Release|Debug|DebugFull // Build release, debug + optimisation or debug without optimisation (for others see the Cmake docs)               #
##      -DPREFIX="<some-prefix>"                   // Installation prefix, default is set to /usr                                                                     #
##      -DENABLE_CLANG=1|0                         // Build codelite with clang code completion support?, default is 1 (with clang)                                   #
##      -DWITH_WXC=1|0                             // Build wxCrafter (sources are not part of codelite distribution) default is 0                                    #
##      -DCOPY_WX_LIBS=1|0                         // Incorporate the wxWidgets libs into CodeLite so the binary doesn't depend on them. default is 0                 #
##      -DPREVENT_WX_ASSERTS=1|0                   // Prevent those annoying wxASSERTS. In release builds the default is 1, in debug 0                                #
##      -DAUTOGEN_REVISION=1|0                     // Should cmake generate makefiles that auto generates the autoversion.cpp file - default is 1                     #
##      -DWITH_PCH=1|0                             // Enable Pre Compiled Header?                                                                                     #
##      -DGTK_USE_AUIBOOK=1|0                      // Under GTK, use wxAuiNotebook instead of the native notebook                                                     #
##      -DWITH_WX_CONFIG=<fullpath>                // Specify a particular wxWidgets build to use. The format must be /path/to/different/wx-config                    #
#######################################################################################################################################################################


#############################################
## Defaults
#############################################

set( CL_PREFIX "/usr" )
if (CMAKE_CURRENT_LIST_DIR) # since cmake 2.8.3
    set( CL_SRC_ROOT ${CMAKE_CURRENT_LIST_DIR})
else()
    set( CL_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}) # which seems to be the same, at least in this situation
endif()

set( USE_CLANG 1 )
set( IS_FREEBSD 0 )
set( BUILD_WXC 0 )
set( CL_COPY_WX_LIBS 0 )

if ( UNIX )
    execute_process(COMMAND pwd OUTPUT_VARIABLE BUILD_DIRECTORY OUTPUT_STRIP_TRAILING_WHITESPACE)
else ( UNIX)
    set ( BUILD_DIRECTORY $ENV{CD} )
endif ( UNIX )

message( "-- BUILD_DIRECTORY is set to ${BUILD_DIRECTORY}")

set (OS_NAME "WIN")
if (UNIX)
  execute_process(COMMAND uname -s OUTPUT_VARIABLE OS_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
message("-- OS name ${OS_NAME}")

if ( APPLE )
  set(WX_COMPONENTS "std")
else ( APPLE )
  set(WX_COMPONENTS "std aui propgrid stc")
endif()

if (WITH_WX_CONFIG)
    set(CL_WX_CONFIG ${WITH_WX_CONFIG})
else()
    set( CL_WX_CONFIG wx-config )
endif()
unset(WITH_WX_CONFIG CACHE)

if (UNIX)
    execute_process(COMMAND which ${CL_WX_CONFIG} OUTPUT_VARIABLE WX_TOOL OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (NOT WX_TOOL)
      message(FATAL_ERROR
"\nNo functional wx_config script was found in your PATH.\nIs the wxWidgets development package installed?"
             )
    else()
      execute_process(COMMAND ${WX_TOOL} --version OUTPUT_VARIABLE WX_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
      string(SUBSTRING "${WX_VERSION}" "0" "1" wxMAJOR_VERSION)
      string(SUBSTRING "${WX_VERSION}" "2" "1" wxMINOR_VERSION)
      string(SUBSTRING "${WX_VERSION}" "4" "1" wxRELEASE_NUMBER)
      if ( wxMAJOR_VERSION LESS 3 AND (wxMINOR_VERSION LESS 9 OR wxRELEASE_NUMBER LESS 5) )
        message(FATAL_ERROR
"\nI'm afraid your wxWidgets version is too old.\nBuilding CodeLite requires at least wxWidgets-2.9.5"
             )
      endif()
    endif()
    message("-- wx-config used is: ${WX_TOOL}")
    message("-- wxWidgets version is: ${WX_VERSION}")
endif (UNIX)

########################################
## Override defaults with user input
########################################

set( CL_INSTALL_LIBDIR "lib" )
if ( PREFIX )
  set ( CL_PREFIX ${PREFIX} )
else()
    # If the caller hasn't set his own destination, install to a multi-arch lib dir if applicable
    if (CMAKE_VERSION VERSION_GREATER 2.8.7 AND ( UNIX AND NOT APPLE ))
        include (GNUInstallDirs) # defines CMAKE_INSTALL_LIBDIR to lib or lib64 or whatever. Available since cmake 2.8.8
        set( CL_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR} )
    endif()
endif(PREFIX)

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

set( PLUGINS_DIR "${CL_PREFIX}/${CL_INSTALL_LIBDIR}/codelite")
add_definitions(-DYY_NEVER_INTERACTIVE=1)
add_definitions(-DINSTALL_DIR=\"${CL_PREFIX}/share/codelite\")
add_definitions(-DPLUGINS_DIR=\"${PLUGINS_DIR}\")

## Allow user to use wxAuiNotebook instead of the native notebook
if ( GTK_USE_AUIBOOK )
    add_definitions( -DGTK_USE_AUIBOOK=1 )
    message("-- Using wxAuiNotebook instead of wxNotebook")
endif ( GTK_USE_AUIBOOK )
unset(GTK_USE_AUIBOOK CACHE)

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

    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "-g -O0")        ## No optimization, debug info
    endif(CMAKE_COMPILER_IS_GNUCXX)

    ## In debug, only add NDEBUG if the user says so
    if (PREVENT_WX_ASSERTS MATCHES 1)
    message("-- Adding -DNDEBUG to definitions")
        add_definitions(-DNDEBUG)
    endif()
    
else ()
    message("-- Building in Release mode")
    set ( DEBUG_BUILD 0 ) 
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

    set(CMAKE_CXX_FLAGS "-O2")        ## Optimize
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
    set(CPACK_PACKAGE_NAME                "codelite")
    set(CPACK_PACKAGE_VERSION             "5.2")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER   "Eran Ifrah <eran.ifrah@gmail.com>") #required
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "codelite IDE for C/C++")
    set(CPACK_DEBIAN_PACKAGE_SECTION      "devel")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY     "optional")
    set(CPACK_DEBIAN_PACKAGE_RECOMMENDS   "build-essential")
    INCLUDE(CPack)
endif( MAKE_DEB )

############################################
## Clang support
############################################

if (USE_CLANG)

if (APPLE)
  set( CLANG_INCLUDE "${CL_SRC_ROOT}/sdk/clang/include" )
  set( CLANG_LIBRARY "-L${CL_SRC_ROOT}/sdk/clang/lib -lclang" )
  set( CLANG_BINARY "${CL_SRC_ROOT}/sdk/clang/lib/libclang.dylib")

elseif (UNIX)
  set( CLANG_INCLUDE "${CL_SRC_ROOT}/sdk/clang/Linux/${ARCH_NAME}/include" )
  set( CLANG_LIBRARY "-L${CL_SRC_ROOT}/sdk/clang/Linux/${ARCH_NAME}/lib -lclang" )
  set( CLANG_BINARY "${CL_SRC_ROOT}/sdk/clang/Linux/${ARCH_NAME}/lib/libclang.so")
endif()

message( "-- clang link line ${CLANG_LIBRARY} ")
message( "-- clang include path ${CLANG_INCLUDE} ")
message( "-- clang binary ${CLANG_BINARY}" )
message( "-- CL_SRC_ROOT is set to => " ${CL_SRC_ROOT} )
message( "-- PREFIX      is set to => " ${CL_PREFIX} )
message( "-- PREFIX/LIB_DIR is set to => " ${CL_PREFIX}/${CL_INSTALL_LIBDIR} )
message( "-- PLUGINS_DIR is set to => " ${PLUGINS_DIR} )
message( "-- wx-config   is set to => " ${WX_TOOL} )

else()
message( "-- *** NOTICE ***: clang support is disabled" )
endif(USE_CLANG)

if ( APPLE )
    execute_process(COMMAND mkdir -p pack)
    execute_process(COMMAND cp ../Runtime/make_mac_bundle.sh pack)
endif( APPLE )

###########################################
## RPATH settings
###########################################

if (UNIX AND NOT APPLE)
    SET(CMAKE_INSTALL_RPATH ${PLUGINS_DIR})
    SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
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

add_subdirectory(sqlite3)
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
    
    if ( BUILD_WXC )
        add_subdirectory(wxcrafter)
    endif( BUILD_WXC )
    
    if ( APPLE )
        message("-- Adding MacBundler...")
        add_subdirectory(MacBundler)
    endif ( APPLE )
endif ( NOT NO_CPP_PLUGINS )

add_subdirectory(CodeFormatter)
add_subdirectory(Copyright)
add_subdirectory(DatabaseExplorer)
add_subdirectory(ExternalTools)
add_subdirectory(SnipWiz)
add_subdirectory(Subversion2)
add_subdirectory(ZoomNavigator)
add_subdirectory(git)

## Executables
add_subdirectory(LiteEditor)
add_subdirectory(codelitegcc)
add_subdirectory(sdk/codelite_indexer)
add_subdirectory(sdk/codelite_cppcheck)

if (USE_CLANG)
    add_subdirectory(codelite_clang)
endif(USE_CLANG)

##
## Setup the proper dependencies
##
if ( USE_PCH )
    add_dependencies(sqlite3lib ${CL_PCH_TARGET})
endif ( USE_PCH )

add_dependencies(wxsqlite3 sqlite3lib)
add_dependencies(databaselayersqlite wxsqlite3)
add_dependencies(wxshapeframework wxsqlite3)
add_dependencies(libcodelite wxshapeframework databaselayersqlite sqlite3 wxsqlite3)
add_dependencies(plugin libcodelite)
add_dependencies(codelite plugin)

# Don't unset this earlier: it would affect the plugins' CMakeLists.txts
unset(PREFIX CACHE)
