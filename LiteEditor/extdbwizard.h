//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : extdbwizard.h              
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
 #ifndef EXTDBWIZARD_H
#define EXTDBWIZARD_H
#include <wx/wizard.h>
#include "ext_db_page1.h"
#include "ext_db_page2.h"
#include "ext_db_page3.h"
#include "extdbdata.h"

class wxBusyInfo;

class ExtDbWizard : public wxWizard 
{
	wxWizardPageSimple *m_page1;
	wxWizardPageSimple *m_page2;
	wxWizardPageSimple *m_page3;
	wxBusyInfo *m_busyInfoDlg;
public:
	ExtDbWizard(wxWindow *parent, wxWindowID id);
	virtual ~ExtDbWizard();

	/**
	 * Start the wizard
	 * \param info if the wizard succeeded, info will contain the data collected from user
	 * \return true on successfull run, false otherwise
	 */
	bool Run(ExtDbData &info);
	
	DECLARE_EVENT_TABLE();
	void OnPageChanging(wxWizardEvent &e);
	void OnPageChanged(wxWizardEvent &e);
	
};

#endif //EXTDBWIZARD_H
