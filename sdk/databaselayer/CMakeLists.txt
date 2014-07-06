# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'databaselayersqlite' this is how it will be called in
# visual studio, and in our makefiles. 
project(databaselayersqlite)

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS std REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

# Include paths
include_directories(./include/wx/dblayer/include src/sqlite3)

if ( USE_PCH )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${CL_PCH_FILE} -Winvalid-pch ")
endif ( USE_PCH )

# Macros
if(WIN32)
    add_definitions(-DWXMAKINGDLL_DATABASELAYER)
endif(WIN32)

# Add RPATH
if (UNIX)
set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")
endif (UNIX)

FILE(GLOB SRCS "src/dblayer/Sqlite*.cpp" "src/dblayer/Database*.cpp" "src/dblayer/Prepared*.cpp" "src/sqlite3/*.c")

if ( WITH_MYSQL )
    find_library(LIBMYSQLCLIENT NAMES mysql mysqlclient mariadb mariadbclient)
    find_path(MYSQLCLIENT_INCLUDE NAMES mysql.h PATH_SUFFIXES mysql mariadb)
    
    if (  ${LIBMYSQLCLIENT} STREQUAL "LIBMYSQLCLIENT-NOTFOUND") 
        message( FATAL_ERROR "-- Could not locate libmysqlclient.so" )
    else ( ${LIBMYSQLCLIENT} STREQUAL "LIBMYSQLCLIENT-NOTFOUND" )
        message( "-- LIBMYSQLCLIENT is set to ${LIBMYSQLCLIENT}" )
    endif ( ${LIBMYSQLCLIENT} STREQUAL "LIBMYSQLCLIENT-NOTFOUND" )
    
    if ( ${MYSQLCLIENT_INCLUDE} STREQUAL "MYSQLCLIENT_INCLUDE-NOTFOUND" )
        message( FATAL_ERROR "-- Could not locate mysql.h" )
    endif ( ${MYSQLCLIENT_INCLUDE} STREQUAL "MYSQLCLIENT_INCLUDE-NOTFOUND" )
    
    add_definitions( -DDBL_USE_MYSQL=1 )
    include_directories(${MYSQLCLIENT_INCLUDE})
    message("-- Adding MySQL include path: ${MYSQLCLIENT_INCLUDE} ")
    FILE(GLOB MYSQL_SRCS "src/dblayer/Mysql*.cpp")
endif ( WITH_MYSQL )

# Define the output
add_library(databaselayersqlite SHARED ${SRCS} ${MYSQL_SRCS})
target_link_libraries(databaselayersqlite ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES})
install(TARGETS databaselayersqlite DESTINATION ${PLUGINS_DIR})
