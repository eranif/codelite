#include "LLDBBreakpointsPane.h"
#include "LLDBBreakpoint.h"
#include "LLDBDebugger.h"

LLDBBreakpointsPane::LLDBBreakpointsPane(wxWindow* parent, LLDBDebugger* lldb)
    : LLDBBreakpointsPaneBase(parent)
    , m_lldb(lldb)
{
    Initialize();
    m_lldb->Bind(wxEVT_LLDB_BREAKPOINTS_UPDATED, &LLDBBreakpointsPane::OnBreakpointsUpdated, this);
}

LLDBBreakpointsPane::~LLDBBreakpointsPane()
{
    m_lldb->Unbind(wxEVT_LLDB_BREAKPOINTS_UPDATED, &LLDBBreakpointsPane::OnBreakpointsUpdated, this);
}

void LLDBBreakpointsPane::OnBreakpointActivated(wxDataViewEvent& event)
{
}

void LLDBBreakpointsPane::OnDeleteAllBreakpoints(wxCommandEvent& event)
{
}

void LLDBBreakpointsPane::OnDeleteAllBreakpointsUI(wxUpdateUIEvent& event)
{
}

void LLDBBreakpointsPane::OnDeleteBreakpoint(wxCommandEvent& event)
{
}

void LLDBBreakpointsPane::OnDeleteBreakpointUI(wxUpdateUIEvent& event)
{
    event.Enable( m_dvListCtrlBreakpoints->GetSelectedItemsCount() );
}

void LLDBBreakpointsPane::OnNewBreakpoint(wxCommandEvent& event)
{
}

void LLDBBreakpointsPane::OnNewBreakpointUI(wxUpdateUIEvent& event)
{
    event.Enable( true );
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
