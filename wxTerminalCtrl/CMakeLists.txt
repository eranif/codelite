cmake_minimum_required(VERSION 3.16)
project(wxTerminal)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if(MINGW)
    if(NOT WXWIN)
        message(FATAL_ERROR "Missing -DWXWIN=<wxWidgets/root/install/dir>")
    endif()
    if(NOT WXCFG)
        set(WXCFG "clang_x64_dll/mswu")
    endif()

    message(STATUS "WXWIN is set to ${WXWIN}")
    message(STATUS "WXCFG is set to ${WXCFG}")

    execute_process(
        COMMAND wx-config --cmake --prefix=${WXWIN} --wxcfg=${WXCFG}
        OUTPUT_VARIABLE wxWidgets_USE_FILE
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "wxWidgets_USE_FILE is set to: ${wxWidgets_USE_FILE}")
else()
    find_package(
        wxWidgets
        COMPONENTS adv
                   aui
                   base
                   core
                   html
                   propgrid
                   xml
                   xrc
                   net
                   stc
                   ribbon
                   richtext
        REQUIRED)
endif()

include(${wxWidgets_USE_FILE})

# prepare list of files
file(GLOB SRCS "*.cpp")
# Define the output
set(RES_FILE "")
if(MINGW)
    set(RES_FILE "win_resources.rc")
endif()
add_executable(codelite-terminal ${SRCS} ${RES_FILE})

set(UTIL_LIB "")
if(UNIX)
    set(UTIL_LIB "-lutil")
endif(UNIX)

include_directories(${CL_SRC_ROOT}/Interfaces)
include_directories(${CL_SRC_ROOT}/CodeLite)
include_directories(${CL_SRC_ROOT}/Plugin)

if(MINGW)
    set(LINKER_OPTIONS "-mwindows")
endif()

add_definitions(-DWXMAKINGDLL_TERMINALCTRL=1)
target_link_libraries(
    codelite-terminal
    libcodelite
    plugin
    ${LINKER_OPTIONS}
    ${wxWidgets_LIBRARIES}
    ${UTIL_LIB})
codelite_install_executable(codelite-terminal)
