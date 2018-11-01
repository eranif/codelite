#include "NodeJSCLIDebugger.h"
#include "NodeJSDebuggerDlg.h"
#include "NodeJSEvents.h"
#include "NoteJSWorkspace.h"
#include "clConsoleBase.h"
#include "codelite_events.h"
#include "dirsaver.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "macros.h"
#include "processreaderthread.h"
#include <wx/msgdlg.h>

#define NODE_CLI_DEBUGGER_NAME "Node.js - CLI"

#define CHECK_RUNNING() \
    if(!IsRunning()) { return; }

#define CHECK_SHOULD_HANDLE(evt)                      \
    evt.Skip();                                       \
    if(!IsRunning()) { return; }                      \
    if(!NodeJSWorkspace::Get()->IsOpen()) { return; } \
    evt.Skip(false);

NodeJSCLIDebugger::NodeJSCLIDebugger()
{
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_START, &NodeJSCLIDebugger::OnDebugStart, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_CONTINUE, &NodeJSCLIDebugger::OnDebugContinue, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STOP, &NodeJSCLIDebugger::OnStopDebugger, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_NEXT, &NodeJSCLIDebugger::OnDebugNext, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_IS_RUNNING, &NodeJSCLIDebugger::OnDebugIsRunning, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, &NodeJSCLIDebugger::OnToggleBreakpoint, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &NodeJSCLIDebugger::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &NodeJSCLIDebugger::OnProcessTerminated, this);
}

NodeJSCLIDebugger::~NodeJSCLIDebugger()
{
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_START, &NodeJSCLIDebugger::OnDebugStart, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_CONTINUE, &NodeJSCLIDebugger::OnDebugContinue, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STOP, &NodeJSCLIDebugger::OnStopDebugger, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_NEXT, &NodeJSCLIDebugger::OnDebugNext, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_IS_RUNNING, &NodeJSCLIDebugger::OnDebugIsRunning, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, &NodeJSCLIDebugger::OnToggleBreakpoint, this);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &NodeJSCLIDebugger::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &NodeJSCLIDebugger::OnProcessTerminated, this);
}

void NodeJSCLIDebugger::OnDebugStart(clDebugEvent& event)
{
    event.Skip();
    CHECK_COND_RET(NodeJSWorkspace::Get()->IsOpen());

    event.SetFeatures(0); // No special features by the NodeJS debugger

    // Ours to handle
    event.Skip(false);

    if(!IsRunning()) {
        NodeJSDebuggerDlg dlg(EventNotifier::Get()->TopFrame(), NodeJSDebuggerDlg::kDebugCLI);
        if(dlg.ShowModal() != wxID_OK) { return; }

        wxString command;
        wxString command_args;
        dlg.GetCommand(command, command_args);
        StartDebugger(command, command_args, dlg.GetWorkingDirectory());

    } else {
        OnDebugContinue(event);
    }
}

void NodeJSCLIDebugger::OnDebugContinue(clDebugEvent& event)
{
    CHECK_SHOULD_HANDLE(event);

    NodeJSCliCommandHandler commandHandler("cont", GetCommandId(), [&](const wxString& outputString) {
        // trigger a 'Callstack' call
        Callstack();
    });
    PushCommand(commandHandler);
}

void NodeJSCLIDebugger::OnStopDebugger(clDebugEvent& event)
{
    CHECK_SHOULD_HANDLE(event);

    // Terminate the process
    m_process->Terminate();
}

void NodeJSCLIDebugger::OnDebugNext(clDebugEvent& event)
{
    CHECK_SHOULD_HANDLE(event);
    NodeJSCliCommandHandler commandHandler("next", GetCommandId(), [&](const wxString& outputString) {
        // trigger a 'Callstack' call
        Callstack();
    });
    PushCommand(commandHandler);
}

bool NodeJSCLIDebugger::IsRunning() const { return m_process != NULL; }

void NodeJSCLIDebugger::DoCleanup()
{
    m_commands.clear();
    m_canInteract = false;
    m_workingDirectory.Clear();
    m_waitingReplyCommands.clear();
    m_outputParser.Clear();
    m_bufferedOutput.clear();
    if(m_process) { m_process->Terminate(); }
}

void NodeJSCLIDebugger::OnDebugIsRunning(clDebugEvent& event)
{
    if(IsRunning()) {
        event.SetAnswer(true);

    } else {
        event.Skip();
    }
}

void NodeJSCLIDebugger::OnProcessTerminated(clProcessEvent& event)
{
    wxUnusedVar(event);
    wxDELETE(m_process);

    clDebugEvent e(wxEVT_NODEJS_CLI_DEBUGGER_STOPPED);
    e.SetDebuggerName(NODE_CLI_DEBUGGER_NAME);
    EventNotifier::Get()->AddPendingEvent(e);
    DoCleanup();
}

void NodeJSCLIDebugger::OnProcessOutput(clProcessEvent& event)
{
    m_bufferedOutput << event.GetOutput();
    clDEBUG() << m_bufferedOutput;

    // We process everyting if it ends with LF
    wxString processOutput = m_bufferedOutput.BeforeLast('\n');
    m_bufferedOutput = m_bufferedOutput.AfterLast('\n');
    m_outputParser.ParseOutput(processOutput);
    if(m_outputParser.HasResults()) {
        auto result = m_outputParser.TakeResult();
        long commandId = result.first;
        wxString output = result.second;
        auto commandHandler = m_waitingReplyCommands.find(commandId);
        if(commandHandler != m_waitingReplyCommands.end()) {
            if(commandHandler->second.action) { commandHandler->second.action(output); }
        }
    }
#ifdef __WXGTK__
    m_canInteract = true;
    ProcessQueue(); // Process the next command
#else
    if(processOutput.EndsWith("debug>") || processOutput.EndsWith("debug> ")) {
        m_canInteract = true;
        ProcessQueue(); // Process the next command
    }
#endif
}

void NodeJSCLIDebugger::StartDebugger(const wxString& command, const wxString& command_args,
                                      const wxString& workingDirectory)
{
    size_t createFlags = IProcessCreateDefault | IProcessDontStripTerminalColours;
    wxString cmd = command;
    if(!command_args.empty()) { cmd << " " << command_args; }
    m_process = ::CreateAsyncProcess(this, cmd, createFlags, workingDirectory);
    if(!m_process) {
        ::wxMessageBox(wxString() << _("Failed to launch NodeJS: ") << command);
        DoCleanup();
        return;
    }

    // Keep the working directory
    m_workingDirectory = workingDirectory;

    clDebugEvent eventStart(wxEVT_NODEJS_CLI_DEBUGGER_STARTED);
    eventStart.SetDebuggerName(NODE_CLI_DEBUGGER_NAME);
    EventNotifier::Get()->AddPendingEvent(eventStart);

    // Apply all breakpoints
    ApplyAllBerakpoints();

    // request the current backtrace
    Callstack();
}

void NodeJSCLIDebugger::Callstack()
{
    NodeJSCliCommandHandler commandHandler("bt", GetCommandId(), [&](const wxString& outputString) {
        clDebugEvent e(wxEVT_NODEJS_CLI_DEBUGGER_UPDATE_CALLSTACK);
        e.SetString(outputString);
        EventNotifier::Get()->AddPendingEvent(e);
    });
    PushCommand(commandHandler);
}

void NodeJSCLIDebugger::ProcessQueue()
{
    if(!m_commands.empty() && IsCanInteract()) {
        NodeJSCliCommandHandler& command = m_commands.front();
        clDEBUG() << "-->#" << command.m_commandID << ":" << command.m_commandText;
        m_process->Write(command.m_commandText);
        if(command.action) { m_waitingReplyCommands.insert({ command.m_commandID, command }); }
        m_commands.erase(m_commands.begin());
    }
}

static long commandID = 0;

void NodeJSCLIDebugger::PushCommand(const NodeJSCliCommandHandler& commandHandler)
{
    m_commands.push_back({ wxString() << "console.log(\"#start_command_" << commandHandler.m_commandID << "\")",
                           commandHandler.m_commandID, nullptr });
    m_commands.push_back(commandHandler);
    m_commands.push_back({ wxString() << "console.log(\"#end_command_" << commandHandler.m_commandID << "\")",
                           commandHandler.m_commandID, nullptr });
    ++commandID;
    ProcessQueue();
}

bool NodeJSCLIDebugger::IsCanInteract() const { return m_process && m_canInteract; }

long NodeJSCLIDebugger::GetCommandId() const { return commandID; }

void NodeJSCLIDebugger::OnToggleBreakpoint(clDebugEvent& event)
{
    event.Skip();
    if(!NodeJSWorkspace::Get()->IsOpen()) { return; }
    event.Skip(false);
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(IsRunning()) {
        if(editor && (editor->GetFileName().GetFullPath() == event.GetFileName())) {
            // Correct editor
            // add marker
            NodeJSBreakpoint bp = m_bptManager.GetBreakpoint(event.GetFileName(), event.GetInt());
            if(bp.IsOk()) {
                DeleteBreakpoint(bp);
            } else {
                SetBreakpoint(event.GetFileName(), event.GetInt());
            }
            // Update the UI
        }
    } else {

        NodeJSBreakpoint bp = m_bptManager.GetBreakpoint(event.GetFileName(), event.GetInt());
        if(bp.IsOk()) {
            m_bptManager.DeleteBreakpoint(event.GetFileName(), event.GetInt());
        } else {
            m_bptManager.AddBreakpoint(event.GetFileName(), event.GetInt());
        }
    }
    if(editor) { m_bptManager.SetBreakpoints(editor); }
}

void NodeJSCLIDebugger::SetBreakpoint(const wxFileName& file, int lineNumber, bool useVoidHandler)
{
    // We have no breakpoint on this file/line (yet)
    m_bptManager.AddBreakpoint(file, lineNumber);
    const NodeJSBreakpoint& bp = m_bptManager.GetBreakpoint(file, lineNumber);
    if(!bp.IsOk()) { return; }

    // NodeJS debugger likes his file path in a certain format (relative, all backslashes escaped)
    wxString file_path = GetBpRelativeFilePath(bp);

    wxString command;
    command << "setBreakpoint(\"" << file_path << "\"," << bp.GetLine() << ")";
    CommandHandlerFunc_t callback = nullptr;
    if(!useVoidHandler) {
        callback = [&](const wxString& outputString) {
            wxUnusedVar(outputString);
            ListBreakpoints();
        };
    }
    NodeJSCliCommandHandler commandHandler(command, GetCommandId(), callback);
    PushCommand(commandHandler);
}

void NodeJSCLIDebugger::DeleteBreakpoint(const NodeJSBreakpoint& bp)
{
    if(!bp.IsOk()) { return; }

    wxString command;
    wxString file_path = GetBpRelativeFilePath(bp);
    command << "clearBreakpoint(\"" << file_path << "\"," << bp.GetLine() << ")";

    // Tell Node to remove this breakpoint
    NodeJSCliCommandHandler commandHandler(command, GetCommandId(), [&](const wxString& outputString) {
        wxUnusedVar(outputString);
        ListBreakpoints();
    });
    PushCommand(commandHandler);
}

void NodeJSCLIDebugger::ListBreakpoints()
{
    // Tell Node to remove this breakpoint
    NodeJSCliCommandHandler commandHandler("breakpoints", GetCommandId(), [&](const wxString& outputString) {
        // Update the UI with the list of breakpoints we got so far
        clDebugEvent e(wxEVT_NODEJS_CLI_DEBUGGER_UPDATE_BREAKPOINTS_VIEW);
        e.SetString(outputString);
        EventNotifier::Get()->AddPendingEvent(e);
    });
    PushCommand(commandHandler);
}

wxString NodeJSCLIDebugger::GetBpRelativeFilePath(const NodeJSBreakpoint& bp) const
{
    wxFileName fn(bp.GetFilename());
    fn.MakeRelativeTo(GetWorkingDirectory());
    wxString file_path = fn.GetFullPath();

    // We need to escapte backslashes, otherwise, it wont work...
    file_path.Replace("\\", "\\\\");
    return file_path;
}

void NodeJSCLIDebugger::ApplyAllBerakpoints()
{
    const NodeJSBreakpoint::List_t& breakpoints = m_bptManager.GetBreakpoints();
    std::for_each(breakpoints.begin(), breakpoints.end(),
                  [&](const NodeJSBreakpoint& bp) { SetBreakpoint(bp.GetFilename(), bp.GetLine(), true); });

    // Now that we have applied all the breapoints, we can ask for list of the actual breakpoints from the debugger
    ListBreakpoints();
}

///===------------------------------------------------------------------------------------------
///===------------------------------------------------------------------------------------------
void NodeProcess::OnTerminate(int pid, int status)
{
    // Notify the debugger the process terminated
}

void NodeProcess::Start(const wxString& command, const wxString& workingDirectory)
{
    DirSaver ds;
    ::wxSetWorkingDirectory(workingDirectory);
    m_pid = wxExecute(command, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER, this);
    m_timer = new wxTimer(this, wxID_ANY);
    Bind(wxEVT_TIMER, &NodeProcess::OnTimer, this, m_timer->GetId());
    m_timer->Start(50, false);
}

void NodeProcess::OnTimer(wxTimerEvent& event) {}

void NodeProcess::Terminate()
{
    if(m_pid != wxNOT_FOUND) {
        Unbind(wxEVT_TIMER, &NodeProcess::OnTimer, this, m_timer->GetId());
        m_timer->Stop();
        wxDELETE(m_timer);
        clKill(m_pid, wxSIGKILL, true);
    }
}

NodeProcess::~NodeProcess() { Terminate(); }
