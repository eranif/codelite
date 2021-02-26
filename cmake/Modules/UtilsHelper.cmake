cmake_minimum_required(VERSION 3.0)

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
endfunction()
