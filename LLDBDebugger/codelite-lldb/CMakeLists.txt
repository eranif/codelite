project(codelite-lldb)
if ( APPLE )
    ## Under Apple, we only support monolithic build of wxWidgets
    find_package(wxWidgets COMPONENTS std REQUIRED)
else ( APPLE )
    find_package(wxWidgets COMPONENTS std aui propgrid stc richtext ribbon REQUIRED)
endif ( APPLE )

FILE(GLOB SRC "*.cpp")
add_executable(codelite-lldb ${SRC})
target_link_libraries(  codelite-lldb
                        ${LINKER_OPTIONS}
                        ${LLDB_LIB}
                        ${wxWidgets_LIBRARIES}
                        -L"${CL_LIBPATH}"
                        -llibcodelite
                        -lplugin
                        -lwxsqlite3 
                        -lsqlite3lib
                    )
