# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'wxshapeframework' this is how it will be called in
# visual studio, and in our makefiles. 
project(wxshapeframework)

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../../lib )

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS std REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

if(WIN32)
    add_definitions(-DWXMAKINGDLL_WXXS)
    add_definitions(-DWXMAKINGDLL_WXSF)
endif(WIN32)

include_directories(./include)

# Add RPATH
SET (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")

FILE(GLOB SRCS "src/*.cpp")

# Define the output
add_library(wxshapeframework SHARED ${SRCS})
target_link_libraries(wxshapeframework ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES})
install(TARGETS wxshapeframework DESTINATION ${PLUGINS_DIR})
