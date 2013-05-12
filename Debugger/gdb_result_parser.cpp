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
#define YYERRCODE 256
short gdb_result_lhs[] = {                                        -1,
    0,    0,    3,    1,    1,    4,    4,    2,    2,    9,
    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
    2,    2,   14,   17,   14,   15,   18,   15,   12,   12,
   20,   20,   19,   19,   21,   21,   13,   13,   22,   23,
   22,   11,   11,   10,   10,    5,    5,    7,    7,    6,
    6,    8,   25,    8,    8,    8,    8,   16,   16,   24,
   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,
   24,   24,   24,
};
short gdb_result_len[] = {                                         2,
    1,    2,    0,    2,    1,    0,    4,   13,   17,    0,
    9,    8,    6,    8,    8,   20,   20,    9,    8,    8,
    8,    1,    3,    0,    6,    3,    0,    6,   13,    5,
    1,    3,    3,    5,    3,    5,    6,    2,    5,    0,
    8,    5,    7,    3,    5,    1,    1,    1,    1,    5,
    7,    3,    0,    6,    3,    3,    5,   11,    5,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,
};
short gdb_result_defred[] = {                                      0,
    5,    0,    1,    0,    2,    0,    0,    4,   22,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   59,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   63,   60,   66,   65,
   61,   62,   64,    0,   67,   68,   69,    0,   72,   73,
   71,   70,    0,    0,    0,   47,   46,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   10,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   55,   56,
    0,    0,    0,   12,   49,   48,   19,    0,    0,    0,
    0,   14,    0,   15,    0,    0,    0,    0,    0,    0,
    0,   21,    0,   20,    0,    0,    0,    0,   11,    0,
   44,    0,    0,    0,    0,    0,    0,    0,    0,   38,
   18,    0,    0,   57,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   58,
   54,    0,   42,   45,    0,    0,    0,    0,    0,    0,
   30,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   32,    0,    0,   28,   25,    0,   43,    0,
    0,    0,    8,    0,    0,   37,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   50,    0,    0,    0,    0,
    0,    0,    0,    7,    0,    0,   41,    0,    0,   51,
    9,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   29,   16,   17,    0,    0,    0,    0,    0,   34,   36,
};
short gdb_result_dgoto[] = {                                       2,
    3,    8,    4,  183,   84,  157,   97,   54,   98,   78,
   79,   83,  110,   87,   85,    9,  149,  148,  216,  145,
  220,  130,  194,   55,  117,
};
short gdb_result_sindex[] = {                                   -227,
    0, -227,    0,  -94,    0,  -11, -217,    0,    0, -257,
   -3,  -17,   -8, -146,  -68, -211,   -4,    4,    7,    9,
   11,   14,   15,   16,   17,  -85,    0,  -75, -206,  -75,
 -188,  -75, -187,  -75,  -75,  -75,    0,    0,    0,    0,
    0,    0,    0,   21,    0,    0,    0,   22,    0,    0,
    0,    0,   24,  -38,   28,    0,    0, -174,   48,  -85,
   57, -116,   60, -254,  -75,  -75,  -16,   18,   23,   64,
 -155,   52,  -85,  -74,    0,   54,  -85,  -35,  -27, -268,
   66,   67,   74,  -85,  -74,  -85,  -74,  -85,    0,    0,
 -168,    0,  -75,    0,    0,    0,    0,  -85,    8,   12,
   10,    0, -152,    0,   73,   75, -129,  -75, -252,  -74,
  -74,    0,  -74,    0,   26,   79,   98, -125,    0,  -85,
    0,  -85,   86,  -75, -112,  114, -108,  100,  101,    0,
    0,    0,    0,    0, -103,  -85,  104,   41,   42,   46,
 -110,  126, -114,  130,  -74,  -75,  -75,  131,  132,    0,
    0,  -90,    0,    0,  -85,  118,  -26, -101,  123, -108,
    0, -105,  -85,  -75,  -75,  142,   62,   70,  -93,  151,
  135,  -69,    0,  -74,  -74,    0,    0,  -60,    0,  -85,
  138,  -96,    0,  -75,  157,    0,    0,  144,   81,   85,
  148, -110,  -70,  176,  -75,    0,  -85,  -46,  -26,  161,
 -105, -116,   99,    0,  151,  -75,    0,  -35,  -27,    0,
    0,  -75,  -74,  -74,  -52,  -74,  -74,  -74,  164,  -74,
    0,    0,    0,  -41,  183,  184,  -75,  -52,    0,    0,
};
short gdb_result_rindex[] = {                                    -89,
    0,  -89,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    2,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    1,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  -22,  -20,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -65,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    3,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  -19,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    3,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -63,  -62,    0,    0,    0,    0,
};
short gdb_result_gindex[] = {                                      0,
  227,    0,    0,   25,  -24,   39,  -64,  -34,    0,   30,
   31,    0,    0,   69,   71,    0,    0,    0,   13,   76,
   19, -142,    0,    0,    0,
};
#define YYTABLESIZE 294
short gdb_result_table[] = {                                       7,
   52,   13,    6,   58,    3,   60,   77,   62,  101,   64,
   65,   66,  105,  102,  104,   57,  103,  169,   96,  174,
  112,   27,  114,   24,   40,   74,  106,   31,    1,   33,
   35,   81,   10,   82,   12,   13,  128,  129,   94,   11,
   14,   86,  100,   15,   53,  131,  132,   56,  133,  111,
   95,  113,   16,  115,   26,   27,   28,   96,  207,   31,
   59,   33,   35,  119,   29,   96,   96,   30,  118,   31,
   26,   32,   23,   39,   33,   34,   35,   36,   61,   63,
  161,   67,   68,  127,   69,  138,   70,  139,   71,   95,
   72,   73,  170,   52,   52,   13,    6,   95,   95,  141,
   75,  151,   26,   80,   23,   39,   88,   91,   89,  186,
  187,   92,   93,   90,   99,   17,   18,  109,   19,   20,
  167,  162,  163,   21,  116,   52,  107,  108,  175,   22,
  120,  123,  122,  124,  205,  125,  121,  126,   23,  135,
   86,  136,  137,  213,  214,  189,  140,   24,  217,  218,
  134,  221,  222,  223,  142,  225,   25,  143,  144,  192,
  146,  147,  203,  150,  152,  153,  154,   76,  155,  158,
  202,  156,  159,  160,  164,  165,  166,   37,  168,  171,
   38,  212,   39,  172,  129,  178,  179,  215,  181,   40,
   41,   42,  180,   43,  182,  184,    6,  185,  190,  191,
  193,    3,  215,  188,  195,  196,   44,  197,  198,   45,
   46,   47,   48,   49,   50,   51,   52,  200,   53,  201,
  204,  206,  219,  210,  224,  226,  227,  228,    5,  211,
  199,  208,  209,  177,  176,  173,    0,    0,    0,  229,
    0,    0,    0,    0,    0,    0,  230,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   52,   13,    6,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   52,   13,    6,
};
short gdb_result_check[] = {                                      94,
    0,    0,    0,   28,   94,   30,  123,   32,   44,   34,
   35,   36,  281,   78,   79,   91,   44,   44,   93,  162,
   85,   44,   87,   44,   44,   60,  295,   93,  256,   93,
   93,  286,   44,  288,  292,  293,  289,  290,   73,  257,
   44,   66,   77,   61,   44,  110,  111,  123,  113,   84,
  125,   86,   61,   88,  123,  267,   61,   93,  201,  125,
  267,  125,  125,   98,   61,   93,   93,   61,   93,   61,
   93,   61,   93,   93,   61,   61,   61,   61,  267,  267,
  145,   61,   61,  108,   61,  120,  125,  122,   61,  125,
  265,   44,  157,   93,   94,   94,   94,  125,  125,  124,
   44,  136,  125,   44,  125,  125,  123,   44,   91,  174,
  175,  267,   61,   91,   61,  262,  263,   44,  265,  266,
  155,  146,  147,  270,  293,  125,   61,   61,  163,  276,
  123,  284,  123,   61,  199,   61,  125,  267,  285,   61,
  165,   44,  268,  208,  209,  180,   61,  294,  213,  214,
  125,  216,  217,  218,  267,  220,  303,   44,  267,  184,
   61,   61,  197,  267,   61,  125,  125,  284,  123,   44,
  195,  282,  287,   44,   44,   44,  267,  263,   61,  281,
  266,  206,  268,   61,  290,   44,  125,  212,  282,  275,
  276,  277,  123,  279,   44,   61,  291,  267,   61,  296,
   44,  291,  227,  264,   61,  125,  292,  123,   61,  295,
  296,  297,  298,  299,  300,  301,  302,  288,  304,   44,
  267,   61,  275,  125,   61,  267,   44,   44,    2,  205,
  192,  202,  202,  165,  164,  160,   -1,   -1,   -1,  227,
   -1,   -1,   -1,   -1,   -1,   -1,  228,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  256,  256,  256,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  291,  291,  291,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 304
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
"GDB_THREAD_GROUPS",
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
"child_pattern : '^' GDB_DONE ',' GDB_CHANGELIST '=' list_open change_set list_close",
"child_pattern : stop_statement",
"asm_insts : list_open child_attributes list_close",
"$$3 :",
"asm_insts : list_open child_attributes list_close $$3 ',' asm_insts",
"change_set : list_open child_attributes list_close",
"$$4 :",
"change_set : list_open child_attributes list_close $$4 ',' change_set",
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
"$$5 :",
"breakpoints : GDB_BKPT '=' list_open child_attributes list_close $$5 ',' breakpoints",
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
"$$6 :",
"child_attributes : child_key '=' GDB_STRING $$6 ',' child_attributes",
"child_attributes : GDB_NEW_CHILDREN '=' '['",
"child_attributes : GDB_THREAD_GROUPS '=' '['",
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
case 23:
{
                    sg_children.push_back( sg_attributes );
                    sg_attributes.clear();
                }
break;
case 24:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
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
case 39:
{ sg_children.push_back( sg_attributes ); sg_attributes.clear();}
break;
case 40:
{ sg_children.push_back( sg_attributes ); sg_attributes.clear();}
break;
case 42:
{
                sg_children.push_back( sg_attributes );
                sg_attributes.clear();
            }
break;
case 43:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
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
case 50:
{
                    sg_children.push_back( sg_attributes );
                    sg_attributes.clear();
                }
break;
case 51:
{sg_children.push_back( sg_attributes ); sg_attributes.clear(); }
break;
case 52:
{
                       if ( yyvsp[-2] == "has_more" ) {
                           sg_children.has_more = (yyvsp[0] == "\"1\"");

                       } else if (!yyvsp[0].empty()) {
                           sg_attributes[yyvsp[-2]] = yyvsp[0];
                       }
                   }
break;
case 53:
{ sg_attributes[yyvsp[-2]] = yyvsp[0];}
break;
case 55:
{ gdbConsumeList(); }
break;
case 56:
{ gdbConsumeList(); }
break;
case 58:
{
                    sg_attributes["reason"] = yyvsp[0];
                    sg_children.push_back( sg_attributes );
                }
break;
case 59:
{
                    sg_attributes["reason"] = yyvsp[0];
                    sg_children.push_back( sg_attributes );
                }
break;
case 60:
{yyval = yyvsp[0];}
break;
case 61:
{yyval = yyvsp[0];}
break;
case 62:
{yyval = yyvsp[0];}
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
