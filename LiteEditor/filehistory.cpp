//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : filehistory.cpp
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
#include "filehistory.h"

#include <wx/menu.h>

FileHistory::FileHistory()
{
}

/**
 * \brief
 */
FileHistory::~FileHistory()
{
}

void FileHistory::GetFiles(wxArrayString &files)
{
    for(size_t i=0; i<GetCount(); i++) {
        files.Add(m_fileHistory[i]);
    }
}

void FileHistory::AddFilesToMenuWithoutExt()
{
    if ( m_fileHistory.empty() )
        return;

    for ( wxList::compatibility_iterator node = m_fileMenus.GetFirst();
          node; node = node->GetNext() ) {
        AddFilesToMenuWithoutExt((wxMenu *) node->GetData());
    }
}

// return the string used for the MRU list items in the menu
//
// NB: the index n is 0-based, as usual, but the strings start from 1
wxString GetMRUEntryLabel(int n, const wxString& path)
{
    // we need to quote '&' characters which are used for mnemonics
    wxString pathInMenu(path);
    pathInMenu.Replace(wxT("&"), "&&");

    return wxString::Format("&%d %s", n + 1, pathInMenu);
}

void FileHistory::AddFilesToMenuWithoutExt(wxMenu* menu)
{
    if ( m_fileHistory.empty() )
        return;

    if ( menu->GetMenuItemCount() )
        menu->AppendSeparator();

    for ( size_t i = 0; i < m_fileHistory.GetCount(); i++ ) {
        wxString label = GetMRUEntryLabel(i, m_fileHistory[i]);
        menu->Append(GetBaseId() + i, label);
    }
}
