#include "CxxVariable.h"
#include <algorithm>
#include "CxxScannerTokens.h"

CxxVariable::CxxVariable() {}

CxxVariable::~CxxVariable() {}

wxString CxxVariable::GetTypeAsString() const
{
    wxString s;
    std::for_each(m_type.begin(), m_type.end(), [&](const CxxVariable::LexerToken& tok) {
        s << tok.text;
        switch(tok.type) {
        case T_AUTO:
        case T_BOOL:
        case T_CHAR:
        case T_CHAR16_T:
        case T_CHAR32_T:
        case T_CONST:
        case T_DOUBLE:
        case T_FLOAT:
        case T_INT:
        case T_LONG:
        case T_MUTABLE:
        case T_REGISTER:
        case T_SHORT:
        case T_SIGNED:
        case T_STATIC:
        case T_UNSIGNED:
        case T_VOLATILE:
        case T_VOID:
        case T_WCHAR_T:
        case ',':
        case '>':
            s << " ";
            break;
        }
    });
    return s;
}
