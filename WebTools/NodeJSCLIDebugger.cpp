#include "NodeJSCLIDebugger.h"
#include "NodeJSDebuggerDlg.h"
#include "NodeJSEvents.h"
#include "NoteJSWorkspace.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "macros.h"
#include "processreaderthread.h"
#include <wx/msgdlg.h>

#define NODE_CLI_DEBUGGER_NAME "Node.js - CLI"
#define CHECK_RUNNING() \
    if(!IsRunning()) { return; }

NodeJSCLIDebugger::NodeJSCLIDebugger()
{
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_START, &NodeJSCLIDebugger::OnDebugStart, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_CONTINUE, &NodeJSCLIDebugger::OnDebugContinue, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STOP, &NodeJSCLIDebugger::OnStopDebugger, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_NEXT, &NodeJSCLIDebugger::OnDebugNext, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_IS_RUNNING, &NodeJSCLIDebugger::OnDebugIsRunning, this);
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
        wxString oneliner = command;
        if(!command_args.IsEmpty()) { oneliner << " " << command_args; }
        StartDebugger(oneliner, dlg.GetWorkingDirectory());

    } else {
        // TODO: continue
    }
}

void NodeJSCLIDebugger::OnDebugContinue(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
}

void NodeJSCLIDebugger::OnStopDebugger(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();

    // Terminate the process
    m_process->Terminate();
}

void NodeJSCLIDebugger::OnDebugNext(clDebugEvent& event)
{
    event.Skip();
    CHECK_RUNNING();
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
    wxString processOutput = event.GetOutput();
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
    if(processOutput.EndsWith("\ndebug>") || processOutput.EndsWith("\ndebug> ")) {
        m_canInteract = true;
        ProcessQueue(); // Process the next command
    }
}

void NodeJSCLIDebugger::StartDebugger(const wxString& command, const wxString& workingDirectory)
{
    size_t createFlags = IProcessCreateDefault;
    m_process = ::CreateAsyncProcess(this, command, createFlags, workingDirectory);
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
