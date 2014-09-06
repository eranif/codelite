# define minimum cmake version
cmake_minimum_required(VERSION 2.8)
project ("codelitephp")
# set the plugin name here
set( PLUGIN_NAME "codelitephp")

if (NOT MINGW)
add_definitions(-DPLUGINS_DIR=\"${PLUGINS_DIR}\")
endif()
add_definitions(-DUSE_SFTP=1)

# Our project is called 'plugin' this is how it will be called in
# visual studio, and in our makefiles. 
project( ${PLUGIN_NAME} )

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
if (WITH_WEBVIEW)
    message("-- Internal Web Browser is enabled")
    find_package(wxWidgets COMPONENTS adv aui base core xml xrc net stc webview REQUIRED)
else()
    message("-- Internal Web Browser is disabled")
    find_package(wxWidgets COMPONENTS adv aui base core xml xrc net stc REQUIRED)
endif()

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

# Validate that -DCL_SRC_ROOT was passed
if ( NOT CL_SRC_ROOT) 
    message(FATAL_ERROR "**** CL_SRC_ROOT variable is not set. Please set to codelite's source folder")
else ( NOT CL_SRC_ROOT )
    message("-- CL_SRC_ROOT is set to ${CL_SRC_ROOT}")
endif( NOT CL_SRC_ROOT )

# Include paths
include_directories("${CL_SRC_ROOT}/Plugin" 
                    "${CL_SRC_ROOT}/sdk/wxsqlite3/include" 
                    "${CL_SRC_ROOT}/CodeLite" 
                    "${CL_SRC_ROOT}/PCH" 
                    "${CL_SRC_ROOT}/Interfaces" 
                    "${CL_SRC_ROOT}/codelitephp/PHPParser" 
                    "${CL_SRC_ROOT}/codelitephp/php-plugin"
                    "${CL_SRC_ROOT}/codelitephp/PHPParserUnitTests")

add_definitions(-DWXUSINGDLL_WXSQLITE3)
add_definitions(-DWXUSINGDLL_CL)
add_definitions(-DWXUSINGDLL_SDK)

if (UNIX)
# Add RPATH
set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES DebugFull)
    ## Debug build of codelite
    set( CL_LIB_DIR lib)
    set ( CL_LIBPATH ${CL_SRC_ROOT}/build-debug/lib )
    
else (CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES DebugFull)
    ## Release build of codelite
    set( CL_LIB_DIR lib)
    set ( CL_LIBPATH ${CL_SRC_ROOT}/build-release/lib )
    
endif (CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES DebugFull)


FILE(GLOB SRCS "php-plugin/*.cpp" "php-plugin/*.c")
FILE(GLOB LIBSRC "PHPParser/*.cpp")
FILE(GLOB UNIT_TESTS_SRC "PHPParserUnitTests/*.cpp")

# Define the output
add_library(${PLUGIN_NAME} SHARED ${SRCS} )
add_library(PHPParser STATIC ${LIBSRC})
add_executable(PHPUnitTests ${UNIT_TESTS_SRC})

if (UNIX)
set ( CMAKE_C_FLAGS "-fPIC" )
set ( CMAKE_CXX_FLAGS "-fPIC" )
endif()
set( ADDITIONAL_LIBRARIES "" )
if (UNIX)
    set(ADDITIONAL_LIBRARIES "-lutil")
endif()
# Remove the "lib" prefix from the plugin name
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} -L"${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" -L"${CL_LIBPATH}" -lPHPParser -lwxsqlite3 -lsqlite3lib -llibcodelite -lplugin ${ADDITIONAL_LIBRARIES} ${wxWidgets_LIBRARIES})
target_link_libraries(PHPUnitTests ${LINKER_OPTIONS} -L"${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" -L"${CL_LIBPATH}" -lPHPParser -lwxsqlite3 -lsqlite3lib -llibcodelite -lplugin ${ADDITIONAL_LIBRARIES} ${wxWidgets_LIBRARIES})

# Make sure that the plugin will not start build before 'plugin.so' is ready
add_dependencies(${PLUGIN_NAME} plugin PHPParser)
add_dependencies(PHPUnitTests plugin PHPParser)

install(TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGINS_DIR})
install(
  DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/resources/cc
  DESTINATION ${PLUGINS_DIR}/resources/php
  USE_SOURCE_PERMISSIONS
  PATTERN ".svn" EXCLUDE
  PATTERN ".git" EXCLUDE
)

target_link_libraries(${PLUGIN_NAME} PHPParser)
target_link_libraries(PHPUnitTests PHPParser)

