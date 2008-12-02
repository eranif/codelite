//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : editorsettingscommentsdoxygenpanelbase.h              
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
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __editorsettingscommentsdoxygenpanelbase__
#define __editorsettingscommentsdoxygenpanelbase__

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorSettingsCommentsDoxygenPanelBase
///////////////////////////////////////////////////////////////////////////////
class EditorSettingsCommentsDoxygenPanelBase : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_checkBoxUseSlash2Stars;
		wxCheckBox* m_checkBoxUseShtroodel;
		wxStaticLine* m_staticline1;
		wxStaticText* m_staticText3;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrlClassPattern;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textCtrlFunctionPattern;
		wxStaticText* m_staticText1;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnUseAtPrefix( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		EditorSettingsCommentsDoxygenPanelBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~EditorSettingsCommentsDoxygenPanelBase();
	
};

#endif //__editorsettingscommentsdoxygenpanelbase__
