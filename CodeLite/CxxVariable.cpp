#include "CxxVariable.h"
#include <algorithm>
#include "CxxScannerTokens.h"

CxxVariable::CxxVariable() {}

CxxVariable::~CxxVariable() {}

wxString CxxVariable::GetTypeAsString(CxxVariable::eStandard standard) const
{
    wxString s;
    std::for_each(m_type.begin(), m_type.end(), [&](const CxxVariable::LexerToken& tok) {
        s << tok.text;
        switch(tok.type) {
        case '<':
            if(standard == kCxx03) {
                s << " ";
            }
            break;
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
            s << " ";
            break;
        }
    });
    return s;
}

wxString CxxVariable::ToString(size_t flags, CxxVariable::eStandard standard) const
{
    wxString str;
    str << GetTypeAsString(standard);
    if(flags & kToString_Name) {
        str << " " << GetName();
    }
    if(flags & kToString_DefaultValue) {
        str << " = " << GetDefaultValue();
    }
    return str;
}
