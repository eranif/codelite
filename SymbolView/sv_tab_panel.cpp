#include "sv_tab_panel.h"
#include "sv_symbol_tree.h"
#include "event_notifier.h"
#include "plugin.h"
#include <wx/wxscintilla.h>

const wxEventType wxEVT_SV_GOTO_DEFINITION  = wxNewEventType();
const wxEventType wxEVT_SV_GOTO_DECLARATION = wxNewEventType();
const wxEventType wxEVT_SV_FIND_REFERENCES  = wxNewEventType();

SymbolViewTabPanel::SymbolViewTabPanel(wxWindow* parent, IManager* mgr)
    : SymbolViewTabPanelBaseClass(parent)
    , m_mgr(mgr)
{
    m_tree = new svSymbolTree(this, m_mgr, wxID_ANY);
    m_tree->Connect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(SymbolViewTabPanel::OnMenu), NULL, this);
    
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
    m_tree->Disconnect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(SymbolViewTabPanel::OnMenu), NULL, this);
    
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
    wxString name = m_textCtrlSearch->GetValue();
    name.Trim().Trim(false);
    m_tree->SelectItemByName(name);
}

void SymbolViewTabPanel::OnSearchEnter(wxCommandEvent& event)
{
    event.Skip();
    wxString name = m_textCtrlSearch->GetValue();
    name.Trim().Trim(false);
    if(name.IsEmpty() == false) {
        m_tree->ActivateSelectedItem();
    }
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
    IEditor* editor = m_mgr->GetActiveEditor();
    if( editor ) {
        m_tree->BuildTree( editor->GetFileName() );
        
        if(editor->GetScintilla()) {
            // make sure we dont steal the focus from the editor...
            editor->GetScintilla()->SetFocus();
        }
        
    } else {
        m_tree->Clear();
    }
}
void SymbolViewTabPanel::OnMenu(wxContextMenuEvent& e)
{
    wxMenu menu;
    menu.Append(wxEVT_SV_GOTO_DEFINITION,  _("Goto Definition"));
    menu.Append(wxEVT_SV_GOTO_DECLARATION, _("Goto Declaration"));
    menu.AppendSeparator();
    menu.Append(wxEVT_SV_FIND_REFERENCES , _("Find References..."));
    
    menu.Connect(wxEVT_SV_GOTO_DEFINITION,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SymbolViewTabPanel::OnGotoImpl),      NULL, this);
    menu.Connect(wxEVT_SV_GOTO_DECLARATION, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SymbolViewTabPanel::OnGotoDecl),      NULL, this);
    menu.Connect(wxEVT_SV_FIND_REFERENCES , wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SymbolViewTabPanel::OnFindReferenes), NULL, this);
    m_tree->PopupMenu(&menu);
}

void SymbolViewTabPanel::OnGotoDecl(wxCommandEvent& e)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_decl"));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void SymbolViewTabPanel::OnGotoImpl(wxCommandEvent& e)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_impl"));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void SymbolViewTabPanel::OnFindReferenes(wxCommandEvent& e)
{    
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_references"));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}
