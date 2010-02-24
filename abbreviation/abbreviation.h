//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : abbreviation.h              
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

#ifndef __abbreviation__
#define __abbreviation__

#include "plugin.h"

class AbbreviationPlugin : public IPlugin
{
	wxEvtHandler *m_topWindow;
protected:
	void OnSettings(wxCommandEvent &e);
	void OnAbbreviations(wxCommandEvent &e);
	void OnAbbrevSelected(wxCommandEvent &e);
	void InitDefaults();
	
public:
	AbbreviationPlugin(IManager *manager);
	~AbbreviationPlugin();
	
	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual clToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
};

#endif //abbreviation

