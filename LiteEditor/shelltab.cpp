#include "shelltab.h"
#include "wx/xrc/xmlres.h"
#include "output_pane.h"
#include "manager.h"
#include "shell_window.h"
#include "frame.h"

ShellTab::ShellTab(wxWindow *parent, wxWindowID id, const wxString &name)
: wxPanel(parent, id)
, m_name(name)
{
	CreateGUIControl();
}

ShellTab::~ShellTab()
{
}

void ShellTab::CreateGUIControl()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(mainSizer);
	//Create the toolbar
	wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_VERTICAL|wxTB_NODIVIDER);

	int id = wxNewId();
	tb->AddTool(id, 
				wxT("Clear All"), 
				wxXmlResource::Get()->LoadBitmap(wxT("document_delete")), 
				wxT("Clear All"));
	Connect( id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ShellTab::OnClearAll ));

	id = wxNewId();
	tb->AddTool(id, 
				wxT("Word Wrap"), 
				wxXmlResource::Get()->LoadBitmap(wxT("word_wrap")), 
				wxT("Word Wrap"),
				wxITEM_CHECK);
				
	Connect( id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ShellTab::OnWordWrap ));
	tb->Realize(); 
	mainSizer->Add(tb, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
	
	m_window = new ShellWindow(this, ManagerST::Get());
	ManagerST::Get()->Connect(m_window->GetId(), wxEVT_SHELLWIN_LINE_ENTERED, wxCommandEventHandler(Manager::OnOutputWindow), NULL, ManagerST::Get());
	ManagerST::Get()->Connect(m_window->GetId(), wxEVT_SHELLWIN_LINE_ENTERED, wxCommandEventHandler(Manager::OnDebuggerWindow), NULL, ManagerST::Get());

	mainSizer->Add(m_window, 1, wxEXPAND | wxALL, 1);
	mainSizer->Layout();
}

void ShellTab::Clear()
{
	m_window->Clear();
}


void ShellTab::OnClearAll(wxCommandEvent &e)
{
	wxUnusedVar(e);
	Clear();
}

void ShellTab::OnWordWrap(wxCommandEvent &e)
{
	wxUnusedVar(e);
	wxScintilla *win = (wxScintilla*)m_window->GetOutWin();
	if(win->GetWrapMode() == wxSCI_WRAP_WORD){
		win->SetWrapMode(wxSCI_WRAP_NONE);
	}else{
		win->SetWrapMode(wxSCI_WRAP_WORD);
	}
}

void ShellTab::AppendText(const wxString &text)
{
	m_window->AppendLine(text);
}
