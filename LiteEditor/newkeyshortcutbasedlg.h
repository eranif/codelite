//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : newkeyshortcutbasedlg.h              
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

#ifndef __newkeyshortcutbasedlg__
#define __newkeyshortcutbasedlg__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NewKeyShortcutBaseDlg
///////////////////////////////////////////////////////////////////////////////
class NewKeyShortcutBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panel1;
		wxStaticText* m_staticTextAction;
		wxStaticText* m_staticText;
		wxTextCtrl* m_textCtrl1;
		wxButton* m_buttonClear;
		wxStaticLine* m_staticline2;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnKeyDown( wxKeyEvent& event ){ event.Skip(); }
		virtual void OnButtonClear( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		NewKeyShortcutBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Set New Key Accelerator:"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 516,187 ), long style = wxDEFAULT_DIALOG_STYLE );
		~NewKeyShortcutBaseDlg();
	
};

#endif //__newkeyshortcutbasedlg__
