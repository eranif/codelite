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
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __envvar_dlg__
#define __envvar_dlg__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EnvVarDlg
///////////////////////////////////////////////////////////////////////////////
class EnvVarDlg : public wxDialog
{
private:

protected:
	wxPanel* m_panel1;
	wxStaticText* m_staticText1;
	wxTextCtrl* m_textName;
	wxStaticText* m_staticText2;
	wxTextCtrl* m_textValue;
	wxStaticLine* m_staticline3;
	wxButton* m_buttonOK;
	wxButton* m_buttonCacnel;

public:
	EnvVarDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Environment Variable"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
	~EnvVarDlg();
	
	wxString GetName() const ;
	wxString GetValue() const ;
	void SetName(const wxString &name);
	void SetValue(const wxString &name);
	void SetStaticText1(const wxString &txt);
	void SetStaticText2(const wxString &txt);
	void DisableName();
	void DisableValue();

};

#endif //__envvar_dlg__
