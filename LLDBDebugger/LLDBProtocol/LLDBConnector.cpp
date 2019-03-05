//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBConnector.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "LLDBConnector.h"
#include "LLDBEvent.h"
#include "LLDBNetworkListenerThread.h"
#include "LLDBRemoteHandshakePacket.h"
#include "LLDBSettings.h"
#include "cl_standard_paths.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "JSON.h"
#include "processreaderthread.h"
#include <wx/filefn.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>

#ifndef __WXMSW__
#include <sys/wait.h>
#include <unistd.h>
#endif

LLDBConnector::LLDBConnector()
    : m_isRunning(false)
    , m_canInteract(false)
    , m_attachedToProcess(false)
    , m_goingDown(false)
{
    Bind(wxEVT_LLDB_EXITED, &LLDBConnector::OnLLDBExited, this);
    Bind(wxEVT_LLDB_STARTED, &LLDBConnector::OnLLDBStarted, this);

    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &LLDBConnector::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &LLDBConnector::OnProcessTerminated, this);
}

LLDBConnector::~LLDBConnector()
{
    StopDebugServer();
    Unbind(wxEVT_LLDB_EXITED, &LLDBConnector::OnLLDBExited, this);
    Unbind(wxEVT_LLDB_STARTED, &LLDBConnector::OnLLDBStarted, this);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &LLDBConnector::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &LLDBConnector::OnProcessTerminated, this);
    Cleanup();
}

bool LLDBConnector::ConnectToLocalDebugger(LLDBConnectReturnObject& ret, int timeout)
{

#ifndef __WXMSW__
    m_goingDown = false;
    clSocketClient* client = new clSocketClient();
    m_socket.reset(client);
    clDEBUG() << "Connecting to codelite-lldb on:" << GetDebugServerPath();

    long msTimeout = timeout * 1000;
    long retriesCount = msTimeout / 250; // We try every 250 ms to connect
    bool connected = false;
    for(long i = 0; i < retriesCount; ++i) {
        if(!client->ConnectLocal(GetDebugServerPath())) {
            wxThread::Sleep(250);
            continue;
        }
        connected = true;
        break;
    }

    if(!connected) { return false; }

    // Start the lldb event thread
    // and start a listener thread which will read replies
    // from codelite-lldb and convert them into LLDBEvent
    socket_t fd = m_socket->GetSocket();
    m_pivot.Clear();
    m_thread.reset(new LLDBNetworkListenerThread(this, m_pivot, fd));
    m_thread->Start();
    clDEBUG() << "Successfully connected to codelite-lldb";
    return true;
#else
    clWARNING() << "LLDB Debugger: can't connect to local debugger - this is not support on MSW";
    return false;
#endif
}

bool LLDBConnector::ConnectToRemoteDebugger(const wxString& ip, int port, LLDBConnectReturnObject& ret, int timeout)
{
    m_goingDown = false;
    m_socket.reset(NULL);
    clSocketClient* client = new clSocketClient();
    m_socket.reset(client);
    clDEBUG() << "Connecting to codelite-lldb on" << ip << ":" << port;

    try {
        bool would_block = false;
        if(!client->ConnectRemote(ip, port, would_block, true)) {
            // select the socket
            if(!would_block) {
                m_socket.reset(NULL);
                return false;
            }

            try {
                if(client->SelectWrite(timeout) == clSocketBase::kTimeout) {
                    m_socket.reset(NULL);
                    return false;
                }
            } catch(clSocketException& e) {
                clDEBUG() << "SelectWrite error:" << e.what();
            }
        }

        // Connected!
        // Read the negotiation packet (part of the protocol only when doing remote connection)
        wxString message;
        if(m_socket->ReadMessage(message, 2) != clSocketBase::kSuccess) {
            m_socket.reset(NULL);
            return false;
        }

        LLDBRemoteHandshakePacket handshake(message);
        ret.SetRemoteHostName(handshake.GetHost());
        ret.SetPivotNeeded(handshake.GetHost() != ::wxGetHostName());

    } catch(clSocketException& e) {
        clWARNING() << "LLDBConnector::ConnectToRemoteDebugger:" << e.what();
        m_socket.reset(NULL);
        return false;
    }
    clDEBUG() << "Successfully connected to codelite-lldb";
    return true;
}

void LLDBConnector::SendCommand(const LLDBCommand& command)
{
    try {
        if(m_socket) {
            // Convert local paths to remote paths if needed
            LLDBCommand updatedCommand = command;
            updatedCommand.UpdatePaths(m_pivot);
            wxString jsonCommand = updatedCommand.ToJSON().format();
            clDEBUG() << "Sending command to LLDB:";
            clDEBUG() << jsonCommand;
            m_socket->WriteMessage(jsonCommand);
        }

    } catch(clSocketException& e) {
        wxUnusedVar(e);
    }
}

void LLDBConnector::SendThreadCommand(const eCommandType commandType, const std::vector<int>& threadIds)
{
    if(IsCanInteract()) {
        LLDBCommand command;
        command.SetCommandType(commandType);
        command.SetThreadIds(threadIds);
        SendCommand(command);
    }
}

void LLDBConnector::InvalidateBreakpoints()
{
    // mark all the breakpoints as "not-applied" (id=-1)
    LLDBBreakpoint::Vec_t updatedList;

    for(size_t i = 0; i < m_breakpoints.size(); ++i) {
        m_breakpoints.at(i)->Invalidate();
        if(wxFileName::Exists(m_breakpoints.at(i)->GetFilename())) { updatedList.push_back(m_breakpoints.at(i)); }
    }
    // we keep only breakpoints with valid filename
    m_breakpoints.swap(updatedList);
    ClearBreakpointDeletionQueue();

    CL_DEBUG("codelite: InvalidateBreakpoints called");
    m_pendingDeletionBreakpoints.clear();
}

bool LLDBConnector::IsBreakpointExists(LLDBBreakpoint::Ptr_t bp) const
{
    return FindBreakpoint(bp) != m_breakpoints.end();
}

LLDBBreakpoint::Vec_t::iterator LLDBConnector::FindBreakpoint(LLDBBreakpoint::Ptr_t bp)
{
    LLDBBreakpoint::Vec_t::iterator iter = m_breakpoints.begin();
    for(; iter != m_breakpoints.end(); ++iter) {
        if((*iter)->SameAs(bp)) { return iter; }
    }
    return m_breakpoints.end();
}

LLDBBreakpoint::Vec_t::const_iterator LLDBConnector::FindBreakpoint(LLDBBreakpoint::Ptr_t bp) const
{
    LLDBBreakpoint::Vec_t::const_iterator iter = m_breakpoints.begin();
    for(; iter != m_breakpoints.end(); ++iter) {
        if((*iter)->SameAs(bp)) { return iter; }
    }
    return m_breakpoints.end();
}

void LLDBConnector::AddBreakpoint(LLDBBreakpoint::Ptr_t breakpoint, bool notify)
{
    if(!IsBreakpointExists(breakpoint)) {
        m_breakpoints.push_back(breakpoint);

        if(notify) {
            LLDBEvent event(wxEVT_LLDB_BREAKPOINTS_UPDATED);
            event.SetBreakpoints(GetAllBreakpoints());
            ProcessEvent(event);
        }
    }
}

void LLDBConnector::ApplyBreakpoints()
{
    if(!m_breakpoints.empty()) {

        if(IsCanInteract()) {
            LLDBCommand command;
            command.SetCommandType(kCommandApplyBreakpoints);
            command.SetBreakpoints(GetUnappliedBreakpoints());
            SendCommand(command);
            m_breakpoints.clear();

        } else {
            Interrupt(kInterruptReasonApplyBreakpoints);
        }
    }
}

void LLDBConnector::Continue()
{
    LLDBCommand command;
    command.SetCommandType(kCommandContinue);
    SendCommand(command);
}

void LLDBConnector::RunTo(const wxFileName& filename, int line)
{
    SendSingleBreakpointCommand(kCommandRunTo, filename, line);
}

void LLDBConnector::JumpTo(const wxFileName& filename, int line)
{
    SendSingleBreakpointCommand(kCommandJumpTo, filename, line);
}

void LLDBConnector::SendSingleBreakpointCommand(const eCommandType commandType, const wxFileName& filename,
                                                const int line)
{
    if(!IsCanInteract()) { return; }

    LLDBCommand lldbCommand;
    lldbCommand.SetCommandType(commandType);
    lldbCommand.SetBreakpoints(LLDBBreakpoint::Vec_t{ LLDBBreakpoint::Ptr_t(new LLDBBreakpoint(filename, line)) });
    SendCommand(lldbCommand);
}

void LLDBConnector::Stop()
{
    if(IsAttachedToProcess()) {
        Detach();

    } else {
        LLDBCommand command;
        command.SetCommandType(kCommandStop);
        SendCommand(command);
    }
}

void LLDBConnector::Detach()
{
    if(IsCanInteract()) {
        CL_DEBUG("Sending 'Detach' command");
        LLDBCommand command;
        command.SetCommandType(kCommandDetach);
        SendCommand(command);

    } else {
        Interrupt(kInterruptReasonDetaching);
    }
}

void LLDBConnector::MarkBreakpointForDeletion(LLDBBreakpoint::Ptr_t bp)
{
    if(!IsBreakpointExists(bp)) { return; }

    LLDBBreakpoint::Vec_t::iterator iter = FindBreakpoint(bp);

    // add the breakpoint to the pending deletion breakpoints
    bp->SetId((*iter)->GetId());
    m_pendingDeletionBreakpoints.push_back(bp);
    m_breakpoints.erase(iter);
}

void LLDBConnector::DeleteBreakpoints()
{
    if(IsCanInteract()) {
        CL_DEBUGS(wxString() << "codelite: deleting breakpoints (total of " << m_pendingDeletionBreakpoints.size()
                             << " breakpoints)");
        LLDBCommand command;
        command.SetCommandType(kCommandDeleteBreakpoint);
        command.SetBreakpoints(m_pendingDeletionBreakpoints);
        SendCommand(command);
        CL_DEBUGS(wxString() << "codelite: DeleteBreakpoints clear pending deletionbreakpoints queue");
        m_pendingDeletionBreakpoints.clear();

    } else {
        CL_DEBUG("codelite: interrupting codelite-lldb for kInterruptReasonDeleteBreakpoint");
        Interrupt(kInterruptReasonDeleteBreakpoint);
    }
}

void LLDBConnector::ClearBreakpointDeletionQueue()
{
    CL_DEBUGS(wxString() << "codelite: ClearBreakpointDeletionQueue called");
    m_pendingDeletionBreakpoints.clear();
}

void LLDBConnector::Next()
{
    LLDBCommand command;
    command.SetCommandType(kCommandNext);
    SendCommand(command);
}

void LLDBConnector::StepIn()
{
    LLDBCommand command;
    command.SetCommandType(kCommandStepIn);
    SendCommand(command);
}

void LLDBConnector::StepOut()
{
    LLDBCommand command;
    command.SetCommandType(kCommandStepOut);
    SendCommand(command);
}

void LLDBConnector::DeleteAllBreakpoints()
{
    if(!IsRunning()) {
        m_pendingDeletionBreakpoints.clear();
        m_breakpoints.clear();

        LLDBEvent event(wxEVT_LLDB_BREAKPOINTS_UPDATED);
        event.SetBreakpoints(GetAllBreakpoints());
        ProcessEvent(event);
        return;
    }

    // mark all breakpoints for deletion
    CL_DEBUGS(wxString() << "codelite: DeleteAllBreakpoints called");
    m_pendingDeletionBreakpoints.swap(m_breakpoints);

    if(!IsCanInteract()) {
        Interrupt(kInterruptReasonDeleteAllBreakpoints);

    } else {
        LLDBCommand command;
        command.SetCommandType(kCommandDeleteAllBreakpoints);
        SendCommand(command);
        m_pendingDeletionBreakpoints.clear();
    }
}

void LLDBConnector::AddBreakpoints(const BreakpointInfo::Vec_t& breakpoints)
{
    LLDBBreakpoint::Vec_t bps = LLDBBreakpoint::FromBreakpointInfoVector(breakpoints);
    AddBreakpoints(bps);
}

void LLDBConnector::AddBreakpoints(const LLDBBreakpoint::Vec_t& breakpoints)
{
    for(size_t i = 0; i < breakpoints.size(); ++i) {
        AddBreakpoint(breakpoints.at(i), false);
    }
}

void LLDBConnector::Cleanup()
{
    // the order matters here, since both are using the same file descriptor
    // but only m_socket does the actual socket shutdown
    m_thread = nullptr;
    m_socket.reset(NULL);
    InvalidateBreakpoints();
    m_isRunning = false;
    m_canInteract = false;
    m_runCommand.Clear();
    m_attachedToProcess = false;
    StopDebugServer();
    m_pivot.Clear();
}

void LLDBConnector::OnLLDBExited(LLDBEvent& event)
{
    event.Skip();
    m_isRunning = false;
}

void LLDBConnector::OnLLDBStarted(LLDBEvent& event)
{
    event.Skip();
    m_isRunning = true;
}

void LLDBConnector::Start(const LLDBCommand& runCommand)
{
    LLDBCommand startCommand;
    startCommand.SetExecutable(runCommand.GetExecutable());
    startCommand.SetCommandType(kCommandStart);
    startCommand.SetWorkingDirectory(runCommand.GetWorkingDirectory());

    // send the settings as well
    LLDBSettings settings;
    settings.Load();
    startCommand.SetSettings(settings);
    SendCommand(startCommand);

    // stash the runCommand for the future 'Run()' call
    m_runCommand.Clear();
    m_runCommand = runCommand;
    m_runCommand.SetCommandType(kCommandRun);
}

void LLDBConnector::Run()
{
    if(m_runCommand.GetCommandType() == kCommandRun) {
        SendCommand(m_runCommand);
        m_runCommand.Clear();
    }
}

void LLDBConnector::UpdateAppliedBreakpoints(const LLDBBreakpoint::Vec_t& breakpoints) { m_breakpoints = breakpoints; }

const LLDBBreakpoint::Vec_t& LLDBConnector::GetAllBreakpoints() const { return m_breakpoints; }

void LLDBConnector::OnProcessOutput(clProcessEvent& event)
{
    wxString output = event.GetOutput();

    wxArrayString lines = ::wxStringTokenize(output, "\n", wxTOKEN_STRTOK);
    for(size_t i = 0; i < lines.GetCount(); ++i) {
        CL_DEBUG("%s", lines.Item(i).Trim());
    }
}

void LLDBConnector::OnProcessTerminated(clProcessEvent& event)
{
    m_process = nullptr;

    AddPendingEvent(LLDBEvent(wxEVT_LLDB_CRASHED));
}

void LLDBConnector::Interrupt(eInterruptReason reason)
{
    LLDBCommand command;
    command.SetCommandType(kCommandInterrupt);
    command.SetInterruptReason(reason);
    SendCommand(command);
}

bool LLDBConnector::LaunchLocalDebugServer(const wxString& debugServer)
{
#if !BUILD_CODELITE_LLDB
    // Not supported :(
    ::wxMessageBox(_("Locally debugging with LLDB on Windows is not supported by LLDB"), "CodeLite",
                   wxICON_WARNING | wxOK | wxCENTER);
    return false;
#endif

    clDEBUG() << "Launching codelite-lldb";

    // Start the debugger
    if(m_process) {
        // another debugger process is already running
        return false;
    }

    // Apply the environment before we start
    // set the LLDB_DEBUGSERVER_PATH env variable
    wxStringMap_t om;
    om["LLDB_DEBUGSERVER_PATH"] = debugServer;

    EnvSetter es(NULL, &om);
    wxFileName fnCodeLiteLLDB(clStandardPaths::Get().GetBinaryFullPath("codelite-lldb"));

    wxString command;
    command << fnCodeLiteLLDB.GetFullPath() << " -s " << GetDebugServerPath();
    clDEBUG() << "LLDB_DEBUGSERVER_PATH is set to" << debugServer;
    m_process.reset(::CreateAsyncProcess(this, command));
    if(!m_process) {
        clERROR() << "LLDBConnector: failed to launch codelite-lldb:" << fnCodeLiteLLDB.GetFullPath();
        return false;

    } else {
        clDEBUG() << "codelite-lldb launched successfully. PID=" << m_process->GetPid();
    }
    return true;
}

void LLDBConnector::StopDebugServer()
{
    if(m_process) {
        m_process->SetHardKill(true); // kill -9
        m_process->Terminate();
        m_process = nullptr;
    }

    wxLogNull noLog;
    clRemoveFile(GetDebugServerPath());
}

LLDBBreakpoint::Vec_t LLDBConnector::GetUnappliedBreakpoints()
{
    LLDBBreakpoint::Vec_t unappliedBreakpoints;
    for(size_t i = 0; i < m_breakpoints.size(); ++i) {
        if(!m_breakpoints.at(i)->IsApplied()) { unappliedBreakpoints.push_back(m_breakpoints.at(i)); }
    }
    return unappliedBreakpoints;
}

void LLDBConnector::RequestLocals()
{
    if(IsCanInteract()) {
        LLDBCommand command;
        command.SetCommandType(kCommandGetLocals);
        SendCommand(command);
    }
}

void LLDBConnector::RequestVariableChildren(int lldbId)
{
    if(IsCanInteract()) {
        LLDBCommand command;
        command.SetCommandType(kCommandExpandVariable);
        command.SetLldbId(lldbId);
        SendCommand(command);
    }
}

void LLDBConnector::SetVariableValue(const int lldbId, const wxString& value)
{
    if(IsCanInteract()) {
        LLDBCommand lldbCommand;
        lldbCommand.SetCommandType(kCommandSetVariableValue);
        lldbCommand.SetLldbId(lldbId);
        lldbCommand.SetExpression(value);
        SendCommand(lldbCommand);
    }
}

void LLDBConnector::SetVariableDisplayFormat(const int lldbId, const eLLDBFormat format)
{
    if(IsCanInteract()) {
        LLDBCommand command;
        command.SetCommandType(kCommandSetVariableDisplayFormat);
        command.SetLldbId(lldbId);
        command.SetDisplayFormat(format);
        SendCommand(command);
    }
}

wxString LLDBConnector::GetDebugServerPath() const
{
    wxString path;
    path << "/tmp/codelite-lldb.";
    path << ::wxGetProcessId() << ".sock";
    return path;
}

void LLDBConnector::SelectFrame(int frameID)
{
    if(IsCanInteract()) {
        LLDBCommand command;
        command.SetCommandType(kCommandSelectFrame);
        command.SetFrameId(frameID);
        SendCommand(command);
    }
}

void LLDBConnector::SelectThread(int threadID)
{
    SendThreadCommand(kCommandSelectThread, std::vector<int>{ threadID });
}

void LLDBConnector::SuspendThreads(const std::vector<int>& threadIds)
{
    SendThreadCommand(kCommandSuspendThreads, threadIds);
}

void LLDBConnector::SuspendOtherThreads(const std::vector<int>& threadIds)
{
    SendThreadCommand(kCommandSuspendOtherThreads, threadIds);
}

void LLDBConnector::ResumeThreads(const std::vector<int>& threadIds)
{
    SendThreadCommand(kCommandResumeThreads, threadIds);
}

void LLDBConnector::ResumeOtherThreads(const std::vector<int>& threadIds)
{
    SendThreadCommand(kCommandResumeOtherThreads, threadIds);
}

void LLDBConnector::ResumeAllThreads() { SendThreadCommand(kCommandResumeAllThreads, std::vector<int>()); }

void LLDBConnector::EvaluateExpression(const wxString& expression)
{
    if(IsCanInteract()) {
        LLDBCommand command;
        command.SetCommandType(kCommandEvalExpression);
        command.SetExpression(expression);
        SendCommand(command);
    }
}

void LLDBConnector::OpenCoreFile(const LLDBCommand& runCommand) { SendCommand(runCommand); }

void LLDBConnector::AttachProcessWithPID(const LLDBCommand& runCommand)
{
    m_attachedToProcess = true;
    SendCommand(runCommand);
}

void LLDBConnector::NextInstruction()
{
    if(IsCanInteract()) {
        LLDBCommand command;
        command.SetCommandType(kCommandNextInstruction);
        SendCommand(command);
    }
}

void LLDBConnector::ShowCurrentFileLine()
{
    if(IsCanInteract()) {
        LLDBCommand command;
        command.SetCommandType(kCommandCurrentFileLine);
        SendCommand(command);
    }
}

wxString LLDBConnector::GetConnectString() const
{
    wxString connectString;
    LLDBSettings settings;
    bool useTcp = settings.Load().IsUsingRemoteProxy();
    if(useTcp) {
        connectString << settings.GetProxyIp() << ":" << settings.GetProxyPort();
    } else {
        connectString << GetDebugServerPath();
    }
    return connectString;
}

bool LLDBConnector::Connect(LLDBConnectReturnObject& ret, const LLDBSettings& settings, int timeout)
{
    ret.Clear();
    if(settings.IsUsingRemoteProxy()) {
        return ConnectToRemoteDebugger(settings.GetProxyIp(), settings.GetProxyPort(), ret, timeout);

    } else {
        return ConnectToLocalDebugger(ret, timeout);
    }
}

void LLDBConnector::StartNetworkThread()
{
    if(!m_thread && m_socket) {
        socket_t fd = m_socket->GetSocket();
        m_thread.reset(new LLDBNetworkListenerThread(this, m_pivot, fd));
        m_thread->Start();
    }
}

void LLDBConnector::AddWatch(const wxString& watch)
{
    LLDBCommand command;
    command.SetCommandType(kCommandAddWatch);
    command.SetExpression(watch);
    SendCommand(command);
}

void LLDBConnector::DeleteWatch(int lldbId)
{
    LLDBCommand command;
    command.SetCommandType(kCommandDeleteWatch);
    command.SetLldbId(lldbId);
    SendCommand(command);
}

void LLDBConnector::SendInterperterCommand(const wxString& command)
{
    LLDBCommand lldbCommand;
    lldbCommand.SetCommandType(kCommandInterperterCommand);
    lldbCommand.SetExpression(command);
    SendCommand(lldbCommand);
}

void LLDBTerminalCallback::OnProcessOutput(const wxString& str) { wxUnusedVar(str); }

void LLDBTerminalCallback::OnProcessTerminated()
{
    m_process = nullptr;
    delete this;
    CL_DEBUG("LLDB terminal process terminated. Cleaning up");
}
