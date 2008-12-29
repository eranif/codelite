//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : unittestpp.h              
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

#ifndef __UnitTestPP__
#define __UnitTestPP__

#include "plugin.h"

class wxMenuItem;

class UnitTestPP : public IPlugin
{
	wxEvtHandler *m_topWindow;
	PipedProcess *m_proc;
public:
	UnitTestPP(IManager *manager);
	~UnitTestPP();
	
	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
	
protected:
	void OnNewSimpleTest(wxCommandEvent &e);
	void OnNewClassTest(wxCommandEvent &e);
	void OnRunUnitTests(wxCommandEvent &e);
	void OnRunUnitTestsUI(wxUpdateUIEvent &e);
	void OnProcessTerminated(wxProcessEvent &e);
	
	void DoCreateSimpleTest(const wxString &name, IEditor *editor);
	void DoCreateFixtureTest(const wxString &name, const wxString &fixture, IEditor *editor);
	
private:
	wxMenu *CreateEditorPopMenu();
};

#endif //UnitTestPP

