#ifndef CXXVARIABLE_H
#define CXXVARIABLE_H

#include "codelite_exports.h"
#include "smart_ptr.h"
#include <list>
#include <set>
#include <map>
#include <wx/string.h>
#include "CxxLexerAPI.h"

class WXDLLIMPEXP_CL CxxVariable
{
public:
    struct LexerToken {
        int type;
        wxString text;
        wxString comment;

        LexerToken()
            : type(0)
        {
        }

        LexerToken(const CxxLexerToken& token) { FromCxxLexerToken(token); }

        void FromCxxLexerToken(const CxxLexerToken& token)
        {
            this->type = token.type;
            this->comment = token.comment;
            this->text = token.text;
        }
        typedef std::list<CxxVariable::LexerToken> List_t;
    };

protected:
    wxString m_name;
    CxxVariable::LexerToken::List_t m_type;

public:
    typedef SmartPtr<CxxVariable> Ptr_t;
    typedef std::list<CxxVariable::Ptr_t> List_t;
    typedef std::map<wxString, CxxVariable::Ptr_t> Map_t;

public:
    CxxVariable();
    virtual ~CxxVariable();

    void SetName(const wxString& name) { this->m_name = name; }
    void SetType(const CxxVariable::LexerToken::List_t& type) { this->m_type = type; }
    const wxString& GetName() const { return m_name; }
    const CxxVariable::LexerToken::List_t& GetType() const { return m_type; }
    wxString GetTypeAsString() const;

    /**
     * @brief return true if this variable was constructed from a statement like:
     * using MyInt = int;
     */
    bool IsUsing() const { return GetTypeAsString() == "using"; }

    /**
     * @brief is this a valid variable?
     */
    bool IsOk() const { return !m_name.IsEmpty() && !m_type.empty(); }

    wxString ToString() const { return (wxString() << "Name: " << GetName() << ", Type: " << GetTypeAsString()); }
};

#endif // CXXVARIABLE_H
