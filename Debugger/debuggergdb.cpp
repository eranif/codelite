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
#include "gdb_result_parser.h"

extern int gdb_result_lex();
extern bool setGdbLexerInput(const std::string &in);
extern void gdb_result_lex_clean();
extern std::string gdb_result_string;
extern void gdb_result_push_buffer(const std::string &new_input);
extern void gdb_result_pop_buffer();

static void GDB_STRIP_QUOATES(wxString &currentToken)
{
	size_t where = currentToken.find(wxT("\""));
	if (where != std::string::npos && where == 0) {
		currentToken.erase(0, 1);
	}

	where = currentToken.rfind(wxT("\""));
	if (where != std::string::npos && where == currentToken.length()-1) {
		currentToken.erase(where);
	}

	where = currentToken.find(wxT("\"\\\\"));
	if (where != std::string::npos && where == 0) {
		currentToken.erase(0, 3);
	}

	where = currentToken.rfind(wxT("\"\\\\"));
	if (where != std::string::npos && where == currentToken.length()-3) {
		currentToken.erase(where);
	}
}

#define GDB_NEXT_TOKEN()\
	{\
		type = gdb_result_lex();\
		currentToken = _U(gdb_result_string.c_str());\
	}

#define GDB_ABORT(ch)\
	if(type != (int)ch){\
		break;\
	}

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
		wxT("v1.0"),
		wxT("Eran Ifrah")
	};
	return info;
}

IDebugger *CreateDebuggerGDB()
{
	static DbgGdb theGdbDebugger;
	return &theGdbDebugger;
}

// Removes MI additional characters from string
static void StipString(wxString &string)
{
	string = string.AfterFirst(wxT('"'));
	string = string.BeforeLast(wxT('"'));

	string.Replace(wxT("\\\\n"), wxT("\n"));
	string.Replace(wxT("\\n"), wxEmptyString);
	string.Replace(wxT("\\\\t"), wxT("\t"));
	string.Replace(wxT("\\\""), wxT("\""));
}

static wxString MakeId()
{
	static size_t counter(0);
	wxString newId;
	newId.Printf(wxT("%08d"), ++counter);
	return newId;
}

DbgGdb::DbgGdb()
		: m_debuggeePid(wxNOT_FOUND)
		, m_isRemote(false)
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
	if (IsBusy()) {
		//dont allow second instance of the debugger
		return false;
	}
	SetBusy(true);
	wxString cmd;

	wxString dbgExeName(debuggerPath);
	if (dbgExeName.IsEmpty()) {
		dbgExeName = wxT("gdb");
	}
	
	wxString actualPath;
	if(ExeLocator::Locate(dbgExeName, actualPath) == false){
		wxMessageBox(wxString::Format(wxT("Failed to locate gdb! at '%s'"), dbgExeName.c_str()),
					 wxT("CodeLite"));
		SetBusy(false);
		return false;
	}
	
#if defined (__WXGTK__)
	//On GTK and other platforms, open a new terminal and direct all
	//debugee process into it
	wxString ptyName;
	if (!m_consoleFinder.FindConsole(wxT("CodeLite: gdb"), ptyName)) {
		SetBusy(false);
		wxLogMessage(wxT("Failed to allocate console for debugger"));
		return false;
	}
	cmd << dbgExeName << wxT(" --tty=") << ptyName << wxT(" --interpreter=mi ");
#elif defined (__WXMAC__)
	cmd << dbgExeName << wxT(" --interpreter=mi ");
#else
	cmd << dbgExeName << wxT(" --interpreter=mi ");
	cmd << ProcUtils::GetProcessNameByPid(pid) << wxT(" ");
#endif

	m_debuggeePid = pid;
	cmd << wxT(" --pid=") << m_debuggeePid;
	wxLogMessage(cmd);

	//m_proc will be deleted upon termination
	m_proc = new PipedProcess(wxNewId(), cmd);
	if (m_proc) {

		//set the environment variables
		m_env->ApplyEnv(NULL);

		if (m_proc->Start() == 0) {

			//set the environment variables
			m_env->UnApplyEnv();

			//failed to start the debugger
			delete m_proc;
			SetBusy(false);
			return false;
		}

		Connect(wxEVT_TIMER, wxTimerEventHandler(DbgGdb::OnTimer), NULL, this);
		m_proc->Connect(wxEVT_END_PROCESS, wxProcessEventHandler(DbgGdb::OnProcessEndEx), NULL, this);
		m_canUse = true;
		m_timer->Start(10);
		wxWakeUpIdle();
		//place breakpoint at first line
#ifdef __WXMSW__
		ExecuteCmd(wxT("set new-console on"));
#endif
		ExecuteCmd(wxT("set unwindonsignal on"));
		//dont wrap lines
		ExecuteCmd(wxT("set width 0"));
		// no pagination
		ExecuteCmd(wxT("set height 0"));

		if (m_info.enablePendingBreakpoints) {
			//a workaround for the MI pending breakpoint
			ExecuteCmd(wxT("set breakpoint pending on"));
		}

		for (size_t i=0; i<cmds.GetCount(); i++) {
			ExecuteCmd(cmds.Item(i));
		}

		//keep the list of breakpoints
		m_bpList = bpList;
		SetBreakpoints();

		if (m_info.breakAtWinMain) {
			//try also to set breakpoint at WinMain
			WriteCommand(wxT("-break-insert main"), NULL);
		}

		m_observer->UpdateGotControl(DBG_END_STEPPING);
		return true;
	}
	return false;
}

bool DbgGdb::Start(const wxString &debuggerPath, const wxString &exeName, const wxString &cwd, const std::vector<BreakpointInfo> &bpList, const wxArrayString &cmds)
{
	if (IsBusy()) {
		//dont allow second instance of the debugger
		return false;
	}

	SetBusy(true);
	wxString cmd;

	wxString dbgExeName(debuggerPath);
	if (dbgExeName.IsEmpty()) {
		dbgExeName = wxT("gdb");
	}
	
	wxString actualPath;
	if(ExeLocator::Locate(dbgExeName, actualPath) == false){
		wxMessageBox(wxString::Format(wxT("Failed to locate gdb! at '%s'"), dbgExeName.c_str()),
					 wxT("CodeLite"));
		SetBusy(false);
		return false;
	}
	
#if defined (__WXGTK__)
	//On GTK and other platforms, open a new terminal and direct all
	//debugee process into it
	wxString ptyName;
	if (!m_consoleFinder.FindConsole(exeName, ptyName)) {
		SetBusy(false);
		wxLogMessage(wxT("Failed to allocate console for debugger, do u have Xterm installed?"));
		return false;
	}
	cmd << dbgExeName << wxT(" --tty=") << ptyName << wxT(" --interpreter=mi ") << exeName;
#elif defined(__WXMAC__)
	cmd << dbgExeName << wxT(" --interpreter=mi ") << exeName;
#else
	cmd << dbgExeName << wxT(" --interpreter=mi ") << exeName;
#endif

	m_debuggeePid = wxNOT_FOUND;
	//m_proc will be deleted upon termination
	m_proc = new PipedProcess(wxNewId(), cmd);
	if (m_proc) {
		//change directory to the debugger working directory as set in the
		//project settings
		DirKeeper keeper;
		wxSetWorkingDirectory(cwd);

		//set the environment variables
		m_env->ApplyEnv(NULL);

		if (m_proc->Start( !m_info.showTerminal ) == 0) {
			//failed to start the debugger
			delete m_proc;
			SetBusy(false);

			//Unset the environment variables
			m_env->UnApplyEnv();

			return false;
		}

		Connect(wxEVT_TIMER, wxTimerEventHandler(DbgGdb::OnTimer), NULL, this);
		m_proc->Connect(wxEVT_END_PROCESS, wxProcessEventHandler(DbgGdb::OnProcessEnd), NULL, this);
		m_canUse = true;
		m_timer->Start(10);
		wxWakeUpIdle();
		//place breakpoint at first line
#ifdef __WXMSW__
		ExecuteCmd(wxT("set  new-console on"));
#endif
		ExecuteCmd(wxT("set unwindonsignal on"));

		if (m_info.enablePendingBreakpoints) {
			ExecuteCmd(wxT("set breakpoint pending on"));
		}

		//dont wrap lines
		ExecuteCmd(wxT("set width 0"));
		// no pagination
		ExecuteCmd(wxT("set height 0"));

		for (size_t i=0; i<cmds.GetCount(); i++) {
			ExecuteCmd(cmds.Item(i));
		}

		//keep the list of breakpoints
		m_bpList = bpList;
		SetBreakpoints();

		if (m_info.breakAtWinMain) {
			//try also to set breakpoint at WinMain
			WriteCommand(wxT("-break-insert main"), NULL);
		}
		return true;
	}
	return false;
}

bool DbgGdb::WriteCommand(const wxString &command, DbgCmdHandler *handler)
{
	wxString cmd;
	wxString id = MakeId( );
	cmd << id << command;
	
	if(m_info.enableDebugLog) {
		m_observer->UpdateAddLine(wxString::Format(wxT("DEBUG>>%s"), cmd.c_str()));
	}
	if (!Write(cmd)) {
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
	m_isRemote = false;
	if (comm.IsEmpty()) {

		// add handler for this command
		return WriteCommand(wxT("-exec-run ") + args, new DbgCmdHandlerAsyncCmd(m_observer));

	} else {
		// attach to the remote gdb server
		m_isRemote = true;
		wxString cmd;
		cmd << wxT("target remote ") << comm << wxT(" ") << args;
		return WriteCommand(cmd, new DbgCmdHandlerRemoteDebugging(m_observer, this));
	}
}

bool DbgGdb::Stop()
{
	if (IsBusy()) {
		Disconnect(wxEVT_TIMER, wxTimerEventHandler(DbgGdb::OnTimer), NULL, this);
		m_proc->Disconnect(wxEVT_END_PROCESS, wxProcessEventHandler(DbgGdb::OnProcessEnd), NULL, this);

		InteractiveProcess::StopProcess();
		SetBusy(false);

		//free allocated console for this session
		m_consoleFinder.FreeConsole();

		//return control to the program
		m_observer->UpdateGotControl(DBG_DBGR_KILLED);
		EmptyQueue();

		m_bpList.empty();
	}
	return true;
}

bool DbgGdb::Next()
{
	return WriteCommand(wxT("-exec-next"), new DbgCmdHandlerAsyncCmd(m_observer));
}

void DbgGdb::SetBreakpoints()
{
	for (size_t i=0; i< m_bpList.size(); i++) {
		BreakpointInfo bpinfo = m_bpList.at(i);
		Break(bpinfo.file, bpinfo.lineno);
	}
}

bool DbgGdb::Break(const wxString &fileName, long lineno)
{
	wxFileName fn(fileName);
	BreakpointInfo bp;
	bp.file = fileName;
	bp.lineno = lineno;

	wxString tmpfileName(fn.GetFullPath());
	tmpfileName.Replace(wxT("\\"), wxT("/"));

	wxString command(wxT("break "));
	tmpfileName.Prepend(wxT("\""));
	command << tmpfileName << wxT(":") << lineno << wxT("\"");
	if (m_info.enableDebugLog) {
		m_observer->UpdateAddLine(command);
	}

	return WriteCommand(command, new DbgCmdHandlerBp(m_observer, bp, &m_bpList));
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
	return IsBusy();
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
	//the order of the commands here is important
	if (m_info.resolveThis) {
		if (!WriteCommand(wxT("-data-evaluate-expression *this"), new DbgCmdHandlerLocals(m_observer, DbgCmdHandlerLocals::This, wxT("*this")))) {
			return false;
		}
	}

	if (!WriteCommand(wxT("-stack-list-arguments 1 0 0"), new DbgCmdHandlerLocals(m_observer, DbgCmdHandlerLocals::FunctionArguments))) {
		return false;
	}
	return WriteCommand(wxT("-stack-list-locals --all-values"), new DbgCmdHandlerLocals(m_observer));
}

bool DbgGdb::ExecuteCmd(const wxString &cmd)
{
	if(m_info.enableDebugLog) {
		m_observer->UpdateAddLine(wxString::Format(wxT("DEBUG>>%s"), cmd.c_str()));
	}
	return Write(cmd);
}

bool DbgGdb::ExecSyncCmd(const wxString &command, wxString &output)
{
	wxString cmd;
	wxString id = MakeId( );
	cmd << id << command;
	//send the command to gdb
	if(m_info.enableDebugLog) {
		m_observer->UpdateAddLine(wxString::Format(wxT("DEBUG>>%s"), cmd.c_str()));
	}
	if (!Write(cmd)) {
		return false;
	}
	bool miCommand(false);
	wxString trimmedCommand(command);

	if (trimmedCommand.Trim().Trim(false).StartsWith(wxT("-"))) {
		miCommand = true;
	}

	//read all output until we found 'XXXXXXXX^done'
	static wxRegEx reCommand(wxT("^([0-9]{8})"));
	const int maxPeeks(100);
	wxString line;
	int counter(0);
	for ( ;; ) {
		//try to read a line from the debugger
		line.Empty();
		ReadLine(line, 1);
		line = line.Trim().Trim(false);

		if (line.IsEmpty()) {
			if (counter < maxPeeks) {
				counter++;
				continue;
			} else {
				break;
			}
		}
		
		if(m_info.enableDebugLog) {
			m_observer->UpdateAddLine(wxString::Format(wxT("DEBUG>>%s"), line.c_str()));
		}
		
		counter = 0;
		if (reCommand.Matches(line)) {
			//not a gdb message, get the command associated with the message
			wxString cmd_id = reCommand.GetMatch(line, 1);
			if (cmd_id != id) {
				
				long expcId(0), recvId(0);
				cmd_id.ToLong(&recvId);
				id.ToLong(&expcId);

				// first, process this reply, since it might be an output from a previous command
				// strip the id from the line
				line = line.Mid(8);
				DoProcessAsyncCommand(line, cmd_id);
				
				if(recvId > expcId) {
					// if we are here, it means that the received ID is greater than ours...
					// so we probably will never get it ..
					return false;
				}
				continue;
			}

			//remove trailing new line
			if (miCommand) {
				// if the execute command is a GDB MI command,
				// remove the command ID (the 8 digit sequence)
				// and append this line to the output
				int where = line.Find(cmd_id);
				if (where != wxNOT_FOUND) {
					output << line.Mid(where + 8);
				}
			}

			output = output.Trim().Trim(false);
			return true;

		} else {
			StipString(line);
			if (!line.Contains(command)) {
				output << line << wxT("\n");
			}
		}
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

bool DbgGdb::RemoveBreak(const wxString &fileName, long lineno)
{
	wxString command;
	wxString fileName_(fileName);
	fileName_.Replace(wxT("\\"), wxT("/"));
	command << wxT("clear \"") << fileName_ << wxT("\":") << lineno;
	return WriteCommand(command, NULL);
}

bool DbgGdb::FilterMessage(const wxString &msg)
{
	if (msg.Contains(wxT("Variable object not found"))) {
		return true;
	}

	if (msg.Contains(wxT("mi_cmd_var_create: unable to create variable object"))) {
		return true;
	}

	if (msg.Contains(wxT("Variable object not found"))) {
		return true;
	}

	if (msg.Contains(wxT("No symbol \"this\" in current context"))) {
		return true;
	}
	return false;
}

void DbgGdb::Poke()
{
	static wxRegEx reCommand(wxT("^([0-9]{8})"));
	
	//poll the debugger output
	wxString line;
	if( !m_proc ) {
		return;
	}

	if (m_debuggeePid == wxNOT_FOUND) {
		if (m_isRemote) {
			m_debuggeePid = m_proc->GetPid();
		} else {
			std::vector<long> children;
			ProcUtils::GetChildren(m_proc->GetPid(), children);
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

	int lineRead(0);
	for ( ;; ) {
		line.Empty();

		//did we reach the maximum reads per interval?
		//if the answer is yes, give up the CPU and wait for another chance
		if (lineRead == 5) {
			break;
		}

		//try to read a line from the debugger
		ReadLine(line, 1);

		line = line.Trim();
		line = line.Trim(false);

		if (m_info.enableDebugLog) {
			//Is logging enabled?
			if (line.IsEmpty() == false) {
				wxString strdebug(wxT("DEBUG>>"));
				strdebug << line;
				m_observer->UpdateAddLine(strdebug);
			}
		}
	
		if(reConnectionRefused.Matches(line)){
			StipString(line);
#ifdef __WXGTK__
			m_consoleFinder.FreeConsole();
#endif
			m_observer->UpdateAddLine(line);
			m_observer->UpdateGotControl(DBG_EXITED_NORMALLY);
			return;
		}
		
		line.Replace(wxT("(gdb)"), wxEmptyString);
		if (line.IsEmpty()) {
			break;
		}
		lineRead++;



		if (line.StartsWith(wxT("~")) || line.StartsWith(wxT("&"))) {
			//just an informative line,
			StipString(line);
			//filter out some gdb error lines...
			if (FilterMessage(line)) {
				continue;
			}
			m_observer->UpdateAddLine(line);

		} else if (reCommand.Matches(line)) {

			//not a gdb message, get the command associated with the message
			wxString id = reCommand.GetMatch(line, 1);

			//strip the id from the line
			line = line.Mid(8);
			DoProcessAsyncCommand(line, id);

		} else if (line.StartsWith(wxT("^done")) || line.StartsWith(wxT("*stopped"))) {
			//Unregistered command, use the default AsyncCommand handler to process the line
			DbgCmdHandlerAsyncCmd cmd(m_observer);
			cmd.ProcessOutput(line);
		} else {
			//Unknow format, just log it
			m_observer->UpdateAddLine(line);
		}
	}
}

void DbgGdb::DoProcessAsyncCommand(wxString &line, wxString &id)
{
	if (line.StartsWith(wxT("^error"))) {
		//the command was error, for example:
		//finish in the outer most frame
		//print the error message and remove the command from the queue
		DbgCmdHandler *handler = PopHandler(id);
		if (handler) {
			delete handler;
		}
		StipString(line);
		//We also need to pass the control back to the program
		m_observer->UpdateGotControl(DBG_CMD_ERROR);

		if (!FilterMessage(line)) {
			m_observer->UpdateAddLine(line);
		}

	} else if (line.StartsWith(wxT("^done"))) {
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
	command << wxT("-var-create ") << watchName << wxT(" 0 ") << expression;
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
	command.Clear();
	//execute the watch command
	command << wxT("-var-delete ") << watchName;

	//we register NULL handler, which means this line can be safely ignored
	return WriteCommand(command, NULL);
}

bool DbgGdb::EvaluateExpressionToTree(const wxString &expression)
{
	wxString command;
	wxString tmp(expression);

	tmp = tmp.Trim().Trim(false);
	command << wxT("-data-evaluate-expression ") << expression;

	//first create the expression
	return WriteCommand(command, new DbgCmdHandlerLocals(m_observer, DbgCmdHandlerLocals::EvaluateExpression, expression));
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

bool DbgGdb::ListThreads(ThreadEntryArray &threads)
{
	static wxRegEx reCommand(wxT("^([0-9]{8})"));
	wxString output;
	if (!ExecSyncCmd(wxT("info threads"), output)) {
		return false;
	}

	//parse the debugger output
	wxStringTokenizer tok(output, wxT("\n"), wxTOKEN_STRTOK);
	while (tok.HasMoreTokens()) {
		ThreadEntry entry;
		wxString line = tok.NextToken();
		line.Replace(wxT("\t"), wxT(" "));
		line = line.Trim().Trim(false);


		if (reCommand.Matches(line)) {
			//this is the ack line, ignore it
			continue;
		}

		wxString tmpline(line);
		if (tmpline.StartsWith(wxT("*"), &line)) {
			//active thread
			entry.active = true;
		} else {
			entry.active = false;
		}

		line = line.Trim().Trim(false);
		line.ToLong(&entry.dbgid);
		entry.more = line.AfterFirst(wxT(' '));
		threads.push_back(entry);
	}
	return true;
}

bool DbgGdb::SelectThread(long threadId)
{
	wxString command;
	command << wxT("-thread-select ") << threadId;
	return WriteCommand(command, NULL);
}

void DbgGdb::OnProcessEndEx(wxProcessEvent &e)
{
	InteractiveProcess::OnProcessEnd(e);
	m_env->UnApplyEnv();
}

bool DbgGdb::GetTip(const wxString& expression, wxString& evaluated)
{
	wxString cmd;
	cmd << wxT("print ") << expression;
	if (ExecSyncCmd(cmd, evaluated)) {
		evaluated = evaluated.Trim().Trim(false);
		//gdb displays the variable name as $<NUMBER>,
		//we simply replace it with the actual string
		static wxRegEx reGdbVar(wxT("^\\$[0-9]+"));
		reGdbVar.ReplaceFirst(&evaluated, expression);
		return true;
	}
	return false;
}

bool DbgGdb::ResolveType(const wxString& expression, wxString& type_name)
{
	wxString output, cmd, var_name;
	cmd << wxT("-var-create - * \"") << expression << wxT("\"");

	if (ExecSyncCmd(cmd, output)) {
		// delete the temporary variable object
		cmd.clear();
		//	wxLogMessage(wxT("ResolveType: gdb returned '") + output + wxT("'"));

		// parse the output
		// ^done,name="var2",numchild="1",value="{...}",type="orxAABOX"
		const wxCharBuffer scannerText =  _C(output);
		setGdbLexerInput(scannerText.data());
		int type;
		wxString currentToken;

		do {
			// ^done
			GDB_NEXT_TOKEN();
			GDB_ABORT('^');
			GDB_NEXT_TOKEN();
			GDB_ABORT(GDB_DONE);

			// ,name="..."
			GDB_NEXT_TOKEN();
			GDB_ABORT(',');
			GDB_NEXT_TOKEN();
			GDB_ABORT(GDB_NAME);
			GDB_NEXT_TOKEN();
			GDB_ABORT('=');
			GDB_NEXT_TOKEN();
			GDB_ABORT(GDB_STRING);
			var_name = currentToken;

			// ,numchild="..."
			GDB_NEXT_TOKEN();
			GDB_ABORT(',');
			GDB_NEXT_TOKEN();
			GDB_ABORT(GDB_NUMCHILD);
			GDB_NEXT_TOKEN();
			GDB_ABORT('=');
			GDB_NEXT_TOKEN();
			GDB_ABORT(GDB_STRING);
			// On Mac this part does not seem to be reported by GDB
#ifndef __WXMAC__
			// ,value="..."
			GDB_NEXT_TOKEN();
			GDB_ABORT(',');
			GDB_NEXT_TOKEN();
			GDB_ABORT(GDB_VALUE);
			GDB_NEXT_TOKEN();
			GDB_ABORT('=');
			GDB_NEXT_TOKEN();
			GDB_ABORT(GDB_STRING);
#endif
			// ,type="..."
			GDB_NEXT_TOKEN();
			GDB_ABORT(',');
			GDB_NEXT_TOKEN();
			GDB_ABORT(GDB_TYPE);
			GDB_NEXT_TOKEN();
			GDB_ABORT('=');
			GDB_NEXT_TOKEN();
			type_name = currentToken;

		} while (0);
		gdb_result_lex_clean();

		GDB_STRIP_QUOATES(type_name);
		GDB_STRIP_QUOATES(var_name);

		// delete the variable object
		cmd.clear();
		cmd << wxT("-var-delete ") << var_name;

		// since the above gdb command yields an output, we use the sync command
		// to get it as well to avoid errors in future calls to the gdb
		ExecSyncCmd(cmd, output);

		return type_name.IsEmpty() == false;
	}
	return false;
}

bool DbgGdb::WatchMemory(const wxString& address, size_t count, wxString& output)
{
	// make the line per WORD size
#ifdef ON_64_BIT
	int divider (8);
#else
	int divider (4);
#endif

	int factor((int)(count/divider));
	if (count % divider != 0) {
		factor = (int)(count / divider) + 1;
	}

	// at this point, 'factor' contains the number rows
	// and the 'divider' is the columns
	wxString cmd, dbg_output;
	cmd << wxT("-data-read-memory \"") << address << wxT("\" x 1 ") << factor << wxT(" ") << divider << wxT(" ?");

	if (ExecSyncCmd(cmd, dbg_output)) {

		//{addr="0x003d3e24",data=["0x65","0x72","0x61","0x6e"],ascii="eran"},
		//{addr="0x003d3e28",data=["0x00","0xab","0xab","0xab"],ascii="xxxx"}

		// search for ,memory=[
		int where = dbg_output.Find(wxT(",memory="));
		if (where != wxNOT_FOUND) {
			dbg_output = dbg_output.Mid((size_t)(where + 9));

			const wxCharBuffer scannerText =  _C(dbg_output);
			setGdbLexerInput(scannerText.data());

			int type;
			wxString currentToken;
			wxString currentLine;
			GDB_NEXT_TOKEN();

			for (int i=0; i<factor && type != 0; i++) {
				currentLine.Clear();

				while (type != GDB_ADDR) {

					if (type == 0) {
						break;
					}

					GDB_NEXT_TOKEN();
					continue;
				}

				// Eof?
				if (type == 0) {
					break;
				}

				GDB_NEXT_TOKEN();	//=
				GDB_NEXT_TOKEN();	//0x003d3e24
				GDB_STRIP_QUOATES(currentToken);
				currentLine << currentToken << wxT(": ");

				GDB_NEXT_TOKEN();	//,
				GDB_NEXT_TOKEN();	//data
				GDB_NEXT_TOKEN();	//=
				GDB_NEXT_TOKEN();	//[

				for (int yy=0; yy<divider; yy++) {
					GDB_NEXT_TOKEN();	//"0x65"
					GDB_STRIP_QUOATES(currentToken);
					currentLine << currentToken << wxT(" ");
					GDB_NEXT_TOKEN();	//, | ]
				}

				GDB_NEXT_TOKEN();	//,
				GDB_NEXT_TOKEN();	//GDB_ASCII
				GDB_NEXT_TOKEN();	//=
				GDB_NEXT_TOKEN();	//ascii_value
				GDB_STRIP_QUOATES(currentToken);

				currentLine << wxT(" : ") << currentToken;

				GDB_STRIP_QUOATES(currentToken);
				output << currentLine << wxT("\n");
				GDB_NEXT_TOKEN();
			}

			gdb_result_lex_clean();
			return true;
		}

		return true;
	}
	return false;
}

bool DbgGdb::SetMemory(const wxString& address, size_t count, const wxString& hex_value)
{
	wxString cmd;
	wxString hexCommaDlimArr;
	wxArrayString hexArr = wxStringTokenize(hex_value, wxT(" "), wxTOKEN_STRTOK);
	
	for(size_t i=0; i<hexArr.GetCount(); i++){
		hexCommaDlimArr << hexArr.Item(i) << wxT(",");
	}
	
	hexCommaDlimArr.RemoveLast();
	cmd << wxT("set {char[") << count << wxT("]}") << address << wxT("={") << hexCommaDlimArr << wxT("}");
	
	return ExecuteCmd(cmd);
}
