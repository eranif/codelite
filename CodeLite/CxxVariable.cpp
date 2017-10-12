#include "CxxVariable.h"
#include <algorithm>
#include "CxxScannerTokens.h"

CxxVariable::CxxVariable(eCxxStandard standard)
    : m_standard(standard)
{
}

CxxVariable::~CxxVariable() {}

wxString CxxVariable::GetTypeAsString() const { return PackType(m_type, m_standard); }

wxString CxxVariable::ToString(size_t flags) const
{
    wxString str;
    str << GetTypeAsString();

    if(!GetPointerOrReference().IsEmpty()) {
        str << GetPointerOrReference();
    }

    if(flags & kToString_Name) {
        str << " " << GetName();
    }

    if((flags & kToString_DefaultValue) && !GetDefaultValue().IsEmpty()) {
        str << " = " << GetDefaultValue();
    }
    return str;
}

wxString CxxVariable::PackType(const CxxVariable::LexerToken::Vec_t& type, eCxxStandard standard)
{ // Example:
    // const std::vector<std::pair<int, int> >& v
    wxString s;
    std::for_each(type.begin(), type.end(), [&](const CxxVariable::LexerToken& tok) {
        if(((tok.type == ',') || (tok.type == '>') || tok.type == '(' || tok.type == ')') && s.Last() == ' ') {
            s.RemoveLast();
        }
        s << tok.text;

        if(standard == eCxxStandard::kCxx03 && (tok.type == '>')) {
            if(s.length() > 1 && s.EndsWith(">>")) {
                s.RemoveLast(2);
                s << "> >";
            }
        }

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
    if(s.EndsWith(" ")) {
        s.RemoveLast();
    }
    return s;
}
