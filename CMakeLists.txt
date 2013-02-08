cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set( PREFIX "/usr" )
set( PLUGINS_DIR "/usr/lib/codelite")
set( CL_ROOT ${CMAKE_CURRENT_LIST_DIR})

message( "CL_ROOT     is set to => " ${CL_ROOT} )
message( "PLUGINS_DIR is set to => " ${PLUGINS_DIR} )
##
## RPATH settings
##
SET(CMAKE_INSTALL_RPATH ${PLUGINS_DIR})
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

##
## Add the folders, the order matters here
##
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
add_subdirectory(LiteEditor)

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

