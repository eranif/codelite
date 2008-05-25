//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : envvar_dlg.h              
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

#ifndef __envvar_dlg__
#define __envvar_dlg__

#include <wx/wx.h>

#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EnvVarDlg
///////////////////////////////////////////////////////////////////////////////
class EnvVarDlg : public wxDialog 
{
private:

protected:
	wxPanel* m_panel1;
	wxStaticText* m_staticText3;
	wxTextCtrl* m_textValue;
	wxStaticText* m_staticText4;
	wxTextCtrl* m_textName;
	wxStaticLine* m_staticline3;
	wxButton* m_buttonOK;
	wxButton* m_buttonCacnel;
public:
	EnvVarDlg( wxWindow* parent, wxString name = wxEmptyString, wxString value = wxEmptyString, int id = wxID_ANY, wxString title = wxT("Environment Variable"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 320,152 ), int style = wxDEFAULT_DIALOG_STYLE);

	wxString GetName() const { return m_textName->GetValue(); }
	wxString GetValue() const { return m_textValue->GetValue(); }
};

#endif //__envvar_dlg__
