#include "CxxVariable.h"
#include <algorithm>

CxxVariable::CxxVariable() {}

CxxVariable::~CxxVariable() {}

wxString CxxVariable::GetTypeAsString() const
{
    wxString s;
    std::for_each(m_type.begin(), m_type.end(), [&](const CxxVariable::LexerToken& tok) {
        if(tok.type == ',') {
            s << ", ";
        } else {
            s << tok.text;
        }
    });
    return s;
}
