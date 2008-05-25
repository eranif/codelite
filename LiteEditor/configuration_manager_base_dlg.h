//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : configuration_manager_base_dlg.h              
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
// C++ code generated with wxFormBuilder (version Jul 28 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __configuration_manager_base_dlg__
#define __configuration_manager_base_dlg__

#include <wx/wx.h>

#include <wx/choice.h>
#include <wx/statline.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ConfigManagerBaseDlg
///////////////////////////////////////////////////////////////////////////////
class ConfigManagerBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText2;
		wxChoice* m_choiceConfigurations;
		wxStaticLine* m_staticline6;
		wxStaticText* m_staticText20;
		wxScrolledWindow* m_scrolledWindow;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		wxButton* m_buttonApply;
	
	public:
		ConfigManagerBaseDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Configuration Manager"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 726,425 ), int style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	
};

#endif //__configuration_manager_base_dlg__
