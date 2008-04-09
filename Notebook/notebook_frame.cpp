#include "wx/textctrl.h"
#include "notebook_frame.h"
#include "custom_notebook.h"
#include <wx/toolbar.h>
#include <wx/statusbr.h>
#include <wx/sizer.h>
#include <wx/menu.h> //wxMenuBar

BEGIN_EVENT_TABLE(NotebookFrame, wxFrame)
	EVT_CLOSE(NotebookFrame::OnClose)
	EVT_MENU(wxID_EXIT, NotebookFrame::OnQuit)
	EVT_MENU(wxID_DELETE, NotebookFrame::OnDeletePage)
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

}

void NotebookFrame::Initialize()
{
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *hsz = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(sz);
	
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
	
	Notebook *book1 = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_RIGHT|wxVB_HAS_X|wxVB_MOUSE_MIDDLE_CLOSE_TAB);
	
	book1->AddPage(new wxTextCtrl(book1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 1"), bmp);
	book1->AddPage(new wxTextCtrl(book1, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 2"), bmp);
	book1->AddPage(new wxTextCtrl(book1, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 3"), bmp);
	book1->AddPage(new wxTextCtrl(book1, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 4"), bmp);
	sz->Layout();
	
	Notebook *book2 = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_LEFT|wxVB_HAS_X);
	
	
	book2->AddPage(new wxTextCtrl(book2, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 1"));
	book2->AddPage(new wxTextCtrl(book2, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 2 With Long"));
	book2->AddPage(new wxTextCtrl(book2, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 3 With"));
	book2->AddPage(new wxTextCtrl(book2, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 4"));
	
	m_topbook = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_TOP|wxVB_HAS_X);
	m_topbook->AddPage(new wxTextCtrl(m_topbook, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 1"), bmp1);
	m_topbook->AddPage(new wxTextCtrl(m_topbook, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 2 With Long"), bmp1);
	m_topbook->AddPage(new wxTextCtrl(m_topbook, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 3"), bmp1);
	m_topbook->AddPage(new wxTextCtrl(m_topbook, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 4"), bmp1);
	m_topbook->AddPage(new wxTextCtrl(m_topbook, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 5 With Longer Title"), bmp1);
	
	Notebook *book4 = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_BOTTOM|wxVB_HAS_X);
	
	book4->AddPage(new wxTextCtrl(book4, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 1"));
	book4->AddPage(new wxTextCtrl(book4, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 2"));
	book4->AddPage(new wxTextCtrl(book4, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 3"));
	book4->AddPage(new wxTextCtrl(book4, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxTAB_TRAVERSAL|wxTE_MULTILINE), wxT("Page 4"));
	
	hsz->Add(m_topbook, 1, wxEXPAND);
	hsz->Add(book2, 1, wxEXPAND);
	hsz->Add(book4, 1, wxEXPAND);
	hsz->Add(book1, 1, wxEXPAND);
	
	sz->Add(hsz, 1, wxEXPAND);
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
	menu->Append(wxID_DELETE);
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
