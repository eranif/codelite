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

#include "PHPEntityBase.h" // Base class: PHPEntityBase
#include "codelite_exports.h"

#include <wx/string.h>

class WXDLLIMPEXP_CL PHPEntityNamespace : public PHPEntityBase
{
protected:
    void DoEnsureNamespacePathExists(wxSQLite3Database& db, const wxString& path);

public:
    void FromJSON(const JSONItem& json) override;
    JSONItem ToJSON() const override;

    wxString GetParentNamespace() const;
    wxString FormatPhpDoc(const CommentConfigData& data) const override;
    wxString GetDisplayName() const override;
    bool Is(eEntityType type) const override;
    wxString Type() const override;
    void FromResultSet(wxSQLite3ResultSet& res) override;
    void Store(PHPLookupTable* lookup) override;
    void PrintStdout(int indent) const override;
    PHPEntityNamespace() = default;
    ~PHPEntityNamespace() override = default;

    /**
     * @brief build namespace from 2 strings
     */
    static wxString BuildNamespace(const wxString& part1, const wxString& part2);
};

#endif // PHPENTITYNAMESPACE_H
