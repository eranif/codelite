project(CxxParser)

# Bison files
BISON_TARGET(CppScopeGrammarYacc "${CMAKE_SOURCE_DIR}/CxxParser/cpp_scope_grammar.y" "${CMAKE_CURRENT_BINARY_DIR}/cpp_scope_grammar.cpp"
            DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/cpp_lexer.h"
            COMPILE_FLAGS "-p cl_scope_ --no-lines")
BISON_TARGET(TypedefGrammarYacc "${CMAKE_SOURCE_DIR}/CxxParser/typedef_grammar.y" "${CMAKE_CURRENT_BINARY_DIR}/typedef_parser.cpp"
            COMPILE_FLAGS "-p cl_typedef_ --no-lines")
BISON_TARGET(CppVariablesGrammarYacc "${CMAKE_SOURCE_DIR}/CxxParser/cpp_variables_grammar.y" "${CMAKE_CURRENT_BINARY_DIR}/var_parser.cpp"
            COMPILE_FLAGS "-p cl_var_ --no-lines")
BISON_TARGET(ExprGrammarYacc "${CMAKE_SOURCE_DIR}/CxxParser/expr_grammar.y" "${CMAKE_CURRENT_BINARY_DIR}/cpp_expr_parser.cpp"
            COMPILE_FLAGS "-p cl_expr_ --no-lines")
BISON_TARGET(CppFuncParserYacc "${CMAKE_SOURCE_DIR}/CxxParser/cpp_func_parser.y" "${CMAKE_CURRENT_BINARY_DIR}/cpp_func_parser.cpp"
            COMPILE_FLAGS "-p cl_func_ --no-lines")

set(BisonSrcs
    "${BISON_CppScopeGrammarYacc_OUTPUTS}"
    "${BISON_TypedefGrammarYacc_OUTPUTS}"
    "${BISON_CppVariablesGrammarYacc_OUTPUTS}"
    "${BISON_ExprGrammarYacc_OUTPUTS}"
    "${BISON_CppFuncParserYacc_OUTPUTS}"
)

# Flex files
FLEX_TARGET(CppFlex "${CMAKE_SOURCE_DIR}/CxxParser/cpp.l" "${CMAKE_CURRENT_BINARY_DIR}/cpp.cpp"
            COMPILE_FLAGS "-Pcl_scope_ --noline --yylineno --batch")
# Increase YY_BUF_SIZE from 16384 to 16384*5
set_source_files_properties("${FLEX_CppFlex_OUTPUTS}" PROPERTIES COMPILE_DEFINITIONS "YY_BUF_SIZE=(16384*5)")
FLEX_TARGET(ExprLexerFlex "${CMAKE_SOURCE_DIR}/CxxParser/expr_lexer.l" "${CMAKE_CURRENT_BINARY_DIR}/expr_lexer.cpp"
            COMPILE_FLAGS "-Pcl_expr_ --noline --yylineno --batch")
# Increase YY_BUF_SIZE from 16384 to 16384*5
set_source_files_properties("${FLEX_ExprLexerFlex_OUTPUTS}" PROPERTIES COMPILE_DEFINITIONS "YY_BUF_SIZE=(16384*5)")

ADD_FLEX_BISON_DEPENDENCY(CppFlex CppScopeGrammarYacc)
ADD_FLEX_BISON_DEPENDENCY(ExprLexerFlex CppScopeGrammarYacc)

set(FlexSrcs "${FLEX_CppFlex_OUTPUTS}" "${FLEX_ExprLexerFlex_OUTPUTS}")

# Define the outputs
add_library(libCxxParser STATIC
    cl_typedef.h
    code_completion_api.h
    expression_result.cpp
    expression_result.h
    function.cpp
    function.h
    variable.cpp
    variable.h
    ${FlexSrcs}
    ${BisonSrcs}
)

if(UNIX AND NOT APPLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

if(APPLE)
    add_definitions(-fPIC)
endif()

# Include paths
target_include_directories(libCxxParser
    PUBLIC "${CL_SRC_ROOT}/CxxParser"
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
)

# "testing" application
add_executable(CxxParser
    main.cpp
)

target_link_libraries(CxxParser PRIVATE libCxxParser)
