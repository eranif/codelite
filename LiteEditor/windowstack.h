//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : windowstack.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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
 #ifndef WINDOWSTACK_H
#define WINDOWSTACK_H

#include "wx/panel.h"
#include "map"
#include "wx/sizer.h"

class WindowStack : public wxPanel {
	std::map<wxString, wxWindow*> m_windows;
	wxBoxSizer *m_mainSizer;
	wxWindow *m_selection;

public:
	WindowStack(wxWindow *parent, wxWindowID id = wxID_ANY);
	virtual ~WindowStack();
	
	void Add(wxWindow *win, const wxString &key);
	void Select(const wxString &key);
	void Clear();
	void Delete(const wxString &key);
	wxWindow *Find(const wxString &key);
};

#endif //WINDOWSTACK_H


