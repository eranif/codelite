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
        m_dvListCtrlBreakpoints->AppendItem( cols );
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
}

void LLDBBreakpointsPane::OnDeleteBreakpointUI(wxUpdateUIEvent& event)
{
}
