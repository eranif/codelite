//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : ext_db_page1.h              
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

#ifndef __ext_db_page1__
#define __ext_db_page1__

#include <wx/wx.h>

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/wizard.h>
#include "dirpicker.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ExtDbPage1
///////////////////////////////////////////////////////////////////////////////
class ExtDbPage1 : public wxWizardPageSimple
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxPanel* m_panel1;
		DirPicker* m_dirPicker;
		wxStaticText* m_staticText3;
		void OnValidate(wxWizardEvent &event);

	public:
		ExtDbPage1(wxWizard* parent);
		wxString GetPath() const { return m_dirPicker->GetPath(); }
        void SetPath(const wxString &dir) { m_dirPicker->SetPath(dir); }

		DECLARE_EVENT_TABLE();
	
};

#endif //__ext_db_page1__
