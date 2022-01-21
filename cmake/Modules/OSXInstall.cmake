cmake_minimum_required(VERSION 3.0)

if(APPLE)
    set( PLUGINS_DIR ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/plugins)
else()
    set( PLUGINS_DIR "${CL_PREFIX}/${CL_INSTALL_LIBDIR}/codelite")
endif()

macro(CL_INSTALL_NAME_TOOL _findwhat_ _binary_)
    if(APPLE)
        install(CODE 
            "
            execute_process(COMMAND /bin/sh -c \"otool -L ${_binary_} | grep ${_findwhat_} |grep -v executable_path \"
                             OUTPUT_VARIABLE RESULT1
                             OUTPUT_STRIP_TRAILING_WHITESPACE
                             )
            if (NOT \"\${RESULT1}\" STREQUAL \"\")
                set(POS -1)
                string(FIND \${RESULT1} \"(\" POS)
                string(SUBSTRING \${RESULT1} 0 \${POS} SONAME)
                string(STRIP \${SONAME} SONAME)
                execute_process(COMMAND basename \${SONAME} OUTPUT_VARIABLE SOBASENAME OUTPUT_STRIP_TRAILING_WHITESPACE)
                execute_process(COMMAND install_name_tool -change \${SONAME} @executable_path/\${SOBASENAME} ${_binary_})
            endif()
            
            ")
    endif()
endmacro()

macro(CL_INSTALL_NAME_TOOL_EX _findwhat_ _replacewith_ _binary_)
    if(APPLE)
        install(CODE 
            "
            execute_process(COMMAND /bin/sh -c \"otool -L ${_binary_} | grep ${_findwhat_} |grep -v executable_path \"
                             OUTPUT_VARIABLE RESULT1
                             OUTPUT_STRIP_TRAILING_WHITESPACE
                             )
            if (NOT \"\${RESULT1}\" STREQUAL \"\")
                set(POS -1)
                string(FIND \${RESULT1} \"(\" POS)
                string(SUBSTRING \${RESULT1} 0 \${POS} SONAME)
                string(STRIP \${SONAME} SONAME)
                execute_process(COMMAND basename \${SONAME} OUTPUT_VARIABLE SOBASENAME OUTPUT_STRIP_TRAILING_WHITESPACE)
                execute_process(COMMAND install_name_tool -change \${SONAME} ${_replacewith_}/\${SOBASENAME} ${_binary_})
            endif()
            
            ")
    endif()
endmacro()

#------------------------------------------------------------------------------------
# A useful macro that accepts the string 
# the search string and runs install_name_tool
# to set it to the @executable_path
#------------------------------------------------------------------------------------
macro(CL_INSTALL_NAME_TOOL_STD _binary_)
    if(APPLE)
        CL_INSTALL_NAME_TOOL("libwx_" ${_binary_})
        CL_INSTALL_NAME_TOOL("libcodelite" ${_binary_})
        CL_INSTALL_NAME_TOOL("libwxsqlite" ${_binary_})
        CL_INSTALL_NAME_TOOL("libplugin" ${_binary_})
        CL_INSTALL_NAME_TOOL("libwxshapeframework" ${_binary_})
        CL_INSTALL_NAME_TOOL("libhunspell" ${_binary_})
        CL_INSTALL_NAME_TOOL("libdatabaselayersqlite" ${_binary_})
        CL_INSTALL_NAME_TOOL("libdatabaselayersqlite" ${_binary_})
        CL_INSTALL_NAME_TOOL("libclang" ${_binary_})
        CL_INSTALL_NAME_TOOL("liblldb" ${_binary_})
    endif()
endmacro()

macro(_FIND_WX_LIBRARIES)
    if(APPLE)
        if(NOT _WX_LIBS_DIR)
            execute_process(COMMAND wx-config --libs OUTPUT_VARIABLE WX_LIBSOUTPUT OUTPUT_STRIP_TRAILING_WHITESPACE)
            if (${WX_LIBSOUTPUT} MATCHES "^-L.+") # In recent, multi-architecture, distro versions it'll start with -L/foo/bar
                string(REGEX REPLACE "^-L([^ ;]+).*" "\\1" _WX_LIBS_DIR ${WX_LIBSOUTPUT})
                message("-- _WX_LIBS_DIR is set to ${_WX_LIBS_DIR}")
            endif()
            set(minusElPos -1)
            string(FIND ${WX_LIBSOUTPUT} "-l" minusElPos REVERSE)
            MATH(EXPR minusElPos "${minusElPos}+2") # Skip the -l
            string(SUBSTRING ${WX_LIBSOUTPUT} ${minusElPos} -1 _WX_LIB_NAME)
            message("-- _WX_LIB_NAME is set to ${_WX_LIB_NAME}")
        endif()
    endif()
endmacro()

# execute the macro once this file is included
_FIND_WX_LIBRARIES()

#--------------------------------------------------------------------
# Install a file into 
# /usr/lib/codelite/share or codelite.app/Contents/SharedSupport
#--------------------------------------------------------------------
macro(CL_INSTALL_FILE_SHARED _filename_)
    if(APPLE)
        install(FILES ${_filename_} DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport)
    else()
        install(FILES ${_filename_} DESTINATION ${CL_PREFIX}/share/codelite)
    endif()
endmacro()

#------------------------------------
# install a plugin
#------------------------------------
macro(CL_INSTALL_PLUGIN _target_)
    if(APPLE)
        install(TARGETS ${_target_} DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/plugins)
        CL_INSTALL_NAME_TOOL_STD(${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/plugins/${_target_}.dylib)
    else()
        install(TARGETS ${_target_} DESTINATION ${PLUGINS_DIR})
    endif()
endmacro()

#------------------------------------
# install a library (shared object, but not a plugin)
#------------------------------------
macro(CL_INSTALL_LIBRARY _target_)
    if(APPLE)
        set(__LIBNAME ${_target_})
        string(FIND ${__LIBNAME} "lib" LIBPREFIX_INDEX)
        if ( LIBPREFIX_INDEX GREATER -1 )
            set(__LIBNAME "lib${LIBPREFIX_INDEX}")
        endif()
        install(TARGETS ${_target_} DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/)
        CL_INSTALL_NAME_TOOL_STD(${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/${__LIBNAME})
    else()
        install(TARGETS ${_target_} DESTINATION ${PLUGINS_DIR})
    endif()
endmacro()

#------------------------------------
# install an executable
#------------------------------------
macro(CL_INSTALL_EXECUTABLE _target_)
    if(APPLE)
        install(TARGETS ${_target_} DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/)
        CL_INSTALL_NAME_TOOL_STD(${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/${_target_})
    else()
        # On non OSX, we place the non plugins next to the plugins
        set (EXE_PERM OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
        
        # On non OSX, we place the non plugins next to the plugins
        install(TARGETS ${_target_} DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
    endif()
endmacro()

#------------------------------------
# install a debugger shared library
#------------------------------------
macro(CL_INSTALL_DEBUGGER _target_)
    if(APPLE)
        install(TARGETS ${_target_} DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/debuggers)
        # now that the plugin is installed, run install_name_tool
        CL_INSTALL_NAME_TOOL_STD(${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/debuggers/${_target_}.dylib)
    else()
        install(TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGINS_DIR}/debuggers)
    endif()
endmacro()

#-------------------------------------------------
# Prepare a skeleton bundle for CodeLite
#-------------------------------------------------
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
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/Resources)
        
        file(COPY ${CL_SRC_ROOT}/codelite_terminal/icon.icns 
            DESTINATION 
            ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/Resources)

        file(COPY ${CL_SRC_ROOT}/bitmaps-light/osx/icon.icns 
            DESTINATION 
            ${CMAKE_BINARY_DIR}/codelite.app/Contents/Resources)
            
        file(COPY ${CL_SRC_ROOT}/Runtime/cl_workspace.icns 
            DESTINATION 
            ${CMAKE_BINARY_DIR}/codelite.app/Contents/Resources)

        # Copy Info.plist
        file(COPY ${CL_SRC_ROOT}/Runtime/Info.plist DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents)
        file(COPY ${CL_SRC_ROOT}/codelite_terminal/Info.plist 
            DESTINATION 
            ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents)
        
        ## Copy external libraries into the bundle folder
        _FIND_WX_LIBRARIES()
        
        file(GLOB WXLIBS ${_WX_LIBS_DIR}/lib${_WX_LIB_NAME}*.dylib)
        foreach(WXLIB ${WXLIBS})
            file(COPY ${WXLIB} DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS)
        endforeach()

        foreach(WXLIB ${WXLIBS})
            file(COPY ${WXLIB} DESTINATION 
                ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-terminal.app/Contents/MacOS)
        endforeach()

        ## Copy Terminal.app launcher script
        file(COPY ${CL_SRC_ROOT}/Runtime/osx-terminal.sh 
             DESTINATION 
             ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS 
             FILE_PERMISSIONS ${EXE_PERM})

        ## Copy pre-built binaries
        file(COPY ${CL_SRC_ROOT}/Runtime/debugserver 
             DESTINATION 
             ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS)
        
        ## Copy and fix libclang.dylib / liblldb
        message(STATUS "Copying ${CL_SRC_ROOT}/sdk/lldb/osx/lib/liblldb.10.0.0svn.dylib -> ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/")
        file(COPY ${CL_SRC_ROOT}/sdk/lldb/osx/lib/liblldb.10.0.0svn.dylib
             DESTINATION 
             ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/
             )

        ## codelite-clang-format
        file(COPY ${CL_SRC_ROOT}/tools/macOS/clang-format 
             DESTINATION 
                  ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/)

        file(COPY ${CL_SRC_ROOT}/tools/macOS/clangd 
             DESTINATION 
                  ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/)
        
        ## folders
        install(
            DIRECTORY ${CL_SRC_ROOT}/Runtime/plugins/resources
            DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/
            USE_SOURCE_PERMISSIONS
            PATTERN ".svn" EXCLUDE
            PATTERN ".git" EXCLUDE
        ) 
        
        install(
            DIRECTORY ${CL_SRC_ROOT}/Runtime/images 
                      ${CL_SRC_ROOT}/Runtime/gdb_printers 
                      ${CL_SRC_ROOT}/Runtime/src/ 
                      ${CL_SRC_ROOT}/Runtime/lexers 
                      ${CL_SRC_ROOT}/Runtime/templates 
                      ${CL_SRC_ROOT}/Runtime/rc
            DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/
            USE_SOURCE_PERMISSIONS
            PATTERN ".svn" EXCLUDE
            PATTERN ".git" EXCLUDE
        )
                    
        install(FILES ${CL_SRC_ROOT}/Runtime/config/codelite.layout.default 
                DESTINATION 
                    ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/codelite.layout)
                    
        install(FILES ${CL_SRC_ROOT}/Runtime/config/codelite.layout.default 
                DESTINATION 
                    ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/codelite.layout)

        install(DIRECTORY ${CL_SRC_ROOT}/sdk/codelite_cppcheck/cfg/
                DESTINATION 
                    ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/config/cppcheck/
                FILES_MATCHING PATTERN "*.cfg"
                )
        
        install(DIRECTORY ${CL_SRC_ROOT}/Runtime/ DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/
                FILES_MATCHING PATTERN "*.zip")
        
        install(FILES ${CL_SRC_ROOT}/Runtime/config/build_settings.xml.default.mac 
                DESTINATION 
                    ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/config/
                RENAME build_settings.xml.default)

        install(FILES ${CL_SRC_ROOT}/Runtime/config/plugins.xml.default
                DESTINATION 
                    ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/config/)
        install(FILES ${CL_SRC_ROOT}/LICENSE DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport)
        
        install(FILES ${CL_SRC_ROOT}/Runtime/config/codelite.xml.default.mac 
                DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/config
                RENAME codelite.xml.default)
                
        install(FILES ${CL_SRC_ROOT}/Runtime/config/debuggers.xml.mac 
                DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/config
                RENAME debuggers.xml.default)
                
#        execute_process(COMMAND 
#                        install_name_tool -change @rpath/libclang.dylib @executable_path/../MacOS/libclang.dylib 
#                        ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite)
    endif()
endmacro()




