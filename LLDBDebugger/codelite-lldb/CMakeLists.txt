project(codelite-lldb)
message("-- Processing target codelite-lldb")

if ( APPLE )
    ## Under Apple, we only support monolithic build of wxWidgets
    find_package(wxWidgets COMPONENTS std REQUIRED)
else ( APPLE )
    find_package(wxWidgets COMPONENTS std aui propgrid stc richtext ribbon REQUIRED)
endif ( APPLE )

# Add RPATH
if (UNIX)
set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")
endif (UNIX)

FILE(GLOB SRC "*.cpp")
include_directories(${CL_SRC_ROOT}/LLDBDebugger)
add_executable(codelite-lldb ${SRC})
link_directories(${LLDB_LIB_PATH})
target_link_libraries(codelite-lldb ${LIBLLDB})
target_link_libraries(  codelite-lldb
                        ${LINKER_OPTIONS}
                        ${wxWidgets_LIBRARIES}
                        -L"${CL_LIBPATH}"
                        -lLLDBProtocol
                        -llibcodelite
                        -lplugin
                        -lwxsqlite3 
                        -lsqlite3lib
                    )
target_link_libraries(codelite-lldb LLDBProtocol)
add_dependencies(codelite-lldb plugin)
install(TARGETS codelite-lldb DESTINATION ${CL_PREFIX}/bin)

if ( MAKE_DEB AND LLDB_OFFICIAL_FOUND MATCHES 1 )
    message("-- Adding lldb-3.5-dev to deb dependencies")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS  "${CPACK_DEBIAN_PACKAGE_DEPENDS} lldb-3.5-dev")
endif()
