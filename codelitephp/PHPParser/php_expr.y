%{
#define YYTOKENTYPE
#include "php_parser_api.h"
#define YYSIZE_T size_t

#ifdef YYSTYPE
#undef YYSTYPE
#endif

#define YYSTYPE wxString
extern void php_expr_error(char*);
extern int php_expr_lex();
extern void php_expr_less(int count);
extern int php_expr_lineno;
extern int php_expr_type;

void consume_method_call_args();

#define RESET_PARSER() PHPParser.resetChain();
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

start: include_statement_list
;

include_statement_list: include_statement include_statement_list
					  | /* empty */
;

include_statement: include_k open_brace expr close_brace ';' 
					{
						PHPParser.setIncludeFile($3);
					}
					| error 
					{
						yyclearin;
						yyerrok;
//						wxPrintf(wxT("error: unexpected '%s' at line %d type=%d\n"),
//								 php_expr_lval.c_str(),
//								 php_expr_lineno,
//								 php_expr_type
//								 );
					}
;

include_k: T_REQUIRE
	   | T_REQUIRE_ONCE
	   | T_INCLUDE
	   | T_INCLUDE_ONCE
;

open_brace: /* empty */
		  | '('
;

close_brace: /* empty */
		   | ')'
;

expr_basic: T_CONSTANT_ENCAPSED_STRING { 
				wxString strippedString($1);
				if(strippedString.StartsWith(wxT("\"")) || strippedString.StartsWith(wxT("'")))
					strippedString.Remove(0, 1);
				
				if(strippedString.EndsWith(wxT("\"")) || strippedString.EndsWith(wxT("'")))
					strippedString.RemoveLast();
				
				$$ = strippedString;
		  }
		  | T_STRING function_call_or_not {
			  
			  if($1 == wxT("getcwd")) {
				$$ = wxGetCwd();
				  
			  } else if($1 == wxT("dirname")) {
				wxFileName fn($2);
				$$ = fn.GetPath();
				  
			  } else if($1 == wxT("DIRECTORY_SEPARATOR")) {
				$$ = wxFileName::GetPathSeparator();
				
			  }
		  }
		  | T_DIR {
			   wxFileName fn(PHPParser.getCurrentFileName());
				$$ = fn.GetPath();
		  }
;

function_call_or_not: '(' file_constants ')' { $$ = $2;    }
				    | /* empty */            { $$.Clear(); }
;

file_constants: T_FILE {
					$$ = PHPParser.getCurrentFileName();
				}
				| T_DIR { 
					wxFileName fn(PHPParser.getCurrentFileName());
					$$ = fn.GetPath();
				}
				| /* empty */
;

expr: expr_basic          { $$ = $1;  }
	| expr '.' expr_basic { $$ << $3; }
	| /* empty */
;

%%


