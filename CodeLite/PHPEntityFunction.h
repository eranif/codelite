#ifndef PHPENTITYFUNCTION_H
#define PHPENTITYFUNCTION_H

#include "codelite_exports.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase
#include <vector>

class WXDLLIMPEXP_CL PHPEntityFunction : public PHPEntityBase
{
public:
    virtual wxString FormatPhpDoc() const;
    virtual wxString GetDisplayName() const;
    virtual bool Is(eEntityType type) const;
    virtual wxString Type() const;
    virtual void FromResultSet(wxSQLite3ResultSet& res);
    virtual void PrintStdout(int indent) const;

protected:
    // The local variabels defined in this function of type
    // PHPEntityVariable
    PHPEntityBase::List_t m_locals;

    // the return value as read from the database
    wxString m_strReturnValue;

    // the signature
    wxString m_strSignature;

public:
    PHPEntityFunction();
    virtual ~PHPEntityFunction();
    const wxString& GetReturnValue() const { return m_strReturnValue; }
    void SetReturnValue(const wxString& strReturnValue) { this->m_strReturnValue = strReturnValue; }

    wxString GetScope() const;
    /**
     * @brief format function signature
     */
    wxString GetSignature() const;

    /**
     * @brief write this object into the database
     * @param db
     */
    virtual void Store(wxSQLite3Database& db);
};

#endif // PHPENTITYFUNCTION_H
