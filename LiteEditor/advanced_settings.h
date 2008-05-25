//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : advanced_settings.h              
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

#ifndef __advanced_settings__
#define __advanced_settings__

#include <wx/wx.h>

#include <wx/button.h>
#include <wx/statline.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include "build_page.h"
#include <wx/toolbook.h>

///////////////////////////////////////////////////////////////////////////
class BuildTabSetting;

///////////////////////////////////////////////////////////////////////////////
/// Class AdvancedDlg
///////////////////////////////////////////////////////////////////////////////
class AdvancedDlg : public wxDialog 
{
	DECLARE_EVENT_TABLE();
	
protected:
	wxNotebook* m_notebook; 
	wxPanel* m_compilersPage;
	wxStaticText* m_staticText1;
	wxButton* m_buttonNewCompiler;
	wxStaticLine* m_staticline2;
	wxNotebook* m_compilersNotebook; 
	wxStaticLine* m_staticline10;
	wxButton* m_buttonOK;
	wxButton* m_buttonCancel;
	BuildPage *m_buildPage;
	BuildTabSetting *m_buildSettings;
	wxMenu *m_rightclickMenu;
	
	void OnButtonNewClicked(wxCommandEvent &);
	void OnButtonOKClicked(wxCommandEvent &);
	void OnDeleteCompiler(wxCommandEvent &);
	void OnMouseRightUp(wxMouseEvent &e);
	void LoadCompilers();
	
public:
	AdvancedDlg( wxWindow* parent, size_t selected_page, int id = wxID_ANY, wxString title = wxT("Build Settings"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize(800, 700), int style = wxDEFAULT_DIALOG_STYLE );
	~AdvancedDlg();
};

#endif //__advanced_settings__
