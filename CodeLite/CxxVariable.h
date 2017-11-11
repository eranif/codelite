#ifndef CXXVARIABLE_H
#define CXXVARIABLE_H

#include "CxxLexerAPI.h"
#include "codelite_exports.h"
#include "smart_ptr.h"
#include <map>
#include <set>
#include <unordered_map>
#include <vector>
#include <wx/string.h>
#include <wxStringHash.h>

class WXDLLIMPEXP_CL CxxVariable
{
public:
    struct LexerToken {
        int type;
        int _depth;
        wxString text;
        wxString comment;

        LexerToken()
            : type(0)
            , _depth(0)
        {
        }

        LexerToken(const CxxLexerToken& token, int depth)
        {
            FromCxxLexerToken(token);
            this->_depth = depth;
        }

        void FromCxxLexerToken(const CxxLexerToken& token)
        {
            this->type = token.type;
            this->comment = token.comment;
            this->text = token.text;
        }
        typedef std::vector<CxxVariable::LexerToken> Vec_t;
    };

    enum eFlags {
        kToString_None = 0,
        // Include the variable name
        kToString_Name = (1 << 0),
        // Include the default value
        kToString_DefaultValue = (1 << 1),
        kToString_Default = kToString_Name,
    };

protected:
    wxString m_name;
    CxxVariable::LexerToken::Vec_t m_type;
    wxString m_defaultValue;
    eCxxStandard m_standard;
    wxString m_pointerOrReference;
    bool m_isAuto;

public:
    typedef SmartPtr<CxxVariable> Ptr_t;
    typedef std::vector<CxxVariable::Ptr_t> Vec_t;
    typedef std::unordered_map<wxString, CxxVariable::Ptr_t> Map_t;

public:
    CxxVariable(eCxxStandard standard);
    CxxVariable()
        : m_standard(eCxxStandard::kCxx11)
    {
    }

    virtual ~CxxVariable();

    void SetName(const wxString& name) { this->m_name = name; }
    void SetType(const CxxVariable::LexerToken::Vec_t& type) { this->m_type = type; }
    const wxString& GetName() const { return m_name; }
    const CxxVariable::LexerToken::Vec_t& GetType() const { return m_type; }
    wxString GetTypeAsString() const;
    wxString GetTypeAsCxxString() const;

    static wxString PackType(const CxxVariable::LexerToken::Vec_t& type, eCxxStandard standard,
                             bool omitClassKeyword = false);

    /**
     * @brief return true if this variable was constructed from a statement like:
     * using MyInt = int;
     */
    bool IsUsing() const { return GetTypeAsString() == "using"; }

    /**
     * @brief is this a valid variable?
     */
    bool IsOk() const { return !m_name.IsEmpty() && !m_type.empty(); }

    /**
     * @brief is this an "auto" variable?
     */
    bool IsAuto() const { return m_isAuto; }

    void SetIsAuto(bool b) { m_isAuto = b; }
    /**
     * @brief return a string representation for this variable
     * @param flags see values in eFlags
     * @param standard format standard
     */
    wxString ToString(size_t flags = CxxVariable::kToString_Default) const;

    void SetDefaultValue(const wxString& defaultValue) { this->m_defaultValue = defaultValue; }
    const wxString& GetDefaultValue() const { return m_defaultValue; }

    void SetPointerOrReference(const wxString& pointerOrReference) { this->m_pointerOrReference = pointerOrReference; }
    const wxString& GetPointerOrReference() const { return m_pointerOrReference; }
};

#endif // CXXVARIABLE_H
