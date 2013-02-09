cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

###############################################################
##
## codelite IDE cmake file
## Typical usage will be (build in release mode):
##
## > mkdir build
## > cd build
## > cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
## > make -jN
## > sudo make install
##
## Optional command line arguments:
##    
##      -DCMAKE_BUILD_TYPE=Relase|Debug
##      -DPREFIX="<some-prefix>"
##      -DENABLE_CLANG=1|0
##      -DWX_CONFIG=<path-to-my-wx-config-tool>
##
###############################################################
#############################################
## Defaults
#############################################

set( CL_PREFIX "/usr" )
set( CL_SRC_ROOT ${CMAKE_CURRENT_LIST_DIR})
set( CL_WX_CONFIG wx-config )
set( USE_CLANG 1 )

########################################
## Override defaults with user input
########################################

if ( PREFIX )
  set ( CL_PREFIX ${PREFIX} )
endif(PREFIX)

if(ENABLE_CLANG MATCHES 0)
  set( USE_CLANG 0 )
endif (ENABLE_CLANG MATCHES 0)

if ( WX_CONFIG )
  set ( CL_WX_CONFIG ${WX_CONFIG} )
endif(WX_CONFIG)

set( PLUGINS_DIR "${CL_PREFIX}/lib/codelite")
add_definitions(-DYY_NEVER_INTERACTIVE=1)
add_definitions(-DINSTALL_DIR=\"${CL_PREFIX}/share/codelite\")
add_definitions(-DPLUGINS_DIR=\"${PLUGINS_DIR}\")

#############################################
## Global optimizations
#############################################

if(CMAKE_BUILD_TYPE MATCHES Release) 
	message("-- Building in Release mode")
	if(CMAKE_COMPILER_IS_GNUCXX)
		set(CMAKE_CXX_FLAGS "-O2")        ## Optimize
		set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
	endif(CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-DNDEBUG)

else ()
	message("-- Building in Debug mode")
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
message( "-- PLUGINS_DIR is set to => " ${PLUGINS_DIR} )
message( "-- PREFIX      is set to => " ${CL_PREFIX} )
message( "-- wx-config   is set to => " ${CL_WX_CONFIG} )

else()
message( "--WARNING: clang support is disabled" )
endif(USE_CLANG)

###########################################
## RPATH settings
###########################################

SET(CMAKE_INSTALL_RPATH ${PLUGINS_DIR})
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


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
add_dependencies(wxsqlite3 sqlite3)
add_dependencies(databaselayersqlite wxsqlite3)
add_dependencies(wxshapeframework wxsqlite3)
add_dependencies(libcodelite wxshapeframework databaselayersqlite sqlite3)
add_dependencies(plugin libcodelite)
add_dependencies(codelite plugin)

