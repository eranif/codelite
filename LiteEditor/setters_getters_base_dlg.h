//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : setters_getters_base_dlg.h
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

#ifndef __setters_getters_base_dlg__
#define __setters_getters_base_dlg__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/checklst.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/wxscintilla.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/dialog.h>
#include "cl_editor.h"
#include "checktreectrl.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SettersGettersBaseDlg
///////////////////////////////////////////////////////////////////////////////
class SettersGettersBaseDlg : public wxDialog
{
private:

protected:
	wxStaticText*    m_staticText2;
	wxTextCtrl*      m_textClassName;
	wxStaticText*    m_staticText3;
	wxCheckTreeCtrl* m_checkListMembers;
	wxButton*        m_buttonCheckAll;
	wxButton*        m_buttonUncheckAll;
	wxStaticText*    m_staticText31;
	LEditor*         m_textPreview;
	wxCheckBox*      m_checkStartWithUppercase;
	wxStaticLine*    m_staticline1;
	wxButton*        m_buttonOK;
	wxButton*        m_buttonCancel;

	// Virtual event handlers, overide them in your derived class
	virtual void OnCheckAll( wxCommandEvent& event ) {
		event.Skip();
	}
	virtual void OnUncheckAll( wxCommandEvent& event ) {
		event.Skip();
	}


public:
	SettersGettersBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Generate Setters/Getters for class"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 655,518 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );

};

#endif //__setters_getters_base_dlg__
