# define minimum cmake version
cmake_minimum_required(VERSION 3.0)
 
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(codelite_indexer)

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS core base REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
#include( "${wxWidgets_USE_FILE}" )

# Include paths
#include_directories("${CL_SRC_ROOT}/Plugin" "${CL_SRC_ROOT}/sdk/wxsqlite3/include" "${CL_SRC_ROOT}/CodeLite" "${CL_SRC_ROOT}/PCH" "${CL_SRC_ROOT}/Interfaces")

# we need wxWidgets flags to be set only for the c++ files, so we do it like this
# by setting the CMAKE_CXX_FLAGS
if ( NOT MINGW )
execute_process(COMMAND ${CL_WX_CONFIG} --cxxflags OUTPUT_VARIABLE WX_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
else ()
execute_process(COMMAND sh ${CL_WX_CONFIG} --cxxflags OUTPUT_VARIABLE WX_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WX_CXX_FLAGS}")

# prepare list of files
FILE(GLOB SRCS "*.cpp" "libctags/*.c" "network/*.cpp")

if (UNIX)
# we require this macro to be set to true
add_definitions(-DHAVE_CONFIG_H)
endif (UNIX)

# and finally prepare list of includes directories
include_directories("${CL_SRC_ROOT}/sdk/codelite_indexer/libctags")

# Define the output
add_executable(codelite_indexer ${SRCS})
target_link_libraries(codelite_indexer ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES})

CL_INSTALL_EXECUTABLE(codelite_indexer)

