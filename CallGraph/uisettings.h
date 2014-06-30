//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : uisettings.h
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
// C++ code generated with wxFormBuilder (version May  7 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __UISETTINGS_H__
#define __UISETTINGS_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
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
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class uisettings
///////////////////////////////////////////////////////////////////////////////
class uisettings : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText_path_gprog;
		wxTextCtrl* m_textCtrl_path_gprof;
		wxButton* m_button_select_gprof;
		wxStaticText* m_staticText_path_dot;
		wxTextCtrl* m_textCtrl_path_dot;
		wxButton* m_button_select_dot;
		wxStaticText* m_staticText5;
		wxStaticText* m_staticText6;
		wxSpinCtrl* m_spinCtrl_treshold_node;
		wxStaticText* m_staticText7;
		wxSpinCtrl* m_spinCtrl_treshold_edge;
		wxStaticLine* m_staticline6;
		wxStaticText* m_staticText8;
		wxCheckBox* m_checkBox_Names;
		wxStaticText* m_staticText9;
		wxSpinCtrl* m_spinCtrl_colors_node;
		wxCheckBox* m_checkBox_Parameters;
		wxStaticText* m_staticText10;
		wxSpinCtrl* m_spinCtrl_colors_edge;
		wxCheckBox* m_checkBox_Namespaces;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnButton_click_select_gprof( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButton_click_select_dot( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckName( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckParam( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButton_click_ok( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButton_click_cancel( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		uisettings( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Settings..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~uisettings();
	
};

#endif //__UISETTINGS_H__
