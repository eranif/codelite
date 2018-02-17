//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CodeLiteLLDBApp.cpp
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

#include "CodeLiteLLDBApp.h"
#include <iostream>
#include <wx/sckaddr.h>
#include "LLDBProtocol/LLDBReply.h"
#include "SocketAPI/clSocketServer.h"
#include "LLDBProtocol/LLDBEnums.h"
#include "clcommandlineparser.h"
#include <lldb/API/SBBreakpointLocation.h>
#include <lldb/API/SBFileSpec.h>
#include <lldb/API/SBCommandReturnObject.h>
#include <lldb/API/SBCommandInterpreter.h>
#include <lldb/API/SBFrame.h>
#include <lldb/API/SBExpressionOptions.h>
#include <wx/socket.h>
#include "LLDBProtocol/LLDBVariable.h"
#include <wx/msgqueue.h>
#include <wx/wxcrtvararg.h>
#include <wx/tokenzr.h>
#include <lldb/API/SBError.h>
#include <lldb/API/SBTypeSummary.h>
#include <lldb/API/SBTypeNameSpecifier.h>
#include <lldb/API/SBTypeCategory.h>
#include <wx/filename.h>
#include "LLDBProtocol/LLDBRemoteHandshakePacket.h"
#include <lldb/API/SBThread.h>

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/**
 * @brief print c-array to the stdout
 */
static void print_c_array(const char** arr)
{
    if(arr) {
        size_t index(0);
        while(arr[index]) {
            wxPrintf("%s\n", arr[index]);
            ++index;
        }
    }
}

static char** _wxArrayStringToCharPtrPtr(const wxArrayString& arr)
{
    char** argv = new char*[arr.size() + 1]; // for the NULL
    for(size_t i = 0; i < arr.size(); ++i) {
        argv[i] = strdup(arr.Item(i).mb_str(wxConvUTF8).data());
    }
    argv[arr.size()] = NULL;
    return argv;
}

static void DELETE_CHAR_PTR_PTR(char** argv)
{
    size_t i = 0;
    while(argv[i]) {
        delete[] argv[i];
        ++i;
    }
    delete[] argv;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define CHECK_DEBUG_SESSION_RUNNING() \
    if(!IsDebugSessionInProgress()) return

CodeLiteLLDBApp::CodeLiteLLDBApp(const wxString& socketPath)
    : m_debuggerSocketPath(socketPath)
    , m_port(wxNOT_FOUND)
{
    DoInitializeApp();
}

CodeLiteLLDBApp::CodeLiteLLDBApp(const wxString& ip, int port)
    : m_ip(ip)
    , m_port(port)
{
    DoInitializeApp();
}

void CodeLiteLLDBApp::DoInitializeApp()
{
    m_networkThread = NULL;
    m_lldbProcessEventThread = NULL;
    m_debuggeePid = wxNOT_FOUND;
    m_interruptReason = kInterruptReasonNone;
    m_exitMainLoop = false;
    m_sessionType = kDebugSessionTypeNormal;

    wxSocketBase::Initialize();
    wxPrintf("codelite-lldb: starting\n");
    // lldbinit file to source
    m_debugger = lldb::SBDebugger::Create(false); // source init file
    wxPrintf("codelite-lldb: lldb initialized successfully\n");
    // register our summary
    OnInit();
}

CodeLiteLLDBApp::~CodeLiteLLDBApp()
{
    wxDELETE(m_networkThread);
    wxDELETE(m_lldbProcessEventThread);
    m_replySocket.reset(NULL);
    OnExit();
}

int CodeLiteLLDBApp::OnExit()
{
    if(m_target.IsValid() && m_debugger.IsValid()) {
        m_debugger.DeleteTarget(m_target);
    }

    if(m_debugger.IsValid()) {
        lldb::SBDebugger::Destroy(m_debugger);
    }
    lldb::SBDebugger::Terminate();
    return TRUE;
}

bool CodeLiteLLDBApp::OnInit()
{
    try {
        wxString connectionString;
        if(m_port == wxNOT_FOUND) {
            connectionString << "unix://" << m_debuggerSocketPath;
        } else {
           connectionString << "tcp://" << m_ip << ":" << m_port;
        }
        m_acceptSocket.Start(connectionString);

    } catch(clSocketException& e) {
        if(m_port == wxNOT_FOUND) {
            wxPrintf("codelite-lldb: caught exception: %s\n", e.what());
            wxPrintf("codelite-lldb: failed to create server on %s. %s\n", m_debuggerSocketPath, strerror(errno));

        } else {
            wxPrintf("codelite-lldb: caught exception: %s\n", e.what());
            wxPrintf("codelite-lldb: failed to create server on %s. %s\n", (wxString() << m_ip << ":" << m_port),
                strerror(errno));
        }
        return false;
    }
    return true;
}

bool CodeLiteLLDBApp::InitializeLLDB(const LLDBCommand& command)
{
    if(IsDebugSessionInProgress()) {
        wxPrintf("codelite-lldb: another session is already in progress\n");
        return false;
    }

    if(!command.GetWorkingDirectory().IsEmpty()) {
        ::wxSetWorkingDirectory(command.GetWorkingDirectory());
    }
    wxPrintf("codelite-lldb: working directory is set to %s\n", ::wxGetCwd());
#if defined(__WXOSX__) || defined(__WXGTK__)
    // On OSX, debugserver executable must exists otherwise lldb will not work properly
    // we ensure that it exists by checking the environment variable LLDB_DEBUGSERVER_PATH
    wxString lldbDebugServer;
    if(!::wxGetEnv("LLDB_DEBUGSERVER_PATH", &lldbDebugServer) || !wxFileName::Exists(lldbDebugServer)) {
        wxPrintf(
            "codelite-lldb: LLDB_DEBUGSERVER_PATH environment does not exist or contains a path to a non existent "
            "file\n");
        NotifyExited();
        return false;
    }
#endif

    m_debuggeePid = wxNOT_FOUND;
    wxPrintf("codelite-lldb: creating target for file '%s'\n", command.GetExecutable());
    m_debugger = lldb::SBDebugger::Create();
    lldb::SBError lldbError;
    m_target = m_debugger.CreateTarget(command.GetExecutable().mb_str().data(), NULL, NULL, true, lldbError);
    if(!m_target.IsValid()) {
        wxPrintf("codelite-lldb: could not create target for file %s. %s\n", command.GetExecutable(),
            lldbError.GetCString());
        NotifyExited();
        return false;
    }
    // Print the content of the summaries (for debugging purposes)
    wxPrintf("codelite-lldb: created target for %s\n", command.GetExecutable());
    m_debugger.SetAsync(true);

    // Keep the settings
    m_settings = command.GetSettings();

    // First, source the .lldbinit file
    wxString source_command;
    source_command << "command source '" << ::wxGetHomeDir() << "/.lldbinit"
                   << "'";
    DoExecutueShellCommand(source_command);

    // Apply the types
    lldb::SBCommandReturnObject ret;
    wxArrayString commands = ::wxStringTokenize(m_settings.GetTypes(), "\n", wxTOKEN_STRTOK);
    for(size_t i = 0; i < commands.GetCount(); ++i) {
        DoExecutueShellCommand(commands.Item(i));
    }
    // DoExecutueShellCommand("type summary list");
    return true;
}

void CodeLiteLLDBApp::StartDebugger(const LLDBCommand& command)
{
    wxPrintf("codelite-lldb: StartDebugger Called\n");
    if(!InitializeLLDB(command)) {
        return;
    }

    // Launch the thread that will handle the LLDB process events
    m_lldbProcessEventThread =
        new LLDBProcessEventHandlerThread(this, m_debugger, m_target.GetProcess(), kDebugSessionTypeNormal);
    m_lldbProcessEventThread->Start();

    // In any case, reset the interrupt reason
    m_interruptReason = kInterruptReasonNone;

    // Notify codelite that the debugger started successfully
    NotifyStarted(kDebugSessionTypeNormal);
}

void CodeLiteLLDBApp::NotifyAllBreakpointsDeleted()
{
    LLDBReply reply;
    reply.SetReplyType(kReplyTypeAllBreakpointsDeleted);
    SendReply(reply);
}

void CodeLiteLLDBApp::NotifyBreakpointsUpdated()
{
    LLDBBreakpoint::Vec_t breakpoints;
    int num = m_target.GetNumBreakpoints();
    wxPrintf("codelite-lldb: Calling NotifyBreakpointsUpdated(). Got %d breakpoints\n", num);
    for(int i = 0; i < num; ++i) {
        lldb::SBBreakpoint bp = m_target.GetBreakpointAtIndex(i);
        if(bp.IsValid() && bp.GetNumResolvedLocations()) {

            // Add the parent breakpoint
            LLDBBreakpoint::Ptr_t mainBreakpoint(new LLDBBreakpoint());
            mainBreakpoint->SetId(bp.GetID());

            lldb::SBBreakpointLocation loc = bp.GetLocationAtIndex(0);
            lldb::SBFileSpec fileLoc = loc.GetAddress().GetLineEntry().GetFileSpec();
            wxFileName bpFile(fileLoc.GetDirectory(), fileLoc.GetFilename());

            mainBreakpoint->SetType(LLDBBreakpoint::kFileLine);
            mainBreakpoint->SetFilename(bpFile.GetFullPath());
            mainBreakpoint->SetLineNumber(loc.GetAddress().GetLineEntry().GetLine());

            if(bp.GetNumLocations() > 1) {

                // add all the children locations to the main breakpoint
                for(size_t i = 0; i < bp.GetNumLocations(); ++i) {
                    lldb::SBBreakpointLocation loc = bp.GetLocationAtIndex(i);

                    lldb::SBFileSpec fileLoc = loc.GetAddress().GetLineEntry().GetFileSpec();
                    wxFileName bpFile(fileLoc.GetDirectory(), fileLoc.GetFilename());

                    // Create a breakpoint for this location
                    LLDBBreakpoint::Ptr_t new_bp(new LLDBBreakpoint());
                    new_bp->SetType(LLDBBreakpoint::kLocation);
                    new_bp->SetFilename(bpFile.GetFullPath());
                    new_bp->SetLineNumber(loc.GetAddress().GetLineEntry().GetLine());
                    new_bp->SetName(loc.GetAddress().GetFunction().GetName());
                    mainBreakpoint->GetChildren().push_back(new_bp);
                }
            } else {
                mainBreakpoint->SetName(loc.GetAddress().GetFunction().GetName());
            }
            breakpoints.push_back(mainBreakpoint);
        }
    }

    LLDBReply reply;
    reply.SetReplyType(kReplyTypeBreakpointsUpdated);
    reply.SetBreakpoints(breakpoints);
    SendReply(reply);
}

void CodeLiteLLDBApp::NotifyExited()
{
    wxPrintf("codelite-lldb: NotifyExited called\n");
    LLDBReply reply;
    reply.SetReplyType(kReplyTypeDebuggerExited);
    SendReply(reply);

    // Let codelite 200 ms head start before starting the cleanup process
    // by doing this, codelite can mark the debugger in "normal" shutdown
    wxThread::Sleep(200);
    Cleanup();
    m_exitMainLoop = true;
}

void CodeLiteLLDBApp::NotifyAborted()
{
    wxPrintf("codelite-lldb: Connection Aborted\n");
    // Let codelite 200 ms head start before starting the cleanup process
    // by doing this, codelite can mark the debugger in "normal" shutdown
    wxThread::Sleep(200);
    Cleanup();
    m_exitMainLoop = true;
}

void CodeLiteLLDBApp::NotifyRunning()
{
    wxPrintf("codelite-lldb: NotifyRunning. Target Process ID %d\n", m_debuggeePid);
    m_variables.clear();
    LLDBReply reply;
    reply.SetReplyType(kReplyTypeDebuggerRunning);
    SendReply(reply);
}

void CodeLiteLLDBApp::NotifyLaunchSuccess()
{
    wxPrintf("codelite-lldb: NotifyLaunchSuccess. Target Process ID %d\n", m_debuggeePid);
    m_variables.clear();
    LLDBReply reply;
    reply.SetReplyType(kReplyTypeLaunchSuccess);
    SendReply(reply);
}

void CodeLiteLLDBApp::NotifyStarted(eLLDBDebugSessionType sessionType)
{
    switch(sessionType) {
    case kDebugSessionTypeAttach:
        wxPrintf("codelite-lldb: NotifyStarted called (kDebugSessionTypeAttach)\n");
        break;
    case kDebugSessionTypeCore:
        wxPrintf("codelite-lldb: NotifyStarted called (kDebugSessionTypeCore)\n");
        break;
    case kDebugSessionTypeNormal:
        wxPrintf("codelite-lldb: NotifyStarted called (kDebugSessionTypeNormal)\n");
        break;
    }
    m_sessionType = sessionType;
    m_variables.clear();
    LLDBReply reply;
    reply.SetDebugSessionType(sessionType);
    reply.SetReplyType(kReplyTypeDebuggerStartedSuccessfully);
    SendReply(reply);
}

void CodeLiteLLDBApp::NotifyStopped()
{
    m_variables.clear();
    LLDBReply reply;
    wxPrintf("codelite-lldb: NotifyStopped() called. m_interruptReason=%d\n", (int)m_interruptReason);
    reply.SetReplyType(kReplyTypeDebuggerStopped);
    reply.SetInterruptResaon(m_interruptReason);

    // If we find a thread that was stopped due to breakpoint, make it the active one
    int threadCount = m_target.GetProcess().GetNumThreads();
    for(int i = 0; i < threadCount; ++i) {
        lldb::SBThread thr = m_target.GetProcess().GetThreadAtIndex(i);
        if(thr.GetStopReason() == lldb::eStopReasonBreakpoint) {
            m_target.GetProcess().SetSelectedThread(thr);
            break;
        }
    }

    lldb::SBThread thread = m_target.GetProcess().GetSelectedThread();
    LLDBBacktrace bt(thread, m_settings);
    reply.SetBacktrace(bt);

    int selectedThreadId = thread.GetThreadID();

    // set the selected frame file:line
    if(thread.IsValid() && thread.GetSelectedFrame().IsValid()) {
        lldb::SBFrame frame = thread.GetSelectedFrame();
        lldb::SBLineEntry lineEntry = thread.GetSelectedFrame().GetLineEntry();
        if(lineEntry.IsValid()) {
            reply.SetLine(lineEntry.GetLine());
            lldb::SBFileSpec fileSepc = frame.GetLineEntry().GetFileSpec();
            reply.SetFilename(wxFileName(fileSepc.GetDirectory(), fileSepc.GetFilename()).GetFullPath());
        }
    }

    // Prepare list of threads
    LLDBThread::Vect_t threads;
    for(int i = 0; i < threadCount; ++i) {
        LLDBThread t;
        lldb::SBThread thr = m_target.GetProcess().GetThreadAtIndex(i);
        t.SetStopReason(thr.GetStopReason());
        t.SetId(thr.GetThreadID());
        t.SetActive(selectedThreadId == (int)thr.GetThreadID());
        lldb::SBFrame frame = thr.GetSelectedFrame();
        t.SetFunc(frame.GetFunctionName() ? frame.GetFunctionName() : "");
        lldb::SBLineEntry lineEntry = thr.GetSelectedFrame().GetLineEntry();

        // get the stop reason string
        char buffer[1024];
        memset(buffer, 0x0, sizeof(buffer));
        thr.GetStopDescription(buffer, sizeof(buffer));
        t.SetStopReasonString(buffer);

        wxPrintf("codelite-lldb: thread %d stop reason is %s\n", t.GetId(), t.GetStopReasonString());
        if(lineEntry.IsValid()) {
            t.SetLine(lineEntry.GetLine());
            lldb::SBFileSpec fileSepc = frame.GetLineEntry().GetFileSpec();
            t.SetFile(wxFileName(fileSepc.GetDirectory(), fileSepc.GetFilename()).GetFullPath());
        }
        threads.push_back(t);
    }

    reply.SetThreads(threads);
    SendReply(reply);

    // reset the interrupt reason
    m_interruptReason = kInterruptReasonNone;
}

void CodeLiteLLDBApp::NotifyStoppedOnFirstEntry()
{
    wxPrintf("codelite-lldb: NotifyStoppedOnFirstEntry()\n");
    m_variables.clear();
    m_watches.clear();
    LLDBReply reply;
    reply.SetReplyType(kReplyTypeDebuggerStoppedOnFirstEntry);
    SendReply(reply);
}

void CodeLiteLLDBApp::SendReply(const LLDBReply& reply)
{
    try {
        m_replySocket->WriteMessage(reply.ToJSON().format());

    } catch(clSocketException& e) {
        wxPrintf("codelite-lldb: failed to send reply. %s. %s.\n", e.what().c_str(), strerror(errno));
    }
}

void CodeLiteLLDBApp::RunDebugger(const LLDBCommand& command)
{
    if(m_debuggeePid != wxNOT_FOUND) {
        wxPrintf("codelite-lldb: another session is already in progress\n");
        return;
    }

    if(m_debugger.IsValid()) {
        m_variables.clear();
        m_watches.clear();

        // Construct char** arrays
        clCommandLineParser parser(command.GetCommandArguments());
        const char** argv = (const char**)_wxArrayStringToCharPtrPtr(parser.ToArray());
        char** penv = command.GetEnvArray();

        std::string tty_c;
        std::string workingDirectory;
        if(!command.GetRedirectTTY().IsEmpty()) {
            tty_c = command.GetRedirectTTY().mb_str(wxConvUTF8).data();
        }

        if(!command.GetWorkingDirectory().IsEmpty()) {
            workingDirectory = command.GetWorkingDirectory().mb_str(wxConvUTF8).data();
        }

        const char* ptty = tty_c.empty() ? NULL : tty_c.c_str();
        wxPrintf("codelite-lldb: running debugger. tty=%s\n", ptty);

        const char* pwd = workingDirectory.empty() ? NULL : workingDirectory.c_str();
        wxPrintf("codelite-lldb: target working directory is set to: %s\n", pwd ? pwd : "NULL");

        // wxPrintf("codelite-lldb: Environment is set to:\n");
        // print_c_array( (const char**)penv );

        wxPrintf("codelite-lldb: Arguments are set to:\n");
        print_c_array(argv);

        // Execute startup commands
        wxArrayString startupCommands = ::wxStringTokenize(command.GetStartupCommands(), "\n", wxTOKEN_STRTOK);
        for(size_t i = 0; i < startupCommands.GetCount(); ++i) {
            lldb::SBCommandReturnObject ret;
            std::string c_command = startupCommands.Item(i).Trim().mb_str(wxConvUTF8).data();
            wxPrintf("codelite-lldb: executing: '%s'\n", c_command.c_str());
            m_debugger.GetCommandInterpreter().HandleCommand(c_command.c_str(), ret);
        }

        lldb::SBError lldbError;
        lldb::SBListener listener = m_debugger.GetListener();
        size_t launchFlags = lldb::eLaunchFlagStopAtEntry;

#ifdef LLDB_ON_WINDOWS
        lldb::SBLaunchInfo li(argv);
        li.SetShell("C:\\Windows\\system32\\cmd.exe");
        li.SetEnvironmentEntries((const char**)penv, true);
        launchFlags |= lldb::eLaunchFlagLaunchInShell;
        launchFlags |= lldb::eLaunchFlagDisableASLR;
        launchFlags |= lldb::eLaunchFlagDebug;
        li.SetLaunchFlags(launchFlags);
        lldb::SBProcess process = m_target.Launch(li, lldbError);
#else
        launchFlags |= lldb::eLaunchFlagLaunchInSeparateProcessGroup;
        lldb::SBProcess process =
            m_target.Launch(listener, argv, (const char**)penv, ptty, ptty, ptty, pwd, launchFlags, true, lldbError);
#endif
        // bool isOk = m_target.LaunchSimple(argv, envp, wd).IsValid();
        DELETE_CHAR_PTR_PTR(const_cast<char**>(argv));
        DELETE_CHAR_PTR_PTR(penv);

        if(!process.IsValid() || !lldbError.Success()) {
            wxPrintf("codelite-lldb: error while launching process. %s\n", lldbError.GetCString());
            NotifyExited();

        } else {
            m_debuggeePid = process.GetProcessID();
#ifdef __WXGTK__
            NotifyLaunchSuccess();
#else
            NotifyRunning();
#endif
        }
    }
}

void CodeLiteLLDBApp::Cleanup()
{
    wxPrintf("codelite-lldb: Cleanup() called...\n");
    m_variables.clear();
    m_watches.clear();

    wxDELETE(m_networkThread);
    wxDELETE(m_lldbProcessEventThread);

    m_interruptReason = kInterruptReasonNone;
    m_debuggeePid = wxNOT_FOUND;

    if(m_target.IsValid()) {
        m_target.DeleteAllBreakpoints();
        m_target.DeleteAllWatchpoints();

        // detach from the process
        if(m_sessionType == kDebugSessionTypeAttach) {
            wxPrintf("codelite-lldb: detaching from process ID %d\n", (int)m_target.GetProcess().GetProcessID());
            m_target.GetProcess().Detach();
        }
        m_debugger.DeleteTarget(m_target);
    }
    m_sessionType = kDebugSessionTypeNormal;
    wxPrintf("codelite-lldb: Cleanup() called... done\n");
}

void CodeLiteLLDBApp::ApplyBreakpoints(const LLDBCommand& command)
{
    wxPrintf("codelite-lldb: ApplyBreakpoints called\n");
    if(m_target.GetProcess().GetState() == lldb::eStateStopped) {
        wxPrintf("codelite-lldb: ApplyBreakpoints: process state is stopped - will apply them now\n");
        // we can apply the breakpoints
        // Apply all breakpoints with an-invalid breakpoint ID
        LLDBBreakpoint::Vec_t breakpoints = command.GetBreakpoints();
        while(!breakpoints.empty()) {
            LLDBBreakpoint::Ptr_t breakPoint = breakpoints.at(0);
            if(!breakPoint->IsApplied()) {
                switch(breakPoint->GetType()) {
                case LLDBBreakpoint::kFunction: {
                    wxPrintf("codelite-lldb: creating breakpoint by name: %s\n", breakPoint->GetName());
                    m_target.BreakpointCreateByName(breakPoint->GetName().mb_str().data(), NULL);
                    break;
                }
                case LLDBBreakpoint::kFileLine: {
                    wxPrintf("codelite-lldb: creating breakpoint by location: %s,%d\n", breakPoint->GetFilename(),
                        breakPoint->GetLineNumber());
                    m_target.BreakpointCreateByLocation(
                        breakPoint->GetFilename().mb_str().data(), breakPoint->GetLineNumber());
                    break;
                }
                }
            }
            breakpoints.erase(breakpoints.begin());
        }
        NotifyBreakpointsUpdated();

    } else {
        wxPrintf("codelite-lldb: ApplyBreakpoints: process state is _NOT_ Stopped - interrupting process\n");
        // interrupt the process
        m_interruptReason = kInterruptReasonApplyBreakpoints;
        m_target.GetProcess().SendAsyncInterrupt();
    }
}

void CodeLiteLLDBApp::Continue(const LLDBCommand& command)
{
    CHECK_DEBUG_SESSION_RUNNING();
    if(m_target.GetProcess().IsValid() && m_target.GetProcess().GetState() == lldb::eStateStopped) {
        m_target.GetProcess().Continue();
    }
}

void CodeLiteLLDBApp::StopDebugger(const LLDBCommand& command)
{
    CHECK_DEBUG_SESSION_RUNNING();
    NotifyExited();
}

void CodeLiteLLDBApp::DeleteAllBreakpoints(const LLDBCommand& command)
{
    CHECK_DEBUG_SESSION_RUNNING();
    wxUnusedVar(command);
    if(m_target.GetProcess().GetState() == lldb::eStateStopped) {
        m_target.DeleteAllBreakpoints();
        NotifyAllBreakpointsDeleted();

    } else {
        m_interruptReason = kInterruptReasonDeleteAllBreakpoints;
        m_target.GetProcess().SendAsyncInterrupt();
    }
}

void CodeLiteLLDBApp::DeleteBreakpoints(const LLDBCommand& command)
{
    CHECK_DEBUG_SESSION_RUNNING();

    const LLDBBreakpoint::Vec_t& bps = command.GetBreakpoints();
    if(bps.empty()) {
        return;
    }

    wxPrintf("codelite-lldb: DeleteBreakpoints called\n");
    if(m_target.GetProcess().GetState() == lldb::eStateStopped) {
        wxPrintf("codelite-lldb: DeleteBreakpoints: process state is Stopped - will apply them now\n");
        for(size_t i = 0; i < bps.size(); ++i) {
            LLDBBreakpoint::Ptr_t breakpoint = bps.at(i);
            wxPrintf("codelite-lldb: deleting breakpoint: %s\n", breakpoint->ToString());
            if(breakpoint->IsApplied()) {
                lldb::SBBreakpoint lldbBreakpoint = m_target.FindBreakpointByID(breakpoint->GetId());
                if(lldbBreakpoint.IsValid()) {
                    lldbBreakpoint.ClearAllBreakpointSites();
                    m_target.BreakpointDelete(lldbBreakpoint.GetID());
                }
            }
        }
        NotifyBreakpointsUpdated();

    } else {
        wxPrintf("codelite-lldb: DeleteBreakpoints: process is Busy - will interrupt it\n");
        m_interruptReason = kInterruptReasonDeleteBreakpoint;
        m_target.GetProcess().SendAsyncInterrupt();
    }
}

void CodeLiteLLDBApp::Next(const LLDBCommand& command)
{
    CHECK_DEBUG_SESSION_RUNNING();
    if(m_target.GetProcess().IsValid() && m_target.GetProcess().GetState() == lldb::eStateStopped) {
        m_target.GetProcess().GetSelectedThread().StepOver();
    }
}

void CodeLiteLLDBApp::StepIn(const LLDBCommand& command)
{
    CHECK_DEBUG_SESSION_RUNNING();
    if(m_target.GetProcess().IsValid() && m_target.GetProcess().GetState() == lldb::eStateStopped) {
        m_target.GetProcess().GetSelectedThread().StepInto();
    }
}

void CodeLiteLLDBApp::StepOut(const LLDBCommand& command)
{
    CHECK_DEBUG_SESSION_RUNNING();
    if(m_target.GetProcess().IsValid() && m_target.GetProcess().GetState() == lldb::eStateStopped) {
        m_target.GetProcess().GetSelectedThread().StepOut();
    }
}

bool CodeLiteLLDBApp::CanInteract()
{
    return IsDebugSessionInProgress() && (m_target.GetProcess().GetState() == lldb::eStateStopped);
}

bool CodeLiteLLDBApp::IsDebugSessionInProgress() { return m_debugger.IsValid() && m_target.IsValid(); }

void CodeLiteLLDBApp::Interrupt(const LLDBCommand& command)
{
    wxPrintf("codelite-lldb: interrupting debugee process\n");
    m_interruptReason = (eInterruptReason)command.GetInterruptReason();
    m_target.GetProcess().SendAsyncInterrupt();
}

void CodeLiteLLDBApp::AcceptNewConnection()
{
    m_replySocket.reset(NULL);
    wxPrintf("codelite-lldb: waiting for new connection\n");
    try {
        while(true) {
            m_replySocket = m_acceptSocket.WaitForNewConnection(1);
            if(m_replySocket) {
                break;
            }
        }

        // Remote connection, send the 'handshake' packet
        if(m_port != wxNOT_FOUND) {
            wxPrintf("codelite-lldb: sending handshake packet\n");
            LLDBRemoteHandshakePacket handshake;
            handshake.SetHost(::wxGetHostName());
            m_replySocket->WriteMessage(handshake.ToJSON().format());
        }

        // handle the connection to the thread
        m_networkThread = new LLDBNetworkServerThread(this, m_replySocket->GetSocket());
        m_networkThread->Start();

    } catch(clSocketException& e) {
        wxPrintf("codelite-lldb: an error occurred while waiting for connection. %s\n", e.what().c_str());
        Cleanup();

        // exit
        throw clSocketException("Failed to accept new connection");
    }
}

void CodeLiteLLDBApp::LocalVariables(const LLDBCommand& command)
{
    wxUnusedVar(command);
    LLDBVariable::Vect_t locals;
    m_variables.clear();

    wxPrintf("codelite-lldb: fetching local variables for selected frame\n");
    lldb::SBFrame frame = m_target.GetProcess().GetSelectedThread().GetSelectedFrame();
    if(!frame.IsValid()) {
        NotifyLocals(locals);
        return;
    }

    // get list of locals
    lldb::SBValueList args = frame.GetVariables(true, true, false, true);
    for(size_t i = 0; i < args.GetSize(); ++i) {
        lldb::SBValue value = args.GetValueAtIndex(i);
        if(value.IsValid()) {
            LLDBVariable::Ptr_t var(new LLDBVariable(value));
            VariableWrapper wrapper;
            wrapper.value = value;
            m_variables.insert(std::make_pair(value.GetID(), wrapper));
            locals.push_back(var);
        }
    }

    // next, add the watches
    for(size_t i = 0; i < m_watches.GetCount(); ++i) {
        lldb::SBValue value = frame.GetValueForVariablePath(m_watches.Item(i).mb_str(wxConvUTF8).data());
        if(value.IsValid()) {
            LLDBVariable::Ptr_t var(new LLDBVariable(value));
            var->SetIsWatch(true);
            // override the name with the actual expression that we want to watch
            var->SetName(m_watches.Item(i));

            // Keep the info about this watch
            VariableWrapper wrapper;
            wrapper.value = value;
            wrapper.isWatch = true;
            wrapper.expression = m_watches.Item(i);
            m_variables.insert(std::make_pair(value.GetID(), wrapper));
            locals.push_back(var);
        }
    }
    NotifyLocals(locals);
}

void CodeLiteLLDBApp::NotifyLocals(LLDBVariable::Vect_t locals)
{
    wxPrintf("codelite-lldb: NotifyLocals called with %d locals\n", (int)locals.size());
    LLDBReply reply;
    reply.SetReplyType(kReplyTypeLocalsUpdated);
    reply.SetVariables(locals);
    SendReply(reply);
}

// we need to return list of children for a variable
// we stashed the variables we got so far inside a map
void CodeLiteLLDBApp::ExpandVariable(const LLDBCommand& command)
{
    int variableId = command.GetLldbId();
    if(variableId == wxNOT_FOUND) {
        return;
    }

    LLDBVariable::Vect_t children;
    std::map<int, VariableWrapper>::iterator iter = m_variables.find(variableId);
    if(iter != m_variables.end()) {
        lldb::SBValue* pvalue = &(iter->second.value);
        lldb::SBValue deReferencedValue;
        int size = pvalue->GetNumChildren();

        lldb::TypeClass typeClass = pvalue->GetType().GetTypeClass();
        if(typeClass & lldb::eTypeClassArray) {
            size > (int)m_settings.GetMaxArrayElements() ? size = m_settings.GetMaxArrayElements() : size = size;
            wxPrintf("codelite-lldb: value %s is an array. Limiting its size\n", pvalue->GetName());
        } /*else if ( typeClass & lldb::eTypeClassPointer ) {
            // dereference is needed
            wxPrintf("codelite-lldb: value '%s' is a class pointer, dereferecning it\n", pvalue->GetName());
            deReferencedValue = pvalue->Dereference();

            // and update the number of children
            pvalue = &deReferencedValue;

            wxPrintf("codelite-lldb: new number of children is set to %d\n", size);
            size = pvalue->GetNumChildren();
        }*/

        for(int i = 0; i < size; ++i) {
            lldb::SBValue child = pvalue->GetChildAtIndex(i);
            if(child.IsValid()) {
                LLDBVariable::Ptr_t var(new LLDBVariable(child));
                children.push_back(var);
                VariableWrapper wrapper;
                wrapper.value = child;
                m_variables.insert(std::make_pair(child.GetID(), wrapper));
            }
        }

        LLDBReply reply;
        reply.SetReplyType(kReplyTypeVariableExpanded);
        reply.SetVariables(children);
        reply.SetLldbId(variableId);
        SendReply(reply);
    }
}

void CodeLiteLLDBApp::CallAfter(CodeLiteLLDBApp::CommandFunc_t func, const LLDBCommand& command)
{
    m_commands_queue.Post(std::make_pair(func, command));
}

void CodeLiteLLDBApp::MainLoop()
{
    try {
        AcceptNewConnection();
        // We got both ends connected
        wxPrintf("codelite-lldb: successfully established connection to codelite\n");

        while(!m_exitMainLoop) {
            CodeLiteLLDBApp::QueueItem_t msg;
            CodeLiteLLDBApp::NotifyFunc_t notify_func = NULL;

            bool got_something = false;
            if(m_commands_queue.ReceiveTimeout(1, msg) == wxMSGQUEUE_NO_ERROR) {
                // Process the command
                CodeLiteLLDBApp::CommandFunc_t pFunc = msg.first;
                LLDBCommand command = msg.second;
                (this->*pFunc)(command);

                got_something = true;
            }

            if(m_notify_queue.ReceiveTimeout(1, notify_func) == wxMSGQUEUE_NO_ERROR) {

                (this->*notify_func)();
                got_something = true;
            }

            if(!got_something) {
                wxThread::Sleep(10);
            }
        }
        wxPrintf("codelite-lldb: terminating\n");

    } catch(clSocketException& e) {
        wxPrintf(
            "codelite-lldb: an error occurred during MainLoop(). %s. strerror=%s\n", e.what().c_str(), strerror(errno));
        // exit now
        exit(0);
    }
}

void CodeLiteLLDBApp::SelectFrame(const LLDBCommand& command)
{
    wxPrintf("codelite-lldb: selecting frame %d\n", command.GetFrameId());
    if(CanInteract() && command.GetFrameId() != wxNOT_FOUND) {
        m_target.GetProcess().GetSelectedThread().SetSelectedFrame(command.GetFrameId());
        m_interruptReason = kInterruptReasonNone;
        NotifyStopped();
    }
}

void CodeLiteLLDBApp::SelectThread(const LLDBCommand& command)
{
    wxPrintf("codelite-lldb: selecting thread %d\n", command.GetThreadId());
    if(CanInteract() && command.GetThreadId() != wxNOT_FOUND) {
        lldb::SBThread thr = m_target.GetProcess().GetThreadByID(command.GetThreadId());
        if(thr.IsValid()) {
            m_target.GetProcess().SetSelectedThread(thr);
            m_interruptReason = kInterruptReasonNone;
            NotifyStopped();
        }
    }
}

void CodeLiteLLDBApp::EvalExpression(const LLDBCommand& command)
{
    wxPrintf("codelite-lldb: evaluating expression '%s'\n", command.GetExpression());

    if(CanInteract()) {
        // Evaluate the expression based on the current frame
        wxString expression = command.GetExpression();
        expression.Trim().Trim(false);
        lldb::SBValue value = m_target.GetProcess().GetSelectedThread().GetSelectedFrame().GetValueForVariablePath(
            expression.mb_str(wxConvUTF8).data());
        if(value.IsValid()) {

            LLDBReply reply;
            reply.SetReplyType(kReplyTypeExprEvaluated);
            reply.SetExpression(command.GetExpression());

            LLDBVariable::Vect_t vars;
            LLDBVariable::Ptr_t var(new LLDBVariable(value));
            vars.push_back(var);
            reply.SetVariables(vars);
            VariableWrapper wrapper;
            wrapper.value = value;
            // Cache the expanded variable (we will need it later for tooltip expansion
            m_variables.insert(std::make_pair(value.GetID(), wrapper));

            SendReply(reply);
        }
    }
}

void CodeLiteLLDBApp::OpenCoreFile(const LLDBCommand& command)
{
    wxPrintf("codeite-lldb: debugging core file '%s'\n", command.GetCorefile());
    wxPrintf("codeite-lldb: executable file '%s'\n", command.GetExecutable());

    if(!InitializeLLDB(command)) {
        return;
    }

    lldb::SBProcess process = m_target.LoadCore(command.GetCorefile().mb_str(wxConvUTF8).data());
    if(!process.IsValid()) {
        wxPrintf("codeite-lldb: error loading core file '%s'\n", command.GetCorefile());
        NotifyExited();
        return;
    }

    // Launch the thread that will handle the LLDB process events
    m_lldbProcessEventThread =
        new LLDBProcessEventHandlerThread(this, m_debugger, m_target.GetProcess(), kDebugSessionTypeCore);
    m_lldbProcessEventThread->Start();

    // Notify codelite that the debugger started successfully
    NotifyStarted(kDebugSessionTypeCore);

    // In any case, reset the interrupt reason
    m_interruptReason = kInterruptReasonNone;

    // Since we are in 'core' session
    // immediately notify about 'stop'
    NotifyStopped();
}

void CodeLiteLLDBApp::AttachProcess(const LLDBCommand& command)
{
    wxPrintf("codeite-lldb: attaching to process with PID %d\n", command.GetProcessID());
    if(!InitializeLLDB(command)) {
        return;
    }

    lldb::SBError errorCode;
    lldb::SBListener listener;
    lldb::SBProcess process = m_target.AttachToProcessWithID(listener, (lldb::pid_t)command.GetProcessID(), errorCode);
    if(!errorCode.Success()) {
        wxPrintf("codeite-lldb: error attaching process %d. '%s'\n", command.GetProcessID(), errorCode.GetCString());
        NotifyExited();
        return;
    }
    wxPrintf("codeite-lldb: process attached successfully\n");

    // Launch the thread that will handle the LLDB process events
    m_lldbProcessEventThread =
        new LLDBProcessEventHandlerThread(this, m_debugger, m_target.GetProcess(), kDebugSessionTypeAttach);
    m_lldbProcessEventThread->Start();

    // In any case, reset the interrupt reason
    m_interruptReason = kInterruptReasonNone;

    // Notify codelite that the debugger started successfully
    NotifyStarted(kDebugSessionTypeAttach);
}

void CodeLiteLLDBApp::DetachDebugger(const LLDBCommand& command)
{
    // detach from the process
    wxPrintf("codelite-lldb: detaching from process\n");

    m_target.DeleteAllBreakpoints();
    m_target.DeleteAllWatchpoints();
    if(m_target.GetProcess().IsValid()) {
        // detch and 'continue'
        m_target.GetProcess().Detach(false);
    }
    NotifyExited();
}

void CodeLiteLLDBApp::NextInstruction(const LLDBCommand& command)
{
    wxPrintf("codelite-lldb: NextInstruction called\n");
    if(m_target.GetProcess().IsValid() && m_target.GetProcess().GetState() == lldb::eStateStopped) {
        m_target.GetProcess().GetSelectedThread().StepInstruction(true);
    }
}

void CodeLiteLLDBApp::ShowCurrentFileLine(const LLDBCommand& command)
{
    wxPrintf("codelite-lldb: ShowCurrentFileLine called\n");
    if(m_target.GetProcess().IsValid() && m_target.GetProcess().GetState() == lldb::eStateStopped) {
        NotifyStopped();
    }
}

void CodeLiteLLDBApp::DoExecutueShellCommand(const wxString& command, bool printOutput)
{
    wxString cmd = command;
    cmd.Trim().Trim(false);

    if(cmd.IsEmpty()) return;

    lldb::SBCommandReturnObject ret;
    m_debugger.GetCommandInterpreter().HandleCommand(command.mb_str(wxConvUTF8).data(), ret);
    if(printOutput && ret.GetOutput()) {
        wxString cmdOutput = ret.GetOutput();
        cmdOutput.Trim().Trim(false);
        if(!cmdOutput.IsEmpty()) {
            wxPrintf("codelite-lldb: output of command '%s':\n%s\n", command, cmdOutput);
        }
    }
}

void CodeLiteLLDBApp::AddWatch(const LLDBCommand& command)
{
    wxString expr = command.GetExpression();
    wxPrintf("codelite-lldb: adding watch '%s'\n", expr);
    m_watches.Add(command.GetExpression());
}

void CodeLiteLLDBApp::DeleteWatch(const LLDBCommand& command)
{
    // Remove it from the m_variables table
    int lldbID = command.GetLldbId();
    wxString expression;
    std::map<int, VariableWrapper>::iterator iter = m_variables.find(lldbID);
    if(iter != m_variables.end()) {
        expression = iter->second.expression;
        wxPrintf("codelite-lldb: deleting watch '%s'\n", expression);
        m_variables.erase(iter);
    }

    // remove it from the watch list
    int where = m_watches.Index(expression);
    if(where != wxNOT_FOUND) {
        m_watches.RemoveAt(where);
    }
}

void CodeLiteLLDBApp::ExecuteInterperterCommand(const LLDBCommand& command)
{

    lldb::SBCommandReturnObject ret;
    std::string c_command = command.GetExpression().mb_str(wxConvUTF8).data();
    wxPrintf("codelite-lldb: ExecuteInterperterCommand: '%s'\n", c_command.c_str());
    m_debugger.GetCommandInterpreter().HandleCommand(c_command.c_str(), ret);

    if(ret.GetOutput()) {
        LLDBReply reply;
        reply.SetText(ret.GetOutput());
        reply.SetReplyType(kReplyTypeInterperterReply);
        SendReply(reply);
    } else if(ret.GetError()) {
        LLDBReply reply;
        reply.SetText(ret.GetError());
        reply.SetReplyType(kReplyTypeInterperterReply);
        SendReply(reply);
    }
}
