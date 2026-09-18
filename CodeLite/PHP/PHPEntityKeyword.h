//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPEntityKeyword.h
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

#ifndef PHPENTITYKEYWORD_H
#define PHPENTITYKEYWORD_H

#include "PHPEntityBase.h"

#include <wx/string.h>

class WXDLLIMPEXP_CL PHPEntityKeyword : public PHPEntityBase
{
public:
    PHPEntityKeyword() = default;
    ~PHPEntityKeyword() override = default;
    void FromJSON(const JSONItem& json) override;
    JSONItem ToJSON() const override;

public:
    wxString FormatPhpDoc(const CommentConfigData& data) const override;
    void FromResultSet(wxSQLite3ResultSet& res) override;
    wxString GetDisplayName() const override;
    bool Is(eEntityType type) const override;
    void PrintStdout(int indent) const override;
    void Store(PHPLookupTable* lookup) override;
    wxString Type() const override;
};

#endif // PHPENTITYKEYWORD_H
