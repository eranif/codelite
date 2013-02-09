# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(libcodelite)
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CL_SRC_ROOT}/lib" )

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS std REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

# Include paths
include_directories("${CL_SRC_ROOT}/sdk/wxsqlite3/include" "${CL_SRC_ROOT}/sdk/codelite_indexer/network" "${CL_SRC_ROOT}/PCH")

## Macros
if(WIN32)
    add_definitions(-DWXMAKINGDLL_CL)
endif(WIN32)

add_definitions(-DWXUSINGDLL_WXSQLITE3)

# Add RPATH
set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")

FILE(GLOB SRCS "*.cpp" "../sdk/codelite_indexer/network/*.cpp" )

# Define the output
add_library(libcodelite SHARED ${SRCS})
target_link_libraries(libcodelite ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES} -L${CL_SRC_ROOT}/lib -lsqlite3 -lwxsqlite3 )
install(TARGETS libcodelite DESTINATION ${PLUGINS_DIR})

