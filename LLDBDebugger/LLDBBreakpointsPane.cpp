#include "LLDBBreakpointsPane.h"
#include "LLDBBreakpoint.h"
#include "LLDBDebugger.h"
#include "event_notifier.h"
#include "LLDBNewBreakpointDlg.h"
#include "lldbdebugger-plugin.h"
#include "ieditor.h"

LLDBBreakpointsPane::LLDBBreakpointsPane(wxWindow* parent, LLDBDebuggerPlugin* plugin)
    : LLDBBreakpointsPaneBase(parent)
    , m_plugin(plugin)
    , m_lldb(plugin->GetLLDB())
{
    Initialize();
    m_lldb->Bind(wxEVT_LLDB_BREAKPOINTS_UPDATED, &LLDBBreakpointsPane::OnBreakpointsUpdated, this);
    m_lldb->Bind(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL, &LLDBBreakpointsPane::OnBreakpointsUpdated, this);
}

LLDBBreakpointsPane::~LLDBBreakpointsPane()
{
    m_lldb->Unbind(wxEVT_LLDB_BREAKPOINTS_UPDATED, &LLDBBreakpointsPane::OnBreakpointsUpdated, this);
    m_lldb->Unbind(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL, &LLDBBreakpointsPane::OnBreakpointsUpdated, this);
}

void LLDBBreakpointsPane::OnBreakpointActivated(wxDataViewEvent& event)
{
    event.Skip();
    CallAfter( &LLDBBreakpointsPane::GotoBreakpoint, GetBreakpoint(event.GetItem()));
}

void LLDBBreakpointsPane::Clear()
{
    // free all user data from the list
    for(size_t i=0; i<m_dvListCtrlBreakpoints->GetItemCount(); ++i) {
        LLDBBreakpoint* bp = GetBreakpoint( m_dvListCtrlBreakpoints->RowToItem(i) );
        wxDELETE( bp );
    }
    m_dvListCtrlBreakpoints->DeleteAllItems();
}

void LLDBBreakpointsPane::Initialize()
{
    Clear();
    const LLDBBreakpoint::Vec_t &breakpoints = m_lldb->GetBreakpoints();
    for(size_t i=0; i<breakpoints.size(); ++i) {
        wxVector<wxVariant> cols;
        const LLDBBreakpoint& bp = breakpoints.at(i);
        cols.push_back( wxString() << bp.GetId() );
        cols.push_back( bp.GetFilename() );
        cols.push_back( wxString() << bp.GetLineNumber() );
        cols.push_back( bp.GetName() );
        m_dvListCtrlBreakpoints->AppendItem( cols, (wxUIntPtr)new LLDBBreakpoint(bp) );
    }
}

void LLDBBreakpointsPane::OnBreakpointsUpdated(LLDBEvent& event)
{
    event.Skip();
    Initialize();
}

void LLDBBreakpointsPane::OnNewBreakpoint(wxCommandEvent& event)
{
    LLDBNewBreakpointDlg dlg(EventNotifier::Get()->TopFrame());
    if ( dlg.ShowModal() == wxID_OK ) {
        LLDBBreakpoint bp = dlg.GetBreakpoint();
        if ( bp.IsValid() ) {

            // Add the breakpoint
            LLDBBreakpoint::Vec_t bps;
            bps.push_back( bp );
            m_lldb->AddBreakpoints( bps );
            
            // If we can't interact with the debugger atm, interrupt it
            if ( !m_lldb->CanInteract() ) {
                m_lldb->Interrupt(LLDBDebugger::kInterruptReasonApplyBreakpoints);
                
            } else {
                // Apply the breakpoints now
                m_lldb->ApplyBreakpoints();
            }
        }
    }
}

void LLDBBreakpointsPane::OnNewBreakpointUI(wxUpdateUIEvent& event)
{
    event.Enable(true);
}

void LLDBBreakpointsPane::OnDeleteAll(wxCommandEvent& event)
{
    wxUnusedVar( event );
    if ( m_lldb->CanInteract() ) {
        m_lldb->DeleteAllBreakpoints();
        
    } else {
        m_lldb->Interrupt(LLDBDebugger::kInterruptReasonDeleteAllBreakpoints);
    }
}

void LLDBBreakpointsPane::OnDeleteAllUI(wxUpdateUIEvent& event)
{
    event.Enable( m_dvListCtrlBreakpoints->GetItemCount() );
}

void LLDBBreakpointsPane::OnDeleteBreakpoint(wxCommandEvent& event)
{
    // get the breakpoint we want to delete
    LLDBBreakpoint::Vec_t bps;
    wxDataViewItemArray items;
    m_dvListCtrlBreakpoints->GetSelections( items );
    for(size_t i=0; i<items.GetCount(); ++i) {
        bps.push_back( *GetBreakpoint( items.Item(i) ) );
    }
    
    if ( !bps.empty() ) {
        m_lldb->DeleteBreakpoints( bps );
    }
}

void LLDBBreakpointsPane::OnDeleteBreakpointUI(wxUpdateUIEvent& event)
{
    event.Enable( m_dvListCtrlBreakpoints->GetSelectedItemsCount() );
}

LLDBBreakpoint* LLDBBreakpointsPane::GetBreakpoint(const wxDataViewItem& item)
{
    LLDBBreakpoint* bp = reinterpret_cast<LLDBBreakpoint*>(m_dvListCtrlBreakpoints->GetItemData( item ));
    return bp;
}

void LLDBBreakpointsPane::GotoBreakpoint(LLDBBreakpoint* bp)
{
    if ( !bp ) return;
    wxFileName fileLoc(bp->GetFilename());
    if ( fileLoc.Exists() ) {
        if ( m_plugin->GetManager()->OpenFile(fileLoc.GetFullPath(), "", bp->GetLineNumber()-1) ) {
            IEditor* editor = m_plugin->GetManager()->GetActiveEditor();
            if ( editor ) {
                editor->SetActive();
            }
        }
    }
}
