if(WXC_APP)
    message("-- Building wxCrafter as a standalone executable ")
    add_definitions(-DSTANDALONE_BUILD=1)
    add_definitions(-DINSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")
else()
    message("-- Building wxCrafter as codelite plugin ")
    add_definitions(-DWXCAPP=0)
endif(WXC_APP)

# set the plugin name here
if(WXC_APP)
    set(PLUGIN_NAME "wxcrafter")
else()
    set(PLUGIN_NAME "wxCrafter")
endif()

if(NOT CL_SRC_ROOT)
    message(FATAL_ERROR "Missing CL_SRC_ROOT")
endif()

set(WXC_DIR "${CL_SRC_ROOT}/wxcrafter")

message("-- Generating makefile for wxCrafter")

# Our project is called 'plugin' this is how it will be called in visual studio, and in our makefiles.
project(${PLUGIN_NAME})

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

# Include paths
include_directories("${CL_SRC_ROOT}/Plugin" "${CL_SRC_ROOT}/sdk/wxsqlite3/include" "${CL_SRC_ROOT}/CodeLite"
                    "${CL_SRC_ROOT}/PCH" "${CL_SRC_ROOT}/Interfaces")

add_definitions(-DWXUSINGDLL_WXSQLITE3)
add_definitions(-DWXUSINGDLL_CL)
add_definitions(-DWXUSINGDLL_SDK)

if(USE_PCH AND NOT MINGW)
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
    add_definitions(-fPIC)
endif()

# Add RPATH When building an application, set the install library to the proper location which is now under
# /usr/lib/x86_64-linux-gnu/wxcrafter
if(NOT MINGW)
    if(WXC_APP)
        set(WXC_INSTALL_LIBDIR "lib")
        if(NOT PREFIX)
            # If the caller hasn't set his own destination, install to a multi-arch lib dir if applicable
            if(CMAKE_VERSION VERSION_GREATER 2.8.7 AND (UNIX AND NOT APPLE))
                include(GNUInstallDirs)
                # defines CMAKE_INSTALL_LIBDIR to lib or lib64 or whatever. Available since cmake 2.8.8
                set(WXC_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
            endif()
        endif(NOT PREFIX)
        set(WXC_LIBS_DIR "${CMAKE_INSTALL_PREFIX}/${WXC_INSTALL_LIBDIR}/${PLUGIN_NAME}")
        set(LINKER_OPTIONS -Wl,-rpath,"${WXC_LIBS_DIR}")
    else(WXC_APP)
        if(NOT OFFICIAL_CL_BUILD)
            set(LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")
        endif(NOT OFFICIAL_CL_BUILD)
    endif(WXC_APP)
endif()

# configure version file, this needs to be done before we set the SRCS variable
configure_file(${CL_SRC_ROOT}/wxcrafter/wxcversion.cpp.in ${CL_SRC_ROOT}/wxcrafter/src/wxcversion.cpp)

file(
    GLOB
    SRCS
    "src/*.cpp"
    "aui/*.cpp"
    "controls/*.cpp"
    "sizers/*.cpp"
    "top_level_windows/*.cpp"
    "xrc_handlers/*.cpp")

if(NOT WXC_APP)
    file(GLOB WXCLIB_SRCS "wxcLib/json_node.cpp" "wxcLib/cJSON.cpp" "wxcLib/wxc*.cpp")
else(NOT WXC_APP)
    file(GLOB WXCLIB_SRCS "wxcLib/*.cpp")
endif(NOT WXC_APP)

include_directories(
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/wxcLib"
    "${CMAKE_CURRENT_LIST_DIR}/src"
    "${CMAKE_CURRENT_LIST_DIR}/aui"
    "${CMAKE_CURRENT_LIST_DIR}/controls"
    "${CMAKE_CURRENT_LIST_DIR}/sizers"
    "${CMAKE_CURRENT_LIST_DIR}/top_level_windows"
    "${CMAKE_CURRENT_LIST_DIR}/xrc_handlers")

add_definitions(-fPIC)
add_library(wxcLib STATIC ${WXCLIB_SRCS})

set(RES_FILE "")
# Define the output
if(MINGW)
    set(RES_FILES "resources/wxcapp_resources.rc")
endif()

# Define the output
if(NOT WXC_APP)
    # plugin mode
    add_library(${PLUGIN_NAME} SHARED ${SRCS})
else()
    # executable
    add_executable(${PLUGIN_NAME} ${SRCS} ${RES_FILES})
endif()

if(NOT WXC_APP)
    target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)
endif()

# Remove the "lib" prefix from the plugin name
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_link_libraries(${PLUGIN_NAME} wxcLib)

if(NOT WXC_APP)
    if(NOT OFFICIAL_CL_BUILD)
        target_link_libraries(
            ${PLUGIN_NAME}
            ${LINKER_OPTIONS}
            ${wxWidgets_LIBRARIES}
            wxsqlite3
            ${SQLite3_LIBRARIES}
            libcodelite
            plugin)
    else(OFFICIAL_CL_BUILD)
        target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES})
    endif(NOT OFFICIAL_CL_BUILD)
else()
    set(WXC_EXTRA_LIBS "")
    if(NOT MINGW)
        set(WXC_EXTRA_LIBS "-lutil")
    else()
        set(WXC_EXTRA_LIBS "-mwindows")
    endif()

    target_link_libraries(
        ${PLUGIN_NAME}
        ${LINKER_OPTIONS}
        ${wxWidgets_LIBRARIES}
        wxsqlite3
        libcodelite
        plugin
        ${WXC_EXTRA_LIBS})
endif()

if(NOT WXC_APP)
    # wxCrafter as codelite's plugin
    cl_install_plugin(${PLUGIN_NAME})
    cl_install_file_shared(${WXC_DIR}/wxgui.zip)
elseif(NOT MINGW)
    set(CPACK_DEB_COMPONENT_INSTALL 1)
    # Installing an application
    install(
        TARGETS ${PLUGIN_NAME}
        DESTINATION ${CL_INSTALL_BIN}
        PERMISSIONS ${EXE_PERM}
        COMPONENT standalone)
    install(
        FILES "${WXC_DIR}/wxgui.zip"
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PLUGIN_NAME}
        COMPONENT standalone)
    install(
        FILES "${CL_SRC_ROOT}/Runtime/codelite-bitmaps-light.zip"
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PLUGIN_NAME}
        COMPONENT standalone)
    install(
        FILES "${CL_SRC_ROOT}/Runtime/codelite-bitmaps-dark.zip"
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PLUGIN_NAME}
        COMPONENT standalone)
    install(
        FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/liblibcodelite.so"
        DESTINATION ${WXC_LIBS_DIR}
        COMPONENT standalone)
    install(
        FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libplugin.so"
        DESTINATION ${WXC_LIBS_DIR}
        COMPONENT standalone)
    install(
        FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libwxsqlite3.so"
        DESTINATION ${WXC_LIBS_DIR}
        COMPONENT standalone)
    install(
        FILES "${CL_SRC_ROOT}/wxcrafter/resources/wxc-logo-64.png"
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PLUGIN_NAME}
        COMPONENT standalone)
    install(
        FILES "${CL_SRC_ROOT}/Runtime/config/codelite.xml.default.gtk"
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PLUGIN_NAME}/config
        RENAME codelite.xml.default
        COMPONENT standalone)

    if(NOT DEBUG_BUILD)
        message("-- Will install ${CL_SRC_ROOT}/wxcrafter/wxCrafter.desktop to /usr/share/applications")
        install(
            FILES "${CL_SRC_ROOT}/wxcrafter/wxCrafter.desktop"
            DESTINATION /usr/share/applications
            COMPONENT standalone)
    endif(NOT DEBUG_BUILD)

    # Install codelite lexer files
    install(
        DIRECTORY ${CL_SRC_ROOT}/Runtime/lexers
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PLUGIN_NAME}
        COMPONENT standalone
        USE_SOURCE_PERMISSIONS
        PATTERN ".svn" EXCLUDE
        PATTERN ".git" EXCLUDE)

    # ##################################################################################################################
    # CPack
    # ##################################################################################################################
    message("-- Generating deb target")
    message("-- CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

    # ##################################################################################################################
    # We need to deduce the location of the wx libs to be installed
    # ##################################################################################################################
    execute_process(
        COMMAND ${WX_TOOL} --libs
        OUTPUT_VARIABLE WX_LIBSOUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(${WX_LIBSOUTPUT} MATCHES "^-L.+")
        # In recent, multi-architecture, distro versions it'll start with -L/foo/bar
        string(REGEX REPLACE "^-L([^ ;]+).*" "\\1" WX_LIBS_DIR ${WX_LIBSOUTPUT})
    else()
        set(WX_LIBS_DIR "/usr/lib")
        # ${WX_LIBS_DIR} will be empty for older, non multi-architecture, distro versions that still use /usr/lib
    endif()
    message("-- wxlibsdir is: ${WXC_LIBS_DIR}")

    # Grab the necessary libs
    execute_process(
        COMMAND ${WX_TOOL} --libs all
        OUTPUT_VARIABLE WX_LIBS_ALL_IN
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REPLACE " " ";" WX_LIBS_ALL_LIST ${WX_LIBS_ALL_IN})
    # the semicolons make the variable into a list

    foreach(ITEM ${WX_LIBS_ALL_LIST})
        # We want to replace -l with lib. We also want to exclude imaginary libs called _all.so, which buggy wx-config
        # scripts can invent
        if(${ITEM} MATCHES "-lwx_.+" AND NOT ${ITEM} MATCHES "-l.+_all.+")
            string(REGEX REPLACE "^-l" "lib" LIBITEM ${ITEM})
            list(APPEND WX_LIBS_ALL ${WX_LIBS_DIR}/${LIBITEM}.so*)
            # we need the terminal [.]so because the rpms contain both dynamic and static libs
        endif()
    endforeach()

    file(GLOB wxfilepaths ${WX_LIBS_ALL})
    install(
        FILES ${wxfilepaths}
        DESTINATION ${WXC_LIBS_DIR}
        COMPONENT standalone)

    set(CPACK_GENERATOR "DEB")
    set(CPACK_PACKAGE_NAME "wxCrafter${GTKNAME}")
    set(CPACK_PACKAGE_VERSION "3.0")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Eran Ifrah <eran@codelite.org>")
    # required
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GUI Rad Tool for wxWidgets")
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "build-essential")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libssh-4, libsqlite3-dev")
    include(CPack)
endif()

if(WXC_APP AND MINGW)
    install(TARGETS ${PLUGIN_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()

message(STATUS "wxCrafter - Done")
