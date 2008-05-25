//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : ext_db_page3.h              
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
// C++ code generated with wxFormBuilder (version Jul 28 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ext_db_page3__
#define __ext_db_page3__

#include <wx/wx.h>

#include <wx/panel.h>
#include <wx/wizard.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ExtDbPage3
///////////////////////////////////////////////////////////////////////////////
class ExtDbPage3 : public wxWizardPageSimple 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxPanel* m_panel1;
		wxComboBox* m_comboDbName;
		wxStaticText* m_staticText3;
		wxCheckBox *m_checkSetAsActive;
		void OnValidate(wxWizardEvent &event);

	public:
		ExtDbPage3(wxWizard* parent);
		wxString GetDbName() const {return m_comboDbName->GetValue();}
		bool AttachDb() const {return m_checkSetAsActive->GetValue();}
		DECLARE_EVENT_TABLE();
};

#endif //__ext_db_page3__

