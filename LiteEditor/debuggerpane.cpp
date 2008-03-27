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
	m_book->Connect(m_book->GetId(), wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CHANGED, wxFlatNotebookEventHandler(DebuggerPane::OnPageChanged), NULL, this);
}

DebuggerPane::~DebuggerPane()
{
	m_book->DeleteAllPages();
}

void DebuggerPane::OnPageChanged(wxFlatNotebookEvent &event)
{
	wxUnusedVar(event);
	ManagerST::Get()->UpdateDebuggerPane();
}

void DebuggerPane::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);
	
	m_imageList.Add(wxXmlResource::Get()->LoadBitmap(wxT("frames")));	  //frames
	m_imageList.Add(wxXmlResource::Get()->LoadBitmap(wxT("watches")));	  //watches
	m_imageList.Add(wxXmlResource::Get()->LoadBitmap(wxT("locals_view"))); //locals
	m_imageList.Add(wxXmlResource::Get()->LoadBitmap(wxT("breakpoint"))); //breakpoint
	m_imageList.Add(wxXmlResource::Get()->LoadBitmap(wxT("threads"))); //threads

	long bookStyle = wxFNB_BOTTOM | wxFNB_NO_X_BUTTON | wxFNB_NO_NAV_BUTTONS | wxFNB_DROPDOWN_TABS_LIST | wxFNB_FF2 | wxFNB_BACKGROUND_GRADIENT | wxFNB_CUSTOM_DLG | wxFNB_TABS_BORDER_SIMPLE; 
	m_book = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, bookStyle);
	m_book->SetImageList(&m_imageList);
	m_book->SetCustomizeOptions(wxFNB_CUSTOM_LOCAL_DRAG | wxFNB_CUSTOM_ORIENTATION | wxFNB_CUSTOM_TAB_LOOK);
	
	mainSizer->Add(m_book, 1, wxEXPAND);
	m_localsTree = new LocalVarsTree(m_book, wxID_ANY);
	m_book->AddPage(m_localsTree, LOCALS, true, 2);

	//add the watches view
	m_watchesTable = new SimpleTable(m_book);
	m_book->AddPage(m_watchesTable, WATCHES, false, 1);

	m_frameList = new ListCtrlPanel(m_book);
	m_book->AddPage(m_frameList, FRAMES, false, 0);

	m_breakpoints = new BreakpointDlg(m_book);
	m_book->AddPage(m_breakpoints, BREAKPOINTS, false, 3);
	
	m_threads = new ThreadListPanel(m_book);
	m_book->AddPage(m_threads, THREADS, false, 4);
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



