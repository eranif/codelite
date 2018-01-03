#include "NodeJSCallstackHandler.h"
#include "NodeJSContinueHandler.h"
#include "NodeJSDebugger.h"
#include "NodeJSDebuggerDlg.h"
#include "NodeJSDebuggerTooltip.h"
#include "NodeJSEvaluateExprHandler.h"
#include "NodeJSEvents.h"
#include "NodeJSGetScriptHandler.h"
#include "NodeJSLookupHandler.h"
#include "NodeJSSelectFrameHandler.h"
#include "NodeJSSetBreakpointHandler.h"
#include "NodeJSWorkspaceUserConfiguration.h"
#include "NoteJSWorkspace.h"
#include "asyncprocess.h"
#include "bookmark_manager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "macros.h"
#include "processreaderthread.h"
#include <algorithm>
#include <signal.h>
#include <wx/log.h>
#include <wx/msgdlg.h>

#define CHECK_RUNNING() \
    if(!IsConnected()) return

NodeJSDebugger::NodeJSDebugger()
    : m_canInteract(false)
    , m_tooltip(NULL)
{
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_START, &NodeJSDebugger::OnDebugStart, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_CONTINUE, &NodeJSDebugger::OnDebugContinue, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STOP, &NodeJSDebugger::OnStopDebugger, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_IS_RUNNING, &NodeJSDebugger::OnDebugIsRunning, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, &NodeJSDebugger::OnToggleBreakpoint, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_NEXT, &NodeJSDebugger::OnDebugNext, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_NEXT_INST, &NodeJSDebugger::OnVoid, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STEP_IN, &NodeJSDebugger::OnDebugStepIn, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STEP_I, &NodeJSDebugger::OnVoid, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STEP_OUT, &NodeJSDebugger::OnDebugStepOut, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_EXPR_TOOLTIP, &NodeJSDebugger::OnTooltip, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_CAN_INTERACT, &NodeJSDebugger::OnCanInteract, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_ATTACH_TO_PROCESS, &NodeJSDebugger::OnAttach, this);

    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &NodeJSDebugger::OnWorkspaceOpened, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &NodeJSDebugger::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_MARK_LINE, &NodeJSDebugger::OnHighlightLine, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_EVAL_EXPRESSION, &NodeJSDebugger::OnEvalExpression, this);

    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &NodeJSDebugger::OnEditorChanged, this);

    Bind(wxEVT_TOOLTIP_DESTROY, &NodeJSDebugger::OnDestroyTip, this);
    m_node.Bind(wxEVT_TERMINAL_COMMAND_EXIT, &NodeJSDebugger::OnNodeTerminated, this);
    m_node.Bind(wxEVT_TERMINAL_COMMAND_OUTPUT, &NodeJSDebugger::OnNodeOutput, this);
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
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STEP_I, &NodeJSDebugger::OnVoid, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STEP_OUT, &NodeJSDebugger::OnDebugStepOut, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_EXPR_TOOLTIP, &NodeJSDebugger::OnTooltip, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_CAN_INTERACT, &NodeJSDebugger::OnCanInteract, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_ATTACH_TO_PROCESS, &NodeJSDebugger::OnAttach, this);

    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &NodeJSDebugger::OnWorkspaceOpened, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &NodeJSDebugger::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_MARK_LINE, &NodeJSDebugger::OnHighlightLine, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_EVAL_EXPRESSION, &NodeJSDebugger::OnEvalExpression, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &NodeJSDebugger::OnEditorChanged, this);

    m_node.Unbind(wxEVT_TERMINAL_COMMAND_EXIT, &NodeJSDebugger::OnNodeTerminated, this);
    m_node.Unbind(wxEVT_TERMINAL_COMMAND_OUTPUT, &NodeJSDebugger::OnNodeOutput, this);
    Unbind(wxEVT_TOOLTIP_DESTROY, &NodeJSDebugger::OnDestroyTip, this);

    m_node.Terminate();

    m_bptManager.Save();
    DoDeleteTempFiles(m_tempFiles);
    m_tempFiles.clear();

    if(m_tooltip) {
        m_tooltip->Destroy();
        m_tooltip = NULL;
    }

    // fire stop event (needed to reload the normal layout)
    clDebugEvent event(wxEVT_NODEJS_DEBUGGER_STOPPED);
    EventNotifier::Get()->AddPendingEvent(event);

    // Clear all markers
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
    std::for_each(editors.begin(), editors.end(), [&](IEditor* e) { e->DelAllCompilerMarkers(); });
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

    event.SetFeatures(0); // No special features by the NodeJS debugger

    // Our to handle
    event.Skip(false);
    if(m_socket && m_socket->IsConnected()) {
        Continue();
        return;
    };

    NodeJSDebuggerDlg dlg(EventNotifier::Get()->TopFrame(), NodeJSDebuggerDlg::kDebug);
    if(dlg.ShowModal() != wxID_OK) { return; }

    StartDebugger(dlg.GetCommand(), dlg.GetWorkingDirectory());
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
    m_node.Terminate();
#if defined(__WXGTK__) || defined(__WXOSX__)
    ConnectionLost("Debug session stopped");
#endif
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
                if(IsConnected()) { SetBreakpoint(bp); }
            }

            // Update the UI
            m_bptManager.SetBreakpoints(editor);
            clDebugEvent event(wxEVT_NODEJS_DEBUGGER_UPDATE_BREAKPOINTS_VIEW);
            EventNotifier::Get()->AddPendingEvent(event);
        }
    }
}

void NodeJSDebugger::OnTooltip(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
    event.Skip(false);

    CHECK_PTR_RET(clGetManager()->GetActiveEditor());

    wxString selection = event.GetString();
    CHECK_COND_RET(!selection.IsEmpty());

    // Build the request
    JSONElement request = JSONElement::createObject();
    request.addProperty("type", "request");
    request.addProperty("command", "evaluate");
    JSONElement args = JSONElement::createObject("arguments");
    request.append(args);
    args.addProperty("expression", selection);

    // Write the command
    m_socket->WriteRequest(request, new NodeJSEvaluateExprHandler(selection));
}

void NodeJSDebugger::OnVoid(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
    event.Skip(false);
    wxUnusedVar(event);
}

bool NodeJSDebugger::IsConnected() { return m_socket && m_socket->IsConnected(); }

void NodeJSDebugger::ConnectionEstablished()
{
    clDebugEvent eventStart(wxEVT_NODEJS_DEBUGGER_STARTED);
    eventStart.SetDebuggerName("Node.js");
    EventNotifier::Get()->AddPendingEvent(eventStart);
}

void NodeJSDebugger::ConnectionLost(const wxString& errmsg)
{
    CL_DEBUG(errmsg);
#ifdef __WXGTK__
#if 0
    if(errmsg != "Debug session stopped") {
        ::wxMessageBox(_("Node.js debugger disconnected unexpectedly\nYou might want to check the console to see if "
                         "there are any useful messages"),
                       _("CodeLite"),
                       wxICON_WARNING | wxOK_DEFAULT | wxCENTER);
    }
#endif
#else
    m_node.Terminate();
    m_socket.Reset(NULL);

    clDebugEvent event(wxEVT_NODEJS_DEBUGGER_STOPPED);
    event.SetDebuggerName("Node.js");
    EventNotifier::Get()->AddPendingEvent(event);
#endif

    // Clear the debugger markers
    ClearDebuggerMarker();
}

void NodeJSDebugger::OnNodeOutput(clCommandEvent& event)
{
    wxUnusedVar(event);
    CL_DEBUG("Node debugger:\n%s", event.GetString());

    clDebugEvent eventLog(wxEVT_NODEJS_DEBUGGER_CONSOLE_LOG);
    eventLog.SetString(event.GetString());
    EventNotifier::Get()->AddPendingEvent(eventLog);
}

void NodeJSDebugger::OnNodeTerminated(clCommandEvent& event)
{
    wxUnusedVar(event);
    EventNotifier::Get()->TopFrame()->Raise();

    // Restart the network thread
    wxBusyCursor bc;
#ifndef __WXGTK__
    m_socket.Reset(NULL);
#else
    m_node.Terminate();
    m_socket.Reset(NULL);

    clDebugEvent eventStop(wxEVT_NODEJS_DEBUGGER_STOPPED);
    eventStop.SetDebuggerName("Node.js");
    EventNotifier::Get()->AddPendingEvent(eventStop);

    // Clear the debugger markers
    ClearDebuggerMarker();
#endif
}

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
    if(requestBacktrace) { Callstack(); }
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
    m_socket->WriteRequest(request, new NodeJSSelectFrameHandler());
}

void NodeJSDebugger::SetCanInteract(bool canInteract)
{
    clDebugEvent event(canInteract ? wxEVT_NODEJS_DEBUGGER_CAN_INTERACT : wxEVT_NODEJS_DEBUGGER_LOST_INTERACT);
    EventNotifier::Get()->ProcessEvent(event);
    this->m_canInteract = canInteract;
    if(!canInteract) { ClearDebuggerMarker(); }
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
#ifdef __WXOSX__
    stc->Refresh();
#endif
}

void NodeJSDebugger::ClearDebuggerMarker()
{
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
    std::for_each(editors.begin(), editors.end(),
                  [&](IEditor* editor) { editor->GetCtrl()->MarkerDeleteAll(smt_indicator); });
}

void NodeJSDebugger::DoHighlightLine(const wxString& filename, int lineNo)
{
    IEditor* activeEditor = clGetManager()->OpenFile(filename, "", lineNo - 1);
    if(activeEditor) { SetDebuggerMarker(activeEditor, lineNo - 1); }
}

void NodeJSDebugger::OnHighlightLine(clDebugEvent& event)
{
    event.Skip();
    int line = event.GetLineNumber();
    wxFileName file = event.GetFileName();

    ClearDebuggerMarker();
    if(file.Exists()) {
        CallAfter(&NodeJSDebugger::DoHighlightLine, file.GetFullPath(), line);

    } else {
        // Probably a node.js internal file
        wxFileName fn(clStandardPaths::Get().GetUserDataDir(), file.GetFullPath());
        fn.AppendDir("webtools");
        fn.AppendDir("tmp");
        if(fn.Exists()) {
            CallAfter(&NodeJSDebugger::DoHighlightLine, fn.GetFullPath(), line);
        } else {
            // Ask the file from nodejs
            GetCurrentFrameSource(file.GetFullPath(), line);
        }
    }
}

void NodeJSDebugger::ExceptionThrown(const NodeJSDebuggerException& exc)
{
    // Switch to the 'Console' view
    clDebugEvent event(wxEVT_NODEJS_DEBUGGER_EXCEPTION_THROWN);
    event.SetFileName(exc.script);
    event.SetLineNumber(exc.line);
    event.SetString(exc.message);
    event.SetInt(exc.column);
    EventNotifier::Get()->AddPendingEvent(event);
}

void NodeJSDebugger::ConnectError(const wxString& errmsg)
{
    ::wxMessageBox(wxString::Format(_("Failed to connect to Node.js debugger:\n'%s'"), errmsg), "CodeLite",
                   wxOK | wxICON_ERROR | wxCENTER);
    m_socket.Reset(NULL);
}

void NodeJSDebugger::BreakOnException(bool b)
{
    // Sanity
    if(!IsConnected()) return;

    // Build the request
    JSONElement request = JSONElement::createObject();
    request.addProperty("type", "request");
    request.addProperty("command", "setexceptionbreak");

    JSONElement args = JSONElement::createObject("arguments");
    request.append(args);
    args.addProperty("type", "uncaught");
    args.addProperty("enabled", b);

    // Write the command
    m_socket->WriteRequest(request, NULL);
}

void NodeJSDebugger::GetCurrentFrameSource(const wxString& filename, int line)
{
    // Sanity
    if(!IsConnected()) return;

    // Build the request
    JSONElement request = JSONElement::createObject();
    request.addProperty("type", "request");
    request.addProperty("command", "source");

    // Write the command
    m_socket->WriteRequest(request, new NodeJSGetScriptHandler(filename, line));
}

void NodeJSDebugger::OnEditorChanged(wxCommandEvent& event)
{
    event.Skip();
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);

    wxStringSet_t tmpFiles = m_tempFiles;
    wxStringSet_t closedTempEditors;
    // Loop over the temp files list
    std::for_each(tmpFiles.begin(), tmpFiles.end(), [&](const wxString& filename) {
        // If the temp file does not match one of the editors, assume it was closed and delete
        // the temporary file
        IEditor::List_t::iterator iter = std::find_if(editors.begin(), editors.end(), [&](IEditor* editor) {
            if(editor->GetFileName().GetFullPath() == filename) return true;
            return false;
        });
        if(iter == editors.end()) {
            closedTempEditors.insert(filename);
            m_tempFiles.erase(filename);
        }
    });

    if(!closedTempEditors.empty()) { DoDeleteTempFiles(closedTempEditors); }
}

void NodeJSDebugger::DoDeleteTempFiles(const wxStringSet_t& files)
{
    std::for_each(files.begin(), files.end(), [&](const wxString& filename) {
        wxLogNull noLog;
        clRemoveFile(filename);
    });
}

void NodeJSDebugger::OnEvalExpression(clDebugEvent& event)
{
    event.Skip();

    // Build the request
    JSONElement request = JSONElement::createObject();
    request.addProperty("type", "request");
    request.addProperty("command", "evaluate");
    JSONElement args = JSONElement::createObject("arguments");
    request.append(args);
    args.addProperty("expression", event.GetString());

    // Write the command
    m_socket->WriteRequest(request, new NodeJSEvaluateExprHandler(event.GetString(), kNodeJSContextConsole));
}

void NodeJSDebugger::Lookup(const std::vector<int>& handles, eNodeJSContext context)
{
    JSONElement request = JSONElement::createObject();
    request.addProperty("type", "request");
    request.addProperty("command", "lookup");

    JSONElement args = JSONElement::createObject("arguments");
    request.append(args);

    JSONElement arrHandles = JSONElement::createArray("handles");
    args.append(arrHandles);

    for(size_t i = 0; i < handles.size(); ++i) {
        arrHandles.arrayAppend(JSONElement("", handles.at(i), cJSON_Number));
    }

    // Write the command
    m_socket->WriteRequest(request, new NodeJSLookupHandler(context));
}

void NodeJSDebugger::ShowTooltip(const wxString& expression, const wxString& jsonOutput)
{
    if(m_tooltip) {
        m_tooltip->Destroy();
        m_tooltip = NULL;
    }

    m_tooltip = new NodeJSDebuggerTooltip(this, expression);
    m_tooltip->ShowTip(jsonOutput);
}

void NodeJSDebugger::OnDestroyTip(clCommandEvent& event)
{
    if(m_tooltip) {
        m_tooltip->Destroy();
        m_tooltip = NULL;
    }
}

void NodeJSDebugger::OnAttach(clDebugEvent& event)
{
#ifdef __WXMSW__
    if(event.GetDebuggerName() != "NodeJS Debugger") {
        event.Skip();
        return;
    }
    event.Skip(false);
    ::wxMessageBox(_("Debugging a running Node.js process is only available on Linux / OSX"), "CodeLite",
                   wxICON_WARNING | wxCENTER | wxOK);
#else

    if(event.GetDebuggerName() != "NodeJS Debugger") {
        event.Skip();
        return;
    }

    event.Skip(false); // ours to handle, stop the event chain

    if(m_socket && m_socket->IsConnected()) {
        ::wxMessageBox(_("An active debug session is already running"), "CodeLite", wxICON_WARNING | wxCENTER | wxOK);
        return;
    };

    ::kill(event.GetInt(), SIGUSR1);
    // already connected?
    m_socket.Reset(new NodeJSSocket(this));
    m_socket->Connect("127.0.0.1", 5858);
#endif
}

void NodeJSDebugger::StartDebugger(const wxString& command, const wxString& workingDirectory)
{
    if(!m_node.ExecuteConsole(command, workingDirectory, true, command)) {
        ::wxMessageBox(_("Failed to start NodeJS application"), "CodeLite", wxOK | wxICON_ERROR | wxCENTER);
        m_socket.Reset(NULL);
    }

    // already connected?
    m_socket.Reset(new NodeJSSocket(this));
    NodeJSWorkspaceUser userConf(NodeJSWorkspace::Get()->GetFilename().GetFullPath());
    userConf.Load();
    m_socket->Connect("127.0.0.1", userConf.GetDebuggerPort());
}
