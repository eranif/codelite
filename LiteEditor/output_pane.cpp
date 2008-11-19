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
 #include "output_pane.h" 
#include "taskpanel.h"
#include "replaceinfilespanel.h"
#include "findresultscontainer.h"
#include "findresultstab.h"
#include <wx/xrc/xmlres.h>
#include "manager.h"
#include "regex_processor.h"
#include "build_settings_config.h"
#include "dirsaver.h"
#include "macros.h"
#include "shell_window.h"
#include "buidltab.h"
#include "errorstab.h"
#include "errorscontainer.h"
#include "custom_notebook.h"
#include "wx/dcbuffer.h"

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
, m_fifTabToUse(0)
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
				 
	m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_TOP|wxVB_TAB_DECORATION);
	
	int width;
	int height;
	
	//get the font we want to use
	wxFont normalFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont boldFont(normalFont);
	boldFont.SetWeight(wxFONTWEIGHT_BOLD);
	
	wxWindow::GetTextExtent(REPLACE_IN_FILES, &width, &height, NULL, NULL, &boldFont);
	
	//add the image size, the x button and some spacers to the width
	width += 16; //image size
	width += 20; //spacers
	
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL | wxGROW, 1);

	// Create the tabs
	m_findResultsTab = new FindResultsContainer(m_book, wxID_ANY);
	m_replaceResultsTab = new ReplaceInFilesPanel(m_book);
	m_buildWin = new BuildTab(m_book, wxID_ANY, BUILD_WIN);
	//m_errorsWin = new ErrorsTab(m_book, wxID_ANY, ERRORS_WIN);
	m_errorsWin = new ErrorsContainer(m_book, wxID_ANY);
	m_outputDebug = new ShellTab(m_book, wxID_ANY, OUTPUT_DEBUG);
	m_outputWind = new ShellTab(m_book, wxID_ANY, OUTPUT_WIN);
	m_taskPanel = new TaskPanel(m_book);
	
	//place a trace window in the notebook as well
	wxTextCtrl *text = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 | wxTE_MULTILINE | wxTE_READONLY| wxHSCROLL);
	m_logTargetOld = wxLog::SetActiveTarget( new wxLogTextCtrl(text) );
	
	//add them to the notebook
	m_book->AddPage(m_findResultsTab, FIND_IN_FILES_WIN, wxXmlResource::Get()->LoadBitmap(wxT("find_results")), false);
	m_book->AddPage(m_replaceResultsTab, REPLACE_IN_FILES, wxXmlResource::Get()->LoadBitmap(wxT("refresh16")), false);
	m_book->AddPage(m_buildWin, BUILD_WIN, wxXmlResource::Get()->LoadBitmap(wxT("build")), false);
	m_book->AddPage(m_errorsWin, ERRORS_WIN, wxXmlResource::Get()->LoadBitmap(wxT("project_conflict")), false);
	m_book->AddPage(m_outputWind, OUTPUT_WIN, wxXmlResource::Get()->LoadBitmap(wxT("output_win")), false);
	m_book->AddPage(m_outputDebug, OUTPUT_DEBUG, wxXmlResource::Get()->LoadBitmap(wxT("debugger_tab")), false);
	m_book->AddPage(text, TRACE_TAB, wxXmlResource::Get()->LoadBitmap(wxT("debug_window")), false);
	m_book->AddPage(m_taskPanel, TASKS, wxXmlResource::Get()->LoadBitmap(wxT("todo")), false);

	mainSizer->Fit(this);
	mainSizer->Layout();
}

void OutputPane::SelectTab(const wxString &tabName)
{
	size_t index = CaptionToIndex(tabName);
	if( index == Notebook::npos )
		return;
	
	if(index != m_book->GetSelection()){
		m_book->SetSelection(index);
	}
}

int OutputPane::CaptionToIndex(const wxString &caption)
{
	size_t i = 0;
	for(; i<m_book->GetPageCount(); i++)
	{
		if(m_book->GetPageText(i) == caption)
			return i;
	}
	return wxNOT_FOUND;
}

FindResultsTab* OutputPane::GetFindResultsTab()
{
	return (FindResultsTab*)m_findResultsTab->GetNotebook()->GetPage((size_t)m_fifTabToUse);
}

void OutputPane::SetFindResultsTab(int which)
{
	m_fifTabToUse = which;
	m_findResultsTab->SetActiveTab(which);
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

ErrorsTab *OutputPane::GetErrorsTab()
{
	return m_errorsWin->GetErrorsTab();
}
