//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SFTPManageBookmarkDlg.h
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

#ifndef SFTPMANAGEBOOKMARKDLG_H
#define SFTPMANAGEBOOKMARKDLG_H
#include "UI.h"

class SFTPManageBookmarkDlg : public SFTPManageBookmarkDlgBase
{
public:
    SFTPManageBookmarkDlg(wxWindow* parent, const wxArrayString& bookmarks);
    virtual ~SFTPManageBookmarkDlg();
    wxArrayString GetBookmarks() const;

protected:
    virtual void OnDelete(wxCommandEvent& event);
    virtual void OnDeleteUI(wxUpdateUIEvent& event);
};
#endif // SFTPMANAGEBOOKMARKDLG_H
