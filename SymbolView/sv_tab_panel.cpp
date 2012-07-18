#include "sv_tab_panel.h"
#include "sv_symbol_tree.h"
#include "event_notifier.h"
#include "plugin.h"

SymbolViewTabPanel::SymbolViewTabPanel(wxWindow* parent, IManager* mgr)
    : SymbolViewTabPanelBaseClass(parent)
    , m_mgr(mgr)
{
    m_tree = new svSymbolTree(this, m_mgr, wxID_ANY);
    m_tree->AssignImageList( svSymbolTree::CreateSymbolTreeImages() );
    
    GetSizer()->Add(m_tree, 1, wxEXPAND);
    
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SymbolViewTabPanel::OnWorkspaceLoaded), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(SymbolViewTabPanel::OnActiveEditorChanged), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(SymbolViewTabPanel::OnEditorClosed), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(SymbolViewTabPanel::OnAllEditorsClosed), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(SymbolViewTabPanel::OnWorkspaceClosed), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_CMD_RETAG_COMPLETED, wxCommandEventHandler(SymbolViewTabPanel::OnFilesTagged), NULL, this);
}

SymbolViewTabPanel::~SymbolViewTabPanel()
{
	EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SymbolViewTabPanel::OnWorkspaceLoaded), NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(SymbolViewTabPanel::OnActiveEditorChanged), NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(SymbolViewTabPanel::OnEditorClosed), NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(SymbolViewTabPanel::OnAllEditorsClosed), NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(SymbolViewTabPanel::OnWorkspaceClosed), NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_CMD_RETAG_COMPLETED, wxCommandEventHandler(SymbolViewTabPanel::OnFilesTagged), NULL, this);
}

void SymbolViewTabPanel::OnSearchSymbol(wxCommandEvent& event)
{
    event.Skip();
}

void SymbolViewTabPanel::OnSearchEnter(wxCommandEvent& event)
{
}

void SymbolViewTabPanel::OnActiveEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    IEditor* editor = reinterpret_cast<IEditor*>(e.GetClientData());
    if(editor) {
        m_tree->BuildTree(editor->GetFileName());
    }
}

void SymbolViewTabPanel::OnAllEditorsClosed(wxCommandEvent& e)
{
    e.Skip();
    m_tree->Clear();
}

void SymbolViewTabPanel::OnEditorClosed(wxCommandEvent& e)
{
    e.Skip();
    m_tree->Clear();
}

void SymbolViewTabPanel::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    m_tree->Clear();
}

void SymbolViewTabPanel::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
}

void SymbolViewTabPanel::OnFilesTagged(wxCommandEvent& e)
{
    e.Skip();
	if(m_mgr->GetActiveEditor()) {
		m_tree->BuildTree( m_mgr->GetActiveEditor()->GetFileName() );
		
	} else {
		m_tree->Clear();
	}
}
