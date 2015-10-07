#ifndef PHPENTITYFUNCTIONALIAS_H
#define PHPENTITYFUNCTIONALIAS_H

#include "codelite_exports.h"
#include "PHPEntityBase.h"

class WXDLLIMPEXP_CL PHPEntityFunctionAlias : public PHPEntityBase
{
    wxString m_realname;
    wxString m_scope;

    PHPEntityBase::Ptr_t m_func;

public:
    virtual wxString FormatPhpDoc() const;
    virtual wxString GetDisplayName() const;
    virtual void PrintStdout(int indent) const;
    virtual wxString Type() const;
    PHPEntityFunctionAlias();
    virtual ~PHPEntityFunctionAlias();

public:
    virtual bool Is(eEntityType type) const;
    virtual void Store(wxSQLite3Database& db);
    virtual void FromResultSet(wxSQLite3ResultSet& res);

    void SetScope(const wxString& scope) { this->m_scope = scope; }
    const wxString& GetScope() const { return m_scope; }
    void SetRealname(const wxString& realname) { this->m_realname = realname; }
    const wxString& GetRealname() const { return m_realname; }
    void SetFunc(PHPEntityBase::Ptr_t func) { this->m_func = func; }
    PHPEntityBase::Ptr_t GetFunc() const { return m_func; }
};

#endif // PHPENTITYFUNCTIONALIAS_H
