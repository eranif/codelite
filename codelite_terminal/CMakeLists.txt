# define minimum cmake version
cmake_minimum_required(VERSION 3.0)
 
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

include_directories(${CL_SRC_ROOT}/Interfaces)
include_directories(${CL_SRC_ROOT}/CodeLite)
include_directories(${CL_SRC_ROOT}/Plugin)
add_definitions(-DWXUSINGDLL_WXSQLITE3)
add_definitions(-DWXUSINGDLL_CL)
add_definitions(-DWXUSINGDLL_SDK)

target_link_libraries(codelite-terminal ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES} ${UTIL_LIB} libcodelite plugin)

if(APPLE)
    install(TARGETS codelite-terminal 
            DESTINATION 
            ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS)
    CL_INSTALL_NAME_TOOL_STD(
        ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS/codelite-terminal)
        
    install(TARGETS plugin DESTINATION 
            ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS)
    CL_INSTALL_NAME_TOOL_STD(
        ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS/libplugin.dylib)

    install(TARGETS libcodelite DESTINATION 
            ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS)
    CL_INSTALL_NAME_TOOL_STD(
        ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS/liblibcodelite.dylib)

    install(TARGETS wxsqlite3 DESTINATION 
            ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS)
    CL_INSTALL_NAME_TOOL_STD(
        ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS/libwxsqlite3.dylib)

    # @executable_path/../../../
    CL_INSTALL_NAME_TOOL_EX("libwx" "@executable_path" 
                ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS/codelite-terminal)
    CL_INSTALL_NAME_TOOL_EX("libwxsqlite3" "@executable_path" 
                ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS/codelite-terminal)
    CL_INSTALL_NAME_TOOL_EX("liblibcodelite" "@executable_path" 
                ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS/codelite-terminal)
    CL_INSTALL_NAME_TOOL_EX("libplugin" "@executable_path" 
                ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS/codelite-terminal)
else()
    install(TARGETS codelite-terminal DESTINATION ${CL_PREFIX}/bin)
endif()
add_dependencies(codelite-terminal plugin)
