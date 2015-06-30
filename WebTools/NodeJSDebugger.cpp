#include "NodeJSDebugger.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include "macros.h"
#include "NoteJSWorkspace.h"
#include <wx/msgdlg.h>
#include "asyncprocess.h"
#include "NodeJSDebuggerDlg.h"
#include "processreaderthread.h"
#include "file_logger.h"

#define CHECK_RUNNING() \
    if(!IsConnected()) return

NodeJSDebugger::NodeJSDebugger()
    : m_node(NULL)
{
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_START, &NodeJSDebugger::OnDebugStart, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_CONTINUE, &NodeJSDebugger::OnDebugContinue, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STOP, &NodeJSDebugger::OnStopDebugger, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_IS_RUNNING, &NodeJSDebugger::OnDebugIsRunning, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, &NodeJSDebugger::OnToggleBreakpoint, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_NEXT, &NodeJSDebugger::OnDebugNext, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_NEXT_INST, &NodeJSDebugger::OnVoid, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STEP_IN, &NodeJSDebugger::OnDebugStepIn, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STEP_OUT, &NodeJSDebugger::OnDebugStepOut, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_EXPR_TOOLTIP, &NodeJSDebugger::OnTooltip, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_CAN_INTERACT, &NodeJSDebugger::OnCanInteract, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &NodeJSDebugger::OnNodeTerminated, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &NodeJSDebugger::OnNodeOutput, this);
}

NodeJSDebugger::~NodeJSDebugger()
{
    m_socket.Reset(NULL);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_START, &NodeJSDebugger::OnDebugStart, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_CONTINUE, &NodeJSDebugger::OnDebugContinue, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STOP, &NodeJSDebugger::OnStopDebugger, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_IS_RUNNING, &NodeJSDebugger::OnDebugIsRunning, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, &NodeJSDebugger::OnToggleBreakpoint, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_NEXT, &NodeJSDebugger::OnDebugNext, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_NEXT_INST, &NodeJSDebugger::OnVoid, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STEP_IN, &NodeJSDebugger::OnDebugStepIn, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STEP_OUT, &NodeJSDebugger::OnDebugStepOut, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_EXPR_TOOLTIP, &NodeJSDebugger::OnTooltip, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_CAN_INTERACT, &NodeJSDebugger::OnCanInteract, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &NodeJSDebugger::OnNodeTerminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &NodeJSDebugger::OnNodeOutput, this);
    if(m_node) {
        m_node->Terminate();
        wxDELETE(m_node);
    }
}

void NodeJSDebugger::OnCanInteract(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
}
void NodeJSDebugger::OnDebugContinue(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
}
void NodeJSDebugger::OnDebugIsRunning(clDebugEvent& event)
{
    if(m_socket && m_socket->IsConnected()) {
        event.SetAnswer(true);

    } else {
        event.Skip();
    }
}

void NodeJSDebugger::OnDebugNext(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
}

void NodeJSDebugger::OnDebugStart(clDebugEvent& event)
{
    event.Skip();
    CHECK_COND_RET(NodeJSWorkspace::Get()->IsOpen());

    // Our to handle
    event.Skip(false);
    if(m_socket && m_socket->IsConnected()) return;

    NodeJSDebuggerDlg dlg(NULL);
    if(dlg.ShowModal() != wxID_OK) {
        return;
    }

    wxString command = dlg.GetCommand();

    // FIXME: extend the NodeJS dialog to allow using different port
    // Launch NodeJS
    m_node = ::CreateAsyncProcess(this, command, IProcessCreateConsole);
    if(!m_node) {
        ::wxMessageBox(_("Failed to start NodeJS application"), "CodeLite", wxOK | wxICON_ERROR | wxCENTER);
        m_socket.Reset(NULL);
    }

    // already connected?
    m_socket.Reset(new NodeJSSocket(this));
    if(!m_socket->Connect("127.0.0.1", 5858) && (m_socket->LastErrorCode() != wxSOCKET_WOULDBLOCK)) {
        ::wxMessageBox(wxString::Format(_("Failed to connect to NodeJS debugger\n%s"), m_socket->LastError()),
                       "CodeLite",
                       wxOK | wxICON_ERROR | wxCENTER);
        m_socket.Reset(NULL);
    }
}

void NodeJSDebugger::OnDebugStepIn(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
}

void NodeJSDebugger::OnDebugStepOut(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
}

void NodeJSDebugger::OnStopDebugger(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
}

void NodeJSDebugger::OnToggleBreakpoint(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
}

void NodeJSDebugger::OnTooltip(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
}

void NodeJSDebugger::OnVoid(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
    wxUnusedVar(event);
}

bool NodeJSDebugger::IsConnected() { return m_socket && m_socket->IsConnected(); }

void NodeJSDebugger::ConnectionEstablished() {}

void NodeJSDebugger::ConnectionLost() { m_socket.Reset(NULL); }

void NodeJSDebugger::OnNodeOutput(clProcessEvent& event)
{
    wxUnusedVar(event);
    CL_DEBUG("Node debugger:\n%s", event.GetOutput());
}

void NodeJSDebugger::OnNodeTerminated(clProcessEvent& event) { wxDELETE(m_node); }
