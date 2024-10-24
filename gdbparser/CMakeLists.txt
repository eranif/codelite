project(gdbparser)

# Bison files
BISON_TARGET(GdbResultParserYacc "${CMAKE_SOURCE_DIR}/gdbparser/gdb_result.y" "${CMAKE_CURRENT_BINARY_DIR}/gdb_result_parser.cpp"
            DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/gdb_result_parser.h"
            COMPILE_FLAGS "-p gdb_result_ --no-lines")

set(BisonSrcs "${BISON_GdbResultParserYacc_OUTPUTS}")

# Flex files
FLEX_TARGET(GdbResultFlex "${CMAKE_SOURCE_DIR}/gdbparser/gdb_result.l" "${CMAKE_CURRENT_BINARY_DIR}/gdbresult.cpp"
            COMPILE_FLAGS "-Pgdb_result_ --noline --yylineno --batch")

ADD_FLEX_BISON_DEPENDENCY(GdbResultFlex GdbResultParserYacc)

set(FlexSrcs "${FLEX_GdbResultFlex_OUTPUTS}")

# Define the outputs
add_library(libgdbparser STATIC
    gdb_parser_incl.h
    ${FlexSrcs}
    ${BisonSrcs}
)

if(UNIX OR APPLE)
    set_target_properties(libgdbparser PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Include paths
target_include_directories(libgdbparser
    PUBLIC "${CL_SRC_ROOT}/gdbparser" "${CMAKE_CURRENT_BINARY_DIR}"
)

include(CTest)
if(BUILD_TESTING)

    # "testing" application
    add_executable(gdbparser
        main.cpp
    )
    target_link_libraries(gdbparser PRIVATE libgdbparser)

    add_test(NAME "gdbparser" COMMAND gdbparser)

endif(BUILD_TESTING)
