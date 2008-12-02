//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : editorsettingsmiscbasepanel.h              
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

#ifndef __editorsettingsmiscbasepanel__
#define __editorsettingsmiscbasepanel__

#include <wx/string.h>
#include <wx/choice.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/spinctrl.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorSettingsMiscBasePanel
///////////////////////////////////////////////////////////////////////////////
class EditorSettingsMiscBasePanel : public wxPanel 
{
	private:
	
	protected:
		wxChoice* m_toolbarIconSize;
		wxCheckBox* m_useSingleToolbar;
		wxStaticText* m_staticText1;
		wxChoice* m_fileEncoding;
		wxCheckBox* m_showSplashScreen;
		wxCheckBox* m_singleAppInstance;
		wxCheckBox* m_versionCheckOnStartup;
		wxCheckBox* m_fullFilePath;
		wxStaticText* m_staticText2;
		wxButton* m_clearButton;
		wxStaticText* m_staticText3;
		wxSpinCtrl* m_maxItemsFindReplace;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClearButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnClearUI( wxUpdateUIEvent& event ){ event.Skip(); }
		
	
	public:
		EditorSettingsMiscBasePanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 598,476 ), long style = wxTAB_TRAVERSAL );
		~EditorSettingsMiscBasePanel();
	
};

#endif //__editorsettingsmiscbasepanel__
