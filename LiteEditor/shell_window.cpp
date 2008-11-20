//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : shell_window.cpp              
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
 #include "shell_window.h"
#include "macros.h"
#include "wx/combobox.h"
#include "wx/timer.h"
#include <wx/splitter.h>

#ifndef wxScintillaEventHandler
#define wxScintillaEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxScintillaEventFunction, &func)
#endif

DEFINE_EVENT_TYPE(wxEVT_SHELLWIN_LINE_ENTERED)
DEFINE_EVENT_TYPE(wxEVT_SHELLWIN_CTRLC)

ShellWindow::ShellWindow(wxWindow *parent, wxEvtHandler *handler)
: wxPanel(parent)
, m_handler(handler)
, m_cur(wxNOT_FOUND)
, m_outputScrolls(true)
{
	Initialize();
	ConnectEvents();
	//GetSizer()->Fit(this);
	GetSizer()->Layout();
}

void ShellWindow::Initialize()
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	int caretSlop = 1;
	int caretZone = 20;
	int caretStrict = 0;
	int caretEven = 0;
	int caretJumps = 0;
	
	m_outWin = new wxScintilla(this);
	sizer->Add(m_outWin, 1, wxEXPAND, 0);
	
	m_outWin->SetXCaretPolicy(caretStrict | caretSlop | caretEven | caretJumps, caretZone);

	caretSlop = 1;
	caretZone = 1;
	caretStrict = 4;
	caretEven = 8;
	caretJumps = 0;
	m_outWin->SetYCaretPolicy(caretStrict | caretSlop | caretEven | caretJumps, caretZone);
	m_outWin->StyleSetBackground(0, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	m_outWin->StyleSetBackground(wxSCI_STYLE_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	m_outWin->SetMarginWidth(1, 0);
	m_outWin->SetMarginWidth(2, 0);
	
	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
		
	m_outWin->StyleSetFont(0, font);
	m_outWin->SetReadOnly(true);
	m_inWin = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_TAB|wxTE_PROCESS_ENTER);
	m_inWin->SetFont(font);
	sizer->Add(m_inWin, 0, wxEXPAND|wxTOP, 5);
}

ShellWindow::~ShellWindow()
{
}

void ShellWindow::ConnectEvents()
{
	// Connect events
	ConnectCmdTextEntered(m_inWin, ShellWindow::OnEnterPressed);
	ConnectKeyDown(m_inWin, ShellWindow::OnKeyDown);
}

void ShellWindow::AppendLine(const wxString &text)
{
    if (m_outputScrolls) {
        // the next 4 lines make sure that the caret is at last line
        // and is visible. This is done before adding the data
        m_outWin->SetSelectionEnd(m_outWin-> GetLength());
        m_outWin->SetSelectionStart(m_outWin->GetLength());
        m_outWin->SetCurrentPos(m_outWin->GetLength());
        m_outWin->EnsureCaretVisible();
    }
    
	// add the text to the control
	m_outWin->SetReadOnly(false);
	m_outWin->InsertText( m_outWin->GetLength(), text );						
	m_outWin->SetReadOnly(true);

    if (m_outputScrolls) {
        // the next 4 lines make sure that the caret is at last line
        // and is visible. This is done before adding the data
        m_outWin->SetSelectionEnd(m_outWin->GetLength());
        m_outWin->SetSelectionStart(m_outWin->GetLength());
        m_outWin->SetCurrentPos(m_outWin->GetLength());
        m_outWin->EnsureCaretVisible();
    }
}

void ShellWindow::Clear()
{
	m_outWin->SetReadOnly(false);
	m_outWin->ClearAll();
	m_outWin->SetReadOnly(true);
}

void ShellWindow::OnEnterPressed(wxCommandEvent &ev)
{
	wxCommandEvent e(wxEVT_SHELLWIN_LINE_ENTERED, GetId());
	e.SetEventObject(this);
	e.SetString(m_inWin->GetValue());
	m_handler->ProcessEvent(e);

	//add this command to the history
	m_inWin->Clear();
	if(m_history.Index(e.GetString()) == wxNOT_FOUND){
		m_history.Add(e.GetString());
		m_cur = (int)m_history.GetCount()-1;
	}
}

void ShellWindow::OnKeyDown(wxKeyEvent &event)
{
	if((event.GetKeyCode() == wxT('c') || event.GetKeyCode() == wxT('C')) && event.GetModifiers() == wxMOD_CONTROL ){
		//Ctrl+C is down
		wxCommandEvent e(wxEVT_SHELLWIN_CTRLC, GetId());
		e.SetEventObject(this);
		m_handler->ProcessEvent(e);
		return;
	}
	switch(event.GetKeyCode())
	{
	case WXK_UP:
		{
			if(m_cur < (int)m_history.GetCount() && !m_history.IsEmpty()){
				if(m_cur+1 < (int)m_history.GetCount() && !m_history.IsEmpty()) m_cur++;
				wxString itemToDisplay = m_history.Item((size_t)m_cur);
				m_inWin->SetValue(itemToDisplay);
				m_inWin->SetInsertionPoint(m_inWin->GetLastPosition());
			}
			return;
		}
	case WXK_DOWN:
		{
			if(m_cur >= 0 && !m_history.IsEmpty()){
				if(m_cur-1 >= 0 && !m_history.IsEmpty()) m_cur--;
				wxString itemToDisplay = m_history.Item((size_t)m_cur);
				m_inWin->SetValue(itemToDisplay);
				m_inWin->SetInsertionPoint(m_inWin->GetLastPosition());
			}
			return;
		}
	default:
		break;
	}
	event.Skip();
}
