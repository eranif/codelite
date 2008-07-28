//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : debuggerpane.cpp              
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
 #include "wx/dcbuffer.h"
#include "memoryview.h"
#include "debuggerpane.h"
#include "localvarstree.h"
#include "simpletable.h"
#include "listctrlpanel.h"
#include "wx/xrc/xmlres.h"
#include "manager.h"
#include "breakpointdlg.h"
#include "threadlistpanel.h"

const wxString DebuggerPane::LOCALS = wxT("Locals");
const wxString DebuggerPane::WATCHES = wxT("Watches");
const wxString DebuggerPane::FRAMES = wxT("Stack");
const wxString DebuggerPane::BREAKPOINTS = wxT("Breakpoints");
const wxString DebuggerPane::THREADS = wxT("Threads");
const wxString DebuggerPane::MEMORY = wxT("Memory");

BEGIN_EVENT_TABLE(DebuggerPane, wxPanel)
	EVT_PAINT(DebuggerPane::OnPaint)
	EVT_ERASE_BACKGROUND(DebuggerPane::OnEraseBg)
	EVT_SIZE(DebuggerPane::OnSize)
	EVT_BOOK_PAGE_CHANGED(wxID_ANY, DebuggerPane::OnPageChanged)
END_EVENT_TABLE()

DebuggerPane::DebuggerPane(wxWindow *parent, const wxString &caption)
		: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 300))
		, m_caption(caption)
		, m_initDone(false)
{
	CreateGUIControls();
}

DebuggerPane::~DebuggerPane()
{
}

void DebuggerPane::OnPageChanged(NotebookEvent &event)
{
	if (m_initDone) {
		if (event.GetEventObject() == m_book) {
			ManagerST::Get()->UpdateDebuggerPane();
		} else {
			event.Skip();
		}
	}
}

void DebuggerPane::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_TAB_DECORATION|wxVB_TOP);
	mainSizer->Add(m_book, 1, wxEXPAND|wxALL, 1);

	m_localsTree = new LocalVarsTree(m_book, wxID_ANY);
	m_book->AddPage(m_localsTree, LOCALS, wxXmlResource::Get()->LoadBitmap(wxT("locals_view")), true);

	//add the watches view
	m_watchesTable = new SimpleTable(m_book);
	m_book->AddPage(m_watchesTable, WATCHES, wxXmlResource::Get()->LoadBitmap(wxT("watches")), false);

	m_frameList = new ListCtrlPanel(m_book);
	m_book->AddPage(m_frameList, FRAMES, wxXmlResource::Get()->LoadBitmap(wxT("frames")), false);

	m_breakpoints = new BreakpointDlg(m_book);
	m_book->AddPage(m_breakpoints, BREAKPOINTS, wxXmlResource::Get()->LoadBitmap(wxT("breakpoint")), false);

	m_threads = new ThreadListPanel(m_book);
	m_book->AddPage(m_threads, THREADS, wxXmlResource::Get()->LoadBitmap(wxT("threads")), false);

	m_memory = new MemoryView(m_book);
	m_book->AddPage(m_memory, MEMORY, wxXmlResource::Get()->LoadBitmap(wxT("memory_view")), false);

	m_initDone = true;
}

void DebuggerPane::SelectTab(const wxString &tabName)
{
	for (size_t i=0; i< m_book->GetPageCount(); i++) {
		if (m_book->GetPageText(i) == tabName) {
			m_book->SetSelection(i);
			break;
		}
	}
}

void DebuggerPane::Clear()
{
	GetLocalsTree()->Clear();
	GetWatchesTable()->Clear();
	GetFrameListView()->Clear();
	GetThreadsView()->Clear();
	GetMemoryView()->Clear();
}

void DebuggerPane::OnEraseBg(wxEraseEvent &e)
{
	wxUnusedVar(e);
}

void DebuggerPane::OnPaint(wxPaintEvent &e)
{
	wxUnusedVar(e);
	wxBufferedPaintDC dc(this);
	dc.SetPen( wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW) );
	dc.SetBrush( *wxTRANSPARENT_BRUSH );

	dc.DrawRectangle( wxRect(GetSize()) );
}

void DebuggerPane::OnSize(wxSizeEvent &e)
{
	Refresh();
	e.Skip();
}
