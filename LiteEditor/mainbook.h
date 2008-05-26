//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : mainbook.h              
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
 #ifndef MAINBOOK_H
#define MAINBOOK_H

#include "wx/event.h"
#include "wx/string.h"
#include "entry.h"
#include <wx/panel.h>

class wxChoice;
class Notebook;
class wxBoxSizer;

class MainBook : public wxPanel
{
	wxChoice *m_choiceFunc;
	wxChoice *m_choiceScope;
	Notebook *m_book;
	wxBoxSizer *m_hsz;
	
protected:
	void OnFuncListMouseDown(wxMouseEvent &e);
	void OnScopeListMouseDown(wxMouseEvent &e);
	void OnScope(wxCommandEvent &e);
	void OnFunction(wxCommandEvent &e);
	
public:
	MainBook(wxWindow *parent);
	~MainBook();
	Notebook *GetNotebook() {return m_book;}
	
	void UpdateScope(TagEntryPtr tag);
	void Clear();
	void ShowNavBar(bool s);
	void HideNavBar(){ ShowNavBar(false); }
	bool IsNavBarShown();
	
};

#endif //MAINBOOK_H
