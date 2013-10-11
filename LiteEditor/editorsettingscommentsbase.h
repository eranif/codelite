//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : editorsettingscommentsbase.h
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
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __editorsettingscommentsbase__
#define __editorsettingscommentsbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorSettingsCommentsBase
///////////////////////////////////////////////////////////////////////////////
class EditorSettingsCommentsBase : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_checkBoxSmartAddFiles;
		wxStaticLine* m_staticline1;
		wxCheckBox* m_checkBoxContCComment;
		wxCheckBox* m_checkBoxContinueCppComment;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText2;
		wxCheckBox* m_checkBoxShift;
		wxCheckBox* m_checkBoxCtrl;
		wxCheckBox* m_checkBoxAlt;
	
	public:
		
		EditorSettingsCommentsBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~EditorSettingsCommentsBase();
	
};

#endif //__editorsettingscommentsbase__
