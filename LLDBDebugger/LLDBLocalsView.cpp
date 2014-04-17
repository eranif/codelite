#include "LLDBLocalsView.h"
#include "LLDBPlugin.h"
#include "file_logger.h"
#include <wx/treelist.h>
#include <wx/wupdlock.h>

LLDBLocalsView::LLDBLocalsView(wxWindow* parent, LLDBPlugin* plugin)
    : LLDBLocalsViewBase(parent)
    , m_plugin(plugin)
{
    m_treeList = new clTreeListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT|wxTR_HAS_BUTTONS|wxTR_FULL_ROW_HIGHLIGHT|wxTR_COLUMN_LINES|wxTR_ROW_LINES|wxTR_TWIST_BUTTONS);
    
    m_treeList->AddColumn(_("Name"), 150);
    m_treeList->AddColumn(_("Summary"), 300);
    m_treeList->AddColumn(_("Value"), 300);
    m_treeList->AddColumn(_("Type"), 300);
    
    m_treeList->AddRoot(_("Local Vairables"));
    GetSizer()->Add(m_treeList, 1, wxEXPAND|wxALL, 2);

    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_STARTED, &LLDBLocalsView::OnLLDBStarted, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_EXITED,  &LLDBLocalsView::OnLLDBExited,  this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_LOCALS_UPDATED, &LLDBLocalsView::OnLLDBLocalsUpdated, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_RUNNING, &LLDBLocalsView::OnLLDBRunning, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_VARIABLE_EXPANDED, &LLDBLocalsView::OnLLDBVariableExpanded, this);
    
    m_treeList->Bind(wxEVT_COMMAND_TREE_ITEM_EXPANDING, &LLDBLocalsView::OnItemExpanding, this);
    GetSizer()->Layout();
}

LLDBLocalsView::~LLDBLocalsView()
{
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_STARTED, &LLDBLocalsView::OnLLDBStarted, this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_EXITED,  &LLDBLocalsView::OnLLDBExited,  this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_LOCALS_UPDATED, &LLDBLocalsView::OnLLDBLocalsUpdated, this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_RUNNING, &LLDBLocalsView::OnLLDBRunning, this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_VARIABLE_EXPANDED, &LLDBLocalsView::OnLLDBVariableExpanded, this);
    m_treeList->Unbind(wxEVT_COMMAND_TREE_ITEM_EXPANDING, &LLDBLocalsView::OnItemExpanding, this);
}

void LLDBLocalsView::OnLLDBExited(LLDBEvent& event)
{
    event.Skip();
    Cleanup();
}

void LLDBLocalsView::OnLLDBRunning(LLDBEvent& event)
{
    event.Skip();
    Cleanup();
}

void LLDBLocalsView::OnLLDBStarted(LLDBEvent& event)
{
    event.Skip();
    Cleanup();
}

void LLDBLocalsView::OnLLDBLocalsUpdated(LLDBEvent& event)
{
    // FIXME: optimize this to only retrieve the top levle variables
    // the children should be obtained in the 'OnItemExpading' event handler
    event.Skip();
    wxWindowUpdateLocker locker(m_treeList);
    Enable(true);
    
    m_treeList->DeleteChildren( m_treeList->GetRootItem() );
    CL_DEBUG("Updating locals view");
    DoAddVariableToView( event.GetVariables(), m_treeList->GetRootItem() );
}

void LLDBLocalsView::DoAddVariableToView(const LLDBVariable::Vect_t& variables, wxTreeItemId parent)
{
    for(size_t i=0; i<variables.size(); ++i) {
        LLDBVariable::Ptr_t variable = variables.at(i);
        wxTreeItemId item = m_treeList->AppendItem(parent, variable->GetName(), wxNOT_FOUND, wxNOT_FOUND, new LLDBLocalVariableClientData(variable));
        m_treeList->SetItemText(item, 1, variable->GetSummary());
        m_treeList->SetItemText(item, 2, variable->GetValue());
        m_treeList->SetItemText(item, 3, variable->GetType());
        if ( variable->IsValueChanged() ) {
            m_treeList->SetItemTextColour( item, "RED" );
        }
        if ( variable->HasChildren() ) {
            // insert dummy item here
            m_treeList->AppendItem(item, "<dummy>");
        }
    }
    
    if ( !variables.empty() ) {
        m_treeList->Expand( parent );
    }
}

void LLDBLocalsView::OnItemExpanding(wxTreeEvent& event)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeList->GetFirstChild(event.GetItem(), cookie);
    if ( m_treeList->GetItemText(child) == "<dummy>") {
        event.Veto();
        m_treeList->DeleteChildren( event.GetItem() );
        
        // query the debugger about the children of this node
        if ( m_plugin->GetLLDB()->IsCanInteract() ) {
            int variableId = GetItemData(event.GetItem())->GetVariable()->GetLldbId();
            m_plugin->GetLLDB()->RequestVariableChildren( variableId );
            m_pendingExpandItems.insert( std::make_pair(variableId, event.GetItem()) );
        }
        
    } else {
        event.Skip();
    }
}

LLDBLocalVariableClientData* LLDBLocalsView::GetItemData(const wxTreeItemId& id)
{
    LLDBLocalVariableClientData* cd = dynamic_cast<LLDBLocalVariableClientData*>(m_treeList->GetItemData(id));
    return cd;
}

void LLDBLocalsView::Cleanup()
{
    m_treeList->DeleteChildren( m_treeList->GetRootItem() );
    m_pendingExpandItems.clear();
}

void LLDBLocalsView::OnLLDBVariableExpanded(LLDBEvent& event)
{
    int variableId = event.GetVariableId();
    // try to locate this item in our map
    LLDBLocalsView::IntItemMap_t::iterator iter = m_pendingExpandItems.find(variableId);
    if ( iter == m_pendingExpandItems.end() ) {
        // does not belong to us - skip it
        event.Skip();
        return;
    }
    
    // add the variables 
    DoAddVariableToView( event.GetVariables(), iter->second);
    m_pendingExpandItems.erase( iter );
}
