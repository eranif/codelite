


# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles.
project(codelite)

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.


# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )
include(UtilsHelper)

# Include paths
include_directories("${CL_SRC_ROOT}/Plugin" "${CL_SRC_ROOT}/sdk/wxsqlite3/include" "${CL_SRC_ROOT}/CodeLite" "${CL_SRC_ROOT}/PCH" "${CL_SRC_ROOT}/Interfaces")

if (NOT MINGW)
add_definitions(-fPIC)
endif (NOT MINGW)

if ( USE_PCH AND NOT MINGW )
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif ()

add_definitions(-DWXUSINGDLL_WXSQLITE3)
add_definitions(-DWXUSINGDLL_CL)
add_definitions(-DWXUSINGDLL_SDK)

if (MINGW)
# disables the part of urlmon.h that defines IsLoggingEnabled
add_definitions(-D_HITLOGGING_DEFINED=0)
endif (MINGW)

if ( USE_CLANG )
  add_definitions(-DHAS_LIBCLANG)
  include_directories(${CLANG_INCLUDE})
endif( USE_CLANG )

FILE(GLOB SRCS "*.cpp")

set (RES_FILE "")
# Define the output
if (MINGW)
    set(RES_FILES "code_parser.rc")
endif()
add_executable(codelite ${SRCS} ${RES_FILES})

set( ADDITIONAL_LIBRARIES "" )

if(MINGW)
    set_target_properties(codelite
                          PROPERTIES
                          OUTPUT_NAME codelite
                          ARCHIVE_OUTPUT_NAME execodelite)
endif()

if (UNIX)
    if ( IS_FREEBSD )
        set(ADDITIONAL_LIBRARIES "-lkvm")
    elseif ( IS_NETBSD )
        set(ADDTIONAL_LIBRARIES "-lutil")
    elseif ( UNIX AND NOT APPLE )
        set(ADDITIONAL_LIBRARIES "-ldl -lutil -lgobject-2.0")
    else ( )
        set(ADDITIONAL_LIBRARIES "-ldl")
    endif ( )
else (UNIX)
    ## Windows??
endif (UNIX)

if(GTK2_FOUND)
    target_link_libraries(codelite ${LINKER_OPTIONS} ${GTK2_LIBRARIES} ${wxWidgets_LIBRARIES} ${CLANG_LIBRARY} -L"${CL_LIBPATH}" libcodelite plugin ${ADDITIONAL_LIBRARIES})
elseif (GTK3_FOUND)
    target_link_libraries(codelite ${LINKER_OPTIONS} ${GTK3_LIBRARIES} ${wxWidgets_LIBRARIES} ${CLANG_LIBRARY} -L"${CL_LIBPATH}" libcodelite plugin ${ADDITIONAL_LIBRARIES})
else()
    target_link_libraries(codelite ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES} ${CLANG_LIBRARY} -L"${CL_LIBPATH}" libcodelite plugin ${ADDITIONAL_LIBRARIES})
endif()

codelite_add_pch(codelite)

##################################################################
## Install
##################################################################

## Copy binaries / scripts
set (EXE_PERM OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

if(NOT APPLE)
    install(FILES ${CL_SRC_ROOT}/codelite_open_helper.py DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
endif()

if(NOT APPLE)
    install(TARGETS codelite DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
    if (UNIX)
        install(FILES ${CL_SRC_ROOT}/Runtime/codelite_exec DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
        install(FILES ${CL_SRC_ROOT}/Runtime/codelite_fix_files DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
        install(FILES ${CL_SRC_ROOT}/Runtime/codelite_kill_children DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
        install(FILES ${CL_SRC_ROOT}/Runtime/codelite_xterm DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
    endif (UNIX)

    ## codelite icons
    install(FILES ${CL_SRC_ROOT}/Runtime/codelite-bitmaps-dark.zip DESTINATION ${CL_PREFIX}/share/codelite)
    install(FILES ${CL_SRC_ROOT}/Runtime/codelite-bitmaps-light.zip DESTINATION ${CL_PREFIX}/share/codelite)

    ## License file
    install(FILES ${CL_SRC_ROOT}/LICENSE DESTINATION ${CL_PREFIX}/share/codelite)

    ## Manual pages
    if(UNIX AND NOT APPLE)
        install(DIRECTORY ${CL_SRC_ROOT}/Runtime/man1 DESTINATION ${CL_PREFIX}/share/man)
    endif()

    ## Copy default configuration files
    install(FILES ${CL_SRC_ROOT}/Runtime/config/build_settings.xml.default
            DESTINATION ${CL_PREFIX}/share/codelite/config)
    install(FILES ${CL_SRC_ROOT}/Runtime/config/debuggers.xml.gtk DESTINATION
            ${CL_PREFIX}/share/codelite/config
            RENAME debuggers.xml.default)
    install(FILES ${CL_SRC_ROOT}/Runtime/config/codelite.xml.default.gtk
            DESTINATION ${CL_PREFIX}/share/codelite/config
            RENAME codelite.xml.default)

    ## Create application launcher, copy application icon to standard location
    if ( UNIX AND NOT APPLE )
        install(FILES ${CL_SRC_ROOT}/Runtime/codelite.desktop DESTINATION ${CL_PREFIX}/share/applications)
        ## Install icons

        ## 32x32
        install(FILES ${CL_SRC_ROOT}/bitmaps-light/32-codelite-logo@2x.png
                DESTINATION ${CL_PREFIX}/share/icons/hicolor/32x32@2x/apps
                RENAME codelite.png)

        install(FILES ${CL_SRC_ROOT}/bitmaps-light/32-codelite-logo.png
                DESTINATION ${CL_PREFIX}/share/icons/hicolor/32x32/apps
                RENAME codelite.png)
        ## 64x64
        install(FILES ${CL_SRC_ROOT}/bitmaps-light/64-codelite-logo@2x.png
                DESTINATION ${CL_PREFIX}/share/icons/hicolor/64x64@2x/apps
                RENAME codelite.png)

        install(FILES ${CL_SRC_ROOT}/bitmaps-light/64-codelite-logo.png
                DESTINATION ${CL_PREFIX}/share/icons/hicolor/64x64/apps
                RENAME codelite.png)

        ## 128x128
        install(FILES ${CL_SRC_ROOT}/bitmaps-light/128-codelite-logo@2x.png
                DESTINATION ${CL_PREFIX}/share/icons/hicolor/128x128@2x/apps
                RENAME codelite.png)

        install(FILES ${CL_SRC_ROOT}/bitmaps-light/128-codelite-logo.png
                DESTINATION ${CL_PREFIX}/share/icons/hicolor/128x128/apps
                RENAME codelite.png)
        ## 256x256
        install(FILES ${CL_SRC_ROOT}/bitmaps-light/256-codelite-logo@2x.png
                DESTINATION ${CL_PREFIX}/share/icons/hicolor/256x256@2x/apps
                RENAME codelite.png)

        install(FILES ${CL_SRC_ROOT}/bitmaps-light/256-codelite-logo.png
                DESTINATION ${CL_PREFIX}/share/icons/hicolor/256x256/apps
                RENAME codelite.png)

        # Clear the icon cache if exists
        install(CODE "execute_process(COMMAND rm -f \$ENV{DESTDIR}/${CL_PREFIX}/share/icons/hicolor/icon-theme.cache)")
    endif (  UNIX AND NOT APPLE )
    codelite_install_script(${CL_SRC_ROOT}/Runtime/codelite-remote)

    install(
        DIRECTORY ${CL_SRC_ROOT}/Runtime/images
                  ${CL_SRC_ROOT}/Runtime/gdb_printers
                  ${CL_SRC_ROOT}/Runtime/src/
                  ${CL_SRC_ROOT}/Runtime/lexers
                  ${CL_SRC_ROOT}/Runtime/templates
                  ${CL_SRC_ROOT}/Runtime/rc
                  ${CL_SRC_ROOT}/Runtime/plugins/resources
        DESTINATION ${CL_PREFIX}/share/codelite
        USE_SOURCE_PERMISSIONS
        PATTERN ".svn" EXCLUDE
        PATTERN ".git" EXCLUDE
    )

    install(
        DIRECTORY ${CL_SRC_ROOT}/translations/
        DESTINATION ${CL_PREFIX}/share/locale
        USE_SOURCE_PERMISSIONS
        FILES_MATCHING PATTERN "codelite.mo"
    )

    if (UNIX AND CL_COPY_WX_LIBS) # If we're packaging with wx2.9
        # We need to deduce the location of the wx libs to be installed
        execute_process(COMMAND ${WX_TOOL} --libs OUTPUT_VARIABLE WX_LIBSOUTPUT OUTPUT_STRIP_TRAILING_WHITESPACE)

        if (${WX_LIBSOUTPUT} MATCHES "^-L.+") # In recent, multi-architecture, distro versions it'll start with -L/foo/bar
            string(REGEX REPLACE "^-L([^ ;]+).*" "\\1" WX_LIBS_DIR ${WX_LIBSOUTPUT})
        else()
			# In recent fedoras, which do things with wx-config that might politely be described as 'strange',  wx-config doesn't emit -L/usr/lib64
			#find_file( USRLIBSIXTYFOUR "libwx_baseu-3.1.so" "/usr/lib64" ) No, this can't cope with e.g. libwx_base2u, or 3.1.3<SomeGitVersion> and find_file doesn't do globs/regex
			execute_process(COMMAND "find /usr/lib64/ -iname libwx_base*3.1*" OUTPUT_VARIABLE wxBASELIBDIR)
			if ( NOT wxBASELIBDIR MATCHES ".*NOTFOUND" )
				set(WX_LIBS_DIR "/usr/lib64")
			else()
				set(WX_LIBS_DIR "/usr/lib") # ${WX_LIBS_DIR} will be empty for older, non multi-architecture, distro versions that still use /usr/lib
			endif()
        endif()
        message("-- wxlibsdir is: ${WX_LIBS_DIR}")

        # Grab the necessary libs
        execute_process(COMMAND ${WX_TOOL} --libs all OUTPUT_VARIABLE WX_LIBS_ALL_IN OUTPUT_STRIP_TRAILING_WHITESPACE)
        string(REPLACE " " ";" WX_LIBS_ALL_LIST ${WX_LIBS_ALL_IN}) # the semicolons make the variable into a list

        foreach(ITEM ${WX_LIBS_ALL_LIST})
            # We want to replace -l with lib. We also want to exclude imaginary libs called _all.so, which buggy wx-config scripts can invent
            if (${ITEM} MATCHES "-lwx_.+" AND NOT ${ITEM} MATCHES "-l.+_all.+")
                string(REGEX REPLACE "^-l" "lib" LIBITEM ${ITEM})
                # openSUSE have started to use libwx_gtk2u_xrc-suse.so instead of libwx_gtk2u_xrc-3.1, so allow for extra text there
                string(REGEX REPLACE "(libwx_[a-z0-9._]+-)(.+)" "\\1*\\2" LIBITEMADJ ${LIBITEM})
                list(APPEND WX_LIBS_ALL ${WX_LIBS_DIR}/${LIBITEMADJ}*.so*) # we need the terminal [.]so because the rpms contain both dynamic and static libs
            endif()
        endforeach()

        file(GLOB wxfilepaths ${WX_LIBS_ALL})
        file(GLOB libssh_lib_list "${LIBSSH_LIB}*")
        message("-- will copy ${libssh_lib_list}")
        install(FILES ${wxfilepaths} ${libssh_lib_list} DESTINATION ${CL_PREFIX}/${CL_INSTALL_LIBDIR}/codelite)
    endif()

    if ( USE_CLANG AND MINGW )
      install(FILES ${CLANG_BINARY} DESTINATION ${PLUGINS_DIR})
    endif( USE_CLANG AND MINGW )
else() # !APPLE
    install(TARGETS codelite DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS PERMISSIONS ${EXE_PERM})
    CL_INSTALL_NAME_TOOL_STD(${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite)
    install(FILES ${CL_SRC_ROOT}/Runtime/OpenTerm DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport PERMISSIONS ${EXE_PERM})
endif()
