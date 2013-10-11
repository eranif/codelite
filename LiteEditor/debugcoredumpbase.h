//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : debugcoredumpbase.h
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

#ifndef __debugcoredumpbase__
#define __debugcoredumpbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DebugCoreDumpDlgBase
///////////////////////////////////////////////////////////////////////////////
class DebugCoreDumpDlgBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText2;
		wxComboBox* m_Core;
		wxButton* m_buttonBrowseCore;
		wxStaticText* m_staticText21;
		wxComboBox* m_ExeFilepath;
		wxButton* m_buttonBrowseExe;
		wxStaticText* m_staticText5;
		wxComboBox* m_WD;
		wxButton* m_buttonBrowseWD;
		wxStaticText* m_staticText3;
		wxChoice* m_choiceDebuggers;
		
		wxStaticLine* m_staticline1;
		wxButton* m_buttonDebug;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnButtonBrowseCore( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonBrowseExe( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonBrowseWD( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonDebug( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDebugBtnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DebugCoreDumpDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Debug a core dump"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~DebugCoreDumpDlgBase();
	
};

#endif //__debugcoredumpbase__
