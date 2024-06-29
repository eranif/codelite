#include "CxxVariable.h"

#include "CxxLexerAPI.h"
#include "CxxScannerTokens.h"
#include "CxxTokenizer.h"

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
    if(IsUsing()) {
        // A name of the real data type is parsed as assignment expression
        return m_defaultValue;
    }
    return PackType(m_type, m_standard, true, table);
}

wxString CxxVariable::ToString(size_t flags, const wxStringTable_t& table) const
{
    wxUnusedVar(table);

    wxString str;
    str << GetTypeAsString({});

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

wxString CxxVariable::PackType(const CxxVariable::LexerToken::Vec_t& type, eCxxStandard standard, bool omitClassKeyword,
                               const wxStringTable_t& table)
{
    CxxTokenizer tokenizer;
    CxxLexerToken token;

    // create a string with spaces
    wxString packed;
    for(const CxxVariable::LexerToken& tok : type) {
        packed << tok.text << " ";
    }

    tokenizer.Reset(packed);
    wxString s;
    while(tokenizer.NextToken(token)) {
        if(s.empty() && (token.GetType() == T_CLASS || token.GetType() == T_STRUCT || token.GetType() == T_ENUM) &&
           omitClassKeyword)
            continue;

        if(token.is_keyword() || token.is_builtin_type()) {
            s << token.GetWXString() << " ";
        } else if(token.is_pp_keyword()) {
            continue;
        } else {
            s << token.GetWXString();
        }
    }
    return s.Trim();
}
