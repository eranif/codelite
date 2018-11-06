#include "CallFrame.h"
#include "DebuggerPane.h"
#include "NodeFileManager.h"
#include "NodeJSCLIDebugger.h"
#include "NodeJSEvents.h"
#include "NoteJSWorkspace.h"
#include "event_notifier.h"
#include <wx/wupdlock.h>

DebuggerPane::DebuggerPane(wxWindow* parent)
    : NodeJSCliDebuggerPaneBase(parent)
{
    m_dvListCtrlCallstack->SetSortFunction(nullptr);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_CLI_DEBUGGER_UPDATE_CALLSTACK, &DebuggerPane::OnUpdateBacktrace, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_CLI_DEBUGGER_STOPPED, &DebuggerPane::OnDebuggerStopped, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_MARK_LINE, &DebuggerPane::OnMarkLine, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_INTERACT, &DebuggerPane::OnInteract, this);
}

DebuggerPane::~DebuggerPane()
{
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_CLI_DEBUGGER_UPDATE_CALLSTACK, &DebuggerPane::OnUpdateBacktrace, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_CLI_DEBUGGER_STOPPED, &DebuggerPane::OnDebuggerStopped, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_MARK_LINE, &DebuggerPane::OnMarkLine, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_INTERACT, &DebuggerPane::OnInteract, this);
}

void DebuggerPane::OnUpdateBacktrace(clDebugCallFramesEvent& event)
{
    event.Skip();
    wxWindowUpdateLocker locker(m_dvListCtrlCallstack);
    const nSerializableObject::Vec_t& frames = event.GetCallFrames();
    m_dvListCtrlCallstack->DeleteAllItems();

    for(size_t i = 0; i < frames.size(); ++i) {
        CallFrame* frame = frames[i]->To<CallFrame>();

        wxVector<wxVariant> cols;
        wxString filename = NodeFileManager::Get().GetFilePath(frame->GetLocation().GetScriptId());
        cols.push_back(wxString() << "#" << i);
        cols.push_back(frame->GetFunctionName());
        cols.push_back(filename);
        cols.push_back(wxString() << (frame->GetLocation().GetLineNumber() + 1));
        m_dvListCtrlCallstack->AppendItem(cols);
        if(i == 0) {
            clDebugEvent event(wxEVT_NODEJS_DEBUGGER_MARK_LINE);
            event.SetLineNumber(frame->GetLocation().GetLineNumber() + 1);
            event.SetFileName(filename);
            EventNotifier::Get()->AddPendingEvent(event);
        }
    }
}
void DebuggerPane::OnDebuggerStopped(clDebugEvent& event)
{
    event.Skip();
    m_dvListCtrlCallstack->DeleteAllItems();
    NodeJSWorkspace::Get()->GetDebugger()->ClearDebuggerMarker();
}

void DebuggerPane::OnMarkLine(clDebugEvent& event)
{
    event.Skip();
    NodeJSWorkspace::Get()->GetDebugger()->ClearDebuggerMarker();
    NodeJSWorkspace::Get()->GetDebugger()->SetDebuggerMarker(event.GetFileName(), event.GetLineNumber());
}

void DebuggerPane::OnInteract(clDebugEvent& event)
{
    event.Skip();
    if(!event.IsAnswer()) {
        m_dvListCtrlCallstack->DeleteAllItems();
        NodeJSWorkspace::Get()->GetDebugger()->ClearDebuggerMarker();
    }
}
