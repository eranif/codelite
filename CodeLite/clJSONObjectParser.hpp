/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     kJS_FUNCTION = 258,
     kJS_VAR = 259,
     kJS_DOT = 260,
     kJS_THIS = 261,
     kJS_CATCH = 262,
     kJS_THROW = 263,
     kJS_SWITCH = 264,
     kJS_CASE = 265,
     kJS_FOR = 266,
     kJS_STRING = 267,
     kJS_IDENTIFIER = 268,
     kJS_PROTOTYPE = 269,
     kJS_RETURN = 270,
     kJS_NEW = 271,
     kJS_TRUE = 272,
     kJS_FALSE = 273,
     kJS_NULL = 274,
     kJS_DEC_NUMBER = 275,
     kJS_OCTAL_NUMBER = 276,
     kJS_HEX_NUMBER = 277,
     kJS_FLOAT_NUMBER = 278,
     kJS_PLUS_PLUS = 279,
     kJS_MINUS_MINUS = 280,
     kJS_LS = 281,
     kJS_RS = 282,
     kJS_LE = 283,
     kJS_GE = 284,
     kJS_EQUAL = 285,
     kJS_NOT_EQUAL = 286,
     kJS_AND_AND = 287,
     kJS_OR_OR = 288,
     kJS_STAR_EQUAL = 289,
     kJS_SLASH_EQUAL = 290,
     kJS_DIV_EQUAL = 291,
     kJS_PLUS_EQUAL = 292,
     kJS_MINUS_EQUAL = 293,
     kJS_RS_ASSIGN = 294,
     kJS_AND_EQUAL = 295,
     kJS_POW_EQUAL = 296,
     kJS_OR_EQUAL = 297,
     kJS_VOID = 298,
     kJS_TYPEOF = 299,
     kJS_DELETE = 300,
     kJS_INSTANCEOF = 301,
     kJS_EQUAL3 = 302,
     kJS_LS_ASSIGN = 303
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE json_lval;


