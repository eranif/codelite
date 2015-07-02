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
#include "globals.h"
#include "imanager.h"
#include "ieditor.h"
#include "NodeJSSetBreakpointHandler.h"
#include <algorithm>
#include "NodeJSContinueHandler.h"
#include "NodeJSCallstackHandler.h"
#include "NodeJSEvents.h"
#include "bookmark_manager.h"

#define CHECK_RUNNING() \
    if(!IsConnected()) return

NodeJSDebugger::NodeJSDebugger()
    : m_node(NULL)
    , m_canInteract(false)
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
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &NodeJSDebugger::OnWorkspaceOpened, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &NodeJSDebugger::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_MARK_LINE, &NodeJSDebugger::OnHighlightLine, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_SELECT_FRAME, &NodeJSDebugger::OnSelectFrame, this);

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
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &NodeJSDebugger::OnWorkspaceOpened, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &NodeJSDebugger::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_MARK_LINE, &NodeJSDebugger::OnHighlightLine, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_SELECT_FRAME, &NodeJSDebugger::OnSelectFrame, this);

    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &NodeJSDebugger::OnNodeTerminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &NodeJSDebugger::OnNodeOutput, this);
    if(m_node) {
        m_node->Terminate();
        wxDELETE(m_node);
    }
    m_bptManager.Save();
}

void NodeJSDebugger::OnCanInteract(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
    event.Skip(false);
    event.SetAnswer(IsCanInteract());
}

void NodeJSDebugger::OnDebugContinue(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
    event.Skip(false);
    Continue();
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

    event.Skip(false);
    // Build the request
    JSONElement request = JSONElement::createObject();
    request.addProperty("type", "request");
    request.addProperty("command", "continue");
    JSONElement args = JSONElement::createObject("arguments");
    request.append(args);
    args.addProperty("stepaction", "next");
    args.addProperty("stepcount", 1); // how we tell nodejs to continue until next bp is hit?

    // Write the command
    m_socket->WriteRequest(request, new NodeJSContinueHandler());
}

void NodeJSDebugger::OnDebugStart(clDebugEvent& event)
{
    event.Skip();
    CHECK_COND_RET(NodeJSWorkspace::Get()->IsOpen());

    // Our to handle
    event.Skip(false);
    if(m_socket && m_socket->IsConnected()) {
        Continue();
        return;
    };

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

    clDebugEvent eventStart(wxEVT_NODEJS_DEBUGGER_STARTED);
    eventStart.SetDebuggerName("Node.js");
    EventNotifier::Get()->AddPendingEvent(eventStart);
}

void NodeJSDebugger::OnDebugStepIn(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();

    event.Skip(false);
    // Build the request
    JSONElement request = JSONElement::createObject();
    request.addProperty("type", "request");
    request.addProperty("command", "continue");
    JSONElement args = JSONElement::createObject("arguments");
    request.append(args);
    args.addProperty("stepaction", "in");
    args.addProperty("stepcount", 1); // how we tell nodejs to continue until next bp is hit?

    // Write the command
    m_socket->WriteRequest(request, new NodeJSContinueHandler());
}

void NodeJSDebugger::OnDebugStepOut(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();

    event.Skip(false);
    // Build the request
    JSONElement request = JSONElement::createObject();
    request.addProperty("type", "request");
    request.addProperty("command", "continue");
    JSONElement args = JSONElement::createObject("arguments");
    request.append(args);
    args.addProperty("stepaction", "out");
    args.addProperty("stepcount", 1); // how we tell nodejs to continue until next bp is hit?

    // Write the command
    m_socket->WriteRequest(request, new NodeJSContinueHandler());
}

void NodeJSDebugger::OnStopDebugger(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();

    event.Skip(false);
    if(m_node) {
        m_node->Terminate();
    }
}

void NodeJSDebugger::OnToggleBreakpoint(clDebugEvent& event)
{
    event.Skip();
    if(NodeJSWorkspace::Get()->IsOpen()) {
        event.Skip(false);
        IEditor* editor = clGetManager()->GetActiveEditor();
        if(editor && (editor->GetFileName().GetFullPath() == event.GetFileName())) {
            // Correct editor
            // add marker
            NodeJSBreakpoint bp = m_bptManager.GetBreakpoint(event.GetFileName(), event.GetInt());
            if(bp.IsOk()) {
                if(bp.IsApplied() && IsConnected()) {
                    // Tell NodeJS to delete this breakpoint
                    DeleteBreakpoint(bp);
                }
                m_bptManager.DeleteBreakpoint(event.GetFileName(), event.GetInt());
            } else {
                // We have no breakpoint on this file/line (yet)
                m_bptManager.AddBreakpoint(event.GetFileName(), event.GetInt());
                bp = m_bptManager.GetBreakpoint(event.GetFileName(), event.GetInt());
                if(IsConnected()) {
                    SetBreakpoint(bp);
                }
            }

            // Update the UI
            m_bptManager.SetBreakpoints(editor);
        }
    }
}

void NodeJSDebugger::OnTooltip(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
    event.Skip(false);
}

void NodeJSDebugger::OnVoid(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
    event.Skip(false);
    wxUnusedVar(event);
}

bool NodeJSDebugger::IsConnected() { return m_socket && m_socket->IsConnected(); }

void NodeJSDebugger::ConnectionEstablished() {}

void NodeJSDebugger::ConnectionLost()
{
    if(m_node) {
        m_node->Terminate();
    }
    m_socket.Reset(NULL);

    clDebugEvent event(wxEVT_NODEJS_DEBUGGER_STOPPED);
    event.SetDebuggerName("Node.js");
    EventNotifier::Get()->AddPendingEvent(event);

    // Clear the debugger markers
    ClearDebuggerMarker();
}

void NodeJSDebugger::OnNodeOutput(clProcessEvent& event)
{
    wxUnusedVar(event);
    CL_DEBUG("Node debugger:\n%s", event.GetOutput());

    clDebugEvent eventLog(wxEVT_NODEJS_DEBUGGER_CONSOLE_LOG);
    eventLog.SetString(event.GetOutput());
    EventNotifier::Get()->AddPendingEvent(eventLog);
}

void NodeJSDebugger::OnNodeTerminated(clProcessEvent& event) { wxDELETE(m_node); }

void NodeJSDebugger::OnWorkspaceClosed(wxCommandEvent& event) { event.Skip(); }

void NodeJSDebugger::OnWorkspaceOpened(wxCommandEvent& event) { event.Skip(); }

void NodeJSDebugger::DeleteBreakpoint(const NodeJSBreakpoint& bp)
{
    // Sanity
    if(!IsConnected()) return;
    if(!bp.IsApplied()) return;

    // Build the request
    JSONElement request = JSONElement::createObject();
    request.addProperty("type", "request");
    request.addProperty("command", "clearbreakpoint");
    JSONElement args = JSONElement::createObject("arguments");
    request.append(args);
    args.addProperty("breakpoint", bp.GetNodeBpID());

    // Write the command
    m_socket->WriteRequest(request, new NodeJSSetBreakpointHandler(bp));
}

void NodeJSDebugger::SetBreakpoint(const NodeJSBreakpoint& bp)
{
    // Sanity
    if(!IsConnected()) return;

    // Build the request
    JSONElement request = JSONElement::createObject();
    request.addProperty("type", "request");
    request.addProperty("command", "setbreakpoint");
    JSONElement args = JSONElement::createObject("arguments");
    request.append(args);
    args.addProperty("type", "script");
    args.addProperty("target", bp.GetFilename());
    args.addProperty("line", bp.GetLine() - 1);
    args.addProperty("column", 0);

    // Write the command
    m_socket->WriteRequest(request, new NodeJSSetBreakpointHandler(bp));
}

void NodeJSDebugger::Continue()
{
    // Sanity
    if(!IsConnected()) return;

    // Build the request
    JSONElement request = JSONElement::createObject();
    request.addProperty("type", "request");
    request.addProperty("command", "continue");

    // Write the command
    m_socket->WriteRequest(request, new NodeJSContinueHandler());
}

void NodeJSDebugger::SetBreakpoints()
{
    // Sanity
    if(!IsConnected()) return;
    const NodeJSBreakpoint::List_t& bps = m_bptManager.GetBreakpoints();
    std::for_each(bps.begin(), bps.end(), [&](const NodeJSBreakpoint& bp) { SetBreakpoint(bp); });
}

void NodeJSDebugger::GotControl(bool requestBacktrace)
{
    SetCanInteract(true);
    EventNotifier::Get()->TopFrame()->Raise();
    if(requestBacktrace) {
        Callstack();
    }
}

void NodeJSDebugger::Callstack()
{
    // Sanity
    if(!IsConnected()) return;

    // Build the request
    JSONElement request = JSONElement::createObject();
    request.addProperty("type", "request");
    request.addProperty("command", "backtrace");

    // Write the command
    m_socket->WriteRequest(request, new NodeJSCallstackHandler());
}

void NodeJSDebugger::SelectFrame(int frameId)
{
    // Sanity
    if(!IsConnected()) return;

    // Build the request
    JSONElement request = JSONElement::createObject();
    request.addProperty("type", "request");
    request.addProperty("command", "frame");

    JSONElement args = JSONElement::createObject("arguments");
    request.append(args);
    args.addProperty("number", frameId);

    // Write the command
    m_socket->WriteRequest(request, NULL);
}

void NodeJSDebugger::SetCanInteract(bool canInteract)
{
    clDebugEvent event(canInteract ? wxEVT_NODEJS_DEBUGGER_CAN_INTERACT : wxEVT_NODEJS_DEBUGGER_LOST_INTERACT);
    EventNotifier::Get()->ProcessEvent(event);
    this->m_canInteract = canInteract;
    if(!canInteract) {
        ClearDebuggerMarker();
    }
}

void NodeJSDebugger::SetDebuggerMarker(IEditor* editor, int lineno)
{
    wxStyledTextCtrl* stc = editor->GetCtrl();
    stc->MarkerDeleteAll(smt_indicator);
    stc->MarkerAdd(lineno, smt_indicator);
    int caretPos = stc->PositionFromLine(lineno);
    stc->SetSelection(caretPos, caretPos);
    stc->SetCurrentPos(caretPos);
    stc->EnsureCaretVisible();
    editor->CenterLine(lineno);
}

void NodeJSDebugger::ClearDebuggerMarker()
{
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
    std::for_each(
        editors.begin(), editors.end(), [&](IEditor* editor) { editor->GetCtrl()->MarkerDeleteAll(smt_indicator); });
}

void NodeJSDebugger::OnHighlightLine(clDebugEvent& event)
{
    event.Skip();
    int line = event.GetInt();
    wxString file = event.GetFileName();

    ClearDebuggerMarker();
    bool res = clGetManager()->OpenFile(file);
    if(res) {
        IEditor* activeEditor = clGetManager()->GetActiveEditor();
        if(activeEditor && (activeEditor->GetFileName().GetFullPath() == file)) {
            SetDebuggerMarker(activeEditor, line - 1);
        }
    }
}

void NodeJSDebugger::OnSelectFrame(clDebugEvent& event)
{
    event.Skip();
    SelectFrame(event.GetInt());
}
