#ifndef PHPENTITYCLASSIMPL_H
#define PHPENTITYCLASSIMPL_H

#include "codelite_exports.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase

class WXDLLIMPEXP_CL PHPEntityClass : public PHPEntityBase
{
    wxString m_name;
    wxString m_extends;
    wxArrayString m_implements;

public:
    virtual wxString ID() const;
    virtual void PrintStdout(int indent) const;

    PHPEntityClass();
    virtual ~PHPEntityClass();
    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetName() const { return m_name; }
    void SetExtends(const wxString& extends) { this->m_extends = extends; }
    const wxString& GetExtends() const { return m_extends; }
    void SetImplements(const wxArrayString& implements) { this->m_implements = implements; }
    const wxArrayString& GetImplements() const { return m_implements; }
};

#endif // PHPENTITYCLASSIMPL_H
