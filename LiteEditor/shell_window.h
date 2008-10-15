//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : shell_window.h              
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
 #ifndef SHELL_WINDOW_H
#define SHELL_WINDOW_H

#include "wx/wxscintilla.h"
#include "wx/event.h"
#include <wx/arrstr.h>

class wxScintilla;
class wxTextCtrl;
class wxSplitterWindow;

extern const wxEventType wxEVT_SHELLWIN_LINE_ENTERED;
extern const wxEventType wxEVT_SHELLWIN_CTRLC;

class ShellWindow : public wxPanel 
{
	wxEvtHandler *m_handler;
	wxScintilla *m_outWin;
	wxTextCtrl *m_inWin;
	wxArrayString m_history;
	int m_cur;
	bool m_keepFocus;
    bool m_outputScrolls;

protected:
	void ConnectEvents();
	void Initialize();
	
public:
	ShellWindow(wxWindow *parent, wxEvtHandler *handler);
	virtual ~ShellWindow();

	//Setters
	void SetHandler(wxEvtHandler*& handler) {this->m_handler = handler;}
    void SetScrollOnOutput(bool scroll) { this->m_outputScrolls = scroll; }
	
	//Getters
	const wxEvtHandler* GetHandler() const {return m_handler;}
	const wxTextCtrl* GetInWin() const {return m_inWin;}
	const wxScintilla* GetOutWin() const {return m_outWin;}
	bool GetScrollOnOutput() { return m_outputScrolls; }
    
	void AppendLine(const wxString &text);
	void Clear();
	void OnKeyDown(wxKeyEvent &event);
	void OnEnterPressed(wxCommandEvent &e);
};
#endif // SHELL_WINDOW_H
