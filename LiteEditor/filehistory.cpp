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
#include "wx/menu.h"

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
	for(size_t i=0; i<GetCount(); i++)
	{
		files.Add(m_fileHistory[i]);
	}
}

void FileHistory::AddFilesToMenuWithoutExt()
{
#if wxVERSION_NUMBER >= 2900
    if ( m_fileHistory.empty() )
#else
	if (m_fileHistoryN == 0)
#endif
        return;

    for ( wxList::compatibility_iterator node = m_fileMenus.GetFirst();
											node; node = node->GetNext() ) {
        AddFilesToMenuWithoutExt((wxMenu *) node->GetData());
    }
}

#if wxVERSION_NUMBER >= 2900
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
#endif

void FileHistory::AddFilesToMenuWithoutExt(wxMenu* menu)
{
#if wxVERSION_NUMBER >= 2900
    if ( m_fileHistory.empty() )
#else
	if (m_fileHistoryN == 0)
#endif
        return;

    if ( menu->GetMenuItemCount() )
        menu->AppendSeparator();

#if wxVERSION_NUMBER >= 2900
    for ( size_t i = 0; i < m_fileHistory.GetCount(); i++ ) {
		wxString label = GetMRUEntryLabel(i, m_fileHistory[i]);
#else
	for ( size_t i = 0; i < m_fileHistoryN; i++) {
		wxString label = wxString::Format(wxT("&%d %s"), i+1, m_fileHistory[i]);
#endif
		if (label.find(wxT('.')) != wxString::npos) {
			label = label.BeforeLast(wxT('.'));
		}
        menu->Append(GetBaseId() + i, label);
    }
}

