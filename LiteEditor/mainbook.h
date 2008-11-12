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

#include <wx/panel.h>
#include "entry.h"

class NavBar;
class Notebook;
class QuickFindBar;

class MainBook : public wxPanel
{
	NavBar *m_navBar;
	Notebook *m_book;
	QuickFindBar *m_quickFindBar;
	
protected:
	void OnMouseDClick(wxMouseEvent &e);
	
public:
	MainBook(wxWindow *parent);
	~MainBook();
    
	Notebook *GetNotebook() {return m_book;}
	
	void UpdateScope(TagEntryPtr tag);
	void Clear();
    
	void ShowNavBar(bool s = true);
	void HideNavBar(){ ShowNavBar(false); }
	bool IsNavBarShown();
    
	void ShowQuickBar(bool s = true);
	void HideQuickBar(){ ShowQuickBar(false); }
};

#endif //MAINBOOK_H
