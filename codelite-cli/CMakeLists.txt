# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
set(CL_SRC_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CL_SRC_ROOT}/cmake/Modules/")
set(BUILD_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIRECTORY}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIRECTORY}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIRECTORY}/lib")

# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(codelite-cli)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS base xml REQUIRED)
include("${wxWidgets_USE_FILE}")

if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()
set(CL_WX_CONFIG "wx-config")
# we need wxWidgets flags to be set only for the c++ files, so we do it like this
# by setting the CMAKE_CXX_FLAGS
if(NOT MINGW)
    execute_process(COMMAND ${CL_WX_CONFIG} --cxxflags OUTPUT_VARIABLE WX_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    execute_process(COMMAND sh ${CL_WX_CONFIG} --cxxflags OUTPUT_VARIABLE WX_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WX_CXX_FLAGS}")
message("${WX_CXX_FLAGS}")

# prepare list of files
FILE(GLOB CODELITELIB_SRCS
          "${CL_SRC_ROOT}/CodeLite/SocketAPI/*.cpp"
          "${CL_SRC_ROOT}/sdk/wxsqlite3/src/*.cpp"
          "${CL_SRC_ROOT}/CodeLite/*.c"
          "${CL_SRC_ROOT}/CodeLite/*.cpp"
          "${CL_SRC_ROOT}/sdk/codelite_indexer/network/*.cpp"
          "${CL_SRC_ROOT}/codelite-cli/sqlite3.c"
          )

FILE(GLOB CODELITESERVER_SRCS "*.cpp")

# and finally prepare list of includes directories
include_directories("${CL_SRC_ROOT}/sdk/wxsqlite3/include" 
                    "${CL_SRC_ROOT}/CodeLite"
                    "${CL_SRC_ROOT}/sdk/codelite_indexer/network")

# Define the output
add_library(codelitelib STATIC ${CODELITELIB_SRCS})
target_compile_definitions(codelitelib PRIVATE wxUSE_GUI=0)

add_executable(codelite-cli ${CODELITESERVER_SRCS})
target_compile_definitions(codelite-cli PRIVATE wxUSE_GUI=0)

set(UTIL_LIB "")
if(UNIX)
    set(UTIL_LIB "-lutil -ldl")
endif(UNIX)

target_link_libraries(codelite-cli 
                        codelitelib
                        ${LINKER_OPTIONS} 
                        ${wxWidgets_LIBRARIES} 
                        ${UTIL_LIB})
#add_subdirectory(codelite-client)
