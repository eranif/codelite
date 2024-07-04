//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPEntityNamespace.h
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

#ifndef PHPENTITYNAMESPACE_H
#define PHPENTITYNAMESPACE_H

#include "codelite_exports.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase
#include <wx/string.h>

class WXDLLIMPEXP_CL PHPEntityNamespace : public PHPEntityBase
{
protected:
    void DoEnsureNamespacePathExists(wxSQLite3Database& db, const wxString& path);

public:
    void FromJSON(const JSONItem& json);
    JSONItem ToJSON() const;
    
    wxString GetParentNamespace() const;
    virtual wxString FormatPhpDoc(const CommentConfigData& data) const;
    virtual wxString GetDisplayName() const;
    virtual bool Is(eEntityType type) const;
    virtual wxString Type() const;
    virtual void FromResultSet(wxSQLite3ResultSet& res);
    virtual void Store(PHPLookupTable* lookup);
    virtual void PrintStdout(int indent) const;
    PHPEntityNamespace();
    virtual ~PHPEntityNamespace();
    
    /**
     * @brief build namespace from 2 strings
     */
    static wxString BuildNamespace(const wxString &part1, const wxString &part2);
};

#endif // PHPENTITYNAMESPACE_H
