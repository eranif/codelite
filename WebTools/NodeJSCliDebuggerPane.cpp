#include "NodeJSCLIDebugger.h"
#include "NodeJSCLIDebuggerFrameEntry.h"
#include "NodeJSCliDebuggerPane.h"
#include "NodeJSEvents.h"
#include "NoteJSWorkspace.h"
#include "event_notifier.h"
#include <wx/wupdlock.h>

NodeJSCliDebuggerPane::NodeJSCliDebuggerPane(wxWindow* parent)
    : NodeJSCliDebuggerPaneBase(parent)
{
    m_dvListCtrlCallstack->SetSortFunction(nullptr);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_CLI_DEBUGGER_UPDATE_CALLSTACK, &NodeJSCliDebuggerPane::OnUpdateBacktrace,
                               this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_CLI_DEBUGGER_STOPPED, &NodeJSCliDebuggerPane::OnDebuggerStopped, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_MARK_LINE, &NodeJSCliDebuggerPane::OnMarkLine, this);
}

NodeJSCliDebuggerPane::~NodeJSCliDebuggerPane()
{
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_CLI_DEBUGGER_UPDATE_CALLSTACK, &NodeJSCliDebuggerPane::OnUpdateBacktrace,
                                 this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_CLI_DEBUGGER_STOPPED, &NodeJSCliDebuggerPane::OnDebuggerStopped, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_MARK_LINE, &NodeJSCliDebuggerPane::OnMarkLine, this);
}

void NodeJSCliDebuggerPane::OnUpdateBacktrace(clDebugEvent& event)
{
    event.Skip();
    wxWindowUpdateLocker locker(m_dvListCtrlCallstack);
    m_dvListCtrlCallstack->DeleteAllItems();
    wxString bt = event.GetString();
    wxArrayString arr = ::wxStringTokenize(bt, "\n", wxTOKEN_STRTOK);
    for(size_t i = 0; i < arr.size(); ++i) {
        const wxString& frame = arr.Item(i).Trim().Trim(false);
        if(!frame.StartsWith("#")) { continue; }
        wxString ID = frame.BeforeFirst(' ');
        wxString where = frame.AfterFirst(' ');
        ID.Trim().Trim(false);
        where.Trim().Trim(false);
        wxVector<wxVariant> cols;
        cols.push_back(ID);
        cols.push_back(where);
        m_dvListCtrlCallstack->AppendItem(cols);
        if(i == 0) {
            clDebugEvent event(wxEVT_NODEJS_DEBUGGER_MARK_LINE);
            NodeJSCLIDebuggerFrameEntry row(where, NodeJSWorkspace::Get()->GetFileName().GetPath());
            event.SetLineNumber(row.GetLineNumber());
            event.SetFileName(row.GetFile());
            EventNotifier::Get()->AddPendingEvent(event);
        }
    }
}

void NodeJSCliDebuggerPane::OnDebuggerStopped(clDebugEvent& event)
{
    event.Skip();
    m_dvListCtrlCallstack->DeleteAllItems();
    NodeJSWorkspace::Get()->GetDebugger()->ClearDebuggerMarker();
}

void NodeJSCliDebuggerPane::OnMarkLine(clDebugEvent& event)
{
    event.Skip();
    NodeJSWorkspace::Get()->GetDebugger()->ClearDebuggerMarker();
    NodeJSWorkspace::Get()->GetDebugger()->SetDebuggerMarker(event.GetFileName(), event.GetLineNumber());
}
