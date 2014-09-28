#ifndef PHPENTITYCLASSIMPL_H
#define PHPENTITYCLASSIMPL_H

#include "codelite_exports.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase

class WXDLLIMPEXP_CL PHPEntityClass : public PHPEntityBase
{
    wxString m_extends;
    wxArrayString m_implements;

public:
    // Save the class into teh database
    virtual void Store(wxSQLite3Database& db);
    
    virtual wxString ID() const;
    virtual void PrintStdout(int indent) const;

    PHPEntityClass();
    virtual ~PHPEntityClass();
    void SetExtends(const wxString& extends) { this->m_extends = extends; }
    const wxString& GetExtends() const { return m_extends; }
    void SetImplements(const wxArrayString& implements) { this->m_implements = implements; }
    const wxArrayString& GetImplements() const { return m_implements; }
    wxString GetImplementsAsString() const {
        return ::wxJoin(m_implements, ';');
    }
};

#endif // PHPENTITYCLASSIMPL_H
