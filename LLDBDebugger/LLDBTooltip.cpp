#include "LLDBTooltip.h"
#include "LLDBPlugin.h"
#include "macros.h"
#include "event_notifier.h"
#include <wx/wupdlock.h>
#include "cl_config.h"

LLDBTooltip::LLDBTooltip(wxWindow* parent, LLDBPlugin* plugin)
    : LLDBTooltipBase(parent)
    , m_plugin(plugin)
    , m_dragging(false)
{
    int initialSizeW = clConfig::Get().Read("LLDBTooltipW", wxNOT_FOUND);
    int initialSizeH = clConfig::Get().Read("LLDBTooltipH", wxNOT_FOUND);
    
    if ( initialSizeH != wxNOT_FOUND && initialSizeW != wxNOT_FOUND ) {
        wxSize initSize( initialSizeW, initialSizeH );
        
        if ( initSize.GetWidth() > 200 && initSize.GetHeight() > 150 ) {
            SetSize( initSize );
            
        } else {
            SetSize( wxSize(200, 150) );
        }
    }
    
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_VARIABLE_EXPANDED, &LLDBTooltip::OnLLDBVariableExpanded, this);
    m_panelStatus->Bind(wxEVT_MOUSE_CAPTURE_LOST, &LLDBTooltip::OnCaptureLost, this);
}

LLDBTooltip::~LLDBTooltip()
{
    if ( m_panelStatus->HasCapture() ) {
        m_panelStatus->ReleaseMouse();
    }

    // store the size
    wxSize sz = GetSize();
    clConfig::Get().Write("LLDBTooltipW", sz.GetWidth());
    clConfig::Get().Write("LLDBTooltipH", sz.GetHeight());

    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_VARIABLE_EXPANDED, &LLDBTooltip::OnLLDBVariableExpanded, this);
    m_panelStatus->Unbind(wxEVT_MOUSE_CAPTURE_LOST, &LLDBTooltip::OnCaptureLost, this);
}

void LLDBTooltip::OnItemExpanding(wxTreeEvent& event)
{
    CHECK_ITEM_RET( event.GetItem() );
    LLDBVariableClientData* data = ItemData( event.GetItem() );
    
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeCtrl->GetFirstChild(event.GetItem(), cookie);
    if ( m_treeCtrl->GetItemText( child ) == "<dummy>" ) {
        
        // dummy item, remove it
        m_treeCtrl->DeleteChildren( event.GetItem() );
        m_plugin->GetLLDB()->RequestVariableChildren( data->GetVariable()->GetLldbId() );
        
        // Store the treeitemid parent in cache
        m_itemsPendingExpansion.insert( std::make_pair(data->GetVariable()->GetLldbId(), event.GetItem()) );
        
    } else {
        event.Skip();
    }
}

void LLDBTooltip::Show(const wxString &displayName, LLDBVariable::Ptr_t variable)
{
    DoCleanup();
    wxTreeItemId item = m_treeCtrl->AddRoot(variable->ToString(displayName), wxNOT_FOUND, wxNOT_FOUND, new LLDBVariableClientData(variable));
    if ( variable->HasChildren() ) {
        m_treeCtrl->AppendItem(item, "<dummy>");
    }
    
    Move( ::wxGetMousePosition() );
    wxPopupWindow::Show();
    m_treeCtrl->SetFocus();
}

void LLDBTooltip::DoCleanup()
{
    m_treeCtrl->DeleteAllItems();
    m_itemsPendingExpansion.clear();
}

void LLDBTooltip::OnCheckMousePosition(wxTimerEvent& event)
{
#ifdef __WXMSW__
    // On Windows, wxPopupWindow does not return a correct position 
    // in screen coordinates. So we use the inside tree-ctrl to 
    // calc our position and size
    wxPoint tipPoint = m_treeCtrl->GetPosition();
    tipPoint = m_treeCtrl->ClientToScreen(tipPoint);
    wxSize  tipSize  = GetSize();
    // Construct a wxRect from the tip size/position
    wxRect rect( tipPoint, tipSize );
#else
    // Linux and OSX it works
    wxRect rect( GetRect() );
#endif

    // and increase it by 15 pixels
    rect.Inflate(15, 15);
    if ( !rect.Contains( ::wxGetMousePosition() ) ) {
        if ( m_panelStatus->HasCapture() ) {
            m_panelStatus->ReleaseMouse();
        }
        m_plugin->CallAfter( &LLDBPlugin::DestroyTooltip );
    }
}

LLDBVariableClientData* LLDBTooltip::ItemData(const wxTreeItemId& item) const
{
     return dynamic_cast<LLDBVariableClientData*>(m_treeCtrl->GetItemData( item ));
}

void LLDBTooltip::OnLLDBVariableExpanded(LLDBEvent& event)
{
    int variableId = event.GetVariableId();
    std::map<int, wxTreeItemId>::iterator iter = m_itemsPendingExpansion.find(event.GetVariableId());
    if ( iter == m_itemsPendingExpansion.end() ) {
        // does not belong to us
        event.Skip();
        return;
    }
    
    wxTreeItemId parentItem = iter->second;
    
    // add the variables to the tree
    for(size_t i=0; i<event.GetVariables().size(); ++i) {
        DoAddVariable( parentItem, event.GetVariables().at(i));
    }
    
    // Expand the parent item
    if ( m_treeCtrl->HasChildren(parentItem) ) {
        m_treeCtrl->Expand( parentItem );
    }

    // remove it
    m_itemsPendingExpansion.erase( iter );
}

void LLDBTooltip::DoAddVariable(const wxTreeItemId& parent, LLDBVariable::Ptr_t variable)
{
    wxTreeItemId item = m_treeCtrl->AppendItem(parent, variable->ToString(), wxNOT_FOUND, wxNOT_FOUND, new LLDBVariableClientData(variable));
    if ( variable->HasChildren() ) {
        m_treeCtrl->AppendItem(item, "<dummy>");
    }
}

void LLDBTooltip::OnStatusBarLeftDown(wxMouseEvent& event)
{
    m_dragging = true;
    wxSetCursor( wxCURSOR_SIZING );
    m_panelStatus->CaptureMouse();
}

void LLDBTooltip::OnStatusBarLeftUp(wxMouseEvent& event)
{
    event.Skip();
    DoUpdateSize(true);
}

void LLDBTooltip::OnCaptureLost(wxMouseCaptureLostEvent& e)
{
    e.Skip();
    if ( m_panelStatus->HasCapture() ) {
        m_panelStatus->ReleaseMouse();
        m_dragging = true;
    }
}

void LLDBTooltip::OnStatusBarMotion(wxMouseEvent& event)
{
    event.Skip();
    if ( m_dragging ) {
        wxRect curect = GetScreenRect();
        wxPoint curpos = ::wxGetMousePosition();

        int xDiff = curect.GetBottomRight().x - curpos.x;
        int yDiff = curect.GetBottomRight().y - curpos.y;

        if ( (abs(xDiff) > 3) || (abs(yDiff) > 3) ) {
            DoUpdateSize(false);
        }
    }
}

void LLDBTooltip::DoUpdateSize(bool performClean)
{
    if ( m_dragging ) {
        wxRect curect = GetScreenRect();
        curect.SetBottomRight( ::wxGetMousePosition() );
        if ( curect.GetHeight() > 100 && curect.GetWidth() > 100 ) {
#ifdef __WXMSW__
            wxWindowUpdateLocker locker(EventNotifier::Get()->TopFrame());
#endif
            SetSize( curect );
        }
    }
    
    if ( performClean ) {
        m_dragging = false;
        if ( m_panelStatus->HasCapture() ) {
            m_panelStatus->ReleaseMouse();
        }
        wxSetCursor( wxNullCursor );
    }
}

void LLDBTooltip::OnStatusEnterWindow(wxMouseEvent& event)
{
    wxSetCursor( wxCURSOR_SIZING );
}

void LLDBTooltip::OnStatusLeaveWindow(wxMouseEvent& event)
{
    wxSetCursor( wxNullCursor );
}
