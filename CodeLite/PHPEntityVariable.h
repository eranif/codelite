#ifndef PHPENTITYVARIABLE_H
#define PHPENTITYVARIABLE_H

#include "codelite_exports.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase
#include <wx/string.h>

class WXDLLIMPEXP_CL PHPEntityVariable : public PHPEntityBase
{
public:
    virtual wxString Type() const;
    virtual void FromResultSet(wxSQLite3ResultSet& res);
    enum eVariableFlags {
        kPublic = (1 << 1),
        kPrivate = (1 << 2),
        kProtected = (1 << 3),
        kMember = (1 << 4),
        kReference = (1 << 5),
        kConst = (1 << 6),
        kFunctionArg = (1 << 7),
    };

protected:
    wxString m_typeHint;

    // Incase this variable is instantiated with an expression
    // keep it
    wxString m_expressionHint;
    wxString m_defaultValue;
    size_t m_flags;

public:
    /**
     * @brief
     * @return
     */
    wxString GetScope() const;
    virtual void Store(wxSQLite3Database& db);
    virtual void PrintStdout(int indent) const;
    /**
     * @brief format this variable
     */
    wxString ToFuncArgString() const;
    PHPEntityVariable();
    virtual ~PHPEntityVariable();

    void SetExpressionHint(const wxString& expressionHint) { this->m_expressionHint = expressionHint; }
    const wxString& GetExpressionHint() const { return m_expressionHint; }
    void SetVisibility(int visibility);
    size_t GetFlags() const { return m_flags; }
    void SetFlags(size_t flags) { this->m_flags = flags; }
    void SetFlag(eVariableFlags flag, bool b = true) { b ? this->m_flags |= flag : this->m_flags &= ~flag; }
    bool Is(eVariableFlags flag) const { return m_flags & flag; }
    void SetIsReference(bool isReference) { SetFlag(kReference, isReference); }
    bool IsReference() const { return Is(kReference); }
    void SetTypeHint(const wxString& typeHint) { this->m_typeHint = typeHint; }
    const wxString& GetTypeHint() const { return m_typeHint; }
    void SetDefaultValue(const wxString& defaultValue) { this->m_defaultValue = defaultValue; }
    const wxString& GetDefaultValue() const { return m_defaultValue; }
};

#endif // PHPENTITYVARIABLE_H
