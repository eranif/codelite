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

DebuggerPane::DebuggerPane(wxWindow *parent, const wxString &caption)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 300))
, m_caption(caption)
{
	CreateGUIControls();
	m_book->Connect(m_book->GetId(), wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(DebuggerPane::OnPageChanged), NULL, this);
}

DebuggerPane::~DebuggerPane()
{
	for(size_t i=0; i< m_book->GetPageCount(); i++) {
		m_book->DeletePage(0);
	}
}

void DebuggerPane::OnPageChanged(wxAuiNotebookEvent &event)
{
	wxUnusedVar(event);
	ManagerST::Get()->UpdateDebuggerPane();
}

void DebuggerPane::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);
	
	long style = 	wxAUI_NB_TAB_SPLIT |
					wxAUI_NB_TAB_MOVE |
					wxAUI_NB_WINDOWLIST_BUTTON |
					wxAUI_NB_TOP|wxNO_BORDER;
	m_book = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	mainSizer->Add(m_book, 1, wxEXPAND);
	
	m_localsTree = new LocalVarsTree(m_book, wxID_ANY);
	m_book->AddPage(m_localsTree, LOCALS, true, wxXmlResource::Get()->LoadBitmap(wxT("locals_view")));

	//add the watches view
	m_watchesTable = new SimpleTable(m_book);
	m_book->AddPage(m_watchesTable, WATCHES, false, wxXmlResource::Get()->LoadBitmap(wxT("watches")));

	m_frameList = new ListCtrlPanel(m_book);
	m_book->AddPage(m_frameList, FRAMES, false, wxXmlResource::Get()->LoadBitmap(wxT("frames")));

	m_breakpoints = new BreakpointDlg(m_book);
	m_book->AddPage(m_breakpoints, BREAKPOINTS, false, wxXmlResource::Get()->LoadBitmap(wxT("breakpoint")));
	
	m_threads = new ThreadListPanel(m_book);
	m_book->AddPage(m_threads, THREADS, false, wxXmlResource::Get()->LoadBitmap(wxT("threads")));
}

void DebuggerPane::SelectTab(const wxString &tabName)
{
	for(size_t i=0; i< (size_t)m_book->GetPageCount(); i++){
		if(m_book->GetPageText(i) == tabName){
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
}



