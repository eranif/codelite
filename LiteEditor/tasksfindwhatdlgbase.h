//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : tasksfindwhatdlgbase.h
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

#ifndef __tasksfindwhatdlgbase__
#define __tasksfindwhatdlgbase__

#include <wx/intl.h>

#include "listctrl_improved.h"
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class TasksFindWhatDlgBase
///////////////////////////////////////////////////////////////////////////////
class TasksFindWhatDlgBase : public wxDialog 
{
	private:
	
	protected:
		ListCtrlImproved *m_list;
		wxButton* m_buttonNew;
		wxButton* m_buttonEdit;
		wxButton* m_buttonDelete;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnNewTask( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditTask( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditTaskUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnDeleteTask( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteTaskUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnButtonOk( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		TasksFindWhatDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Find What..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~TasksFindWhatDlgBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class NewTaskDialog
///////////////////////////////////////////////////////////////////////////////
class NewTaskDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText2;
		
		wxStaticLine* m_staticline2;
		wxButton* m_button5;
		wxButton* m_button6;
	
	public:
		wxTextCtrl* m_name;
		wxTextCtrl* m_regex;
		
		NewTaskDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("New Task"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~NewTaskDialog();
	
};

#endif //__tasksfindwhatdlgbase__
