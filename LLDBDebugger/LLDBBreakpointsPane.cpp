#include "LLDBBreakpointsPane.h"
#include "LLDBBreakpoint.h"
#include "LLDBDebugger.h"

LLDBBreakpointsPane::LLDBBreakpointsPane(wxWindow* parent, LLDBDebugger* lldb)
    : LLDBBreakpointsPaneBase(parent)
    , m_lldb(lldb)
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
}

void LLDBBreakpointsPane::OnNewBreakpointUI(wxUpdateUIEvent& event)
{
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
