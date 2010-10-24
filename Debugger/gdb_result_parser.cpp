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
#define YYERRCODE 256
short gdb_result_lhs[] = {                                        -1,
    0,    0,    3,    1,    1,    2,    8,    2,    2,    2,
    2,    2,    2,    2,    2,   11,   14,   14,   15,   15,
   12,   16,   17,   16,   10,   18,   10,    9,   19,    9,
    4,    4,    6,    6,    5,   20,    5,    7,   22,    7,
   13,   13,   21,   21,   21,   21,   21,   21,   21,
};
short gdb_result_len[] = {                                         2,
    1,    2,    0,    2,    1,   12,    0,    9,    8,    8,
   20,   20,    9,    8,    1,   13,    3,    5,    3,    5,
    6,    5,    0,    8,    5,    0,    8,    3,    0,    6,
    1,    1,    1,    1,    5,    0,    8,    3,    0,    6,
   11,    5,    1,    1,    1,    1,    1,    1,    1,
};
short gdb_result_defred[] = {                                      0,
    5,    0,    1,    0,    2,    0,    0,    4,   15,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   42,    0,    0,    0,    0,    0,    0,
   46,   43,   49,   48,   44,   45,   47,    0,    0,   32,
   31,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    7,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   34,   33,   14,    0,    0,    0,    9,   10,
    0,    0,    0,    0,    0,    0,    0,    8,    0,    0,
    0,    0,    0,   13,    0,    0,    0,    0,    0,    0,
    0,    0,   41,   40,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   30,    0,    6,    0,    0,    0,    0,
    0,    0,    0,    0,   21,    0,   27,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   37,    0,    0,    0,    0,
    0,    0,    0,   16,   24,   11,   12,    0,    0,    0,
    0,    0,   18,   20,
};
short gdb_result_dgoto[] = {                                       2,
    3,    8,    4,  137,   99,   65,   38,   66,   55,   56,
   59,   74,    9,  138,  143,  109,  133,  103,   89,  126,
   39,   76,
};
short gdb_result_sindex[] = {                                   -239,
    0, -239,    0,  -94,    0,  -31, -237,    0,    0, -268,
  -18,  -30,  -26, -243,  -89, -231,  -24,  -23,  -22,  -11,
   -9,   -7, -210,    0,  -80,  -80, -206,  -80, -205,  -80,
    0,    0,    0,    0,    0,    0,    0,  -95,    7,    0,
    0, -195, -210,   29, -120,   30, -211,   33, -189,   18,
  -81,    0,   19, -210,  -81,  -81, -200,   23,   41, -207,
    0,  -80,    0,    0,    0, -210,  -35,  -33,    0,    0,
   37, -177, -190,  -81,   40,   60, -163,    0, -210,    0,
  -80,   66,   51,    0, -152, -210,   57,   -6,   76, -161,
 -165,  -80,    0,    0, -144,    0,    2,   63,  -81,   67,
 -160,   83,   85,    0,    8,    0, -135,   72,  -81, -130,
 -149, -210,   92,  -80,    0,   77,    0,   12, -148, -210,
  -80,    0,   78,  -81, -120,   97,  -80,    0,  -81,  -81,
 -161,  -80,   98,  -81,  -81,    0, -132,  -81, -160,  -81,
  -81,   84,  -81,    0,    0,    0,    0, -123,  102,  103,
  -80, -132,    0,    0,
};
short gdb_result_rindex[] = {                                    -92,
    0,  -92,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  -38,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  -36,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  -34,    0,    0,    0,    0,    0,  -29,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  -79,  -77,
    0,    0,    0,    0,
};
short gdb_result_gindex[] = {                                      0,
  146,    0,    0,  -21,   20,  -27,   -3,    0,  -76,  -93,
    0,    0,    0,   -2,    3,   11,    0,    0,    0,    0,
    0,    0,
};
#define YYTABLESIZE 292
short gdb_result_table[] = {                                       7,
   38,    3,   54,   42,   43,   29,   45,   26,   47,   36,
   41,   64,   10,   17,   23,   19,    1,  117,   17,   11,
  104,   18,   19,   12,   13,   14,   20,   69,   70,   48,
   15,  130,   21,   23,   16,   24,   25,   26,   27,   51,
   77,   22,   40,   63,   39,   17,   84,   19,  129,   28,
   68,   29,   31,   30,   28,   32,   25,   33,   35,   90,
   44,   46,   78,   22,   34,   35,   36,   49,   37,   50,
  101,  106,   52,   57,   58,   88,   60,   61,   62,   67,
   71,  115,   94,   72,   73,   75,   28,   79,   25,   82,
   35,   80,  120,   38,   38,   22,  128,   81,   83,  125,
   85,  134,  135,   86,   87,  132,  140,  141,  118,   91,
  144,   92,  146,  147,   93,  149,  124,   95,   96,   97,
   98,  100,  102,  105,   54,   38,  110,  107,  111,  108,
  112,  113,  114,  116,   53,  119,  122,  121,  127,  123,
  131,  139,  142,  150,  148,  151,  152,    5,  153,  145,
  136,    0,    0,    0,  154,    0,    0,    0,    0,    0,
    0,    0,    0,   53,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    6,    0,    3,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   38,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   38,
};
short gdb_result_check[] = {                                      94,
    0,   94,  123,   25,   26,   44,   28,   44,   30,   44,
   91,   93,   44,   93,   44,   93,  256,  111,  262,  257,
   97,  265,  266,  292,  293,   44,  270,   55,   56,  125,
   61,  125,  276,  123,   61,  267,   61,   61,   61,   43,
   62,  285,  123,  125,   44,  125,   74,  125,  125,   61,
   54,   61,  263,   61,   93,  266,   93,  268,   93,   81,
  267,  267,   66,   93,  275,  276,  277,   61,  279,  265,
   92,   99,   44,   44,  286,   79,   44,  267,   61,   61,
  281,  109,   86,   61,   44,  293,  125,  123,  125,  267,
  125,  125,  114,   93,   94,  125,  124,   61,  289,  121,
   61,  129,  130,   44,  268,  127,  134,  135,  112,   44,
  138,   61,  140,  141,  267,  143,  120,   61,  125,   44,
  282,  287,  267,   61,  123,  125,   44,   61,   44,  290,
  123,  267,   61,  264,  284,   44,  125,   61,   61,  288,
   44,   44,  275,  267,   61,   44,   44,    2,  151,  139,
  131,   -1,   -1,   -1,  152,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  284,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
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
#define YYMAXTOKEN 293
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
"GDB_BKPT","GDB_STOPPED","GDB_TIME","GDB_REASON",
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
"child_pattern : stop_statement",
"bpt_table_hdr : GDB_NR_ROWS '=' GDB_STRING ',' GDB_NR_COLS '=' GDB_STRING ',' GDB_HDR '=' list_open bpt_hdr_table_description list_close",
"bpt_hdr_table_description : list_open bpt_table_description_attr list_close",
"bpt_hdr_table_description : list_open bpt_table_description_attr list_close ',' bpt_hdr_table_description",
"bpt_table_description_attr : GDB_IDENTIFIER '=' GDB_STRING",
"bpt_table_description_attr : GDB_IDENTIFIER '=' GDB_STRING ',' bpt_table_description_attr",
"bpt_table_body : ',' GDB_BODY '=' list_open breakpoints list_close",
"breakpoints : GDB_BKPT '=' list_open child_attributes list_close",
"$$3 :",
"breakpoints : GDB_BKPT '=' list_open child_attributes list_close $$3 ',' breakpoints",
"mac_locals : GDB_VAROBJ '=' '{' child_attributes '}'",
"$$4 :",
"mac_locals : GDB_VAROBJ '=' '{' child_attributes '}' $$4 ',' mac_locals",
"locals : '{' child_attributes '}'",
"$$5 :",
"locals : '{' child_attributes '}' $$5 ',' locals",
"list_open : '['",
"list_open : '{'",
"list_close : ']'",
"list_close : '}'",
"children : GDB_CHILD '=' '{' child_attributes '}'",
"$$6 :",
"children : GDB_CHILD '=' '{' child_attributes '}' $$6 ',' children",
"child_attributes : child_key '=' GDB_STRING",
"$$7 :",
"child_attributes : child_key '=' GDB_STRING $$7 ',' child_attributes",
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
case 22:
{ sg_children.push_back( sg_attributes ); sg_attributes.clear();}
break;
case 23:
{ sg_children.push_back( sg_attributes ); sg_attributes.clear();}
break;
case 25:
{
				sg_children.push_back( sg_attributes );
				sg_attributes.clear();
			}
break;
case 26:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 28:
{
				sg_children.push_back( sg_attributes );
				sg_attributes.clear();
			}
break;
case 29:
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
case 38:
{ sg_attributes[yyvsp[-2]] = yyvsp[0]; }
break;
case 39:
{ sg_attributes[yyvsp[-2]] = yyvsp[0]; }
break;
case 41:
{
					sg_attributes["reason"] = yyvsp[0];
					sg_children.push_back( sg_attributes );
				}
break;
case 42:
{
					sg_attributes["reason"] = yyvsp[0];
					sg_children.push_back( sg_attributes );
				}
break;
case 43:
{yyval = yyvsp[0];}
break;
case 44:
{yyval = yyvsp[0];}
break;
case 45:
{yyval = yyvsp[0];}
break;
case 46:
{yyval = yyvsp[0];}
break;
case 47:
{yyval = yyvsp[0];}
break;
case 48:
{yyval = yyvsp[0];}
break;
case 49:
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
