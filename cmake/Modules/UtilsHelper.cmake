cmake_minimum_required(VERSION 3.5)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

include(OSXInstall)

# ------------------------------------
# install script
# ------------------------------------
macro(codelite_install_script _script_)
    set(EXE_PERM
        OWNER_EXECUTE
        OWNER_WRITE
        OWNER_READ
        GROUP_EXECUTE
        GROUP_READ
        WORLD_EXECUTE
        WORLD_READ)
    install(
        FILES ${_script_}
        DESTINATION ${CL_INSTALL_BIN}
        PERMISSIONS ${EXE_PERM})
endmacro()

# ------------------------------------
# install an executable
# ------------------------------------
macro(codelite_install_executable TARGET)
    if(NOT APPLE)
        set(EXE_PERM
            OWNER_EXECUTE
            OWNER_WRITE
            OWNER_READ
            GROUP_EXECUTE
            GROUP_READ
            WORLD_EXECUTE
            WORLD_READ)

        install(
            TARGETS ${TARGET}
            DESTINATION ${CL_INSTALL_BIN}
            PERMISSIONS ${EXE_PERM})
    endif()
endmacro()

# ------------------------------------
# install an executable
# ------------------------------------
macro(codelite_install_svgs)
    install(
        DIRECTORY ${CL_SRC_ROOT}/svgs/light-theme ${CL_SRC_ROOT}/svgs/dark-theme
        DESTINATION ${CL_RESOURCES_DIR}/svgs
        FILES_MATCHING
        PATTERN "*.svg")
endmacro()

function(get_distro_name DISTRO_NAME)
    execute_process(
        COMMAND /bin/bash "-c" "cat /etc/os-release |grep ^ID=|cut -d = -f 2"
        OUTPUT_VARIABLE _DISTRO_ID
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(
        COMMAND /bin/bash "-c" "cat /etc/os-release |grep VERSION_ID=|cut -d = -f 2"
        OUTPUT_VARIABLE _VERSION_ID
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    # clean the output
    string(REPLACE "\"" "" _DISTRO_ID "${_DISTRO_ID}")
    string(REPLACE "\"" "" _VERSION_ID "${_VERSION_ID}")
    string(REPLACE "." "" _DISTRO_ID "${_DISTRO_ID}")
    string(REPLACE "." "" _VERSION_ID "${_VERSION_ID}")
    set(${DISTRO_NAME} "")
    string(FIND ${_DISTRO_ID} "fedora" POS)
    if(POS GREATER -1)
        set(${DISTRO_NAME}
            "fedora_${_VERSION_ID}"
            PARENT_SCOPE)
    endif()

    string(FIND ${_DISTRO_ID} "ubuntu" POS)
    if(POS GREATER -1)
        set(${DISTRO_NAME}
            "${_DISTRO_ID}_${_VERSION_ID}"
            PARENT_SCOPE)
    endif()

    string(FIND ${_DISTRO_ID} "debian" POS)
    if(POS GREATER -1)
        set(${DISTRO_NAME}
            "${_DISTRO_ID}_${_VERSION_ID}"
            PARENT_SCOPE)
    endif()
endfunction()

if(MINGW)
    macro(msys_list_deps __NAME__ OUT_LIST)
        execute_process(
            COMMAND sh -c "/usr/bin/cygcheck ${__NAME__} |grep clang64|grep -v -w Found|grep -v ${__NAME__}"
            OUTPUT_VARIABLE __dep_list
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        string(REPLACE "\n" ";" ${OUT_LIST} "${__dep_list}")
        string(REPLACE " " "" ${OUT_LIST} "${${OUT_LIST}}")
        string(REPLACE "\\" "/" ${OUT_LIST} "${${OUT_LIST}}")
    endmacro()

    macro(msys_install_clang64_tool tool_name install_dir)
        msys_list_deps(${MSYS2_BASE}/clang64/bin/${tool_name} CLANGD_DEPS)
        # install the tool itself
        install(FILES "${MSYS2_BASE}/clang64/bin/${tool_name}" DESTINATION ${install_dir})

        # and all its dlls
        foreach(DLL ${CLANGD_DEPS})
            install(FILES "${DLL}" DESTINATION ${install_dir})
        endforeach()
    endmacro()
endif()

set(PCH_HEADERS_LIST
    <wx/wxprec.h>
    <wx/app.h>
    <wx/artprov.h>
    <wx/bitmap.h>
    <wx/button.h>
    <wx/checklst.h>
    <wx/choice.h>
    <wx/colour.h>
    <wx/dialog.h>
    <wx/ffile.h>
    <wx/filedlg.h>
    <wx/font.h>
    <wx/frame.h>
    <wx/gdicmn.h>
    <wx/icon.h>
    <wx/image.h>
    <wx/imaggif.h>
    <wx/imaglist.h>
    <wx/intl.h>
    <wx/log.h>
    <wx/menu.h>
    <wx/msgdlg.h>
    <wx/notebook.h>
    <wx/panel.h>
    <wx/scrolwin.h>
    <wx/settings.h>
    <wx/sizer.h>
    <wx/splitter.h>
    <wx/statbox.h>
    <wx/stattext.h>
    <wx/statusbr.h>
    <wx/string.h>
    <wx/textctrl.h>
    <wx/tooltip.h>
    <wx/treectrl.h>
    <wx/wupdlock.h>
    <wx/xrc/xmlres.h>
    <set>
    <unordered_set>
    <unordered_map>
    <vector>
    <list>
    <map>
    <memory>)
macro(codelite_add_exported_pch _TARGET_)
    target_precompile_headers(${_TARGET_} PUBLIC ${PCH_HEADERS_LIST})
endmacro()

macro(codelite_add_pch _TARGET_)
    target_precompile_headers(${_TARGET_} PRIVATE ${PCH_HEADERS_LIST})
endmacro()

# ------------------------------------
# install a library (shared object, but not a plugin)
# ------------------------------------
macro(codelite_install_library_target TARGET)
    if(APPLE)
        install(TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/)
    elseif(MINGW)
        # under windows (MinGW) we install libraries under the "bin" folder
        install(TARGETS ${TARGET} DESTINATION "${CL_INSTALL_BIN}")
    else()
        # Under linux, we install libraries in the plugins directory
        install(TARGETS ${TARGET} DESTINATION ${PLUGINS_DIR})
    endif()
endmacro()

# Determine if we are running on Windows using MSYS2 shell or using MinGW tools (but not using MSYS)
if(UNIX)
    execute_process(
        COMMAND /bin/sh "-c" "uname -s|grep MSYS|cut -d_ -f1"
        OUTPUT_VARIABLE _OS_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(FIND "${_OS_NAME}" "MSYS" POS)
    if(POS GREATER -1)
        set(MSYS_SHELL 1)
        set(MSW 1)
    endif()
elseif(WIN32)
    execute_process(
        COMMAND sh "-c" "uname -o|grep -E 'Msys|Cygwin'|cut -d_ -f1"
        OUTPUT_VARIABLE _OS_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if("${_OS_NAME}" MATCHES "Msys|Cygwin")
        set(MSYS_SHELL 1)
        set(MSW 1)
    else()
        set(WIN_CMD_SHELL 1)
        set(MSW 1)
        execute_process(
            COMMAND sh "-c" "uname -s"
            OUTPUT_VARIABLE _OS_NAME
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        message(FATAL_ERROR "${_OS_NAME}")
    endif()
endif() # UNIX -> WIN32
