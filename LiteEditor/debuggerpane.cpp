#include "wx/dcbuffer.h"
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

BEGIN_EVENT_TABLE(DebuggerPane, wxPanel)
	EVT_PAINT(DebuggerPane::OnPaint)
	EVT_ERASE_BACKGROUND(DebuggerPane::OnEraseBg)
	EVT_SIZE(DebuggerPane::OnSize)
END_EVENT_TABLE()

DebuggerPane::DebuggerPane(wxWindow *parent, const wxString &caption)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 300))
, m_caption(caption)
{
	CreateGUIControls();
	m_book->Connect(m_book->GetId(), wxEVT_COMMAND_BOOK_PAGE_CHANGED, NotebookEventHandler(DebuggerPane::OnPageChanged), NULL, this);
}

DebuggerPane::~DebuggerPane()
{
}

void DebuggerPane::OnPageChanged(NotebookEvent &event)
{
	wxUnusedVar(event);
	ManagerST::Get()->UpdateDebuggerPane();
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
}

void DebuggerPane::SelectTab(const wxString &tabName)
{
	for(size_t i=0; i< m_book->GetPageCount(); i++){
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
