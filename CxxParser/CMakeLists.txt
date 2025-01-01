project(CxxParser)

# Bison files
bison_target(
    CppScopeGrammarYacc "${CMAKE_SOURCE_DIR}/CxxParser/cpp_scope_grammar.y"
    "${CMAKE_CURRENT_BINARY_DIR}/cpp_scope_grammar.cpp"
    DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/cpp_lexer.h"
    COMPILE_FLAGS "-p cl_scope_ --no-lines")
bison_target(TypedefGrammarYacc "${CMAKE_SOURCE_DIR}/CxxParser/typedef_grammar.y"
             "${CMAKE_CURRENT_BINARY_DIR}/typedef_parser.cpp" COMPILE_FLAGS "-p cl_typedef_ --no-lines")
bison_target(CppVariablesGrammarYacc "${CMAKE_SOURCE_DIR}/CxxParser/cpp_variables_grammar.y"
             "${CMAKE_CURRENT_BINARY_DIR}/var_parser.cpp" COMPILE_FLAGS "-p cl_var_ --no-lines")
bison_target(ExprGrammarYacc "${CMAKE_SOURCE_DIR}/CxxParser/expr_grammar.y"
             "${CMAKE_CURRENT_BINARY_DIR}/cpp_expr_parser.cpp" COMPILE_FLAGS "-p cl_expr_ --no-lines")
bison_target(CppFuncParserYacc "${CMAKE_SOURCE_DIR}/CxxParser/cpp_func_parser.y"
             "${CMAKE_CURRENT_BINARY_DIR}/cpp_func_parser.cpp" COMPILE_FLAGS "-p cl_func_ --no-lines")

set(BisonSrcs
    "${BISON_CppScopeGrammarYacc_OUTPUTS}" "${BISON_TypedefGrammarYacc_OUTPUTS}"
    "${BISON_CppVariablesGrammarYacc_OUTPUTS}" "${BISON_ExprGrammarYacc_OUTPUTS}" "${BISON_CppFuncParserYacc_OUTPUTS}")

# Flex files
flex_target(CppFlex "${CMAKE_SOURCE_DIR}/CxxParser/cpp.l" "${CMAKE_CURRENT_BINARY_DIR}/cpp.cpp"
            COMPILE_FLAGS "-Pcl_scope_ --noline --yylineno --batch")
# Increase YY_BUF_SIZE from 16384 to 16384*5
set_source_files_properties("${FLEX_CppFlex_OUTPUTS}" PROPERTIES COMPILE_DEFINITIONS "YY_BUF_SIZE=(16384*5)")
flex_target(ExprLexerFlex "${CMAKE_SOURCE_DIR}/CxxParser/expr_lexer.l" "${CMAKE_CURRENT_BINARY_DIR}/expr_lexer.cpp"
            COMPILE_FLAGS "-Pcl_expr_ --noline --yylineno --batch")
# Increase YY_BUF_SIZE from 16384 to 16384*5
set_source_files_properties("${FLEX_ExprLexerFlex_OUTPUTS}" PROPERTIES COMPILE_DEFINITIONS "YY_BUF_SIZE=(16384*5)")

add_flex_bison_dependency(CppFlex CppScopeGrammarYacc)
add_flex_bison_dependency(ExprLexerFlex CppScopeGrammarYacc)

set(FlexSrcs "${FLEX_CppFlex_OUTPUTS}" "${FLEX_ExprLexerFlex_OUTPUTS}")

# Define the outputs
add_library(
    libCxxParser STATIC
    cl_typedef.h
    code_completion_api.h
    expression_result.cpp
    expression_result.h
    function.cpp
    function.h
    variable.cpp
    variable.h
    ${FlexSrcs}
    ${BisonSrcs})

if(UNIX OR APPLE)
    set_target_properties(libCxxParser PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Include paths
target_include_directories(
    libCxxParser
    PUBLIC "${CL_SRC_ROOT}/CxxParser"
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
if(WIN32)
    target_compile_definitions(libCxxParser PRIVATE WXMAKINGDLL_CL)
endif()

# "testing" application
add_executable(CxxParser main.cpp)

target_link_libraries(CxxParser PRIVATE libCxxParser)
