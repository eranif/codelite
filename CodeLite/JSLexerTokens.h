#ifndef JSTokens_H__
#define JSTokens_H__

// Range 300-400 are for C++ keywords and operators
enum {
    kJS_FUNCTION = 300,
    kJS_VAR,
    kJS_DOT,
    kJS_THIS,
    kJS_CATCH,
    kJS_THROW,
    kJS_SWITCH,
    kJS_CASE,
    kJS_FOR,
    kJS_STRING,
    kJS_IDENTIFIER,
    kJS_PROTOTYPE,
    kJS_RETURN,
    kJS_NEW,
};

// Numbers
enum {
    kJS_DEC_NUMBER = 500,
    kJS_OCTAL_NUMBER,
    kJS_HEX_NUMBER,
    kJS_FLOAT_NUMBER,
};

// Operators
enum {
    kJS_PLUS_PLUS  = 600,
    kJS_MINUS_MINUS,
    kJS_LS, // left Shift
    kJS_RS, // Right Shift
    kJS_LE, // less equal
    kJS_GE, // greater than-equal
    kJS_EQUAL,
    kJS_NOT_EQUAL,
    kJS_AND_AND,
    kJS_OR_OR,
    kJS_STAR_EQUAL,
    kJS_SLASH_EQUAL,
    kJS_DIV_EQUAL,
    kJS_PLUS_EQUAL,
    kJS_MINUS_EQUAL,
    kJS_LS_ASSIGN,
    kJS_RS_ASSIGN,
    kJS_AND_EQUAL,
    kJS_POW_EQUAL,
    kJS_OR_EQUAL,
};

// Special tokens
enum { kJS_C_COMMENT = 700, kJS_CXX_COMMENT, kJS_NEWLINE, kJS_WHITESPACE };
#endif
