#include "output_pane.h"
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
#include "custom_notebook.h"
#include "wx/dcbuffer.h"

const wxString OutputPane::FIND_IN_FILES_WIN = wxT("Find Results");
const wxString OutputPane::BUILD_WIN         = wxT("Build");
const wxString OutputPane::OUTPUT_WIN        = wxT("Output");
const wxString OutputPane::OUTPUT_DEBUG       = wxT("Debug");
const wxString OutputPane::REPLACE_IN_FILES   = wxT("Replace Results");

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
	m_outputDebug = new ShellTab(m_book, wxID_ANY, OUTPUT_DEBUG);
	m_outputWind = new ShellTab(m_book, wxID_ANY, OUTPUT_WIN);
	
	//place a trace window in the notebook as well
	wxTextCtrl *text = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 | wxTE_MULTILINE | wxTE_READONLY| wxHSCROLL);
	m_logTargetOld = wxLog::SetActiveTarget( new wxLogTextCtrl(text) );
	
	//add them to the notebook
	m_book->AddPage(m_findResultsTab, FIND_IN_FILES_WIN, wxXmlResource::Get()->LoadBitmap(wxT("find_results")), false);
	m_book->AddPage(m_replaceResultsTab, REPLACE_IN_FILES, wxXmlResource::Get()->LoadBitmap(wxT("refresh16")), false);
	m_book->AddPage(m_buildWin, BUILD_WIN, wxXmlResource::Get()->LoadBitmap(wxT("build")), false);
	m_book->AddPage(m_outputWind, OUTPUT_WIN, wxXmlResource::Get()->LoadBitmap(wxT("output_win")), false);
	m_book->AddPage(m_outputDebug, OUTPUT_DEBUG, wxXmlResource::Get()->LoadBitmap(wxT("debugger_tab")), false);
	m_book->AddPage(text, wxT("Trace"), wxXmlResource::Get()->LoadBitmap(wxT("debug_window")), false);

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
