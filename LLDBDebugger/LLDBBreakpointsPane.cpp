#include "LLDBBreakpointsPane.h"
#include "LLDBBreakpoint.h"
#include "LLDBDebugger.h"
#include "event_notifier.h"
#include "LLDBNewBreakpointDlg.h"
#include "lldbdebugger-plugin.h"
#include "ieditor.h"
#include "LLDBProtocol/LLDBEnums.h"

class LLDBBreakpointClientData : public wxClientData
{
    LLDBBreakpoint::Ptr_t m_breakpoint;
public:
    LLDBBreakpointClientData(LLDBBreakpoint::Ptr_t bp) : m_breakpoint(bp) {}

    LLDBBreakpoint::Ptr_t GetBreakpoint() {
        return m_breakpoint;
    }
};

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
    m_dataviewModel->Clear();
}

void LLDBBreakpointsPane::Initialize()
{
    Clear();
    const LLDBBreakpoint::Vec_t &breakpoints = m_lldb->GetBreakpoints();
    for(size_t i=0; i<breakpoints.size(); ++i) {
        
        wxVector<wxVariant> cols;
        LLDBBreakpoint::Ptr_t bp = breakpoints.at(i);
        cols.push_back( wxString() << bp->GetId() );
        cols.push_back( bp->GetFilename() );
        cols.push_back( wxString() << bp->GetLineNumber() );
        cols.push_back( bp->GetName() );
        wxDataViewItem parent = m_dataviewModel->AppendItem(wxDataViewItem(NULL), cols, new LLDBBreakpointClientData(bp));
        
        // add the children breakpoints (sites)
        if ( !bp->GetChildren().empty() ) {
            const LLDBBreakpoint::Vec_t& children = bp->GetChildren();
            for(size_t i=0; i<children.size(); ++i) {
                cols.clear();
                LLDBBreakpoint::Ptr_t breakpoint = children.at(i);
                cols.push_back( "" );
                cols.push_back( breakpoint->GetFilename() );
                cols.push_back( wxString() << breakpoint->GetLineNumber() );
                cols.push_back( breakpoint->GetName() );
                m_dataviewModel->AppendItem(parent, cols, new LLDBBreakpointClientData(breakpoint));
            }
        }
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
        LLDBBreakpoint::Ptr_t bp = dlg.GetBreakpoint();
        if ( bp->IsValid() ) {

            // Add the breakpoint
            LLDBBreakpoint::Vec_t bps;
            bps.push_back( bp );
            m_lldb->AddBreakpoints( bps );

            // If we can't interact with the debugger atm, interrupt it
            if ( !m_lldb->CanInteract() ) {
                m_lldb->Interrupt(kInterruptReasonApplyBreakpoints);

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
        m_lldb->Interrupt(kInterruptReasonDeleteAllBreakpoints);
    }
}

void LLDBBreakpointsPane::OnDeleteAllUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_dataviewModel->IsEmpty() );
}

void LLDBBreakpointsPane::OnDeleteBreakpoint(wxCommandEvent& event)
{
    // get the breakpoint we want to delete
    LLDBBreakpoint::Vec_t bps;
    wxDataViewItemArray items;
    m_dataview->GetSelections( items );
    for(size_t i=0; i<items.GetCount(); ++i) {
        bps.push_back( GetBreakpoint( items.Item(i) ) );
    }

    if ( !bps.empty() ) {
        m_lldb->DeleteBreakpoints( bps );
    }
}

void LLDBBreakpointsPane::OnDeleteBreakpointUI(wxUpdateUIEvent& event)
{
    wxDataViewItem item = m_dataview->GetSelection();
    LLDBBreakpoint::Ptr_t bp = GetBreakpoint( item );
    event.Enable( bp && !bp->IsLocation() );
}

LLDBBreakpoint::Ptr_t LLDBBreakpointsPane::GetBreakpoint(const wxDataViewItem& item)
{
    if ( !item.IsOk() ) {
        return LLDBBreakpoint::Ptr_t(NULL);
    }
    
    LLDBBreakpointClientData* cd = dynamic_cast<LLDBBreakpointClientData*>(m_dataviewModel->GetClientObject(item));
    return cd->GetBreakpoint();
}

void LLDBBreakpointsPane::GotoBreakpoint(LLDBBreakpoint::Ptr_t bp)
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
