#include "CxxScannerTokens.h"
#include "CxxVariable.h"
#include <algorithm>

CxxVariable::CxxVariable(eCxxStandard standard)
    : m_standard(standard)
    , m_isAuto(false)
{
}

CxxVariable::~CxxVariable() {}

wxString CxxVariable::GetTypeAsString(const wxStringTable_t& table) const
{
    return PackType(m_type, m_standard, false, table);
}
wxString CxxVariable::GetTypeAsCxxString(const wxStringTable_t& table) const
{
    return PackType(m_type, m_standard, true, table);
}

wxString CxxVariable::ToString(size_t flags, const wxStringTable_t& table) const
{
    wxString str;
    str << GetTypeAsString(table);

    if(!GetPointerOrReference().IsEmpty()) { str << GetPointerOrReference(); }

    if(flags & kToString_Name) { str << " " << GetName(); }

    if((flags & kToString_DefaultValue) && !GetDefaultValue().IsEmpty()) { str << " = " << GetDefaultValue(); }
    return str;
}

wxString CxxVariable::PackType(const CxxVariable::LexerToken::Vec_t& type, eCxxStandard standard, bool omitClassKeyword,
                               const wxStringTable_t& table)
{
    // Example:
    // const std::vector<std::pair<int, int> >& v
    // "strcut stat buff" if we pass "omitClassKeyword" as true, the type is set to "stat" only
    wxString s;
    std::for_each(type.begin(), type.end(), [&](const CxxVariable::LexerToken& tok) {
        // "strcut stat buff" if we pass "omitClassKeyword" as true, the type is set to "stat" only
        // we do the same for class, enum and struct
        if(s.empty() && (tok.type == T_CLASS || tok.type == T_STRUCT || tok.type == T_ENUM) && omitClassKeyword) return;

        if((!s.empty() && s.Last() == ' ') &&
           ((tok.type == ',') || (tok.type == '>') || tok.type == '(' || tok.type == ')')) {
            s.RemoveLast();
        }

        // Do we need to revert macros?
        if((tok.GetType() == T_IDENTIFIER) && !table.empty() && table.count(tok.text)) {
            s << table.find(tok.text)->second;
        } else {
            s << tok.text;
        }

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
    if(!s.empty() && s.EndsWith(" ")) { s.RemoveLast(); }
    return s;
}
