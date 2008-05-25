//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : pluginwizard_page1.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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

#ifndef __pluginwizard_page1__
#define __pluginwizard_page1__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/wizard.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class PluginWizardPage1
///////////////////////////////////////////////////////////////////////////////
class PluginWizardPage1 : public wxWizardPageSimple {
protected:
	void OnValidate(wxWizardEvent &event);
protected:
	wxStaticText* m_staticText5;
	wxStaticLine* m_staticline2;
	wxStaticText* m_staticText1;
	wxTextCtrl* m_textCtrlPluginName;
	wxStaticText* m_staticText3;
	wxTextCtrl* m_textCtrlDescription;

public:
	PluginWizardPage1( wxWizard* parent);
	
	wxString GetName() const{return m_textCtrlPluginName->GetValue();}
	wxString GetDescription() const{return m_textCtrlDescription->GetValue();}
	
	DECLARE_EVENT_TABLE()
};

#endif //__pluginwizard_page1__

