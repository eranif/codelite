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
#define GDB_DISPLAYHINT 295
#define GDB_HAS_MORE 296
#define GDB_NEW_NUM_CHILDREN 297
#define GDB_NEW_CHILDREN 298
#define GDB_FUNC_NAME 299
#define GDB_OFFSET 300
#define GDB_INSTRUCTION 301
#define GDB_ADDRESS 302
#define GDB_ASM_INSTS 303
#define GDB_THREAD_GROUPS 304
#define GDB_REGISTER_NAMES 305
#define YYERRCODE 256
short gdb_result_lhs[] = {                                        -1,
    0,    0,    3,    1,    1,    4,    4,    2,    2,    9,
    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
   16,    2,    2,    2,   14,   19,   14,   17,   20,   17,
   12,   12,   15,   15,   21,   21,   22,   22,   13,   13,
   23,   24,   23,   11,   11,   10,   10,    5,    5,    7,
    7,    6,    6,    8,   26,    8,    8,   27,    8,    8,
   18,   18,   25,   25,   25,   25,   25,   25,   25,   25,
   25,   25,   25,   25,   25,   25,
};
short gdb_result_len[] = {                                         2,
    1,    2,    0,    2,    1,    0,    4,   13,   17,    0,
    9,    8,    6,    8,    8,   20,   20,    9,    8,    8,
    0,    9,    8,    1,    3,    0,    6,    3,    0,    6,
   13,    5,    1,    3,    3,    5,    3,    5,    6,    2,
    5,    0,    8,    5,    7,    3,    5,    1,    1,    1,
    1,    5,    7,    3,    0,    6,    3,    0,    6,    5,
   11,    5,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,
};
short gdb_result_defred[] = {                                      0,
    5,    0,    1,    0,    2,    0,    0,    4,   24,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   62,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   66,   63,
   69,   68,   64,   65,   67,    0,   70,   71,   72,    0,
   75,   76,   74,   73,    0,    0,    0,   49,   48,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   21,    0,
    0,    0,    0,    0,    0,    0,    0,   10,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   57,   58,    0,    0,    0,   12,   51,   50,
   19,    0,    0,    0,    0,   14,    0,   15,    0,    0,
    0,    0,    0,    0,    0,   23,    0,   20,    0,    0,
    0,    0,    0,    0,    0,   11,    0,   46,    0,    0,
    0,    0,    0,    0,    0,    0,   40,   18,    0,    0,
    0,   22,   60,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   34,   59,
   61,   56,    0,   44,   47,    0,    0,    0,    0,    0,
   32,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   30,   27,    0,   45,    0,    0,
    0,    8,    0,    0,   39,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   52,    0,    0,    0,    0,    0,
    0,    0,    7,    0,    0,   43,    0,    0,   53,    9,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   31,
   16,   17,    0,    0,    0,    0,    0,   36,   38,
};
short gdb_result_dgoto[] = {                                       2,
    3,    8,    4,  192,   87,  168,  101,   56,  102,   81,
   82,   86,  114,   90,  120,   91,   88,    9,  158,  157,
  225,  229,  137,  203,   57,  124,  122,
};
short gdb_result_sindex[] = {                                   -237,
    0, -237,    0,  -94,    0,   -3, -207,    0,    0, -256,
   14,   -2,    4, -232,  -62, -199,    9,   13,   16,   18,
   19,   20,   21,   22,   23,   24,  -58,    0,  -84, -181,
  -84, -178,  -84, -177,  -84,  -84,  -84,  -84,    0,    0,
    0,    0,    0,    0,    0,   30,    0,    0,    0,   31,
    0,    0,    0,    0,   32,  -22,   39,    0,    0, -161,
   61,  -58,   67, -114,   68, -246,  -84,  -84,    0,  -10,
   25,   28,   70, -152,   59,  -58,  -70,    0,   60,  -58,
  -27,  -26, -260,   63,   64,   78,  -58,  -70,  -58,  -70,
 -139,  -58,    0,    0, -164,    0,  -84,    0,    0,    0,
    0,  -58,   10,    7,   12,    0, -147,    0,   77,   79,
 -126,  -84, -243,  -70,  -70,    0,  -70,    0,   99,  -70,
   29,  100,   84,  102, -121,    0,  -58,    0,  -58,   87,
  -84, -112,  109, -139,   97,  101,    0,    0,    0,    0,
 -139,    0,    0,  -58, -103,  -58,  104,   41,   42,   46,
 -111,  128, -110,  -70,  -84,  -84,  129,  131,    0,    0,
    0,    0,  -87,    0,    0,  -58,  117,  -24, -100,  121,
    0, -107,  -58,  -84,  -84,  140,   62,   65,  -97,  142,
  133,  -75,  -70,  -70,    0,    0,  -69,    0,  -58,  137,
  -96,    0,  -84,  145,    0,    0,  138,   76,   80,  143,
 -111,  -82,  163,  -84,    0,  -58,  -55,  -24,  148, -107,
 -114,   89,    0,  142,  -84,    0,  -27,  -26,    0,    0,
  -84,  -70,  -70,  -60,  -70,  -70,  -70,  155,  -70,    0,
    0,    0,  -45,  179,  180,  -84,  -60,    0,    0,
};
short gdb_result_rindex[] = {                                    -89,
    0,  -89,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    2,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    1,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  -68,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  -18,  -17,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    3,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  -15,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    3,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -65,  -61,    0,    0,    0,    0,
};
short gdb_result_gindex[] = {                                      0,
  223,    0,    0,   15,  -25,   26,  -66,   47,    0,   17,
   34,    0,    0,   51,  -78,    0,   56,    0,    0,    0,
   -5,   -4, -158,    0,    0,    0,    0,
};
#define YYTABLESIZE 294
short gdb_result_table[] = {                                       7,
   54,   13,    6,   60,    3,   62,   59,   64,   80,   66,
   67,   68,   69,  183,  106,  108,  105,  107,    1,  179,
  109,  116,  100,  118,   33,   29,   26,   35,   42,   17,
   18,   37,   19,   20,  110,   12,   13,   21,   58,   84,
   10,   85,   89,   22,   55,  135,  136,  138,  139,   11,
  140,  216,   23,  142,   99,  154,   33,   14,   15,   35,
   27,   24,  159,   37,   16,  100,  100,   28,  100,   29,
   25,  125,   26,   30,   28,   25,   31,   41,   32,   33,
   34,   35,   36,   37,   38,   61,  134,  171,   63,   65,
   70,   71,   72,   54,   54,   13,    6,   99,   99,   74,
   99,  180,   73,   75,   76,  151,   28,   25,   77,   41,
   78,   83,   92,   95,   96,   93,  195,  196,   94,   97,
  103,  113,   98,  111,  112,   54,  104,  119,  123,  172,
  173,  128,  127,  115,  129,  117,  130,  131,  121,  132,
  133,  214,  141,  144,  145,  146,  147,  150,  126,   89,
  222,  223,  153,  143,  152,  226,  227,  155,  230,  231,
  232,  156,  234,  161,  163,  164,  165,  201,  166,   79,
  167,  169,  174,  148,  175,  149,  170,  178,  211,  176,
  181,  182,  136,  187,  190,  191,  188,  189,  202,  221,
  160,  194,  162,  193,  197,  224,    6,  199,  204,  200,
  205,    3,  206,  207,   39,  209,  210,   40,  215,   41,
  224,  213,  177,  219,  228,  233,   42,   43,   44,  184,
   45,  235,  236,  237,    5,  186,  208,  217,  220,  185,
  238,    0,  239,   46,    0,  198,   47,   48,   49,   50,
   51,   52,   53,   54,  218,   55,    0,    0,    0,    0,
    0,    0,  212,    0,    0,    0,   54,   13,    6,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   54,   13,    6,
};
short gdb_result_check[] = {                                      94,
    0,    0,    0,   29,   94,   31,   91,   33,  123,   35,
   36,   37,   38,  172,   81,   82,   44,   44,  256,   44,
  281,   88,   93,   90,   93,   44,   44,   93,   44,  262,
  263,   93,  265,  266,  295,  292,  293,  270,  123,  286,
   44,  288,   68,  276,   44,  289,  290,  114,  115,  257,
  117,  210,  285,  120,  125,  134,  125,   44,   61,  125,
  123,  294,  141,  125,   61,   93,   93,  267,   93,   61,
  303,   97,  305,   61,   93,   93,   61,   93,   61,   61,
   61,   61,   61,   61,   61,  267,  112,  154,  267,  267,
   61,   61,   61,   93,   94,   94,   94,  125,  125,   61,
  125,  168,  125,  265,   44,  131,  125,  125,   62,  125,
   44,   44,  123,   44,  267,   91,  183,  184,   91,   61,
   61,   44,   76,   61,   61,  125,   80,  267,  293,  155,
  156,  125,  123,   87,  123,   89,  284,   61,   92,   61,
  267,  208,   44,   44,   61,   44,  268,   61,  102,  175,
  217,  218,   44,  125,  267,  222,  223,   61,  225,  226,
  227,   61,  229,  267,   61,  125,  125,  193,  123,  284,
  282,   44,   44,  127,   44,  129,  287,   61,  204,  267,
  281,   61,  290,   44,  282,   44,  125,  123,   44,  215,
  144,  267,  146,   61,  264,  221,  291,   61,   61,  296,
  125,  291,  123,   61,  263,  288,   44,  266,   61,  268,
  236,  267,  166,  125,  275,   61,  275,  276,  277,  173,
  279,  267,   44,   44,    2,  175,  201,  211,  214,  174,
  236,   -1,  237,  292,   -1,  189,  295,  296,  297,  298,
  299,  300,  301,  302,  211,  304,   -1,   -1,   -1,   -1,
   -1,   -1,  206,   -1,   -1,   -1,  256,  256,  256,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  291,  291,  291,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 305
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
"GDB_DISPLAYHINT","GDB_HAS_MORE","GDB_NEW_NUM_CHILDREN","GDB_NEW_CHILDREN",
"GDB_FUNC_NAME","GDB_OFFSET","GDB_INSTRUCTION","GDB_ADDRESS","GDB_ASM_INSTS",
"GDB_THREAD_GROUPS","GDB_REGISTER_NAMES",
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
"bpt_table_body : ',' GDB_BODY '=' list_open breakpoints list_close",
"bpt_table_body : ',' breakpoints",
"breakpoints : GDB_BKPT '=' list_open child_attributes list_close",
"$$6 :",
"breakpoints : GDB_BKPT '=' list_open child_attributes list_close $$6 ',' breakpoints",
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
"$$7 :",
"child_attributes : child_key '=' GDB_STRING $$7 ',' child_attributes",
"child_attributes : GDB_NEW_CHILDREN '=' '['",
"$$8 :",
"child_attributes : GDB_THREAD_GROUPS '=' '[' $$8 ',' child_attributes",
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
    int depth = 1;
    while(depth > 0) {
        int ch = gdb_result_lex();
        if(ch == 0) {
            break;
        }
        if(ch == ']') {
            depth--;
            continue;
        } else if(ch == ']') {
            depth ++ ;
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
                /*printf("CodeLite: syntax error, unexpected token '%s' found\n", gdb_result_lval.c_str());*/
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
case 19:
{
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
{sg_currentArrayString.clear();}
break;
case 22:
{
                    
                }
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
case 33:
{ sg_currentArrayString.push_back( yyvsp[0] ); }
break;
case 34:
{ sg_currentArrayString.push_back( yyvsp[-2] ); }
break;
case 41:
{ sg_children.push_back( sg_attributes ); sg_attributes.clear();}
break;
case 42:
{ sg_children.push_back( sg_attributes ); sg_attributes.clear();}
break;
case 44:
{
                sg_children.push_back( sg_attributes );
                sg_attributes.clear();
            }
break;
case 45:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 46:
{
                sg_children.push_back( sg_attributes );
                sg_attributes.clear();
            }
break;
case 47:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 52:
{
                    sg_children.push_back( sg_attributes );
                    sg_attributes.clear();
                }
break;
case 53:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 54:
{
                       if ( yyvsp[-2] == "has_more" ) {
                           sg_children.has_more = (yyvsp[0] == "\"1\"");

                       } else if (!yyvsp[0].empty()) {
                           sg_attributes[yyvsp[-2]] = yyvsp[0];
                       }
                   }
break;
case 55:
{ sg_attributes[yyvsp[-2]] = yyvsp[0];}
break;
case 57:
{ gdbConsumeList(); }
break;
case 58:
{ gdbConsumeList(); }
break;
case 61:
{
                    sg_attributes["reason"] = yyvsp[0];
                    sg_children.push_back( sg_attributes );
                }
break;
case 62:
{
                    sg_attributes["reason"] = yyvsp[0];
                    sg_children.push_back( sg_attributes );
                }
break;
case 63:
{yyval = yyvsp[0];}
break;
case 64:
{yyval = yyvsp[0];}
break;
case 65:
{yyval = yyvsp[0];}
break;
case 66:
{yyval = yyvsp[0];}
break;
case 67:
{yyval = yyvsp[0];}
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
