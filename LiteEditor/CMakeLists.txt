# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(codelite)

if ( UNIX AND NOT APPLE )
    execute_process(COMMAND sed s@%%PREFIX%%@${CL_PREFIX}@g Runtime/codelite.desktop.template WORKING_DIRECTORY "${CL_SRC_ROOT}" OUTPUT_FILE Runtime/codelite.desktop)
    message("-- Generating desktop file...")
endif (  UNIX AND NOT APPLE )

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS ${WX_COMPONENTS} REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

# Include paths
include_directories("${CL_SRC_ROOT}/Plugin" "${CL_SRC_ROOT}/sdk/wxsqlite3/include" "${CL_SRC_ROOT}/CodeLite" "${CL_SRC_ROOT}/PCH" "${CL_SRC_ROOT}/Interfaces")

## On UNIX we require GTK
if (UNIX AND NOT APPLE)

    if (GTK_VERSION EQUAL 3)
        set(OLD_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}) # Cache the current value
        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
        find_package(GTK3)
        set(CMAKE_MODULE_PATH ${OLD_CMAKE_MODULE_PATH}) # Reset, else the official path isn't used again :/
        if (GTK3_FOUND)
            include_directories(${GTK3_INCLUDE_DIRS})
        else (GTK3_FOUND)
            message(FATAL_ERROR "Could not locate GTK.")
        endif (GTK3_FOUND)
    else()
        find_package(GTK2)
        if (GTK2_FOUND)
            include_directories(${GTK2_INCLUDE_DIRS})
        else (GTK2_FOUND)
            message(FATAL_ERROR "Could not locate GTK.")
        endif (GTK2_FOUND)
    endif()

endif (UNIX AND NOT APPLE)
if (NOT MINGW)
add_definitions(-fPIC)
endif (NOT MINGW)

if ( USE_PCH )
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif ( USE_PCH )

add_definitions(-DWXUSINGDLL_WXSQLITE3)
add_definitions(-DWXUSINGDLL_CL)
add_definitions(-DWXUSINGDLL_SDK)

if ( USE_CLANG )
  add_definitions(-DHAS_LIBCLANG)
  include_directories(${CLANG_INCLUDE})
endif( USE_CLANG )

# Add RPATH
if (UNIX)
set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")
endif (UNIX)

FILE(GLOB SRCS "*.cpp")

# Define the output
if (MINGW)
    add_definitions(-DWINDRES)
    add_executable(codelite ${SRCS} code_parser.rc)
else()
    add_executable(codelite ${SRCS})
endif()

set( ADDITIONAL_LIBRARIES "" )

if (UNIX)
    if ( IS_FREEBSD )
        set(ADDITIONAL_LIBRARIES "-lkvm")
    elseif ( UNIX AND NOT APPLE )
        set(ADDITIONAL_LIBRARIES "-ldl -lutil -lgobject-2.0")
    else ( )
        set(ADDITIONAL_LIBRARIES "-ldl")
    endif ( )
else (UNIX)
    ## Windows??
endif (UNIX)

if(GTK2_FOUND)
    target_link_libraries(codelite ${LINKER_OPTIONS} ${GTK2_LIBRARIES} ${wxWidgets_LIBRARIES} ${CLANG_LIBRARY} -L"${CL_LIBPATH}" -lwxsqlite3 -lsqlite3lib -llibcodelite -lplugin ${ADDITIONAL_LIBRARIES})
elseif (GTK3_FOUND)
    target_link_libraries(codelite ${LINKER_OPTIONS} ${GTK3_LIBRARIES} ${wxWidgets_LIBRARIES} ${CLANG_LIBRARY} -L"${CL_LIBPATH}" -lwxsqlite3 -lsqlite3lib -llibcodelite -lplugin ${ADDITIONAL_LIBRARIES})
else()
    target_link_libraries(codelite ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES} ${CLANG_LIBRARY} -L"${CL_LIBPATH}" -lwxsqlite3 -lsqlite3lib -llibcodelite -lplugin ${ADDITIONAL_LIBRARIES})
endif()

##################################################################
## Install
##################################################################

## Copy binaries / scripts
set (EXE_PERM OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

install(TARGETS codelite DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
if (UNIX)
install(FILES ${CL_SRC_ROOT}/Runtime/codelite_exec DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
install(FILES ${CL_SRC_ROOT}/Runtime/codelite_fix_files DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
install(FILES ${CL_SRC_ROOT}/Runtime/codelite_kill_children DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
install(FILES ${CL_SRC_ROOT}/Runtime/codelite_xterm DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
install(FILES ${CL_SRC_ROOT}/Runtime/clg++ DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
install(FILES ${CL_SRC_ROOT}/Runtime/clgcc DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
endif (UNIX)

# Install clang-format
# We name it codelite-clang-format to avoid collision with any official installation
if ( UNIX AND NOT APPLE )
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        install(FILES ${CL_SRC_ROOT}/sdk/clang/Linux/x86_64/bin/codelite-clang-format DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
    else()
        install(FILES ${CL_SRC_ROOT}/sdk/clang/Linux/i386/bin/codelite-clang-format DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
    endif()
endif()

## codelite icons
install(FILES ${CL_SRC_ROOT}/Runtime/codelite-icons-fresh-farm.zip DESTINATION ${CL_PREFIX}/share/codelite)
install(FILES ${CL_SRC_ROOT}/Runtime/codelite-icons-dark.zip DESTINATION ${CL_PREFIX}/share/codelite)
install(FILES ${CL_SRC_ROOT}/Runtime/codelite-icons.zip DESTINATION ${CL_PREFIX}/share/codelite)

## License file
install(FILES ${CL_SRC_ROOT}/LICENSE DESTINATION ${CL_PREFIX}/share/codelite)

## Manual pages
install(DIRECTORY ${CL_SRC_ROOT}/Runtime/man1 DESTINATION ${CL_PREFIX}/share/man)

## Copy default configuration files
install(FILES ${CL_SRC_ROOT}/Runtime/config/accelerators.conf.default DESTINATION ${CL_PREFIX}/share/codelite/config)
install(FILES ${CL_SRC_ROOT}/Runtime/config/build_settings.xml.default DESTINATION ${CL_PREFIX}/share/codelite/config)
install(FILES ${CL_SRC_ROOT}/Runtime/config/debuggers.xml.gtk DESTINATION ${CL_PREFIX}/share/codelite/config RENAME debuggers.xml.default)
install(FILES ${CL_SRC_ROOT}/Runtime/config/codelite.xml.default.gtk DESTINATION ${CL_PREFIX}/share/codelite/config RENAME codelite.xml.default)
if ( UNIX AND NOT APPLE )
install(FILES ${CL_SRC_ROOT}/Runtime/codelite.desktop DESTINATION ${CL_PREFIX}/share/applications)
endif (  UNIX AND NOT APPLE )

install(
  DIRECTORY ${CL_SRC_ROOT}/Runtime/plugins/resources
  DESTINATION ${PLUGINS_DIR}
  USE_SOURCE_PERMISSIONS
  PATTERN ".svn" EXCLUDE
  PATTERN ".git" EXCLUDE
) 

install(
  DIRECTORY ${CL_SRC_ROOT}/Runtime/images ${CL_SRC_ROOT}/Runtime/gdb_printers ${CL_SRC_ROOT}/Runtime/src/ ${CL_SRC_ROOT}/Runtime/lexers ${CL_SRC_ROOT}/Runtime/templates ${CL_SRC_ROOT}/Runtime/rc
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
        set(WX_LIBS_DIR "/usr/lib") # ${WX_LIBS_DIR} will be empty for older, non multi-architecture, distro versions that still use /usr/lib
    endif()
    message("-- wxlibsdir is: ${WX_LIBS_DIR}")

    # Grab the necessary libs
    execute_process(COMMAND ${WX_TOOL} --libs all OUTPUT_VARIABLE WX_LIBS_ALL_IN OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REPLACE " " ";" WX_LIBS_ALL_LIST ${WX_LIBS_ALL_IN}) # the semicolons make the variable into a list

    foreach(ITEM ${WX_LIBS_ALL_LIST})
        # We want to replace -l with lib. We also want to exclude imaginary libs called _all.so, which buggy wx-config scripts can invent
        if (${ITEM} MATCHES "-lwx_.+" AND NOT ${ITEM} MATCHES "-l.+_all.+")
            string(REGEX REPLACE "^-l" "lib" LIBITEM ${ITEM})
            list(APPEND WX_LIBS_ALL ${WX_LIBS_DIR}/${LIBITEM}.so*) # we need the terminal [.]so because the rpms contain both dynamic and static libs
        endif()
    endforeach()
    
    file(GLOB wxfilepaths ${WX_LIBS_ALL})
    install(FILES ${wxfilepaths} DESTINATION ${CL_PREFIX}/${CL_INSTALL_LIBDIR}/codelite)
endif()

if ( USE_CLANG )
  install(FILES ${CLANG_BINARY} DESTINATION ${PLUGINS_DIR})
endif( USE_CLANG )
