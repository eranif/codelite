cmake_minimum_required(VERSION 3.0)

macro(CL_OSX_COPY_BREW_LIB lib_full_path destination_folder)
    # if(APPLE) # when using brew, the libraries are symlinked to ../Cellar/... # we need to fix this by copying the
    # real file and then creating a symbolic link # in our directory
    #
    # # what we want to do here is: # alter the symlink libfoo.dylib -> ../Cellar/lib/libfoo.1.X.dylib # into
    # libfoo.dylib -> real-libfoo.dylib execute_process(COMMAND basename ${lib_full_path} OUTPUT_VARIABLE __lib_basename
    # OUTPUT_STRIP_TRAILING_WHITESPACE) execute_process(COMMAND cp -L ${lib_full_path}
    # ${destination_folder}/real-${__lib_basename}) execute_process(COMMAND /bin/sh -c "cd ${destination_folder} && ln
    # -sf real-${__lib_basename} ${__lib_basename}") endif()
endmacro()

macro(CL_OSX_FIND_BREW_LIB __lib_name LIB_OUTPUT_VARIABLE)
    if(APPLE)
        unset(${LIB_OUTPUT_VARIABLE} CACHE)
        find_library(
            ${LIB_OUTPUT_VARIABLE}
            NAMES ${__lib_name}
            HINTS ${BREW_PREFIX}/lib
            PATH_SUFFIXES lib)
    endif()
endmacro()

macro(CL_OSX_FIND_BREW_HEADER __header_name HEADER_OUTPUT_VARIABLE)
    if(APPLE)
        unset(${HEADER_OUTPUT_VARIABLE} CACHE)
        find_path(
            ${HEADER_OUTPUT_VARIABLE}
            NAMES ${__header_name}
            HINTS ${BREW_PREFIX}/include
            PATH_SUFFIXES include)
    endif()
endmacro()

function(copy_extra_homebrew_libs)
    # if these files also exists under /opt/homebrew -> use this path instead
    execute_process(COMMAND cp -L "/opt/homebrew/opt/libssh/lib/libssh.4.dylib"
                            ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS ERROR_QUIET OUTPUT_QUIET)
    execute_process(COMMAND cp -L "/opt/homebrew/opt/hunspell/lib/libhunspell-1.7.0.dylib"
                            ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS ERROR_QUIET OUTPUT_QUIET)
    copy_lib_deps("/opt/homebrew/opt/libssh/lib/libssh.4.dylib" "homebrew")
endfunction()

function(copy_lib_deps LIB_FULLPATH DEPS_PATTERN)
    execute_process(
        COMMAND sh -c "otool -L ${LIB_FULLPATH} |grep ${DEPS_PATTERN}|grep -v ${LIB_FULLPATH} |cut -d '(' -f1"
        OUTPUT_VARIABLE _HOMEBREW_DEPS
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REPLACE " " "" _HOMEBREW_DEPS "${_HOMEBREW_DEPS}")
    string(REPLACE "\t" "" _HOMEBREW_DEPS "${_HOMEBREW_DEPS}")
    string(REPLACE "\n" ";" _HOMEBREW_DEPS "${_HOMEBREW_DEPS}")
    foreach(DLL ${_HOMEBREW_DEPS})
        message(STATUS "Copying ${DLL} into bundle/MacOS")
        execute_process(COMMAND cp -L ${DLL} ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS)
        if(NOT "${DLL}" STREQUAL "")
            copy_lib_deps("${DLL}" "homebrew")
        endif()
    endforeach()
endfunction()

macro(_FIND_WX_LIBRARIES)
    if(APPLE)
        if(NOT _WX_LIBS_DIR)
            execute_process(
                COMMAND wx-config --libs
                OUTPUT_VARIABLE WX_LIBSOUTPUT
                OUTPUT_STRIP_TRAILING_WHITESPACE)
            if(${WX_LIBSOUTPUT} MATCHES "^-L.+") # In recent, multi-architecture, distro versions it'll start with
                                                 # -L/foo/bar
                string(REGEX REPLACE "^-L([^ ;]+).*" "\\1" _WX_LIBS_DIR ${WX_LIBSOUTPUT})
                message("-- _WX_LIBS_DIR is set to ${_WX_LIBS_DIR}")
            endif()
            set(minusElPos -1)
            string(FIND ${WX_LIBSOUTPUT} "-l" minusElPos REVERSE)
            math(EXPR minusElPos "${minusElPos}+2") # Skip the -l
            string(SUBSTRING ${WX_LIBSOUTPUT} ${minusElPos} -1 _WX_LIB_NAME)
            message("-- _WX_LIB_NAME is set to ${_WX_LIB_NAME}")
        endif()
    endif()
endmacro()

# execute the macro once this file is included
_find_wx_libraries()

# --------------------------------------------------------------------
# Install a file into /usr/lib/codelite/share or codelite.app/Contents/SharedSupport
# --------------------------------------------------------------------
macro(CL_INSTALL_FILE_SHARED _filename_)
    install(FILES ${_filename_} DESTINATION ${CL_RESOURCES_DIR})
endmacro()

# ------------------------------------
# install a plugin
# ------------------------------------
macro(CL_INSTALL_PLUGIN _target_)
    if(APPLE)
        install(TARGETS ${_target_} DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/plugins)
    else()
        install(TARGETS ${_target_} DESTINATION ${PLUGINS_DIR})
    endif()
endmacro()

# ------------------------------------
# install an executable
# ------------------------------------
macro(CL_INSTALL_EXECUTABLE _target_)
    if(APPLE)
        install(TARGETS ${_target_} DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/)
    else()
        set(EXE_PERM
            OWNER_EXECUTE
            OWNER_WRITE
            OWNER_READ
            GROUP_EXECUTE
            GROUP_READ
            WORLD_EXECUTE
            WORLD_READ)

        install(
            TARGETS ${_target_}
            DESTINATION ${CL_INSTALL_BIN}
            PERMISSIONS ${EXE_PERM})
    endif()
endmacro()

# ------------------------------------
# install a debugger shared library
# ------------------------------------
macro(CL_INSTALL_DEBUGGER _target_)
    if(APPLE)
        install(TARGETS ${_target_} DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/debuggers)
    elseif(MINGW)
        install(TARGETS ${_target_} DESTINATION ${CL_INSTALL_BIN}/debuggers)
    else()
        install(TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGINS_DIR}/debuggers)
    endif()
endmacro()

# -------------------------------------------------
# Prepare a skeleton bundle for CodeLite
# -------------------------------------------------
macro(OSX_MAKE_BUNDLE_DIRECTORY)
    if(APPLE)
        if(NOT CL_SRC_ROOT)
            set(CL_SRC_ROOT "/Users/eran/devl/codelite")
        endif()

        message("-- Removing old bundle folder...")
        file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/codelite.app)
        message("-- Removing old bundle folder...done")
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codelite.app)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codelite.app/Contents)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codelite.app/Contents/Resources)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/plugins)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/resources)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/debuggers)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/config)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/config/cppcheck)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/dics)

        copy_extra_homebrew_libs()

        file(COPY ${CL_SRC_ROOT}/svgs/logo/osx/icon.icns
             DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/Resources)

        file(COPY ${CL_SRC_ROOT}/Runtime/cl_workspace.icns
             DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/Resources)

        # Copy Info.plist
        file(COPY ${CL_SRC_ROOT}/Runtime/Info.plist DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents)

        # Copy external libraries into the bundle folder
        _find_wx_libraries()

        set(WX_DYLIB ${_WX_LIBS_DIR}/lib${_WX_LIB_NAME}.dylib)
        message(STATUS "wxWidgets lib is: ${WX_DYLIB}")
        copy_lib_deps(${WX_DYLIB} "homebrew")

        file(GLOB WXLIBS ${_WX_LIBS_DIR}/lib${_WX_LIB_NAME}*.dylib)
        foreach(WXLIB ${WXLIBS})
            file(COPY ${WXLIB} DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS)
        endforeach()
        # Copy Terminal.app launcher script
        file(
            COPY ${CL_SRC_ROOT}/Runtime/osx-terminal.sh
            DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS
            FILE_PERMISSIONS ${EXE_PERM})

        # codelite-clang-format
        file(COPY ${CL_SRC_ROOT}/tools/macOS/clang-format DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/)

        # folders
        install(
            DIRECTORY ${CL_SRC_ROOT}/Runtime/plugins/resources
            DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/
            USE_SOURCE_PERMISSIONS
            PATTERN ".svn" EXCLUDE
            PATTERN ".git" EXCLUDE)

        install(
            DIRECTORY ${CL_SRC_ROOT}/Runtime/images ${CL_SRC_ROOT}/Runtime/gdb_printers ${CL_SRC_ROOT}/Runtime/src/
                      ${CL_SRC_ROOT}/Runtime/lexers ${CL_SRC_ROOT}/Runtime/templates ${CL_SRC_ROOT}/Runtime/rc
            DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/
            USE_SOURCE_PERMISSIONS
            PATTERN ".svn" EXCLUDE
            PATTERN ".git" EXCLUDE)

        install(FILES ${CL_SRC_ROOT}/Runtime/config/codelite.layout.default
                DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/codelite.layout)

        install(FILES ${CL_SRC_ROOT}/Runtime/config/codelite.layout.default
                DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/codelite.layout)

        install(
            DIRECTORY ${CL_SRC_ROOT}/sdk/codelite_cppcheck/cfg/
            DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/config/cppcheck/
            FILES_MATCHING
            PATTERN "*.cfg")

        install(
            DIRECTORY ${CL_SRC_ROOT}/Runtime/
            DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/
            FILES_MATCHING
            PATTERN "*.zip")

        install(
            FILES ${CL_SRC_ROOT}/Runtime/config/build_settings.xml.default.mac
            DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/config/
            RENAME build_settings.xml.default)

        install(FILES ${CL_SRC_ROOT}/Runtime/config/plugins.xml.default
                DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/config/)
        install(FILES ${CL_SRC_ROOT}/LICENSE DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport)

        install(
            FILES ${CL_SRC_ROOT}/Runtime/config/codelite.xml.default.mac
            DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/config
            RENAME codelite.xml.default)

        install(
            FILES ${CL_SRC_ROOT}/Runtime/config/debuggers.xml.mac
            DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/config
            RENAME debuggers.xml.default)
    endif()
endmacro()
