%{
// Copyright Eran Ifrah(c)
%}

%{
/*************** Includes and Defines *****************************/
#include "string"
#include "vector"
#include "stdio.h"
#include "map"
#include "function.h"

#define YYDEBUG_LEXER_TEXT (cl_func_lval)
#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

#ifdef yylex
#undef yylex
#define yylex cl_scope_lex
#endif

int cl_func_parse();
void cl_func_error(char *string);

static FunctionList *g_funcs = NULL;
static clFunction curr_func;

//---------------------------------------------
// externs defined in the lexer
//---------------------------------------------
extern char *cl_func_text;
extern int cl_scope_lex();
extern bool setLexerInput(const std::string &in, const std::map<std::string, std::string> &ignoreTokens);
extern int cl_scope_lineno;
extern void cl_scope_lex_clean();


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
%token  LE_INLINE          LE_OVERLOAD LE_OVERRIDE  LE_FINAL
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
				|         LE_TIME_T       { $$ = $1; }
				|         LE_SIZE_T       { $$ = $1; }
                ;

basic_type_name:  LE_UNSIGNED basic_type_name_inter { $$ = $1 + " " + $2; }
				| LE_SIGNED basic_type_name_inter   { $$ = $1 + " " + $2; }
				| LE_LONG LE_LONG                   { $$ = $1 + " " + $2; }
				| LE_LONG LE_INT                    { $$ = $1 + " " + $2; }
				| basic_type_name_inter             { $$ = $1;            }
				;


/* ========================================================================*/
/* find declarations                                                       */
/* ========================================================================*/

translation_unit :      /*empty*/
						| translation_unit external_decl
						;

external_decl :     {curr_func.Reset();} function_decl
					|  error {
							//printf("CodeLite: syntax error, unexpected token '%s' found\n", cl_func_lval.c_str());
						}
					;

/*templates*/
template_arg        : /* empty */	{ $$ = "";}
						| template_specifiter LE_IDENTIFIER {$$ = $1 + " " + $2;}
						;

template_arg_list   : template_arg                       { $$ = $1; }
					| template_arg_list ',' template_arg { $$ = $1 + " " + $2 + " " + $3; }
					;

template_specifiter	:	LE_CLASS	{ $$ = $1; }
							|	LE_TYPENAME	{ $$ = $1; }
							;

opt_template_qualifier	: /*empty*/
							| LE_TEMPLATE '<' template_arg_list '>'	{ $$ = $1 + $2 + $3 + $4;}
							;

/* the following rules are for template parameters no declarations! */
template_parameter_list	: /* empty */		{$$ = "";}
							| template_parameter	{$$ = $1;}
							| template_parameter_list ',' template_parameter {$$ = $1 + $2 + $3;}
							;

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

func_name: LE_IDENTIFIER {$$ = $1;}
		 | '~' LE_IDENTIFIER {$$ = $1 + $2;}
		 | LE_OPERATOR any_operator {$$ = $1 + $2;}
		 ;

any_operator:
        '+'
        | '-'
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
        | '(' ')'
        | '[' ']'
        | LE_NEW
        | LE_DELETE
        | ','
        ;

/* functions */
function_decl	: 	stmnt_starter opt_template_qualifier virtual_spec const_spec variable_decl const_spec nested_scope_specifier func_name '(' {func_consumeFuncArgList();} const_spec declare_throw opt_pure_virtual override_final_specifier func_postfix
					{
						//trim down trailing '::' from scope name
						$6.erase($6.find_last_not_of(":")+1);
						curr_func.m_isVirtual = $3.find("virtual") != std::string::npos;
						curr_func.m_isPureVirtual = $13.find("=") != std::string::npos;
						curr_func.m_isConst = $11.find("const") != std::string::npos;
						curr_func.m_isFinal = $14.find("final") != std::string::npos;
						curr_func.m_name = $8;
						curr_func.m_scope = $7;
						curr_func.m_retrunValusConst = $4;
						curr_func.m_lineno = cl_scope_lineno;
						curr_func.m_throws = $12;
                        curr_func.m_returnValue.m_rightSideConst = $6;
                        curr_func.m_returnValue.m_isConst = !$4.empty();
						if(g_funcs)
						{
							g_funcs->push_back(curr_func);
						}
						curr_func.Reset();
					}
					;

declare_throw: 	/*empty*/ {$$ = "";}
			|	LE_THROW '(' template_parameter_list ')' {$$ = $3;}
			;

override_final_specifier: /* empty */ {$$ = "";}
    | LE_OVERRIDE
    | LE_FINAL
    ;
    
func_postfix: '{'
				| ';'
				;

nested_scope_specifier		: /*empty*/ {$$ = "";}
							| nested_scope_specifier scope_specifier {	$$ = $1 + $2;}
							;

opt_pure_virtual 	: /*empty*/ {$$ = "";}
						| '=' LE_OCTALconstant {$$ = $1 + $2;}
						;

scope_specifier	:	LE_IDENTIFIER LE_CLCL {$$ = $1+ $2;}
						|	LE_IDENTIFIER  '<' {func_consumeTemplateDecl();} LE_CLCL {$$ = $1 + $4;}
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
variable_decl		:	nested_scope_specifier basic_type_name special_star_amp
							{
								$1.erase($1.find_last_not_of(":")+1);
								curr_func.m_returnValue.m_type = $2;
								curr_func.m_returnValue.m_typeScope = $1;
								curr_func.m_returnValue.m_starAmp = $3;
								curr_func.m_returnValue.m_isPtr = ($3.find("*") != (size_t)-1);
								$$ = $1 + $2 + $3;
							}
						|	nested_scope_specifier LE_IDENTIFIER special_star_amp
							{
								$1.erase($1.find_last_not_of(":")+1);
								curr_func.m_returnValue.m_type = $2;
								curr_func.m_returnValue.m_typeScope = $1;
								curr_func.m_returnValue.m_starAmp = $3;
								curr_func.m_returnValue.m_isPtr = ($3.find("*") != (size_t)-1);
								$$ = $1 + $2 + $3  ;
							}

| 	nested_scope_specifier LE_IDENTIFIER '<' template_parameter_list '>' special_star_amp
							{
								$1.erase($1.find_last_not_of(":")+1);
								curr_func.m_returnValue.m_type = $2;
								curr_func.m_returnValue.m_typeScope = $1;
								curr_func.m_returnValue.m_starAmp = $6;
								curr_func.m_returnValue.m_isPtr = ($6.find("*") != (size_t)-1);
								curr_func.m_returnValue.m_isTemplate = true;
								curr_func.m_returnValue.m_templateDecl = $4;
								$$ = $1 + $2 + $3  + $4 + $5 + $6 ;
							}
						;
%%
void yyerror(char *s) {}

void func_consumeFuncArgList()
{
	curr_func.m_signature = "(";

	int depth = 1;
	while(depth > 0)
	{
		int ch = cl_scope_lex();
		if(ch == 0)
		{
			break;
		}

		curr_func.m_signature += cl_func_lval;
		curr_func.m_signature += " ";
		if(ch == ')')
		{
			depth--;
			continue;
		}
		else if(ch == '(')
		{
			depth ++ ;
			continue;
		}
	}
}

/**
 * consume all token until matching closing brace is found
 */
void func_consumeDecl()
{
	int depth = 1;
	while(depth > 0)
	{
		int ch = cl_scope_lex();
		//printf("ch=%d\n", ch);
		//fflush(stdout);
		if(ch ==0)
		{
			break;
		}
		if(ch == '}')
		{
			depth--;
			continue;
		}
		else if(ch == '{')
		{
			depth ++ ;
			continue;
		}
	}

}

void func_consumeTemplateDecl()
{
	int depth = 1;
	while(depth > 0)
	{
		int ch = cl_scope_lex();
		//printf("ch=%d\n", ch);
		//fflush(stdout);
		if(ch ==0){
			break;
		}

		if(ch == '>')
		{
			depth--;
			continue;
		}
		else if(ch == '<')
		{
			depth ++ ;
			continue;
		}
	}
}

// return the scope name at the end of the input string
void get_functions(const std::string &in, FunctionList &li, const std::map<std::string, std::string> &ignoreTokens)
{
	if( !setLexerInput(in, ignoreTokens) )
	{
		return;
	}

	g_funcs = &li;

	//call tghe main parsing routine
	cl_func_parse();
	g_funcs = NULL;

	//do the lexer cleanup
	cl_scope_lex_clean();
}
