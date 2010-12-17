#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define yyparse gdb_result_parse
#define yylex gdb_result_lex
#define yyerror gdb_result_error
#define yychar gdb_result_char
#define yyval gdb_result_val
#define yylval gdb_result_lval
#define yydebug gdb_result_debug
#define yynerrs gdb_result_nerrs
#define yyerrflag gdb_result_errflag
#define yyss gdb_result_ss
#define yyssp gdb_result_ssp
#define yyvs gdb_result_vs
#define yyvsp gdb_result_vsp
#define yylhs gdb_result_lhs
#define yylen gdb_result_len
#define yydefred gdb_result_defred
#define yydgoto gdb_result_dgoto
#define yysindex gdb_result_sindex
#define yyrindex gdb_result_rindex
#define yygindex gdb_result_gindex
#define yytable gdb_result_table
#define yycheck gdb_result_check
#define yyname gdb_result_name
#define yyrule gdb_result_rule
#define YYPREFIX "gdb_result_"
/* Copyright Eran Ifrah(c)*/

#include <string>
#include <vector>
#include <map>
#include <cstdio>
#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

#ifdef yylex
#undef yylex
#define yylex gdb_result_lex
#endif

int  gdb_result_lex();
void gdb_result_error(const char*);
bool setGdbLexerInput(const std::string &in, bool ascii, bool wantWhitespace);
void gdb_result_lex_clean();
int  gdb_result_parse();
void cleanup();

extern std::string gdb_result_lval;
static std::map<std::string, std::string>               sg_attributes;
static std::vector<std::map<std::string, std::string> > sg_children;
static std::vector<std::string>                         sg_locals;
#define GDB_DONE 257
#define GDB_RUNNING 258
#define GDB_CONNECTED 259
#define GDB_ERROR 260
#define GDB_EXIT 261
#define GDB_STACK_ARGS 262
#define GDB_VALUE 263
#define GDB_ARGS 264
#define GDB_FRAME 265
#define GDB_NAME 266
#define GDB_STRING 267
#define GDB_LEVEL 268
#define GDB_ESCAPED_STRING 269
#define GDB_LOCALS 270
#define GDB_INTEGER 271
#define GDB_OCTAL 272
#define GDB_HEX 273
#define GDB_FLOAT 274
#define GDB_IDENTIFIER 275
#define GDB_NUMCHILD 276
#define GDB_TYPE 277
#define GDB_DATA 278
#define GDB_ADDR 279
#define GDB_ASCII 280
#define GDB_CHILDREN 281
#define GDB_CHILD 282
#define GDB_MORE 283
#define GDB_VAROBJ 284
#define GDB_BREAKPOINT_TABLE 285
#define GDB_NR_ROWS 286
#define GDB_NR_COLS 287
#define GDB_HDR 288
#define GDB_BODY 289
#define GDB_BKPT 290
#define GDB_STOPPED 291
#define GDB_TIME 292
#define GDB_REASON 293
#define GDB_CHANGELIST 294
#define YYERRCODE 256
short gdb_result_lhs[] = {                                        -1,
    0,    0,    3,    1,    1,    2,    8,    2,    2,    2,
    2,    2,    2,    2,    2,    2,    2,    2,   13,   15,
   13,   11,   11,   17,   17,   16,   16,   18,   18,   12,
   12,   19,   20,   19,   10,   10,    9,    9,    4,    4,
    6,    6,    5,    5,    7,   22,    7,    7,   14,   14,
   21,   21,   21,   21,   21,   21,   21,
};
short gdb_result_len[] = {                                         2,
    1,    2,    0,    2,    1,   12,    0,    9,    8,    6,
    8,    8,   20,   20,    9,    8,    8,    1,    3,    0,
    6,   13,    5,    1,    3,    3,    5,    3,    5,    6,
    2,    5,    0,    8,    5,    7,    3,    5,    1,    1,
    1,    1,    5,    7,    3,    0,    6,    5,   11,    5,
    1,    1,    1,    1,    1,    1,    1,
};
short gdb_result_defred[] = {                                      0,
    5,    0,    1,    0,    2,    0,    0,    4,   18,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   50,    0,    0,    0,    0,
    0,    0,    0,    0,   54,   51,   57,   56,   52,   53,
   55,    0,    0,    0,   40,   39,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    7,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    9,   42,   41,   16,    0,
    0,    0,    0,   11,    0,   12,    0,    0,    0,    0,
    0,    0,   17,    0,    0,    0,    0,    8,    0,   37,
    0,    0,    0,    0,    0,    0,    0,   31,   15,    0,
   48,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   49,   47,    0,   35,   38,    0,
    0,    0,    0,    0,   23,    0,    0,    0,    0,    0,
    0,    0,    6,    0,   25,    0,    0,   21,    0,   36,
    0,    0,    0,   30,    0,    0,    0,    0,    0,    0,
    0,   43,    0,    0,    0,    0,    0,    0,   34,    0,
    0,   44,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   22,   13,   14,    0,    0,    0,    0,    0,   27,
   29,
};
short gdb_result_dgoto[] = {                                       2,
    3,    8,    4,   70,  132,   79,   43,   80,   64,   65,
   69,   91,   71,    9,  124,  177,  121,  181,  108,  160,
   44,   96,
};
short gdb_result_sindex[] = {                                   -238,
    0, -238,    0,  -94,    0,  -20, -220,    0,    0, -279,
   -5,  -17,  -13, -230,  -81, -213,    6,    7,    8,   10,
   16,   20,   21,   22, -190,    0,  -76, -205,  -76, -176,
  -76, -175,  -76,  -76,    0,    0,    0,    0,    0,    0,
    0,   37,  -22,   48,    0,    0, -155,   67, -190,   68,
 -118,   69, -269,  -76,   -9,   71, -151,   56, -190,  -72,
    0,   57, -190,  -37,  -32, -162,   59,   60,   79, -190,
  -72, -190, -169,    0,  -76,    0,    0,    0,    0, -190,
    4,    5,   17,    0, -159,    0,   70, -126,  -76, -232,
  -72,  -72,    0,   18,   81,  100, -122,    0, -190,    0,
 -190,   86,  -76,  104, -116,   88,   91,    0,    0,    0,
    0, -114, -190,   93,   30,   31,   34, -124, -128,  116,
  -72,  -76,  -76,  117,    0,    0, -105,    0,    0, -190,
  102,  -28,  103, -116,    0, -123, -190,  -76,  124,   44,
   47, -111,    0,  -95,    0,  -72,  -72,    0,  -90,    0,
 -190,  112,  131,    0,    0,  115,   52,   55, -109,  136,
  -76,    0, -190,  120, -123, -118,   58,  -76,    0,  -37,
  -32,    0,  -76,  -72,  -72,  -93,  -72,  -72,  -72,  123,
  -72,    0,    0,    0,  -82,  142,  143,  -76,  -93,    0,
    0,
};
short gdb_result_rindex[] = {                                    -91,
    0,  -91,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    2,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    1,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  -19,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  -66,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -18,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  -65,  -62,    0,    0,    0,
    0,
};
short gdb_result_gindex[] = {                                      0,
  186,    0,    0,  -23,    0,  -42,  -29,    0,   23,   24,
    0,    0,   53,    0,    0,   11,   61,    3, -127,    0,
    0,    0,
};
#define YYTABLESIZE 293
short gdb_result_table[] = {                                       7,
   45,   10,    3,   47,   63,   49,   83,   51,  146,   53,
   54,   85,   12,   13,   46,  142,   67,    1,   68,   60,
   78,   84,   86,   10,   20,   33,   24,   26,   93,   76,
   28,   17,   18,   82,   19,   20,   11,  169,   14,   21,
   92,   25,   94,   15,   46,   22,   45,   16,  109,  110,
   98,   97,   77,   26,   23,   78,  106,  107,   24,   26,
   78,   48,   28,   24,   78,  105,   27,   28,   29,  115,
   30,  116,   35,   19,   32,   36,   31,   37,  135,  118,
   32,   33,   34,  126,   38,   39,   40,   77,   41,  143,
   50,   52,   77,   45,   45,   10,   77,   55,  136,  137,
  140,   42,   56,  154,  155,   19,   32,  147,   57,   58,
   59,   61,   66,   72,   73,   74,   75,   81,   87,   88,
   89,  157,   90,   95,  102,   45,   99,  174,  175,  100,
  103,  178,  179,  167,  182,  183,  184,  166,  186,  101,
  104,  112,  111,  113,  173,  114,  117,  119,  122,  176,
  120,  123,  125,  127,  128,  129,  130,  131,  133,  134,
  138,  139,  141,  144,  176,   62,  107,  149,  150,  151,
  152,  153,  158,  156,  159,  161,  162,  163,  164,  165,
  168,  180,  172,  185,  187,  188,  189,    5,  170,  171,
  148,  191,    0,    0,  145,    0,    6,    0,  190,    3,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   45,   10,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   45,   10,
};
short gdb_result_check[] = {                                      94,
    0,    0,   94,   27,  123,   29,   44,   31,  136,   33,
   34,   44,  292,  293,   91,   44,  286,  256,  288,   49,
   93,   64,   65,   44,   44,   44,   93,   93,   71,   59,
   93,  262,  263,   63,  265,  266,  257,  165,   44,  270,
   70,  123,   72,   61,   44,  276,  123,   61,   91,   92,
   80,   75,  125,  267,  285,   93,  289,  290,  125,  125,
   93,  267,  125,  294,   93,   89,   61,   61,   61,   99,
   61,  101,  263,   93,   93,  266,   61,  268,  121,  103,
   61,   61,   61,  113,  275,  276,  277,  125,  279,  132,
  267,  267,  125,   93,   94,   94,  125,   61,  122,  123,
  130,  292,  125,  146,  147,  125,  125,  137,   61,  265,
   44,   44,   44,  123,   44,  267,   61,   61,  281,   61,
   61,  151,   44,  293,  284,  125,  123,  170,  171,  125,
   61,  174,  175,  163,  177,  178,  179,  161,  181,  123,
  267,   61,  125,   44,  168,  268,   61,   44,   61,  173,
  267,   61,  267,   61,  125,  125,  123,  282,  287,   44,
   44,  267,   61,   61,  188,  284,  290,   44,  125,  123,
  282,  267,   61,  264,   44,   61,  125,  123,  288,   44,
   61,  275,  125,   61,  267,   44,   44,    2,  166,  166,
  138,  189,   -1,   -1,  134,   -1,  291,   -1,  188,  291,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  256,  256,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  291,  291,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 294
#if YYDEBUG
char *gdb_result_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'='",0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,"']'","'^'",0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"GDB_DONE","GDB_RUNNING","GDB_CONNECTED","GDB_ERROR","GDB_EXIT",
"GDB_STACK_ARGS","GDB_VALUE","GDB_ARGS","GDB_FRAME","GDB_NAME","GDB_STRING",
"GDB_LEVEL","GDB_ESCAPED_STRING","GDB_LOCALS","GDB_INTEGER","GDB_OCTAL",
"GDB_HEX","GDB_FLOAT","GDB_IDENTIFIER","GDB_NUMCHILD","GDB_TYPE","GDB_DATA",
"GDB_ADDR","GDB_ASCII","GDB_CHILDREN","GDB_CHILD","GDB_MORE","GDB_VAROBJ",
"GDB_BREAKPOINT_TABLE","GDB_NR_ROWS","GDB_NR_COLS","GDB_HDR","GDB_BODY",
"GDB_BKPT","GDB_STOPPED","GDB_TIME","GDB_REASON","GDB_CHANGELIST",
};
char *gdb_result_rule[] = {
"$accept : parse",
"parse : children_list",
"parse : parse children_list",
"$$1 :",
"children_list : $$1 child_pattern",
"children_list : error",
"child_pattern : '^' GDB_DONE ',' GDB_NUMCHILD '=' GDB_STRING ',' GDB_CHILDREN '=' list_open children list_close",
"$$2 :",
"child_pattern : '^' GDB_DONE ',' GDB_NAME '=' GDB_STRING ',' $$2 child_attributes",
"child_pattern : '^' GDB_DONE ',' GDB_VALUE '=' GDB_STRING ',' child_attributes",
"child_pattern : '^' GDB_DONE ',' GDB_VALUE '=' GDB_STRING",
"child_pattern : '^' GDB_DONE ',' GDB_LOCALS '=' list_open locals list_close",
"child_pattern : '^' GDB_DONE ',' GDB_LOCALS '=' list_open mac_locals list_close",
"child_pattern : '^' GDB_DONE ',' GDB_STACK_ARGS '=' list_open GDB_FRAME '=' list_open GDB_LEVEL '=' GDB_STRING ',' GDB_ARGS '=' list_open locals list_close list_close list_close",
"child_pattern : '^' GDB_DONE ',' GDB_STACK_ARGS '=' list_open GDB_FRAME '=' list_open GDB_LEVEL '=' GDB_STRING ',' GDB_ARGS '=' list_open mac_locals list_close list_close list_close",
"child_pattern : '^' GDB_DONE ',' GDB_BREAKPOINT_TABLE '=' list_open bpt_table_hdr bpt_table_body list_close",
"child_pattern : '^' GDB_DONE ',' GDB_FRAME '=' list_open child_attributes list_close",
"child_pattern : '^' GDB_DONE ',' GDB_CHANGELIST '=' list_open change_set list_close",
"child_pattern : stop_statement",
"change_set : list_open child_attributes list_close",
"$$3 :",
"change_set : list_open child_attributes list_close $$3 ',' change_set",
"bpt_table_hdr : GDB_NR_ROWS '=' GDB_STRING ',' GDB_NR_COLS '=' GDB_STRING ',' GDB_HDR '=' list_open bpt_hdr_table_description list_close",
"bpt_table_hdr : GDB_HDR '=' list_open comma_separated_strings list_close",
"comma_separated_strings : GDB_STRING",
"comma_separated_strings : GDB_STRING ',' comma_separated_strings",
"bpt_hdr_table_description : list_open bpt_table_description_attr list_close",
"bpt_hdr_table_description : list_open bpt_table_description_attr list_close ',' bpt_hdr_table_description",
"bpt_table_description_attr : GDB_IDENTIFIER '=' GDB_STRING",
"bpt_table_description_attr : GDB_IDENTIFIER '=' GDB_STRING ',' bpt_table_description_attr",
"bpt_table_body : ',' GDB_BODY '=' list_open breakpoints list_close",
"bpt_table_body : ',' breakpoints",
"breakpoints : GDB_BKPT '=' list_open child_attributes list_close",
"$$4 :",
"breakpoints : GDB_BKPT '=' list_open child_attributes list_close $$4 ',' breakpoints",
"mac_locals : GDB_VAROBJ '=' '{' child_attributes '}'",
"mac_locals : mac_locals ',' GDB_VAROBJ '=' '{' child_attributes '}'",
"locals : '{' child_attributes '}'",
"locals : locals ',' '{' child_attributes '}'",
"list_open : '['",
"list_open : '{'",
"list_close : ']'",
"list_close : '}'",
"children : GDB_CHILD '=' '{' child_attributes '}'",
"children : children ',' GDB_CHILD '=' '{' child_attributes '}'",
"child_attributes : child_key '=' GDB_STRING",
"$$5 :",
"child_attributes : child_key '=' GDB_STRING $$5 ',' child_attributes",
"child_attributes : GDB_TIME '=' '{' child_attributes '}'",
"stop_statement : GDB_STOPPED ',' GDB_TIME '=' '{' child_attributes '}' ',' GDB_REASON '=' GDB_STRING",
"stop_statement : GDB_STOPPED ',' GDB_REASON '=' GDB_STRING",
"child_key : GDB_NAME",
"child_key : GDB_NUMCHILD",
"child_key : GDB_TYPE",
"child_key : GDB_VALUE",
"child_key : GDB_ADDR",
"child_key : GDB_IDENTIFIER",
"child_key : GDB_LEVEL",
};
#endif
#ifndef YYSTYPE
typedef int YYSTYPE;
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
void cleanup()
{
	sg_attributes.clear();
	sg_children.clear();
	sg_locals.clear();
}

void gdbParseListChildren( const std::string &in, std::vector<std::map<std::string, std::string> > &children)
{
	cleanup();

	setGdbLexerInput(in, true, false);
	gdb_result_parse();

	children = sg_children;
	gdb_result_lex_clean();
}
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 3:
{ cleanup(); }
break;
case 5:
{
				/*printf("CodeLite: syntax error, unexpected token '%s' found\n", gdb_result_lval.c_str());*/
				}
break;
case 7:
{sg_attributes[yyvsp[-3]] = yyvsp[-1];}
break;
case 8:
{
					sg_children.push_back( sg_attributes );
					sg_attributes.clear();
				}
break;
case 9:
{
					sg_attributes[yyvsp[-4]] = yyvsp[-2];
					sg_children.push_back( sg_attributes );
					sg_attributes.clear();
				}
break;
case 10:
{
					sg_attributes[yyvsp[-2]] = yyvsp[0];
					sg_children.push_back( sg_attributes );
					sg_attributes.clear();
				}
break;
case 16:
{
					sg_children.push_back( sg_attributes );
					sg_attributes.clear();
				}
break;
case 19:
{
					sg_children.push_back( sg_attributes );
					sg_attributes.clear();
				}
break;
case 20:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 32:
{ sg_children.push_back( sg_attributes ); sg_attributes.clear();}
break;
case 33:
{ sg_children.push_back( sg_attributes ); sg_attributes.clear();}
break;
case 35:
{
				sg_children.push_back( sg_attributes );
				sg_attributes.clear();
			}
break;
case 36:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 37:
{
				sg_children.push_back( sg_attributes );
				sg_attributes.clear();
			}
break;
case 38:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 43:
{
					sg_children.push_back( sg_attributes );
					sg_attributes.clear();
				}
break;
case 44:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 45:
{
						if(!yyvsp[0].empty()) {
							sg_attributes[yyvsp[-2]] = yyvsp[0];
						}
					}
break;
case 46:
{ sg_attributes[yyvsp[-2]] = yyvsp[0];}
break;
case 49:
{
					sg_attributes["reason"] = yyvsp[0];
					sg_children.push_back( sg_attributes );
				}
break;
case 50:
{
					sg_attributes["reason"] = yyvsp[0];
					sg_children.push_back( sg_attributes );
				}
break;
case 51:
{yyval = yyvsp[0];}
break;
case 52:
{yyval = yyvsp[0];}
break;
case 53:
{yyval = yyvsp[0];}
break;
case 54:
{yyval = yyvsp[0];}
break;
case 55:
{yyval = yyvsp[0];}
break;
case 56:
{yyval = yyvsp[0];}
break;
case 57:
{yyval = yyvsp[0];}
break;
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
