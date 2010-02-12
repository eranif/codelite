%x PRE_PROCESSOR_STATE
%x CATCH_STATE
%x FOR_STATE
%x C_COMMENT_STATE
%x CPP_COMMENT_STATE

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


%{
// Avoid spam output
#ifdef  ECHO
#undef  ECHO
#endif
#define ECHO

// Never exit
#ifdef  YY_FATAL_ERROR
#undef  YY_FATAL_ERROR
#endif
#define YY_FATAL_ERROR(msg)

#include <vector>
#include <string>

static std::vector<std::string> scope_stack;
static std::string              current_scope;
static int                      current_state       = INITIAL;
static int                      current_brace_depth = 1;
static std::string              catch_for_scope;
%}

%%

<*>"//" {
	BEGIN(CPP_COMMENT_STATE);
}

<*>"/*" {
	BEGIN(C_COMMENT_STATE);
}

{horizontal_white}+                                                     {current_scope += " ";}
({v_tab}|{c_return}|{form_feed})+                                       {current_scope += " ";}
({horizontal_white}|{v_tab}|{c_return}|{form_feed})*"\n"                {current_scope += " ";}

<CATCH_STATE>{horizontal_white}+                                        {catch_for_scope += " ";}
<CATCH_STATE>({v_tab}|{c_return}|{form_feed})+                          {catch_for_scope += " ";}
<CATCH_STATE>({horizontal_white}|{v_tab}|{c_return}|{form_feed})*"\n"   {catch_for_scope += " ";}
<FOR_STATE>{horizontal_white}+                                          {catch_for_scope += " ";}
<FOR_STATE>({v_tab}|{c_return}|{form_feed})+                            {catch_for_scope += " ";}
<FOR_STATE>({horizontal_white}|{v_tab}|{c_return}|{form_feed})*"\n"     {catch_for_scope += " ";}
			
"L"?[']{c_char}+[']     {/* eat a string */}
"L"?["]{s_char}*["]     {/* eat a string */}
"#"                     {BEGIN(PRE_PROCESSOR_STATE);}
<PRE_PROCESSOR_STATE>\         {} /* continue same state on next line */
<PRE_PROCESSOR_STATE>\n        {BEGIN(current_state);}
<PRE_PROCESSOR_STATE>.         {} /* consume it */
catch                   {
	current_state = CATCH_STATE;
	catch_for_scope.clear();
	BEGIN(CATCH_STATE);
}
<CATCH_STATE>\(         {catch_for_scope += ";";}
<CATCH_STATE>\)         {
	catch_for_scope += ";"; 
	current_state = INITIAL; 
	BEGIN(INITIAL);
}
<CATCH_STATE>.          {
	// default keep the current token
	catch_for_scope += yytext;
}
for[ ]*\(               {
	current_state = FOR_STATE;
	BEGIN(FOR_STATE);
}
<FOR_STATE>\(           {
	current_brace_depth++;
	catch_for_scope += yytext;
}

<FOR_STATE>\)           {
	current_brace_depth--;
	if(current_brace_depth == 0) {
		current_state = INITIAL; 
		catch_for_scope += ";";
		BEGIN(INITIAL);
	} else {
		catch_for_scope += yytext;
	}
}
<FOR_STATE>.          {
	// default keep the current token
	catch_for_scope += yytext;
}
\(                     {
	current_scope += "(";
	if(current_scope.empty() == false) {
		scope_stack.push_back(current_scope);
	}
	current_scope.clear();
}
\)                     {
	if ( !scope_stack.empty() ) {
		current_scope = scope_stack.back();
		scope_stack.pop_back();
		
		current_scope += ")";
	} else {
		current_scope.clear();
	}
}
\{                     {
	current_scope += "{";
	scope_stack.push_back(current_scope);
	current_scope.clear();
	
	if(catch_for_scope.empty() == false)
		current_scope += catch_for_scope;
	catch_for_scope.clear();
}
\}                     {
	if ( !scope_stack.empty() ) {
		current_scope = scope_stack.back();
		
		scope_stack.pop_back();
		current_scope += "}";
	} else {
		current_scope.clear();
	}
}
auto                {}
break               {}
case                {}
continue            {}
default             {}
define              {}
defined             {}
do                  {}
elif                {}
else                {}
endif               {}
error               {}
goto                {}
if                  {}
ifdef               {}
ifndef              {}
include             {}
line                {}
pragma              {}
return              {}
sizeof              {}
switch              {}
undef               {}
union               {}
while               {}
class               {}
namespace           {}
delete              {}
friend              {}
inline              {}
operator            {}
overload            {}
protected           {}
private             {}
public              {}
virtual             {}
template            {}
try                 {}
typename            {}
dynamic_cast        {}
static_cast         {}
const_cast          {}
reinterpret_cast    {}
using               {}
throw               {}
.                   {
	if(catch_for_scope.empty() == false)
		current_scope += catch_for_scope;
	catch_for_scope.clear();
	// default keep the current token
	current_scope += yytext;
}
<CPP_COMMENT_STATE>\n     {
	BEGIN(current_state);
}
<CPP_COMMENT_STATE>.      {}

<C_COMMENT_STATE>"*/"     {
	BEGIN(current_state);
}
<C_COMMENT_STATE>.        {}
<<EOF>> {
	if(current_scope.empty() == false) {
		scope_stack.push_back( current_scope );
		current_scope.clear();
	}
	yyterminate();
}
%%

int yywrap() {
	return 1; 
}

int OptimizeScope(const std::string &inputScope, std::string &optimizedScope)
{
	BEGIN INITIAL;
	current_state = INITIAL;
	current_scope.clear();
	yy_scan_string(inputScope.c_str());
	
	int rc = scope_optimizer_lex();
	if (scope_stack.empty()) {
		optimizedScope = inputScope;
		return rc;
	}
	
	size_t i = 0;
	current_scope.clear();
	for (; i < scope_stack.size(); i++)
		current_scope += scope_stack.at(i);
	
	// if the current scope is not empty, terminate it with ';' and return
	if ( current_scope.empty() == false ) {
		current_scope += ";";
		optimizedScope = current_scope;
	}
	return rc;
}

