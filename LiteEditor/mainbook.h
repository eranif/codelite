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

class wxChoice;

class MainBook : public wxEvtHandler 
{
	wxChoice *m_choiceFunc;
	wxChoice *m_choiceScope;
protected:
	void OnFuncListMouseDown(wxMouseEvent &e);
	void OnScopeListMouseDown(wxMouseEvent &e);
	void OnScope(wxCommandEvent &e);
	void OnFunction(wxCommandEvent &e);
	
public:
	MainBook(wxChoice* choiceFunc, wxChoice* choiceScope);
	~MainBook();
	
	void UpdateScope(TagEntryPtr tag);
	void Clear();
};

#endif //MAINBOOK_H
