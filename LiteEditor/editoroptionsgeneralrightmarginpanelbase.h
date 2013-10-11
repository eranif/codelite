//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : editoroptionsgeneralrightmarginpanelbase.h
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
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __EDITOROPTIONSGENERALRIGHTMARGINPANELBASE_H__
#define __EDITOROPTIONSGENERALRIGHTMARGINPANELBASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/radiobut.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/clrpicker.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorOptionsGeneralRightMarginPanelBase
///////////////////////////////////////////////////////////////////////////////
class EditorOptionsGeneralRightMarginPanelBase : public wxPanel 
{
	private:
	
	protected:
		wxRadioButton* m_radioBtnRMDisabled;
		wxRadioButton* m_radioBtnRMLine;
		wxRadioButton* m_radioBtnRMBackground;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_rightMarginColumn;
		wxStaticText* m_staticText2;
		wxColourPickerCtrl* m_rightMarginColour;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRightMarginIndicator( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		EditorOptionsGeneralRightMarginPanelBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 263,143 ), long style = wxTAB_TRAVERSAL ); 
		~EditorOptionsGeneralRightMarginPanelBase();
	
};

#endif //__EDITOROPTIONSGENERALRIGHTMARGINPANELBASE_H__
