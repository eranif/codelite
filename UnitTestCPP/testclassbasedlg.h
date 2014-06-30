//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : testclassbasedlg.h
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

#ifndef __testclassbasedlg__
#define __testclassbasedlg__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/checklst.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class TestClassBaseDlg
///////////////////////////////////////////////////////////////////////////////
class TestClassBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrlClassName;
		wxButton* m_buttonClass;
		wxButton* m_buttonRefresh;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrlFixtureName;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrlFileName;
		wxStaticText* m_staticText5;
		wxChoice* m_choiceProjects;
		wxStaticText* m_staticText6;
		wxCheckListBox* m_checkListMethods;
		wxButton* m_buttonCheckAll;
		wxButton* m_buttonUnCheckAll;
		wxStaticLine* m_staticline2;
		wxButton* m_buttonCreate;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnShowClassListDialog( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRefreshFunctions( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRefreshButtonUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnCheckAll( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnUnCheckAll( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonOk( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		
		TestClassBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Create UnitTests for Class.."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~TestClassBaseDlg();
	
};

#endif //__testclassbasedlg__
