#ifndef CXXVARIABLE_H
#define CXXVARIABLE_H

#include "codelite_exports.h"
#include "smart_ptr.h"
#include <list>
#include <wx/string.h>
#include "CxxLexerAPI.h"

class WXDLLIMPEXP_CL CxxVariable
{
    wxString m_name;
    CxxLexerToken::List_t m_type;

public:
    typedef SmartPtr<CxxVariable> Ptr_t;
    typedef std::list<CxxVariable::Ptr_t> List_t;

public:
    CxxVariable();
    virtual ~CxxVariable();

    void SetName(const wxString& name) { this->m_name = name; }
    void SetType(const CxxLexerToken::List_t& type) { this->m_type = type; }
    const wxString& GetName() const { return m_name; }
    const CxxLexerToken::List_t& GetType() const { return m_type; }
};

#endif // CXXVARIABLE_H
