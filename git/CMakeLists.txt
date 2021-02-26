# define minimum cmake version
cmake_minimum_required(VERSION 3.0)

# set the plugin name here
set( PLUGIN_NAME "git")

# Our project is called 'plugin' this is how it will be called in
# visual studio, and in our makefiles. 
project( ${PLUGIN_NAME} )

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS ${WX_COMPONENTS} REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

# Include paths
include_directories("${CL_SRC_ROOT}/Plugin" "${CL_SRC_ROOT}/sdk/wxsqlite3/include" "${CL_SRC_ROOT}/CodeLite" "${CL_SRC_ROOT}/PCH" "${CL_SRC_ROOT}/Interfaces")

add_definitions(-DWXUSINGDLL_WXSQLITE3)
add_definitions(-DWXUSINGDLL_CL)
add_definitions(-DWXUSINGDLL_SDK)

if (UNIX AND NOT APPLE)
    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )
endif()

if ( APPLE )
    add_definitions(-fPIC)
endif()


FILE(GLOB SRCS "*.cpp")

# Define the output
add_library(${PLUGIN_NAME} SHARED ${SRCS})

# Remove the "lib" prefix from the plugin name
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_link_libraries(${PLUGIN_NAME}
                      ${LINKER_OPTIONS}
                      ${wxWidgets_LIBRARIES}
                      libcodelite
                      plugin
                      )

CL_INSTALL_PLUGIN(${PLUGIN_NAME})
