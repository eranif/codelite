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
    PHPEntityKeyword();
    virtual ~PHPEntityKeyword();
    void FromJSON(const JSONElement& json);
    JSONElement ToJSON() const;
    
public:
    virtual wxString FormatPhpDoc(const CommentConfigData& data) const;
    virtual void FromResultSet(wxSQLite3ResultSet& res);
    virtual wxString GetDisplayName() const;
    virtual bool Is(eEntityType type) const;
    virtual void PrintStdout(int indent) const;
    virtual void Store(PHPLookupTable* lookup);
    virtual wxString Type() const;
};

#endif // PHPENTITYKEYWORD_H
