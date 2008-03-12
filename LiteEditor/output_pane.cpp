#include "output_pane.h"
#include "findresultscontainer.h"
#include "findresultstab.h"
#include <wx/xrc/xmlres.h>
#include "wx/wxFlatNotebook/wxFlatNotebook.h"
#include "manager.h"
#include "regex_processor.h"
#include "build_settings_config.h"
#include "dirsaver.h"
#include "macros.h"
#include "shell_window.h"
#include "buidltab.h"

const wxString OutputPane::FIND_IN_FILES_WIN = wxT("Find Results");
const wxString OutputPane::BUILD_WIN         = wxT("Build");
const wxString OutputPane::OUTPUT_WIN        = wxT("Output");
const wxString OutputPane::OUTPUT_DEBUG       = wxT("Debug");

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
	m_book->DeleteAllPages();
}

void OutputPane::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	long style = wxFNB_NO_X_BUTTON | 
				 wxFNB_NO_NAV_BUTTONS | 
				 wxFNB_DROPDOWN_TABS_LIST | 
				 wxFNB_FF2 | 
				 wxFNB_CUSTOM_DLG | 
				 wxFNB_BACKGROUND_GRADIENT | 
				 wxFNB_TABS_BORDER_SIMPLE |
				 wxFNB_BOTTOM ; 
				 
	m_book = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	m_book->SetCustomizeOptions(wxFNB_CUSTOM_LOCAL_DRAG | wxFNB_CUSTOM_ORIENTATION | wxFNB_CUSTOM_TAB_LOOK);
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL | wxGROW, 1);

	m_images.Add(wxXmlResource::Get()->LoadBitmap(wxT("find_results")));
	m_images.Add(wxXmlResource::Get()->LoadBitmap(wxT("build")));
	m_images.Add(wxXmlResource::Get()->LoadBitmap(wxT("output_win")));
	m_images.Add(wxXmlResource::Get()->LoadBitmap(wxT("debug_window")));
	m_images.Add(wxXmlResource::Get()->LoadBitmap(wxT("debugger_tab")));
	m_book->SetImageList( &m_images );

	// Create the tabs
	m_findResultsTab = new FindResultsContainer(m_book, wxID_ANY);
	m_buildWin = new BuildTab(m_book, wxID_ANY, BUILD_WIN);
	m_outputDebug = new ShellTab(m_book, wxID_ANY, OUTPUT_DEBUG);
	m_outputWind = new ShellTab(m_book, wxID_ANY, OUTPUT_WIN);
	
	//add them to the notebook
	m_book->AddPage(m_findResultsTab, FIND_IN_FILES_WIN, true, 0);
	m_book->AddPage(m_buildWin, BUILD_WIN, false, 1);
	m_book->AddPage(m_outputWind, OUTPUT_WIN, false, 2);
	m_book->AddPage(m_outputDebug, OUTPUT_DEBUG, false, 4);
	
	//place a trace window in the notebook as well
	wxTextCtrl *text = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 | wxTE_MULTILINE | wxTE_READONLY| wxHSCROLL);
	m_logTargetOld = wxLog::SetActiveTarget( new wxLogTextCtrl(text) );
	
	m_book->AddPage(text, wxT("Trace"), false, 3);

	mainSizer->Fit(this);
	mainSizer->Layout();
}

void OutputPane::SelectTab(const wxString &tabName)
{
	int index = CaptionToIndex(tabName);
	if( index == wxNOT_FOUND )
		return;
	
	if(index != m_book->GetSelection()){
		m_book->SetSelection((size_t)index);
	}
}

int OutputPane::CaptionToIndex(const wxString &caption)
{
	int i = 0;
	for(; i<m_book->GetPageCount(); i++)
	{
		if(m_book->GetPageText((size_t)i) == caption)
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
