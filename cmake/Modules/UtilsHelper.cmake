cmake_minimum_required(VERSION 3.0)

#------------------------------------
# install script
#------------------------------------
macro(codelite_install_script _script_)
    set (EXE_PERM OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
    if(APPLE)
        install(FILES ${_script_} DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/ PERMISSIONS ${EXE_PERM})
    else()
        # On non OSX, we place the non plugins next to the plugins
        install(FILES ${_script_} DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
    endif()
endmacro()

function(get_distro_name DISTRO_NAME)
    execute_process(COMMAND /bin/bash "-c" "cat /etc/os-release |grep ^ID=|cut -d = -f 2"
                    OUTPUT_VARIABLE _DISTRO_ID OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND /bin/bash "-c" "cat /etc/os-release |grep VERSION_ID=|cut -d = -f 2"
                    OUTPUT_VARIABLE _VERSION_ID OUTPUT_STRIP_TRAILING_WHITESPACE)
    
    # clean the output
    string (REPLACE "\"" "" _DISTRO_ID "${_DISTRO_ID}")
    string (REPLACE "\"" "" _VERSION_ID "${_VERSION_ID}")
    string (REPLACE "." "" _DISTRO_ID "${_DISTRO_ID}")
    string (REPLACE "." "" _VERSION_ID "${_VERSION_ID}")
    set(${DISTRO_NAME} "")
    string(FIND ${_DISTRO_ID} "fedora" POS)
    if (POS GREATER -1)
        set (${DISTRO_NAME} "fedora_${_VERSION_ID}" PARENT_SCOPE)
    endif()
    
    string(FIND ${_DISTRO_ID} "ubuntu" POS)
    if (POS GREATER -1)
        set (${DISTRO_NAME} "${_DISTRO_ID}_${_VERSION_ID}" PARENT_SCOPE)
    endif()
    
    string(FIND ${_DISTRO_ID} "debian" POS)
    if (POS GREATER -1)
        set (${DISTRO_NAME} "${_DISTRO_ID}_${_VERSION_ID}" PARENT_SCOPE)
    endif()
endfunction()

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
    <memory>
    )
macro(codelite_add_exported_pch _TARGET_)
    target_precompile_headers(
        ${_TARGET_} 
    PUBLIC
        ${PCH_HEADERS_LIST}
    )
endmacro()

macro(codelite_add_pch _TARGET_)
    target_precompile_headers(
        ${_TARGET_} 
    PRIVATE
        ${PCH_HEADERS_LIST}
    )
endmacro()

# Determine if we are running on Windows using MSYS2 shell or using MinGW tools (but not using MSYS)
if (UNIX)
    execute_process(COMMAND /bin/sh "-c" "uname -s|grep MSYS|cut -d_ -f1"
                    OUTPUT_VARIABLE _OS_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(FIND "${_OS_NAME}" "MSYS" POS)
    if (POS GREATER -1)
        set(MSYS_SHELL 1)
        set(MSW 1)
    endif()
elseif(WIN32)
    execute_process(COMMAND sh "-c" "uname -o|grep -E 'Msys|Cygwin'|cut -d_ -f1"
                    OUTPUT_VARIABLE _OS_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)

    if ("${_OS_NAME}" MATCHES "Msys|Cygwin")
        set(MSYS_SHELL 1)
        set(MSW 1)
    else()
        set(WIN_CMD_SHELL 1)
        set(MSW 1)
        execute_process(COMMAND sh "-c" "uname -s"
                        OUTPUT_VARIABLE _OS_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)
        message(FATAL_ERROR "${_OS_NAME}")
    endif()
endif() # UNIX -> WIN32
