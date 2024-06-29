//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPScannerTokens.h
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

#ifndef PHPScannerTokens_H
#define PHPScannerTokens_H

// Range 300-400
enum {
    kPHP_T_INCLUDE = 300,
    kPHP_T_INCLUDE_ONCE,
    kPHP_T_EVAL,
    kPHP_T_REQUIRE,
    kPHP_T_REQUIRE_ONCE,
    kPHP_T_LOGICAL_OR,
    kPHP_T_LOGICAL_XOR,
    kPHP_T_LOGICAL_AND,
    kPHP_T_PRINT,
    kPHP_T_YIELD,
    kPHP_T_PLUS_EQUAL,
    kPHP_T_MINUS_EQUAL,
    kPHP_T_MUL_EQUAL,
    kPHP_T_DIV_EQUAL,
    kPHP_T_CONCAT_EQUAL,
    kPHP_T_MOD_EQUAL,
    kPHP_T_AND_EQUAL,
    kPHP_T_OR_EQUAL,
    kPHP_T_XOR_EQUAL,
    kPHP_T_SL_EQUAL,
    kPHP_T_SR_EQUAL,
    kPHP_T_BOOLEAN_OR,
    kPHP_T_BOOLEAN_AND,
    kPHP_T_IS_EQUAL,
    kPHP_T_IS_NOT_EQUAL,
    kPHP_T_IS_IDENTICAL,
    kPHP_T_IS_NOT_IDENTICAL,
    kPHP_T_IS_SMALLER_OR_EQUAL,
    kPHP_T_IS_GREATER_OR_EQUAL,
    kPHP_T_SL,
    kPHP_T_SR,
    kPHP_T_INSTANCEOF,
    kPHP_T_INC,
    kPHP_T_DEC,
    kPHP_T_INT_CAST,
    kPHP_T_DOUBLE_CAST,
    kPHP_T_STRING_CAST,
    kPHP_T_ARRAY_CAST,
    kPHP_T_OBJECT_CAST,
    kPHP_T_BOOL_CAST,
    kPHP_T_UNSET_CAST,
    kPHP_T_NEW,
    kPHP_T_CLONE,
    kPHP_T_EXIT,
    kPHP_T_IF,
    kPHP_T_ELSEIF,
    kPHP_T_ELSE,
    kPHP_T_ENDIF,
    kPHP_T_LNUMBER,
    kPHP_T_DNUMBER,
    kPHP_T_IDENTIFIER,
    kPHP_T_STRING_VARNAME,
    kPHP_T_VARIABLE,
    kPHP_T_THIS,
    kPHP_T_PARENT,
    kPHP_T_SELF,
    kPHP_T_NUM_STRING,
    kPHP_T_INLINE_HTML,
    kPHP_T_CHARACTER,
    kPHP_T_BAD_CHARACTER,
    kPHP_T_ENCAPSED_AND_WHITESPACE,
    kPHP_T_CONSTANT_ENCAPSED_STRING,
    kPHP_T_ECHO,
    kPHP_T_DO,
    kPHP_T_WHILE,
    kPHP_T_ENDWHILE,
    kPHP_T_FOR,
    kPHP_T_ENDFOR,
    kPHP_T_FOREACH,
    kPHP_T_ENDFOREACH,
    kPHP_T_DECLARE,
    kPHP_T_ENDDECLARE,
    kPHP_T_AS,
    kPHP_T_SWITCH,
    kPHP_T_ENDSWITCH,
    kPHP_T_CASE,
    kPHP_T_DEFAULT,
    kPHP_T_BREAK,
    kPHP_T_CONTINUE,
    kPHP_T_GOTO,
    kPHP_T_FUNCTION,
    kPHP_T_CONST,
    kPHP_T_RETURN,
    kPHP_T_TRY,
    kPHP_T_CATCH,
    kPHP_T_THROW,
    kPHP_T_USE,
    kPHP_T_INSTEADOF,
    kPHP_T_GLOBAL,
    kPHP_T_STATIC,
    kPHP_T_ABSTRACT,
    kPHP_T_FINAL,
    kPHP_T_PRIVATE,
    kPHP_T_PROTECTED,
    kPHP_T_PUBLIC,
    kPHP_T_VAR,
    kPHP_T_UNSET,
    kPHP_T_ISSET,
    kPHP_T_EMPTY,
    kPHP_T_HALT_COMPILER,
    kPHP_T_CLASS,
    kPHP_T_TRAIT,
    kPHP_T_ENUM,
    kPHP_T_INTERFACE,
    kPHP_T_EXTENDS,
    kPHP_T_IMPLEMENTS,
    kPHP_T_OBJECT_OPERATOR,
    kPHP_T_SPACE_SHIP_OPERATPR,
    kPHP_T_DOUBLE_ARROW,
    kPHP_T_LIST,
    kPHP_T_ARRAY,
    kPHP_T_CALLABLE,
    kPHP_T_CLASS_C,
    kPHP_T_TRAIT_C,
    kPHP_T_METHOD_C,
    kPHP_T_FUNC_C,
    kPHP_T_LINE,
    kPHP_T_FILE,
    kPHP_T_OPEN_TAG,
    kPHP_T_OPEN_TAG_WITH_ECHO,
    kPHP_T_CLOSE_TAG,
    kPHP_T_WHITESPACE,
    kPHP_T_START_HEREDOC,
    kPHP_T_END_HEREDOC,
    kPHP_T_DOLLAR_OPEN_CURLY_BRACES,
    kPHP_T_CURLY_OPEN,
    kPHP_T_PAAMAYIM_NEKUDOTAYIM,
    kPHP_T_NAMESPACE,
    kPHP_T_NS_C,
    kPHP_T_DIR,
    kPHP_T_DEFINE,
    kPHP_T_NS_SEPARATOR,
};

// Special tokens
enum { kPHP_T_C_COMMENT = 500, kPHP_T_CXX_COMMENT, kPHP_T_NEWLINE };
#endif
