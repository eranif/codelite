# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(codelite-terminal)

if ( UNIX OR MINGW AND NOT APPLE )
    find_package(wxWidgets COMPONENTS std stc aui REQUIRED)
else ( UNIX AND NOT APPLE )
    find_package(wxWidgets COMPONENTS std stc REQUIRED)
endif ( UNIX OR MINGW AND NOT APPLE )

# we need wxWidgets flags to be set only for the c++ files, so we do it like this
# by setting the CMAKE_CXX_FLAGS
if ( NOT MINGW )
    execute_process(COMMAND ${CL_WX_CONFIG} --debug=no --cxxflags OUTPUT_VARIABLE WX_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
else ()
    execute_process(COMMAND sh ${CL_WX_CONFIG} --debug=no --cxxflags OUTPUT_VARIABLE WX_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
endif ()
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WX_CXX_FLAGS}")

add_definitions(-DNDEBUG)

# prepare list of files
FILE(GLOB SRCS "*.cpp")

# Define the output
set (RES_FILE "")
if (MINGW)
    set (RES_FILE "win_resources.rc")
endif()
add_executable(codelite-terminal ${SRCS} ${RES_FILE})

set ( UTIL_LIB "" )
if ( UNIX )
    set( UTIL_LIB "-lutil")
endif ( UNIX )

target_link_libraries(codelite-terminal ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES} ${UTIL_LIB} )

if(APPLE)
    install(TARGETS codelite-terminal 
            DESTINATION 
            ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS)
    # @executable_path/../../../
    CL_INSTALL_NAME_TOOL_EX("libwx_" "@executable_path/../../../" 
                ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS/codelite-terminal)
else()
    install(TARGETS codelite-terminal DESTINATION ${CL_PREFIX}/bin)
endif()
add_dependencies(codelite-terminal plugin)
