//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : output_pane.cpp
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
#include <wx/xrc/xmlres.h>
#include <wx/dcbuffer.h>

#include "output_pane.h"

#include "custom_notebook.h"
#include "findresultstab.h"
#include "replaceinfilespanel.h"
#include "buidltab.h"
#include "errorstab.h"
#include "shelltab.h"
#include "taskpanel.h"


const wxString OutputPane::FIND_IN_FILES_WIN= wxT("Find Results");
const wxString OutputPane::BUILD_WIN        = wxT("Build");
const wxString OutputPane::ERRORS_WIN       = wxT("Errors");
const wxString OutputPane::OUTPUT_WIN		= wxT("Output");
const wxString OutputPane::OUTPUT_DEBUG     = wxT("Debug");
const wxString OutputPane::REPLACE_IN_FILES = wxT("Replace Results");
const wxString OutputPane::TASKS			= wxT("Tasks");
const wxString OutputPane::TRACE_TAB		= wxT("Trace");


BEGIN_EVENT_TABLE(OutputPane, wxPanel)
    EVT_PAINT(OutputPane::OnPaint)
    EVT_ERASE_BACKGROUND(OutputPane::OnEraseBg)
    EVT_SIZE(OutputPane::OnSize)
END_EVENT_TABLE()

OutputPane::OutputPane(wxWindow *parent, const wxString &caption)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 300))
    , m_caption(caption)
    , m_logTargetOld(NULL)
{
	CreateGUIControls();
}

OutputPane::~OutputPane()
{
	delete wxLog::SetActiveTarget(m_logTargetOld);
}

void OutputPane::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_TOP);
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL | wxGROW, 1);

	m_buildWin = new BuildTab(m_book, wxID_ANY, BUILD_WIN);
	m_book->AddPage(m_buildWin, BUILD_WIN, BUILD_WIN, wxXmlResource::Get()->LoadBitmap(wxT("build")));

	m_errorsWin = new ErrorsTab(m_buildWin, m_book, wxID_ANY, ERRORS_WIN);
	m_book->AddPage(m_errorsWin, ERRORS_WIN, ERRORS_WIN, wxXmlResource::Get()->LoadBitmap(wxT("project_conflict")));

	m_findResultsTab = new FindResultsTab(m_book, wxID_ANY, FIND_IN_FILES_WIN);
	m_book->AddPage(m_findResultsTab, FIND_IN_FILES_WIN, FIND_IN_FILES_WIN, wxXmlResource::Get()->LoadBitmap(wxT("find_results")));

	m_replaceResultsTab = new ReplaceInFilesPanel(m_book, wxID_ANY, REPLACE_IN_FILES);
	m_book->AddPage(m_replaceResultsTab, REPLACE_IN_FILES, REPLACE_IN_FILES, wxXmlResource::Get()->LoadBitmap(wxT("refresh16")));

	m_outputWind = new ShellTab(m_book, wxID_ANY, OUTPUT_WIN);
	m_book->AddPage(m_outputWind, OUTPUT_WIN, OUTPUT_WIN, wxXmlResource::Get()->LoadBitmap(wxT("output_win")));

	m_outputDebug = new DebugTab(m_book, wxID_ANY, OUTPUT_DEBUG);
	m_book->AddPage(m_outputDebug, OUTPUT_DEBUG, OUTPUT_DEBUG, wxXmlResource::Get()->LoadBitmap(wxT("debugger_tab")));

	wxTextCtrl *text = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 | wxTE_MULTILINE | wxTE_READONLY| wxHSCROLL);
	m_book->AddPage(text, TRACE_TAB, TRACE_TAB, wxXmlResource::Get()->LoadBitmap(wxT("debug_window")));
	m_logTargetOld = wxLog::SetActiveTarget( new wxLogTextCtrl(text) );

	m_taskPanel = new TaskPanel(m_book, wxID_ANY, TASKS);
	m_book->AddPage(m_taskPanel, TASKS, TASKS, wxXmlResource::Get()->LoadBitmap(wxT("todo")));

	mainSizer->Layout();
}

void OutputPane::OnEraseBg(wxEraseEvent &e)
{
	wxUnusedVar(e);
}

void OutputPane::OnPaint(wxPaintEvent &e)
{
	wxUnusedVar(e);

	wxBufferedPaintDC dc(this);
	dc.SetPen( wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW) );
	dc.SetBrush( *wxTRANSPARENT_BRUSH );
	dc.DrawRectangle( wxRect(GetSize()) );
}

void OutputPane::OnSize(wxSizeEvent &e)
{
	Refresh();
	e.Skip();
}
