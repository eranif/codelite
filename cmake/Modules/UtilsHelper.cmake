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

# Determine if we are running on Windows using MSYS2 shell or using MinGW tools (but not using MSYS)
if (UNIX)
    execute_process(COMMAND /bin/sh "-c" "uname -s|grep MSYS|cut -d_ -f1"
                    OUTPUT_VARIABLE _OS_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(FIND ${_OS_NAME} "MSYS" POS)
    if (POS GREATER -1)
        set(MSYS_SHELL 1)
        set(MSW 1)
    endif()
elseif(WIN32)
    set(WIN_CMD_SHELL 1)
    set(MSW 1)
    execute_process(COMMAND sh "-c" "uname -s"
                    OUTPUT_VARIABLE _OS_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(FATAL_ERROR "${_OS_NAME}")
endif()
