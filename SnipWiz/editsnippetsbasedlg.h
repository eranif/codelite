//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : editsnippetsbasedlg.h
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

#ifndef __editsnippetsbasedlg__
#define __editsnippetsbasedlg__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/html/htmlwin.h>
#include <wx/notebook.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditSnippetsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class EditSnippetsBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook1;
		wxPanel* m_panel2;
		wxListBox* m_listBox1;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrlMenuEntry;
		
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrlAccelerator;
		wxButton* m_buttonShortcut;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrlSnippet;
		wxButton* m_buttonAdd;
		wxButton* m_buttonChange;
		wxButton* m_buttonRemove;
		wxPanel* m_panel3;
		wxHtmlWindow* m_htmlWinAbout;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonClose;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonKeyShortcut( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAddSnippet( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAddSnippetUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnChangeSnippet( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnChangeSnippetUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnRemoveSnippet( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRemoveSnippetUI( wxUpdateUIEvent& event ){ event.Skip(); }
		
	
	public:
		
		EditSnippetsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Edit Snippets"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE ); 
		~EditSnippetsBaseDlg();
	
};

#endif //__editsnippetsbasedlg__
