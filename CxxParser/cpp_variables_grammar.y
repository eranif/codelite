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

void cl_scope_error(char *string);
int  cl_var_parse();
void syncParser();
void var_consumeDefaultValue(char c1, char c2);
void var_consumeDefaultValueIfNeeded();

static  VariableList *        gs_vars = NULL;
static  std::vector<Variable> gs_names;
static  bool                  g_isUsedWithinFunc = false;
static  std::string           s_tmpString;
static  Variable              curr_var;
static  std::string           s_templateInitList;
static  bool                  isBasicType = false;

//---------------------------------------------
// externs defined in the lexer
//---------------------------------------------
extern char *cl_scope_text;
extern int cl_scope_lex();
extern void cl_scope_less(int count);

extern int cl_scope_lineno;
extern std::vector<std::string> currentScope;
extern bool setLexerInput(const std::string &in, const std::map<std::string, std::string> &ignoreMap);
extern void setUseIgnoreMacros(bool ignore);
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
%token  LE_TEMPLATE           LE_TYPENAME
%token  LE_THROW              LE_CATCH
/* ANSI C Grammar suggestions */
%token  LE_IDENTIFIER              LE_STRINGliteral
%token  LE_FLOATINGconstant        LE_INTEGERconstant        LE_CHARACTERconstant
%token  LE_OCTALconstant           LE_HEXconstant
%token  LE_POUNDPOUND LE_CComment LE_CPPComment LE_NAMESPACE LE_USING

/* New Lexical element, whereas ANSI C suggested non-terminal */
%token  LE_TYPEDEFname

/* Multi-Character operators */
%token   LE_ARROW                                                        /*    ->                              */
%token   LE_ICR LE_DECR                                                 /*    ++      --                      */
%token   LE_LS LE_RS                                                    /*    <<      >>                      */
%token   LE_LE LE_GE LE_EQ LE_NE                                      /*    <=      >=      ==      !=      */
%token   LE_ANDAND LE_OROR                                              /*    &&      ||                      */
%token   LE_ELLIPSIS                                                     /*    ...                             */
            /* Following are used in C++, not ANSI C        */
%token   LE_CLCL                                                         /*    ::                              */
%token   LE_DOTstar LE_ARROWstar                                        /*    .*       ->*                    */

/* modifying assignment operators */
%token  LE_MULTassign  LE_DIVassign    LE_MODassign       /*   *=      /=      %=      */
%token  LE_PLUSassign  LE_MINUSassign                      /*   +=      -=              */
%token  LE_LSassign    LE_RSassign                         /*   <<=     >>=             */
%token  LE_ANDassign   LE_ERassign     LE_ORassign        /*   &=      ^=      |=      */
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

basic_type_name:    LE_UNSIGNED basic_type_name_inter   { $$ = $1 + " " + $2; }
                |   LE_SIGNED basic_type_name_inter     { $$ = $1 + " " + $2; }
                |   basic_type_name_inter               { $$ = $1; }
                |   LE_SHORT basic_type_name            { $$ = $1 + " " + $2;}
                |   LE_LONG basic_type_name { $$ = $1 + " " + $2; }
                ;

/* ========================================================================*/
/* find declarations                                                       */
/* ========================================================================*/

translation_unit    :        /*empty*/
                        | translation_unit external_decl
                        ;

external_decl        :    {curr_var.Reset(); gs_names.clear(); s_tmpString.clear(); s_templateInitList.clear();} variables
                        | '$' basic_type_name ';' /* dummy '@' to avoid conflicts .. */
                        {
                            isBasicType = true;
                        }
                        | error {
                            yyclearin;    //clear lookahead token
                            yyerrok;
//                            printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_var_lval.c_str(), cl_scope_lineno);
                            }
                        ;
/* the following rules are for template parameters no declarations! */
parameter_list    : /* empty */        {$$ = "";}
                            | template_parameter    {$$ = $1;}
                            | parameter_list ',' template_parameter {$$ = $1 + $2 + " " + $3;}
                            ;

template_parameter    :    const_or_volatile_spec nested_scope_specifier LE_IDENTIFIER special_star_amp
                        {
                            $$ = $1 +  $2 + $3 +$4;
                        }
                    |      const_or_volatile_spec nested_scope_specifier basic_type_name special_star_amp
                        {
                            $$ = $1 +  $2 + $3 +$4;
                        }
                    |      const_or_volatile_spec nested_scope_specifier LE_IDENTIFIER '<' parameter_list '>' special_star_amp
                        {
                            $$ = $1 + $2 + $3 +$4 + $5 + $6 + $7 + " " ;
                        }
                        ;

//the main rule for finding variables
//in the code. if this rule succeeded, the variables
//is added to the gs_vars vriable
variables           : stmnt_starter LE_AUTO LE_IDENTIFIER '=' {var_consumeAutoAssignment($3);}
                    {}
                    
                    LE_TYPEDEF LE_STRUCT optional_struct_name '{' {var_consumBracketsContent('{');} typedef_name_list ';'
                    {
                    }
                    | stmnt_starter variable_decl special_star_amp const_or_volatile_spec variable_name_list '{'{var_consumBracketsContent('{');} ';'
                    {
                        if(gs_vars)
                            {
                                Variable var;
                                std::string pattern;
                                curr_var.m_isPtr = ($3.find("*") != (size_t)-1);
                                curr_var.m_starAmp = $3;
                                curr_var.m_lineno = cl_scope_lineno;
                                curr_var.m_rightSideConst = $4;
                                if(curr_var.m_templateDecl.empty())
                                    curr_var.m_templateDecl = s_templateInitList;
                                s_templateInitList.clear();

                                for(size_t i=0; i< gs_names.size(); i++)
                                {
                                    //create new variable for every variable name found
                                    var = curr_var;
                                    var.m_pattern      = $2 + " " + $3 + " " + $4 ;
                                    if(var.m_completeType.empty()) {
                                        var.m_completeType = $2 + " " + $3 + " " + $4 ;
                                    }
                                    var.m_name         = gs_names.at(i).m_name;
                                    var.m_defaultValue = gs_names.at(i).m_defaultValue;

                                    if(i != 0) {
                                        var.m_isPtr        = gs_names.at(i).m_isPtr;
                                        var.m_starAmp      = gs_names.at(i).m_starAmp;
                                    }
                                    gs_vars->push_back(var);
                                }
                                curr_var.Reset();
                                gs_names.clear();
                            }
                    }
                    | stmnt_starter variable_decl special_star_amp const_or_volatile_spec variable_name_list postfix
                        {
                            if(gs_vars)
                            {
                                Variable var;
                                std::string pattern;
                                curr_var.m_isPtr = ($3.find("*") != (size_t)-1);
                                curr_var.m_starAmp = $3;
                                curr_var.m_lineno = cl_scope_lineno;
                                curr_var.m_rightSideConst = $4;
                                if(curr_var.m_templateDecl.empty())
                                    curr_var.m_templateDecl = s_templateInitList;
                                s_templateInitList.clear();

                                for(size_t i=0; i< gs_names.size(); i++)
                                {
                                    //create new variable for every variable name found
                                    var = curr_var;
                                    var.m_pattern      = $2 + " " + $3 + " " + $4 ;
                                    if(var.m_completeType.empty()) {
                                        var.m_completeType = $2 + " " + $3 + " " + $4 ;
                                    }
                                    var.m_name         = gs_names.at(i).m_name;
                                    var.m_defaultValue = gs_names.at(i).m_defaultValue;

                                    if(i != 0) {
                                        var.m_isPtr        = gs_names.at(i).m_isPtr;
                                        var.m_starAmp      = gs_names.at(i).m_starAmp;
                                    }
                                    gs_vars->push_back(var);
                                }
                                curr_var.Reset();
                                gs_names.clear();
                            }
                        }
                        //
                        // Functions arguments:
                        //
                        | '(' variable_decl special_star_amp const_or_volatile_spec LE_IDENTIFIER postfix2
                        {
                            if(gs_vars)
                            {
                                Variable var;
                                std::string pattern;
                                curr_var.m_pattern       = $2 + " " + $3 + " " + $4 + " " + $5;
                                curr_var.m_completeType  = $2 + " " + $3 + " " + $4 ;
                                curr_var.m_isPtr         = ($3.find("*") != (size_t)-1);
                                curr_var.m_starAmp       = $3;
                                curr_var.m_arrayBrackets = $6;
                                curr_var.m_rightSideConst= $4;
                                curr_var.m_lineno        = cl_scope_lineno;
                                if(curr_var.m_templateDecl.empty())
                                    curr_var.m_templateDecl = s_templateInitList;
                                s_templateInitList.clear();    
                                
                                //create new variable for every variable name found
                                var = curr_var;
                                var.m_name               = $5;
                                gs_vars->push_back(var);
                                curr_var.Reset();
                                gs_names.clear();
                            }
                        }
                        | ',' variable_decl special_star_amp const_or_volatile_spec LE_IDENTIFIER postfix2
                        {
                            if(gs_vars && g_isUsedWithinFunc)
                            {
                                Variable var;
                                std::string pattern;
                                curr_var.m_pattern         = $2 + " " + $3 + " " + $4 + " " + $5;
                                curr_var.m_completeType    = $2 + " " + $3 + " " + $4 ;
                                curr_var.m_isPtr           = ($3.find("*") != (size_t)-1);
                                curr_var.m_starAmp         = $3;
                                curr_var.m_arrayBrackets   = $6;
                                curr_var.m_rightSideConst  = $4;
                                curr_var.m_lineno          = cl_scope_lineno;
                                
                                if(curr_var.m_templateDecl.empty())
                                    curr_var.m_templateDecl = s_templateInitList;
                                s_templateInitList.clear();    
                                
                                //create new variable for every variable name found
                                var = curr_var;
                                var.m_name = $5;
                                gs_vars->push_back(var);

                                curr_var.Reset();
                                gs_names.clear();
                            }
                        }
                        // Function arguments without identifier
                        | '(' variable_decl special_star_amp const_or_volatile_spec postfix3
                        {
                            if(gs_vars && g_isUsedWithinFunc)
                            {
                                Variable var;
                                std::string pattern;
                                curr_var.m_pattern       = $2 + " " + $3 + " " + $4;
                                curr_var.m_completeType  = $2 + " " + $3 + " " + $4;
                                curr_var.m_isPtr         = ($3.find("*") != (size_t)-1);
                                curr_var.m_starAmp       = $3;
                                curr_var.m_rightSideConst= $4;
                                curr_var.m_lineno        = cl_scope_lineno;
                                if(curr_var.m_templateDecl.empty())
                                    curr_var.m_templateDecl = s_templateInitList;
                                s_templateInitList.clear();    
                                
                                //create new variable for every variable name found
                                var = curr_var;
                                var.m_name = "";
                                gs_vars->push_back(var);

                                curr_var.Reset();
                                gs_names.clear();
                            }
                            if($5 == ",") {
                                cl_scope_less(0);
                            }
                        }
                        | ',' variable_decl special_star_amp const_or_volatile_spec postfix3
                        {
                            if(gs_vars && g_isUsedWithinFunc)
                            {
                                Variable var;
                                std::string pattern;
                                curr_var.m_pattern       = $2 + " " + $3 + " " + $4;
                                curr_var.m_completeType  = $2 + " " + $3 + " " + $4;
                                curr_var.m_isPtr         = ($3.find("*") != (size_t)-1);
                                curr_var.m_starAmp       = $3;
                                curr_var.m_lineno        = cl_scope_lineno;
                                curr_var.m_rightSideConst= $4;
                                if(curr_var.m_templateDecl.empty())
                                    curr_var.m_templateDecl = s_templateInitList;
                                s_templateInitList.clear();    
                                
                                //create new variable for every variable name found
                                var = curr_var;
                                var.m_name = "";
                                gs_vars->push_back(var);

                                curr_var.Reset();
                                gs_names.clear();
                            }
                            if($5 == ",") {
                                cl_scope_less(0);
                            }
                        }
                        | ellipsis_prefix LE_ELLIPSIS ')'
                        {
                            /* special type of argument: Ellipsis, can only be at the end of function argument */
                            if(gs_vars && g_isUsedWithinFunc)
                            {
                                Variable var;
                                std::string pattern;
                                curr_var.m_pattern      = $1 + " " + $2 + " " + $3;
                                curr_var.m_completeType = $2;
                                curr_var.m_isPtr        = false;
                                curr_var.m_starAmp      = "";
                                curr_var.m_lineno       = cl_scope_lineno;
                                curr_var.m_isEllipsis   = true;

                                var = curr_var;
                                gs_vars->push_back(var);

                                curr_var.Reset();
                                gs_names.clear();
                            }
                        }
                        ;

optional_struct_name     : /* empty */
                        | LE_IDENTIFIER
                        | '*'  LE_IDENTIFIER
                        | '**' LE_IDENTIFIER
                        ;
                        
fully_qualified_identifier_name: LE_IDENTIFIER {$$ = $1;}
                                | scope_specifier LE_IDENTIFIER {$$ = $2;}
                                ;
                                
typedef_name_list: optional_struct_name
                 | typedef_name_list ',' optional_struct_name
                 ;

ellipsis_prefix: '(' {$$ = $1;}
                |',' {$$ = $1;}
                ;

variable_name_list: fully_qualified_identifier_name {var_consumeDefaultValueIfNeeded();}
                    {
                        curr_var.m_name = $1;
                        gs_names.push_back(curr_var);
                    }
                    | variable_name_list ','  special_star_amp fully_qualified_identifier_name {var_consumeDefaultValueIfNeeded();}
                    {
                        //collect all the names
                        curr_var.m_name = $4;
                           curr_var.m_isPtr = ($3.find("*") != (size_t)-1);
                        curr_var.m_starAmp = $3;
                        gs_names.push_back(curr_var);
                        $$ = $1 + $2 + " " + $3 + $4;
                    }
                    ;

postfix3: ','
        | ')'
        ;

postfix2: /*empty*/ {$$ = "";}
        | '=' {var_consumeDefaultValue(',', ')'); $$ = ""; }
        | ')' { $$ = ""; }
        | '[' { $$ = $1 + var_consumBracketsContent('[');}
        ;

postfix: ';'
        | '='
        | ')'
        | '(' { $$ = $1 + var_consumBracketsContent('(');}
        | '[' { $$ = $1 + var_consumBracketsContent('[');}
        ;
/*
applicable for C++, for cases where a function is declared as
void scope::foo(){ ... }
*/
scope_specifier    : LE_IDENTIFIER LE_CLCL 
                {
                    $$ = $1+ $2; 
                }
                | LE_IDENTIFIER  '<' parameter_list '>' LE_CLCL
                {
                    $$ = $1 + $2 + $3 + $4 + $5; 
                    s_templateInitList = $2 + $3 + $4;
                }
                ;
                
nested_scope_specifier: /*empty*/ {$$ = "";}
                    | nested_scope_specifier scope_specifier {    $$ = $1 + $2;}
                    ;

volatile_qualifier: /* empty */ { $$ = "";           }
                  | LE_VOLATILE { $$ = $1; curr_var.m_isVolatile = true; }
                  ;
                  
const_or_volatile_spec: /* empty */    { $$ = "";           }
                    | LE_CONST volatile_qualifier { $$ = $1;           }
                    | LE_CONST '*'     { $$ = $1 + $2; curr_var.m_rightSideConst = $$;     }
                    | LE_CONST '&'     { $$ = $1 + $2; curr_var.m_rightSideConst = $$;     }
                    | LE_CONST '*' '*' { $$ = $1 + $2 + $3; curr_var.m_rightSideConst = $$;}
                    | LE_VOLATILE { $$ = $1; curr_var.m_isVolatile = true; }
                    | LE_ENUM const_or_volatile_spec { curr_var.m_enumInTypeDecl = true; }
                    ;

amp_item            : /*empty*/    {$$ = ""; }
                    | '&'            { $$ = $1; }
                    ;
                    
at_list             :  /* empty */ {$$ = "";}
                    | '@'          {$$ = "*";}
                    ;
                    
star_list           : /*empty*/        {$$ = ""; }
                    | star_list '*'    {$$ = $1 + $2;}
                    ;

special_star_amp:star_list amp_item at_list { $$ = $1 + $2 + $3; }
                        ;

stmnt_starter       : /*empty*/ {$$ = "";}
                    | ';' { $$ = ";";}
                    | '{' { $$ = "{";}
                    | '}' { $$ = "}";}
                    | ':' { $$ = ":";}    //e.g. private: std::string m_name;
                    ;

/** Variables **/
variable_decl       :   const_or_volatile_spec basic_type_name
                        {
                            $$ = $1 + " " + $2;
                            $2.erase($2.find_last_not_of(":")+1);
                            curr_var.m_type = $2;
                            curr_var.m_isBasicType = true;
                            curr_var.m_isConst = ($1.find("const") != std::string::npos);
                        }
                        |   const_or_volatile_spec nested_scope_specifier LE_IDENTIFIER
                        {
                            $$ = $1 + " " + $2 + $3;
                            $2.erase($2.find_last_not_of(":")+1);
                            curr_var.m_typeScope = $2;
                            curr_var.m_type = $3;
                            curr_var.m_isConst = ($1.find("const") != std::string::npos);
                            s_tmpString.clear();
                        }
                        |   const_or_volatile_spec nested_scope_specifier LE_IDENTIFIER '<' parameter_list '>'
                        {
                            $$ = $1 + " " + $2 + $3 + " " + $4 + $5 + $6;
                            $2.erase($2.find_last_not_of(":")+1);
                            curr_var.m_typeScope = $2;
                            curr_var.m_type = $3;
                            curr_var.m_isTemplate = true;
                            curr_var.m_templateDecl = $4 +$5 +$6;
                            curr_var.m_isConst = ($1.find("const") != std::string::npos);
                            s_tmpString.clear();
                        }
                        /*| const_or_volatile_spec LE_STRUCT nested_scope_specifier LE_IDENTIFIER  '{' {s_tmpString = var_consumBracketsContent('{');} ';'
                        {
                            $$ = $1 + " " + $2 + " " + $3 + " " + $4 + $5 + $6 + s_tmpString;
                            $3.erase($3.find_last_not_of(":")+1);
                            curr_var.m_typeScope = $3;
                            curr_var.m_type = $4;
                            curr_var.m_completeType = $3 + $4;
                            curr_var.m_isConst = ($1.find("const") != std::string::npos);
                            s_tmpString.clear();
                        }*/
                        | const_or_volatile_spec LE_STRUCT nested_scope_specifier LE_IDENTIFIER
                        {
                            $$ = $1 + " " + $2 + " " + $3 + " " + $4;
                            $3.erase($3.find_last_not_of(":")+1);
                            curr_var.m_typeScope = $3;
                            curr_var.m_type = $4;
                            curr_var.m_isTemplate = !curr_var.m_templateDecl.empty();
                            curr_var.m_completeType = $3 + $4;
                            curr_var.m_isConst = ($1.find("const") != std::string::npos);
                            s_tmpString.clear();
                        }
                        ;
%%
void yyerror(char *s) {}

void var_consumeAutoAssignment(const std::string& varname)
{
    // Collect everything until we encounter the first ';'
    std::string expression;
    while ( true ) {
        int ch = cl_scope_lex();
        if(ch == 0){
            break;
        }
        
        if ( ch == ';' ) {
            // add an auto variable
            Variable var;
            var.m_name = varname;
            var.m_isAuto = true;
            var.m_completeType.swap(expression);
            curr_var.m_lineno = cl_scope_lineno;
            
            s_templateInitList.clear();
            gs_vars->push_back(var);
            
            curr_var.Reset();
            gs_names.clear();
            
            break;
        } else {
            expression.append( cl_scope_text ).append(" ");
        }
    }
}

std::string var_consumBracketsContent(char openBrace)
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

void var_consumeDefaultValueIfNeeded()
{
    int ch = cl_scope_lex();
    if(ch != '=') {
        cl_scope_less(0);
        return;
    }
    var_consumeDefaultValue(';', ',');
}
 
void var_consumeDefaultValue(char c1, char c2)
{
    int depth = 0;

    curr_var.m_defaultValue.clear();
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

void clean_up()
{
    gs_vars = NULL;

    // restore settings
    setUseIgnoreMacros(true);
    g_isUsedWithinFunc = false;
    isBasicType = false;
    
    //do the lexer cleanup
    cl_scope_lex_clean();
}

// return the scope name at the end of the input string
void get_variables(const std::string &in, VariableList &li, const std::map<std::string, std::string> &ignoreMap, bool isUsedWithinFunc)
{
    //provide the lexer with new input
    if( !setLexerInput(in, ignoreMap) ){
        return;
    }

    //set the parser local output to our variable list
    gs_vars = &li;
    setUseIgnoreMacros(false);

    // the 'g_isUsedWithinFunc' allows us to parse variabels without name
    // this is typical when used as function declaration (e.g. void setValue(bool);)
    g_isUsedWithinFunc = isUsedWithinFunc;

    //call tghe main parsing routine
    cl_var_parse();
    clean_up();
}

bool is_primitive_type(const std::string &in)
{
    std::string input = "@"; // Hack the input string...
    input += in;
    input += ";";
    
    const std::map<std::string, std::string> ignoreMap;
    if(!setLexerInput(input, ignoreMap)) {
        return false;
    }

    isBasicType = false;
    cl_var_parse();
    bool res = isBasicType;
    clean_up();
    return res;
}
