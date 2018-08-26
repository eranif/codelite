//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : dockablepanemenumanager.h              
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

#ifndef __dockablepanemenumanager__
#define __dockablepanemenumanager__

#include <wx/menu.h>
#include <wx/aui/framemanager.h>
#include <map>
#include "codelite_exports.h"

class wxAuiManager;

//extern const wxEventType wxEVT_CMD_HIDE_PANE;
//extern const wxEventType wxEVT_CMD_SHOW_PANE;
//
class WXDLLIMPEXP_SDK DockablePaneMenuManager : public wxEvtHandler {
	wxMenuBar *m_mb;
	std::map<int, wxString> m_id2nameMap;
	wxAuiManager *m_aui;
	
	void OnDockpaneMenuItem(wxCommandEvent &e);
	void OnDockpaneMenuItemUI(wxUpdateUIEvent &e);
	
public:
	DockablePaneMenuManager(wxMenuBar *mb, wxAuiManager *aui);
	virtual ~DockablePaneMenuManager();
	
	void AddMenu(const wxString &name);
	void RemoveMenu(const wxString &name);
	
	wxString NameById(int id);
	wxArrayString GetDeatchedPanesList();
	bool IsPaneDetached(const wxString &name);
	
	static void HackShowPane(wxAuiPaneInfo &pane_info, wxAuiManager *pAui);
	static void HackHidePane(bool commit, wxAuiPaneInfo &pane_info, wxAuiManager *pAui);
};

#endif // __dockablepanemenumanager__
