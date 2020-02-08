//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SFTPManageBookmarkDlg.cpp
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

#include "SFTPManageBookmarkDlg.h"
#include "globals.h"
#include "windowattrmanager.h"

SFTPManageBookmarkDlg::SFTPManageBookmarkDlg(wxWindow* parent, const wxArrayString& bookmarks)
    : SFTPManageBookmarkDlgBase(parent)
{
    m_listBoxBookmarks->Append(bookmarks);
    SetName("SFTPManageBookmarkDlg");
    ::clSetSmallDialogBestSizeAndPosition(this);
}

SFTPManageBookmarkDlg::~SFTPManageBookmarkDlg() {}

void SFTPManageBookmarkDlg::OnDelete(wxCommandEvent& event)
{
    int sel = m_listBoxBookmarks->GetSelection();
    if(sel == wxNOT_FOUND)
        return;

    m_listBoxBookmarks->Delete(sel);
}

void SFTPManageBookmarkDlg::OnDeleteUI(wxUpdateUIEvent& event)
{
    event.Enable(m_listBoxBookmarks->GetSelection() != wxNOT_FOUND);
}

wxArrayString SFTPManageBookmarkDlg::GetBookmarks() const
{
    wxArrayString bookmarks;
    for(unsigned int i = 0; i < m_listBoxBookmarks->GetCount(); ++i) {
        bookmarks.Add(m_listBoxBookmarks->GetString(i));
    }
    return bookmarks;
}
