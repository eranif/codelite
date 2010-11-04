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
    2,    2,    2,    2,    2,    2,   13,   15,   13,   11,
   11,   17,   17,   16,   16,   18,   18,   12,   12,   19,
   20,   19,   10,   10,    9,    9,    4,    4,    6,    6,
    5,    5,    7,   22,    7,   14,   14,   21,   21,   21,
   21,   21,   21,   21,
};
short gdb_result_len[] = {                                         2,
    1,    2,    0,    2,    1,   12,    0,    9,    8,    8,
   20,   20,    9,    8,    8,    1,    3,    0,    6,   13,
    5,    1,    3,    3,    5,    3,    5,    6,    2,    5,
    0,    8,    5,    7,    3,    5,    1,    1,    1,    1,
    5,    7,    3,    0,    6,   11,    5,    1,    1,    1,
    1,    1,    1,    1,
};
short gdb_result_defred[] = {                                      0,
    5,    0,    1,    0,    2,    0,    0,    4,   16,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   47,    0,    0,    0,    0,    0,
    0,    0,   51,   48,   54,   53,   49,   50,   52,    0,
    0,   38,   37,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    7,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   40,   39,
   14,    0,    0,    0,    0,    9,    0,   10,    0,    0,
    0,    0,    0,    0,   15,    0,    0,    0,    8,    0,
   35,    0,    0,    0,    0,    0,    0,    0,   29,   13,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   46,   45,    0,   33,   36,    0,
    0,    0,    0,    0,   21,    0,    0,    0,    0,    0,
    0,    0,    6,    0,   23,    0,    0,   19,    0,   34,
    0,    0,    0,   28,    0,    0,    0,    0,    0,    0,
    0,   41,    0,    0,    0,    0,    0,    0,   32,    0,
    0,   42,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   20,   11,   12,    0,    0,    0,    0,    0,   25,
   27,
};
short gdb_result_dgoto[] = {                                       2,
    3,    8,    4,   64,  122,   71,   40,   72,   58,   59,
   63,   83,   65,    9,  114,  167,  111,  171,   99,  150,
   41,   87,
};
short gdb_result_sindex[] = {                                   -226,
    0, -226,    0,  -94,    0,   -3, -209,    0,    0, -258,
    5,  -11,   -9, -238,  -63, -206,    2,   10,   13,   15,
   16,   18,   19, -193,    0,  -80,  -80, -205,  -80, -182,
  -80,  -80,    0,    0,    0,    0,    0,    0,    0,  -38,
   35,    0,    0, -166, -193,   59, -120,   60, -257,  -80,
   61, -161,   46,  -81,    0,   47, -193,  -36,  -28, -172,
   49,   50,   68, -193,  -81, -180,    0,  -80,    0,    0,
    0, -193,   -8,   -7,   -4,    0, -162,    0,   67, -137,
  -80, -253,  -81,  -81,    0,   70,   88, -135,    0, -193,
    0, -193,   73,  -80,   91, -130,   77,   78,    0,    0,
    0, -127, -193,   81,   20,   21,   24, -139, -143,  104,
  -81,  -80,  -80,  105,    0,    0, -117,    0,    0, -193,
   90,  -27,   92, -130,    0, -138, -193,  -80,  110,   30,
   34, -124,    0, -108,    0,  -81,  -81,    0, -104,    0,
 -193,  100,  118,    0,    0,  102,   40,   43, -121,  124,
  -80,    0, -193,  108, -138, -120,   45,  -80,    0,  -36,
  -28,    0,  -80,  -81,  -81, -103,  -81,  -81,  -81,  112,
  -81,    0,    0,    0,  -96,  130,  131,  -80, -103,    0,
    0,
};
short gdb_result_rindex[] = {                                    -92,
    0,  -92,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    1,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  -25,    0,    0,    0,    0,    0,    0,    0,    0,  -71,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -24,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  -70,  -67,    0,    0,    0,
    0,
};
short gdb_result_gindex[] = {                                      0,
  174,    0,    0,  -22,    0,  -44,  -39,    0,   22,   23,
    0,    0,   52,    0,    0,   -1,   57,    3, -113,    0,
    0,    0,
};
#define YYTABLESIZE 292
short gdb_result_table[] = {                                       7,
   43,    3,   57,   44,   45,   54,   47,   75,   49,   50,
   43,   70,  136,   76,   78,   77,  132,   74,   18,   31,
   85,   22,   24,   17,   84,   26,   18,   19,   61,    1,
   62,   20,   89,   12,   13,   97,   98,   21,  100,  101,
   10,  159,   42,   69,   44,   88,   22,   11,   14,   15,
  105,   16,  106,   22,   24,   23,   70,   26,   96,   24,
   25,   46,   26,  116,   70,   70,  125,   17,   30,   33,
   27,  108,   34,   28,   35,   29,   30,  133,   31,   32,
  130,   36,   37,   38,   48,   39,   51,  137,   69,  126,
  127,  144,  145,   43,   43,   52,   69,   69,   53,   17,
   30,  147,   55,   60,   66,   67,   68,   73,   79,   80,
   81,   82,   86,  157,   90,  164,  165,   91,   92,  168,
  169,   93,  172,  173,  174,   43,  176,   94,  156,   95,
  102,  103,  104,  107,  109,  163,  110,  112,  113,  115,
  166,  117,  121,  123,  118,  119,  120,  124,  128,  129,
  131,   98,  134,  139,  140,  166,  141,  142,  143,  146,
  148,  149,  151,   56,  152,  153,  154,  155,  158,  162,
  177,  170,  175,  178,  179,    5,  180,  160,  161,  138,
  135,  181,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    6,    0,    3,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   43,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   43,
};
short gdb_result_check[] = {                                      94,
    0,   94,  123,   26,   27,   45,   29,   44,   31,   32,
   91,   93,  126,   58,   59,   44,   44,   57,   44,   44,
   65,   93,   93,  262,   64,   93,  265,  266,  286,  256,
  288,  270,   72,  292,  293,  289,  290,  276,   83,   84,
   44,  155,  123,  125,   44,   68,  285,  257,   44,   61,
   90,   61,   92,  125,  125,  294,   93,  125,   81,  123,
  267,  267,   61,  103,   93,   93,  111,   93,   93,  263,
   61,   94,  266,   61,  268,   61,   61,  122,   61,   61,
  120,  275,  276,  277,  267,  279,  125,  127,  125,  112,
  113,  136,  137,   93,   94,   61,  125,  125,  265,  125,
  125,  141,   44,   44,   44,  267,   61,   61,  281,   61,
   61,   44,  293,  153,  123,  160,  161,  125,  123,  164,
  165,  284,  167,  168,  169,  125,  171,   61,  151,  267,
   61,   44,  268,   61,   44,  158,  267,   61,   61,  267,
  163,   61,  282,  287,  125,  125,  123,   44,   44,  267,
   61,  290,   61,   44,  125,  178,  123,  282,  267,  264,
   61,   44,   61,  284,  125,  123,  288,   44,   61,  125,
  267,  275,   61,   44,   44,    2,  178,  156,  156,  128,
  124,  179,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  291,   -1,  291,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  256,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  291,
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
case 14:
{
					sg_children.push_back( sg_attributes );
					sg_attributes.clear();
				}
break;
case 17:
{
					sg_children.push_back( sg_attributes );
					sg_attributes.clear();
				}
break;
case 18:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 30:
{ sg_children.push_back( sg_attributes ); sg_attributes.clear();}
break;
case 31:
{ sg_children.push_back( sg_attributes ); sg_attributes.clear();}
break;
case 33:
{
				sg_children.push_back( sg_attributes );
				sg_attributes.clear();
			}
break;
case 34:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
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
case 41:
{
					sg_children.push_back( sg_attributes );
					sg_attributes.clear();
				}
break;
case 42:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 43:
{
						if(!yyvsp[0].empty()) {
							sg_attributes[yyvsp[-2]] = yyvsp[0];
						}
					}
break;
case 44:
{ sg_attributes[yyvsp[-2]] = yyvsp[0];}
break;
case 46:
{
					sg_attributes["reason"] = yyvsp[0];
					sg_children.push_back( sg_attributes );
				}
break;
case 47:
{
					sg_attributes["reason"] = yyvsp[0];
					sg_children.push_back( sg_attributes );
				}
break;
case 48:
{yyval = yyvsp[0];}
break;
case 49:
{yyval = yyvsp[0];}
break;
case 50:
{yyval = yyvsp[0];}
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
