//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : addincludefiledlgbase.h              
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
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __addincludefiledlgbase__
#define __addincludefiledlgbase__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/wxscintilla.h>
#ifdef __VISUALC__
#include <wx/link_additions.h>
#endif //__VISUALC__
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AddIncludeFileDlgBase
///////////////////////////////////////////////////////////////////////////////
class AddIncludeFileDlgBase : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_mainPanel;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrlLineToAdd;
		wxButton* m_buttonClearCachedPaths;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textCtrlFullPath;
		wxStaticText* m_staticText6;
		wxScintilla* m_textCtrlPreview;
		wxButton* m_buttonUp;
		wxButton* m_buttonDown;
		wxStaticLine* m_staticline5;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTextUpdated( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnClearCachedPaths( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonUp( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonDown( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		AddIncludeFileDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Add Include File:"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 626,479 ), long style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__addincludefiledlgbase__
