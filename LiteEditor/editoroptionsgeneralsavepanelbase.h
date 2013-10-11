//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : editoroptionsgeneralsavepanelbase.h
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
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __editoroptionsgeneralsavepanelbase__
#define __editoroptionsgeneralsavepanelbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorOptionsGeneralSavePanelBase
///////////////////////////////////////////////////////////////////////////////
class EditorOptionsGeneralSavePanelBase : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_checkBoxTrimLine;
		wxCheckBox* m_checkBoxDontTrimCurrentLine;
		
		
		
		wxCheckBox* m_checkBoxTrimModifiedLines;
		wxCheckBox* m_checkBoxAppendLF;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTrimCaretLineUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		EditorOptionsGeneralSavePanelBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~EditorOptionsGeneralSavePanelBase();
	
};

#endif //__editoroptionsgeneralsavepanelbase__
