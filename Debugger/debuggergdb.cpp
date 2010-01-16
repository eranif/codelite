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

#ifdef __WXMSW__
#include "windows.h"

// On Windows lower than XP, the function DebugBreakProcess does not exist
// so we need to bind it dynamically
typedef WINBASEAPI BOOL WINAPI (*DBG_BREAK_PROC_FUNC_PTR)(HANDLE);
DBG_BREAK_PROC_FUNC_PTR DebugBreakProcessFunc = NULL;
HINSTANCE Kernel32Dll = NULL;

// define a dummy control handler
BOOL CtrlHandler( DWORD fdwCtrlType )
{
	wxUnusedVar(fdwCtrlType);

	// return FALSE so other process in our group are allowed to process this event
	return FALSE;
}
#endif

#if defined(__WXGTK__) || defined (__WXMAC__)
#include <sys/types.h>
#include <signal.h>
#endif

//Using the running image of child Thread 46912568064384 (LWP 7051).
static wxRegEx reInfoProgram1(wxT("\\(LWP[ \t]([0-9]+)\\)"));
//Using the running image of child process 10011.
static wxRegEx reInfoProgram2(wxT("child process ([0-9]+)"));
//Using the running image of child thread 4124.0x117c
static wxRegEx reInfoProgram3(wxT("Using the running image of child thread ([0-9]+)"));

#ifdef __WXMSW__
static wxRegEx reConnectionRefused(wxT("[0-9a-zA-Z/\\\\-\\_]*:[0-9]+: No connection could be made because the target machine actively refused it."));
#else
static wxRegEx reConnectionRefused(wxT("[0-9a-zA-Z/\\\\-\\_]*:[0-9]+: Connection refused."));
#endif
DebuggerInfo GetDebuggerInfo()
{
	DebuggerInfo info = {
		wxT("GNU gdb debugger"),
		wxT("CreateDebuggerGDB"),
		wxT("v2.0"),
		wxT("Eran Ifrah")
	};
	return info;
}

IDebugger *CreateDebuggerGDB()
{
	static DbgGdb theGdbDebugger;
	theGdbDebugger.SetName(wxT("GNU gdb debugger"));
	return &theGdbDebugger;
}

// Removes MI additional characters from string
static void StripString(wxString &string)
{
	string.Replace(wxT("\\n\""), wxT("\""));
	string = string.AfterFirst(wxT('"'));
	string = string.BeforeLast(wxT('"'));
	string.Replace(wxT("\\\""), wxT("\""));
	string.Replace(wxT("\\\\"), wxT("\\"));
	string.Replace(wxT("\\\\r\\\\n"), wxT("\r\n"));
	string.Replace(wxT("\\\\n"), wxT("\n"));
	string.Replace(wxT("\\\\r"), wxT("\r"));
	string = string.Trim();
}

static wxString MakeId()
{
	static size_t counter(0);
	wxString newId;
	newId.Printf(wxT("%08d"), ++counter);
	return newId;
}

BEGIN_EVENT_TABLE(DbgGdb, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  DbgGdb::OnDataRead)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, DbgGdb::OnProcessEnd)
END_EVENT_TABLE()

DbgGdb::DbgGdb()
		: m_debuggeePid(wxNOT_FOUND)
		, m_cliHandler (NULL)
{
#ifdef __WXMSW__
	Kernel32Dll = LoadLibrary(wxT("kernel32.dll"));
	if (Kernel32Dll) {
		DebugBreakProcessFunc = (DBG_BREAK_PROC_FUNC_PTR)GetProcAddress(Kernel32Dll, "DebugBreakProcess");
	} else {
		// we dont have DebugBreakProcess, try to work with Control handlers
		if (SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE ) == FALSE) {
			wxLogMessage(wxString::Format(wxT("failed to install ConsoleCtrlHandler: %d"), GetLastError()));
		}
	}
	if (SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE ) == FALSE) {
		wxLogMessage(wxString::Format(wxT("failed to install ConsoleCtrlHandler: %d"), GetLastError()));
	}
#endif
}

DbgGdb::~DbgGdb()
{
#ifdef __WXMSW__
	if ( Kernel32Dll ) {
		FreeLibrary(Kernel32Dll);
		Kernel32Dll = NULL;
	}
#endif
}

void DbgGdb::RegisterHandler(const wxString &id, DbgCmdHandler *cmd)
{
	m_handlers[id] = cmd;
}

DbgCmdHandler *DbgGdb::PopHandler(const wxString &id)
{
	HandlersMap::iterator it = m_handlers.find(id);
	if (it == m_handlers.end()) {
		return NULL;
	}
	DbgCmdHandler *cmd = it->second;
	m_handlers.erase(it);
	return cmd;
}

void DbgGdb::EmptyQueue()
{
	HandlersMap::iterator iter = m_handlers.begin();
	while (iter != m_handlers.end()) {
		delete iter->second;
		iter++;
	}
	m_handlers.clear();
}

bool DbgGdb::Start(const wxString &debuggerPath, const wxString & exeName, int pid, const std::vector<BreakpointInfo> &bpList, const wxArrayString &cmds)
{
	wxString dbgExeName;
	if ( ! DoLocateGdbExecutable(debuggerPath, dbgExeName) ) {
		return false;
	}

	wxString cmd;

#if defined (__WXGTK__) || defined (__WXMAC__)
	//On GTK and other platforms, open a new terminal and direct all
	//debugee process into it
	wxString ptyName;
	if (!m_consoleFinder.FindConsole(wxT("CodeLite: gdb"), ptyName)) {
		wxLogMessage(wxT("Failed to allocate console for debugger"));
		return false;
	}
	cmd << dbgExeName << wxT(" --tty=") << ptyName << wxT(" --interpreter=mi ");
#else
	cmd << dbgExeName << wxT(" --interpreter=mi ");
	cmd << ProcUtils::GetProcessNameByPid(pid) << wxT(" ");
#endif

	m_debuggeePid = pid;
	cmd << wxT(" --pid=") << m_debuggeePid;
	wxLogMessage(cmd);

	//set the environment variables
	EnvSetter env(m_env);

	m_observer->UpdateAddLine(wxString::Format(wxT("Current working dir: %s"), wxGetCwd().c_str()));
	m_observer->UpdateAddLine(wxString::Format(wxT("Launching gdb from : %s"), wxGetCwd().c_str()));
	m_observer->UpdateAddLine(wxString::Format(wxT("Starting debugger  : %s"), cmd.c_str()));

	m_gdbProcess = CreateAsyncProcess(this, cmd);
	if (!m_gdbProcess) {
		return false;
	}

	DoInitializeGdb(bpList, cmds);
	m_observer->UpdateGotControl(DBG_END_STEPPING);
	return true;
}

bool DbgGdb::Start(const wxString &debuggerPath, const wxString &exeName, const wxString &cwd, const std::vector<BreakpointInfo> &bpList, const wxArrayString &cmds)
{

	wxString dbgExeName;
	if ( ! DoLocateGdbExecutable(debuggerPath, dbgExeName) ) {
		return false;
	}

	wxString cmd;
#if defined (__WXGTK__) || defined (__WXMAC__)
	//On GTK and other platforms, open a new terminal and direct all
	//debugee process into it
	wxString ptyName;
	if (!m_consoleFinder.FindConsole(exeName, ptyName)) {
		wxLogMessage(wxT("Failed to allocate console for debugger, do u have Xterm installed?"));
		return false;
	}
	cmd << dbgExeName << wxT(" --tty=") << ptyName << wxT(" --interpreter=mi ") << exeName;
#else
	cmd << dbgExeName << wxT(" --interpreter=mi ") << exeName;
#endif

	m_debuggeePid = wxNOT_FOUND;

	//set the environment variables
	EnvSetter env(m_env);

	m_observer->UpdateAddLine(wxString::Format(wxT("Current working dir: %s"), wxGetCwd().c_str()));
	m_observer->UpdateAddLine(wxString::Format(wxT("Launching gdb from : %s"), cwd.c_str()));
	m_observer->UpdateAddLine(wxString::Format(wxT("Starting debugger  : %s"), cmd.c_str()));
	m_gdbProcess = CreateAsyncProcess(this, cmd, cwd);
	if (!m_gdbProcess) {
		return false;
	}

	DoInitializeGdb(bpList, cmds);
	return true;
}

bool DbgGdb::WriteCommand(const wxString &command, DbgCmdHandler *handler)
{
	wxString cmd;
	wxString id = MakeId( );
	cmd << id << command;

	if (!ExecuteCmd(cmd)) {
		return false;
	}
	RegisterHandler(id, handler);
	return true;
}

bool DbgGdb::Start(const wxString &exeName, const wxString &cwd, const std::vector<BreakpointInfo> &bpList, const wxArrayString &cmds)
{
	return Start(wxT("gdb"), exeName, cwd, bpList, cmds);
}

bool DbgGdb::Run(const wxString &args, const wxString &comm)
{
	if ( !GetIsRemoteDebugging() ) {

		// add handler for this command
		return WriteCommand(wxT("-exec-run ") + args, new DbgCmdHandlerAsyncCmd(m_observer));

	} else {
		// attach to the remote gdb server
		wxString cmd;
		//cmd << wxT("-target-select remote ") << comm << wxT(" ") << args;
		cmd << wxT("target remote ") << comm << wxT(" ") << args;
		return WriteCommand(cmd, new DbgCmdHandlerRemoteDebugging(m_observer, this));
		
	}
}

bool DbgGdb::Stop()
{
	if (m_gdbProcess) {
		delete m_gdbProcess;
		m_gdbProcess = NULL;
	}

	//free allocated console for this session
	m_consoleFinder.FreeConsole();

	//return control to the program
	m_observer->UpdateGotControl(DBG_DBGR_KILLED);
	EmptyQueue();
	m_gdbOutputArr.Clear();
	m_bpList.clear();
	return true;
}

bool DbgGdb::Next()
{
	return WriteCommand(wxT("-exec-next"), new DbgCmdHandlerAsyncCmd(m_observer));
}

void DbgGdb::SetBreakpoints()
{
	for (size_t i=0; i< m_bpList.size(); i++) {
		// Without the 'unnecessary' cast in the next line, bpinfo.bp_type is seen as (e.g.) 4 instead of BP_type_tempbreak, ruining switch statments :/
		BreakpointInfo bpinfo = (BreakpointInfo)m_bpList.at(i);
		Break(bpinfo);
	}
}

bool DbgGdb::Break(const BreakpointInfo& bp)
{
	wxFileName fn(bp.file);

	// by default, use full paths for the file name when setting breakpoints
	wxString tmpfileName(fn.GetFullPath());;
	if (m_info.useRelativeFilePaths) {
		// user set the option to use relative paths (file name w/o the full path)
		tmpfileName = fn.GetFullName();
	}

	tmpfileName.Replace(wxT("\\"), wxT("/"));

	wxString command;
	switch (bp.bp_type) {
	case BP_type_watchpt:
		//----------------------------------
		// Watchpoints
		//----------------------------------
		command = wxT("-break-watch ");
		switch (bp.watchpoint_type) {
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
		command = wxT("-break-insert -t ");
		break;

	case BP_type_condbreak:
	case BP_type_break:
	default:
		// Should be standard breakpts. But if someone tries to make an ignored temp bp
		// it won't have the BP_type_tempbreak type, so check again here
		command =  (bp.is_temp ? wxT("-break-insert -t ") : wxT("-break-insert "));
		break;
	}

	//------------------------------------------------------------------------
	// prepare the 'break where' string (address, file:line or regex)
	//------------------------------------------------------------------------
	wxString breakWhere, ignoreCounnt, condition, gdbCommand;
	if (bp.memory_address.IsEmpty() == false) {

		// Memory is easy: just prepend *. gdb copes happily with (at least) hex or decimal
		breakWhere << wxT('*') << bp.memory_address;

	} else if (bp.bp_type != BP_type_watchpt) {
		// Function and Lineno locations can/should be prepended by a filename (but see later)
		if (! tmpfileName.IsEmpty() && bp.lineno > 0 ) {
			breakWhere << wxT("\"\\\"") << tmpfileName << wxT(":") << bp.lineno << wxT("\\\"\"");
		} else if (! bp.function_name.IsEmpty()) {
			if (bp.regex) {
				// update the command
				command = wxT("-break-insert -r ");
			}
			breakWhere = bp.function_name;
		}
	}

	//------------------------------------------------------------------------
	// prepare the conditions
	//------------------------------------------------------------------------
	if (bp.conditions.IsEmpty() == false) {
		condition << wxT("-c ") << wxT("\"") << bp.conditions << wxT("\" ");
	}

	//------------------------------------------------------------------------
	// prepare the ignore count
	//------------------------------------------------------------------------
	if (bp.ignore_number > 0 ) {
		ignoreCounnt << wxT("-i ") << bp.ignore_number << wxT(" ");
	}

	// concatenate all the string into one command to pass to gdb
	gdbCommand << command << condition << ignoreCounnt << breakWhere;

	// execute it
	return WriteCommand(gdbCommand, new DbgCmdHandlerBp(m_observer, this, bp, &m_bpList, bp.bp_type));
}

bool DbgGdb::SetIgnoreLevel(const int bid, const int ignorecount)
{
	if (bid == -1) {	// Sanity check
		return false;
	}

	wxString command(wxT("-break-after "));
	command << bid << wxT(" ") << ignorecount;
	return WriteCommand(command, NULL);
}

bool DbgGdb::SetEnabledState(const int bid, const bool enable)
{
	if (bid == -1) {	// Sanity check
		return false;
	}

	wxString command(wxT("-break-disable "));
	if (enable) {
		command = wxT("-break-enable ");
	}
	command << bid;
	return WriteCommand(command, NULL);
}

bool DbgGdb::SetCondition(const BreakpointInfo& bp)
{
	if (bp.debugger_id == -1) {	// Sanity check
		return false;
	}

	wxString command(wxT("-break-condition "));
	command << bp.debugger_id << wxT(" ") << bp.conditions;
	return WriteCommand(command, new DbgCmdSetConditionHandler(m_observer, bp));
}

bool DbgGdb::SetCommands(const BreakpointInfo& bp)
{
	if (bp.debugger_id == -1) {	// Sanity check
		return false;
	}
	// There isn't (currentl) a MI command-list command, so use the CLI one
	// This doesn't actually work either, but at least the commands are visible in -break-list
	wxString command(wxT("commands "));
	command << bp.debugger_id << wxT('\n') << bp.commandlist << wxT("\nend");

	if (m_info.enableDebugLog) {
		m_observer->UpdateAddLine(command);
	}

	// If we really wanted, we could get the output (for bp 3) of "commands 3"
	// but as that's not very informative, and we're only faking the command-list anyway, don't bother
	return WriteCommand(command, NULL);
}

bool DbgGdb::Continue()
{
	return WriteCommand(wxT("-exec-continue"), new DbgCmdHandlerAsyncCmd(m_observer));
}

bool DbgGdb::StepIn()
{
	return WriteCommand(wxT("-exec-step"), new DbgCmdHandlerAsyncCmd(m_observer));
}

bool DbgGdb::StepOut()
{
	return WriteCommand(wxT("-exec-finish"), new DbgCmdHandlerAsyncCmd(m_observer));
}

bool DbgGdb::IsRunning()
{
	return m_gdbProcess != NULL;
}

bool DbgGdb::Interrupt()
{
	if (m_debuggeePid > 0) {
		m_observer->UpdateAddLine(wxString::Format(wxT("Interrupting debugee process: %d"), m_debuggeePid));

#ifdef __WXMSW__
		if ( DebugBreakProcessFunc ) {
			// we have DebugBreakProcess
			HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)m_debuggeePid);
			BOOL res = DebugBreakProcessFunc(process);
			return res == TRUE;
		}
		// on Windows version < XP we need to find a solution for interrupting the
		// debuggee process
		return false;
#else
		kill(m_debuggeePid, SIGINT);
		return true;
#endif
	}
	return false;
}

bool DbgGdb::QueryFileLine()
{
#if defined (__WXMSW__) || defined (__WXGTK__)
	return WriteCommand(wxT("-file-list-exec-source-file"), new DbgCmdHandlerGetLine(m_observer));
#else
	//Mac
	return WriteCommand(wxT("-stack-list-frames 0 0"), new DbgCmdHandlerGetLine(m_observer));
#endif
}

bool DbgGdb::QueryLocals()
{
	bool res = WriteCommand(wxT("-stack-list-locals 2"), new DbgCmdHandlerLocals(m_observer));
	if ( !res ) {
		return false;
	}
	return WriteCommand(wxT("-stack-list-arguments 2 0 0 "), new DbgCmdHandlerFuncArgs(m_observer));
}

bool DbgGdb::ExecuteCmd(const wxString &cmd)
{
	if( m_gdbProcess ) {
		if (m_info.enableDebugLog) {
			m_observer->UpdateAddLine(wxString::Format(wxT("DEBUG>>%s"), cmd.c_str()));
		}
		return m_gdbProcess->Write(cmd);
	}
	return false;
}

bool DbgGdb::RemoveAllBreaks()
{
	return ExecuteCmd(wxT("delete"));
}

bool DbgGdb::RemoveBreak(int bid)
{
	wxString command;
	command << wxT("-break-delete ") << bid;
	return WriteCommand(command, NULL);
}

bool DbgGdb::FilterMessage(const wxString &msg)
{
	wxString tmpmsg ( msg );
	StripString( tmpmsg );
	tmpmsg.Trim().Trim(false);

	if (tmpmsg.Contains(wxT("Variable object not found")) || msg.Contains(wxT("Variable object not found"))) {
		return true;
	}

	if (tmpmsg.Contains(wxT("mi_cmd_var_create: unable to create variable object"))||msg.Contains(wxT("mi_cmd_var_create: unable to create variable object"))) {
		return true;
	}

	if (tmpmsg.Contains(wxT("Variable object not found"))|| msg.Contains(wxT("Variable object not found"))) {
		return true;
	}

	if (tmpmsg.Contains(wxT("No symbol \"this\" in current context"))||msg.Contains(wxT("No symbol \"this\" in current context"))) {
		return true;
	}

	if ( tmpmsg.Contains(wxT("*running,thread-id")) ) {
		return true;
	}

	if (tmpmsg.StartsWith(wxT(">"))||msg.StartsWith(wxT(">"))) {
		// shell line
		return true;
	}
	return false;
}

void DbgGdb::Poke()
{
	static wxRegEx reCommand(wxT("^([0-9]{8})"));

	//poll the debugger output
	wxString line;
	if ( !m_gdbProcess || m_gdbOutputArr.IsEmpty() ) {
		return;
	}

	if (m_debuggeePid == wxNOT_FOUND) {
		if (GetIsRemoteDebugging()) {
			m_debuggeePid = m_gdbProcess->GetPid();
			
		} else {
			std::vector<long> children;
			ProcUtils::GetChildren(m_gdbProcess->GetPid(), children);
			std::sort(children.begin(), children.end());
			if (children.empty() == false) {
				m_debuggeePid = children.at(0);
			}

			if (m_debuggeePid != wxNOT_FOUND) {
				wxString msg;
				msg << wxT("Debuggee process ID: ") << m_debuggeePid;
				m_observer->UpdateAddLine(msg);
			}
		}
	}

	while ( DoGetNextLine( line ) ) {

		// For string manipulations without damaging the original line read
		wxString tmpline ( line );
		StripString( tmpline );
		tmpline.Trim().Trim(false);

		if (m_info.enableDebugLog) {
			//Is logging enabled?

			if (line.IsEmpty() == false && !tmpline.StartsWith(wxT(">")) ) {
				wxString strdebug(wxT("DEBUG>>"));
				strdebug << line;
				m_observer->UpdateAddLine(strdebug);
			}
		}

		if (reConnectionRefused.Matches(line)) {
			StripString(line);
#ifdef __WXGTK__
			m_consoleFinder.FreeConsole();
#endif
			m_observer->UpdateAddLine(line);
			m_observer->UpdateGotControl(DBG_EXITED_NORMALLY);
			return;
		}

		if( tmpline.StartsWith(wxT(">")) ) {
			// Shell line, probably user command line
			continue;
		}

		if (line.StartsWith(wxT("~")) || line.StartsWith(wxT("&"))) {

			// lines starting with ~ are considered "console stream" message
			// and are important to the CLI handler
			bool consoleStream(false);
			if ( line.StartsWith(wxT("~")) ) {
				consoleStream = true;
			}

			// Filter out some gdb error lines...
			if (FilterMessage(line)) {
				continue;
			}

			StripString( line );

			// If we got a valid "CLI Handler" instead of writing the output to
			// the output view, concatenate it into the handler buffer
			if ( GetCliHandler() && consoleStream ) {
				GetCliHandler()->Append( line );
			} else if ( consoleStream ) {
				// log message
				m_observer->UpdateAddLine( line );
			}
		} else if (reCommand.Matches(line)) {

			//not a gdb message, get the command associated with the message
			wxString id = reCommand.GetMatch(line, 1);

			if ( GetCliHandler() && GetCliHandler()->GetCommandId() == id ) {
				// probably the "^done" message of the CLI command
				GetCliHandler()->ProcessOutput( line );
				SetCliHandler( NULL ); // we are done processing the CLI

			} else {
				//strip the id from the line
				line = line.Mid(8);
				DoProcessAsyncCommand(line, id);

			}
		} else if (line.StartsWith(wxT("^done")) || line.StartsWith(wxT("*stopped"))) {
			//Unregistered command, use the default AsyncCommand handler to process the line
			DbgCmdHandlerAsyncCmd cmd(m_observer);
			cmd.ProcessOutput(line);
		} else {
			//Unknow format, just log it
			if( m_info.enableDebugLog && !FilterMessage(line)) {
				m_observer->UpdateAddLine(line);
			}
		}
	}
}

void DbgGdb::DoProcessAsyncCommand(wxString &line, wxString &id)
{
	if (line.StartsWith(wxT("^error"))) {

		// the command was error, for example:
		// finish in the outer most frame
		// print the error message and remove the command from the queue
		DbgCmdHandler *handler = PopHandler(id);

		if (handler && handler->WantsErrors()) {
			handler->ProcessOutput(line);
		}

		if (handler) {
			delete handler;
		}

		StripString(line);
		//We also need to pass the control back to the program
		m_observer->UpdateGotControl(DBG_CMD_ERROR);

		if ( !FilterMessage(line) && m_info.enableDebugLog ) {
			m_observer->UpdateAddLine(line);
		}

	} else if (line.StartsWith(wxT("^done")) || line.StartsWith(wxT("^connected"))) {
		//The synchronous operation was successful, results are the return values.
		DbgCmdHandler *handler = PopHandler(id);
		if (handler) {
			handler->ProcessOutput(line);
			delete handler;
		}

	} else if (line.StartsWith(wxT("^running"))) {
		//asynchronous command was executed
		//send event that we dont have the control anymore
		m_observer->UpdateLostControl();
		
	} else if (line.StartsWith(wxT("*stopped"))) {
		//get the stop reason,
		if (line == wxT("*stopped")) {
			if (m_bpList.empty()) {

				ExecuteCmd(wxT("set auto-solib-add off"));
				ExecuteCmd(wxT("set stop-on-solib-events 0"));

			} else {

				//no reason for the failure, this means that we stopped due to
				//hitting a loading of shared library
				//try to place all breakpoints which previously failed
				SetBreakpoints();
			}

			Continue();

		} else {
			//GDB/MI Out-of-band Records
			//caused by async command, this line indicates that we have the control back
			DbgCmdHandler *handler = PopHandler(id);
			if (handler) {
				handler->ProcessOutput(line);
				delete handler;
			}
		}
	}
}

bool DbgGdb::EvaluateExpressionToString(const wxString &expression, const wxString &format)
{
	static int counter(0);
	wxString watchName(wxT("watch_num_"));
	watchName << ++counter;

	wxString command;
	command << wxT("-var-create ") << watchName << wxT(" * ") << expression;
	//first create the expression
	bool res = WriteCommand(command, new DbgCmdHandlerVarCreator(m_observer));
	if (!res) {
		//probably gdb is down
		return false;
	}

	command.clear();
	command << wxT("-var-set-format ") << watchName << wxT(" ") << format;
	//first create the expression
	res = WriteCommand(command, NULL);
	if (!res) {
		//probably gdb is down
		return false;
	}

	//execute the watch command
	command.clear();
	command << wxT("-var-evaluate-expression ") << watchName;
	res = WriteCommand(command, new DbgCmdHandlerEvalExpr(m_observer, expression));
	if (!res) {
		//probably gdb is down
		return false;
	}

	//and make sure we delete this variable
	return DeleteVariableObject( watchName );
}

bool DbgGdb::ListFrames()
{
	return WriteCommand(wxT("-stack-list-frames"), new DbgCmdStackList(m_observer));
}

bool DbgGdb::SetFrame(int frame)
{
	wxString command;
	command << wxT("frame ") << frame;
	return WriteCommand(command, new DbgCmdSelectFrame(m_observer));
}

bool DbgGdb::ListThreads()
{
	return ExecCLICommand(wxT("info threads"), new DbgCmdListThreads(m_observer));
}

bool DbgGdb::SelectThread(long threadId)
{
	wxString command;
	command << wxT("-thread-select ") << threadId;
	return WriteCommand(command, NULL);
}

void DbgGdb::OnProcessEnd(wxCommandEvent &e)
{
	ProcessEventData *ped = (ProcessEventData *)e.GetClientData();
	delete ped;

	if ( m_gdbProcess ) {
		delete m_gdbProcess;
		m_gdbProcess = NULL;
	}

	m_observer->UpdateGotControl(DBG_EXITED_NORMALLY);
	m_gdbOutputArr.Clear();
	SetIsRemoteDebugging(false);
}

bool DbgGdb::GetAsciiViewerContent(const wxString &dbgCommand, const wxString& expression)
{
	wxString cmd;
	cmd << dbgCommand << wxT(" ") << expression;

	return ExecCLICommand(cmd, new DbgCmdGetTipHandler(m_observer, expression));
}

bool DbgGdb::ResolveType(const wxString& expression)
{
	wxString cmd;
	cmd << wxT("-var-create - * \"") << expression << wxT("\"");
	return WriteCommand(cmd, new DbgCmdResolveTypeHandler(expression, this));
}

bool DbgGdb::WatchMemory(const wxString& address, size_t count)
{
	// make the line per WORD size
	int divider (sizeof(unsigned long));
	int factor((int)(count/divider));
	if (count % divider != 0) {
		factor = (int)(count / divider) + 1;
	}

	// at this point, 'factor' contains the number rows
	// and the 'divider' is the columns
	wxString cmd;
	cmd << wxT("-data-read-memory \"") << address << wxT("\" x 1 ") << factor << wxT(" ") << divider << wxT(" ?");
	return WriteCommand(cmd, new DbgCmdWatchMemory(m_observer, address, count));
}

bool DbgGdb::SetMemory(const wxString& address, size_t count, const wxString& hex_value)
{
	wxString cmd;
	wxString hexCommaDlimArr;
	wxArrayString hexArr = wxStringTokenize(hex_value, wxT(" "), wxTOKEN_STRTOK);

	for (size_t i=0; i<hexArr.GetCount(); i++) {
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

void DbgGdb::BreakList()
{
	(void)WriteCommand(wxT("-break-list"), new DbgCmdBreakList(m_observer));
}

bool DbgGdb::DoLocateGdbExecutable(const wxString& debuggerPath, wxString& dbgExeName)
{
	if (m_gdbProcess) {
		//dont allow second instance of the debugger
		return false;
	}
	wxString cmd;

	dbgExeName = debuggerPath;
	if (dbgExeName.IsEmpty()) {
		dbgExeName = wxT("gdb");
	}

	wxString actualPath;
	if (ExeLocator::Locate(dbgExeName, actualPath) == false) {
		wxMessageBox(wxString::Format(wxT("Failed to locate gdb! at '%s'"), dbgExeName.c_str()),
		             wxT("CodeLite"));
		return false;
	}

	// set the debugger specific startup commands
	wxString startupInfo ( m_info.startupCommands );

	// We must replace TABS with spaces or else gdb will hang...
	startupInfo.Replace(wxT("\t"), wxT(" "));

	// Write the content into a file
	wxString codelite_gdbinit_file;
#ifdef __WXMSW__
	codelite_gdbinit_file << wxFileName::GetTempDir() << wxFileName::GetPathSeparator() << wxT("codelite_gdbinit.txt");
#else
	wxString home_env;
	if( !wxGetEnv(wxT("HOME"), &home_env) ) {
		m_observer->UpdateAddLine(wxString::Format(wxT("Failed to read HOME environment variable")));
	} else {
		codelite_gdbinit_file  << home_env << wxT("/") << wxT(".gdbinit");
		if( wxFileName::FileExists(codelite_gdbinit_file) && !wxFileName::FileExists(codelite_gdbinit_file + wxT(".backup"))) {
			wxCopyFile(codelite_gdbinit_file, codelite_gdbinit_file + wxT(".backup") );
			m_observer->UpdateAddLine(wxString::Format(wxT(".gdbinit file was backup to %s"), wxString(codelite_gdbinit_file + wxT(".backup")).c_str()));
		}
	}
#endif
	wxFFile file;
	if (!file.Open(codelite_gdbinit_file, wxT("w+b"))) {
		m_observer->UpdateAddLine(wxString::Format(wxT("Failed to generate gdbinit file at %s"), codelite_gdbinit_file.c_str()));

	} else {
		m_observer->UpdateAddLine(wxString::Format(wxT("Using gdbinit file: %s"), codelite_gdbinit_file.c_str()));
		file.Write(startupInfo);
		file.Close();
#ifdef __WXMSW__
		dbgExeName << wxT(" --command=\"") << codelite_gdbinit_file << wxT("\"");
#endif

	}

	return true;
}


bool DbgGdb::DoInitializeGdb(const std::vector<BreakpointInfo> &bpList, const wxArrayString &cmds)
{
	//place breakpoint at first line
#ifdef __WXMSW__
	ExecuteCmd(wxT("set  new-console on"));
#endif
	ExecuteCmd(wxT("set unwindonsignal on"));

	if (m_info.enablePendingBreakpoints) {
		ExecuteCmd(wxT("set breakpoint pending on"));
	}

	if (m_info.catchThrow) {
		ExecuteCmd(wxT("catch throw"));
	}

#ifdef __WXMSW__
	if (m_info.debugAsserts) {
		ExecuteCmd(wxT("break assert"));
	}
#endif

	ExecuteCmd(wxT("set width 0"));
	ExecuteCmd(wxT("set height 0"));
	ExecuteCmd(wxT("set print pretty on"));  // pretty printing

	// Number of elements to show for arrays (including strings)
	wxString sizeCommand;
	sizeCommand << wxT("set print elements ") << m_info.maxDisplayStringSize;
	ExecuteCmd( sizeCommand );

	// set the project startup commands
	for (size_t i=0; i<cmds.GetCount(); i++) {
		ExecuteCmd(cmds.Item(i));
	}

	// keep the list of breakpoints
	m_bpList = bpList;
	
	if(GetIsRemoteDebugging() == false) 
		// When remote debugging, apply the breakpoints after we connect the 
		// gdbserver
		SetBreakpoints();

	if (m_info.breakAtWinMain) {
		//try also to set breakpoint at WinMain
		WriteCommand(wxT("-break-insert main"), NULL);
	}
	return true;
}

bool DbgGdb::ExecCLICommand(const wxString& command, DbgCmdCLIHandler* handler)
{
	wxString cmd;
	wxString id = MakeId( );
	cmd << id << command;
	//send the command to gdb
	if (!ExecuteCmd(cmd)) {
		return false;
	}

	if ( handler ) {
		handler->SetCommandId(id);
		SetCliHandler( handler );
	}
	return true;
}


void DbgGdb::SetCliHandler(DbgCmdCLIHandler* handler)
{
	if( m_cliHandler ) {
		delete m_cliHandler;
	}
	m_cliHandler = handler;
}

DbgCmdCLIHandler* DbgGdb::GetCliHandler()
{
	return m_cliHandler;
}

bool DbgGdb::ListChildren(const wxString& name, int userReason)
{
	wxString cmd;
	cmd << wxT("-var-list-children \"") << name << wxT("\"");
	return WriteCommand(cmd, new DbgCmdListChildren(m_observer, name, userReason));
}

bool DbgGdb::CreateVariableObject(const wxString& expression, int userReason)
{
	wxString cmd;
	cmd << wxT("-var-create - * \"") << expression << wxT("\"");
	return WriteCommand(cmd, new DbgCmdCreateVarObj(m_observer, expression, userReason));
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
	return WriteCommand(cmd, new DbgCmdEvalVarObj(m_observer, name, userReason));
}

void DbgGdb::OnDataRead(wxCommandEvent& e)
{
	// Data arrived from the debugger
	ProcessEventData *ped = (ProcessEventData *)e.GetClientData();

	wxString bufferRead;
	bufferRead << ped->GetData();
	delete ped;

	wxArrayString lines = wxStringTokenize(bufferRead, wxT("\n"), wxTOKEN_STRTOK);
	for(size_t i=0; i<lines.GetCount(); i++) {
		wxString line = lines.Item(i);
		line.Replace(wxT("(gdb)"), wxT(""));
		line.Trim().Trim(false);
		if ( line.IsEmpty() == false ) {
			m_gdbOutputArr.Add( line );
			//wxPrintf(wxT("Debugger: %s\n"), line.c_str());
		}
	}

	if ( m_gdbOutputArr.IsEmpty() == false ) {
		// Trigger GDB processing
		Poke();
	}
}

bool DbgGdb::DoGetNextLine(wxString& line)
{
	line.Clear();
	if ( m_gdbOutputArr.IsEmpty() ){
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
