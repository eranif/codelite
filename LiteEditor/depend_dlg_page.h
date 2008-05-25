//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : depend_dlg_page.h              
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
 ///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __depend_dlg_page__
#define __depend_dlg_page__

#include <wx/wx.h>

#include <wx/checklst.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DependenciesPage
///////////////////////////////////////////////////////////////////////////////
class DependenciesPage : public wxPanel 
{
private:

protected:
	wxStaticText* m_staticText1;
	wxCheckListBox* m_checkListProjectList;
	wxStaticText* m_staticText2;
	wxListBox* m_listBoxBuildOrder;
	wxString m_projectName;

	void Init();
	void OnUpCommand(wxListBox *list);
	void OnDownCommand(wxListBox *list);

	virtual void OnCheckListItemToggled(wxCommandEvent &event);
	virtual void OnMoveUp(wxCommandEvent &event);
	virtual void OnMoveDown(wxCommandEvent &event);

public:
	DependenciesPage( wxWindow* parent, const wxString &projName, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 500,300 ), int style = wxTAB_TRAVERSAL );
	void Save();
	
};

#endif //__depend_dlg_page__
