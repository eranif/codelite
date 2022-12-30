project(codelite-terminal)

if ( UNIX OR MINGW AND NOT APPLE )

else ( UNIX AND NOT APPLE )

endif ( UNIX OR MINGW AND NOT APPLE )

include(${wxWidgets_USE_FILE})

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

if (MINGW)
    set(LINKER_OPTIONS "-mwindows")
endif()

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
    install(TARGETS codelite-terminal DESTINATION ${CL_INSTALL_BIN})
endif()
add_dependencies(codelite-terminal plugin)
