%{
// Copyright Eran Ifrah(c)
%}

%{
/*************** Includes and Defines *****************************/
#include <string>
#include <vector>
#include <stdio.h>
#include <map>
#include <string.h>

#define YYDEBUG_LEXER_TEXT (cl_scope_lval)
#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/
static std::string readInitializer(const char* delim);
static void readClassName();

static std::string className;

static std::string templateInitList;
int cl_scope_parse();
void cl_scope_error(char *string);
void syncParser();

static std::vector<std::string> gs_additionlNS;

//---------------------------------------------
// externs defined in the lexer
//---------------------------------------------
extern char *cl_scope_text;
extern int cl_scope_lex();
extern bool setLexerInput(const std::string &in, const std::map<std::string, std::string> &ignoreTokens);
extern int cl_scope_lineno;
extern std::vector<std::string> currentScope;
extern void printScopeName();	//print the current scope name
extern void increaseScope();	//increase scope with anonymouse value
extern std::string getCurrentScope();
extern void cl_scope_lex_clean();
extern void cl_scope_less(int count);

/*************** Standard ytab.c continues here *********************/
%}

/*************************************************************************/

/* This group is used by the C/C++ language parser */
%token  LE_AUTO            LE_DOUBLE          LE_INT             LE_STRUCT
%token  LE_BREAK           LE_ELSE            LE_LONG            LE_SWITCH
%token  LE_CASE            LE_ENUM            LE_REGISTER        LE_TYPEDEF
%token  LE_CHAR            LE_EXTERN          LE_RETURN          LE_UNION
%token  LE_CONST           LE_FLOAT           LE_SHORT           LE_UNSIGNED      LE_BOOL
%token  LE_CONTINUE        LE_FOR             LE_SIGNED          LE_VOID
%token  LE_DEFAULT         LE_GOTO            LE_SIZEOF          LE_VOLATILE
%token  LE_DO              LE_IF              LE_STATIC          LE_WHILE

/* The following are used in C++ only.  ANSI C would call these IDENTIFIERs */
%token  LE_NEW             LE_DELETE
%token  LE_THIS
%token  LE_OPERATOR
%token  LE_CLASS
%token  LE_PUBLIC          LE_PROTECTED       LE_PRIVATE
%token  LE_VIRTUAL         LE_FRIEND
%token  LE_INLINE          LE_OVERLOAD LE_OVERRIDE LE_FINAL
%token  LE_TEMPLATE		   LE_TYPENAME
%token  LE_THROW		  	LE_CATCH
/* ANSI C Grammar suggestions */
%token  LE_IDENTIFIER              LE_STRINGliteral
%token  LE_FLOATINGconstant        LE_INTEGERconstant        LE_CHARACTERconstant
%token  LE_OCTALconstant           LE_HEXconstant
%token  LE_POUNDPOUND LE_CComment LE_CPPComment LE_NAMESPACE LE_USING

/* New Lexical element, whereas ANSI C suggested non-terminal */
%token  LE_TYPEDEFname

/* Multi-Character operators */
%token   LE_ARROW            											/*    ->                              */
%token   LE_ICR LE_DECR         										/*    ++      --                      */
%token   LE_LS LE_RS            										/*    <<      >>                      */
%token   LE_LE LE_GE LE_EQ LE_NE      								/*    <=      >=      ==      !=      */
%token   LE_ANDAND LE_OROR      										/*    &&      ||                      */
%token   LE_ELLIPSIS         											/*    ...                             */
            /* Following are used in C++, not ANSI C        */
%token   LE_CLCL             											/*    ::                              */
%token   LE_DOTstar LE_ARROWstar										/*    .*       ->*                    */

/* modifying assignment operators */
%token  LE_MULTassign  LE_DIVassign    LE_MODassign   	/*   *=      /=      %=      */
%token  LE_PLUSassign  LE_MINUSassign              		/*   +=      -=              */
%token  LE_LSassign    LE_RSassign                 		/*   <<=     >>=             */
%token  LE_ANDassign   LE_ERassign     LE_ORassign    	/*   &=      ^=      |=      */
%token  LE_MACRO
%token  LE_DYNAMIC_CAST
%token  LE_STATIC_CAST
%token  LE_CONST_CAST
%token  LE_REINTERPRET_CAST
%token  LE_SIZE_T
%token  LE_TIME_T

%token LE_DECLSPEC
%token LE_DLLIMPORT
%token LE_DLLIEXPORT

%start   translation_unit

%%
/* Costants */
basic_type_name_inter:    LE_INT          { $$ = $1; }
                |         LE_CHAR         { $$ = $1; }
                |         LE_SHORT        { $$ = $1; }
                |         LE_LONG         { $$ = $1; }
                |         LE_FLOAT        { $$ = $1; }
                |         LE_DOUBLE       { $$ = $1; }
                |         LE_SIGNED       { $$ = $1; }
                |         LE_UNSIGNED     { $$ = $1; }
                |         LE_VOID         { $$ = $1; }
                |         LE_BOOL         { $$ = $1; }
                |         LE_SIZE_T       { $$ = $1; }
                ;

basic_type_name:	LE_UNSIGNED basic_type_name_inter     { $$ = $1 + " " + $2; }
                |	LE_SIGNED basic_type_name_inter     { $$ = $1 + " " + $2; }
                |	LE_LONG LE_LONG                     { $$ = $1 + " " + $2; }
                |	LE_LONG LE_INT                         { $$ = $1 + " " + $2; }
                |	basic_type_name_inter                   { $$ = $1; }
                ;


/* ========================================================================*/
/* find declarations																   */
/* ========================================================================*/

translation_unit	:		/*empty*/
                        | translation_unit external_decl
                        ;

external_decl			:	class_decl
                        |   dummy_case
                        |	enum_decl
                        |	union_decl
                        | 	function_decl
                        |	namespace_decl
                        |	using_namespace
                        | 	scope_reducer
                        | 	scope_increaer
                        |  	question_expression
                        | 	error {
                                //printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_scope_text, cl_scope_lineno);
                                //syncParser();
                            }
                        ;


/*templates*/
template_arg		:	/* empty */	{ $$ = "";}
                        | template_specifiter LE_IDENTIFIER {$$ = $1 + " " + $2;}
                        | template_specifiter LE_IDENTIFIER '=' {$$ = $1 + " " + $2 + "=" + readInitializer(",>");}
                        ;

template_arg_list	:	template_arg	{ $$ = $1; }
                        | 	template_arg_list ',' template_arg	{ $$ = $1 + " " + $2 + " " + $3; }
                        ;

template_specifiter	:	LE_CLASS	{ $$ = $1; }
                            |	LE_TYPENAME	{ $$ = $1; }
                            ;

opt_template_qualifier	: /*empty*/
                            | LE_TEMPLATE '<' template_arg_list '>' { 
                                $$ = $1 + $2 + $3 + $4;
                            }
                            ;
///*inheritance*/
//derivation_list			:	/*empty*/ {$$ = "";}
//							|	parent_class {$$ = $1;}
//							| 	derivation_list ',' parent_class {$$ = $1 + $2 + $3;}
//							;

//parent_class				: 	access_specifier class_name opt_template_specifier {$$ = $1 + " " + $2 + $3;}
                            ;
//class_name                  : LE_IDENTIFIER                    {$$ = $1;}
//							| class_name LE_CLCL LE_IDENTIFIER {$$ = $1 + $2 + $3;}
                            ;

//opt_template_specifier	: /*empty*/	{$$ = "";}
//							| '<' template_parameter_list '>' {$$ = $1 + $2 + $3;}
//							;

//access_specifier			:	/*empty*/	{$$ = "";}
//							|	LE_PUBLIC {$$ = $1;}
//							| 	LE_PRIVATE {$$ = $1;}
//							| 	LE_PROTECTED {$$ = $1;}
                            ;

/* the following rules are for template parameters no declarations! */
template_parameter_list	: /* empty */		{$$ = "";}
                            | template_parameter	{$$ = $1;}
                            | template_parameter_list ',' template_parameter {$$ = $1 + $2 + $3;}
                            ;

/*template_parameter		:	const_spec nested_scope_specifier LE_IDENTIFIER special_star_amp {$$ = $1 + $2 + $3 +$4;}
                            ;*/
template_parameter	:	const_spec nested_scope_specifier LE_IDENTIFIER special_star_amp
                        {
                            $$ = $1 +  $2 + $3 +$4;
                        }
                    |  	const_spec nested_scope_specifier basic_type_name special_star_amp
                        {
                            $$ = $1 +  $2 + $3 +$4;
                        }
                    |  	const_spec nested_scope_specifier LE_IDENTIFIER '<' template_parameter_list '>' special_star_amp
                        {
                            $$ = $1 + $2 + $3 +$4 + $5 + $6 + $7 + " " ;
                        }
                        ;

using_namespace:	LE_USING LE_NAMESPACE nested_scope_specifier LE_IDENTIFIER ';'
                    {
                        gs_additionlNS.push_back($3+$4);
                    }
                ;

/* namespace */
namespace_decl	:	stmnt_starter LE_NAMESPACE nested_scope_specifier LE_IDENTIFIER '{'
                        {
                            currentScope.push_back($3+$4);
                            
                        }
                    |	stmnt_starter LE_NAMESPACE '{'
                        {
                            //anonymouse namespace
                            increaseScope();
                            
                        }
                    ;
/* the class rule itself */
class_decl	:	stmnt_starter opt_template_qualifier class_keyword
                {
                    readClassName();
                    //increase the scope level
                    if(className.empty() == false) {
                        currentScope.push_back( className );
                        printScopeName();
                    }
                }
                ;

scope_reducer		:	'}'	{
                                if(currentScope.empty())
                                {
                                    //fatal error!
                                    //printf("CodeLite: fatal error - cant go beyond global scope!\n");
                                }
                                else
                                {
                                    currentScope.pop_back();
                                    printScopeName();
                                }
                            }
                    ;
scope_increaer	:	'{' {
                                //increase random scope
                                increaseScope();
                                printScopeName();
                             }

question_expression : '?'
                        {
                            consumeNotIncluding(';');
                        }

class_keyword: 	LE_CLASS	{$$ = $1;}
             |	LE_STRUCT	{$$ = $1;}
             ;

func_name: LE_IDENTIFIER {$$ = $1;}
         | LE_OPERATOR any_operator {$$ = $1 + $2;}
         ;

any_operator:
        '+'
        | '='
        | '*'
        | '/'
        | '%'
        | '^'
        | '&'
        | '|'
        | '~'
        | '!'
        | '<'
        | '>'
        | LE_LS
        | LE_RS
        | LE_ANDAND
        | LE_OROR
        | LE_ARROW
        | LE_ARROWstar
        | '.'
        | LE_DOTstar
        | LE_ICR
        | LE_DECR
        | LE_LE
        | LE_GE
        | LE_EQ
        | LE_NE
        | '(' ')' {$$ = $1 + $2;}
        | '[' ']' {$$ = $1 + $2;}
        | LE_NEW
        | LE_DELETE
        | ','
        | LE_MULTassign
        | LE_DIVassign
        | LE_MODassign
        | LE_PLUSassign
        | LE_MINUSassign
        | LE_LSassign
        | LE_RSassign
        | LE_ANDassign
        | LE_ERassign
        | LE_ORassign
        ;

optional_initialization_list: '{' {$$ = '{';}/* empty */
        | ':' {consumeInitializationList() /*eat everything including the open brace*/;}
        ;

declare_throw: 	/*empty*/ {$$ = "";}
            |	LE_THROW '(' template_parameter_list ')' {$$ = $3;}
            ;

dummy_case : LE_CASE LE_IDENTIFIER 
{
    // eat up all tokens not including the ':'
    consumeNotIncluding(':');
}
;
/* functions */
function_decl	: 	stmnt_starter opt_template_qualifier virtual_spec const_spec variable_decl nested_scope_specifier func_name '(' {consumeFuncArgList();} const_spec declare_throw '{'
                    {
                        //printf("found function %s\n", $7.c_str());
                        //trim down trailing '::' from scope name
                        if($6.find_last_not_of(":") != std::string::npos){
                            $6.erase($6.find_last_not_of(":")+1);
                        }
                        currentScope.push_back($6);
                        printScopeName();
                    }
                |	stmnt_starter opt_template_qualifier virtual_spec const_spec nested_scope_specifier func_name '(' {consumeFuncArgList();}  optional_initialization_list
                    {

                        //trim down trailing '::' from scope name
                        if($5.find_last_not_of(":") != std::string::npos){
                            $5.erase($5.find_last_not_of(":")+1);
                        }
                        currentScope.push_back($5);
                        printScopeName();
                    }
                |	stmnt_starter opt_template_qualifier virtual_spec const_spec nested_scope_specifier '~' func_name '(' {consumeFuncArgList();} const_spec  '{'
                    {

                        //trim down trailing '::' from scope name
                        if($5.find_last_not_of(":") != std::string::npos){
                            $5.erase($5.find_last_not_of(":")+1);
                        }
                        currentScope.push_back($5);
                        printScopeName();
                    }
                ;

/*
applicable for C++, for cases where a function is declared as
void scope::foo(){ ... }
*/
nested_scope_specifier		: /*empty*/ {$$ = "";}
                            | nested_scope_specifier scope_specifier {	$$ = $1 + $2;}
                            ;

scope_specifier		:	LE_IDENTIFIER LE_CLCL {$$ = $1+ $2;}
                        |	LE_IDENTIFIER  '<' {consumeTemplateDecl();} LE_CLCL {$$ = $1 + $4;}
                        ;

virtual_spec		:	/* empty */	{$$ = ""; }
                        | 	LE_VIRTUAL 	{ $$ = $1; }
                        ;

const_spec			:	/* empty */	{$$ = ""; }
                        | 	LE_CONST 	{ $$ = $1; }
                        ;

amp_item				:	/*empty*/	{$$ = ""; }
                        |   '&' 			{ $$ = $1; }
                        ;

star_list			: 	/*empty*/		{$$ = ""; }
                        |	star_list '*'	{$$ = $1 + $2;}
                        ;

special_star_amp		:	star_list amp_item { $$ = $1 + $2; }
                        ;

stmnt_starter		:	/*empty*/ {$$ = "";}
                        | ';' { $$ = ";";}
                        | ':' { $$ = ":";}	//e.g. private: std::string m_name;
                        ;

/** Variables **/
variable_decl			:	nested_scope_specifier basic_type_name special_star_amp
                            {$$ = $1 + $2 + $3  ;}
                        |	nested_scope_specifier LE_IDENTIFIER special_star_amp
                            {$$ = $1 + $2 + $3  ;}
                        | 	nested_scope_specifier LE_IDENTIFIER '<' template_parameter_list '>' special_star_amp
                            {$$ = $1 + $2 + $3  + $4 + $5 + $6 ;}
                        ;

enum_decl				:	stmnt_starter LE_ENUM LE_IDENTIFIER '{' {currentScope.push_back($3); printScopeName();} enum_arg_list '}'
                        {
                            currentScope.pop_back();//reduce the scope
                            printScopeName();
                            //printf("found enum: %s, args are: %s\n", $2.c_str(), $5.c_str());
                        }
                        ;

enum_optional_assign	:	/*empty*/ {$$ = "";}
                        |	'=' LE_HEXconstant	{$$ = $1 + $2;}
                        |	'='	 LE_OCTALconstant {$$ = $1 + $2;}
                        |	'='	 LE_INTEGERconstant {$$ = $1 + $2;}
                        ;

enum_argument			:	LE_IDENTIFIER	enum_optional_assign {$$ = $1 + $2;}
                        ;
enum_arg_list			:	/*empty*/ {$$ = "";}
                        |	enum_argument	{$$ = $1;}
                        |	enum_arg_list ',' enum_argument {$$ = $1 + $2 + $3;}
                        ;

union_decl			:	stmnt_starter LE_UNION LE_IDENTIFIER '{'
                            {
                                currentScope.push_back($3);
                                printScopeName();
                                consumeDecl();
                                printScopeName();
                            }
                        ;
%%
void yyerror(char *s) {}

void syncParser(){
    //move lexer to the next ';' line or scope opening '{'
    //int ch = cl_scope_lex();
}

//swallow all tokens up to the first '{'
void consumeInitializationList(){
    while( true ){
        int ch = cl_scope_lex();
        if(ch == 0){
            break;
        }

        if(ch == '{'){
            break;
        }
    }
}

void consumeFuncArgList(){
    int depth = 1;
    while(depth > 0){
        int ch = cl_scope_lex();
        if(ch == 0){
            break;
        }

        if(ch == ')'){
            depth--;
            continue;
        }
        else if(ch == '('){
            depth ++ ;
            continue;
        }
    }
}

void readClassName()
{
#define NEXT_TOK()          c = cl_scope_lex(); if(c == 0) {className.clear(); return;}
#define BREAK_IF_NOT(x)     if(c != (int)x) {className.clear(); break;}
#define BREAK_IF_NOT2(x, y) if(c != (int)x && c != (int)y) break;
#define BREAK_IF(x)         if(c == (int)x) break;

    className.clear();
    
    // look ahead and see if we can see another 
    while( true ){
        int c = cl_scope_lex();
        if(c == 0){ // EOF?
            className.clear();
            break;
        }
        
        if(c == LE_MACRO || c == LE_FINAL) {
            continue;
            
        } else if(c == LE_IDENTIFIER) {
            className = cl_scope_text;
        
        } else if(c == LE_DECLSPEC && className.empty()) {
            // found decl sepc
            
            // Next token is '('
            NEXT_TOK();
            BREAK_IF_NOT('(');
            
            // Next token is LE_DLLIMPORT or LE_DLLEXPORT
            NEXT_TOK();
            BREAK_IF_NOT2(LE_DLLIEXPORT, LE_DLLIMPORT);
            
            // Next token should be closing brace
            NEXT_TOK();
            BREAK_IF_NOT(')');
            
        } else if( (c == '{') && (!className.empty()) ){
            // The following is the class content
            break;
            
        } else if( c == ':' && !className.empty() ) {
            // we got the class name, and we found ':'
            // read all tokens up until the first open brace
            while (true) {
                NEXT_TOK();
                if( c == (int)'{') {
                    return;
                }
            }
        } else {
            className.clear();
            break;
            
        }
    }
}

/**
 * consume all token until matching closing brace is found
 */
void consumeDecl()
{
    int depth = 1;
    while(depth > 0)
    {
        int ch = cl_scope_lex();
        if(ch ==0)
        {
            break;
        }
        if(ch == '}')
        {
            depth--;
            if(depth == 0) currentScope.pop_back();//reduce the scope
            continue;
        }
        else if(ch == '{')
        {
            depth ++ ;
            continue;
        }
    }

}

void consumeTemplateDecl()
{
    templateInitList.clear();
    int dep = 0;
    while( true ){
        int c = cl_scope_lex();
        if(c == 0){ // EOF?
            break;
        }

        if(c == '>' && dep == 0){
            templateInitList += cl_scope_text;
            break;
            
        } else {
            templateInitList += cl_scope_text;
            templateInitList += " ";
        }
        
        switch(c) {
        case (int)'<':
            dep++;
            break;
        case (int)'>':
            dep--;
            break;
        default:
            break;
        }
    }
    
    if(templateInitList.empty() == false)
        templateInitList.insert(0, "<");
}

std::string readInitializer(const char* delim)
{
    std::string intializer;
    int dep = 0;
    while( true ){
        int c = cl_scope_lex();
        if(c == 0){ // EOF?
            break;
        }
        
        if(strchr(delim, (char)c) && dep == 0){
            cl_scope_less(0);
            break;
            
        } else {
            intializer += cl_scope_text;
            intializer += " ";
        }
        
        switch(c) {
        case (int)'<':
            dep++;
            break;
        case (int)'>':
            dep--;
            break;
        default:
            break;
        }
    }
    return intializer;
}
//swallow all tokens up to the first '{'
void consumeNotIncluding(int ch){
    while( true ){
        int c = cl_scope_lex();
        if(c == 0){ // EOF?
            break;
        }

        //keep the function signature
        if(c == ch){
            cl_scope_less(0);
            break;
        }
    }
}

// return the scope name at the end of the input string
std::string get_scope_name(	const std::string &in,
                            std::vector<std::string> &additionalNS,
                            const std::map<std::string, std::string> &ignoreTokens)
{
    if( !setLexerInput(in, ignoreTokens) ){
        return "";
    }

    //call tghe main parsing routine
    cl_scope_parse();
    std::string scope = getCurrentScope();
    //do the lexer cleanup
    cl_scope_lex_clean();

    for(size_t i=0; i<gs_additionlNS.size(); i++){
        additionalNS.push_back(gs_additionlNS.at(i));
    }
    gs_additionlNS.clear();
    return scope;
}
