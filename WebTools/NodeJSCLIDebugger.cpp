#include "NodeJSCLIDebugger.h"
#include "NodeJSDebuggerDlg.h"
#include "NoteJSWorkspace.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "macros.h"
#include "processreaderthread.h"
#include <wx/msgdlg.h>
#include "NodeJSEvents.h"

#define CHECK_RUNNING() \
    if(!IsRunning()) { return; }

NodeJSCLIDebugger::NodeJSCLIDebugger()
{
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_START, &NodeJSCLIDebugger::OnDebugStart, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_CONTINUE, &NodeJSCLIDebugger::OnDebugContinue, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STOP, &NodeJSCLIDebugger::OnStopDebugger, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_IS_RUNNING, &NodeJSCLIDebugger::OnDebugIsRunning, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &NodeJSCLIDebugger::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &NodeJSCLIDebugger::OnProcessTerminated, this);
}

NodeJSCLIDebugger::~NodeJSCLIDebugger()
{
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_START, &NodeJSCLIDebugger::OnDebugStart, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_CONTINUE, &NodeJSCLIDebugger::OnDebugContinue, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STOP, &NodeJSCLIDebugger::OnStopDebugger, this);
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
}

bool NodeJSCLIDebugger::IsRunning() const { return m_process != NULL; }

void NodeJSCLIDebugger::DoCleanup()
{
    m_debuggerOutput.Clear();
    m_commands.clear();
    m_canInteract = false;
    m_workingDirectory.Clear();
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
}

void NodeJSCLIDebugger::OnProcessOutput(clProcessEvent& event)
{
    wxString str = event.GetOutput();
    for(size_t i = 0; i < str.size(); ++i) {
        wxChar ch = str[i];
        switch(ch) {
        case '\r': {
            // Unwind back and delete everything until the first '\n' we find or start of string
            size_t where = m_debuggerOutput.rfind('\n');
            if(where != wxString::npos) {
                m_debuggerOutput.Truncate(where + 1);
            } else {
                m_debuggerOutput.Clear();
            }
        } break;
        case '\b': {
            if(!m_debuggerOutput.IsEmpty() && (m_debuggerOutput.Last() != '\n')) {
                // remove last char
                m_debuggerOutput.Truncate(m_debuggerOutput.size() - 1);
            }

        } break;
        case '\n': {
            m_debuggerOutput << ch;

        } break;
        default: {
            m_debuggerOutput << ch;

        } break;
        }
    }
    const char* p = m_debuggerOutput.mb_str(wxConvUTF8).data();
    wxUnusedVar(p);
    if(m_debuggerOutput.EndsWith("\ndebug>") || m_debuggerOutput.EndsWith("\ndebug> ")) {
        if(!m_commands.empty() && m_commands.front().in_progress) {

            // the output from the last command belongs to this command, let it handle it
            const NodeJSCliCommandHandler& handler = m_commands.front();
            if(handler.action) { handler.action(m_debuggerOutput); }

            // remove the first entry from the command queue
            m_commands.erase(m_commands.begin());
        }
        m_debuggerOutput.Clear();
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
    eventStart.SetDebuggerName("Node.js - CLI");
    EventNotifier::Get()->AddPendingEvent(eventStart);

    // request the current backtrace
    Callstack();
}

void NodeJSCLIDebugger::Callstack()
{
    NodeJSCliCommandHandler commandHandler("bt", [&](const wxString& outputString) {
        clDEBUG() << "Callstack handler:[\n" << outputString << "\n]";
    });
    PushCommand(commandHandler);
}

void NodeJSCLIDebugger::ProcessQueue()
{
    if(!m_commands.empty() && IsCanInteract()) {
        NodeJSCliCommandHandler& command = m_commands.front();
        if(command.in_progress) { return; }
        clDEBUG() << "NodeJSCLIDebugger:" << command.m_commandText;
        m_process->Write(command.m_commandText);
        command.in_progress = true;
    }
}

void NodeJSCLIDebugger::PushCommand(const NodeJSCliCommandHandler& commandHandler)
{
    m_commands.push_back(commandHandler);
    ProcessQueue();
}

bool NodeJSCLIDebugger::IsCanInteract() const { return m_process && m_canInteract; }
