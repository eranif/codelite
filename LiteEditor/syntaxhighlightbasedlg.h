//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : syntaxhighlightbasedlg.h              
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

#ifndef __syntaxhighlightbasedlg__
#define __syntaxhighlightbasedlg__

#include <wx/statline.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SyntaxHighlightBaseDlg
///////////////////////////////////////////////////////////////////////////////
class SyntaxHighlightBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		wxButton* m_buttonApply;
		wxButton* m_buttonDefaults;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnButtonOK( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonApply( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRestoreDefaults( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		SyntaxHighlightBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Syntax Highlight:"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 557,373 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SyntaxHighlightBaseDlg();
	
};

#endif //__syntaxhighlightbasedlg__
