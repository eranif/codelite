/**
 *
 * To generate C++/Header files run:
 * bison.exe -d -l -pjson_ -o clJSONObjectParser.cpp clJSONObjectParser.y
 */
%{
#define YYTOKENTYPE
#define YYSTYPE_IS_DECLARED

#define YYSTYPE char*

#include <wx/string.h>
#include "JSLexerTokens.h"
#include "JSLexerAPI.h" // Contains yygut definition
#include "JSLookUpTable.h"


void yyerror (void* yyscanner, char const *msg);

/* our parser will invoke this function json_lex
 * which is a wrapper to the real lex function: js_lex
 */
extern int json_lex(void* yyscanner);

/* the real lex function */
extern int js_lex(void* yyscanner);

/* consume tokens until found 'until' */
static void clParseJS_ReadUntil(void *scanner, int until);


struct clJSONParserData {
    JSLookUpTable::Ptr_t lookup;
    JSObject::Vec_t objStack;
};

static void clParseJS_FinalizeObject(clJSONParserData* parserData, const wxString& objType);
#define PARSER_DATA() static_cast<clJSONParserData*>(static_cast<JSLexerUserData*>(jsLexerGetUserData(scanner))->parserData)

%}

%lex-param {void* scanner}
%parse-param {void* scanner}

%token kJS_FUNCTION kJS_VAR kJS_DOT kJS_THIS
%token kJS_CATCH kJS_THROW kJS_SWITCH kJS_CASE
%token kJS_FOR kJS_STRING kJS_IDENTIFIER kJS_PROTOTYPE
%token kJS_RETURN kJS_NEW kJS_TRUE kJS_FALSE kJS_NULL
%token kJS_DEC_NUMBER kJS_OCTAL_NUMBER kJS_HEX_NUMBER
%token kJS_FLOAT_NUMBER kJS_PLUS_PLUS kJS_MINUS_MINUS
%token kJS_LS kJS_RS kJS_LE kJS_GE kJS_EQUAL
%token kJS_NOT_EQUAL kJS_AND_AND kJS_OR_OR kJS_STAR_EQUAL kJS_SLASH_EQUAL
%token kJS_DIV_EQUAL kJS_PLUS_EQUAL kJS_MINUS_EQUAL kJS_RS_ASSIGN kJS_AND_EQUAL
%token kJS_POW_EQUAL kJS_OR_EQUAL kJS_VOID kJS_TYPEOF kJS_DELETE kJS_INSTANCEOF 
%token kJS_EQUAL3 kJS_LS_ASSIGN

%start   program

%%

program:
    | literalObject {
        clJSONParserData *parserData = PARSER_DATA();
        clParseJS_FinalizeObject(parserData, parserData->lookup->GenerateNewType());
    }
    | literalArray {
        clJSONParserData *parserData = PARSER_DATA();
        clParseJS_FinalizeObject(parserData, "Array");
    }
    | number { 
        clJSONParserData *parserData = PARSER_DATA();
        clParseJS_FinalizeObject(parserData, "Number");
    }
    | kJS_STRING { 
        clJSONParserData *parserData = PARSER_DATA();
        clParseJS_FinalizeObject(parserData, "String");
    }
    | kJS_TRUE { 
        clJSONParserData *parserData = PARSER_DATA();
        clParseJS_FinalizeObject(parserData, "Boolean");
    }
    | kJS_FALSE { 
        clJSONParserData *parserData = PARSER_DATA();
        clParseJS_FinalizeObject(parserData, "Boolean");
    }
    ;

literalObject: '{' keyValues '}'
    ;
    
literalArray: '[' { clParseJS_ReadUntil(scanner, ']'); } ']'
    ;
    
propertyKey: kJS_IDENTIFIER {
        clJSONParserData *parserData = PARSER_DATA();
        JSObject::Ptr_t o = parserData->lookup->NewObject();
        o->SetName(::jsLexerCurrentToken(scanner));
        parserData->objStack.push_back(o);
    }
    | kJS_STRING {
        clJSONParserData *parserData = PARSER_DATA();
        JSObject::Ptr_t o = parserData->lookup->NewObject();
        wxString name = ::jsLexerCurrentToken(scanner);
        name.Remove(0, 1).RemoveLast();
        o->SetName(name);
        parserData->objStack.push_back(o);
    }
    ;

keyValues: keyValue
    | keyValues ',' keyValue
    ;

/** JSON object key: value **/
keyValue: /* empty */
    | propertyKey ':' number { 
        clJSONParserData *parserData = PARSER_DATA();
        clParseJS_FinalizeObject(parserData, "Number");
    }
    | propertyKey ':' literalObject {
        clJSONParserData *parserData = PARSER_DATA();
        clParseJS_FinalizeObject(parserData, parserData->lookup->GenerateNewType());
    }
    | propertyKey ':' literalArray {
        clJSONParserData *parserData = PARSER_DATA();
        clParseJS_FinalizeObject(parserData, "Array");
    }
    | propertyKey ':' kJS_STRING {
        clJSONParserData *parserData = PARSER_DATA();
        clParseJS_FinalizeObject(parserData, "String");
    }
    | propertyKey ':' kJS_NULL {
        clJSONParserData *parserData = PARSER_DATA();
        clParseJS_FinalizeObject(parserData, "null");
    }
    | propertyKey ':' kJS_TRUE {
        clJSONParserData *parserData = PARSER_DATA();
        clParseJS_FinalizeObject(parserData, "Boolean");
    }
    | propertyKey ':' kJS_FALSE {
        clJSONParserData *parserData = PARSER_DATA();
        clParseJS_FinalizeObject(parserData, "Boolean");
    }
    ;
    
number: kJS_DEC_NUMBER
    | kJS_HEX_NUMBER
    | kJS_OCTAL_NUMBER
    | kJS_FLOAT_NUMBER
    ;
    
%%

void yyerror (void* yyscanner, char const *msg) {}

/**
 * @brief a wrapper for the common used lexer
 */
int json_lex(void* yyscanner) { return js_lex(yyscanner); }

static void clParseJS_FinalizeObject(clJSONParserData* parserData, const wxString& objType)
{
    // take the current object
    JSObject::Ptr_t o = parserData->objStack.at(parserData->objStack.size() - 1);
    o->SetType(objType);
    o->SetPath(objType);
    
    if(parserData->objStack.size() > 1) {
        // we are done with this object, remove it from the stack
        parserData->objStack.pop_back();
        JSObject::Ptr_t parent = parserData->objStack.at(parserData->objStack.size() - 1);
        parent->AddProperty(o);
    }
}
//--------------------------
// Public API
//--------------------------
JSObject::Ptr_t clParseJSVariable(const wxString& content, JSLookUpTable::Ptr_t lookup)
{
    void* scanner = ::jsLexerNew(content);
    
    // set the parser data
    JSLexerUserData *lexerData = ::jsLexerGetUserData(scanner);
    clJSONParserData *parserData = new clJSONParserData;
    parserData->lookup = lookup;
    lexerData->parserData = parserData;
    
    // we start the parsing with an object in the stack
    JSObject::Ptr_t result = lookup->NewObject();
    result->SetUndefined();
    
    parserData->objStack.push_back(result);
    
    // parse
    json_parse(scanner);
    wxDELETE(parserData);
    
    // destroy the scanner
    ::jsLexerDestroy(&scanner);
    return result;
}

static void clParseJS_ReadUntil(void *scanner, int until)
{
    JSLexerToken token;
    int depth(0);
    while(true) {
        
        // EOF?
        int type = json_lex(scanner);
        if(type == 0) break;
        
        if(type == '{') {
            ++depth;
        } else if(type == '}') {
            --depth;
        }

        // breaking condition
        if((type == until) && (depth == 0)) {
            ::jsLexerUnget(scanner);
            break;
        }
    }
}
