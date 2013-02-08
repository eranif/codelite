# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'databaselayersqlite' this is how it will be called in
# visual studio, and in our makefiles. 
project(databaselayersqlite)
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../../lib )

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS std REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

# Include paths
include_directories(./include/wx/dblayer/include src/sqlite3)

# Macros
if(WIN32)
    add_definitions(-DWXMAKINGDLL_DATABASELAYER)
endif(WIN32)

# Add RPATH
SET (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")

FILE(GLOB SRCS "src/dblayer/Sqlite*.cpp" "src/dblayer/Database*.cpp" "src/dblayer/Prepared*.cpp" "src/sqlite3/*.c")

# Define the output
add_library(databaselayersqlite SHARED ${SRCS})
target_link_libraries(databaselayersqlite ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES})
install(TARGETS databaselayersqlite DESTINATION ${PLUGINS_DIR})
