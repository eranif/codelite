//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : tabgroupbasedlgs.h
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

#ifndef __tabgroupbasedlgs__
#define __tabgroupbasedlgs__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/listbox.h>
#include <wx/checkbox.h>
#include <wx/statline.h>
#include <wx/dialog.h>
#include <wx/checklst.h>
#include <wx/textctrl.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class LoadTabGroupBaseDlg
///////////////////////////////////////////////////////////////////////////////
class LoadTabGroupBaseDlg : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText1;
		wxButton* m_buttonBrowse;
		wxListBox* m_listBox;
		wxCheckBox* m_replaceCheck;
		wxStaticLine* m_staticline3;
		wxButton* m_button6;
		wxButton* m_button5;

		// Virtual event handlers, overide them in your derived class
		virtual void OnBrowse( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnItemActivated( wxCommandEvent& event ){ event.Skip(); }


	public:

		LoadTabGroupBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Load a tab group"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~LoadTabGroupBaseDlg();

};

///////////////////////////////////////////////////////////////////////////////
/// Class SaveTabGroupBaseDlg
///////////////////////////////////////////////////////////////////////////////
class SaveTabGroupBaseDlg : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText1;
		wxCheckListBox* m_ListTabs;
		wxButton* m_buttonCheckAll;
		wxButton* m_buttonClearAll;
		wxStaticLine* m_staticline1;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textName;
		wxStaticLine* m_staticline11;
		wxButton* m_button6;
		wxButton* m_button5;

		// Virtual event handlers, overide them in your derived class
		virtual void OnCheckAll( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCheckAllUpdateUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnClearAll( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnClearAllUpdateUI( wxUpdateUIEvent& event ){ event.Skip(); }


	public:
		
		SaveTabGroupBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Save a tab group"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SaveTabGroupBaseDlg();

};

#endif //__tabgroupbasedlgs__
