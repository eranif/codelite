%x PRE_PROCESSOR_STATE
%x CATCH_STATE
%x FOR_STATE
%x C_COMMENT_STATE
%x CPP_COMMENT_STATE
%x LAMBDA_STATE


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

lambda_1 \[[ \t]*&[ \t]*\]
lambda_2 \[[ \t]*=[ \t]*\]
lambda_3 \[[ \t]*this[ \t]*\]
lambda ({lambda_1}|{lambda_2}|{lambda_3})

%{
// Avoid spam output
#ifdef  ECHO
#undef  ECHO
#endif
#define ECHO

// Return values for lambda state
#define __IDENTIFIER 900

// Never exit
#ifdef  YY_FATAL_ERROR
#undef  YY_FATAL_ERROR
#endif
#define YY_FATAL_ERROR(msg)

#include <vector>
#include <string>

struct ScopeEntry {
    std::string str;
    int         line;

    ScopeEntry() : line(-1) {}
    void clear() {
        line = -1;
        str.clear();
    }
};

static std::vector<ScopeEntry> scope_stack;
static ScopeEntry              current_scope;
static int                     current_state       = 0;
static int                     current_brace_depth = 1;
static std::string             catch_for_scope;
static void consume_lambda_signature();

#define PUSH_SCOPE(scope) {\
    if(scope.line == -1)\
        scope.line = scope_optimizer_lineno;\
    scope_stack.push_back(scope);\
}

%}

%option yylineno

%%

<*>"//" {
    BEGIN(CPP_COMMENT_STATE);
}

<*>"/*" {
    BEGIN(C_COMMENT_STATE);
}

{horizontal_white}+                                                     {current_scope.str += " ";}
({v_tab}|{c_return}|{form_feed})+                                       {current_scope.str += " ";}
({horizontal_white}|{v_tab}|{c_return}|{form_feed})*"\n"                {current_scope.str += " ";}

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
{lambda}[ ]*\( {
    BEGIN(LAMBDA_STATE);
    consume_lambda_signature();
    BEGIN(INITIAL);
}
<LAMBDA_STATE>{identifier} {
    return __IDENTIFIER;
}
<LAMBDA_STATE>. {
    return yytext[0];
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
        current_brace_depth = 1;
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
    current_scope.str += "(";
    if(current_scope.str.empty() == false) {
        PUSH_SCOPE(current_scope);
    }
    current_scope.clear();
}
\)                     {
    if ( !scope_stack.empty() ) {
        current_scope = scope_stack.back();
        scope_stack.pop_back();
        
        current_scope.str += ")";
    } else {
        current_scope.clear();
    }
}
\{                     {
    current_scope.str += "{";
    PUSH_SCOPE(current_scope);
    current_scope.clear();
    
    if(catch_for_scope.empty() == false)
        current_scope.str += catch_for_scope;
    catch_for_scope.clear();
}
\}                     {
    if ( !scope_stack.empty() ) {
        current_scope = scope_stack.back();
        
        scope_stack.pop_back();
        current_scope.str += "}";
    } else {
        current_scope.clear();
    }
}
auto                {current_scope.str += yytext;}
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
class               { current_scope.str += yytext; }
struct              { current_scope.str += yytext; }
namespace           { current_scope.str += yytext; }
delete              {}
friend              {}
inline              {}
operator            {}
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
using               { current_scope.str += yytext; }
throw               { current_scope.str += yytext; }
{identifier}        |
.                   {
    if(catch_for_scope.empty() == false)
        catch_for_scope.clear();
        
    // default keep the current token
    current_scope.str += yytext;
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
    if(current_scope.str.empty() == false) {
        PUSH_SCOPE(current_scope);
        current_scope.clear();
    }
    yyterminate();
}
%%

int yywrap() {
    return 1; 
}

static void consume_lambda_signature() {
    int depth = 1;
    catch_for_scope += ";";
    std::string curarg;
    while(true) {
        int rc = scope_optimizer_lex();
        if(rc == 0) break;
        switch(rc) {
        case '<': 
            ++depth;
            curarg += "< ";
            break;
        case '>':
            --depth;
            curarg += "> ";
            break;
        case '(':
            ++depth;
            curarg += ")";
            break;
        case ')':
            --depth;
            if(depth == 0) {
                if(!curarg.empty()) {
                    curarg += ";";
                    catch_for_scope += curarg;
                }
                return;
            } else {
                curarg += ")";
            }
            break;
        case ',':
            if(depth == 1) {
                curarg += ";";
                catch_for_scope += curarg;
                curarg.clear();
            } else {
                curarg += ",";
            }
            break;
        default:
            curarg += yytext;
            break;
        }
    }
}

void scope_optimizer_clean()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
    yy_delete_buffer(YY_CURRENT_BUFFER);
    
    scope_stack.clear();
    current_scope.clear();
    current_state = INITIAL;
    current_brace_depth = 1;
    catch_for_scope.clear();
}

int OptimizeScope(const std::string &inputScope, std::string &optimizedScope, int localsLine, std::string &localsScope)
{
    BEGIN 0;
    yy_scan_string(inputScope.c_str());
    
    int rc = scope_optimizer_lex();
    if (scope_stack.empty()) {
        optimizedScope = inputScope;
        scope_optimizer_clean();
        return rc;
    }
    
    std::string tmp_scope;
    for (size_t i = 0; i < scope_stack.size(); i++) {
        tmp_scope += scope_stack.at(i).str;
        if(scope_stack.at(i).line >= localsLine) {
            localsScope += scope_stack.at(i).str;
        }
    }

    // if the current scope is not empty, terminate it with ';' and return
    if ( tmp_scope.empty() == false ) {
        tmp_scope  += ";";
        optimizedScope = tmp_scope;
    }
    scope_optimizer_clean();
    return rc;
}

