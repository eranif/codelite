//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPEntityFunctionAlias.h
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
    virtual wxString FormatPhpDoc(const CommentConfigData& data) const;
    virtual wxString GetDisplayName() const;
    virtual void PrintStdout(int indent) const;
    virtual wxString Type() const;
    PHPEntityFunctionAlias();
    virtual ~PHPEntityFunctionAlias();

public:
    virtual bool Is(eEntityType type) const;
    virtual void Store(PHPLookupTable* lookup);
    virtual void FromResultSet(wxSQLite3ResultSet& res);

    void SetScope(const wxString& scope) { this->m_scope = scope; }
    const wxString& GetScope() const { return m_scope; }
    void SetRealname(const wxString& realname) { this->m_realname = realname; }
    const wxString& GetRealname() const { return m_realname; }
    void SetFunc(PHPEntityBase::Ptr_t func) { this->m_func = func; }
    PHPEntityBase::Ptr_t GetFunc() const { return m_func; }
};

#endif // PHPENTITYFUNCTIONALIAS_H
