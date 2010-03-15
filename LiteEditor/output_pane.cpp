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
#include <wx/toolbook.h>
#include <wx/imaglist.h>
#include "frame.h"
#include "editor_config.h"
#include <wx/dcbuffer.h>
#include "output_pane.h"
#include "findresultstab.h"
#include "replaceinfilespanel.h"
#include "buidltab.h"
#include "errorstab.h"
#include "shelltab.h"
#include "taskpanel.h"

const wxString OutputPane::FIND_IN_FILES_WIN = wxT("Search");
const wxString OutputPane::BUILD_WIN         = wxT("Build");
const wxString OutputPane::ERRORS_WIN        = wxT("Errors");
const wxString OutputPane::OUTPUT_WIN        = wxT("Output");
const wxString OutputPane::OUTPUT_DEBUG      = wxT("Debug");
const wxString OutputPane::REPLACE_IN_FILES  = wxT("Replace");
const wxString OutputPane::TASKS             = wxT("Tasks");
const wxString OutputPane::TRACE_TAB         = wxT("Trace");


#if wxCHECK_VERSION(2, 9, 0)
#    if defined(__WXMSW__)
#        define USE_TOOLBOOK 0
#        define BOOK_ORIENTATION wxBK_TOP
#    else
#        define USE_TOOLBOOK 0
#        define BOOK_ORIENTATION wxBK_BOTTOM
#    endif
#else  // wx2.8
#    if defined(__WXMSW__)||defined(__WXGTK__)
#        define USE_TOOLBOOK 1
#        define BOOK_ORIENTATION wxBK_RIGHT
#    else
#        define USE_TOOLBOOK 0
#        define BOOK_ORIENTATION wxBK_BOTTOM
#    endif
#endif

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

#if USE_TOOLBOOK
	m_book = new wxToolbook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, BOOK_ORIENTATION);
#else
	m_book = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, BOOK_ORIENTATION);
#endif

#if defined (__WXGTK__) && USE_TOOLBOOK
	// Force the toolbar to use horizontal txt aligned to the bitmap
	long style = ((wxToolbook*)m_book)->GetToolBar()->GetWindowStyleFlag();
	style |= wxTB_HORZ_TEXT;
	((wxToolbook*)m_book)->GetToolBar()->SetWindowStyleFlag(style);
#endif

	// Calculate the widthest tab (the one with the 'Workspcae' label)
	int xx, yy;
	wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxWindow::GetTextExtent(REPLACE_IN_FILES, &xx, &yy, NULL, NULL, &fnt);
	
	wxImageList *imageList = new wxImageList(16, 16, true);
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("build")));
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("error")));
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("find_results")));
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("refresh16")));
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("output_win")));
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("debugger_tab")));
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("debug_window")));
	imageList->Add(wxXmlResource::Get()->LoadBitmap(wxT("todo")));
	m_book->AssignImageList( imageList );
	
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL | wxGROW, 0);

	m_buildWin = new BuildTab(m_book, wxID_ANY, BUILD_WIN);
	m_book->AddPage(m_buildWin, BUILD_WIN, true, 0);
	
	m_errorsWin = new ErrorsTab(m_buildWin, m_book, wxID_ANY, ERRORS_WIN);
	m_book->AddPage(m_errorsWin, ERRORS_WIN, false, 1);

	m_findResultsTab = new FindResultsTab(m_book, wxID_ANY, FIND_IN_FILES_WIN, true);
	m_book->AddPage(m_findResultsTab, FIND_IN_FILES_WIN, false, 2);
	
	m_replaceResultsTab = new ReplaceInFilesPanel(m_book, wxID_ANY, REPLACE_IN_FILES);
	m_book->AddPage(m_replaceResultsTab, REPLACE_IN_FILES, false, 3);
	
	m_outputWind = new ShellTab(m_book, wxID_ANY, OUTPUT_WIN);
	m_book->AddPage(m_outputWind, OUTPUT_WIN, false, 4);
	
	m_outputDebug = new DebugTab(m_book, wxID_ANY, OUTPUT_DEBUG);
	m_book->AddPage(m_outputDebug, OUTPUT_DEBUG, false, 5);
	
	wxTextCtrl *text = new wxTextCtrl(m_book, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 | wxTE_MULTILINE | wxTE_READONLY| wxHSCROLL);
	m_book->AddPage(text, TRACE_TAB, false, 6);
	
	m_logTargetOld = wxLog::SetActiveTarget( new wxLogTextCtrl(text) );

	m_taskPanel = new TaskPanel(m_book, wxID_ANY, TASKS);
	m_book->AddPage(m_taskPanel, TASKS, false, 7);

#if USE_TOOLBOOK
	((wxToolbook*)m_book)->GetToolBar()->Realize();
#endif
	mainSizer->Layout();
}
