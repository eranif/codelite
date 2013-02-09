# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(codelite)

###########################################
## Generate autoversion.cpp file
###########################################
ADD_CUSTOM_TARGET(GIT_REVISION COMMAND "${CL_SRC_ROOT}/git-revision.sh" WORKING_DIRECTORY "${CL_SRC_ROOT}"
                   COMMENT Generating LiteEditor/autoversion.cpp file)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CL_SRC_ROOT}/bin" )

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS std aui propgrid stc REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

# Include paths
include_directories("${CL_SRC_ROOT}/Plugin" "${CL_SRC_ROOT}/sdk/wxsqlite3/include" "${CL_SRC_ROOT}/CodeLite" "${CL_SRC_ROOT}/PCH" "${CL_SRC_ROOT}/Interfaces")

## On UNIX we require GTK
if (UNIX AND NOT APPLE)

    find_package(GTK2)
    if (GTK2_FOUND)
        include_directories("${GTK2_INCLUDE_DIRS}")
    else (GTK2_FOUND)
        message(FATAL_ERROR "Could not locate GTK.")
    endif (GTK2_FOUND)

endif (UNIX AND NOT APPLE)

add_definitions(-DWXUSINGDLL_WXSQLITE3)
add_definitions(-DWXUSINGDLL_CL)
add_definitions(-DWXUSINGDLL_SDK)

if ( USE_CLANG )
  add_definitions(-DHAS_LIBCLANG)
  include_directories(${CLANG_INCLUDE})
endif( USE_CLANG )

# Add RPATH
set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")

FILE(GLOB SRCS "*.cpp")

# Define the output
add_executable(codelite ${SRCS} "${CL_SRC_ROOT}/LiteEditor/autoversion.cpp")

if(GTK2_FOUND)
    target_link_libraries(codelite ${LINKER_OPTIONS} ${GTK2_LIBRARIES} ${wxWidgets_LIBRARIES} ${CLANG_LIBRARY} -L${CL_SRC_ROOT}/lib -lwxsqlite3 -lsqlite3 -llibcodelite -lplugin -lutil)
else (GTK2_FOUND)
    target_link_libraries(codelite ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES} ${CLANG_LIBRARY} -L${CL_SRC_ROOT}/lib -lwxsqlite3 -lsqlite3 -llibcodelite -lplugin -lutil)
endif(GTK2_FOUND)

##################################################################
## Install
##################################################################

## Copy binaries / scripts
set (EXE_PERM OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

install(TARGETS codelite DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
install(FILES ${CL_SRC_ROOT}/Runtime/codelite_exec DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
install(FILES ${CL_SRC_ROOT}/Runtime/codelite_fix_files DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
install(FILES ${CL_SRC_ROOT}/Runtime/codelite_kill_children DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})
install(FILES ${CL_SRC_ROOT}/Runtime/codelite_xterm DESTINATION ${CL_PREFIX}/bin PERMISSIONS ${EXE_PERM})

## codelite icons
install(FILES ${CL_SRC_ROOT}/Runtime/codelite-icons-fresh-farm.zip DESTINATION ${CL_PREFIX}/share/codelite)
install(FILES ${CL_SRC_ROOT}/Runtime/codelite-icons.zip DESTINATION ${CL_PREFIX}/share/codelite)

## License file
install(FILES ${CL_SRC_ROOT}/LICENSE DESTINATION ${CL_PREFIX}/share/codelite)

## Copy default configuration files
install(FILES ${CL_SRC_ROOT}/Runtime/config/accelerators.conf.default DESTINATION ${CL_PREFIX}/share/codelite/config)
install(FILES ${CL_SRC_ROOT}/Runtime/config/build_settings.xml.default DESTINATION ${CL_PREFIX}/share/codelite/config)
install(FILES ${CL_SRC_ROOT}/Runtime/config/debuggers.xml.gtk DESTINATION ${CL_PREFIX}/share/codelite/config RENAME debuggers.xml.default)
install(FILES ${CL_SRC_ROOT}/Runtime/config/codelite.xml.default.gtk DESTINATION ${CL_PREFIX}/share/codelite/config RENAME codelite.xml.default)
install(FILES ${CL_SRC_ROOT}/Runtime/codelite.desktop DESTINATION ${CL_PREFIX}/share/applications)

install(
  DIRECTORY ${CL_SRC_ROOT}/Runtime/plugins/resources
  DESTINATION ${PLUGINS_DIR}
  USE_SOURCE_PERMISSIONS
  PATTERN ".svn" EXCLUDE
  PATTERN ".git" EXCLUDE
) 

install(
  DIRECTORY ${CL_SRC_ROOT}/Runtime/images ${CL_SRC_ROOT}/Runtime/src/ ${CL_SRC_ROOT}/Runtime/lexers ${CL_SRC_ROOT}/Runtime/templates ${CL_SRC_ROOT}/Runtime/rc
  DESTINATION ${CL_PREFIX}/share/codelite
  USE_SOURCE_PERMISSIONS
  PATTERN ".svn" EXCLUDE
  PATTERN ".git" EXCLUDE
) 


if ( USE_CLANG )
  install(FILES ${CLANG_BINARY} DESTINATION ${PLUGINS_DIR})
endif( USE_CLANG )

add_dependencies(codelite GIT_REVISION)
