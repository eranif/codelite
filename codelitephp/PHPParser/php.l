%{
#include "php_parser_api.h"

#define ECHO

#ifdef YYSTYPE
#undef YYSTYPE
#endif

#define YYSTYPE wxString

extern YYSTYPE php_lval;
#include "php_parser.cpp.h"

bool is_func_modifier(int x);

#define LEX_RETURN(x) {\
	PHPParser.setYytext(wxString(yytext, wxConvUTF8));\
	PHPParser.setYytype(x);\
	PHPParser.setLineNumber(yylineno);\
	php_lval = wxString(yytext, wxConvUTF8);\
	return x;\
}

%}

%option yylineno


identifier [a-zA-Z_][0-9a-zA-Z_]*

exponent_part [eE][-+]?[0-9]+
fractional_constant ([0-9]*"."[0-9]+)|([0-9]+".")
floating_constant (({fractional_constant}{exponent_part}?)|([0-9]+{exponent_part}))[FfLl]?

integer_suffix_opt ([uU]?[lL]?)|([lL][uU])
decimal_constant [1-9][0-9]*{integer_suffix_opt}
octal_constant "0"[0-7]*{integer_suffix_opt}
hex_constant "0"[xX][0-9a-fA-F]+{integer_suffix_opt}

simple_escape [abfnrtv'"?\\]
octal_escape  [0-7]{1,3}
hex_escape "x"[0-9a-fA-F]+

escape_sequence [\\]({simple_escape}|{octal_escape}|{hex_escape})
c_char [^'\\\n]|{escape_sequence}
s_char [^"\\\n]|{escape_sequence}
WHITESPACE [ \t]+
TABS_AND_SPACES [ \t]*
LABEL [a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*
NEWLINE ("\n"|"\r\n")
h_tab [\011]
form_feed [\014]
v_tab [\013]
c_return [\015]

horizontal_white [ ]|{h_tab}
%x PHP
%x CPP_COMMENT
%x DOC_COMMENT
%x HEREDOC
%x DSTRING
%x SINGLE_STRING

%%
<INITIAL>"<script"{WHITESPACE}+"language"{WHITESPACE}*"="{WHITESPACE}*("php"|"\"php\""|"'php'"){WHITESPACE}*">" {
	BEGIN(PHP);
}
<INITIAL>"<?=" {
	BEGIN(PHP);
}
<INITIAL>"<?php" {
	BEGIN(PHP);
}
<INITIAL>"<?" {
	BEGIN(PHP);
}
<INITIAL>.   {}

<PHP>{horizontal_white}+ {}
<PHP>({horizontal_white}|{v_tab}|{c_return}|{form_feed})*"\n" {}
<PHP>({v_tab}|{c_return}|{form_feed})+ {}
<PHP>"$"{LABEL}             {LEX_RETURN(T_VARIABLE);}
<PHP>b?"<<<"{TABS_AND_SPACES}({LABEL}|([']{LABEL}['])|(["]{LABEL}["])){NEWLINE} {
	// Keep the LABEL, we will need for later
	PHPParser.setPhpHeredocLabel(wxString(yytext, wxConvUTF8));
	BEGIN(HEREDOC);
	return T_START_HEREDOC;
}
<PHP>{decimal_constant}     {LEX_RETURN(T_DNUMBER);}
<PHP>{octal_constant}       {LEX_RETURN(T_LNUMBER);}
<PHP>{hex_constant}         {LEX_RETURN(T_LNUMBER);}
<PHP>{floating_constant}    {LEX_RETURN(T_DNUMBER);}
<PHP>"define"               {LEX_RETURN(T_DEFINE);}
<PHP>"exit"                 {LEX_RETURN(T_EXIT);}
<PHP>"die"                  {LEX_RETURN(T_EXIT);}
<PHP>"function"             {LEX_RETURN(T_FUNCTION);}
<PHP>"const"                {LEX_RETURN(T_CONST);}
<PHP>"return"               {LEX_RETURN(T_RETURN);}
<PHP>"try"                  {LEX_RETURN(T_TRY);}
<PHP>"catch"                {LEX_RETURN(T_CATCH);}
<PHP>"throw"                {LEX_RETURN(T_THROW);}
<PHP>"if"                   {LEX_RETURN(T_IF);}
<PHP>"elseif"               {LEX_RETURN(T_ELSEIF);}
<PHP>"endif"                {LEX_RETURN(T_ENDIF);}
<PHP>"else"                 {LEX_RETURN(T_ELSE);}
<PHP>"while"                {LEX_RETURN(T_WHILE);}
<PHP>"endwhile"             {LEX_RETURN(T_ENDWHILE);}
<PHP>"do"                   {LEX_RETURN(T_DO);}
<PHP>"for"                  {LEX_RETURN(T_FOR);}
<PHP>"endfor"               {LEX_RETURN(T_ENDFOR);}
<PHP>"foreach"              {LEX_RETURN(T_FOREACH);}
<PHP>"endforeach"           {LEX_RETURN(T_ENDFOREACH);}
<PHP>"declare"              {LEX_RETURN(T_DECLARE);}
<PHP>"enddeclare"           {LEX_RETURN(T_ENDDECLARE);}
<PHP>"instanceof"           {LEX_RETURN(T_INSTANCEOF);}
<PHP>"as"                   {LEX_RETURN(T_AS);}
<PHP>"switch"               {LEX_RETURN(T_SWITCH);}
<PHP>"endswitch"            {LEX_RETURN(T_ENDSWITCH);}
<PHP>"case"                 {LEX_RETURN(T_CASE);}
<PHP>"default"              {LEX_RETURN(T_DEFAULT);}
<PHP>"break"                {LEX_RETURN(T_BREAK);}
<PHP>"continue"             {LEX_RETURN(T_CONTINUE);}
<PHP>"goto"                 {LEX_RETURN(T_GOTO);}
<PHP>"echo"                 {LEX_RETURN(T_ECHO);}
<PHP>"print"                {LEX_RETURN(T_PRINT);}
<PHP>"class"                {LEX_RETURN(T_CLASS);}
<PHP>"interface"            {LEX_RETURN(T_INTERFACE);}
<PHP>"trait"                {LEX_RETURN(T_TRAIT);}
<PHP>"extends"              {LEX_RETURN(T_EXTENDS);}
<PHP>"implements"           {LEX_RETURN(T_IMPLEMENTS);}
<PHP>"->"                   {LEX_RETURN(T_OBJECT_OPERATOR);}
<PHP>"::"                   {LEX_RETURN(T_PAAMAYIM_NEKUDOTAYIM);}
<PHP>"\\"                   {LEX_RETURN(T_NS_SEPARATOR);}
<PHP>"new"                  {LEX_RETURN(T_NEW);}
<PHP>"clone"                {LEX_RETURN(T_CLONE);}
<PHP>"var"                  {LEX_RETURN(T_VAR);}
<PHP>"eval"                 {LEX_RETURN(T_EVAL);}
<PHP>"include"              {LEX_RETURN(T_INCLUDE);}
<PHP>"include_once"         {LEX_RETURN(T_INCLUDE_ONCE);}
<PHP>"require"              {LEX_RETURN(T_REQUIRE);}
<PHP>"require_once"         {LEX_RETURN(T_REQUIRE_ONCE);}
<PHP>"namespace"            {LEX_RETURN(T_NAMESPACE);}
<PHP>"use"                  {LEX_RETURN(T_USE);}
<PHP>"insteadof"            {LEX_RETURN(T_INSTEADOF);}
<PHP>"global"               {LEX_RETURN(T_GLOBAL);}
<PHP>"isset"                {LEX_RETURN(T_ISSET);}
<PHP>"empty"                {LEX_RETURN(T_EMPTY);}
<PHP>"static"               {LEX_RETURN(T_STATIC);}
<PHP>"abstract"             {LEX_RETURN(T_ABSTRACT);}
<PHP>"__halt_compiler"      {LEX_RETURN(T_ABSTRACT);}
<PHP>"final"                {LEX_RETURN(T_FINAL);}
<PHP>"private"              {LEX_RETURN(T_PRIVATE);}
<PHP>"protected"            {LEX_RETURN(T_PROTECTED);}
<PHP>"public"               {LEX_RETURN(T_PUBLIC);}
<PHP>"unset"                {LEX_RETURN(T_UNSET);}
<PHP>"=>"                   {LEX_RETURN(T_DOUBLE_ARROW);}
<PHP>"list"                 {LEX_RETURN(T_LIST);}
<PHP>"array"                {LEX_RETURN(T_ARRAY);}
<PHP>"callable"             {LEX_RETURN(T_CALLABLE);}
<PHP>"++"                   {LEX_RETURN(T_INC);}
<PHP>"--"                   {LEX_RETURN(T_DEC);}
<PHP>"==="                  {LEX_RETURN(T_IS_IDENTICAL);}
<PHP>"!=="                  {LEX_RETURN(T_IS_NOT_IDENTICAL);}
<PHP>"=="                   {LEX_RETURN(T_IS_EQUAL);}
<PHP>"!="|"<>"              {LEX_RETURN(T_IS_NOT_EQUAL);}
<PHP>"<="                   {LEX_RETURN(T_IS_SMALLER_OR_EQUAL);}
<PHP>">="                   {LEX_RETURN(T_IS_GREATER_OR_EQUAL);}
<PHP>"+="                   {LEX_RETURN(T_PLUS_EQUAL);}
<PHP>"-="                   {LEX_RETURN(T_MINUS_EQUAL);}
<PHP>"*="                   {LEX_RETURN(T_MUL_EQUAL);}
<PHP>"/="                   {LEX_RETURN(T_DIV_EQUAL);}
<PHP>".="                   {LEX_RETURN(T_CONCAT_EQUAL);}
<PHP>"%="                   {LEX_RETURN(T_MOD_EQUAL);}
<PHP>"<<="                  {LEX_RETURN(T_SL_EQUAL);}
<PHP>">>="                  {LEX_RETURN(T_SR_EQUAL);}
<PHP>"&="                   {LEX_RETURN(T_AND_EQUAL);}
<PHP>"|="                   {LEX_RETURN(T_OR_EQUAL);}
<PHP>"^="                   {LEX_RETURN(T_XOR_EQUAL);}
<PHP>"||"                   {LEX_RETURN(T_BOOLEAN_OR);}
<PHP>"&&"                   {LEX_RETURN(T_BOOLEAN_AND);}
<PHP>"OR"                   {LEX_RETURN(T_LOGICAL_OR);}
<PHP>"AND"                  {LEX_RETURN(T_LOGICAL_AND);}
<PHP>"XOR"                  {LEX_RETURN(T_LOGICAL_XOR);}
<PHP>"<<"                   {LEX_RETURN(T_SL);}
<PHP>">>"                   {LEX_RETURN(T_SR);}
<PHP>"__CLASS__"            {LEX_RETURN(T_CLASS_C);}
<PHP>"__TRAIT__"            {LEX_RETURN(T_TRAIT_C);}
<PHP>"__FUNCTION__"         {LEX_RETURN(T_FUNC_C);}
<PHP>"__METHOD__"           {LEX_RETURN(T_METHOD_C);}
<PHP>"__LINE__"             {LEX_RETURN(T_LINE);}
<PHP>"__FILE__"             {LEX_RETURN(T_FILE);}
<PHP>"__DIR__"              {LEX_RETURN(T_DIR);}
<PHP>"__NAMESPACE__"        {LEX_RETURN(T_NS_C);}
<PHP>"yield"                {LEX_RETURN(T_YIELD);}
<PHP>"("{TABS_AND_SPACES}("int"|"integer"){TABS_AND_SPACES}")" {
	LEX_RETURN(T_INT_CAST);
}

<PHP>"("{TABS_AND_SPACES}("real"|"double"|"float"){TABS_AND_SPACES}")" {
	LEX_RETURN(T_DOUBLE_CAST);
}

<PHP>"("{TABS_AND_SPACES}("string"|"binary"){TABS_AND_SPACES}")" {
	LEX_RETURN(T_STRING_CAST);
}

<PHP>"("{TABS_AND_SPACES}"array"{TABS_AND_SPACES}")" {
	LEX_RETURN(T_ARRAY_CAST);
}

<PHP>"("{TABS_AND_SPACES}"object"{TABS_AND_SPACES}")" {
	LEX_RETURN(T_OBJECT_CAST);
}

<PHP>"("{TABS_AND_SPACES}("bool"|"boolean"){TABS_AND_SPACES}")" {
	return T_BOOL_CAST;
}

<PHP>"("{TABS_AND_SPACES}("unset"){TABS_AND_SPACES}")" {
	return T_UNSET_CAST;
}
<PHP>"#"|"//"               {BEGIN(CPP_COMMENT);}
<PHP>"/*"                   {
	BEGIN(DOC_COMMENT);
	PHPParser.clearDocComment();
}
<PHP>{LABEL}                {LEX_RETURN(T_STRING);}
<PHP>("?>"|"</script"{WHITESPACE}*">")"\n"? {
	BEGIN(INITIAL);
	return T_CLOSE_TAG;
}
<PHP>"%>"{NEWLINE}? {
	BEGIN(INITIAL);
}
<PHP>"'"                      {BEGIN(SINGLE_STRING); php_lval = wxT("'");}
<PHP>\"                       {BEGIN(DSTRING); php_lval = wxT("\""); }
<PHP><<EOF>>                  {
	PHPParser.setYytext(wxT(""));
	PHPParser.setYytype(0);
	PHPParser.setLineNumber(php_lineno);
	php_lval = wxString(yytext, wxConvUTF8);\
	yyterminate();
	return 0;
}
<PHP>.                     {LEX_RETURN(yytext[0]);}

<SINGLE_STRING>"\n" {}
<SINGLE_STRING>{c_char}  {php_lval << wxString(yytext, wxConvUTF8);}
<SINGLE_STRING>"'"               {
	BEGIN(PHP);
	php_lval << wxT("'");
	PHPParser.setYytext(php_lval);
	PHPParser.setYytype(T_CONSTANT_ENCAPSED_STRING);
	PHPParser.setLineNumber(yylineno);
	return T_CONSTANT_ENCAPSED_STRING;
}
<SINGLE_STRING>. {}

<DSTRING>"\n" {}
<DSTRING>{s_char}  {php_lval << wxString(yytext, wxConvUTF8);}
<DSTRING>["]                     {
	BEGIN(PHP);
	php_lval << wxT("\"");
	PHPParser.setYytext(php_lval);
	PHPParser.setYytype(T_CONSTANT_ENCAPSED_STRING);
	PHPParser.setLineNumber(yylineno);
	return T_CONSTANT_ENCAPSED_STRING;
}
<DSTRING>.         {}

<HEREDOC>^{LABEL}{NEWLINE}?";" {
	wxString label(yytext, wxConvUTF8);
	label.RemoveLast();
	int sizeToReturn = label.Len();
	label.Trim().Trim(false);
	if(label == PHPParser.getPhpHeredocLabel()) {
		yyless(sizeToReturn); // put back the ';'
		BEGIN(PHP);
		return T_END_HEREDOC;

	} else {
		// do nothing
	}
}
<HEREDOC>"\n"   {}
<HEREDOC>.      {}

<CPP_COMMENT>{NEWLINE} {BEGIN(PHP);}
<CPP_COMMENT>.         {}

<DOC_COMMENT>"*/" {
	PHPParser.setDocCommentLine(php_lineno);
	BEGIN(PHP);
}
<DOC_COMMENT>{NEWLINE} {PHPParser.appendDocComment(wxT("\n"));}
<DOC_COMMENT>. {
	PHPParser.appendDocComment(yytext[0]);
}
%%

extern int php_parse();

int php_wrap(){return 1;}

int PHP_Scan( const wxString &filePath)
{
	BEGIN INITIAL;
	php_lineno = 1;
	FILE* fp = fopen(filePath.To8BitData(), "rb");
	if ( fp == NULL ) {
		//printf("%s\n", strerror(errno));
		// failed to open input file...
		return -1;
	}

	yy_switch_to_buffer( yy_create_buffer(fp, YY_BUF_SIZE) );
	int rc = php_parse();

    if ( YY_CURRENT_BUFFER->yy_input_file ) {
        fclose( YY_CURRENT_BUFFER->yy_input_file );
        YY_CURRENT_BUFFER->yy_input_file = NULL;

    }

    yy_delete_buffer    ( YY_CURRENT_BUFFER    );
    yyterminate();
	return rc;
}

int PHP_Lex( const wxString &filePath)
{
	BEGIN INITIAL;
	php_lineno = 1;
	FILE* fp = fopen(filePath.To8BitData(), "rb");
	if ( fp == NULL ) {
		return -1;
	}

	yy_switch_to_buffer( yy_create_buffer(fp, YY_BUF_SIZE) );
	php_in = fp;
	int type = 0;
	while( true ) {
		type = php_lex();
		if(type == 0) break;
	}

    if ( YY_CURRENT_BUFFER->yy_input_file ) {
        fclose( YY_CURRENT_BUFFER->yy_input_file );
        YY_CURRENT_BUFFER->yy_input_file = NULL;

    }

    yy_delete_buffer    ( YY_CURRENT_BUFFER    );
	return type;
}

int PHP_ScanString( const wxString &inputString )
{
	BEGIN INITIAL;
	yy_scan_string( inputString.To8BitData() );
	php_lineno = 1;

	int rc = php_parse();

    yyterminate();
	return rc;
}

void php_error(char* st)
{
}

void php_less(int count){
	yyless(count);
}

bool is_func_modifier(int x) {
	switch(x){
	case T_PUBLIC   :
	case T_PROTECTED:
	case T_PRIVATE  :
	case T_STATIC   :
	case T_ABSTRACT :
	case T_FINAL:
	case T_FUNCTION:
		return true;
	default:
		return false;
	}
}
