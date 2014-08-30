%{

#define YYTOKENTYPE
#include "php_parser_api.h"
#include "php_helpers.h"
#define YYSIZE_T size_t

#ifdef YYSTYPE
#undef YYSTYPE 
#endif

#define YYSTYPE wxString
extern void php_error(char*);
extern int php_lex();
extern void php_less(int count);
bool update_member_initialization(const wxString &expr, const wxString &type);
void consume_method_body();
void consume_define_args();
#define RESET_PARSER() PHPParser.getCurrentEntry().reset();

static wxString last_local_variable_name;
%}


%token END 0 "end of file"
%left T_INCLUDE T_INCLUDE_ONCE T_EVAL T_REQUIRE T_REQUIRE_ONCE
%token T_INCLUDE      "include (T_INCLUDE)"
%token T_INCLUDE_ONCE "include_once (T_INCLUDE_ONCE)"
%token T_EVAL         "eval (T_EVAL)"
%token T_REQUIRE      "require (T_REQUIRE)"
%token T_REQUIRE_ONCE "require_once (T_REQUIRE_ONCE)"
%left ','
%left T_LOGICAL_OR
%token T_LOGICAL_OR   "or (T_LOGICAL_OR)"
%left T_LOGICAL_XOR
%token T_LOGICAL_XOR  "xor (T_LOGICAL_XOR)"
%left T_LOGICAL_AND
%token T_LOGICAL_AND  "and (T_LOGICAL_AND)"
%right T_PRINT
%token T_PRINT        "print (T_PRINT)"
%right T_YIELD
%token T_YIELD        "yield (T_YIELD)"
%left '=' T_PLUS_EQUAL T_MINUS_EQUAL T_MUL_EQUAL T_DIV_EQUAL T_CONCAT_EQUAL T_MOD_EQUAL T_AND_EQUAL T_OR_EQUAL T_XOR_EQUAL T_SL_EQUAL T_SR_EQUAL
%token T_PLUS_EQUAL   "+= (T_PLUS_EQUAL)"
%token T_MINUS_EQUAL  "-= (T_MINUS_EQUAL)"
%token T_MUL_EQUAL    "*= (T_MUL_EQUAL)"
%token T_DIV_EQUAL    "/= (T_DIV_EQUAL)"
%token T_CONCAT_EQUAL ".= (T_CONCAT_EQUAL)"
%token T_MOD_EQUAL    "%= (T_MOD_EQUAL)"
%token T_AND_EQUAL    "&= (T_AND_EQUAL)"
%token T_OR_EQUAL     "|= (T_OR_EQUAL)"
%token T_XOR_EQUAL    "^= (T_XOR_EQUAL)"
%token T_SL_EQUAL     "<<= (T_SL_EQUAL)"
%token T_SR_EQUAL     ">>= (T_SR_EQUAL)"
%left '?' ':'
%left T_BOOLEAN_OR
%token T_BOOLEAN_OR   "|| (T_BOOLEAN_OR)"
%left T_BOOLEAN_AND
%token T_BOOLEAN_AND  "&& (T_BOOLEAN_AND)"
%left '|'
%left '^'
%left '&'
%nonassoc T_IS_EQUAL T_IS_NOT_EQUAL T_IS_IDENTICAL T_IS_NOT_IDENTICAL
%token T_IS_EQUAL     "== (T_IS_EQUAL)"
%token T_IS_NOT_EQUAL "!= (T_IS_NOT_EQUAL)"
%token T_IS_IDENTICAL "=== (T_IS_IDENTICAL)"
%token T_IS_NOT_IDENTICAL "!== (T_IS_NOT_IDENTICAL)"
%nonassoc '<' T_IS_SMALLER_OR_EQUAL '>' T_IS_GREATER_OR_EQUAL
%token T_IS_SMALLER_OR_EQUAL "<= (T_IS_SMALLER_OR_EQUAL)"
%token T_IS_GREATER_OR_EQUAL ">= (T_IS_GREATER_OR_EQUAL)"
%left T_SL T_SR
%token T_SL "<< (T_SL)"
%token T_SR ">> (T_SR)"
%left '+' '-' '.'
%left '*' '/' '%'
%right '!'
%nonassoc T_INSTANCEOF
%token T_INSTANCEOF  "instanceof (T_INSTANCEOF)"
%right '~' T_INC T_DEC T_INT_CAST T_DOUBLE_CAST T_STRING_CAST T_ARRAY_CAST T_OBJECT_CAST T_BOOL_CAST T_UNSET_CAST '@'
%token T_INC "++ (T_INC)"
%token T_DEC "-- (T_DEC)"
%token T_INT_CAST    "(int) (T_INT_CAST)"
%token T_DOUBLE_CAST "(double) (T_DOUBLE_CAST)"
%token T_STRING_CAST "(string) (T_STRING_CAST)"
%token T_ARRAY_CAST  "(array) (T_ARRAY_CAST)"
%token T_OBJECT_CAST "(object) (T_OBJECT_CAST)"
%token T_BOOL_CAST   "(bool) (T_BOOL_CAST)"
%token T_UNSET_CAST  "(unset) (T_UNSET_CAST)"
%right '['
%nonassoc T_NEW T_CLONE
%token T_NEW       "new (T_NEW)"
%token T_CLONE     "clone (T_CLONE)"
%token T_EXIT      "exit (T_EXIT)"
%token T_IF        "if (T_IF)"
%left T_ELSEIF
%token T_ELSEIF    "elseif (T_ELSEIF)"
%left T_ELSE
%token T_ELSE      "else (T_ELSE)"
%left T_ENDIF
%token T_ENDIF     "endif (T_ENDIF)"
%token T_LNUMBER   "integer number (T_LNUMBER)"
%token T_DNUMBER   "floating-point number (T_DNUMBER)"
%token T_STRING    "identifier (T_STRING)"
%token T_STRING_VARNAME "variable name (T_STRING_VARNAME)"
%token T_VARIABLE  "variable (T_VARIABLE)"
%token T_NUM_STRING "number (T_NUM_STRING)"
%token T_INLINE_HTML
%token T_CHARACTER
%token T_BAD_CHARACTER
%token T_ENCAPSED_AND_WHITESPACE  "quoted-string and whitespace (T_ENCAPSED_AND_WHITESPACE)"
%token T_CONSTANT_ENCAPSED_STRING "quoted-string (T_CONSTANT_ENCAPSED_STRING)"
%token T_ECHO       "echo (T_ECHO)"
%token T_DO         "do (T_DO)"
%token T_WHILE      "while (T_WHILE)"
%token T_ENDWHILE   "endwhile (T_ENDWHILE)"
%token T_FOR        "for (T_FOR)"
%token T_ENDFOR     "endfor (T_ENDFOR)"
%token T_FOREACH    "foreach (T_FOREACH)"
%token T_ENDFOREACH "endforeach (T_ENDFOREACH)"
%token T_DECLARE    "declare (T_DECLARE)"
%token T_ENDDECLARE "enddeclare (T_ENDDECLARE)"
%token T_AS         "as (T_AS)"
%token T_SWITCH     "switch (T_SWITCH)"
%token T_ENDSWITCH  "endswitch (T_ENDSWITCH)"
%token T_CASE       "case (T_CASE)"
%token T_DEFAULT    "default (T_DEFAULT)"
%token T_BREAK      "break (T_BREAK)"
%token T_CONTINUE   "continue (T_CONTINUE)"
%token T_GOTO       "goto (T_GOTO)"
%token T_FUNCTION   "function (T_FUNCTION)"
%token T_CONST      "const (T_CONST)"
%token T_RETURN     "return (T_RETURN)"
%token T_TRY        "try (T_TRY)"
%token T_CATCH      "catch (T_CATCH)"
%token T_THROW      "throw (T_THROW)"
%token T_USE        "use (T_USE)"
%token T_INSTEADOF  "insteadof (T_INSTEADOF)"
%token T_GLOBAL     "global (T_GLOBAL)"
%right T_STATIC T_ABSTRACT T_FINAL T_PRIVATE T_PROTECTED T_PUBLIC
%token T_STATIC     "static (T_STATIC)"
%token T_ABSTRACT   "abstract (T_ABSTRACT)"
%token T_FINAL      "final (T_FINAL)"
%token T_PRIVATE    "private (T_PRIVATE)"
%token T_PROTECTED  "protected (T_PROTECTED)"
%token T_PUBLIC     "public (T_PUBLIC)"
%token T_VAR        "var (T_VAR)"
%token T_UNSET      "unset (T_UNSET)"
%token T_ISSET      "isset (T_ISSET)"
%token T_EMPTY      "empty (T_EMPTY)"
%token T_HALT_COMPILER "__halt_compiler (T_HALT_COMPILER)"
%token T_CLASS      "class (T_CLASS)"
%token T_TRAIT      "trait (T_TRAIT)"
%token T_INTERFACE  "interface (T_INTERFACE)"
%token T_EXTENDS    "extends (T_EXTENDS)"
%token T_IMPLEMENTS "implements (T_IMPLEMENTS)"
%token T_OBJECT_OPERATOR "-> (T_OBJECT_OPERATOR)"
%token T_DOUBLE_ARROW    "=> (T_DOUBLE_ARROW)"
%token T_LIST            "list (T_LIST)"
%token T_ARRAY           "array (T_ARRAY)"
%token T_CALLABLE        "callable (T_CALLABLE)"
%token T_CLASS_C         "__CLASS__ (T_CLASS_C)"
%token T_TRAIT_C         "__TRAIT__ (T_TRAIT_C)"
%token T_METHOD_C        "__METHOD__ (T_METHOD_C)"
%token T_FUNC_C          "__FUNCTION__ (T_FUNC_C)"
%token T_LINE            "__LINE__ (T_LINE)"
%token T_FILE            "__FILE__ (T_FILE)"
%token T_COMMENT         "comment (T_COMMENT)"
%token T_DOC_COMMENT     "doc comment (T_DOC_COMMENT)"
%token T_OPEN_TAG        "open tag (T_OPEN_TAG)"
%token T_OPEN_TAG_WITH_ECHO "open tag with echo (T_OPEN_TAG_WITH_ECHO)"
%token T_CLOSE_TAG       "close tag (T_CLOSE_TAG)"
%token T_WHITESPACE      "whitespace (T_WHITESPACE)"
%token T_START_HEREDOC   "heredoc start (T_START_HEREDOC)"
%token T_END_HEREDOC     "heredoc end (T_END_HEREDOC)"
%token T_DOLLAR_OPEN_CURLY_BRACES "${ (T_DOLLAR_OPEN_CURLY_BRACES)"
%token T_CURLY_OPEN      "{$ (T_CURLY_OPEN)"
%token T_PAAMAYIM_NEKUDOTAYIM ":: (T_PAAMAYIM_NEKUDOTAYIM)"
%token T_NAMESPACE       "namespace (T_NAMESPACE)"
%token T_NS_C            "__NAMESPACE__ (T_NS_C)"
%token T_DIR             "__DIR__ (T_DIR)"
%token T_DEFINE          "define (T_DEFINE)"
%token T_NS_SEPARATOR    "\\ (T_NS_SEPARATOR)"

%% /* Rules */

start:
	top_statement_list
;

top_statement_list: top_statement_list   top_statement
				  | /* empty */
;

namespace_name:
	  T_STRING                               { $$ = $1; }
	| namespace_name T_NS_SEPARATOR T_STRING { $$ = $1 + $2 + $3; }
;

top_statement: statement
	|  T_DEFINE '(' {consume_define_args();} ')'
	|  function_declaration_statement
	|  class_declaration_statement
	|  T_HALT_COMPILER '(' ')' ';'
	|  T_NAMESPACE namespace_name ';' { PHPParser.setNameSpace( $2 ); }
	|  T_NAMESPACE namespace_name '{' { PHPParser.setNameSpace( $2 ); } top_statement_list '}'
	|  T_NAMESPACE '{' {PHPParser.setNameSpace( "<global>" );} top_statement_list '}'
	|  T_USE use_declarations ';' 
	|  constant_declaration ';'
	| error {
		//wxPrintf(wxT("error: unexpected %s at line %d, type: %d\n"),
		//		 PHPParser.getYytext().c_str(),
		//		 PHPParser.getLineNumber(),
		//		 PHPParser.getYytype());
		if(PHPParser.getYytype() != 0) {
			RESET_PARSER();
		}
	}
;

use_declarations:
		use_declarations ',' use_declaration
	|	use_declaration { $$ = $1; }
;

use_declaration:
		namespace_name { PHPParser.addUseStatement($1); }
	|	namespace_name T_AS T_STRING { PHPParser.addUseStatement($1, $3); }
	|	T_NS_SEPARATOR namespace_name { PHPParser.addUseStatement($1 + $2); }
	|	T_NS_SEPARATOR namespace_name T_AS T_STRING { PHPParser.addUseStatement($1 + $2, $4); }
;

constant_declaration:
		constant_declaration ',' T_STRING '=' static_scalar
		{
			PHPParser.getCurrentEntry().setName($3);
			PHPParser.getCurrentEntry().setKind(PHP_Kind_Constant);
			PHPParser.getCurrentEntry().addAttribute(PHP_Attr_Const);
            PHPParser.getCurrentEntry().FixDollarPrefix();
			PHPParser.addCurrentEntryToClass();
		}
		|   T_CONST T_STRING '=' static_scalar
		{
			PHPParser.getCurrentEntry().setName($2);
			PHPParser.getCurrentEntry().setKind(PHP_Kind_Constant);
			PHPParser.getCurrentEntry().addAttribute(PHP_Attr_Const);
            PHPParser.getCurrentEntry().FixDollarPrefix();
			PHPParser.addCurrentEntryToClass();
		}
;

inner_statement_list:
		inner_statement_list   inner_statement
	|   /* empty */
;


inner_statement:
		statement
	|	function_declaration_statement
	|	class_declaration_statement
	|	T_HALT_COMPILER '(' ')' ';'
;


statement:
		unticked_statement
	|	T_STRING ':'
;

unticked_statement:
		'{' inner_statement_list '}'
	|	T_IF parenthesis_expr  statement  elseif_list else_single
	|	T_IF parenthesis_expr ':'  inner_statement_list  new_elseif_list new_else_single T_ENDIF ';'
	|	T_WHILE '(' expr  ')'  while_statement
	|	T_DO statement T_WHILE parenthesis_expr ';'
	|	T_FOR
			'('
				for_expr
			';'
				for_expr
			';'
				for_expr
			')'
			for_statement
	|	T_SWITCH parenthesis_expr	 switch_case_list
	|	T_BREAK ';'
	|	T_BREAK expr ';'
	|	T_CONTINUE ';'
	|	T_CONTINUE expr ';'
	|	T_RETURN ';'
	|	T_RETURN expr_without_variable ';'
	|	T_RETURN variable ';'
    |   yield_expr ';'
	|	T_GLOBAL global_var_list ';'
	|	T_STATIC static_var_list ';'
	|	T_ECHO echo_expr_list ';'
	|	T_INLINE_HTML
	|	expr ';'
	|	T_UNSET '(' unset_variables ')' ';'
	|	T_FOREACH '(' variable T_AS

		foreach_variable foreach_optional_arg ')'
		foreach_statement
	|	T_FOREACH '(' expr_without_variable T_AS

		variable foreach_optional_arg ')'
		foreach_statement
	|	T_DECLARE  '(' declare_list ')' declare_statement
	|	';'		/* empty statement */
	|	T_TRY  '{' inner_statement_list '}'
		T_CATCH '('
		fully_qualified_class_name
		T_VARIABLE
		{
			PHPParser.newLocal($8, $7);
		}
		')'
		'{' inner_statement_list '}'
		additional_catches
	|	T_THROW expr ';'
	|	T_GOTO T_STRING ';'
;


additional_catches:
		non_empty_additional_catches
	|	/* empty */
;

non_empty_additional_catches:
		additional_catch
	|	non_empty_additional_catches additional_catch
;


additional_catch:
	T_CATCH '(' fully_qualified_class_name  T_VARIABLE ')'  '{' inner_statement_list '}'
;


unset_variables:
		unset_variable
	|	unset_variables ',' unset_variable
;

unset_variable:
		variable
;

function_declaration_statement:
		unticked_function_declaration_statement
;

class_declaration_statement:
		unticked_class_declaration_statement
;


is_reference:
		/* empty */
	|	'&'   { PHPParser.getCurrentEntry().addAttribute(PHP_Attr_Reference); }
;


unticked_function_declaration_statement:
		T_FUNCTION {
			// if the current line number is one line below the
			// collected doc comment, attach the doc comment to this
			// function
			PHPParser.assignDocComment();
		} is_reference T_STRING
			{
				PHPParser.getCurrentEntry().setName($4);
				PHPParser.getCurrentEntry().setKind(PHP_Kind_Function);
                PHPParser.getCurrentEntry().FixDollarPrefix();
                PHPParser.getCurrentEntry().setNameSpace( PHPParser.getCurrentNamespace() );
			}
			'(' parameter_list ')' '{' {
				if(PHPParser.getParseOutlineOnly()) {
					// Skip the method body
					consume_method_body();
				}
			} inner_statement_list '}'
			{
				/* add the function to the results */
				PHPParser.addCurrentEntryToClass();
			}
;

unticked_class_declaration_statement:
		class_entry_type T_STRING { PHPParser.newClass($2); } extends_from

			implements_list
			'{'
				class_statement_list
			'}'
			{
				// class is closed. set the scope back to
				// global scope
				PHPParser.setGlobalScope();
			}
	|   interface_entry T_STRING { PHPParser.newClass($2); }

			interface_extends_list
			'{'
				class_statement_list
			'}'
			{
				// interface is closed. set the scope back to
				// global scope
				PHPParser.setGlobalScope();
			}
;


class_entry_type:
		T_CLASS
	|	T_ABSTRACT T_CLASS
	|	T_TRAIT
	|	T_FINAL T_CLASS
;

extends_from:
		/* empty */
	|	T_EXTENDS fully_qualified_class_name { PHPParser.getCurrentClass()->setInherits( PHPParser.expandUseStatement($2) ); }
;

interface_entry:
	T_INTERFACE
;

interface_extends_list:
		/* empty */
	|	T_EXTENDS interface_list
;

implements_list:
		/* empty */
	|	T_IMPLEMENTS interface_list
;

interface_list:
		fully_qualified_class_name { 
            PHPParser.getCurrentClass()->addInterface( PHPParser.expandUseStatement( $1 ) ); 
        }
	|	interface_list ',' fully_qualified_class_name { PHPParser.getCurrentClass()->addInterface( PHPParser.expandUseStatement( $3 ) ); }
;

foreach_optional_arg:
		/* empty */
	|	T_DOUBLE_ARROW foreach_variable
;


foreach_variable:
		variable
	|	'&' variable
;

for_statement:
		statement
	|	':' inner_statement_list T_ENDFOR ';'
;


foreach_statement:
		statement
	|	':' inner_statement_list T_ENDFOREACH ';'
;


declare_statement:
		statement
	|	':' inner_statement_list T_ENDDECLARE ';'
;


declare_list:
		T_STRING '=' static_scalar
	|	declare_list ',' T_STRING '=' static_scalar
;


switch_case_list:
		'{' case_list '}'
	|	'{' ';' case_list '}'
	|	':' case_list T_ENDSWITCH ';'
	|	':' ';' case_list T_ENDSWITCH ';'
;


case_list:
		/* empty */
	|	case_list T_CASE expr case_separator  inner_statement_list
	|	case_list T_DEFAULT case_separator  inner_statement_list
;


case_separator:
		':'
	|	';'
;


while_statement:
		statement
	|	':' inner_statement_list T_ENDWHILE ';'
;



elseif_list:
		/* empty */
	|	elseif_list T_ELSEIF parenthesis_expr  statement
;


new_elseif_list:
		/* empty */
	|	new_elseif_list T_ELSEIF parenthesis_expr ':'  inner_statement_list
;


else_single:
		/* empty */
	|	T_ELSE statement
;


new_else_single:
		/* empty */
	|	T_ELSE ':' inner_statement_list
;


parameter_list:
		non_empty_parameter_list
	|	/* empty */
;


non_empty_parameter_list:
		optional_class_type T_VARIABLE
	{
		PHPParser.getCurrentEntry().newArgument($2);
		PHPParser.getCurrentEntry().getCurrentArg().setType($1);
	}
	| optional_class_type '&' T_VARIABLE
	{
		PHPParser.getCurrentEntry().newArgument($3);
		PHPParser.getCurrentEntry().getCurrentArg().setType($1);
		PHPParser.getCurrentEntry().getCurrentArg().addAttribute(PHP_Attr_Reference);
	}
	| optional_class_type '&' T_VARIABLE '=' static_scalar
	{
		PHPParser.getCurrentEntry().newArgument($3);
		PHPParser.getCurrentEntry().getCurrentArg().setType($1);
		PHPParser.getCurrentEntry().getCurrentArg().addAttribute(PHP_Attr_Reference);
	}
	| optional_class_type T_VARIABLE '=' static_scalar
	{
		PHPParser.getCurrentEntry().newArgument($2);
		PHPParser.getCurrentEntry().getCurrentArg().setType($1);
	}
	| non_empty_parameter_list ',' optional_class_type T_VARIABLE
	{
		PHPParser.getCurrentEntry().newArgument($4);
		PHPParser.getCurrentEntry().getCurrentArg().setType($3);
	}
	| non_empty_parameter_list ',' optional_class_type '&' T_VARIABLE
	{
		PHPParser.getCurrentEntry().newArgument($5);
		PHPParser.getCurrentEntry().getCurrentArg().setType($3);
		PHPParser.getCurrentEntry().getCurrentArg().addAttribute(PHP_Attr_Reference);
	}
	| non_empty_parameter_list ',' optional_class_type '&' T_VARIABLE	 '=' static_scalar
	{
		PHPParser.getCurrentEntry().newArgument($5);
		PHPParser.getCurrentEntry().getCurrentArg().setType($3);
		PHPParser.getCurrentEntry().getCurrentArg().addAttribute(PHP_Attr_Reference);
	}
	| non_empty_parameter_list ',' optional_class_type T_VARIABLE '=' static_scalar
	{
		PHPParser.getCurrentEntry().newArgument($4);
		PHPParser.getCurrentEntry().getCurrentArg().setType($3);
	}
;

optional_class_type:
		/* empty */ { $$.Clear(); }
	|	T_ARRAY     { $$ = $1; }
	|	T_CALLABLE  { $$ = $1; }
	|	fully_qualified_class_name { $$ = $1; }
;


function_call_parameter_list: non_empty_function_call_parameter_list
                            | yield_expr
                            | /* empty */
;

non_empty_function_call_parameter_list:
		expr_without_variable
	|	variable
	|	'&' w_variable
	|	non_empty_function_call_parameter_list ',' expr_without_variable
	|	non_empty_function_call_parameter_list ',' variable
	|	non_empty_function_call_parameter_list ',' '&' w_variable
;

global_var_list:
		global_var_list ',' global_var
	|	global_var
;


global_var:
	T_VARIABLE
	{
		PHPParser.newLocal($1, wxT(""), PHP_Attr_Global);
	}
	|	'$' r_variable
	{
		PHPParser.newLocal($1 + $2, wxT(""), PHP_Attr_Global);
	}
	|	'$' '{' expr '}'
;


static_var_list:
		static_var_list ',' T_VARIABLE
	|   static_var_list ',' T_VARIABLE '=' static_scalar
	|   T_VARIABLE
	|   T_VARIABLE '=' static_scalar
;


class_statement_list:
		class_statement_list class_statement
	|   /* empty */
;


class_statement:
		variable_modifiers  class_variable_declaration ';'
	|   class_constant_declaration ';'
	|   trait_use_statement
	|   method_modifiers function {
		
        PHPParser.getCurrentEntry().setKind(PHP_Kind_Function);
        PHPParser.getCurrentEntry().setNameSpace( PHPParser.getCurrentNamespace() );
        
		// if the current line number is one line below the
		// collected doc comment, attach the doc comment to this
		// function
		PHPParser.assignDocComment();
	} is_reference T_STRING { PHPParser.getCurrentEntry().setName($5); } '('
			parameter_list ')' method_body
;

trait_use_statement:
		T_USE trait_list trait_adaptations
;

trait_list:
		fully_qualified_class_name                { PHPParser.getCurrentClass()->addTrait($1); }
	|   trait_list ',' fully_qualified_class_name { PHPParser.getCurrentClass()->addTrait($3); }
;

trait_adaptations:
		';'
	|   '{' trait_adaptation_list '}'
;

trait_adaptation_list:
		/* empty */
	|   non_empty_trait_adaptation_list
;

non_empty_trait_adaptation_list:
		trait_adaptation_statement
	|   non_empty_trait_adaptation_list trait_adaptation_statement
;

trait_adaptation_statement:
		trait_precedence ';'
	|   trait_alias ';'
;

trait_precedence:
	trait_method_reference_fully_qualified T_INSTEADOF trait_reference_list
;

trait_reference_list:
		fully_qualified_class_name
	|   trait_reference_list ',' fully_qualified_class_name
;

trait_method_reference:
		T_STRING
	|   trait_method_reference_fully_qualified
;

trait_method_reference_fully_qualified:
	fully_qualified_class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING
;

trait_alias:
		trait_method_reference T_AS trait_modifiers T_STRING
	|   trait_method_reference T_AS member_modifier
;

trait_modifiers:
		/* empty */					 /* No change of methods visibility */
	|   member_modifier	 /* REM: Keep in mind, there are not only visibility modifiers */
;

method_body:
		';' /* abstract method */       { PHPParser.addCurrentEntryToClass(); }
	|   '{' {
			if(PHPParser.getParseOutlineOnly()) {
				// Skip the method body
				consume_method_body();
			}
		} inner_statement_list '}' { PHPParser.addCurrentEntryToClass(); }
;

variable_modifiers:
		non_empty_member_modifiers
	|   T_VAR
;

method_modifiers:
		/* empty */
	|   non_empty_member_modifiers
;

non_empty_member_modifiers:
		member_modifier
	|   non_empty_member_modifiers member_modifier
;

member_modifier:
		T_PUBLIC     { PHPParser.getCurrentEntry().addAttribute(PHP_Access_Public);}
	|	T_PROTECTED  { PHPParser.getCurrentEntry().addAttribute(PHP_Access_Protected);}
	|	T_PRIVATE    { PHPParser.getCurrentEntry().addAttribute(PHP_Access_Private);}
	|	T_STATIC     { PHPParser.getCurrentEntry().addAttribute(PHP_Attr_Static);}
	|	T_ABSTRACT   { PHPParser.getCurrentEntry().addAttribute(PHP_Attr_Abstract);}
	|	T_FINAL      { PHPParser.getCurrentEntry().addAttribute(PHP_Attr_Final);}
;

class_variable_declaration:
		class_variable_declaration ',' T_VARIABLE
	{
        // member name does not contains the $
		PHPParser.getCurrentEntry().setName($3);
		PHPParser.getCurrentEntry().setKind(PHP_Kind_Member);
        PHPParser.getCurrentEntry().FixDollarPrefix();
		// if the current line number is one line below the
		// collected doc comment, attach the doc comment to this
		// function
		PHPParser.assignDocComment();
		PHPParser.addCurrentEntryToClass();
	}
	| class_variable_declaration ',' T_VARIABLE '=' static_scalar
	{
		PHPParser.getCurrentEntry().setName($3);
		PHPParser.getCurrentEntry().setKind(PHP_Kind_Member);
        PHPParser.getCurrentEntry().FixDollarPrefix();
		// if the current line number is one line below the
		// collected doc comment, attach the doc comment to this
		// function
		PHPParser.assignDocComment();
		PHPParser.addCurrentEntryToClass();
	}
	| T_VARIABLE
	{
		PHPParser.getCurrentEntry().setName($1);
		PHPParser.getCurrentEntry().setKind(PHP_Kind_Member);
        PHPParser.getCurrentEntry().FixDollarPrefix();
		// if the current line number is one line below the
		// collected doc comment, attach the doc comment to this
		// function
		PHPParser.assignDocComment();
		PHPParser.addCurrentEntryToClass();
	}
	| T_VARIABLE '=' static_scalar
	{
		PHPParser.getCurrentEntry().setName($1);
		PHPParser.getCurrentEntry().setKind(PHP_Kind_Member);
        PHPParser.getCurrentEntry().FixDollarPrefix();
		// if the current line number is one line below the
		// collected doc comment, attach the doc comment to this
		// function
		PHPParser.assignDocComment();
		PHPParser.addCurrentEntryToClass();
	}
;

class_constant_declaration:
		class_constant_declaration ',' T_STRING '=' static_scalar
		{
			PHPParser.getCurrentEntry().setKind(PHP_Kind_Constant);
			PHPParser.getCurrentEntry().setName($3);
            PHPParser.getCurrentEntry().FixDollarPrefix();
			PHPParser.getCurrentEntry().setDefaultValue($5);
			// if the current line number is one line below the
			// collected doc comment, attach the doc comment to this
			// function
			PHPParser.assignDocComment();
			PHPParser.addCurrentEntryToClass();
		}
		| T_CONST T_STRING '=' static_scalar
		{
			PHPParser.getCurrentEntry().setKind(PHP_Kind_Constant);
			PHPParser.getCurrentEntry().setName($2);
			PHPParser.getCurrentEntry().addAttribute(PHP_Attr_Const);
            PHPParser.getCurrentEntry().FixDollarPrefix();
			PHPParser.getCurrentEntry().setDefaultValue($4);
			PHPParser.addCurrentEntryToClass();
		}
;

echo_expr_list:
		echo_expr_list ',' expr
	|	expr
;


for_expr:
		/* empty */
	|	non_empty_for_expr
;

non_empty_for_expr:
		non_empty_for_expr ','	 expr
	|	expr
;

expr_without_variable:
		T_LIST '('  assignment_list ')' '=' expr
	|   variable '=' expr                                           { 
        if(!update_member_initialization($1, $3)) {
            PHPParser.newLocal($1, $3);
        }
    }
	|   variable '=' '&' variable                                   { if(!update_member_initialization($1, $4)) PHPParser.newLocal($1, wxT(""), PHP_Attr_Reference);    }
	|   variable '=' '&' T_NEW class_name_reference  ctor_arguments { if(!update_member_initialization($1, $5)) PHPParser.newLocal($1, $5, PHP_Attr_Reference);}
	|   T_NEW class_name_reference  ctor_arguments                  { $$ = $2; }
	|   T_CLONE expr
	|   variable T_PLUS_EQUAL expr   { PHPParser.newLocal($1, $3);}
	|   variable T_MINUS_EQUAL expr  { PHPParser.newLocal($1, $3);}
	|   variable T_MUL_EQUAL expr    { PHPParser.newLocal($1, $3);}
	|   variable T_DIV_EQUAL expr    { PHPParser.newLocal($1, $3);}
	|   variable T_CONCAT_EQUAL expr { PHPParser.newLocal($1, $3);}
	|   variable T_MOD_EQUAL expr    { PHPParser.newLocal($1, $3);}
	|   variable T_AND_EQUAL expr    { PHPParser.newLocal($1, $3);}
	|   variable T_OR_EQUAL expr     { PHPParser.newLocal($1, $3);}
	|   variable T_XOR_EQUAL expr    { PHPParser.newLocal($1, $3);}
	|   variable T_SL_EQUAL expr     { PHPParser.newLocal($1, $3);}
	|   variable T_SR_EQUAL expr     { PHPParser.newLocal($1, $3);}
	|   rw_variable T_INC
	|   T_INC rw_variable
	|   rw_variable T_DEC
	|   T_DEC rw_variable
	|   expr T_BOOLEAN_OR  expr
	|   expr T_BOOLEAN_AND  expr
	|   expr T_LOGICAL_OR  expr
	|   expr T_LOGICAL_AND  expr
	|   expr T_LOGICAL_XOR expr
	|   expr '|' expr
	|   expr '&' expr
	|   expr '^' expr
	|   expr '.' expr
	|   expr '+' expr
	|   expr '-' expr
	|   expr '*' expr
	|   expr '/' expr
	|   expr '%' expr
	|   expr T_SL expr
	|   expr T_SR expr
	|   '+' expr %prec T_INC
	|   '-' expr %prec T_INC
	|   '!' expr
	|   '~' expr
	|   expr T_IS_IDENTICAL expr
	|   expr T_IS_NOT_IDENTICAL expr
	|   expr T_IS_EQUAL expr
	|   expr T_IS_NOT_EQUAL expr
	|   expr '<' expr
	|   expr T_IS_SMALLER_OR_EQUAL expr
	|   expr '>' expr
	|   expr T_IS_GREATER_OR_EQUAL expr
	|   expr T_INSTANCEOF class_name_reference
	|   parenthesis_expr
	|   expr '?'
		expr ':'
		expr
	|	expr '?' ':'
		expr
	|	internal_functions_in_yacc
	|	T_INT_CAST expr
	|	T_DOUBLE_CAST expr
	|	T_STRING_CAST expr
	|	T_ARRAY_CAST expr
	|	T_OBJECT_CAST expr
	|	T_BOOL_CAST expr
	|	T_UNSET_CAST expr
	|	T_EXIT exit_expr
	|	'@'  expr
	|	scalar
	|	T_ARRAY '(' array_pair_list ')'
 	|	'[' array_pair_list ']'
	|	'`' backticks_expr '`'
	|	T_PRINT expr
    |  T_YIELD
	|	function is_reference '(' { 
        PHPParser.pushAnonymousFunction(); /* switch to annonymous function */
    } parameter_list ')' lexical_vars '{' 
        inner_statement_list '}' 
    { PHPParser.popAnonymousFunction(); }
	|	T_STATIC function is_reference '(' {
        PHPParser.pushAnonymousFunction(); /* switch to annonymous function */
    } parameter_list ')' lexical_vars '{' inner_statement_list '}' 
    { PHPParser.popAnonymousFunction(); }
;

yield_expr:
        T_YIELD 
    |   T_YIELD variable
    |   T_YIELD expr T_DOUBLE_ARROW expr_without_variable 
    |   T_YIELD expr T_DOUBLE_ARROW variable
;

function:
	T_FUNCTION
;

lexical_vars:
		/* empty */
	|	T_USE '(' lexical_var_list ')'
;

lexical_var_list:
		lexical_var_list ',' T_VARIABLE
	|	lexical_var_list ',' '&' T_VARIABLE
	|	T_VARIABLE
	|	'&' T_VARIABLE
;

function_call:
		namespace_name '('
				function_call_parameter_list
				')'
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name '('
				function_call_parameter_list
				')'
	|	T_NS_SEPARATOR namespace_name '('
				function_call_parameter_list
				')'
	|	class_name T_PAAMAYIM_NEKUDOTAYIM variable_name '('
			function_call_parameter_list
			')'
	|	class_name T_PAAMAYIM_NEKUDOTAYIM variable_without_objects '('
			function_call_parameter_list
			')'
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM variable_name '('
			function_call_parameter_list
			')'
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM variable_without_objects '('
			function_call_parameter_list
			')'
	|	variable_without_objects  '('
			function_call_parameter_list ')'

;

class_name:
		T_STATIC                                   { $$ = $1; }
	|	namespace_name                             { $$ = $1; }
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name  { $$ = $1 + $2 + $3; }
	|	T_NS_SEPARATOR namespace_name              { $$ = $1 + $2;}
;

fully_qualified_class_name:
		namespace_name                             { $$= $1; }
	|   T_NAMESPACE T_NS_SEPARATOR namespace_name  { $$ = $1 + $2 + $3;}
	|   T_NS_SEPARATOR namespace_name              { $$ = $1 + $2;}
;



class_name_reference:
		class_name
	|	dynamic_class_name_reference
;


dynamic_class_name_reference:
		base_variable T_OBJECT_OPERATOR
			object_property  dynamic_class_name_variable_properties

	|	base_variable
;


dynamic_class_name_variable_properties:
		dynamic_class_name_variable_properties dynamic_class_name_variable_property
	|	/* empty */
;


dynamic_class_name_variable_property:
		T_OBJECT_OPERATOR object_property
;

parenthesis_expr:  '(' expr ')'        { $$ = $2; }
                |  '(' yield_expr ')' { $$ = $2; }
;

exit_expr:
		/* empty */
	|	'(' ')'
	|	parenthesis_expr
;

backticks_expr:
		/* empty */
	|	T_ENCAPSED_AND_WHITESPACE
	|	encaps_list
;


ctor_arguments:
		/* empty */
	|	'(' function_call_parameter_list ')'
;


common_scalar:
		T_LNUMBER
	|	T_DNUMBER
	|	T_CONSTANT_ENCAPSED_STRING
	|	T_LINE
	|	T_FILE
	|	T_DIR
	|	T_CLASS_C
	|	T_TRAIT_C
	|	T_METHOD_C
	|	T_FUNC_C
	|	T_NS_C
	|	T_START_HEREDOC T_ENCAPSED_AND_WHITESPACE T_END_HEREDOC
	|	T_START_HEREDOC T_END_HEREDOC
;


static_scalar: /* compile-time evaluated scalars */
		common_scalar
	|	namespace_name
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name
	|	T_NS_SEPARATOR namespace_name
	|	'+' static_scalar
	|	'-' static_scalar
	|	T_ARRAY '(' static_array_pair_list ')'
	|	'[' static_array_pair_list ']'
	|	static_class_constant
;

static_class_constant:
		class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING
;

scalar:
		T_STRING_VARNAME
	|	class_constant
	|	namespace_name
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name
	|	T_NS_SEPARATOR namespace_name
	|	common_scalar
	|	'"' encaps_list '"'
	|	T_START_HEREDOC encaps_list T_END_HEREDOC
;


static_array_pair_list:
		/* empty */
	|	non_empty_static_array_pair_list possible_comma
;

possible_comma:
		/* empty */
	|	','
;

non_empty_static_array_pair_list:
		non_empty_static_array_pair_list ',' static_scalar T_DOUBLE_ARROW static_scalar
	|	non_empty_static_array_pair_list ',' static_scalar
	|	static_scalar T_DOUBLE_ARROW static_scalar
	|	static_scalar
;

expr:
		r_variable
	|	expr_without_variable
;

r_variable:
	variable
;


w_variable:
	variable
;

rw_variable:
	variable
;

variable:
	 base_variable_with_function_calls T_OBJECT_OPERATOR object_property  method_or_not variable_properties { $$ = $1 + $2 + $3;}
	| base_variable_with_function_calls                                                                     { $$ = $1;}
;

variable_properties:
		variable_properties variable_property
	|	/* empty */
;


variable_property:
		T_OBJECT_OPERATOR object_property  method_or_not
;

array_method_dereference:
		array_method_dereference '[' dim_offset ']'
	|	method '[' dim_offset ']'
;

method:
		'('
				function_call_parameter_list ')'

;

method_or_not:
		method
	|	array_method_dereference
	|	/* empty */
;

variable_without_objects:
		reference_variable
	|	simple_indirect_reference reference_variable
;

static_member:
		class_name T_PAAMAYIM_NEKUDOTAYIM variable_without_objects
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM variable_without_objects

;

variable_class_name:
		reference_variable
;

array_function_dereference:
		array_function_dereference '[' dim_offset ']'
	|	function_call
		'[' dim_offset ']'
;

base_variable_with_function_calls:
        base_variable                 { $$ = $1;    }
	|   array_function_dereference    { $$.Clear(); }
	|   function_call                 { $$ = $1; }
;


base_variable:
		reference_variable
	|	simple_indirect_reference reference_variable
	|	static_member
;

reference_variable:
		reference_variable '[' dim_offset ']' { $$ = $1;    }
	|	reference_variable '{' expr '}'       { $$.Clear(); }
	|	compound_variable                     { $$ = $1;    }
;

compound_variable:
		T_VARIABLE { $$ = $1; }
	|	'$' '{' expr '}'
;

dim_offset:
		/* empty */
	|	expr
;


object_property:
		object_dim_list
	|	variable_without_objects
;

object_dim_list:
		object_dim_list '[' dim_offset ']'
	|	object_dim_list '{' expr '}'
	|	variable_name
;

variable_name:
		T_STRING
	|	'{' expr '}'
;

simple_indirect_reference:
		'$'
	|	simple_indirect_reference '$'
;

assignment_list:
		assignment_list ',' assignment_list_element
	|	assignment_list_element
;


assignment_list_element:
		variable
	|	T_LIST '('  assignment_list ')'
	|	/* empty */
;


array_pair_list:
		/* empty */
	|	non_empty_array_pair_list possible_comma
;

non_empty_array_pair_list:
		non_empty_array_pair_list ',' expr T_DOUBLE_ARROW expr
	|	non_empty_array_pair_list ',' expr
	|	expr T_DOUBLE_ARROW expr
	|	expr
	|	non_empty_array_pair_list ',' expr T_DOUBLE_ARROW '&' w_variable
	|	non_empty_array_pair_list ',' '&' w_variable
	|	expr T_DOUBLE_ARROW '&' w_variable
	|	'&' w_variable
;

encaps_list:
		encaps_list encaps_var
	|	encaps_list T_ENCAPSED_AND_WHITESPACE
	|	encaps_var
	|	T_ENCAPSED_AND_WHITESPACE encaps_var
;



encaps_var:
		T_VARIABLE
	|	T_VARIABLE '['  encaps_var_offset ']'
	|	T_VARIABLE T_OBJECT_OPERATOR T_STRING
	|	T_DOLLAR_OPEN_CURLY_BRACES expr '}'
	|	T_DOLLAR_OPEN_CURLY_BRACES T_STRING_VARNAME '[' expr ']' '}'
	|	T_CURLY_OPEN variable '}'
;


encaps_var_offset:
		T_STRING
	|	T_NUM_STRING
	|	T_VARIABLE
;


internal_functions_in_yacc:
		T_ISSET '(' isset_variables ')'
	|	T_EMPTY '(' variable ')'
	|	T_INCLUDE expr
	|	T_INCLUDE_ONCE expr
	|	T_EVAL '(' expr ')'
	|	T_REQUIRE expr
	|	T_REQUIRE_ONCE expr
;

isset_variables:
		variable
	|	isset_variables ','  variable
;

class_constant:
		class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING
;

%%

bool update_member_initialization(const wxString &expr, const wxString &type)
{
	// parse the current expression and see if we are initializing
	// a member variable
	PHPParser.parseString(false, PHP_PREFIX_WITH_SPACE + expr, PHPGlobals::Type_Expr, false);
	if(PHPParser.getCurchain().size() != 2)
		return false;

	if(PHPParser.getCurchain().at(0).getKind() != PHP_Kind_This)
		return false;

	// Must not be in a global scope
	if(PHPParser.getCurrentClass()->isGlobalScopeClass())
		return false;

	PHPEntry *member = PHPParser.getCurrentClass()->findChild(PHPParser.getCurchain().at(1).getName(), PHP_Kind_Member);
	if(member) {
		// We found a match for this member
		// update its type
		member->setType(type);
		return true;
	}
	return false;
}

void consume_method_body()
{
	wxString type, typeScope;
	PHPParser.skipMethodBodyKeepReturnStatement(type, typeScope);
	if(PHPParser.getCurrentEntry().getKind() == PHP_Kind_Function) {
		PHPParser.getCurrentEntry().setType(type);
		PHPParser.getCurrentEntry().setTypeScope(typeScope);
	}
}

void consume_define_args()
{
	YYSTYPE name, value, curvalue;
	int depth = 1;
	while(depth > 0)
	{
		int ch = php_lex();
		if(ch ==0){
			break;
		}
		switch(ch) {
		case ')':
			depth--;
			if(depth == 0) {
				value = curvalue;
				value.RemoveLast();
				php_less(0);
				if(!name.IsEmpty() && !value.IsEmpty()) {
					PHPParser.getCurrentEntry().setName(name);
					PHPParser.getCurrentEntry().setKind(PHP_Kind_Constant);
                    PHPParser.getCurrentEntry().FixDollarPrefix();
					PHPParser.getCurrentEntry().setDefaultValue(value);
					PHPParser.assignDocComment();
					PHPParser.addCurrentEntryToClass();
				}
				return;
			} else {
				curvalue << php_lval << wxT(" ");
			}
			break;
		case T_CONSTANT_ENCAPSED_STRING:
			{
				YYSTYPE tmp;
				tmp = php_lval;
				if(name.IsEmpty()) { // still looking for the name of the define
					if(tmp.StartsWith(wxT("\"")) || tmp.StartsWith(wxT("'")))
						tmp.Remove(0, 1);
						
					if(tmp.EndsWith(wxT("\"")) || tmp.EndsWith(wxT("'")))
						tmp.RemoveLast();
				}
				curvalue << tmp << wxT(" ");
			}
			break;
		case ',':
			name = curvalue;
			name.RemoveLast();
			curvalue.Clear();
			break;
			
		case '(':
			depth++;
			curvalue << php_lval << wxT(" ");
			break;
		default:
			curvalue << php_lval << wxT(" ");
			break;
		}
	}
}
