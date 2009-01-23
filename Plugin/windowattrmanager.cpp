//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : windowattrmanager.cpp              
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

#include "editor_config.h"
#include "windowattrmanager.h"
#include <wx/settings.h>

void WindowAttrManager::Load(wxWindow* win, const wxString& name, IConfigTool* conf)
{
	if(conf == NULL) {
		conf = EditorConfigST::Get();
	}
	
	SimpleRectValue val;
	if(conf->ReadObject(name, &val)) {
		int screenX = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
		int screenY = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
		if ((val.GetRect().GetLeft() < screenX) && (val.GetRect().GetTop() < screenY)) {
			win->Move(val.GetRect().GetTopLeft());
			win->SetSize(val.GetRect().GetSize());
		}
	}
	
}

void WindowAttrManager::Save(wxWindow* win, const wxString& name, IConfigTool* conf)
{
	if(conf == NULL) {
		conf = EditorConfigST::Get();
	}
	
	SimpleRectValue val;
	val.SetRect(wxRect(win->GetPosition(), win->GetSize()));
	
	conf->WriteObject(name, &val);
}

