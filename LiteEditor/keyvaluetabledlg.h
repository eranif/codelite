//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : keyvaluetabledlg.h              
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
// C++ code generated with wxFormBuilder (version Jun  6 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __keyvaluetabledlg__
#define __keyvaluetabledlg__

#include <wx/wx.h>

#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/button.h>
#include "tags_database.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class KeyValueTableDlg
///////////////////////////////////////////////////////////////////////////////
class KeyValueTableDlg : public wxDialog 
{
private:
	void InitVars();
	void ConnectEvents();

protected:
	void OnEditVar(wxCommandEvent &event);
	void OnItemSelected(wxListEvent &event);
	void OnItemActivated(wxListEvent &event);

protected:
	wxListCtrl* m_listVarsTable;
	wxStaticLine* m_staticline4;
	wxButton* m_buttonOK;
	wxButton* m_buttonCancel;
	wxString m_selectedVarName;
	wxString m_selectedVarValue;
	TagsDatabase *m_db;

public:
	KeyValueTableDlg( wxWindow* parent, TagsDatabase *db, int id = wxID_ANY, wxString title = wxT("Variables "), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 552,330 ), int style = wxDEFAULT_DIALOG_STYLE );

};

#endif //__keyvaluetabledlg__
