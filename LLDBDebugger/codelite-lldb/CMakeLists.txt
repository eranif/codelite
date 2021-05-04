project(codelite-lldb)
message("-- Processing target codelite-lldb")

if ( APPLE )
    ## Under Apple, we only support monolithic build of wxWidgets
    
else ( APPLE )
    
endif ( APPLE )

FILE(GLOB SRC "*.cpp")
include_directories(${CL_SRC_ROOT}/LLDBDebugger)

# codelite-lldb
add_executable(codelite-lldb ${SRC})
link_directories(${LLDB_LIB_PATH})
target_link_libraries(codelite-lldb ${LIBLLDB})
target_link_libraries(  codelite-lldb
                        ${LINKER_OPTIONS}
                        ${wxWidgets_LIBRARIES}
                        LLDBProtocol
                        libcodelite
                        plugin
                    )

CL_INSTALL_EXECUTABLE(codelite-lldb)
