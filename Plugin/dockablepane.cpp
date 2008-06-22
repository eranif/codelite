#include <wx/xrc/xmlres.h>
#include "custom_notebook.h"
#include <wx/sizer.h>
#include "dockablepane.h"

const wxEventType wxEVT_CMD_DELETE_DOCKPANE = wxNewEventType();

DockablePane::DockablePane(wxWindow* parent, Notebook* book, wxWindow* child, const wxString& title, const wxBitmap& bmp, wxSize size)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
, m_child(child)
, m_book(book)
, m_text(title)
, m_bmp(bmp)
{
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	SetSizer(sz);
	
	m_tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	sz->Add(m_tb, 0, wxEXPAND);
	m_tb->AddTool(XRCID("redock_pane"), wxT("Dock at parent notebook"), wxXmlResource::Get()->LoadBitmap(wxT("tab")), wxT("Dock at parent notebook"));
	m_tb->Realize();
	
	Connect(XRCID("redock_pane"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( DockablePane::DockPaneBackToNotebook ));
	m_child->Reparent(this);
	sz->Add(m_child, 1, wxEXPAND);
	sz->Layout();
}

DockablePane::~DockablePane()
{
}

void DockablePane::DockPaneBackToNotebook(wxCommandEvent& e)
{
	wxUnusedVar(e);
	
	// first detach the child from this pane
	wxSizer *sz = GetSizer();
	sz->Detach(m_child);
	
	// now we can add it to the noteook (it will be automatically be reparented to the notebook)
	m_book->AddPage(m_child, m_text, m_bmp, false);
	
	wxCommandEvent event(wxEVT_CMD_DELETE_DOCKPANE);
	event.SetClientData(this);
	GetParent()->AddPendingEvent(event);
}


