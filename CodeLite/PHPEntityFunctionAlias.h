#ifndef PHPENTITYFUNCTIONALIAS_H
#define PHPENTITYFUNCTIONALIAS_H

#include "codelite_exports.h"
#include "PHPEntityFunction.h"

class WXDLLIMPEXP_CL PHPEntityFunctionAlias : public PHPEntityFunction
{
    wxString m_realname;

public:
    PHPEntityFunctionAlias();
    virtual ~PHPEntityFunctionAlias();

public:
    virtual bool Is(eEntityType type) const;
    virtual void Store(wxSQLite3Database& db);
    virtual void FromResultSet(wxSQLite3ResultSet& res);

    void SetRealname(const wxString& realname) { this->m_realname = realname; }
    const wxString& GetRealname() const { return m_realname; }
};

#endif // PHPENTITYFUNCTIONALIAS_H
