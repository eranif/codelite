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
    virtual wxString GetNameOnly() const;
    virtual bool Is(eEntityType type) const;
    virtual wxString Type() const;
    virtual void FromResultSet(wxSQLite3ResultSet& res);
    virtual void PrintStdout(int indent) const;
    enum eFunctionFlags {
        kPublic = (1 << 1),
        kPrivate = (1 << 2),
        kProtected = (1 << 3),
        kFinal = (1 << 4),
        kStatic = (1 << 5),
        kAbstract = (1 << 6),
    };

protected:
    // The local variabels defined in this function of type
    // PHPEntityVariable
    PHPEntityBase::List_t m_locals;

    // The function return value
    PHPEntityBase::Ptr_t m_returnValue;

    // Function flags
    size_t m_flags;

    // the return value as read from the database
    wxString m_strReturnValue;

    // the signature
    wxString m_strSignature;

public:
    PHPEntityFunction();
    virtual ~PHPEntityFunction();
    const wxString& GetReturnValue() const { return m_strReturnValue; }
    void SetStrReturnValue(const wxString& strReturnValue) { this->m_strReturnValue = strReturnValue; }

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

    // Accessors
    size_t GetFlags() const { return m_flags; }
    void SetFlags(size_t flags);
    bool HasFlag(eFunctionFlags flag) const { return m_flags & flag; }
    void SetReturnValue(PHPEntityBase::Ptr_t returnValue) { this->m_returnValue = returnValue; }
};

#endif // PHPENTITYFUNCTION_H
