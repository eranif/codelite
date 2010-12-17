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
    5,    5,    7,   22,    7,    7,   14,   14,   21,   21,
   21,   21,   21,   21,   21,
};
short gdb_result_len[] = {                                         2,
    1,    2,    0,    2,    1,   12,    0,    9,    8,    8,
   20,   20,    9,    8,    8,    1,    3,    0,    6,   13,
    5,    1,    3,    3,    5,    3,    5,    6,    2,    5,
    0,    8,    5,    7,    3,    5,    1,    1,    1,    1,
    5,    7,    3,    0,    6,    5,   11,    5,    1,    1,
    1,    1,    1,    1,    1,
};
short gdb_result_defred[] = {                                      0,
    5,    0,    1,    0,    2,    0,    0,    4,   16,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   48,    0,    0,    0,    0,    0,
    0,    0,   52,   49,   55,   54,   50,   51,   53,    0,
    0,    0,   38,   37,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    7,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   40,   39,   14,    0,    0,    0,    0,    9,    0,
   10,    0,    0,    0,    0,    0,    0,   15,    0,    0,
    0,    0,    8,    0,   35,    0,    0,    0,    0,    0,
    0,    0,   29,   13,    0,   46,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   47,
   45,    0,   33,   36,    0,    0,    0,    0,    0,   21,
    0,    0,    0,    0,    0,    0,    0,    6,    0,   23,
    0,    0,   19,    0,   34,    0,    0,    0,   28,    0,
    0,    0,    0,    0,    0,    0,   41,    0,    0,    0,
    0,    0,    0,   32,    0,    0,   42,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   20,   11,   12,    0,
    0,    0,    0,    0,   25,   27,
};
short gdb_result_dgoto[] = {                                       2,
    3,    8,    4,   66,  127,   74,   41,   75,   60,   61,
   65,   86,   67,    9,  119,  172,  116,  176,  103,  155,
   42,   91,
};
short gdb_result_sindex[] = {                                   -240,
    0, -240,    0,  -94,    0,  -11, -234,    0,    0, -271,
  -10,  -30,  -24, -224,  -88, -228,  -12,   -2,    4,   10,
   11,   15,   18, -188,    0,  -79,  -79, -227,  -79, -216,
  -79,  -79,    0,    0,    0,    0,    0,    0,    0,   20,
  -43,   22,    0,    0, -181, -188,   48, -120,   57, -273,
  -79,  -18,   58, -161,   47,  -69,    0,   49, -188,  -40,
  -35, -172,   50,   51,   69, -188,  -69, -188, -178,    0,
  -79,    0,    0,    0, -188,   -7,   -8,   -5,    0, -164,
    0,   60, -145,  -79, -264,  -69,  -69,    0,   -1,   62,
   81, -141,    0, -188,    0, -188,   70,  -79,   86, -135,
   72,   73,    0,    0,    0,    0, -131, -188,   76,   13,
   14,   17, -139, -146,  100,  -69,  -79,  -79,  101,    0,
    0, -121,    0,    0, -188,   87,  -27,   88, -135,    0,
 -140, -188,  -79,  107,   29,   32, -124,    0, -102,    0,
  -69,  -69,    0,  -98,    0, -188,  106,  124,    0,    0,
  108,   45,   52, -116,  127,  -79,    0, -188,  112, -140,
 -120,   53,  -79,    0,  -40,  -35,    0,  -79,  -69,  -69,
 -101,  -69,  -69,  -69,  115,  -69,    0,    0,    0,  -90,
  135,  136,  -79, -101,    0,    0,
};
short gdb_result_rindex[] = {                                    -92,
    0,  -92,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    1,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -26,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -65,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  -25,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  -63,  -61,    0,    0,    0,    0,
};
short gdb_result_gindex[] = {                                      0,
  179,    0,    0,  -21,    0,  -13,  -39,    0,   21,   23,
    0,    0,   54,    0,    0,    2,   59,    5, -117,    0,
    0,    0,
};
#define YYTABLESIZE 292
short gdb_result_table[] = {                                       7,
   43,    3,   59,   78,   45,   46,   56,   48,   80,   50,
   51,   44,   63,  141,   64,    1,  137,   18,   31,   77,
   12,   13,   11,   73,  101,  102,   87,   22,   89,   24,
   15,   26,   10,   14,   24,   93,   16,   17,   25,   47,
   18,   19,  164,   43,   44,   20,   79,   81,   26,   92,
   49,   21,   73,   88,  110,   72,  111,   73,   27,   22,
   22,   24,  100,   26,   28,   73,   17,   30,  121,   23,
   29,   30,  104,  105,   33,   31,  113,   34,   32,   35,
   52,   53,   54,   55,   72,  135,   36,   37,   38,   72,
   39,   57,  142,   43,   43,  131,  132,   72,   17,   30,
   62,   69,  130,   40,   68,   70,  152,   71,   82,   76,
   83,   84,   85,  138,   90,   94,   95,   96,  162,   97,
   98,   99,  107,  106,  108,   43,  109,  149,  150,  114,
  112,  115,  117,  118,  161,  120,  122,  123,  124,  125,
  128,  168,  126,  129,  133,  134,  171,  136,  139,  102,
  144,  169,  170,  145,  146,  173,  174,  147,  177,  178,
  179,  171,  181,   58,  148,  151,  153,  154,  156,  157,
  160,  159,  163,  175,  158,  180,  182,  167,  183,  184,
    5,  165,    0,  166,  185,    0,  143,  140,  186,    0,
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
    0,   94,  123,   44,   26,   27,   46,   29,   44,   31,
   32,   91,  286,  131,  288,  256,   44,   44,   44,   59,
  292,  293,  257,   93,  289,  290,   66,   93,   68,   93,
   61,   93,   44,   44,  123,   75,   61,  262,  267,  267,
  265,  266,  160,  123,   44,  270,   60,   61,   61,   71,
  267,  276,   93,   67,   94,  125,   96,   93,   61,  125,
  285,  125,   84,  125,   61,   93,   93,   93,  108,  294,
   61,   61,   86,   87,  263,   61,   98,  266,   61,  268,
   61,  125,   61,  265,  125,  125,  275,  276,  277,  125,
  279,   44,  132,   93,   94,  117,  118,  125,  125,  125,
   44,   44,  116,  292,  123,  267,  146,   61,  281,   61,
   61,   61,   44,  127,  293,  123,  125,  123,  158,  284,
   61,  267,   61,  125,   44,  125,  268,  141,  142,   44,
   61,  267,   61,   61,  156,  267,   61,  125,  125,  123,
  287,  163,  282,   44,   44,  267,  168,   61,   61,  290,
   44,  165,  166,  125,  123,  169,  170,  282,  172,  173,
  174,  183,  176,  284,  267,  264,   61,   44,   61,  125,
   44,  288,   61,  275,  123,   61,  267,  125,   44,   44,
    2,  161,   -1,  161,  183,   -1,  133,  129,  184,   -1,
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
case 47:
{
					sg_attributes["reason"] = yyvsp[0];
					sg_children.push_back( sg_attributes );
				}
break;
case 48:
{
					sg_attributes["reason"] = yyvsp[0];
					sg_children.push_back( sg_attributes );
				}
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
case 55:
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
