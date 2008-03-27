#include "wx/xrc/xmlres.h"
#include "workspacetab.h"
#include "manager.h"
#include "fileview.h"
#include "wx/combobox.h"
#include "wx/sizer.h"
#include "wx/button.h"
#include "frame.h"
#include "macros.h"

WorkspaceTab::WorkspaceTab(wxWindow *parent)
: wxPanel(parent)
{
	CreateGUIControls();
}

WorkspaceTab::~WorkspaceTab()
{
}

void WorkspaceTab::OnCollapseAll(wxCommandEvent &event)
{
	if(ManagerST::Get()->IsWorkspaceOpen() == false) {
		return;
	}
	
	wxTreeItemId root = m_fileView->GetRootItem();
	if(root.IsOk() == false) {
		return;
	}
	
	if(m_fileView->ItemHasChildren(root) == false) {
		return;
	}
	
	//iterate over all the projects items and collapse them all
	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_fileView->GetFirstChild(root, cookie);
	while( child.IsOk() ) {
		m_fileView->CollapseAllChildren(child);
		child = m_fileView->GetNextChild(root, cookie);
	}
}

void WorkspaceTab::CreateGUIControls()
{
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	SetSizer(sz);
	
	//add toolbar on top of the workspace tab that includes a single button that collapse all 
	//tree items
	
	wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORIZONTAL| wxTB_HORZ_TEXT);
	int id = wxNewId();
	tb->AddTool(id, wxT("Collapse All"), wxXmlResource::Get()->LoadBitmap(wxT("collapse")), wxT("Collapse All"), wxITEM_NORMAL);
	tb->Realize();
	
	Connect( id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WorkspaceTab::OnCollapseAll ));
	Connect( id, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WorkspaceTab::OnCollapseAllUI ));
	sz->Add(tb, 0, wxEXPAND, 0);
	
	wxStaticLine *staticline = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	sz->Add(staticline, 0, wxEXPAND);
	
	//add the workspace configuration combobox
	wxArrayString choices;
	m_workspaceConfig = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	m_workspaceConfig->Enable(false);
	
	// Connect an event to handle changes in the choice control
	ConnectCombo(m_workspaceConfig, Frame::OnWorkspaceConfigChanged);
	sz->Add(new wxStaticText(this, wxID_ANY, wxT("Configuration:")), 0, wxEXPAND| wxTOP | wxLEFT, 5);
	sz->Add(m_workspaceConfig, 0, wxEXPAND| wxTOP, 5);

	//add the fileview tab
	m_fileView = new FileViewTree(this, wxID_ANY);
	sz->Add(m_fileView, 1, wxEXPAND|wxTOP, 2);
}

void WorkspaceTab::BuildFileTree()
{
	m_fileView->BuildTree();
}

void WorkspaceTab::OnCollapseAllUI(wxUpdateUIEvent &event)
{
	event.Enable(ManagerST::Get()->IsWorkspaceOpen());
}
