//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : browse_record.h
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
#ifndef BROWSE_HISTORY_H
#define BROWSE_HISTORY_H

#include <wx/string.h>

class BrowseRecord
{
public:
    wxString filename;
    wxString project;
    int lineno = wxNOT_FOUND;
    int column = wxNOT_FOUND;
    int firstLineInView = wxNOT_FOUND;

public:
    BrowseRecord()
        : filename(wxEmptyString)
        , project(wxEmptyString)
        , lineno(wxNOT_FOUND)
        , column(wxNOT_FOUND)
        , firstLineInView(wxNOT_FOUND)
    {
    }

    ~BrowseRecord()
    {
    }

    bool IsSameAs(const BrowseRecord& other) const
    {
        return filename == other.filename && lineno == other.lineno;
    }
    
    /**
     * @brief having a file name is enough for a jumping record
     */
    bool IsOk() const
    {
        return !filename.empty();
    }
};

#endif // BROWSE_HISTORY_H
