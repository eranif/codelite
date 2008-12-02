//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : editorsettingsbookmarksbasepanel.h              
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
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __editorsettingsbookmarksbasepanel__
#define __editorsettingsbookmarksbasepanel__

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/clrpicker.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorSettingsBookmarksBasePanel
///////////////////////////////////////////////////////////////////////////////
class EditorSettingsBookmarksBasePanel : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_displaySelection;
		wxStaticText* m_staticText3;
		wxChoice* m_bookMarkShape;
		wxStaticText* m_staticText4;
		wxColourPickerCtrl* m_backgroundColor;
		wxStaticText* m_staticText5;
		wxColourPickerCtrl* m_foregroundColor;
		wxStaticText* m_staticText6;
		wxColourPickerCtrl* m_highlightColor;
	
	public:
		EditorSettingsBookmarksBasePanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~EditorSettingsBookmarksBasePanel();
	
};

#endif //__editorsettingsbookmarksbasepanel__
