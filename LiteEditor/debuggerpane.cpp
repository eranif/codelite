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
#include <wx/aui/framemanager.h>
#include "debuggerasciiviewer.h"
#include "dockablepane.h"
#include "editor_config.h"
#include "detachedpanesinfo.h"
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

const wxString DebuggerPane::LOCALS       = wxT("Locals");
const wxString DebuggerPane::WATCHES      = wxT("Watches");
const wxString DebuggerPane::FRAMES       = wxT("Call Stack");
const wxString DebuggerPane::BREAKPOINTS  = wxT("Breakpoints");
const wxString DebuggerPane::THREADS      = wxT("Threads");
const wxString DebuggerPane::MEMORY       = wxT("Memory");
const wxString DebuggerPane::ASCII_VIEWER = wxT("Ascii Viewer");

#define ADD_DEBUGGER_PAGE(win, name, bmp) \
	if( detachedPanes.Index(name) != wxNOT_FOUND ) {\
		new DockablePane(GetParent(), m_book, win, name, bmp, wxSize(200, 200));\
	} else {\
		m_book->AddPage(win, name, name, bmp, false);\
	}

BEGIN_EVENT_TABLE(DebuggerPane, wxPanel)
	EVT_PAINT(DebuggerPane::OnPaint)
	EVT_ERASE_BACKGROUND(DebuggerPane::OnEraseBg)
	EVT_SIZE(DebuggerPane::OnSize)
	EVT_BOOK_PAGE_CHANGED(wxID_ANY, DebuggerPane::OnPageChanged)
END_EVENT_TABLE()

DebuggerPane::DebuggerPane(wxWindow *parent, const wxString &caption, wxAuiManager *mgr)
		: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 300))
		, m_caption(caption)
		, m_initDone(false)
		, m_mgr(mgr)
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

	long bookStyle = wxVB_TOP;
	EditorConfigST::Get()->GetLongValue(wxT("DebuggerBook"), bookStyle);

	m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, bookStyle);
	mainSizer->Add(m_book, 1, wxEXPAND|wxALL, 1);

	// Calculate the widthest tab (the one with the 'Call Stack' label)
	int xx, yy;
	wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxWindow::GetTextExtent(wxT("Breakpoints"), &xx, &yy, NULL, NULL, &fnt);
	m_book->SetFixedTabWidth(xx + 20 + 16); // Icon + Text

	// load list of detached panes
	wxArrayString detachedPanes;
	DetachedPanesInfo dpi;
	EditorConfigST::Get()->ReadObject(wxT("DetachedPanesList"), &dpi);
	detachedPanes = dpi.GetPanes();

	m_localsTree = new LocalVarsTree(m_book, wxID_ANY);
	ADD_DEBUGGER_PAGE(m_localsTree, LOCALS, wxXmlResource::Get()->LoadBitmap(wxT("locals_view")));

	//add the watches view
	m_watchesTable = new SimpleTable(m_book);
	ADD_DEBUGGER_PAGE(m_watchesTable, WATCHES, wxXmlResource::Get()->LoadBitmap(wxT("watches")));
	
	m_asciiViewer = new DebuggerAsciiViewer(this);
	ADD_DEBUGGER_PAGE(m_asciiViewer, ASCII_VIEWER, wxXmlResource::Get()->LoadBitmap(wxT("watches")));
	
	m_frameList = new ListCtrlPanel(m_book);
	ADD_DEBUGGER_PAGE(m_frameList, FRAMES, wxXmlResource::Get()->LoadBitmap(wxT("frames")));

	m_breakpoints = new BreakpointDlg(m_book);
	ADD_DEBUGGER_PAGE(m_breakpoints, BREAKPOINTS, wxXmlResource::Get()->LoadBitmap(wxT("breakpoint")));

	m_threads = new ThreadListPanel(m_book);
	ADD_DEBUGGER_PAGE(m_threads, THREADS, wxXmlResource::Get()->LoadBitmap(wxT("threads")));

	m_memory = new MemoryView(m_book);
	ADD_DEBUGGER_PAGE(m_memory, MEMORY, wxXmlResource::Get()->LoadBitmap(wxT("memory_view")));

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
