#include "foldtoolbargroup.h"
#include "wx/textctrl.h"
#include "notebook_frame.h"
#include "custom_notebook.h"
#include <wx/toolbar.h>
#include <wx/statusbr.h>
#include <wx/sizer.h>
#include <wx/menu.h> //wxMenuBar
#include "foldtoolbar.h"
#include "sample_toolbar.h"

BEGIN_EVENT_TABLE(NotebookFrame, wxFrame)
	EVT_CLOSE(NotebookFrame::OnClose)
	EVT_MENU(wxID_EXIT, NotebookFrame::OnQuit)
	EVT_MENU(wxID_DELETE, NotebookFrame::OnDeletePage)
	EVT_MENU(wxID_NEW, NotebookFrame::OnNewPage)
END_EVENT_TABLE()

NotebookFrame::NotebookFrame(wxWindow* parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style)
		: wxFrame(parent, id, title, pos, size, style)
{
	Initialize();
}

NotebookFrame::~NotebookFrame()
{
	m_mgr.UnInit();
}

void NotebookFrame::Initialize()
{
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	SetSizer(sz);
	CreateFoldToolbar();
	
	wxPanel *mainPanel = new wxPanel(this, wxID_ANY);
	sz->Add(mainPanel, 1, wxEXPAND);
	m_mgr.SetManagedWindow( mainPanel );
	
	wxBitmap bmp, bmp1;
	bmp.LoadFile(wxT("../folder.png"), wxBITMAP_TYPE_PNG);
	bmp1.LoadFile(wxT("../bookmark.png"), wxBITMAP_TYPE_PNG);
	
	//Create a status bar 
	wxStatusBar* statusBar = new wxStatusBar(this, wxID_ANY);
	statusBar->SetFieldsCount(1);
	SetStatusBar(statusBar);
	GetStatusBar()->SetStatusText(wxT("Ready"));
	
	//Create a menu bar
	CreateMenuBar();
	
	wxMenu *menu = new wxMenu();
	menu->Append(wxID_NEW);
	menu->Append(wxID_DELETE);
	
	Notebook *book1 = new Notebook(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_RIGHT|wxVB_HAS_X|wxVB_MOUSE_MIDDLE_CLOSE_TAB);
	book1->SetRightClickMenu(menu);
	
	book1->AddPage(new wxTextCtrl(book1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 1"), bmp);
	book1->AddPage(new wxTextCtrl(book1, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 2"), bmp);
	book1->AddPage(new wxTextCtrl(book1, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 3"), bmp);
	book1->AddPage(new wxTextCtrl(book1, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 4"), bmp);
	
	Notebook *book2 = new Notebook(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_LEFT|wxVB_HAS_X);
	
	
	book2->AddPage(new wxTextCtrl(book2, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 1"));
	book2->AddPage(new wxTextCtrl(book2, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 2 With Long"));
	book2->AddPage(new wxTextCtrl(book2, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 3 With"));
	book2->AddPage(new wxTextCtrl(book2, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 4"));

	
	m_topbook = new Notebook(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_TOP|wxVB_HAS_X);
	m_topbook->AddPage(new wxTextCtrl(m_topbook, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 1"), bmp1);
	m_topbook->AddPage(new wxTextCtrl(m_topbook, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 2 With Long"), bmp1);
	m_topbook->AddPage(new wxTextCtrl(m_topbook, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 3"), bmp1);
	m_topbook->AddPage(new wxTextCtrl(m_topbook, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 4"), bmp1);
	m_topbook->AddPage(new wxTextCtrl(m_topbook, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 5 With Longer Title"), bmp1);

	Notebook *book4 = new Notebook(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_BOTTOM|wxVB_HAS_X);
	
	book4->AddPage(new wxTextCtrl(book4, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 1"));
	book4->AddPage(new wxTextCtrl(book4, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 2"));
	book4->AddPage(new wxTextCtrl(book4, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 3"));
	book4->AddPage(new wxTextCtrl(book4, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 4"));

	wxAuiPaneInfo paneInfo;
	m_mgr.AddPane(m_topbook, paneInfo.Name(wxT("Center Book")).Caption(wxT("Center Book")).CenterPane());
	
	paneInfo = wxAuiPaneInfo();
	m_mgr.AddPane(book2, paneInfo.Name(wxT("Left Book")).BestSize(150, 150).Caption(wxT("Left Book")).Left().Layer(1).Position(1).CloseButton(false));
	
	paneInfo = wxAuiPaneInfo();
	m_mgr.AddPane(book1, paneInfo.Name(wxT("Right Book")).BestSize(150, 150).Caption(wxT("Right Book")).Right().Layer(1).Position(1).CloseButton(false));
	
	paneInfo = wxAuiPaneInfo();
	m_mgr.AddPane(book4, paneInfo.Name(wxT("Bottom Book")).BestSize(150, 150).Caption(wxT("Bottom Book")).Bottom().Layer(1).Position(1).CloseButton(false));
	
	m_mgr.Update();
	
	sz->Layout();
}

void NotebookFrame::OnClose(wxCloseEvent &e)
{
	e.Skip();
}

void NotebookFrame::OnQuit(wxCommandEvent &e)
{
	wxUnusedVar(e);
	Close();
}

void NotebookFrame::CreateMenuBar()
{
	wxMenuBar *mb = new wxMenuBar();
	//File Menu
	wxMenu *menu = new wxMenu();
	menu->Append(wxID_DELETE, wxT("Delete Tab\tCtrl+D"));
	menu->Append(wxID_NEW, wxT("New Tab\tCtrl+N"));
	menu->AppendSeparator();
	menu->Append(wxID_EXIT);
	
	
	mb->Append(menu, wxT("&File"));
	SetMenuBar(mb);
}

void NotebookFrame::OnDeletePage(wxCommandEvent &e)
{
	if(m_topbook && m_topbook->GetPageCount() > 0) {
		m_topbook->DeletePage(m_topbook->GetSelection());
	}
}

void NotebookFrame::OnNewPage(wxCommandEvent &e)
{
	static int pageCount(4);
	wxBitmap bmp1;
	bmp1.LoadFile(wxT("../bookmark.png"), wxBITMAP_TYPE_PNG);

	wxString caption;
	caption << wxT("Page Number ") << ++pageCount;
	if(m_topbook) {
		m_topbook->AddPage(new wxTextCtrl(m_topbook, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), caption, bmp1, false);
	}
}

void NotebookFrame::OnPanelDClick(wxMouseEvent &e)
{
	Freeze();
	static bool expanded(true);
	if(expanded) {
		barPanel->SetSizeHints(-1, 10);
	}else{
		barPanel->SetSizeHints(-1, 100);
	}
	expanded = !expanded;
	GetSizer()->Layout();
	Thaw();
}
void NotebookFrame::CreateFoldToolbar()
{
	barPanel = new FoldToolBar(this, 0);
	GetSizer()->Add(barPanel, 0, wxEXPAND);
	
	//create sample toolbar group
	FoldToolbarGroup *group1 = new FoldToolbarGroup(barPanel, wxT("Group One"));
	MyToolbar *mybar = new MyToolbar(group1);
	group1->Add(mybar, 0);
	
	barPanel->AddGroup(group1, 0);
	barPanel->AddGroup(new FoldToolbarGroup(barPanel, wxT("Group Two")), 1);
	barPanel->Realize();
	
	barPanel->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(NotebookFrame::OnPanelDClick), NULL, this);
}
