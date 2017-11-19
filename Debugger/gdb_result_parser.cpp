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
#include "gdb_parser_incl.h"
#include <stdlib.h>

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
static GdbStringMap_t            sg_attributes;
static GdbChildrenInfo           sg_children;
static std::vector<std::string>  sg_locals;
static std::vector<std::string>  sg_currentArrayString;
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
#define GDB_VARIABLES 271
#define GDB_INTEGER 272
#define GDB_OCTAL 273
#define GDB_HEX 274
#define GDB_FLOAT 275
#define GDB_IDENTIFIER 276
#define GDB_NUMCHILD 277
#define GDB_TYPE 278
#define GDB_DATA 279
#define GDB_ADDR 280
#define GDB_ASCII 281
#define GDB_CHILDREN 282
#define GDB_CHILD 283
#define GDB_MORE 284
#define GDB_VAROBJ 285
#define GDB_BREAKPOINT_TABLE 286
#define GDB_NR_ROWS 287
#define GDB_NR_COLS 288
#define GDB_HDR 289
#define GDB_BODY 290
#define GDB_BKPT 291
#define GDB_STOPPED 292
#define GDB_TIME 293
#define GDB_REASON 294
#define GDB_CHANGELIST 295
#define GDB_DISPLAYHINT 296
#define GDB_HAS_MORE 297
#define GDB_NEW_NUM_CHILDREN 298
#define GDB_NEW_CHILDREN 299
#define GDB_FUNC_NAME 300
#define GDB_OFFSET 301
#define GDB_INSTRUCTION 302
#define GDB_ADDRESS 303
#define GDB_ASM_INSTS 304
#define GDB_THREAD_GROUPS 305
#define GDB_REGISTER_NAMES 306
#define GDB_DYNAMIC 307
#define YYERRCODE 256
short gdb_result_lhs[] = {                                        -1,
    0,    0,    3,    1,    1,    4,    4,    2,    2,    9,
    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
    2,   16,    2,    2,    2,   14,   19,   14,   17,   20,
   17,   12,   12,   15,   15,   21,   21,   22,   22,   13,
   13,   24,   23,   23,   11,   11,   10,   10,    5,    5,
    7,    7,    6,    6,   25,   25,    8,   27,    8,    8,
   28,    8,    8,    8,    8,   18,   18,   26,   26,   26,
   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,
   26,   26,
};
short gdb_result_len[] = {                                         2,
    1,    2,    0,    2,    1,    0,    4,   13,   17,    0,
    9,    8,    6,    8,    8,    8,   20,   20,    9,    8,
    8,    0,    9,    8,    1,    3,    0,    6,    3,    0,
    6,   13,    5,    1,    3,    3,    5,    3,    5,    6,
    2,    3,    1,    3,    5,    7,    3,    5,    1,    1,
    1,    1,    5,    7,    1,    3,    3,    0,    6,    3,
    0,    6,    5,    7,    5,   11,    5,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,
};
short gdb_result_defred[] = {                                      0,
    5,    0,    1,    0,    2,    0,    0,    4,   25,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   67,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   71,   68,   74,   73,   69,   70,   72,    0,   75,   76,
   77,    0,   80,   81,   79,   78,    0,   82,    0,    0,
   49,   50,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   22,    0,    0,    0,    0,    0,    0,    0,
    0,   10,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   12,   51,   52,   20,    0,    0,    0,    0,
   14,    0,   16,   15,    0,    0,    0,    0,    0,    0,
    0,   24,    0,   21,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   11,    0,   47,    0,    0,    0,    0,
    0,    0,    0,    0,   41,    0,   19,    0,    0,    0,
   23,   65,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   35,   62,   56,    0,   66,   59,    0,   45,   48,    0,
    0,    0,    0,    0,   33,    0,   42,   44,    0,    0,
   64,    0,    0,    0,    0,    0,    0,    0,    0,   31,
   28,    0,   46,    0,    0,    0,    8,    0,    0,   40,
    0,    0,    0,    0,    0,    0,    0,   53,    0,    0,
    0,    0,    0,    0,    7,    0,    0,    0,    0,   54,
    9,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   32,   17,   18,    0,    0,    0,    0,    0,   37,   39,
};
short gdb_result_dgoto[] = {                                       2,
    3,    8,    4,  207,   92,  182,  106,   59,  107,   85,
   86,   91,  120,   95,  126,   96,   93,    9,  170,  169,
  236,  240,  145,  146,  130,   60,  132,  128,
};
short gdb_result_sindex[] = {                                   -241,
    0, -241,    0,  -94,    0,  -15, -216,    0,    0, -243,
    4,  -17,   -8,   17,  -69, -212,   -4,   -3,   11,   15,
   16,   18,   19,   20,   21,   22,   23,  -29,    0,  -74,
 -203,  -74, -202,  -74,  -74, -201,  -74,  -74,  -74,  -74,
    0,    0,    0,    0,    0,    0,    0,   24,    0,    0,
    0,   26,    0,    0,    0,    0,   27,    0,  -47,   28,
    0,    0, -162,   60,  -29,   64, -115,  -13,   67, -251,
  -74,  -74,    0,  -10,   29,   30,   70, -152,   55,  -29,
  -73,    0,   57,  -29,  -34,  -33,  -34, -269,   63,   66,
   81,  -29,  -73,  -29,  -73, -136,  -29,    0, -134, -160,
    0,  -74,    0,    0,    0,    0,  -29,   12,   13,   14,
    0, -149,    0,    0,   78,   79, -126,  -74, -119,  -73,
  -73,    0,  -73,    0,   98,  -73,   31,   99,  100,   52,
   85,  103, -120,    0,  -29,    0,  -29,   88,  -74, -117,
  107, -136,   91,  -29,    0,  109,    0,    0,    0, -136,
    0,    0,  -29, -134,  110, -112,  -29,   96,   33,   34,
   38, -118,  119, -122,  -73,   76,   47,   50,  133,  135,
    0,    0,    0,  -29,    0,    0,  -81,    0,    0,  -29,
  127,  -32,  -93,  129,    0,   50,    0,    0,  -74,  -74,
    0,  147,   68,   71,  -88,  152,  136,  -65,  111,    0,
    0,  -61,    0,  -29,  145,  -90,    0,  -74,  164,    0,
  148,   87,   90,  149, -118,  -75,  -74,    0,  -29,  -52,
  -32,  156, -115,   93,    0,  152,  -74,  -34,  -33,    0,
    0,  -74,  -73,  -73,  -57,  -73,  -73,  -73,  159,  -73,
    0,    0,    0,  -46,  178,  179,  -74,  -57,    0,    0,
};
short gdb_result_rindex[] = {                                    -92,
    0,  -92,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    6,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    1,    0,    0,
    3,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -63,    0,    0,    0,  131,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  -62,    0,  -19,  -18,    0,
    0,    0,    0,    0,    5,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    7,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    7,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -56,  -54,    0,    0,    0,    0,
};
short gdb_result_gindex[] = {                                      0,
  223,    0,    0,    2,  -16,   35,  -53,   25,    0,  -59,
    9,    0,    0,   36, -107,    0,   41,    0,    0,    0,
  -20,  -12, -140,    0,   84,    0,    0,    0,
};
#define YYTABLESIZE 323
short gdb_result_table[] = {                                       7,
   60,    3,   57,  144,   63,   13,    6,   84,   87,  110,
  112,  195,  115,   63,    1,   65,   61,   67,   68,  104,
   70,   71,   72,   73,   30,   27,  116,  188,   10,   34,
   43,  111,  113,  114,  165,   89,   36,   90,   38,  122,
   11,  124,  171,   15,   61,  199,   58,   14,   62,   12,
   13,  105,   16,   28,   29,   94,   30,   31,  104,  104,
  104,   34,   43,   64,   66,   69,  147,  148,   36,  149,
   38,   32,  151,   29,   26,   33,   34,   77,   35,   36,
   37,   38,   39,   40,   74,  133,   75,   76,   78,   81,
  105,  105,  105,   60,   60,   57,   57,   63,   63,   13,
    6,  142,   79,   80,  103,   29,   26,   82,  109,   84,
   88,  185,   97,  100,  101,  102,  121,  108,  123,   98,
   99,  127,  162,  117,  119,   60,  118,   57,  196,   63,
  125,  134,  129,  131,  135,  138,  137,  136,  139,  140,
  141,  150,  153,  154,  155,  156,  157,  158,  161,  163,
  164,  166,  168,  174,  175,  152,  177,  178,  179,  159,
  180,  160,  183,  228,  181,  184,  186,  226,  167,   83,
  143,  187,  144,   94,  233,  234,  189,  172,  190,  237,
  238,  176,  241,  242,  243,  192,  245,  194,  197,  198,
  202,  215,  203,  204,  205,  206,  208,    6,  191,    3,
  223,  209,  211,  210,  193,  213,  214,  216,  217,  220,
  232,  218,  219,  222,  225,  235,  227,  230,  239,  244,
  246,  247,  248,   55,    5,  201,  249,  231,  212,  200,
  235,  229,    0,   41,    0,  250,   42,  173,   43,    0,
    0,    0,    0,  224,    0,    0,   44,   45,   46,  221,
   47,    0,    0,    0,    0,    0,   60,    0,   57,    0,
   63,   13,    6,   48,    0,    0,   49,   50,   51,   52,
   53,   54,   55,   56,    0,   57,    0,   58,   17,   18,
    0,   19,   20,    0,    0,    0,   21,   22,    0,    0,
    0,    0,   60,   23,   57,    0,   63,   13,    6,    0,
    0,    0,   24,    0,    0,    0,    0,    0,    0,    0,
    0,   25,    0,    0,    0,    0,    0,    0,    0,    0,
   26,    0,   27,
};
short gdb_result_check[] = {                                      94,
    0,   94,    0,  123,    0,    0,    0,  123,   68,   44,
   44,   44,  282,   30,  256,   32,   91,   34,   35,   93,
   37,   38,   39,   40,   44,   44,  296,  168,   44,   93,
   93,   85,   86,   87,  142,  287,   93,  289,   93,   93,
  257,   95,  150,   61,   44,  186,   44,   44,  123,  293,
  294,  125,   61,  123,  267,   72,   61,   61,   93,   93,
   93,  125,  125,  267,  267,  267,  120,  121,  125,  123,
  125,   61,  126,   93,   93,   61,   61,  125,   61,   61,
   61,   61,   61,   61,   61,  102,   61,   61,   61,   65,
  125,  125,  125,   93,   94,   93,   94,   93,   94,   94,
   94,  118,  265,   44,   80,  125,  125,   44,   84,  123,
   44,  165,  123,   44,  267,   61,   92,   61,   94,   91,
   91,   97,  139,   61,   44,  125,   61,  125,  182,  125,
  267,  107,  267,  294,  123,  285,  123,  125,   61,   61,
  267,   44,   44,   44,   93,   61,   44,  268,   61,  267,
   44,   61,   44,   44,  267,  125,   61,  125,  125,  135,
  123,  137,   44,  223,  283,  288,   91,  221,  144,  285,
  290,  125,  123,  190,  228,  229,   44,  153,   44,  233,
  234,  157,  236,  237,  238,  267,  240,   61,  282,   61,
   44,  208,  125,  123,  283,   44,   61,  292,  174,  292,
  217,  267,  264,   93,  180,   61,  297,   44,   61,   61,
  227,  125,  123,  289,  267,  232,   61,  125,  276,   61,
  267,   44,   44,   93,    2,  190,  247,  226,  204,  189,
  247,  223,   -1,  263,   -1,  248,  266,  154,  268,   -1,
   -1,   -1,   -1,  219,   -1,   -1,  276,  277,  278,  215,
  280,   -1,   -1,   -1,   -1,   -1,  256,   -1,  256,   -1,
  256,  256,  256,  293,   -1,   -1,  296,  297,  298,  299,
  300,  301,  302,  303,   -1,  305,   -1,  307,  262,  263,
   -1,  265,  266,   -1,   -1,   -1,  270,  271,   -1,   -1,
   -1,   -1,  292,  277,  292,   -1,  292,  292,  292,   -1,
   -1,   -1,  286,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  295,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  304,   -1,  306,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 307
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
"GDB_LEVEL","GDB_ESCAPED_STRING","GDB_LOCALS","GDB_VARIABLES","GDB_INTEGER",
"GDB_OCTAL","GDB_HEX","GDB_FLOAT","GDB_IDENTIFIER","GDB_NUMCHILD","GDB_TYPE",
"GDB_DATA","GDB_ADDR","GDB_ASCII","GDB_CHILDREN","GDB_CHILD","GDB_MORE",
"GDB_VAROBJ","GDB_BREAKPOINT_TABLE","GDB_NR_ROWS","GDB_NR_COLS","GDB_HDR",
"GDB_BODY","GDB_BKPT","GDB_STOPPED","GDB_TIME","GDB_REASON","GDB_CHANGELIST",
"GDB_DISPLAYHINT","GDB_HAS_MORE","GDB_NEW_NUM_CHILDREN","GDB_NEW_CHILDREN",
"GDB_FUNC_NAME","GDB_OFFSET","GDB_INSTRUCTION","GDB_ADDRESS","GDB_ASM_INSTS",
"GDB_THREAD_GROUPS","GDB_REGISTER_NAMES","GDB_DYNAMIC",
};
char *gdb_result_rule[] = {
"$accept : parse",
"parse : children_list",
"parse : parse children_list",
"$$1 :",
"children_list : $$1 child_pattern",
"children_list : error",
"has_more_attr :",
"has_more_attr : ',' GDB_HAS_MORE '=' GDB_STRING",
"child_pattern : '^' GDB_DONE ',' GDB_NUMCHILD '=' GDB_STRING ',' GDB_CHILDREN '=' list_open children list_close has_more_attr",
"child_pattern : '^' GDB_DONE ',' GDB_NUMCHILD '=' GDB_STRING ',' GDB_DISPLAYHINT '=' GDB_STRING ',' GDB_CHILDREN '=' list_open children list_close has_more_attr",
"$$2 :",
"child_pattern : '^' GDB_DONE ',' GDB_NAME '=' GDB_STRING ',' $$2 child_attributes",
"child_pattern : '^' GDB_DONE ',' GDB_VALUE '=' GDB_STRING ',' child_attributes",
"child_pattern : '^' GDB_DONE ',' GDB_VALUE '=' GDB_STRING",
"child_pattern : '^' GDB_DONE ',' GDB_LOCALS '=' list_open locals list_close",
"child_pattern : '^' GDB_DONE ',' GDB_VARIABLES '=' list_open locals list_close",
"child_pattern : '^' GDB_DONE ',' GDB_LOCALS '=' list_open mac_locals list_close",
"child_pattern : '^' GDB_DONE ',' GDB_STACK_ARGS '=' list_open GDB_FRAME '=' list_open GDB_LEVEL '=' GDB_STRING ',' GDB_ARGS '=' list_open locals list_close list_close list_close",
"child_pattern : '^' GDB_DONE ',' GDB_STACK_ARGS '=' list_open GDB_FRAME '=' list_open GDB_LEVEL '=' GDB_STRING ',' GDB_ARGS '=' list_open mac_locals list_close list_close list_close",
"child_pattern : '^' GDB_DONE ',' GDB_BREAKPOINT_TABLE '=' list_open bpt_table_hdr bpt_table_body list_close",
"child_pattern : '^' GDB_DONE ',' GDB_FRAME '=' list_open child_attributes list_close",
"child_pattern : '^' GDB_DONE ',' GDB_ASM_INSTS '=' list_open asm_insts list_close",
"$$3 :",
"child_pattern : '^' GDB_DONE ',' GDB_REGISTER_NAMES '=' list_open $$3 comma_separated_strings list_close",
"child_pattern : '^' GDB_DONE ',' GDB_CHANGELIST '=' list_open change_set list_close",
"child_pattern : stop_statement",
"asm_insts : list_open child_attributes list_close",
"$$4 :",
"asm_insts : list_open child_attributes list_close $$4 ',' asm_insts",
"change_set : list_open child_attributes list_close",
"$$5 :",
"change_set : list_open child_attributes list_close $$5 ',' change_set",
"bpt_table_hdr : GDB_NR_ROWS '=' GDB_STRING ',' GDB_NR_COLS '=' GDB_STRING ',' GDB_HDR '=' list_open bpt_hdr_table_description list_close",
"bpt_table_hdr : GDB_HDR '=' list_open comma_separated_strings list_close",
"comma_separated_strings : GDB_STRING",
"comma_separated_strings : GDB_STRING ',' comma_separated_strings",
"bpt_hdr_table_description : list_open bpt_table_description_attr list_close",
"bpt_hdr_table_description : list_open bpt_table_description_attr list_close ',' bpt_hdr_table_description",
"bpt_table_description_attr : GDB_IDENTIFIER '=' GDB_STRING",
"bpt_table_description_attr : GDB_IDENTIFIER '=' GDB_STRING ',' bpt_table_description_attr",
"bpt_table_body : ',' GDB_BODY '=' '[' breakpoints ']'",
"bpt_table_body : ',' breakpoints",
"bp_internal : '{' child_attributes '}'",
"breakpoints : bp_internal",
"breakpoints : bp_internal ',' breakpoints",
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
"string_list : GDB_STRING",
"string_list : GDB_STRING ',' string_list",
"child_attributes : child_key '=' GDB_STRING",
"$$6 :",
"child_attributes : child_key '=' GDB_STRING $$6 ',' child_attributes",
"child_attributes : GDB_NEW_CHILDREN '=' '['",
"$$7 :",
"child_attributes : GDB_NEW_CHILDREN '=' '[' $$7 ',' child_attributes",
"child_attributes : GDB_THREAD_GROUPS '=' '[' string_list ']'",
"child_attributes : GDB_THREAD_GROUPS '=' '[' string_list ']' ',' child_attributes",
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
"child_key : GDB_DISPLAYHINT",
"child_key : GDB_HAS_MORE",
"child_key : GDB_NEW_NUM_CHILDREN",
"child_key : GDB_ADDRESS",
"child_key : GDB_INSTRUCTION",
"child_key : GDB_FUNC_NAME",
"child_key : GDB_OFFSET",
"child_key : GDB_DYNAMIC",
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
    sg_currentArrayString.clear();
}

void gdbConsumeList()
{
    //printf("Consume List is called\n");
    int depth = 1;
    while(depth > 0) {
        int ch = gdb_result_lex();
        if(ch == 0) {
            break;
        }
        if(ch == ']') {
            depth--;
        } else if(ch == '[') {
            depth++;
            continue;
        }
    }
}

void gdbParseListChildren( const std::string &in, GdbChildrenInfo &children)
{
    cleanup();

    setGdbLexerInput(in, true, false);
    gdb_result_parse();

    children = sg_children;
    gdb_result_lex_clean();
}

void gdbParseRegisterNames( const std::string &in, std::vector<std::string>& names )
{
    cleanup();

    setGdbLexerInput(in, true, false);
    gdb_result_parse();

    names = sg_currentArrayString;
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
                //printf("CodeLite: syntax error, unexpected token '%s' found\n", gdb_result_lval.c_str());
                }
break;
case 7:
{
                  sg_children.has_more = (yyvsp[0] == "\"1\"");
              }
break;
case 10:
{sg_attributes[yyvsp[-3]] = yyvsp[-1];}
break;
case 11:
{
                    sg_children.push_back( sg_attributes );
                    sg_attributes.clear();
                }
break;
case 12:
{
                    sg_attributes[yyvsp[-4]] = yyvsp[-2];
                    sg_children.push_back( sg_attributes );
                    sg_attributes.clear();
                }
break;
case 13:
{
                    sg_attributes[yyvsp[-2]] = yyvsp[0];
                    sg_children.push_back( sg_attributes );
                    sg_attributes.clear();
                }
break;
case 20:
{
                    sg_children.push_back( sg_attributes );
                    sg_attributes.clear();
                }
break;
case 21:
{
                    sg_children.push_back( sg_attributes );
                    sg_attributes.clear();
                }
break;
case 22:
{sg_currentArrayString.clear();}
break;
case 23:
{
                    
                }
break;
case 26:
{
                    sg_children.push_back( sg_attributes );
                    sg_attributes.clear();
                }
break;
case 27:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 29:
{
                    sg_children.push_back( sg_attributes );
                    sg_attributes.clear();
                }
break;
case 30:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 34:
{ sg_currentArrayString.push_back( yyvsp[0] ); }
break;
case 35:
{ sg_currentArrayString.push_back( yyvsp[-2] ); }
break;
case 42:
{ sg_children.push_back( sg_attributes ); sg_attributes.clear();}
break;
case 45:
{
                sg_children.push_back( sg_attributes );
                sg_attributes.clear();
            }
break;
case 46:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 47:
{
                sg_children.push_back( sg_attributes );
                sg_attributes.clear();
            }
break;
case 48:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 53:
{
                    sg_children.push_back( sg_attributes );
                    sg_attributes.clear();
                }
break;
case 54:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 57:
{
                        if ( yyvsp[-2] == "has_more" || yyvsp[-2] == "dynamic" ) {
                           sg_children.has_more = (yyvsp[0] == "\"1\"");
                           
                        } 
                        if (!yyvsp[0].empty()) {
                           sg_attributes[yyvsp[-2]] = yyvsp[0];
                        }
                    }
break;
case 58:
{ sg_attributes[yyvsp[-2]] = yyvsp[0];}
break;
case 60:
{ gdbConsumeList(); }
break;
case 61:
{ gdbConsumeList(); }
break;
case 66:
{
                    sg_attributes["reason"] = yyvsp[0];
                    sg_children.push_back( sg_attributes );
                }
break;
case 67:
{
                    sg_attributes["reason"] = yyvsp[0];
                    sg_children.push_back( sg_attributes );
                }
break;
case 68:
{yyval = yyvsp[0];}
break;
case 69:
{yyval = yyvsp[0];}
break;
case 70:
{yyval = yyvsp[0];}
break;
case 71:
{yyval = yyvsp[0];}
break;
case 72:
{yyval = yyvsp[0];}
break;
case 73:
{yyval = yyvsp[0];}
break;
case 74:
{yyval = yyvsp[0];}
break;
case 75:
{yyval = yyvsp[0];}
break;
case 76:
{yyval = yyvsp[0];}
break;
case 77:
{yyval = yyvsp[0];}
break;
case 78:
{yyval = yyvsp[0];}
break;
case 79:
{yyval = yyvsp[0];}
break;
case 80:
{yyval = yyvsp[0];}
break;
case 81:
{yyval = yyvsp[0];}
break;
case 82:
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
