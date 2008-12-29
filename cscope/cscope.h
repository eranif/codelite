//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : cscope.h              
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
 #ifndef __Cscope__
#define __Cscope__

#include "plugin.h"
#include "map"
#include "vector"
#include "cscopeentrydata.h"

class CscopeTab;

class Cscope : public IPlugin
{
	wxEvtHandler *m_topWindow;
	CscopeTab *m_cscopeWin;
public:
	Cscope(IManager *manager);
	~Cscope();
	
	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
	
protected:
	//Helper
	//------------------------------------------
	wxMenu *CreateEditorPopMenu();
	wxString GetCscopeExeName();
	wxString DoCreateListFile();
	void DoCscopeCommand(const wxString &command, const wxString &endMsg);
	
	//Event handlers
	//------------------------------------------
	void OnFindSymbol(wxCommandEvent &e);
	void OnFindGlobalDefinition(wxCommandEvent &e);
	void OnFindFunctionsCalledByThisFuncion(wxCommandEvent &e);
	void OnFindFunctionsCallingThisFunction(wxCommandEvent &e);
	void OnCScopeThreadEnded(wxCommandEvent &e);
	void OnCScopeThreadUpdateStatus(wxCommandEvent &e);
	void OnCscopeUI(wxUpdateUIEvent &e);
};
 
#endif //Cscope

