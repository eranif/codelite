project(codelite-lldb-tester)
if ( APPLE )
    ## Under Apple, we only support monolithic build of wxWidgets
    find_package(wxWidgets COMPONENTS std REQUIRED)
else ( APPLE )
    find_package(wxWidgets COMPONENTS std aui propgrid stc richtext ribbon REQUIRED)
endif ( APPLE )

FILE(GLOB SRC "*.cpp")
include_directories(${CL_SRC_ROOT}/LLDBDebugger)
add_executable(codelite-lldb-tester ${SRC})
target_link_libraries(  codelite-lldb-tester
                        ${LINKER_OPTIONS}
                        -L"${CL_LIBPATH}"
                        -lLLDBProtocol
                        ${LLDB_LIB}
                        ${wxWidgets_LIBRARIES}
                        -llibcodelite
                        -lplugin
                        -lwxsqlite3 
                        -lsqlite3lib
                    )
target_link_libraries(codelite-lldb-tester LLDBProtocol)
add_dependencies(codelite-lldb-tester plugin)

