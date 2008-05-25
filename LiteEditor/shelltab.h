//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : shelltab.h              
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
 #ifndef SHELLTAB_H
#define SHELLTAB_H

#include "wx/wxscintilla.h"
#include "wx/panel.h"

class ShellWindow;

class ShellTab : public wxPanel
{
	wxString m_name;
	ShellWindow *m_window;

protected:
	void CreateGUIControl();
	void OnClearAll(wxCommandEvent &e);
	void OnWordWrap(wxCommandEvent &e);

public:
	ShellTab(wxWindow *parent, wxWindowID id, const wxString &name);
	virtual ~ShellTab();

	const wxString &GetCaption() const {return m_name;}
	void Clear();
	void AppendText(const wxString &text);
	ShellWindow *GetShell() {return m_window;}
};

#endif //SHELLTAB_H



