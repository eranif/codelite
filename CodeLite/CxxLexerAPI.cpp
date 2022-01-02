#include "CxxLexerAPI.h"

#include "CxxScannerTokens.h"

#include <unordered_set>

using namespace std;
namespace
{
unordered_set<int> set_number_tokens = {
    T_DEC_NUMBER,
    T_OCTAL_NUMBER,
    T_HEX_NUMBER,
    T_FLOAT_NUMBER,
};

unordered_set<int> set_pp_tokens = {
    T_PP_DEFINE,       T_PP_DEFINED,
    T_PP_IF,           T_PP_IFNDEF,
    T_PP_IFDEF,        T_PP_ELSE,
    T_PP_ELIF,         T_PP_LINE,
    T_PP_PRAGMA,       T_PP_UNDEF,
    T_PP_ERROR,        T_PP_ENDIF,
    T_PP_IDENTIFIER,   T_PP_DEC_NUMBER,
    T_PP_OCTAL_NUMBER, T_PP_HEX_NUMBER,
    T_PP_FLOAT_NUMBER, T_PP_STRING,
    T_PP_AND,          T_PP_OR,
    T_PP_STATE_EXIT,   T_PP_INCLUDE_FILENAME,
    T_PP_INCLUDE,      T_PP_GT,
    T_PP_GTEQ,         T_PP_LT,
    T_PP_LTEQ,
};

unordered_set<int> set_operators_tokens = {
    T_DOT_STAR,   T_DOUBLE_COLONS, T_ARROW,     T_ARROW_STAR, T_PLUS_PLUS, T_MINUS_MINUS, T_LS,          T_LE,
    T_GE,         T_EQUAL,         T_NOT_EQUAL, T_AND_AND,    T_OR_OR,     T_STAR_EQUAL,  T_SLASH_EQUAL, T_DIV_EQUAL,
    T_PLUS_EQUAL, T_MINUS_EQUAL,   T_LS_ASSIGN, T_RS_ASSIGN,  T_AND_EQUAL, T_POW_EQUAL,   T_OR_EQUAL,    T_3_DOTS,
};

unordered_set<int> set_builtin_types = {
    T_BOOL, T_CHAR,  T_CHAR16_T, T_CHAR32_T, T_DOUBLE, T_FLOAT,   T_INT,
    T_LONG, T_SHORT, T_SIGNED,   T_UNSIGNED, T_VOID,   T_WCHAR_T,
};
unordered_set<int> set_keywords_tokens = {
    T_ALIGNAS,     T_ALIGNOF,      T_AND,          T_AND_EQ,
    T_ASM,         T_AUTO,         T_BITAND,       T_BITOR,
    T_BREAK,       T_CASE,         T_CATCH,

    T_CLASS,       T_COMPL,        T_CONST,        T_CONSTEXPR,
    T_CONST_CAST,  T_CONTINUE,     T_DECLTYPE,     T_DEFAULT,
    T_DELETE,      T_DO,           T_DYNAMIC_CAST, T_ELSE,
    T_ENUM,        T_EXPLICIT,     T_EXPORT,       T_EXTERN,
    T_FALSE,       T_FINAL,        T_FOR,          T_FRIEND,
    T_GOTO,        T_IF,           T_INLINE,

    T_MUTABLE,     T_NAMESPACE,    T_NEW,          T_NOEXCEPT,
    T_NOT,         T_NOT_EQ,       T_NULLPTR,      T_OPERATOR,
    T_OR,          T_OR_EQ,        T_OVERRIDE,     T_PRIVATE,
    T_PROTECTED,   T_PUBLIC,       T_REGISTER,     T_REINTERPRET_CAST,
    T_RETURN,      T_SIZEOF,       T_STATIC,       T_STATIC_ASSERT,
    T_STATIC_CAST, T_STRUCT,       T_SWITCH,       T_TEMPLATE,
    T_THIS,        T_THREAD_LOCAL, T_THROW,        T_TRUE,
    T_TRY,         T_TYPEDEF,      T_TYPEID,       T_TYPENAME,
    T_UNION,       T_USING,        T_VIRTUAL,      T_VOLATILE,
    T_WHILE,       T_XOR,          T_XOR_EQ,
};

} // namespace

bool CxxLexerToken::is_builtin_type() const { return set_builtin_types.count(GetType()); }
bool CxxLexerToken::is_keyword() const { return set_keywords_tokens.count(GetType()); }
bool CxxLexerToken::is_pp_keyword() const { return set_pp_tokens.count(GetType()); }
bool CxxLexerToken::is_operator() const { return set_operators_tokens.count(GetType()); }
bool CxxLexerToken::is_number() const { return set_number_tokens.count(GetType()); }
