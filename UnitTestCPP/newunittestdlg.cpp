//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : newunittestdlg.cpp              
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

#include "newunittestdlg.h"

NewUnitTestDlg::NewUnitTestDlg( wxWindow* parent )
:
NewUnitTestBaseDlg( parent )
{
	m_textCtrlTestName->SetFocus();
}

void NewUnitTestDlg::OnUseFixture( wxCommandEvent& event )
{
	if(event.IsChecked()) {
		m_textCtrlFixtureName->Enable();
		m_staticText32->Enable();
	} else {
		m_textCtrlFixtureName->Disable();
		m_staticText32->Disable();
	}
}
