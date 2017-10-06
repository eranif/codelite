//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPEntityClass.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef PHPENTITYCLASSIMPL_H
#define PHPENTITYCLASSIMPL_H

#include "codelite_exports.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase
#include "PHPDocVar.h"

class WXDLLIMPEXP_CL PHPEntityClass : public PHPEntityBase
{
    wxString m_extends;
    wxArrayString m_implements;
    wxArrayString m_traits;
    PHPDocVar::List_t m_varPhpDocs; // List of @var defined inside the class body

public:
    virtual wxString FormatPhpDoc(const CommentConfigData& data) const;
    virtual wxString GetDisplayName() const;
    virtual bool Is(eEntityType type) const;
    virtual wxString Type() const;
    // Save the class into teh database
    virtual void Store(PHPLookupTable* lookup);
    virtual void FromResultSet(wxSQLite3ResultSet& res);
    virtual void PrintStdout(int indent) const;

    /**
     * @brief return an array of inheritance (extends, implementes and traits)
     */
    wxArrayString GetInheritanceArray() const;

    PHPEntityClass();
    virtual ~PHPEntityClass();
    void SetExtends(const wxString& extends) { this->m_extends = extends; }
    const wxString& GetExtends() const { return m_extends; }
    void SetImplements(const wxArrayString& implements) { this->m_implements = implements; }
    const wxArrayString& GetImplements() const { return m_implements; }
    void SetTraits(const wxArrayString& traits) { this->m_traits = traits; }
    const wxArrayString& GetTraits() const { return m_traits; }
    wxString GetImplementsAsString() const { return ::wxJoin(m_implements, ';'); }
    wxString GetTraitsAsString() const { return ::wxJoin(m_traits, ';'); }

    const PHPDocVar::List_t& GetVarPhpDocs() const { return m_varPhpDocs; }
    PHPDocVar::List_t& GetVarPhpDocs() { return m_varPhpDocs; }
    void AddVarPhpDoc(PHPDocVar::Ptr_t doc) { GetVarPhpDocs().push_back(doc); }

    // Aliases
    void SetIsInterface(bool b) { SetFlag(kClass_Interface, b); }
    bool IsInterface() const { return HasFlag(kClass_Interface); }
    void SetIsTrait(bool b) { SetFlag(kClass_Trait, b); }
    bool IsTrait() const { return HasFlag(kClass_Trait); }
    void SetIsAbstractClass(bool b) { SetFlag(kClass_Abstract, b); }
    bool IsAbstractClass() const { return HasFlag(kClass_Abstract); }
};

#endif // PHPENTITYCLASSIMPL_H
