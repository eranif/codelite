%{
// Copyright Eran Ifrah(c)
%}

%{
/*************** Includes and Defines *****************************/
#include "string"
#include "vector"
#include "stdio.h"
#include "map"
#include "variable.h"
#include "cl_typedef.h"

#ifdef yylex
#undef yylex
#define yylex cl_scope_lex
#endif

#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

int  cl_typedef_parse();
void syncParser();
static std::string typedef_consumBracketsContent(char openBrace);
static void typedef_consumeDefaultValue(char c1, char c2);

static  VariableList *           gs_vars = NULL;
static  std::vector<std::string> gs_names;
static  bool                     g_isUsedWithinFunc = false;
static  std::string              s_tmpString;
static  Variable                 curr_var;
static  clTypedefList            gs_typedefs;
static  clTypedef                gs_currentTypedef;
static  std::string              s_templateInitList;

//---------------------------------------------
// externs defined in the lexer
//---------------------------------------------
extern char *cl_scope_text;
extern int   cl_scope_lex();
extern void  cl_scope_less(int count);
extern int   cl_scope_lineno;
extern void  cl_scope_lex_clean();
extern bool  setLexerInput(const std::string &in, const std::map<std::string, std::string> &ignoreMap);
extern void  setUseIgnoreMacros(bool ignore);


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
%token  LE_INLINE          LE_OVERLOAD      LE_OVERRIDE LE_FINAL
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
                ;

basic_type_name:	LE_UNSIGNED basic_type_name_inter     { $$ = $1 + " " + $2; }
                |	LE_SIGNED basic_type_name_inter     { $$ = $1 + " " + $2; }
                |	LE_LONG LE_LONG                     { $$ = $1 + " " + $2; }
                |	LE_LONG LE_INT                         { $$ = $1 + " " + $2; }
                |	basic_type_name_inter                   { $$ = $1; }
                ;

/* ========================================================================*/
/* find declarations	                                                   */
/* ========================================================================*/

translation_unit	:        /*empty*/
                        | translation_unit external_decl
                        ;

external_decl	    :     {curr_var.Reset(); gs_names.clear(); s_tmpString.clear(); gs_currentTypedef.clear(); s_templateInitList.clear();} typedefs
						| error {
                            yyclearin;    //clear lookahead token
                            yyerrok;
//                            printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_scope_text, cl_scope_lineno);
                            }
                        ;
/** Typedefs **/

typedefs             : stmnt_starter LE_TYPEDEF opt_typename_specifier real_type new_name ';'
  					{
						if(gs_currentTypedef.m_realType.m_templateDecl.empty())
							gs_currentTypedef.m_realType.m_templateDecl = s_templateInitList;
						s_templateInitList.clear();
						gs_typedefs.push_back(gs_currentTypedef);

					}
					 | stmnt_starter opt_template_qualifier LE_USING new_name '=' opt_typename_specifier real_type ';'
					 {
						if(gs_currentTypedef.m_realType.m_templateDecl.empty())
							gs_currentTypedef.m_realType.m_templateDecl = s_templateInitList;
						s_templateInitList.clear();
						gs_typedefs.push_back(gs_currentTypedef);
					 }
					;

real_type : variable_decl special_star_amp
			{
				gs_currentTypedef.m_realType = curr_var;
				gs_currentTypedef.m_realType.m_isPtr = ($2.find("*") != std::string::npos);
			}
			;

new_name   : LE_IDENTIFIER { gs_currentTypedef.m_name = $1; }
		   ;

opt_typename_specifier  :  /* empty */ { $$ = ""; }
                        |  LE_TYPENAME { $$ = $1; }
                        ;

/*templates*/
opt_template_qualifier	: /*empty*/
                            | LE_TEMPLATE '<' {s_tmpString = typedef_consumBracketsContent('<');}
                            {
                                $$ = $1 + $2 + s_tmpString;
                                s_tmpString.clear();
                            }
                            ;

/* the following rules are for template parameters no declarations! */
parameter_list	: /* empty */        {$$ = "";}
                            | template_parameter	{$$ = $1;}
                            | parameter_list ',' template_parameter {$$ = $1 + $2 + " " + $3;}
                            ;

template_parameter	:   const_spec nested_scope_specifier LE_IDENTIFIER special_star_amp
                    {
						$$ = $2 + $3 +$4;
					}
                    |   const_spec nested_scope_specifier basic_type_name special_star_amp
					{
						$$ = $2 + $3 +$4;
					}
                    |   const_spec nested_scope_specifier LE_IDENTIFIER '<' parameter_list '>' special_star_amp
					{
						$$ = $2 + $3 +$4 + $5 + $6 + $7 + " " ;
					}
;

/*
applicable for C++, for cases where a function is declared as
void scope::foo(){ ... }
*/
scope_specifier	:	LE_IDENTIFIER LE_CLCL {$$ = $1+ $2; }
                |	LE_IDENTIFIER  '<' parameter_list '>' LE_CLCL {$$ = $1 ; s_templateInitList = $2 + $3 + $4;}
                ;

nested_scope_specifier: /*empty*/ {$$ = "";}
                    | nested_scope_specifier scope_specifier {    $$ = $1 + $2;}
                    ;

const_spec	        : /* empty */      {$$ = "";                          }
                    | LE_CONST         { $$ = $1;                         }
                    | LE_CONST '*'     { $$ = $1 + $2; curr_var.m_rightSideConst = $$;     }
                    | LE_CONST '&'     { $$ = $1 + $2; curr_var.m_rightSideConst = $$;     }
					| LE_CONST '*' '*' { $$ = $1 + $2 + $3; curr_var.m_rightSideConst = $$;}
                    ;

amp_item	        : /*empty*/    {$$ = ""; }
                    | '&'            { $$ = $1; }
                    ;

star_list           : /*empty*/        {$$ = ""; }
                    | star_list '*'    {$$ = $1 + $2;}
                    ;

special_star_amp:star_list amp_item { $$ = $1 + $2; }
                        ;

stmnt_starter       : /*empty*/ {$$ = "";}
                    | ';' { $$ = ";";}
                    | '{' { $$ = "{";}
//						| '(' { $$ = "(";}
                    | '}' { $$ = "}";}
                    | ':' { $$ = ":";}    //e.g. private: std::string m_name;
//						| '=' { $$ = "=";}
                    ;

/** Variables **/
variable_decl       :   const_spec basic_type_name
                        {
                            $$ = $1 + " " + $2;
                            $2.erase($2.find_last_not_of(":")+1);
                        	curr_var.m_type = $2;
							curr_var.m_isBasicType = true;
                        	curr_var.m_isConst = !$1.empty();
                        }
                        |   const_spec nested_scope_specifier LE_IDENTIFIER
                        {
							$$ = $1 + " " + $2 + $3;
                            $2.erase($2.find_last_not_of(":")+1);
                        	curr_var.m_typeScope = $2;
                        	curr_var.m_type = $3;
                        	curr_var.m_isConst = !$1.empty();
                        	s_tmpString.clear();
                        }
                        |   const_spec nested_scope_specifier LE_IDENTIFIER '<' parameter_list '>'
                        {
                            $$ = $1 + " " + $2 + $3 + " " + $4 + $5 + $6;
                            $2.erase($2.find_last_not_of(":")+1);
                        	curr_var.m_typeScope = $2;
                        	curr_var.m_type = $3;
                        	curr_var.m_isTemplate = true;
                        	curr_var.m_templateDecl = $4 +$5 +$6;
                        	curr_var.m_isConst = !$1.empty();
                        	s_tmpString.clear();
                        }
                        | const_spec LE_STRUCT nested_scope_specifier LE_IDENTIFIER  '{' {s_tmpString = typedef_consumBracketsContent('{');}
                        {
                            $$ = $1 + " " + $2 + " " + $3 + " " + $4 + $5 + $6 + s_tmpString;
                            $3.erase($3.find_last_not_of(":")+1);
                        	curr_var.m_typeScope = $3;
                        	curr_var.m_type = $4;
                        	curr_var.m_isTemplate = false;
                        	curr_var.m_isConst = !$1.empty();
                        	s_tmpString.clear();
                        }
                        | const_spec LE_STRUCT nested_scope_specifier LE_IDENTIFIER
                        {
                            $$ = $0;
                            $3.erase($3.find_last_not_of(":")+1);
                        	curr_var.m_typeScope = $3;
                        	curr_var.m_type = $4;
                        	curr_var.m_isTemplate = false;
                        	curr_var.m_isConst = !$1.empty();
                        	s_tmpString.clear();
                        }
                        ;

%%
void yyerror(char *s) {}


std::string typedef_consumBracketsContent(char openBrace)
{
	char closeBrace;

	switch(openBrace) {
	case '(': closeBrace = ')'; break;
	case '[': closeBrace = ']'; break;
	case '<': closeBrace = '>'; break;
	case '{': closeBrace = '}'; break;
	default:
    	openBrace = '(';
    	closeBrace = ')';
    	break;
    }

	std::string consumedData;
	int depth = 1;
	while(depth > 0)
    {
    	int ch = cl_scope_lex();
    	if(ch == 0){
        	break;
        }


    	if(ch == closeBrace)
        {
        	consumedData.erase(0, consumedData.find_first_not_of(" "));
        	consumedData.erase(consumedData.find_last_not_of(" ")+1);
        	consumedData += cl_scope_text;

        	depth--;
        	continue;
        }
    	else if(ch == openBrace)
        {
        	consumedData.erase(0, consumedData.find_first_not_of(" "));
        	consumedData.erase(consumedData.find_last_not_of(" ")+1);
        	consumedData += cl_scope_text;

        	depth ++ ;
        	continue;
        }

    	consumedData += cl_scope_text;
    	consumedData += " ";
    }

	return consumedData;
}

void typedef_consumeDefaultValue(char c1, char c2)
{
	int depth = 0;

	while (depth >= 0) {
    	int ch = cl_scope_lex();
    	if(ch == 0) { break;}

    	if(ch == c1 && depth == 0) {
        	cl_scope_less(0);
        	break;
        }

    	if(ch == c2 && depth == 0) {
        	cl_scope_less(0);
        	break;
        }

    	curr_var.m_defaultValue += cl_scope_text;
    	if(ch == ')' || ch == '}'){
        	depth--;
        	continue;
        } else if(ch == '(' || ch == '{') {
        	depth ++ ;
        	continue;
        }
    }
}

void do_clean_up()
{
	gs_vars = NULL;

    // restore settings
	setUseIgnoreMacros(true);
	g_isUsedWithinFunc = false;
	gs_typedefs.clear();

    //do the lexer cleanup
	cl_scope_lex_clean();
}

// return the scope name at the end of the input string
void get_typedefs(const std::string &in, clTypedefList &li)
{
	std::map<std::string, std::string> dummy;

    // provide the lexer with new input
	if( !setLexerInput(in, dummy) ){
    	return;
    }

	// set the parser local output to our variable list
	cl_typedef_parse();
	li = gs_typedefs;

	do_clean_up();
}
