//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : editcmpfileinfobase.h
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
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __editcmpfileinfobase__
#define __editcmpfileinfobase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditCmpFileInfoBase
///////////////////////////////////////////////////////////////////////////////
class EditCmpFileInfoBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrlFileType;
		wxStaticText* m_staticText2;
		wxChoice* m_choiceFileTypeIs;
		wxStaticText* m_staticText4;
		
		wxTextCtrl* m_textCtrl2;
		wxButton* m_buttonHelp;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFileTypeText( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCompilationLine( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonHelp( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonOk( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonOkUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		
		EditCmpFileInfoBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("File Type Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~EditCmpFileInfoBase();
	
};

#endif //__editcmpfileinfobase__
