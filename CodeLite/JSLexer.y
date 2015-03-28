/**
 *
 * To generate C++ file, run this:
 * bison.exe -l -pjs_ -o JSParser.cpp JSLexer.y
 */
%{
    #include <wx/string.h>
    #include "JSLexerTokens.h"
    #define YYSTYPE char*
    #define YYTOKENTYPE
    typedef void* JSScanner_t;
    void yyerror (JSScanner_t yyscanner, char const *msg);
    extern int js_lex(JSScanner_t yyscanner);
%}

%lex-param {JSScanner_t scanner}
%parse-param {JSScanner_t scanner}

%token kJS_FUNCTION kJS_VAR kJS_DOT kJS_THIS
%token kJS_CATCH kJS_THROW kJS_SWITCH kJS_CASE
%token kJS_FOR kJS_STRING kJS_IDENTIFIER kJS_PROTOTYPE
%token kJS_RETURN kJS_NEW kJS_TRUE kJS_FALSE kJS_NULL
%token kJS_DEC_NUMBER kJS_OCTAL_NUMBER kJS_HEX_NUMBER
%token kJS_FLOAT_NUMBER kJS_PLUS_PLUS kJS_MINUS_MINUS
%token kJS_LS kJS_RS kJS_LE kJS_GE kJS_EQUAL
%token kJS_NOT_EQUAL kJS_AND_AND kJS_OR_OR kJS_STAR_EQUAL kJS_SLASH_EQUAL
%token kJS_DIV_EQUAL kJS_PLUS_EQUAL kJS_MINUS_EQUAL kJS_RS_ASSIGN kJS_AND_EQUAL
%token kJS_POW_EQUAL kJS_OR_EQUAL kJS_VOID kJS_TYPEOF

%%

program:
    program statement '\n'
    | /* NULL */
    ;

statement: kJS_FUNCTION kJS_IDENTIFIER '(' argumentList ')' '{' statements '}'
    | variableDecl
    ;

statements: /* empty */
    | variableDecl
    ;

variableDecl: kJS_VAR kJS_IDENTIFIER '=' varAssignment
    ;

varAssignment: kJS_NEW kJS_IDENTIFIER ctorArgumentList ';'
    | literalObject
    | number
    | kJS_STRING
    | kJS_IDENTIFIER
    ;

ctorArgumentList: /* empty */
    | '(' argumentList ')'
    ;

literalObject: '{' keyValues '}'
    ;

propertyKey: kJS_IDENTIFIER
    | kJS_STRING
    ;

/** JSON object key: value **/
keyValue: /* empty */
    | propertyKey ':' number
    | propertyKey ':' literalObject
    | propertyKey ':' kJS_STRING
    | propertyKey ':' kJS_NULL
    | propertyKey ':' kJS_TRUE
    | propertyKey ':' kJS_FALSE
    ;

keyValues: keyValue
    | keyValues ',' keyValue
    ;
    
number: kJS_DEC_NUMBER
    | kJS_HEX_NUMBER
    | kJS_OCTAL_NUMBER
    | kJS_FLOAT_NUMBER
    ;
    
argumentList: /* empty */
    | kJS_IDENTIFIER
    | argumentList ',' kJS_IDENTIFIER
    ;

%%
void yyerror (JSScanner_t yyscanner, char const *msg) {}