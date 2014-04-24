#include "LLDBBreakpointsPane.h"
#include "LLDBProtocol/LLDBBreakpoint.h"
#include "event_notifier.h"
#include "LLDBNewBreakpointDlg.h"
#include "LLDBPlugin.h"
#include "ieditor.h"
#include "LLDBProtocol/LLDBEnums.h"
#include "file_logger.h"

class LLDBBreakpointClientData : public wxClientData
{
    LLDBBreakpoint::Ptr_t m_breakpoint;
public:
    LLDBBreakpointClientData(LLDBBreakpoint::Ptr_t bp) : m_breakpoint(bp) {}

    LLDBBreakpoint::Ptr_t GetBreakpoint() {
        return m_breakpoint;
    }
};

LLDBBreakpointsPane::LLDBBreakpointsPane(wxWindow* parent, LLDBPlugin* plugin)
    : LLDBBreakpointsPaneBase(parent)
    , m_plugin(plugin)
    , m_connector(plugin->GetLLDB())
{
    Initialize();
    m_connector->Bind(wxEVT_LLDB_BREAKPOINTS_UPDATED, &LLDBBreakpointsPane::OnBreakpointsUpdated, this);
    m_connector->Bind(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL, &LLDBBreakpointsPane::OnBreakpointsUpdated, this);
}

LLDBBreakpointsPane::~LLDBBreakpointsPane()
{
    m_connector->Unbind(wxEVT_LLDB_BREAKPOINTS_UPDATED, &LLDBBreakpointsPane::OnBreakpointsUpdated, this);
    m_connector->Unbind(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL, &LLDBBreakpointsPane::OnBreakpointsUpdated, this);
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
    LLDBBreakpoint::Vec_t breakpoints = m_connector->GetAllBreakpoints();
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
    CL_DEBUG("Setting LLDB breakpoints to:");
    for(size_t i=0; i<event.GetBreakpoints().size(); ++i) {
        CL_DEBUG("%s", event.GetBreakpoints().at(i)->ToString());
    }
    m_connector->UpdateAppliedBreakpoints( event.GetBreakpoints() );
    Initialize();
}

void LLDBBreakpointsPane::OnNewBreakpoint(wxCommandEvent& event)
{
    LLDBNewBreakpointDlg dlg(EventNotifier::Get()->TopFrame());
    if ( dlg.ShowModal() == wxID_OK ) {
        LLDBBreakpoint::Ptr_t bp = dlg.GetBreakpoint();
        if ( bp->IsValid() ) {
            m_connector->AddBreakpoint( bp );
            m_connector->ApplyBreakpoints();
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
    m_connector->DeleteAllBreakpoints();
}

void LLDBBreakpointsPane::OnDeleteAllUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_dataviewModel->IsEmpty() );
}

void LLDBBreakpointsPane::OnDeleteBreakpoint(wxCommandEvent& event)
{
    // get the breakpoint we want to delete
    wxDataViewItemArray items;
    m_dataview->GetSelections( items );
    for(size_t i=0; i<items.GetCount(); ++i) {
        m_connector->MarkBreakpointForDeletion( GetBreakpoint( items.Item(i) ) );
    }
    m_connector->DeleteBreakpoints();
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
