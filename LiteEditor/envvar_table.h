//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : envvar_table.h              
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

#ifndef __envvar_table__
#define __envvar_table__

#include <wx/wx.h>

#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EnvVarsTableDlg
///////////////////////////////////////////////////////////////////////////////
class EnvVarsTableDlg : public wxDialog 
{
private:
	void InitVars();
	void ConnectEvents();

protected:
	void OnNewVar(wxCommandEvent &event);
	void OnEditVar(wxCommandEvent &event);
	void OnDeleteVar(wxCommandEvent &event);
	void OnItemSelected(wxListEvent &event);
	void OnItemActivated(wxListEvent &event);
	void OnSelectionValid(wxUpdateUIEvent &event);
	
	void DoEditItem(long item);
	void OnButtonOk(wxCommandEvent &e);
	void OnButtonCancel(wxCommandEvent &e);
	
protected:
	wxListCtrl* m_listVarsTable;
	wxStaticLine* m_staticline4;
	wxButton* m_buttonDelete;
	wxButton* m_buttonCancel;
	wxButton* m_buttonNew;
	wxButton* m_editButton;
	wxButton* m_buttonOk;
	long m_selectedItem;

public:
	EnvVarsTableDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Environment Variables"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 552,330 ), int style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	~EnvVarsTableDlg();
};

#endif //__envvar_table__
