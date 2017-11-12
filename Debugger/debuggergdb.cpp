//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debuggergdb.cpp
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
#include "debuggergdb.h"
#include <wx/msgdlg.h>
#include "processreaderthread.h"
#include "event_notifier.h"
#include "asyncprocess.h"
#include <wx/ffile.h>
#include "exelocator.h"
#include "environmentconfig.h"
#include "dirkeeper.h"
#include "dbgcmd.h"
#include "wx/regex.h"
#include "debuggerobserver.h"
#include "wx/filename.h"
#include "procutils.h"
#include "wx/tokenzr.h"
#include <algorithm>
#include "file_logger.h"
#include "globals.h"

#ifdef __WXMSW__
#include "windows.h"

// On Windows lower than XP, the function DebugBreakProcess does not exist
// so we need to bind it dynamically
typedef BOOL WINAPI (*DBG_BREAK_PROC_FUNC_PTR)(HANDLE);
DBG_BREAK_PROC_FUNC_PTR DebugBreakProcessFunc = NULL;
HINSTANCE Kernel32Dll = NULL;

// define a dummy control handler
static BOOL CtrlHandler(DWORD fdwCtrlType)
{
    wxUnusedVar(fdwCtrlType);

    // return FALSE so other process in our group are allowed to process this event
    return FALSE;
}

static BOOL SigHandler(DWORD CtrlType)
{
    if(CtrlType == CTRL_C_EVENT) {
        return TRUE;
    }
    return FALSE;
}

#endif

#include <sys/types.h>
#include <signal.h>

#if 0
#define DBG_LOG 1
static wxFFile gfp(wxT("debugger.log"), wxT("w+"));
#else
#define DBG_LOG 0
#endif

const wxEventType wxEVT_GDB_STOP_DEBUGGER = wxNewEventType();

// Using the running image of child Thread 46912568064384 (LWP 7051).
static wxRegEx reInfoProgram1(wxT("\\(LWP[ \t]([0-9]+)\\)"));
// Using the running image of child process 10011.
static wxRegEx reInfoProgram2(wxT("child process ([0-9]+)"));
// Using the running image of child thread 4124.0x117c
static wxRegEx reInfoProgram3(wxT("Using the running image of child thread ([0-9]+)"));

#ifdef __WXMSW__
static wxRegEx reConnectionRefused(
    wxT("[0-9a-zA-Z/\\\\-\\_]*:[0-9]+: No connection could be made because the target machine actively refused it."));
#else
static wxRegEx reConnectionRefused(wxT("[0-9a-zA-Z/\\\\-\\_]*:[0-9]+: Connection refused."));
#endif
DebuggerInfo GetDebuggerInfo()
{
    DebuggerInfo info = { wxT("GNU gdb debugger"), wxT("CreateDebuggerGDB"), wxT("v2.0"), wxT("Eran Ifrah") };
    return info;
}

IDebugger* CreateDebuggerGDB()
{
    static DbgGdb theGdbDebugger;
    theGdbDebugger.SetName(wxT("GNU gdb debugger"));

    DebuggerInformation info;
    info.name = theGdbDebugger.GetName();
    theGdbDebugger.SetDebuggerInformation(info);
    return &theGdbDebugger;
}

// Removes MI additional characters from string
static void StripString(wxString& string)
{
    string.Replace(wxT("\\n\""), wxT("\""));
    string = string.AfterFirst(wxT('"'));
    string = string.BeforeLast(wxT('"'));
    string.Replace(wxT("\\\""), wxT("\""));
    string.Replace(wxT("\\\\"), wxT("\\"));
    string.Replace(wxT("\\\\r\\\\n"), wxT("\r\n"));
    string.Replace(wxT("\\\\n"), wxT("\n"));
    string.Replace(wxT("\\\\r"), wxT("\r"));
#ifdef __WXMSW__
    string.Replace("\\r\\n", "\r\n");
#endif
    string = string.Trim();
}

static wxString MakeId()
{
    static unsigned int counter(0);
    wxString newId;
    newId.Printf(wxT("%08u"), ++counter);
    return newId;
}

DbgGdb::DbgGdb()
    : m_debuggeePid(wxNOT_FOUND)
    , m_cliHandler(NULL)
    , m_break_at_main(false)
    , m_attachedMode(false)
    , m_goingDown(false)
    , m_reverseDebugging(false)
    , m_isRecording(false)
    , m_internalBpId(wxNOT_FOUND)
{
#ifdef __WXMSW__
    Kernel32Dll = LoadLibrary(wxT("kernel32.dll"));
    if(Kernel32Dll) {
        DebugBreakProcessFunc = (DBG_BREAK_PROC_FUNC_PTR)GetProcAddress(Kernel32Dll, "DebugBreakProcess");
    } else {
        // we dont have DebugBreakProcess, try to work with Control handlers
        if(SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE) == FALSE) {
            wxLogMessage(wxString::Format(wxT("failed to install ConsoleCtrlHandler: %d"), GetLastError()));
        }
    }
    if(SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE) == FALSE) {
        wxLogMessage(wxString::Format(wxT("failed to install ConsoleCtrlHandler: %d"), GetLastError()));
    }
#endif

    // List of commands which supports the "--reverse" switch
    m_reversableCommands.insert("-exec-continue");
    m_reversableCommands.insert("-exec-step");
    m_reversableCommands.insert("-exec-finish");
    m_reversableCommands.insert("-exec-next");
    m_reversableCommands.insert("-exec-next-instruction");

    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &DbgGdb::OnDataRead, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &DbgGdb::OnProcessEnd, this);
    EventNotifier::Get()->Connect(wxEVT_GDB_STOP_DEBUGGER, wxCommandEventHandler(DbgGdb::OnKillGDB), NULL, this);
}

DbgGdb::~DbgGdb()
{
#ifdef __WXMSW__
    if(Kernel32Dll) {
        FreeLibrary(Kernel32Dll);
        Kernel32Dll = NULL;
    }
#endif
    EventNotifier::Get()->Disconnect(wxEVT_GDB_STOP_DEBUGGER, wxCommandEventHandler(DbgGdb::OnKillGDB), NULL, this);
}

void DbgGdb::RegisterHandler(const wxString& id, DbgCmdHandler* cmd) { m_handlers[id] = cmd; }

DbgCmdHandler* DbgGdb::PopHandler(const wxString& id)
{
    // Check if we got some gaps in the protocol
    //    long nId;
    //    id.ToCLong(&nId);
    //    --nId;
    //    wxString oldId = wxString::Format(wxT("%08d"), (int)nId);

    HandlersMap_t::iterator it = m_handlers.find(id);
    if(it == m_handlers.end()) {
        return NULL;
    }

    DbgCmdHandler* cmd = it->second;
    //    if(it != m_handlers.begin()) {
    //        --it;
    //        if(it->first != oldId) {
    //            CL_WARNING("Request to process handler %s while handler %s is still in the queue!!", id, it->first);
    //        }
    //        ++it;
    //    }
    m_handlers.erase(id);
    return cmd;
}

void DbgGdb::EmptyQueue()
{
    HandlersMap_t::iterator iter = m_handlers.begin();
    while(iter != m_handlers.end()) {
        delete iter->second;
        iter++;
    }
    m_handlers.clear();
}

bool DbgGdb::Start(const DebugSessionInfo& si)
{
    // set the environment variables
    EnvSetter env(m_env, NULL, m_debuggeeProjectName, wxEmptyString);

    wxString dbgExeName;
    if(!DoLocateGdbExecutable(si.debuggerPath, dbgExeName)) {
        return false;
    }

    wxString cmd;
#if defined(__WXGTK__) || defined(__WXMAC__)
    cmd << dbgExeName;
    if(!si.ttyName.IsEmpty()) {
        cmd << wxT(" --tty=") << si.ttyName;
    }
    cmd << wxT(" --interpreter=mi ") << si.exeName;
#else
    cmd << dbgExeName << wxT(" --interpreter=mi ") << si.exeName;
#endif

    m_debuggeePid = wxNOT_FOUND;
    m_attachedMode = false;

    m_observer->UpdateAddLine(wxString::Format(wxT("Current working dir: %s"), wxGetCwd().c_str()));
    m_observer->UpdateAddLine(wxString::Format(wxT("Launching gdb from : %s"), si.cwd.c_str()));
    m_observer->UpdateAddLine(wxString::Format(wxT("Starting debugger  : %s"), cmd.c_str()));

#ifdef __WXMSW__
    // When using remote debugging on Windows we need a console window, as this is the only
    // mechanism to send a Ctrl-C event and signal a SIGINT to interrupt the target.
    bool needs_console = GetIsRemoteDebugging() || m_info.showTerminal;
#else
    bool needs_console = m_info.showTerminal;
#endif

    size_t flags = needs_console ? IProcessCreateConsole : IProcessCreateDefault;
    if(m_info.flags & DebuggerInformation::kRunAsSuperuser) {
        flags |= IProcessCreateAsSuperuser;
    }

    m_gdbProcess = CreateAsyncProcess(this, cmd, flags, si.cwd);
    if(!m_gdbProcess) {
        return false;
    }

#ifdef __WXMSW__
    if(GetIsRemoteDebugging()) {
        // This doesn't really make sense, but AttachConsole fails without it...
        wxMilliSleep(1000);

        if(!AttachConsole(m_gdbProcess->GetPid()))
            m_observer->UpdateAddLine(wxString::Format(wxT("AttachConsole returned error %d"), GetLastError()));

        // We can at least make the window invisible if the user doesn't want to see it.
        if(!m_info.showTerminal) SetWindowPos(GetConsoleWindow(), HWND_BOTTOM, 0, 0, 0, 0, SWP_HIDEWINDOW);

        // Finally we ignore SIGINT so we don't get killed by our own signal
        SetConsoleCtrlHandler((PHANDLER_ROUTINE)SigHandler, TRUE);
    }
#endif
    m_gdbProcess->SetHardKill(true);
    DoInitializeGdb(si);
    return true;
}

bool DbgGdb::WriteCommand(const wxString& command, DbgCmdHandler* handler)
{
    wxString cmd;
    wxString id = MakeId();
    cmd << id << command;

    // Support for reverse debugging
    if(IsReverseDebuggingEnabled() && m_reversableCommands.count(command)) {
        cmd << " --reverse";
    }

    if(!ExecuteCmd(cmd)) {
        CL_WARNING("Failed to send command: %s", cmd);
        return false;
    }
    RegisterHandler(id, handler);
    return true;
}

bool DbgGdb::Run(const wxString& args, const wxString& comm)
{
    if(!GetIsRemoteDebugging()) {

        // add handler for this command
        wxString setArgsCommands;
        setArgsCommands << wxT("-exec-arguments ") << args;
        if(!WriteCommand(setArgsCommands, NULL)) return false;

        return WriteCommand(wxT("-exec-run "), new DbgCmdHandlerExecRun(m_observer, this));

    } else {
        // attach to the remote gdb server
        wxString cmd;
        // cmd << wxT("-target-select remote ") << comm << wxT(" ") << args;
        if(GetIsRemoteExtended())
            cmd << wxT("target extended-remote ") << comm << wxT(" ") << args;
        else
            cmd << wxT("target remote ") << comm << wxT(" ") << args;
        return WriteCommand(cmd, new DbgCmdHandlerRemoteDebugging(m_observer, this));
    }
}

void DbgGdb::DoCleanup()
{
#ifdef __WXMSW__
    if(GetIsRemoteDebugging()) {
        SetConsoleCtrlHandler((PHANDLER_ROUTINE)SigHandler, FALSE);
        FreeConsole(); // Disconnect any existing console window.
    }
#endif

    wxDELETE(m_gdbProcess);
    SetIsRecording(false);
    m_reverseDebugging = false;
    m_goingDown = false;
    m_attachedMode = false;

    SetIsRemoteDebugging(false);
    SetIsRemoteExtended(false);
    EmptyQueue();
    m_gdbOutputArr.Clear();
    m_bpList.clear();
    m_debuggeeProjectName.Clear();

    // Clear any bufferd output
    m_gdbOutputIncompleteLine.Clear();

    // Free allocated console for this session
    m_consoleFinder.FreeConsole();
}

bool DbgGdb::Stop()
{
    m_goingDown = true;

    if(!m_attachedMode) {
        clKill((int)m_debuggeePid, wxSIGKILL, true, (m_info.flags & DebuggerInformation::kRunAsSuperuser));
    }

    wxCommandEvent event(wxEVT_GDB_STOP_DEBUGGER);
    EventNotifier::Get()->AddPendingEvent(event);
    return true;
}

bool DbgGdb::Next() { return WriteCommand(wxT("-exec-next"), new DbgCmdHandlerAsyncCmd(m_observer, this)); }

bool DbgGdb::NextInstruction()
{
    return WriteCommand(wxT("-exec-next-instruction"), new DbgCmdHandlerAsyncCmd(m_observer, this));
}

void DbgGdb::SetBreakpoints()
{
    for(size_t i = 0; i < m_bpList.size(); i++) {
        // Without the 'unnecessary' cast in the next line, bpinfo.bp_type is seen as (e.g.) 4 instead of
        // BP_type_tempbreak, ruining switch statments :/
        BreakpointInfo bpinfo = (BreakpointInfo)m_bpList.at(i);
        Break(bpinfo);
    }
}

bool DbgGdb::Break(const BreakpointInfo& bp)
{
    wxString breakinsertcmd(wxT("-break-insert "));
    if(m_info.enablePendingBreakpoints) {
        breakinsertcmd << wxT("-f ");
    }

    wxFileName fn(bp.file);

    // by default, use full paths for the file name when setting breakpoints
    wxString tmpfileName(fn.GetFullPath());
    if(m_info.useRelativeFilePaths) {
        // user set the option to use relative paths (file name w/o the full path)
        tmpfileName = fn.GetFullName();
    }

    tmpfileName.Replace(wxT("\\"), wxT("/"));

    wxString command;
    switch(bp.bp_type) {
    case BP_type_watchpt:
        //----------------------------------
        // Watchpoints
        //----------------------------------
        command = wxT("-break-watch ");
        switch(bp.watchpoint_type) {
        case WP_watch:
            // nothing to add, simple watchpoint - trigrred when BP is write
            break;
        case WP_rwatch:
            // read watchpoint
            command << wxT("-r ");
            break;
        case WP_awatch:
            // access watchpoint
            command << wxT("-a ");
            break;
        }
        command << bp.watchpt_data;
        break;

    case BP_type_tempbreak:
        //----------------------------------
        // Temporary breakpoints
        //----------------------------------
        command = breakinsertcmd + wxT("-t ");
        break;

    case BP_type_condbreak:
    case BP_type_break:
    default:
        // Should be standard breakpts. But if someone tries to make an ignored temp bp
        // it won't have the BP_type_tempbreak type, so check again here
        command << breakinsertcmd;
        if(bp.is_temp) {
            command << " -t ";
        }
        if(!bp.is_enabled) {
            command << " -d ";
        }
        break;
    }

    //------------------------------------------------------------------------
    // prepare the 'break where' string (address, file:line or regex)
    //------------------------------------------------------------------------
    wxString breakWhere, ignoreCounnt, condition, gdbCommand;
    if(bp.memory_address.IsEmpty() == false) {

        // Memory is easy: just prepend *. gdb copes happily with (at least) hex or decimal
        breakWhere << wxT('*') << bp.memory_address;

    } else if(bp.bp_type != BP_type_watchpt) {
        // Function and Lineno locations can/should be prepended by a filename (but see later)
        if(!tmpfileName.IsEmpty() && bp.lineno > 0) {
            breakWhere << tmpfileName << wxT(":") << bp.lineno;
            breakWhere.Prepend("\"").Append("\"");
        } else if(!bp.function_name.IsEmpty()) {
            if(bp.regex) {
                // update the command
                command = breakinsertcmd + wxT("-r ");
            }
            breakWhere = bp.function_name;
        }
    }

    //------------------------------------------------------------------------
    // prepare the conditions
    //------------------------------------------------------------------------
    if(bp.conditions.IsEmpty() == false) {
        condition << wxT("-c ") << wxT("\"") << bp.conditions << wxT("\" ");
    }

    //------------------------------------------------------------------------
    // prepare the ignore count
    //------------------------------------------------------------------------
    if(bp.ignore_number > 0) {
        ignoreCounnt << wxT("-i ") << bp.ignore_number << wxT(" ");
    }

    // concatenate all the string into one command to pass to gdb
    gdbCommand << command << condition << ignoreCounnt << breakWhere;

    // execute it
    DbgCmdHandlerBp* dbgCommandHandler = new DbgCmdHandlerBp(m_observer, this, bp, &m_bpList, bp.bp_type);
    return WriteCommand(gdbCommand, dbgCommandHandler);
}

bool DbgGdb::SetIgnoreLevel(double bid, const int ignorecount)
{
    if(bid == -1) { // Sanity check
        return false;
    }

    wxString command(wxT("-break-after "));
    command << bid << wxT(" ") << ignorecount;
    return WriteCommand(command, NULL);
}

bool DbgGdb::SetEnabledState(double bid, const bool enable)
{
    if(bid == -1) { // Sanity check
        return false;
    }

    wxString command(wxT("-break-disable "));
    if(enable) {
        command = wxT("-break-enable ");
    }
    command << bid;
    return WriteCommand(command, NULL);
}

bool DbgGdb::SetCondition(const BreakpointInfo& bp)
{
    if(bp.debugger_id == -1) { // Sanity check
        return false;
    }

    wxString command(wxT("-break-condition "));
    command << bp.debugger_id << wxT(" ") << bp.conditions;
    return WriteCommand(command, new DbgCmdSetConditionHandler(m_observer, bp));
}

bool DbgGdb::SetCommands(const BreakpointInfo& bp)
{
    if(bp.debugger_id == -1) { // Sanity check
        return false;
    }
    // There isn't (currentl) a MI command-list command, so use the CLI one
    // This doesn't actually work either, but at least the commands are visible in -break-list
    wxString command(wxT("commands "));
    command << bp.debugger_id << wxT('\n') << bp.commandlist << wxT("\nend");

    if(m_info.enableDebugLog) {
        m_observer->UpdateAddLine(command);
    }

    // If we really wanted, we could get the output (for bp 3) of "commands 3"
    // but as that's not very informative, and we're only faking the command-list anyway, don't bother
    return WriteCommand(command, NULL);
}

bool DbgGdb::Continue() { return WriteCommand("-exec-continue", new DbgCmdHandlerAsyncCmd(m_observer, this)); }

bool DbgGdb::StepIn() { return WriteCommand(wxT("-exec-step"), new DbgCmdHandlerAsyncCmd(m_observer, this)); }

bool DbgGdb::StepInInstruction() { return WriteCommand(wxT("-exec-step-instruction"), new DbgCmdHandlerAsyncCmd(m_observer, this)); }

bool DbgGdb::StepOut() { return WriteCommand(wxT("-exec-finish"), new DbgCmdHandlerAsyncCmd(m_observer, this)); }

bool DbgGdb::IsRunning() { return m_gdbProcess != NULL; }

bool DbgGdb::Interrupt()
{
    if(m_debuggeePid > 0) {
        m_observer->UpdateAddLine(wxString::Format(wxT("Interrupting debugee process: %ld"), m_debuggeePid));

#ifdef __WXMSW__
        if(!GetIsRemoteDebugging() && DebugBreakProcessFunc) {
            // we have DebugBreakProcess
            HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)m_debuggeePid);
            BOOL res = DebugBreakProcessFunc(process);
            CloseHandle(process);
            return res == TRUE;
        }

        if(GetIsRemoteDebugging()) {
            // We need to send GDB a Ctrl-C event.  Using DebugBreakProcess just leaves
            // it unresponsive.
            return GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
        }

        // on Windows version < XP we need to find a solution for interrupting the
        // debuggee process
        return false;
#else
        // Send SIGINT to the process to interrupt it
        clKill((int)m_debuggeePid, wxSIGINT, false, (m_info.flags & DebuggerInformation::kRunAsSuperuser));
        // kill(m_debuggeePid, SIGINT);
        return true;
#endif
    } else {
        ::wxMessageBox(_("Can't interrupt debuggee process: I don't know its PID!"), wxT("CodeLite"));
    }
    return false;
}

bool DbgGdb::QueryFileLine()
{
#if defined(__WXGTK__) || defined(__WXMAC__)
    if(!WriteCommand(wxT("-stack-info-frame"), new DbgCmdHandlerGetLine(m_observer, this))) return false;
#elif defined(__WXMSW__)
    if(!WriteCommand(wxT("-file-list-exec-source-file"), new DbgCmdHandlerGetLine(m_observer, this))) return false;
#endif
    return true;
}

bool DbgGdb::QueryLocals() { return WriteCommand(wxT("-stack-list-variables 2"), new DbgCmdHandlerLocals(m_observer)); }

bool DbgGdb::ExecuteCmd(const wxString& cmd)
{
    static wxLongLong commandsCounter = 0;
    if(m_gdbProcess) {
        if(m_info.enableDebugLog) {
            CL_DEBUG("DEBUG>>%s", cmd);
            m_observer->UpdateAddLine(wxString::Format(wxT("DEBUG>>%s"), cmd));
        }
#ifdef __WXMSW__
        // Ugly hack to fix bug https://github.com/eranif/codelite/issues/906
        if(commandsCounter >= 10) {
            ::wxMilliSleep(2);
            commandsCounter = 0;
        }
#endif
        ++commandsCounter;
        return m_gdbProcess->Write(cmd);
    }
    return false;
}

bool DbgGdb::RemoveAllBreaks() { return ExecuteCmd(wxT("delete")); }

bool DbgGdb::RemoveBreak(double bid)
{
    wxString command;
    command << wxT("-break-delete ") << bid;
    return WriteCommand(command, NULL);
}

bool DbgGdb::FilterMessage(const wxString& msg)
{
    wxString tmpmsg(msg);
    StripString(tmpmsg);
    tmpmsg.Trim().Trim(false);

    if(tmpmsg.Contains(wxT("Variable object not found")) || msg.Contains(wxT("Variable object not found"))) {
        return true;
    }

    if(tmpmsg.Contains(wxT("mi_cmd_var_create: unable to create variable object")) ||
        msg.Contains(wxT("mi_cmd_var_create: unable to create variable object"))) {
        return true;
    }

    if(tmpmsg.Contains(wxT("Variable object not found")) || msg.Contains(wxT("Variable object not found"))) {
        return true;
    }

    if(tmpmsg.Contains(wxT("No symbol \"this\" in current context")) ||
        msg.Contains(wxT("No symbol \"this\" in current context"))) {
        return true;
    }

    if(tmpmsg.Contains(wxT("*running,thread-id"))) {
        return true;
    }

    if(tmpmsg.StartsWith(wxT(">")) || msg.StartsWith(wxT(">"))) {
        // shell line
        return true;
    }
    return false;
}

void DbgGdb::Poke()
{
    static wxRegEx reCommand(wxT("^([0-9]{8})"));

    // poll the debugger output
    wxString curline;
    if(!m_gdbProcess || m_gdbOutputArr.IsEmpty()) {
        return;
    }

    while(DoGetNextLine(curline)) {

        GetDebugeePID(curline);

        // For string manipulations without damaging the original line read
        wxString tmpline(curline);
        StripString(tmpline);
        tmpline.Trim().Trim(false);
        if(m_info.enableDebugLog) {
            // Is logging enabled?

            if(curline.IsEmpty() == false && !tmpline.StartsWith(wxT(">"))) {
                wxString strdebug(wxT("DEBUG>>"));
                strdebug << curline;
                clDEBUG() << strdebug << clEndl;
                m_observer->UpdateAddLine(strdebug);
            }
        }

        if(reConnectionRefused.Matches(curline)) {
            StripString(curline);
#ifdef __WXGTK__
            m_consoleFinder.FreeConsole();
#endif
            m_observer->UpdateAddLine(curline);
            m_observer->UpdateGotControl(DBG_EXITED_NORMALLY);
            return;
        }

        // Check for "Operation not permitted" usually means
        // that the process does not have enough permission to
        // attach to the process
        if(curline.Contains(wxT("Operation not permitted"))) {
#ifdef __WXGTK__
            m_consoleFinder.FreeConsole();
#endif
            m_observer->UpdateAddLine(_("Failed to start debugger: permission denied"));
            m_observer->UpdateGotControl(DBG_EXITED_NORMALLY);
            return;
        }

        if(tmpline.StartsWith(wxT(">"))) {
            // Shell line, probably user command line
            continue;
        }

        if(curline.StartsWith(wxT("~")) || curline.StartsWith(wxT("&")) || curline.StartsWith("@")) {

            // lines starting with ~ are considered "console stream" message
            // and are important to the CLI handler
            bool consoleStream(false);
            bool targetConsoleStream(false);

            if(curline.StartsWith(wxT("~"))) {
                consoleStream = true;
            }

            if(curline.StartsWith(wxT("@"))) {
                targetConsoleStream = true;
            }

            // Filter out some gdb error lines...
            if(FilterMessage(curline)) {
                continue;
            }

            StripString(curline);

            // If we got a valid "CLI Handler" instead of writing the output to
            // the output view, concatenate it into the handler buffer
            if(targetConsoleStream) {
                m_observer->UpdateAddLine(curline);

            } else if(consoleStream && GetCliHandler()) {
                GetCliHandler()->Append(curline);

            } else if(consoleStream) {
                // log message
                m_observer->UpdateAddLine(curline);
            }

        } else if(reCommand.Matches(curline)) {

            // not a gdb message, get the command associated with the message
            wxString id = reCommand.GetMatch(curline, 1);

            if(GetCliHandler() && GetCliHandler()->GetCommandId() == id) {
                // probably the "^done" message of the CLI command
                GetCliHandler()->ProcessOutput(curline);
                SetCliHandler(NULL); // we are done processing the CLI

            } else {
                // strip the id from the line
                curline = curline.Mid(8);
                DoProcessAsyncCommand(curline, id);
            }
        } else if(curline.StartsWith(wxT("^done")) || curline.StartsWith(wxT("*stopped"))) {
            // Unregistered command, use the default AsyncCommand handler to process the line
            DbgCmdHandlerAsyncCmd cmd(m_observer, this);
            cmd.ProcessOutput(curline);
        } else {
            // Unknow format, just log it
            if(m_info.enableDebugLog && !FilterMessage(curline)) {
                m_observer->UpdateAddLine(curline);
            }
        }
    }
}

void DbgGdb::DoProcessAsyncCommand(wxString& line, wxString& id)
{
    if(line.StartsWith(wxT("^error"))) {

        // the command was error, for example:
        // finish in the outer most frame
        // print the error message and remove the command from the queue
        DbgCmdHandler* handler = PopHandler(id);
        bool errorProcessed(false);

        if(handler && handler->WantsErrors()) {
            errorProcessed = handler->ProcessOutput(line);
        }

        if(handler) {
            delete handler;
        }

        StripString(line);

        // We also need to pass the control back to the program
        if(!errorProcessed) {
            m_observer->UpdateGotControl(DBG_CMD_ERROR);
        }

        if(!FilterMessage(line) && m_info.enableDebugLog) {
            m_observer->UpdateAddLine(line);
        }

    } else if(line.StartsWith(wxT("^done")) || line.StartsWith(wxT("^connected"))) {
        // The synchronous operation was successful, results are the return values.
        DbgCmdHandler* handler = PopHandler(id);
        if(handler) {
            handler->ProcessOutput(line);
            delete handler;
        }

    } else if(line.StartsWith(wxT("^running"))) {
        // asynchronous command was executed
        // send event that we dont have the control anymore
        m_observer->UpdateLostControl();

    } else if(line.StartsWith(wxT("*stopped"))) {
        // get the stop reason,
        if(line == wxT("*stopped")) {
            if(m_bpList.empty()) {

                ExecuteCmd(wxT("set auto-solib-add off"));
                ExecuteCmd(wxT("set stop-on-solib-events 0"));

            } else {

                // no reason for the failure, this means that we stopped due to
                // hitting a loading of shared library
                // try to place all breakpoints which previously failed
                SetBreakpoints();
            }

            Continue();

        } else {
            // GDB/MI Out-of-band Records
            // caused by async command, this line indicates that we have the control back
            DbgCmdHandler* handler = PopHandler(id);
            if(handler) {
                handler->ProcessOutput(line);
                delete handler;
            }
        }
    }
}

bool DbgGdb::EvaluateExpressionToString(const wxString& expression, const wxString& format)
{
    static int counter(0);
    wxString watchName(wxT("watch_num_"));
    watchName << ++counter;

    wxString command;
    command << wxT("-var-create ") << watchName << wxT(" * ") << expression;
    // first create the expression
    bool res = WriteCommand(command, new DbgCmdHandlerVarCreator(m_observer));
    if(!res) {
        // probably gdb is down
        return false;
    }

    command.clear();
    command << wxT("-var-set-format ") << watchName << wxT(" ") << format;
    // first create the expression
    res = WriteCommand(command, NULL);
    if(!res) {
        // probably gdb is down
        return false;
    }

    // execute the watch command
    command.clear();
    command << wxT("-var-evaluate-expression ") << watchName;
    res = WriteCommand(command, new DbgCmdHandlerEvalExpr(m_observer, expression));
    if(!res) {
        // probably gdb is down
        return false;
    }

    // and make sure we delete this variable
    return DeleteVariableObject(watchName);
}

bool DbgGdb::ListFrames()
{
    int max = m_info.maxCallStackFrames;
#ifndef __WXMAC__
    wxString command = wxString::Format("-stack-list-frames 0 %i", max);

#else
    // Under OSX GDB version is old and does not support the min-max version
    // of callstack info
    wxString command = "-stack-list-frames";

#endif

    return WriteCommand(command, new DbgCmdStackList(m_observer));
}

bool DbgGdb::SetFrame(int frame)
{
    wxString command;
    command << wxT("frame ") << frame;
    return WriteCommand(command, new DbgCmdSelectFrame(m_observer));
}

bool DbgGdb::ListThreads() { return WriteCommand(wxT("-thread-info"), new DbgCmdListThreads(m_observer)); }

bool DbgGdb::SelectThread(long threadId)
{
    wxString command;
    command << wxT("-thread-select ") << threadId;
    return WriteCommand(command, NULL);
}

void DbgGdb::OnProcessEnd(clProcessEvent& e)
{
    DoCleanup();
    m_observer->UpdateGotControl(DBG_EXITED_NORMALLY);
}

bool DbgGdb::GetAsciiViewerContent(const wxString& dbgCommand, const wxString& expression)
{
    wxString cmd;
    cmd << dbgCommand << wxT(" ") << expression;

    return ExecCLICommand(cmd, new DbgCmdGetTipHandler(m_observer, expression));
}

bool DbgGdb::ResolveType(const wxString& expression, int userReason)
{
    wxString cmd;
    cmd << wxT("-var-create - * \"") << expression << wxT("\"");
    return WriteCommand(cmd, new DbgCmdResolveTypeHandler(expression, this, userReason));
}

bool DbgGdb::WatchMemory(const wxString& address, size_t count, size_t columns)
{
    // make the line per WORD size
    int divider(columns);
    int factor((int)(count / divider));
    if(count % divider != 0) {
        factor = (int)(count / divider) + 1;
    }

    // at this point, 'factor' contains the number rows
    // and the 'divider' is the columns
    wxString cmd;
    cmd << wxT("-data-read-memory \"") << address << wxT("\" x 1 ") << factor << wxT(" ") << divider << wxT(" ?");
    return WriteCommand(cmd, new DbgCmdWatchMemory(m_observer, address, count, columns));
}

bool DbgGdb::SetMemory(const wxString& address, size_t count, const wxString& hex_value)
{
    wxString cmd;
    wxString hexCommaDlimArr;
    wxArrayString hexArr = wxStringTokenize(hex_value, wxT(" "), wxTOKEN_STRTOK);

    for(size_t i = 0; i < hexArr.GetCount(); i++) {
        hexCommaDlimArr << hexArr.Item(i) << wxT(",");
    }

    hexCommaDlimArr.RemoveLast();
    cmd << wxT("set {char[") << count << wxT("]}") << address << wxT("={") << hexCommaDlimArr << wxT("}");

    return ExecuteCmd(cmd);
}

void DbgGdb::SetDebuggerInformation(const DebuggerInformation& info)
{
    IDebugger::SetDebuggerInformation(info);
    m_consoleFinder.SetConsoleCommand(info.consoleCommand);
}

void DbgGdb::BreakList() { (void)WriteCommand(wxT("-break-list"), new DbgCmdBreakList(m_observer)); }

bool DbgGdb::DoLocateGdbExecutable(const wxString& debuggerPath, wxString& dbgExeName)
{
    if(m_gdbProcess) {
        // dont allow second instance of the debugger
        return false;
    }
    wxString cmd;

    dbgExeName = debuggerPath;
    if(dbgExeName.IsEmpty()) {
        dbgExeName = wxT("gdb");
    }

    wxString actualPath;
    if(ExeLocator::Locate(dbgExeName, actualPath) == false) {
        wxMessageBox(wxString::Format(wxT("Failed to locate gdb! at '%s'"), dbgExeName.c_str()), wxT("CodeLite"));
        return false;
    }

    // set the debugger specific startup commands
    wxString startupInfo(m_info.startupCommands);

    // We must replace TABS with spaces or else gdb will hang...
    startupInfo.Replace(wxT("\t"), wxT(" "));

    // Write the content into a file
    wxString codelite_gdbinit_file;
    codelite_gdbinit_file << wxFileName::GetTempDir() << wxFileName::GetPathSeparator() << wxT("codelite_gdbinit.txt");

    wxFFile file;
    if(!file.Open(codelite_gdbinit_file, wxT("w+b"))) {
        m_observer->UpdateAddLine(
            wxString::Format(wxT("Failed to generate gdbinit file at %s"), codelite_gdbinit_file.c_str()));

    } else {
        m_observer->UpdateAddLine(wxString::Format(wxT("Using gdbinit file: %s"), codelite_gdbinit_file.c_str()));
        file.Write(startupInfo);
        file.Close();

        dbgExeName << wxT(" --command=\"") << codelite_gdbinit_file << wxT("\"");
    }

    return true;
}

// Initialization stage
bool DbgGdb::DoInitializeGdb(const DebugSessionInfo& sessionInfo)
{
    m_goingDown = false;
    m_internalBpId = wxNOT_FOUND;
#ifdef __WXMSW__
    ExecuteCmd(wxT("set  new-console on"));
#endif
    ExecuteCmd(wxT("set unwindonsignal on"));

    wxString breakinsertcmd(wxT("-break-insert "));

    if(m_info.enablePendingBreakpoints) {
        ExecuteCmd(wxT("set breakpoint pending on"));
        breakinsertcmd << wxT("-f ");
    }

    if(m_info.catchThrow) {
#ifdef __WXMSW__
        WriteCommand("-break-insert -f __cxa_throw", NULL);
#else
        ExecuteCmd(wxT("catch throw"));
#endif
    }

#ifdef __WXMSW__
    if(m_info.debugAsserts) {
        ExecuteCmd(wxT("break assert"));
    }
#endif

    if(!(m_info.flags & DebuggerInformation::kPrintObjectOff)) {
        ExecuteCmd("set print object on");
    }

    ExecuteCmd(wxT("set width 0"));
    ExecuteCmd(wxT("set height 0"));

    // Number of elements to show for arrays (including strings)
    wxString sizeCommand;
    sizeCommand << wxT("set print elements ") << m_info.maxDisplayStringSize;
    ExecuteCmd(sizeCommand);

    // set the project startup commands
    for(size_t i = 0; i < sessionInfo.cmds.GetCount(); i++) {
        ExecuteCmd(sessionInfo.cmds.Item(i));
    }

    // keep the list of breakpoints
    m_bpList = sessionInfo.bpList;

    bool setBreakpointsAfterMain(m_info.applyBreakpointsAfterProgramStarted);
    if(GetIsRemoteDebugging() == false && !setBreakpointsAfterMain) {
        // When remote debugging, apply the breakpoints after we connect the
        // gdbserver
        SetBreakpoints();

    } else if(setBreakpointsAfterMain && m_bpList.empty() == false) {
        // Place an 'internal' breakpoint at main. Once this breakpoint is hit
        // set all breakpoints and remove the 'internal' one.
        // Then 'continue', unless the user has said he actually _wants_ to break at main
        WriteCommand(breakinsertcmd + wxT("-t main"), new DbgFindMainBreakpointIdHandler(m_observer, this));
    }

    if(m_info.breakAtWinMain) {
        // Set a breakpoint at WinMain
        // Use a temporary one, so that it isn't duplicated in future sessions
        WriteCommand(breakinsertcmd + wxT("-t main"), NULL);
        // Flag that we've done this. DbgFindMainBreakpointIdHandler::ProcessOutput uses this
        // to decide whether or not to 'continue' after setting BPs after main()
        SetShouldBreakAtMain(true);
    } else {
        SetShouldBreakAtMain(false); // Needs explicitly to be set, in case the user has just changed his options
    }

    // Enable python based pretty printing?
    if(sessionInfo.enablePrettyPrinting) {
        WriteCommand(wxT("-enable-pretty-printing"), NULL);
    }

    // Add the additional search paths
    for(size_t i = 0; i < sessionInfo.searchPaths.GetCount(); ++i) {
        wxString dirCmd;
        wxString path = sessionInfo.searchPaths.Item(i);
        path.Trim().Trim(false);
        if(path.Contains(" ")) {
            path.Prepend('"').Append('"');
        }
        dirCmd << "-environment-directory " << path;
        WriteCommand(dirCmd, NULL);
    }
    return true;
}

bool DbgGdb::ExecCLICommand(const wxString& command, DbgCmdCLIHandler* handler)
{
    wxString cmd;
    wxString id = MakeId();
    cmd << id << command;
    // send the command to gdb
    if(!ExecuteCmd(cmd)) {
        return false;
    }

    if(handler) {
        handler->SetCommandId(id);
        SetCliHandler(handler);
    }
    return true;
}

void DbgGdb::SetCliHandler(DbgCmdCLIHandler* handler)
{
    if(m_cliHandler) {
        delete m_cliHandler;
    }
    m_cliHandler = handler;
}

DbgCmdCLIHandler* DbgGdb::GetCliHandler() { return m_cliHandler; }

bool DbgGdb::ListChildren(const wxString& name, int userReason)
{
    wxString cmd;
    // use -var-list-children 2 ("--simple-values")
    cmd << wxT("-var-list-children \"") << name << wxT("\"");
    return WriteCommand(cmd, new DbgCmdListChildren(m_observer, name, userReason));
}

bool DbgGdb::CreateVariableObject(const wxString& expression, bool persistent, int userReason)
{
    wxString cmd;
    cmd << wxT("-var-create - ");

    if(persistent) {
        cmd << wxT("* ");
    } else {
        cmd << wxT("@ ");
    }
    cmd << wxT("\"") << expression << wxT("\"");
    return WriteCommand(cmd, new DbgCmdCreateVarObj(m_observer, this, expression, userReason));
}

bool DbgGdb::DeleteVariableObject(const wxString& name)
{
    wxString cmd;
    cmd << wxT("-var-delete \"") << name << wxT("\"");
    return WriteCommand(cmd, NULL);
}

bool DbgGdb::EvaluateVariableObject(const wxString& name, int userReason)
{
    wxString cmd;
    cmd << wxT("-var-evaluate-expression \"") << name << wxT("\"");
    // CL_DEBUG("GDB>> %s", cmd);
    return WriteCommand(cmd, new DbgCmdEvalVarObj(m_observer, name, userReason));
}

void DbgGdb::OnDataRead(clProcessEvent& e)
{
    // Data arrived from the debugger
    wxString bufferRead;
    bufferRead << e.GetOutput();

    if(!m_gdbProcess || !m_gdbProcess->IsAlive()) return;

    CL_DEBUG("GDB>> %s", bufferRead);
    wxArrayString lines = wxStringTokenize(bufferRead, wxT("\n"), wxTOKEN_STRTOK);
    if(lines.IsEmpty()) return;

    // Prepend the partially saved line from previous iteration to the first line
    // of this iteration
    lines.Item(0).Prepend(m_gdbOutputIncompleteLine);
    m_gdbOutputIncompleteLine.Clear();

    // If the last line is in-complete, remove it from the array and keep it for next iteration
    if(!bufferRead.EndsWith(wxT("\n"))) {
        m_gdbOutputIncompleteLine = lines.Last();
        lines.RemoveAt(lines.GetCount() - 1);
    }

    for(size_t i = 0; i < lines.GetCount(); i++) {
        wxString line = lines.Item(i);

        line.Replace(wxT("(gdb)"), wxT(""));
        line.Trim().Trim(false);
        if(line.IsEmpty() == false) {
            m_gdbOutputArr.Add(line);
        }
    }

    if(m_gdbOutputArr.IsEmpty() == false) {
        // Trigger GDB processing
        Poke();
    }
}

bool DbgGdb::DoGetNextLine(wxString& line)
{
    line.Clear();
    if(m_gdbOutputArr.IsEmpty()) {
        return false;
    }
    line = m_gdbOutputArr.Item(0);
    m_gdbOutputArr.RemoveAt(0);
    line.Replace(wxT("(gdb)"), wxT(""));
    line.Trim().Trim(false);
    if(line.IsEmpty()) {
        return false;
    }
    return true;
}

void DbgGdb::SetInternalMainBpID(int bpId) { m_internalBpId = bpId; }

bool DbgGdb::Restart() { return WriteCommand(wxT("-exec-run "), new DbgCmdHandlerExecRun(m_observer, this)); }

bool DbgGdb::SetVariableObbjectDisplayFormat(const wxString& name, DisplayFormat displayFormat)
{
    wxString df, cmd;
    switch(displayFormat) {
    case DBG_DF_BINARY:
        df = wxT("binary");
        break;
    case DBG_DF_DECIMAL:
        df = wxT("decimal");
        break;
    case DBG_DF_HEXADECIMAL:
        df = wxT("hexadecimal");
        break;
    case DBG_DF_OCTAL:
        df = wxT("octal");
        break;
    default:
    case DBG_DF_NATURAL:
        df = wxT("natural");
        break;
    }

    cmd << wxT("-var-set-format \"") << name << wxT("\" ") << df;
    return WriteCommand(cmd, NULL);
}

bool DbgGdb::UpdateVariableObject(const wxString& name, int userReason)
{
    // FIXME: this seems to cause a mess in gdb output, disable it for now

    // wxString cmd;
    // cmd << wxT("-var-update \"") << name << wxT("\" ");
    // return WriteCommand(cmd, new DbgVarObjUpdate(m_observer, this, name, userReason));
    return true;
}

bool DbgGdb::UpdateWatch(const wxString& name)
{
    wxString cmd;
    cmd << wxT("-var-update \"") << name << wxT("\" ");
    return WriteCommand(cmd, new DbgVarObjUpdate(m_observer, this, name, DBG_USERR_WATCHTABLE));
}

void DbgGdb::AssignValue(const wxString& expression, const wxString& newValue)
{
    wxString cmd;
    cmd << wxT("set variable ") << expression << wxT("=") << newValue;
    ExecuteCmd(cmd);
}

void DbgGdb::GetDebugeePID(const wxString& line)
{
    if(m_debuggeePid == wxNOT_FOUND) {
        if(GetIsRemoteDebugging()) {
            m_debuggeePid = m_gdbProcess->GetPid();

        } else {

            static wxRegEx reDebuggerPidWin(wxT("New Thread ([0-9]+)\\.(0[xX][0-9a-fA-F]+)"));
            static wxRegEx reGroupStarted(wxT("id=\"([0-9]+)\""));
            static wxRegEx reSwitchToThread(wxT("Switching to process ([0-9]+)"));

            // test for the debuggee PID
            // in the line with the following pattern:
            // =thread-group-started,id="i1",pid="15599"
            if(m_debuggeePid < 0 && !line.IsEmpty()) {
                wxString debuggeePidStr;

                if(line.Contains(wxT("=thread-group-started")) && reGroupStarted.Matches(line)) {
                    debuggeePidStr = reGroupStarted.GetMatch(line, 1);

                } else if(line.Contains(wxT("=thread-group-created")) && reGroupStarted.Matches(line)) {
                    debuggeePidStr = reGroupStarted.GetMatch(line, 1);

                } else if(reDebuggerPidWin.Matches(line)) {
                    debuggeePidStr = reDebuggerPidWin.GetMatch(line, 1);

                } else if(reSwitchToThread.Matches(line)) {
                    debuggeePidStr = reSwitchToThread.GetMatch(line, 1);
                }

                if(!debuggeePidStr.IsEmpty()) {
                    long iPid(0);
                    if(debuggeePidStr.ToLong(&iPid)) {
                        m_debuggeePid = iPid;
                        wxString msg;
                        msg << wxT(">> Debuggee process ID: ") << m_debuggeePid;
                        m_observer->UpdateAddLine(msg);

                        // Now there's a known pid, the debugger can be interrupted to let any to-be-disabled bps be
                        // disabled. So...
                        m_observer->DebuggerPidValid();
                    }
                }
            }
        }
    }
}

bool DbgGdb::Jump(wxString filename, int line)
{
    BreakpointInfo bp;
    bp.Create(filename, line, -1);
    bp.bp_type = BP_type_tempbreak;
    Break(bp);

    // by default, use full paths for the file name
    wxFileName fn(filename);
    wxString tmpfileName(fn.GetFullPath());
    if(m_info.useRelativeFilePaths) {
        // user set the option to use relative paths (file name w/o the full path)
        tmpfileName = fn.GetFullName();
    }

    tmpfileName.Replace(wxT("\\"), wxT("/"));

    wxString command;
    command << wxT("-exec-jump ") << wxT("\"\\\"") << tmpfileName << wxT(":") << line << wxT("\\\"\"");
    // return WriteCommand( command, new DbgCmdHandlerAsyncCmd( m_observer, this ) );
    return ExecCLICommand(command, new DbgCmdJumpHandler(m_observer));
}

void DbgGdb::OnKillGDB(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoCleanup();
    m_observer->UpdateGotControl(DBG_DBGR_KILLED);
}

bool DbgGdb::Disassemble(const wxString& filename, int lineNumber)
{
    // Trigger a "disasseble" call
    if(/*filename.IsEmpty() || lineNumber == wxNOT_FOUND*/ true) {
        // Use the $pc
        if(!WriteCommand("-data-disassemble -s \"$pc -100\" -e \"$pc + 100\" -- 0",
               new DbgCmdHandlerDisasseble(m_observer, this)))
            return false;

    } else {
        // else, use the file and line provided
        wxString tmpfile = filename;
        tmpfile.Replace("\\", "/"); // gdb does not like backslashes...

        if(!WriteCommand(wxString() << "-data-disassemble -f \"" << tmpfile << "\" -l " << lineNumber << " -n -1 -- 0",
               new DbgCmdHandlerDisasseble(m_observer, this)))
            return false;
    }

    // get the current instruction
    if(!WriteCommand(
           "-data-disassemble -s \"$pc\" -e \"$pc + 1\" -- 0", new DbgCmdHandlerDisassebleCurLine(m_observer, this)))
        return false;

    return true;
}

bool DbgGdb::Attach(const DebugSessionInfo& si)
{
    // set the environment variables
    EnvSetter env(m_env, NULL, m_debuggeeProjectName, wxEmptyString);

    wxString dbgExeName;
    if(!DoLocateGdbExecutable(si.debuggerPath, dbgExeName)) {
        return false;
    }

    wxString cmd;
#if defined(__WXGTK__) || defined(__WXMAC__)
    cmd << dbgExeName;
    if(!si.ttyName.IsEmpty()) {
        cmd << wxT(" --tty=") << si.ttyName;
    }
    cmd << wxT(" --interpreter=mi ");
#else
    cmd << dbgExeName << wxT(" --interpreter=mi ");
    cmd << ProcUtils::GetProcessNameByPid(si.PID) << wxT(" ");
#endif

    // if(sudoCmd.IsEmpty() == false) {
    //    cmd.Prepend(sudoCmd + wxT(" "));
    //}

    m_attachedMode = true;
    m_debuggeePid = si.PID;
    cmd << wxT(" --pid=") << m_debuggeePid;
    wxLogMessage(cmd);

    m_observer->UpdateAddLine(wxString::Format(wxT("Current working dir: %s"), wxGetCwd().c_str()));
    m_observer->UpdateAddLine(wxString::Format(wxT("Launching gdb from : %s"), wxGetCwd().c_str()));
    m_observer->UpdateAddLine(wxString::Format(wxT("Starting debugger  : %s"), cmd.c_str()));
    
    // Build the process creation flags
    size_t createFlags = IProcessCreateDefault;
    if(m_info.flags & DebuggerInformation::kRunAsSuperuser) {
        createFlags |= IProcessCreateAsSuperuser;
    }
    m_gdbProcess = CreateAsyncProcess(this, cmd, createFlags);
    if(!m_gdbProcess) {
        return false;
    }
    m_gdbProcess->SetHardKill(true);

    DoInitializeGdb(si);
    m_observer->UpdateGotControl(DBG_END_STEPPING);
    return true;
}

bool DbgGdb::ListRegisters()
{
    return WriteCommand("-data-list-register-names", new DbgCmdHandlerRegisterNames(m_observer, this));
}

void DbgGdb::EnableReverseDebugging(bool b) { m_reverseDebugging = b; }

void DbgGdb::EnableRecording(bool b)
{
    if(b) {
        WriteCommand("target record-full", new DbgCmdRecordHandler(m_observer, this));
    } else {
        WriteCommand("record stop", NULL);

        // If recording is OFF, disable the reverse-debugging switch
        SetIsRecording(false);
        m_reverseDebugging = false;
    }
}

bool DbgGdb::IsReverseDebuggingEnabled() const { return m_reverseDebugging; }
