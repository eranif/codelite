//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : editoroptionsgeneralcodenavpanel.cpp              
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

#include "editoroptionsgeneralcodenavpanel.h"

EditorOptionsGeneralCodeNavPanel::EditorOptionsGeneralCodeNavPanel( wxWindow* parent )
: EditorOptionsGeneralCodeNavPanelBase( parent )
, TreeBookNode<EditorOptionsGeneralCodeNavPanel>()
{
	long value = 0;
	EditorConfigST::Get()->GetLongValue(wxT("QuickCodeNavigationUsesMouseMiddleButton"), value);
	switch(value){
	case 1:
		m_radioBoxNavigationMethodLeft->SetValue(false);
		m_radioBoxNavigationMethodMiddle->SetValue(true);
		break;
	case 0:
	default:
		m_radioBoxNavigationMethodLeft->SetValue(true);
		m_radioBoxNavigationMethodMiddle->SetValue(false);
		break;
	}
}


void EditorOptionsGeneralCodeNavPanel::Save(OptionsConfigPtr options)
{
	EditorConfigST::Get()->SaveLongValue(wxT("QuickCodeNavigationUsesMouseMiddleButton"), m_radioBoxNavigationMethodMiddle->GetValue() ? 1 : 0);
}
