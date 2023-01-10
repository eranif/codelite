# 3.16 and later is required so we can build CodeLite with PCH
cmake_minimum_required(VERSION 3.16)

# ######################################################################################################################
#
# CodeLite IDE cmake file Typical usage will be (build in release mode):
#
# Linux / macOS:
# -------------------
# ~~~
# mkdir build
# cd build
# cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
# make -j$(nproc) install
# ~~~
#
# MSYS2
# -------------------
#
# ~~~
# mkdir build-release
# cd build-release
# cmake -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release .. -DWXWIN=C:/msys64/path/to/wx-dir
# make -j$(nproc)
# ~~~
#
# Optional command line arguments:
#
# -DCMAKE_BUILD_TYPE=Release|Debug|DebugFull -DCOPY_WX_LIBS=1|0 -DPREVENT_WX_ASSERTS=1|0 -DAUTOGEN_REVISION=1|0
# -DWITH_PCH=1|0 -DWITH_NATIVEBOOK=1|0 -DWITH_WXPATH=/path/to/different_wx-config/directory/ -DMAKE_DEB=1|0
# -DENABLE_SFTP=1|0 // When set to 1 codelite is built with SFTP/Remoty support. -DWITH_WX_CONFIG=/path/to/wx-config
#
# Windows / MSYS options:
#
# -DWXWIN=<fullpath>                          // https://docs.codelite.org/build/build_wx_widgets/#windows
# -DWITH_POSIX_LAYOUT // When building under Windows, install CodeLite using POSIX layout
#
# ######################################################################################################################

if(NOT CMAKE_VERSION VERSION_LESS 3.1) # THIS MUST STAY AT THE TOP OF THE FILE
    cmake_policy(VERSION 3.1) # Doing this prevents multiple, very verbose warnings about policy CMP0053 not being set
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# ######################################################################################################################
# Defaults
# ######################################################################################################################
project("CodeLite")

if(UNIX AND NOT APPLE)
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        # user did not pass -DCMAKE_INSTALL_PREFIX and this is the first time we are executing cmake, the default is set
        # to /usr/local and we do not want that..
        set(CMAKE_INSTALL_PREFIX
            "/usr"
            CACHE ON "Install prefix" FORCE)
    endif()
endif()

if(MINGW)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
    set(CL_PREFIX "${CMAKE_INSTALL_PREFIX}")
    set(MSYS2_USR_BIN $ENV{WD})
    string(REPLACE "\\usr\\bin" "" MSYS2_BASE "${MSYS2_USR_BIN}")
    string(REPLACE "\\" "/" MSYS2_BASE "${MSYS2_BASE}")
elseif(UNIX AND NOT APPLE)
    if(CL_PREFIX)
        # user provided CL_PREFIX -> use it instead of CMAKE_INSTALL_PREFIX
        set(CMAKE_INSTALL_PREFIX "${CL_PREFIX}")
    else()
        # update CL_PREFIX to CMAKE_INSTALL_PREFIX
        set(CL_PREFIX "${CMAKE_INSTALL_PREFIX}")
    endif()
endif()

message(STATUS "CMAKE_INSTALL_PREFIX is set to ${CMAKE_INSTALL_PREFIX}")
set(CODELITE_VERSION "17.0.0")

# for now, we keep wxCrafter version to be the same as CodeLite version this allows for better support
set(WXCRAFTER_VERSION ${CODELITE_VERSION})

# Generate compile_commands.json file for code completion
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CL_SRC_ROOT ${CMAKE_CURRENT_LIST_DIR})

find_package(SQLite3)
include(UtilsHelper)

if(NOT SQLite3_FOUND)
    if(UNIX AND NOT APPLE)
        message(FATAL_ERROR " **** Could not find Sqlite3. Please install libsqlite3-dev package **** ")
    endif()
    set(SQLite3_INCLUDE_DIRS ${CL_SRC_ROOT}/sqlite3)
    set(SQLite3_LIBRARIES "-lsqlite3lib")
endif()

set(IS_FREEBSD 0)
set(IS_NETBSD 0)
set(BUILD_WXC 0)
set(CL_COPY_WX_LIBS 0)
set(WITH_SFTP 1)

if(APPLE)
    # add homebrew path to cmake search pathsbrew --prefix
    execute_process(
        COMMAND brew --prefix
        OUTPUT_VARIABLE BREW_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    list(APPEND CMAKE_SYSTEM_PREFIX_PATH ${BREW_PREFIX})
    message(STATUS "CMAKE_SYSTEM_PREFIX_PATH is set to ${CMAKE_SYSTEM_PREFIX_PATH}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations") # Avoid very multiple warnings spam due to
                                                                       # deprecated wx methods
if(UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem /usr/include/harfbuzz"
    )# Needed for fedora 31 so far, but will spread. See https://gitlab.kitware.com/cmake/cmake/issues/19531
endif()

if(UNIX
   AND NOT APPLE
   AND NOT CYGWIN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-new-dtags")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--disable-new-dtags")
endif()

if(UNIX)
    # Configure CCache if available
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    endif(CCACHE_FOUND)
endif(UNIX)

if(UNIX)
    set(BUILD_DIRECTORY ${CMAKE_BINARY_DIR})
elseif(MINGW)
    set(BUILD_DIRECTORY ${CMAKE_BINARY_DIR})
else()
    set(BUILD_DIRECTORY $ENV{CD})
endif()

message("-- BUILD_DIRECTORY is set to ${BUILD_DIRECTORY}")

set(OS_NAME "WIN")
if(UNIX)
    execute_process(
        COMMAND uname -s
        OUTPUT_VARIABLE OS_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
message("-- OS name ${OS_NAME}")

if(APPLE)
    set(WX_COMPONENTS "std aui propgrid ribbon")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
else(APPLE)
    set(WX_COMPONENTS "std aui propgrid stc ribbon richtext")
endif()

if(UNIX)
    find_path(CTAGS_SUBMODULE CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/ctags/)
    if(NOT CTAGS_SUBMODULE)
        message(STATUS "Could not locate `${CMAKE_SOURCE_DIR}/ctags/CMakeLists.txt` file")
        message(FATAL_ERROR "Please make sure you have run `git submodule update --init`")
    endif()
endif()

if(WITH_WXPATH)
    set(ENV{PATH} ${WITH_WXPATH}:$ENV{PATH})
endif()
unset(WITH_WXPATH CACHE)

if(WITH_WX_CONFIG)
    set(CL_WX_CONFIG ${WITH_WX_CONFIG})
else(WITH_WX_CONFIG)
    set(CL_WX_CONFIG wx-config)
endif()

if(UNIX AND NOT MINGW)
    execute_process(
        COMMAND which ${CL_WX_CONFIG}
        OUTPUT_VARIABLE WX_TOOL
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT WX_TOOL)
        message(
            FATAL_ERROR
                "\nNo functional wx_config script was found in your PATH.\nIs the wxWidgets development package installed?"
        )
    else()
        # cmake find_package() will try env var WX_CONFIG as wx-config tool path before checking its builtin hardcoded
        # naming conbinations for wx-config tool
        set(ENV{WX_CONFIG} ${WX_TOOL})
        execute_process(
            COMMAND sh ${WX_TOOL} --version
            OUTPUT_VARIABLE WX_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        if("${WX_VERSION}" VERSION_LESS "3.1.6")
            message(
                FATAL_ERROR
                    "I'm afraid your wxWidgets version is too old.\nBuilding CodeLite requires at least wxWidgets-3.1.6"
            )
        endif()
        if(MINGW)
            execute_process(
                COMMAND sh ${WX_TOOL} --debug=no --rescomp
                OUTPUT_VARIABLE WX_RC_FLAGS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
            string(REGEX REPLACE "windres" "" WX_RC_FLAGS ${WX_RC_FLAGS})
            set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} ${WX_RC_FLAGS}")
            add_definitions(-D__WXMSW__)
        endif(MINGW)
    endif()
    message("-- wx-config used is: ${WX_TOOL}")
    message("-- wxWidgets version is: ${WX_VERSION}")
    if(NOT APPLE AND NOT MINGW)
        # Is the wx we are using built on gtk2 or 3?
        execute_process(
            COMMAND ${WX_TOOL} --selected_config
            OUTPUT_VARIABLE WX_GTK_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        string(SUBSTRING "${WX_GTK_VERSION}" "3" "1" GTK_VERSION)
        message("-- gtk version is: ${GTK_VERSION}")
    endif()
endif()

# ######################################################################################################################
# Override defaults with user input
# ######################################################################################################################

set(CL_INSTALL_LIBDIR "lib")

if(NOT MINGW)
    if(NOT CL_PREFIX)
        # If the caller hasn't set his own destination, install to a multi-arch lib dir if applicable
        if(CMAKE_VERSION VERSION_GREATER 2.8.7 AND (UNIX AND NOT APPLE))
            if(CMAKE_VERSION VERSION_GREATER 3.0.0)
                # Prior to this, afaict the GNUInstallDirs module worked whatever the prefix Since, it looks at
                # CMAKE_INSTALL_PREFIX which is /usr/local by default, and refuses to run the multiarch-setting code
                # unless it's /usr/ So, partly to comply with the default documented above, & partly for packaging,
                # explicitly set it to /usr
                set(CMAKE_INSTALL_PREFIX "/usr")
            endif()
            include(GNUInstallDirs) # defines CMAKE_INSTALL_LIBDIR to lib or lib64 or whatever. Available since cmake
                                    # 2.8.8
            set(CL_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
        endif()

        set(CL_PREFIX "/usr")
    endif(NOT CL_PREFIX)
endif()

# Define the PLUGINS_DIR variable
if(MINGW)
    # MinGW
    set(MINGW_INSTALL_PLUGINS_DIR "${CMAKE_INSTALL_PREFIX}/plugins")
    set(MINGW_INSTALL_BIN "${CMAKE_INSTALL_PREFIX}")
endif()

if(APPLE)
    set(PLUGINS_DIR ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport/plugins)
    set(CL_INSTALL_BIN ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS)
    set(CL_RESOURCES_DIR ${CMAKE_BINARY_DIR}/codelite.app/Contents/SharedSupport)
elseif(MINGW)
    set(PLUGINS_DIR "${MINGW_INSTALL_PLUGINS_DIR}")
    set(CL_INSTALL_BIN "${MINGW_INSTALL_BIN}")
    set(CL_RESOURCES_DIR "${MINGW_INSTALL_BIN}")
else()
    # /usr/lib/codelite
    set(PLUGINS_DIR "${CL_PREFIX}/${CL_INSTALL_LIBDIR}/codelite")
    # /usr/bin
    set(CL_INSTALL_BIN "${CMAKE_INSTALL_PREFIX}/bin")
    # /usr/share/codelite
    set(CL_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/share/codelite")
endif()

message(STATUS "CL_INSTALL_BIN is set to ${CL_INSTALL_BIN}")
message(STATUS "PLUGINS_DIR is set to ${PLUGINS_DIR}")

# Try to link-to or build lldb?
if(UNIX)
    set(WITH_LLDB 1)
else()
    set(WITH_LLDB 0)
endif()
if(ENABLE_LLDB MATCHES 0)
    set(WITH_LLDB 0)
    message("-- Disabling lldb support")
endif()
unset(ENABLE_LLDB CACHE)

set(DISABLE_CXX 0)
if(PHP_BUILD MATCHES 1)
    set(DISABLE_CXX 1)
    add_definitions(-DPHP_BUILD=1)
else()
    set(DISABLE_CXX 0)
endif()

# ######################################################################################################################
# Locate libssh
# ######################################################################################################################
# Enable SFTP support?
if(ENABLE_SFTP MATCHES 0)
    set(WITH_SFTP 0)
endif(ENABLE_SFTP MATCHES 0)

if(MAKE_RPM_SLIM MATCHES 1)
    set(WITH_SFTP 0)
endif()

unset(ENABLE_SFTP CACHE)
include(OSXInstall)

if(WITH_SFTP)
    if(UNIX OR MINGW)
        # Linux / MinGW / Apple / CYGWIN
        if(MINGW OR CYGWIN)
            find_library(
                LIBSSH_LIB
                NAMES ssh
                PATH_SUFFIXES lib)
            find_path(
                LIBSSH_INCLUDE_DIR
                NAMES libssh.h
                PATH_SUFFIXES include/libssh)
        elseif(APPLE)
            cl_osx_find_brew_lib(libssh.4.dylib LIBSSH_LIB)
            cl_osx_find_brew_header(libssh/libssh.h LIBSSH_INCLUDE_DIR)
            message(STATUS "LIBSSH_INCLUDE_DIR is set to ${LIBSSH_INCLUDE_DIR}")
            include_directories(${LIBSSH_INCLUDE_DIR})
        elseif(UNIX)
            # Linux
            find_library(
                LIBSSH_LIB
                NAMES libssh.so
                HINTS /usr/local/lib /usr/lib ${CMAKE_INSTALL_LIBDIR})
            find_path(
                LIBSSH_INCLUDE_DIR
                NAMES libssh.h
                HINTS /usr/local/include /usr/include
                PATH_SUFFIXES libssh)
        endif()
        string(FIND ${LIBSSH_INCLUDE_DIR} "NOTFOUND" LIBSSH_NOT_FOUND_POS)
        if(LIBSSH_NOT_FOUND_POS GREATER -1)
            if(UNIX AND NOT APPLE)
                # Linux / FreeBSD
                message(FATAL_ERROR "**** NOTICE: Install libssh-dev and try again")
            elseif(APPLE)
                message(FATAL_ERROR "Could not locate libssh. Please run 'brew install libssh'")
            elseif(NOT MINGW)
                # OSX
                message(FATAL_ERROR "**** NOTICE: Install libssh and try again (brew install libssh)")
            endif()
            message(FATAL_ERROR "-- Could not find libssh")
        endif()
    endif()
    message("-- LIBSSH_LIB is set to ${LIBSSH_LIB}")
endif(WITH_SFTP)

# On UNIX we require GTK
if(UNIX AND NOT APPLE)
    if(GTK_VERSION EQUAL 3)
        set(OLD_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}) # Cache the current value
        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
        find_package(GTK3)
        set(CMAKE_MODULE_PATH ${OLD_CMAKE_MODULE_PATH}) # Reset, else the official path isn't used again :/
        if(GTK3_FOUND)
            include_directories(${GTK3_INCLUDE_DIRS})
        else(GTK3_FOUND)
            message(FATAL_ERROR "Could not locate GTK.")
        endif(GTK3_FOUND)
    else()
        find_package(GTK2)
        if(GTK2_FOUND)
            include_directories(${GTK2_INCLUDE_DIRS})
        else(GTK2_FOUND)
            message(FATAL_ERROR "Could not locate GTK.")
        endif(GTK2_FOUND)
    endif()

endif(UNIX AND NOT APPLE)

# find wxWidgets once
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
        COMMAND ${CL_WX_CONFIG} --cmake --prefix=${WXWIN} --wxcfg=${WXCFG}
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

if(AUTOGEN_REVISION MATCHES 0)
    set(MAKE_AUTOGEN_REVISION_STRING 0)
else(AUTOGEN_REVISION MATCHES 1)
    set(MAKE_AUTOGEN_REVISION_STRING 1)
endif(AUTOGEN_REVISION MATCHES 0)
unset(AUTOGEN_REVISION CACHE)

set(BUILD_WXC 1)
if(UNIX AND NOT APPLE)
    set(WX_COMPONENTS "std aui propgrid stc richtext ribbon")
endif(UNIX AND NOT APPLE)

# package the wx libs?
if(COPY_WX_LIBS MATCHES 1)
    set(CL_COPY_WX_LIBS 1)
endif()
unset(COPY_WX_LIBS CACHE)

# file encoding detect
if(UNIX
   AND NOT APPLE
   OR MINGW)
    # Linux
    if(MINGW)
        find_library(
            LIBUCHARDET_LIB
            NAMES uchardet
            PATH_SUFFIXES lib)
        find_path(
            LIBUCHARDET_INCLUDE_DIR
            NAMES uchardet.h
            PATH_SUFFIXES include/uchardet)
    else()
        find_library(
            LIBUCHARDET_LIB
            NAMES libuchardet.so
            HINTS /usr/local/lib /usr/lib ${CMAKE_INSTALL_LIBDIR})
        find_path(
            LIBUCHARDET_INCLUDE_DIR
            NAMES uchardet.h
            HINTS /usr/local/include /usr/include
            PATH_SUFFIXES uchardet)
    endif()
else(
    UNIX
    AND NOT APPLE
    OR MINGW)
    # OSX set( LIBUCHARDET_INCLUDE_DIR ${CL_SRC_ROOT}/sdk/uchardet/include) set( LIBUCHARDET_LIB
    # ${CL_SRC_ROOT}/sdk/uchardet/lib/osx/uchardet.a)
endif(
    UNIX
    AND NOT APPLE
    OR MINGW)
if(LIBUCHARDET_LIB AND LIBUCHARDET_INCLUDE_DIR)
    add_definitions(-DUSE_UCHARDET=1)
    include_directories(${LIBUCHARDET_INCLUDE_DIR})
else(
    UNIX
    AND NOT APPLE
    OR MINGW)
    set(LIBUCHARDET_LIB "")
endif(LIBUCHARDET_LIB AND LIBUCHARDET_INCLUDE_DIR)

# Under OSX, create the skeleton bundle directory
osx_make_bundle_directory()

add_definitions(-DYY_NEVER_INTERACTIVE=1)
add_definitions(-DwxUSE_GUI=1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(NOT MINGW)
    add_definitions(-DINSTALL_DIR=\"${CL_PREFIX}/share/codelite\")
    add_definitions(-DPLUGINS_DIR=\"${PLUGINS_DIR}\")
else()
    if(WITH_POSIX_LAYOUT)
        add_definitions(-DNDEBUG)
        add_definitions(-DUSE_POSIX_LAYOUT)
    endif()
endif()

message("-- PLUGINS_DIR is set to ${PLUGINS_DIR}")

# Allow user to use wxAuiNotebook instead of the native notebook
if(UNIX
   AND NOT APPLE
   AND NOT MINGW)
    # by default, use GTK native notebook control
    set(CL_GTK_USE_NATIVEBOOK 1)
    if(WITH_NATIVEBOOK MATCHES 0)
        set(CL_GTK_USE_NATIVEBOOK 0)
    endif()

    if(CL_GTK_USE_NATIVEBOOK MATCHES 1)
        add_definitions(-DGTK_USE_NATIVEBOOK=1)
        message("-- Using Native Notebook class")
    else()
        add_definitions(-DGTK_USE_NATIVEBOOK=0)
        message("-- Using custom Notebook")
    endif()
else() # NOT LINUX
    add_definitions(-DGTK_USE_NATIVEBOOK=0)
    message("-- Using custom Notebook")
endif()

unset(CL_GTK_USE_NATIVEBOOK CACHE)

# ######################################################################################################################
# Global optimizations
# ######################################################################################################################

if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES DebugFull)
    message("-- Building in ${CMAKE_BUILD_TYPE} mode")
    set(DEBUG_BUILD 1)
    set(CL_PCH_FILE "${CL_SRC_ROOT}/PCH/precompiled_header_dbg.h")
    set(CL_PCH_TARGET "precompiled_header_dbg.h.gch")

    # Set the libraries outout directory
    set(CL_BIN_DIR bin)
    set(CL_LIB_DIR lib)
    if(APPLE)
        add_custom_target(distclean COMMAND cd ${CL_SRC_ROOT}/PCH && $(MAKE) type=debug clean)
    endif(APPLE)

    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIRECTORY}/${CL_LIB_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIRECTORY}/${CL_BIN_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIRECTORY}/${CL_LIB_DIR}")
    set(CL_LIBPATH "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

    message("-- Executables will be written into ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    message("-- Shared Objects will be written into ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0") # No optimization, debug info
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")

    # In debug, only add NDEBUG if the user says so
    if(PREVENT_WX_ASSERTS MATCHES 1)
        message("-- Adding -DNDEBUG to definitions")
        add_definitions(-DNDEBUG)
    endif()

else()
    message("-- Building in Release mode")
    set(DEBUG_BUILD 0)
    set(CMAKE_INSTALL_DO_STRIP TRUE)
    message("-- CMAKE_INSTALL_DO_STRIP is " ${CMAKE_INSTALL_DO_STRIP})
    if(UNIX AND NOT APPLE)
        # Avoid hardening-no-relro wrarning message from lintian
        if(EXISTS "/usr/bin/dpkg-buildflags")
            execute_process(
                COMMAND /usr/bin/dpkg-buildflags --get CFLAGS
                OUTPUT_VARIABLE EXTRA_CFLAGS
                OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
            execute_process(
                COMMAND /usr/bin/dpkg-buildflags --get CPPFLAGS
                OUTPUT_VARIABLE EXTRA_CPPFLAGS
                OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
            execute_process(
                COMMAND /usr/bin/dpkg-buildflags --get CXXFLAGS
                OUTPUT_VARIABLE EXTRA_CXXFLAGS
                OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
            execute_process(
                COMMAND /usr/bin/dpkg-buildflags --get LDFLAGS
                OUTPUT_VARIABLE EXTRA_LDFLAGS
                OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXXFLAGS}")
            add_definitions(${EXTRA_CPPFLAGS})
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")
            set(LINKER_OPTIONS "${EXTRA_LDFLAGS}")
            list(APPEND LINKER_OPTIONS "-s") # strip binaries
        endif()
    endif()

    set(CL_PCH_FILE "${CL_SRC_ROOT}/PCH/precompiled_header_release.h")
    set(CL_PCH_TARGET "precompiled_header_release.h.gch")
    if(APPLE)
        add_custom_target(distclean COMMAND cd ${CL_SRC_ROOT}/PCH && $(MAKE) type=release clean)
    endif(APPLE)

    # Set the libraries outout directory
    set(CL_BIN_DIR bin)
    set(CL_LIB_DIR lib)

    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIRECTORY}/${CL_LIB_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIRECTORY}/${CL_BIN_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIRECTORY}/${CL_LIB_DIR}")
    set(CL_LIBPATH "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

    message("-- Executables will be written into ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    message("-- Shared Objects will be written into ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2") # Optimize
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s") # Strip binary
    endif(CMAKE_COMPILER_IS_GNUCXX)

    # In release, add NDEBUG unless explicitly told not to
    if(NOT PREVENT_WX_ASSERTS MATCHES 0)
        message("-- Adding -DNDEBUG to definitions")
        add_definitions(-DNDEBUG)
    endif()

endif()
unset(CMAKE_BUILD_TYPE CACHE)
unset(PREVENT_WX_ASSERTS CACHE)

# ######################################################################################################################
# Determine if 32 or 64 bit
# ######################################################################################################################

set(ARCH 32)
set(ARCH_NAME i386)
set(OS_CODENAME "unknown")

if(UNIX AND NOT APPLE)
    execute_process(
        COMMAND /bin/sh -c "lsb_release -a|grep -i Codename | cut -d: -f2"
        OUTPUT_VARIABLE OS_CODENAME
        OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    execute_process(
        COMMAND /bin/sh -c "lsb_release -a|grep -i Distributor | cut -d: -f2"
        OUTPUT_VARIABLE OS_DISTRO
        OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    execute_process(
        COMMAND /bin/sh -c "uname -m"
        OUTPUT_VARIABLE OS_ARCH
        OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)

    # replace any tab/space with nothing
    string(REPLACE " " "" DISTRO_CODENAME "${OS_DISTRO}-${OS_CODENAME}-${OS_ARCH}")
    string(REPLACE "\t" "" DISTRO_CODENAME "${DISTRO_CODENAME}")
    string(TOLOWER ${DISTRO_CODENAME} DISTRO_CODENAME)

    string(REPLACE " " "" OS_CODENAME "${OS_CODENAME}")
    string(REPLACE "\t" "" OS_CODENAME "${OS_CODENAME}")
    if(NOT "${OS_CODENAME}" STREQUAL "") # It'll be empty if lsb_release isn't installed, which causes cmake to error
                                         # out on the next line
        string(TOLOWER ${OS_CODENAME} OS_CODENAME)
    endif()

    set(CPACK_SYSTEM_NAME "${DISTRO_CODENAME}")
    message("-- CPACK_SYSTEM_NAME ${CPACK_SYSTEM_NAME}")
endif(UNIX AND NOT APPLE)
message("-- OS_CODENAME is set to ${OS_CODENAME}")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH 64)
    set(ARCH_NAME x86_64)
endif()

message("-- ARCH ${ARCH}")
message("-- ARCH_NAME ${ARCH_NAME}")

# ######################################################################################################################
# CPack
# ######################################################################################################################
if(MAKE_DEB)
    get_distro_name(distro)
    message("-- Distro name: ${distro}")

    set(DEB_DEPS "")
    if(${distro} STREQUAL "debian_10")
        set(DEB_DEPS "clang-tools (>= 7), clang-format (>= 10)")
    endif()

    if(${distro} MATCHES "ubuntu")
        set(DEB_DEPS "clangd (>= 10), clang-format (>= 10)")
    endif()

    message("-- Generating deb target")
    if(${ARCH} EQUAL 32)
        message("-- CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
    else()
        message("-- CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
    endif()

    if(GTK_VERSION EQUAL 3)
        set(LIBGTK "libgtk-3-dev")
        set(GTKNAME "gtk3")
    else()
        set(LIBGTK "libgtk2.0-0")
        set(GTKNAME "gtk2")
    endif()
    if(PHP_BUILD)
        set(GTKNAME "${GTKNAME}-php")
    endif()
    set(CPACK_GENERATOR "DEB")
    set(CPACK_PACKAGE_NAME "CodeLite")
    set(CPACK_PACKAGE_VERSION "${CODELITE_VERSION}-${GTKNAME}")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Eran Ifrah <eran@codelite.org>")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C/C++/Rust/Python/PHP/Node.js IDE (Integrated Development Environment)")
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "build-essential, git, subversion, gdb, xterm, gcc, g++")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "${DEB_DEPS}")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${CL_SRC_ROOT}/DESC")
    set(CPACK_STRIP_FILES TRUE)
    include(CPack)
endif(MAKE_DEB)

if(MAKE_RPM)
    message("-- Generating rpm target")
    message("-- CPACK_PACKAGE_ARCHITECTURE amd64")
    if(GTK_VERSION EQUAL 3)
        set(GTKNAME "gtk3")
    else()
        set(GTKNAME "gtk2")
    endif()
    set(CPACK_RPM_PACKAGE_AUTOREQ 0)
    set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
        "/usr/share/icons;/usr/share/locale/;/usr/share/locale/cs;/usr/share/locale/cs/LC_MESSAGES;/usr/share/locale/ja_JP;/usr/share/locale/ja_JP/LC_MESSAGES;/usr/share/locale/ru_RU;/usr/share/locale/ru_RU/LC_MESSAGES;/usr/share/locale/zh_CN;/usr/share/locale/zh_CN/LC_MESSAGES;/usr/share/man;/usr/share/icons/hicolor;/usr/share/icons/hicolor/128x128;/usr/share/icons/hicolor/128x128/apps;/usr/share/icons/hicolor/256x256;/usr/share/icons/hicolor/256x256/apps;/usr/share/icons/hicolor/32x32;/usr/share/icons/hicolor/32x32/apps;/usr/share/icons/hicolor/64x64;/usr/share/icons/hicolor/64x64/apps;/usr/share/applications"
    )
    set(CPACK_RPM_PACKAGE_REQUIRES "gcc, g++, git, sqlite, libssh, gtk3, cmake")
    set(CPACK_GENERATOR "RPM")
    set(CPACK_RPM_PACKAGE_NAME "CodeLite")
    set(CPACK_RPM_FILE_NAME "CodeLite-${CODELITE_VERSION}-x86_64.rpm")
    set(CPACK_RPM_PACKAGE_VERSION "${CODELITE_VERSION}-${GTKNAME}")
    set(CPACK_PACKAGE_CONTACT "Eran Ifrah <eran@codelite.org>")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C/C++/Rust/Python/PHP/Node.js IDE (Integrated Development Environment)")
    set(CPACK_PACKAGE_VENDOR "The CodeLite Team")
    set(CPACK_STRIP_FILES TRUE)
    include(CPack)
endif(MAKE_RPM)

if(MAKE_RPM_SLIM)
    message("-- Generating rpm target")
    message("-- CPACK_PACKAGE_ARCHITECTURE amd64")
    if(GTK_VERSION EQUAL 3)
        set(GTKNAME "gtk3")
    else()
        set(GTKNAME "gtk2")
    endif()
    set(CPACK_RPM_PACKAGE_AUTOREQ 0)
    set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
        "/usr/share/icons;/usr/share/locale/;/usr/share/locale/cs;/usr/share/locale/cs/LC_MESSAGES;/usr/share/locale/ja_JP;/usr/share/locale/ja_JP/LC_MESSAGES;/usr/share/locale/ru_RU;/usr/share/locale/ru_RU/LC_MESSAGES;/usr/share/locale/zh_CN;/usr/share/locale/zh_CN/LC_MESSAGES;/usr/share/man;/usr/share/icons/hicolor;/usr/share/icons/hicolor/128x128;/usr/share/icons/hicolor/128x128/apps;/usr/share/icons/hicolor/256x256;/usr/share/icons/hicolor/256x256/apps;/usr/share/icons/hicolor/32x32;/usr/share/icons/hicolor/32x32/apps;/usr/share/icons/hicolor/64x64;/usr/share/icons/hicolor/64x64/apps;/usr/share/applications"
    )
    set(CPACK_RPM_PACKAGE_REQUIRES "gcc72, g++72, git, sqlite, gtk3, cmake")
    set(CPACK_GENERATOR "RPM")
    set(CPACK_RPM_PACKAGE_NAME "CodeLite")
    set(CPACK_RPM_FILE_NAME "CodeLite-slim-${CODELITE_VERSION}-gtk2-x86_64.rpm")
    set(CPACK_RPM_PACKAGE_VERSION "${CODELITE_VERSION}-${GTKNAME}")
    set(CPACK_PACKAGE_CONTACT "Eran Ifrah <eran@codelite.org>")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C/C++ IDE (Integrated Development Environment)")
    set(CPACK_PACKAGE_VENDOR "Eran Ifrah")
    set(CPACK_STRIP_FILES TRUE)
    include(CPack)
endif(MAKE_RPM_SLIM)

# ######################################################################################################################
# SFTP support
# ######################################################################################################################
if(WITH_SFTP)

    # Default is set to 1
    add_definitions(-DUSE_SFTP=1)
    message("-- USE_SFTP is set to 1")

else(WITH_SFTP)
    add_definitions(-DUSE_SFTP=0)
    message("-- USE_SFTP is set to 0")
    message("-- *** NOTICE ***: SFTP support is disabled ")

endif(WITH_SFTP)

# ######################################################################################################################
# RPATH settings
# ######################################################################################################################

if(UNIX AND NOT APPLE)
    set(CMAKE_SKIP_BUILD_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    list(PREPEND CMAKE_INSTALL_RPATH "${PLUGINS_DIR}")
endif(UNIX AND NOT APPLE)

if(WITH_PCH)
    set(USE_PCH 1)
endif(WITH_PCH)
unset(WITH_PCH CACHE)

if(MINGW AND USE_PCH)
    if(CMAKE_VERSION VERSION_LESS 3.16)
        message(FATAL_ERROR " **** MINGW and PCH requires at least cmake version 3.16 **** ")
    endif()
endif()

# ######################################################################################################################
# Add the folders, the order matters here
# ######################################################################################################################
if(APPLE)
    add_definitions(-mmacosx-version-min=10.8)
endif()

if(NOT SQLite3_FOUND)
    # On Linux, we use the distro's libsqlite3
    add_subdirectory(sqlite3)
endif()

# build the Core part of CodeLite (regardless of the final target)
add_subdirectory(sdk/wxsqlite3)
add_subdirectory(sdk/wxshapeframework)
add_subdirectory(sdk/databaselayer)
add_subdirectory(CodeLite)
add_subdirectory(Plugin)
add_subdirectory(PCH)

if(WXC_APP)
    add_subdirectory(wxcrafter)
else()
    if(UNIX OR APPLE)
        add_subdirectory(ctags)
    endif()

    if(MINGW)
        # build wx-config for Windows
        add_subdirectory(wx-config-msys2)
        install(TARGETS wx-config DESTINATION ${CL_INSTALL_BIN})
    endif()

    if(NOT NO_CORE_PLUGINS)
        add_subdirectory(abbreviation)
        if(NOT DISABLE_CXX)
            add_subdirectory(Gizmos)
            add_subdirectory(Debugger)
            add_subdirectory(UnitTestCPP)
            add_subdirectory(QmakePlugin)
            add_subdirectory(cppchecker)
            add_subdirectory(wxformbuilder)
            add_subdirectory(CMakePlugin)
            add_subdirectory(cscope)
            add_subdirectory(EOSWiki)
            add_subdirectory(ContinuousBuild)
        endif()
        add_subdirectory(AutoSave)
        add_subdirectory(CodeLiteDiff)
        add_subdirectory(git)
        add_subdirectory(Outline)
        add_subdirectory(CodeFormatter)
        add_subdirectory(Copyright)
        add_subdirectory(DatabaseExplorer)
        add_subdirectory(ExternalTools)
        add_subdirectory(SnipWiz)
        add_subdirectory(Subversion2)
        add_subdirectory(ZoomNavigator)
        add_subdirectory(SpellChecker)
        add_subdirectory(Tail)
        add_subdirectory(EditorConfigPlugin)
        add_subdirectory(PHPLint)
        add_subdirectory(PHPRefactoring)
        add_subdirectory(codelite_vim)
        add_subdirectory(Docker)
        add_subdirectory(LanguageServer)
        add_subdirectory(Rust)
        if(WITH_SFTP)
            add_subdirectory(SFTP)
            add_subdirectory(Remoty)
        endif(WITH_SFTP)
        add_subdirectory(DebugAdapterClient)
        add_subdirectory(codelitephp)
        add_subdirectory(WordCompletion)
        add_subdirectory(HelpPlugin)
        add_subdirectory(WebTools)
        add_subdirectory(SmartCompletion)

        if(UNIX
           AND NOT APPLE
           AND NOT DISABLE_CXX)
            # Add valgrind plugin
            add_subdirectory(MemCheck)
        endif()

        if(APPLE AND NOT DISABLE_CXX)
            message("-- Adding MacBundler...")
            add_subdirectory(MacBundler)
        endif()
        add_subdirectory(CallGraph)
    endif(NOT NO_CORE_PLUGINS)
    add_subdirectory(wxcrafter)

    # Executables
    if(NOT SDK_ONLY)
        add_subdirectory(LiteEditor)
        add_subdirectory(codelitegcc)
        add_subdirectory(codelite_make)
        if(NOT APPLE)
            # exclude codelite-terminal on macOS
            add_subdirectory(codelite_terminal)
        endif()
        if(MINGW)
            # codelite-exec
            add_subdirectory(le_exec)

            # makedirs
            add_subdirectory(TestDir)
        endif()
        add_subdirectory(sdk/codelite_cppcheck)
        add_subdirectory(codelite_echo)
        add_subdirectory(ctagsd)
    endif()
    #
    # Setup the proper dependencies
    #
    if(NOT SQLite3_FOUND)
        if(USE_PCH AND NOT MINGW)
            add_dependencies(sqlite3lib ${CL_PCH_TARGET})
        endif()
        add_dependencies(wxsqlite3 sqlite3lib)
    endif()

    add_dependencies(databaselayersqlite wxsqlite3)
    add_dependencies(wxshapeframework wxsqlite3)
    add_dependencies(libcodelite wxshapeframework databaselayersqlite wxsqlite3)
    add_dependencies(plugin libcodelite)

    if(NOT SDK_ONLY)
        add_dependencies(codelite plugin)
    endif()

    # Include our custom plugin.cmake module
    include(plugin)

    # Scan for user plugins
    cl_scan_for_plugins()
endif() # NOT WXC_APP

if(NOT WXC_APP)
    if(UNIX AND NOT APPLE)
        set_target_properties(ctags PROPERTIES OUTPUT_NAME "codelite-ctags")
        install(
            TARGETS ctags
            DESTINATION ${CL_PREFIX}/bin
            PERMISSIONS ${EXE_PERM})
    elseif(APPLE)
        set_target_properties(ctags PROPERTIES OUTPUT_NAME "codelite-ctags")
        install(
            TARGETS ctags
            DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS
            PERMISSIONS ${EXE_PERM})
        cl_install_name_tool_std(${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/codelite-ctags)
    endif()
endif()

if(MINGW)
    # determine MSYS2 path
    write_file(${CMAKE_BINARY_DIR}/msys2-environment "export WXWIN=${WXWIN}")
    write_file(${CMAKE_BINARY_DIR}/msys2-environment "export MSYS2_HOME=${MSYS2_BASE}" APPEND)

    # install DLLs required by MinGW

    # TODO: this part should be configurable / auto-detected
    list(
        APPEND
        SYSTEM_DLLS
        libc++.dll
        libcrypto-1_1-x64.dll
        libhunspell-1.7-0.dll
        libsqlite3-0.dll
        libssh.dll
        libunwind.dll
        libwinpthread-1.dll
        zlib1.dll)

    list(
        APPEND
        MSYS2_CORE_FILES
        msys-intl-8.dll
        msys-2.0.dll
        msys-iconv-2.dll
        rm.exe
        mkdir.exe
        cp.exe
        ls.exe
        mv.exe)

    list(APPEND WXWIDGETS_DLLS wxmsw330u_clang_custom.dll)

    list(APPEND CODELITE_RESOURCES ${CMAKE_SOURCE_DIR}/Runtime/codelite-bitmaps-dark.zip
         ${CMAKE_SOURCE_DIR}/Runtime/codelite-bitmaps-light.zip ${CMAKE_SOURCE_DIR}/wxcrafter/wxgui.zip
         ${CMAKE_SOURCE_DIR}/LICENSE)

    foreach(DLL ${SYSTEM_DLLS})
        install(FILES "${MSYS2_BASE}/clang64/bin/${DLL}" DESTINATION "${CMAKE_INSTALL_PREFIX}")
    endforeach()

    foreach(DLL ${MSYS2_CORE_FILES})
        install(FILES "${MSYS2_BASE}/usr/bin/${DLL}" DESTINATION "${CMAKE_INSTALL_PREFIX}")
    endforeach()

    foreach(DLL ${WXWIDGETS_DLLS})
        install(FILES "${WXWIN}/lib/clang_x64_dll/${DLL}" DESTINATION "${CMAKE_INSTALL_PREFIX}")
    endforeach()

    foreach(RESOURCE ${CODELITE_RESOURCES})
        install(FILES "${RESOURCE}" DESTINATION "${CMAKE_INSTALL_PREFIX}")
    endforeach()
endif()

if(MINGW AND NOT WXC_APP)
    # configure the innosetup file
    configure_file(${CL_SRC_ROOT}/InnoSetup/codelite64_mingw.iss.in ${BUILD_DIRECTORY}/codelite64_mingw.iss)

    # CodeLite only related installation
    find_program(
        INNO_SETUP_CMP
        NAMES "ISCC" "ISCC.exe"
        PATHS "c:/Program Files (x86)/Inno Setup 6")
    if(NOT INNO_SETUP_CMP)
        message(WARNING "\n** Could not locate ISCC compiler! \"setup\" target is not created **")
        message(WARNING "\n** You can install InnoSetup compiler from here: https://jrsoftware.org/ **\n")
    else()
        message(STATUS "InnoSetup compiler is set to: ${INNO_SETUP_CMP}")

        # fake target that executes "install". we then use this target as the dependency for the `setup` target
        add_custom_target(codelite-install COMMAND "${CMAKE_COMMAND}" --build "${BUILD_DIRECTORY}" --target install)

        add_custom_target(
            setup
            COMMAND "${INNO_SETUP_CMP}" ${BUILD_DIRECTORY}/codelite64_mingw.iss
            WORKING_DIRECTORY "${CL_SRC_ROOT}/InnoSetup"
            DEPENDS codelite-install)
    endif()
elseif(MINGW AND WXC_APP)
    # configure the innosetup file
    configure_file(${CL_SRC_ROOT}/wxcrafter/wxcrafter64_mingw.iss.in ${BUILD_DIRECTORY}/wxcrafter64_mingw.iss)

    # wxCrafter App
    find_program(
        INNO_SETUP_CMP
        NAMES "ISCC" "ISCC.exe"
        PATHS "c:/Program Files (x86)/Inno Setup 6")
    if(NOT INNO_SETUP_CMP)
        message(WARNING "** Could not locate ISCC compiler! **")
    else()
        message(STATUS "InnoSetup compiler is set to: ${INNO_SETUP_CMP}")
        add_custom_target(
            setup
            COMMAND "${INNO_SETUP_CMP}" ${BUILD_DIRECTORY}/wxcrafter64_mingw.iss
            WORKING_DIRECTORY "${CL_SRC_ROOT}/InnoSetup")
    endif()
endif()
