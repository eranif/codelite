cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set( PLUGINS_DIR "/usr/lib/codelite")
set( CL_ROOT ${CMAKE_CURRENT_LIST_DIR})
message( "CL_ROOT     is set to => " ${CL_ROOT} )
message( "PLUGINS_DIR is set to => " ${PLUGINS_DIR} )

project(codelite)

## Add the folders, the order matters here
add_subdirectory(sqlite3)
add_subdirectory(sdk/wxsqlite3)
add_subdirectory(sdk/wxshapeframework)
add_subdirectory(sdk/databaselayer)
add_subdirectory(CodeLite)
add_subdirectory(Plugin)
add_subdirectory(abbreviation)

