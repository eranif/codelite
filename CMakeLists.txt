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
##      -DCMAKE_BUILD_TYPE=Relase|Debug          // Build Debug Or Release                                                                                            #
##      -DPREFIX="<some-prefix>"                 // Installation prefix, default is set to /usr                                                                       #
##      -DENABLE_CLANG=1|0                       // Build codelite with clang code completion support?, default is 1 (with clang)                                     #
##      -DWITH_WXC=1|0                           // Build wxCrafter (sources are not part of codelite distribution) default is 0                                      #
##      -DCOPY_WX_LIBS=1|0                       // Incorporate the wxWidgets libs into CodeLite so the binary doesn't depend on them. default is 0                   #
##                                                                                                                                                                    #
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
set( CL_WX_CONFIG wx-config )
set( USE_CLANG 1 )
set( IS_FREEBSD 0 )
set( BUILD_WXC 0 )
set( CL_COPY_WX_LIBS 0 )

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

if (UNIX)
    execute_process(COMMAND which wx-config OUTPUT_VARIABLE WX_TOOL OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (NOT WX_TOOL)
      message(FATAL_ERROR
"\nNo functional wx_config script was found in your PATH.\nIs the wxWidgets development package installed?"
             )
    else()
      execute_process(COMMAND ${WX_TOOL} --version OUTPUT_VARIABLE WX_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
      string(SUBSTRING "${WX_VERSION}" "0" "1" wxMAJOR_VERSION)
      string(SUBSTRING "${WX_VERSION}" "2" "1" wxMINOR_VERSION)
      string(SUBSTRING "${WX_VERSION}" "4" "1" wxRELEASE_NUMBER)
      if ( wxMAJOR_VERSION LESS 3 AND (wxMINOR_VERSION LESS 9 OR wxRELEASE_NUMBER LESS 4) )
        message(FATAL_ERROR
"\nI'm afraid your wxWidgets version is too old.\nBuilding CodeLite requires at least wxWidgets-2.9.4"
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

## build wxCrafter?
if ( WITH_WXC )
    set(BUILD_WXC 1)
    if ( UNIX AND NOT APPLE )
        set(WX_COMPONENTS "std aui propgrid stc richtext")
    endif (UNIX AND NOT APPLE )
endif ( WITH_WXC )

## package the wx libs?
if (COPY_WX_LIBS MATCHES 1)
  set( CL_COPY_WX_LIBS 1 )
endif()

set( PLUGINS_DIR "${CL_PREFIX}/${CL_INSTALL_LIBDIR}/codelite")
add_definitions(-DYY_NEVER_INTERACTIVE=1)
add_definitions(-DINSTALL_DIR=\"${CL_PREFIX}/share/codelite\")
add_definitions(-DPLUGINS_DIR=\"${PLUGINS_DIR}\")

#############################################
## Global optimizations
#############################################

if(CMAKE_BUILD_TYPE MATCHES Release)
    message("-- Building in Release mode")
    
    ## Set the libraries outout directory
    set( CL_BIN_DIR bin)
    set( CL_LIB_DIR lib)

    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CL_SRC_ROOT}/${CL_LIB_DIR}" )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CL_SRC_ROOT}/${CL_BIN_DIR}" )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CL_SRC_ROOT}/${CL_LIB_DIR}" )
    set( CL_LIBPATH                     "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

    message("-- Executables will be written into ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    message("-- Shared Objects will be written into ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "-O2")        ## Optimize
        set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
    endif(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-DNDEBUG)

else ()
    message("-- Building in Debug mode")
    ## Set the libraries outout directory
    set( CL_BIN_DIR bin_debug)
    set( CL_LIB_DIR lib_debug)

    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CL_SRC_ROOT}/${CL_LIB_DIR}" )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CL_SRC_ROOT}/${CL_BIN_DIR}" )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CL_SRC_ROOT}/${CL_LIB_DIR}" )
    set( CL_LIBPATH                     "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

    message("-- Executables will be written into ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    message("-- Shared Objects will be written into ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "-g -O0")        ## No optimization, debug info
    endif(CMAKE_COMPILER_IS_GNUCXX)

endif()

#############################################
## Determine if 32 or 64 bit
#############################################

set(ARCH 32)
set(ARCH_NAME i386)

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
    set(CPACK_PACKAGE_VERSION             "5.1")
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
message( "--WARNING: clang support is disabled" )
endif(USE_CLANG)

###########################################
## RPATH settings
###########################################

if (UNIX AND NOT APPLE)
    SET(CMAKE_INSTALL_RPATH ${PLUGINS_DIR})
    SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif(UNIX AND NOT APPLE)

###########################################
## Add the folders, the order matters here
###########################################

add_subdirectory(sqlite3)
add_subdirectory(sdk/wxsqlite3)
add_subdirectory(sdk/wxshapeframework)
add_subdirectory(sdk/databaselayer)
add_subdirectory(CodeLite)
add_subdirectory(Plugin)
add_subdirectory(abbreviation)
add_subdirectory(CallGraph)
add_subdirectory(CodeFormatter)
add_subdirectory(ContinuousBuild)
add_subdirectory(Copyright)
add_subdirectory(DatabaseExplorer)
add_subdirectory(Debugger)
add_subdirectory(ExternalTools)
add_subdirectory(Gizmos)
add_subdirectory(Outline)
add_subdirectory(SnipWiz)
add_subdirectory(QmakePlugin)
add_subdirectory(Subversion2)
add_subdirectory(UnitTestCPP)
add_subdirectory(ZoomNavigator)
add_subdirectory(cppchecker)
add_subdirectory(git)
add_subdirectory(cscope)
add_subdirectory(wxformbuilder)

if ( BUILD_WXC )
    add_subdirectory(wxcrafter)
endif( BUILD_WXC )

## Executables
add_subdirectory(LiteEditor)
add_subdirectory(codelitegcc)
add_subdirectory(sdk/codelite_indexer)
add_subdirectory(sdk/codelite_cppcheck)
if (USE_CLANG)
  add_subdirectory(codelite_clang)
endif(USE_CLANG)

if ( APPLE )
message(Adding MacBundler...)
add_subdirectory(MacBundler)
endif ( APPLE )

##
## Setup the proper dependencies
##
add_dependencies(wxsqlite3 sqlite3lib)
add_dependencies(databaselayersqlite wxsqlite3)
add_dependencies(wxshapeframework wxsqlite3)
add_dependencies(libcodelite wxshapeframework databaselayersqlite sqlite3)
add_dependencies(plugin libcodelite)
add_dependencies(codelite plugin)

