//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : newunittestbasedlg.h              
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
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __newunittestbasedlg__
#define __newunittestbasedlg__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NewUnitTestBaseDlg
///////////////////////////////////////////////////////////////////////////////
class NewUnitTestBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText30;
		wxTextCtrl* m_textCtrlTestName;
		wxCheckBox* m_checkBoxUseFixture;
		wxPanel* m_filler;
		wxStaticText* m_staticText32;
		wxTextCtrl* m_textCtrlFixtureName;
		wxStaticLine* m_staticline11;
		wxButton* m_button24;
		wxButton* m_button23;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnUseFixture( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		NewUnitTestBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("New Unit Test"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 656,200 ), long style = wxDEFAULT_DIALOG_STYLE );
		~NewUnitTestBaseDlg();
	
};

#endif //__newunittestbasedlg__
