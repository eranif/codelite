


# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(codelite-client)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.

include("${wxWidgets_USE_FILE}")

# we need wxWidgets flags to be set only for the c++ files, so we do it like this
# by setting the CMAKE_CXX_FLAGS
if(NOT MINGW)
    execute_process(COMMAND ${CL_WX_CONFIG} --cxxflags OUTPUT_VARIABLE WX_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    execute_process(COMMAND sh ${CL_WX_CONFIG} --cxxflags OUTPUT_VARIABLE WX_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WX_CXX_FLAGS}")
message("${WX_CXX_FLAGS}")

FILE(GLOB CODELITECLIENT_SRCS "*.cpp")

# and finally prepare list of includes directories
include_directories("${CL_SRC_ROOT}/sdk/wxsqlite3/include" 
                    "${CL_SRC_ROOT}/CodeLite"
                    "${CL_SRC_ROOT}/sdk/codelite_indexer/network")

# Define the output
add_executable(codelite-client ${CODELITECLIENT_SRCS})
target_compile_definitions(codelite-client PRIVATE wxUSE_GUI=1)
if(UNIX)
    set(UTIL_LIB "${UTIL_LIB} -lutil")
endif(UNIX)
target_link_libraries(codelite-client
                      codelitelib
                      ${LINKER_OPTIONS} 
                      ${wxWidgets_LIBRARIES} 
                      ${UTIL_LIB}
                      )
                    