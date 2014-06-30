//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : gdb_result_parser.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

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
#define YYERRCODE 256
short gdb_result_lhs[] = {                                        -1,
    0,    0,    3,    1,    1,    4,    4,    2,    2,    9,
    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
    2,   16,    2,    2,    2,   14,   19,   14,   17,   20,
   17,   12,   12,   15,   15,   21,   21,   22,   22,   13,
   13,   23,   24,   23,   11,   11,   10,   10,    5,    5,
    7,    7,    6,    6,    8,   26,    8,    8,   27,    8,
    8,   18,   18,   25,   25,   25,   25,   25,   25,   25,
   25,   25,   25,   25,   25,   25,   25,
};
short gdb_result_len[] = {                                         2,
    1,    2,    0,    2,    1,    0,    4,   13,   17,    0,
    9,    8,    6,    8,    8,    8,   20,   20,    9,    8,
    8,    0,    9,    8,    1,    3,    0,    6,    3,    0,
    6,   13,    5,    1,    3,    3,    5,    3,    5,    6,
    2,    5,    0,    8,    5,    7,    3,    5,    1,    1,
    1,    1,    5,    7,    3,    0,    6,    3,    0,    6,
    5,   11,    5,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,
};
short gdb_result_defred[] = {                                      0,
    5,    0,    1,    0,    2,    0,    0,    4,   25,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   63,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   67,   64,   70,   69,   65,   66,   68,    0,   71,   72,
   73,    0,   76,   77,   75,   74,    0,    0,    0,   50,
   49,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   22,    0,    0,    0,    0,    0,    0,    0,    0,
   10,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   58,   59,    0,    0,
    0,   12,   52,   51,   20,    0,    0,    0,    0,   14,
    0,   16,   15,    0,    0,    0,    0,    0,    0,    0,
   24,    0,   21,    0,    0,    0,    0,    0,    0,    0,
   11,    0,   47,    0,    0,    0,    0,    0,    0,    0,
    0,   41,   19,    0,    0,    0,   23,   61,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   35,   60,   62,   57,    0,   45,   48,
    0,    0,    0,    0,    0,   33,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   31,
   28,    0,   46,    0,    0,    0,    8,    0,    0,   40,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   53,
    0,    0,    0,    0,    0,    0,    0,    7,    0,    0,
   44,    0,    0,   54,    9,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   32,   17,   18,    0,    0,    0,
    0,    0,   37,   39,
};
short gdb_result_dgoto[] = {                                       2,
    3,    8,    4,  197,   91,  173,  105,   58,  106,   84,
   85,   90,  119,   94,  125,   95,   92,    9,  163,  162,
  230,  234,  142,  208,   59,  129,  127,
};
short gdb_result_sindex[] = {                                   -222,
    0, -222,    0,  -94,    0,   -7, -213,    0,    0, -253,
    4,  -14,  -11,  -52,  -70, -212,   -5,   -1,    7,   10,
   14,   16,   17,   18,   19,   20,   21,  -15,    0,  -72,
 -206,  -72, -205,  -72,  -72, -202,  -72,  -72,  -72,  -72,
    0,    0,    0,    0,    0,    0,    0,   22,    0,    0,
    0,   23,    0,    0,    0,    0,   24,  -39,   26,    0,
    0, -177,   46,  -15,   47, -117,  -30,   54, -254,  -72,
  -72,    0,  -22,    9,   25,   63, -159,   48,  -15,  -73,
    0,   49,  -15,  -36,  -26,  -36, -265,   51,   52,   70,
  -15,  -73,  -15,  -73, -152,  -15,    0,    0, -176,    0,
  -72,    0,    0,    0,    0,  -15,   -3,   -4,    5,    0,
 -163,    0,    0,   62,   64, -143,  -72, -248,  -73,  -73,
    0,  -73,    0,   83,  -73,    6,   85,   69,   93, -129,
    0,  -15,    0,  -15,   79,  -72, -126,   98, -152,   82,
   84,    0,    0,    0,    0, -152,    0,    0,  -15, -123,
  -15,   86,   27,   28,   31, -134,  106, -132,  -73,  -72,
  -72,  114,  115,    0,    0,    0,    0, -105,    0,    0,
  -15,   99,  -23, -119,  103,    0, -122,  -15,  -72,  -72,
  126,   56,   57, -106,  135,  121,  -84,  -73,  -73,    0,
    0,  -79,    0,  -15,  125, -109,    0,  -72,  146,    0,
    0,  130,   67,   71,  132, -134,  -93,  153,  -72,    0,
  -15,  -68,  -23,  139, -122, -117,   78,    0,  135,  -72,
    0,  -36,  -26,    0,    0,  -72,  -73,  -73,  -69,  -73,
  -73,  -73,  144,  -73,    0,    0,    0,  -59,  165,  168,
  -72,  -69,    0,    0,
};
short gdb_result_rindex[] = {                                    -90,
    0,  -90,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    2,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    1,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -71,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -21,  -20,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    3,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  -19,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    3,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  -67,  -66,
    0,    0,    0,    0,
};
short gdb_result_gindex[] = {                                      0,
  213,    0,    0,   -2,  -25,   15,  -56,   55,    0,  -51,
    8,    0,    0,   40, -107,    0,   43,    0,    0,    0,
  -18,  -13, -166,    0,    0,    0,    0,
};
#define YYTABLESIZE 295
short gdb_result_table[] = {                                       7,
   55,   13,    6,    3,   62,   83,   64,  109,   66,   67,
  188,   69,   70,   71,   72,   86,  114,  111,   61,  104,
  184,   34,   30,   27,   43,   36,   38,  110,  112,  113,
  115,  159,   88,    1,   89,  121,   10,  123,  164,   12,
   13,  140,  141,   11,   56,   93,   15,   14,  221,   16,
   60,  103,   28,   34,   29,   30,  104,   36,   38,   31,
   63,   65,  143,  144,   68,  145,  104,   32,  147,  104,
   33,   29,   26,   42,   34,  130,   35,   36,   37,   38,
   39,   40,   73,   74,   75,   76,   77,   78,  103,   79,
   81,  139,   83,   55,   55,   13,    6,   87,  103,   97,
   96,  103,  176,   29,   26,   42,   99,  100,  101,  107,
  156,  116,  117,  118,  124,   98,  185,  128,   80,  132,
  133,  135,  136,  138,  137,   55,  146,  134,  149,  150,
  148,  200,  201,  102,  177,  178,  151,  108,  152,  155,
  157,  158,  160,  166,  161,  120,  168,  122,  172,  174,
  126,  169,  170,  171,   93,  175,  219,  179,  180,  183,
  131,  181,  186,  187,  222,  227,  228,   82,  141,  192,
  231,  232,  206,  235,  236,  237,  195,  239,  196,  194,
  193,  198,  199,  216,  202,  204,  153,  205,  154,  207,
  209,  210,  212,  211,  226,  214,  215,    6,  218,  220,
  229,    3,  224,  165,  238,  167,  233,  240,  241,   17,
   18,  242,   19,   20,    5,  229,  225,   21,   22,  191,
  213,  190,  243,  223,   23,  182,    0,    0,  244,    0,
    0,    0,  189,   24,    0,    0,    0,    0,    0,    0,
    0,    0,   25,    0,    0,    0,    0,   41,  203,    0,
   42,   26,   43,   27,    0,    0,   55,   13,    6,    0,
   44,   45,   46,    0,   47,  217,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   48,    0,    0,
   49,   50,   51,   52,   53,   54,   55,   56,    0,   57,
    0,    0,   55,   13,    6,
};
short gdb_result_check[] = {                                      94,
    0,    0,    0,   94,   30,  123,   32,   44,   34,   35,
  177,   37,   38,   39,   40,   67,  282,   44,   91,   93,
   44,   93,   44,   44,   44,   93,   93,   84,   85,   86,
  296,  139,  287,  256,  289,   92,   44,   94,  146,  293,
  294,  290,  291,  257,   44,   71,   61,   44,  215,   61,
  123,  125,  123,  125,  267,   61,   93,  125,  125,   61,
  267,  267,  119,  120,  267,  122,   93,   61,  125,   93,
   61,   93,   93,   93,   61,  101,   61,   61,   61,   61,
   61,   61,   61,   61,   61,  125,   61,  265,  125,   44,
   44,  117,  123,   93,   94,   94,   94,   44,  125,   91,
  123,  125,  159,  125,  125,  125,   44,  267,   61,   61,
  136,   61,   61,   44,  267,   91,  173,  294,   64,  123,
  125,  285,   61,  267,   61,  125,   44,  123,   44,   61,
  125,  188,  189,   79,  160,  161,   44,   83,  268,   61,
  267,   44,   61,  267,   61,   91,   61,   93,  283,   44,
   96,  125,  125,  123,  180,  288,  213,   44,   44,   61,
  106,  267,  282,   61,  216,  222,  223,  285,  291,   44,
  227,  228,  198,  230,  231,  232,  283,  234,   44,  123,
  125,   61,  267,  209,  264,   61,  132,  297,  134,   44,
   61,  125,   61,  123,  220,  289,   44,  292,  267,   61,
  226,  292,  125,  149,   61,  151,  276,  267,   44,  262,
  263,   44,  265,  266,    2,  241,  219,  270,  271,  180,
  206,  179,  241,  216,  277,  171,   -1,   -1,  242,   -1,
   -1,   -1,  178,  286,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  295,   -1,   -1,   -1,   -1,  263,  194,   -1,
  266,  304,  268,  306,   -1,   -1,  256,  256,  256,   -1,
  276,  277,  278,   -1,  280,  211,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  293,   -1,   -1,
  296,  297,  298,  299,  300,  301,  302,  303,   -1,  305,
   -1,   -1,  292,  292,  292,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 306
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
case 43:
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
case 55:
{
                       if ( yyvsp[-2] == "has_more" ) {
                           sg_children.has_more = (yyvsp[0] == "\"1\"");

                       } else if (!yyvsp[0].empty()) {
                           sg_attributes[yyvsp[-2]] = yyvsp[0];
                       }
                   }
break;
case 56:
{ sg_attributes[yyvsp[-2]] = yyvsp[0];}
break;
case 58:
{ gdbConsumeList(); }
break;
case 59:
{ gdbConsumeList(); }
break;
case 62:
{
                    sg_attributes["reason"] = yyvsp[0];
                    sg_children.push_back( sg_attributes );
                }
break;
case 63:
{
                    sg_attributes["reason"] = yyvsp[0];
                    sg_children.push_back( sg_attributes );
                }
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
case 77:
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
