%{

    /* Copyright (C) 1989-1991 James A. Roskind, All rights reserved.
    This lexer description was written by James A.  Roskind.  Copying
    of  this  file, as a whole, is permitted providing this notice is
    intact  and  applicable   in   all   complete   copies.    Direct
    translations  as a whole to other lexer generator input languages
    (or lexical description languages)  is  permitted  provided  that
    this  notice  is  intact and applicable in all such copies, along
    with a disclaimer that  the  contents  are  a  translation.   The
    reproduction  of derived files or text, such as modified versions
    of this file, or the output of scanner generators, is  permitted,
    provided   the  resulting  work  includes  the  copyright  notice
    "Portions Copyright (c) 1989, 1990 James  A.   Roskind".  Derived
    products  must  also  provide  the notice "Portions Copyright (c)
    1989, 1990 James A.  Roskind" in  a  manner  appropriate  to  the
    utility,   and  in  keeping  with  copyright  law  (e.g.:  EITHER
    displayed when first invoked/executed; OR displayed  continuously
    on  display terminal; OR via placement in the object code in form
    readable in a printout, with or near the title of the work, or at
    the end of the file).  No royalties, licenses or  commissions  of
    any  kind  are  required  to copy this file, its translations, or
    derivative products, when the copies are made in compliance  with
    this  notice.  Persons  or  corporations  that  do make copies in
    compliance  with  this  notice  may  charge  whatever  price   is
    agreeable  to  a buyer, for such copies or derivative works. THIS
    FILE IS PROVIDED ``AS IS'' AND WITHOUT  ANY  EXPRESS  OR  IMPLIED
    WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

    James A. Roskind
    Independent Consultant
    516 Latania Palm Drive
    Indialantic FL, 32903
    (407)729-4348
    jar@hq.ileaf.com
    or ...!uunet!leafusa!jar

    ---end of copyright notice---


COMMENTS-

My  goal  is  to  see  software  developers adopt my C++ grammar as a
standard until such time as a better  standard  is  accessible.   The
only  way  to  get it to become a standard, is to be sure that people
know that derivations are based on a specific work.   The  intent  of
releasing  this Flex input file is to facilitate experimentation with
my C++ grammar. The intent  of  the  copyright  notice  is  to  allow
arbitrary  commercial and non-commercial use of this file, as long as
reference is given to my standardization effort.   Without  reference
to  a specific standard, many alternative grammars would develop.  By
referring to the standard, the C++ grammar is given publicity,  which
should  lead  to further use in compatible products and systems.  The
benefits  of  such  a  standard  to  commercial  products  (browsers,
beautifiers,  translators,  compilers,  ...) should be obvious to the
developers, in that other compatible products will  emerge,  and  the
value  of  all  conforming  products  will rise.  Most developers are
aware of the value of acquiring  a  fairly  complete  grammar  for  a
language,  and  the  copyright  notice (and the resulting affiliation
with my work) should not be too high a price to pay.  By copyrighting
my work, I have some minor control over what this standard is, and  I
can  (hopefully)  keep it from degrading without my approval.  I will
consistently attempt to provide upgraded grammars that are  compliant
with  the  current  art, and the ANSI C++ Committee recommendation in
particular.  A  developer  is  never  prevented  from  modifying  the
grammar or this file to improve it in whatever way is seen fit. There
is  also  no  restriction on the sale of copies, or derivative works,
providing the request in the copyright notice are satisfied.

If you are not "copying" my work, but  are  rather  only  abstracting
some of my work, an acknowledgment with references to such a standard
would  be  appreciated.  Specifically, agreements with my grammar and
its resolution of otherwise ambiguous constructs, should be noted.

Simply put: "make whatever use you would like of the grammar and this
file, but include the ``portions Copyright ...'' as  a  reference  to
this standard."


*/

/* Last modified 7/4/91, Version 2.0 */

/* File cpp5.l, becomes lex.yy.c after processing by FLEX  */

/* This file is a dramatically cut down version of the FLEX input file
used in  my  ANSI  C  Preprocessor.   The  executable  version  of  my
preprocessor  is  available on many platforms (shareware), but this is
the only source extract that is currently being distributed.   If  you
need   a   full   ANSI   C  preprocessor,  with  extensive  diagnostic
capabilities and customization facilities, please contact  me  at  the
addresses given above.  Current platforms include IBMPC (DOS/OS2), Sun
(SPARC   and  Motorola),  and  IBM  R/6000.   ...  end  of  commercial
announcement.

This file is being distributed to facilitate experimentation  and  use
of my C and C++ grammar.


Comment  removal  must  be done during the lexing, as context (such as
enclosure in string literals) must be  observed.   For  this  cut-down
lexer,  we  will  assume that comments have been removed (don't assume
this if you are writing a compiler or browser!).


/* For each IDENTIFIER like string that is found,  there  are  several
distinct interpretations that can be applied:

1)  The  preprocessor  may  interpret  the  string as a "keyword" in a
directive (eg: "pragma" or "include", "defined").

2) The parser may interpret the string as a keyword. (eg: "int").

3) Both parser and preprocessor may interpret the string as a  keyword
(eg: "if").

Since  this  file  is based on source that actually lexically analyses
text for both preprocessing and parsing, macro definitions  were  used
throughout.   The macro definitions supplied here have been customized
to a C++ parse only, and  all  preprocessor  keywords  are  passed  as
IDENTIFIER  or  TYPEDEFname.   Also, since there is no symbol table to
interrogate to decide whether a string  is  a  TYPEDEFname,  I  simply
assume  that  any  identifier beginning with an upper case letter is a
TYPEDEFname.  This hack  should  allow  you  to  check  out  how  code
segments  are  parsed  using my grammar.  Unfortunately, if you really
want to parse major league code, you have to write a symbol table, and
maintain appropriate scoping information.



*/


/* Included code before lex code */
/*************** Includes and Defines *****************************/


#include "y.tab.h" /* YACC generated definitions based on C++ grammar */

typedef char * YYSTYPE; /* interface with lexer: should be  in  header
                        file*/

extern char  *  yylval;  /*  We  will always point at the text of the lexeme.
          This makes it easy to print out nice trees when  YYDEBUG  is
          enabled.   (see  C++  grammar  file  and  its  definition of
          YYDEBUG_LEXER_TEXT to be "yylval" */

#include <stdlib.h>
#include <string.h>

/* Prototypes */


#define WHITE_RETURN(x) /* do nothing */

#define NEW_LINE_RETURN() WHITE_RETURN('\n')

#define PA_KEYWORD_RETURN(x)   RETURN_VAL(x)  /* standard C PArser Keyword */
#define CPP_KEYWORD_RETURN(x)  PA_KEYWORD_RETURN(x)  /* C++ keyword */
#define PPPA_KEYWORD_RETURN(x) RETURN_VAL(x)  /* both PreProcessor and PArser keyword */
#define PP_KEYWORD_RETURN(x)   IDENTIFIER_RETURN()

#define IDENTIFIER_RETURN() RETURN_VAL(IDENTIFIER)

#define PPOP_RETURN(x)       RETURN_VAL((int)*yytext) /* PreProcess and Parser operator */
#define NAMED_PPOP_RETURN(x) RETURN_VAL(x)
#define ASCIIOP_RETURN(x)    RETURN_VAL((int)*yytext) /* a single character operator */
#define NAMEDOP_RETURN(x)    RETURN_VAL(x)            /* a multichar operator, with a name */

#define NUMERICAL_RETURN(x) RETURN_VAL(x)            /* some sort of constant */
#define LITERAL_RETURN(x)   RETURN_VAL(x)            /* a string literal */
#define C_COMMENT_RETURN(x) RETURN_VAL(x)	     /* C Style comment  */
#define RETURN_VAL(x) return(x);
#define yywrap() 1



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


h_tab [\011]
form_feed [\014]
v_tab [\013]
c_return [\015]

horizontal_white [ ]|{h_tab}


%%

"/*" {
	/* Handle C-Style comment */
	register int c;
	if(m_keepComments) m_comment += yytext;
	for ( ; ; )
	{
		while ( (c = yyinput()) != '*' && c != EOF )
		{
			if(m_keepComments) m_comment += c;
			
			; /* eat up text of comment */
		}
		if(m_keepComments) m_comment += c;
		if ( c == '*' )
		{
			while ( (c = yyinput()) == '*' )
			{
				if(m_keepComments) m_comment += c;
				;
			}
			if(m_keepComments) m_comment += c;
			if ( c == '/' )
			{
				break; /* found the end */
			}
		}
		if ( c == EOF )
		{
			break;
		}
	}
	if(m_keepComments ) return CComment;
     }

"//" {
	/* Handle CPP style comment 		 */
	/* Constume everything until the newline */
	register int c;
	if(m_keepComments) m_comment += yytext;
	while((c = yyinput()) != '\n' && c != EOF)
	{
		if(m_keepComments) m_comment += c;
	}
	if(m_keepComments) m_comment += c;
	if(m_keepComments ) return CPPComment;
     }

{horizontal_white}+     {
			if(!m_returnWhite)
				WHITE_RETURN(' ');
			else
				return ' ';
					
			}

({v_tab}|{c_return}|{form_feed})+   {
			if(!m_returnWhite)
				WHITE_RETURN(' ');
			else
				return ' ';
			}


({horizontal_white}|{v_tab}|{c_return}|{form_feed})*"\n"   {
			if(!m_returnWhite)
				NEW_LINE_RETURN();
			else
				return '\n';
			}

auto                {PA_KEYWORD_RETURN(AUTO);}
break               {PA_KEYWORD_RETURN(BREAK);}
case                {PA_KEYWORD_RETURN(CASE);}
char                {PA_KEYWORD_RETURN(lexCHAR);}
const               {PA_KEYWORD_RETURN(lexCONST);}
continue            {PA_KEYWORD_RETURN(CONTINUE);}
default             {PA_KEYWORD_RETURN(lexDEFAULT);}
define             {PP_KEYWORD_RETURN(DEFINE);}
defined            {PP_KEYWORD_RETURN(OPDEFINED);}
do                  {PA_KEYWORD_RETURN(DO);}
double              {PA_KEYWORD_RETURN(lexDOUBLE);}
elif                {PP_KEYWORD_RETURN(ELIF);}
else              {PPPA_KEYWORD_RETURN(ELSE);}
endif              {PP_KEYWORD_RETURN(ENDIF);}
enum                {PA_KEYWORD_RETURN(ENUM);}
error              {PP_KEYWORD_RETURN(ERROR);}
extern              {PA_KEYWORD_RETURN(EXTERN);}
float               {PA_KEYWORD_RETURN(lexFLOAT);}
for                 {PA_KEYWORD_RETURN(FOR);}
goto                {PA_KEYWORD_RETURN(GOTO);}
if                {PPPA_KEYWORD_RETURN(IF);}
ifdef              {PP_KEYWORD_RETURN(IFDEF);}
ifndef             {PP_KEYWORD_RETURN(IFNDEF);}
include            {PP_KEYWORD_RETURN(INCLUDE); }
int                 {PA_KEYWORD_RETURN(lexINT);}
line               {PP_KEYWORD_RETURN(LINE);}
long                {PA_KEYWORD_RETURN(lexLONG);}
pragma             {PP_KEYWORD_RETURN(PRAGMA);}
register            {PA_KEYWORD_RETURN(REGISTER);}
return              {PA_KEYWORD_RETURN(RETURN);}
short               {PA_KEYWORD_RETURN(SHORT);}
signed              {PA_KEYWORD_RETURN(SIGNED);}
sizeof              {PA_KEYWORD_RETURN(SIZEOF);}
static              {PA_KEYWORD_RETURN(STATIC);}
struct              {PA_KEYWORD_RETURN(lexSTRUCT);}
switch              {PA_KEYWORD_RETURN(SWITCH);}
typedef             {PA_KEYWORD_RETURN(TYPEDEF);}
undef              {PP_KEYWORD_RETURN(UNDEF);}
union               {PA_KEYWORD_RETURN(UNION);}
unsigned            {PA_KEYWORD_RETURN(UNSIGNED);}
void                {PA_KEYWORD_RETURN(lexVOID);}
volatile            {PA_KEYWORD_RETURN(VOLATILE);}
while               {PA_KEYWORD_RETURN(WHILE);}


class               {CPP_KEYWORD_RETURN(lexCLASS);}
namespace           {CPP_KEYWORD_RETURN(lexNAMESPACE);}
delete              {CPP_KEYWORD_RETURN(lexDELETE);}
friend              {CPP_KEYWORD_RETURN(FRIEND);}
inline              {CPP_KEYWORD_RETURN(INLINE);}
new                 {CPP_KEYWORD_RETURN(NEW);}
operator            {CPP_KEYWORD_RETURN(OPERATOR);}
overload            {CPP_KEYWORD_RETURN(OVERLOAD);}
protected           {CPP_KEYWORD_RETURN(lexPROTECTED);}
private             {CPP_KEYWORD_RETURN(lexPRIVATE);}
public              {CPP_KEYWORD_RETURN(lexPUBLIC);}
this                {CPP_KEYWORD_RETURN(lexTHIS);}
virtual             {CPP_KEYWORD_RETURN(VIRTUAL);}

{identifier}          {IDENTIFIER_RETURN();}

{decimal_constant}  {NUMERICAL_RETURN(INTEGERconstant);}
{octal_constant}    {NUMERICAL_RETURN(OCTALconstant);}
{hex_constant}      {NUMERICAL_RETURN(HEXconstant);}
{floating_constant} {NUMERICAL_RETURN(FLOATINGconstant);}


"L"?[']{c_char}+[']     {
			NUMERICAL_RETURN(CHARACTERconstant);
			}


"L"?["]{s_char}*["]     {
			LITERAL_RETURN(STRINGliteral);}




"("                  {PPOP_RETURN(LP);}
")"                  {PPOP_RETURN(RP);}
","                  {PPOP_RETURN(COMMA);}
"#"                  {NAMED_PPOP_RETURN('#') ;}
"##"                 {NAMED_PPOP_RETURN(POUNDPOUND);}

"{"                  {ASCIIOP_RETURN(LC);}
"}"                  {ASCIIOP_RETURN(RC);}
"["                  {ASCIIOP_RETURN(LB);}
"]"                  {ASCIIOP_RETURN(RB);}
"."                  {ASCIIOP_RETURN(DOT);}
"&"                  {ASCIIOP_RETURN(AND);}
"*"                  {ASCIIOP_RETURN(STAR);}
"+"                  {ASCIIOP_RETURN(PLUS);}
"-"                  {ASCIIOP_RETURN(MINUS);}
"~"                  {ASCIIOP_RETURN(NEGATE);}
"!"                  {ASCIIOP_RETURN(NOT);}
"/"                  {ASCIIOP_RETURN(DIV);}
"%"                  {ASCIIOP_RETURN(MOD);}
"<"                  {ASCIIOP_RETURN(LT);}
">"                  {ASCIIOP_RETURN(GT);}
"^"                  {ASCIIOP_RETURN(XOR);}
"|"                  {ASCIIOP_RETURN(PIPE);}
"?"                  {ASCIIOP_RETURN(QUESTION);}
":"                  {ASCIIOP_RETURN(COLON);}
";"                  {ASCIIOP_RETURN(SEMICOLON);}
"="                  {ASCIIOP_RETURN(ASSIGN);}

".*"                 {NAMEDOP_RETURN(DOTstar);}
"::"                 {NAMEDOP_RETURN(CLCL);}
"->"                 {NAMEDOP_RETURN(ARROW);}
"->*"                {NAMEDOP_RETURN(ARROWstar);}
"++"                 {NAMEDOP_RETURN(ICR);}
"--"                 {NAMEDOP_RETURN(DECR);}
"<<"                 {NAMEDOP_RETURN(LS);}
">>"                 {NAMEDOP_RETURN(RS);}
"<="                 {NAMEDOP_RETURN(LE);}
">="                 {NAMEDOP_RETURN(GE);}
"=="                 {NAMEDOP_RETURN(EQ);}
"!="                 {NAMEDOP_RETURN(NE);}
"&&"                 {NAMEDOP_RETURN(ANDAND);}
"||"                 {NAMEDOP_RETURN(OROR);}
"*="                 {NAMEDOP_RETURN(MULTassign);}
"/="                 {NAMEDOP_RETURN(DIVassign);}
"%="                 {NAMEDOP_RETURN(MODassign);}
"+="                 {NAMEDOP_RETURN(PLUSassign);}
"-="                 {NAMEDOP_RETURN(MINUSassign);}
"<<="                {NAMEDOP_RETURN(LSassign);}
">>="                {NAMEDOP_RETURN(RSassign);}
"&="                 {NAMEDOP_RETURN(ANDassign);}
"^="                 {NAMEDOP_RETURN(ERassign);}
"|="                 {NAMEDOP_RETURN(ORassign);}
"..."                {NAMEDOP_RETURN(ELLIPSIS);}
.                    {return yytext[0];}


%%

/*******************************************************************/
