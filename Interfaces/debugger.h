//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debugger.h
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
#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "wx/string.h"
#include "wx/arrstr.h"
#include "wx/event.h"
#include "vector"

enum DebuggerCommands {
	DBG_PAUSE = 0,
	DBG_NEXT,
	DBG_STEPIN,
	DBG_STEPOUT,
	DBG_SHOW_CURSOR
};

//-------------------------------------------------------
// Data structures used by the debugger
//-------------------------------------------------------
struct DebuggerInfo {
	wxString name;
	wxString initFuncName;
	wxString version;
	wxString author;
};

struct StackEntry {
	wxString level;
	wxString address;
	wxString function;
	wxString file;
	wxString line;
};

struct ThreadEntry {
	bool 	 	active;
	long 		dbgid;
	wxString	more;
};

typedef std::vector<StackEntry> StackEntryArray;
typedef std::vector<ThreadEntry> ThreadEntryArray;

struct BreakpointInfo {
	wxString file;
	int lineno;
};

class DebuggerInformation
{
public:
	wxString name;
	wxString path;
	bool enableDebugLog;
	bool enablePendingBreakpoints;
	bool breakAtWinMain;
	bool resolveThis;
	
public:
	DebuggerInformation()
			: name(wxEmptyString)
			, path(wxEmptyString)
			, enableDebugLog(false)
			, enablePendingBreakpoints(true)
#ifdef __WXMAC__
			, breakAtWinMain(true)
#else
			, breakAtWinMain(false)
#endif
			, resolveThis(false)
	{}

	~DebuggerInformation() {}
};


class IDebuggerObserver;
class EnvironmentConfig;

//-------------------------------------------------------
// The debugger interface
//-------------------------------------------------------
/**
 * \ingroup Interfaces
 * Defines the debugger interface
 *
 * \version 1.0
 * first version
 *
 * \date 08-22-2007
 *
 * \author Eran
 *
 */
class IDebugger
{
protected:
	IDebuggerObserver *m_observer;
	DebuggerInformation m_info;
	EnvironmentConfig *m_env;

public:
	IDebugger() : m_env(NULL) {};
	virtual ~IDebugger() {};
	virtual void SetObserver(IDebuggerObserver *observer) {
		m_observer = observer;
	}
	virtual IDebuggerObserver *GetObserver() {
		return m_observer;
	}
	virtual void SetEnvironment(EnvironmentConfig *env) {
		m_env = env;
	}
	virtual void SetDebuggerInformation(const DebuggerInformation& info) {m_info = info;}

	//-------------------------------------------------------------
	// Debugger operations
	//-------------------------------------------------------------
	
	/**
	 * \brief start the debugger 
	 * \param debuggerPath path to the debugger executable
	 * \param exeName executable to debug
	 * \param cwd working directory to set
	 * \param bpList list of breakpoints to place before running the program
	 * \param cmds list of commands that will be passed to the debugger at startup 
	 * \return true on success, false otherwise
	 */
	virtual bool Start(const wxString &debuggerPath, const wxString &exeName, const wxString &cwd, const std::vector<BreakpointInfo> &bpList, const wxArrayString &cmds) = 0;
	
	/**
	 * \brief start the debugger. this method is for convinience and uses the default debugger path
	 * \param exeName executable to debug
	 * \param cwd working directory to set
	 * \param bpList list of breakpoints to place before running the program
	 * \param cmds list of commands that will be passed to the debugger at startup 
	 * \return true on success, false otherwise
	 */
	virtual bool Start(const wxString &exeName, const wxString &cwd, const std::vector<BreakpointInfo> &bpList, const wxArrayString &cmds) = 0;
	
	/**
	 * \brief use this method when attempting to attach a running process
	 * \param debuggerPath debugger path
	 * \param exeName executable to debug
 	 * \param pid the running instance process ID
	 * \param bpList list of breakpoints to set
	 * \param cmds list of commands that will be passed to the debugger at startup
	 * \return 
	 */
	virtual bool Start(const wxString &debuggerPath, const wxString &exeName, int pid, const std::vector<BreakpointInfo> &bpList, const wxArrayString &cmds) = 0;
	
	/**
	 * \brief Run the program under the debugger. This method must be called *after* Start() has been called
	 * \param args arguments to pass to the debuggee process
	 * \return true on success, false otherwise
	 */
	virtual bool Run(const wxString &args) = 0;
	/**
	 * \brief Stop the debugger
	 * \return true on success, false otherwise
	 */
	virtual bool Stop() = 0;
	/**
	 * \brief attempt to interrupt the running debugger
	 * \return true on success, false otherwise
	 */
	virtual bool Interrupt() = 0;
	/**
	 * \brief return true of the debugger is currently running
	 * \return true on success, false otherwise
	 */
	virtual bool IsRunning() = 0;
	/**
	 * \brief step to next line
	 * \return true on success, false otherwise
	 */
	virtual bool Next() = 0;
	/**
	 * \brief continue execution of the debugger, until next breakpoint is hit, or program terminates
	 * \return true on success, false otherwise
	 */
	virtual bool Continue() = 0;
	/**
	 * \brief step into method
	 * \return true on success, false otherwise
	 */
	virtual bool StepIn() = 0;
	/**
	 * \brief step out the current method (gdb's 'finish' command)
	 * \return true on success, false otherwise
	 */
	virtual bool StepOut() = 0;
	/**
	 * \brief set break point at given file and line. If the breakpoint was placed successfully, observer->UpdateBpAdded() will be invoked
	 */
	virtual bool Break(const wxString &file, long lineno) = 0;
	/**
	 * \brief remove breakpoint from given file and line
	 */
	virtual bool RemoveBreak(const wxString &file, long lineno) = 0;
	/**
	 * \brief remove breakpoint by its internal ID
	 */
	virtual bool RemoveBreak(int bid) = 0;
	/**
	 * \brief clear all breakpoints set (gdb's 'clear' command)
	 */
	virtual bool RemoveAllBreaks() = 0;
	/**
	 * \brief ask the debugger to query about its file & line. Once the result arrives, the observer's UpdateFileLine() will be invoked 
	 */
	virtual bool QueryFileLine() = 0;
	/**
	 * \brief executes a command that does not yield any output from the debugger
	 */
	virtual bool ExecuteCmd(const wxString &cmd) = 0;
	/**
	 * \brief excute a command and waits from its answer from the debugger
	 * \param command command to execute
	 * \param output the debugger response
	 */
	virtual bool ExecSyncCmd(const wxString &command, wxString &output) = 0;
	/**
	 * \brief ask the debugger to print the current stack local variable. When the results arrives, the observer->UpdateLocals() is called
	 * \return true on success, false otherwise
	 */
	virtual bool QueryLocals() = 0;
	/**
	 * \brief list the current stack trace. once the results are received from the debugger, m_observer->UpdateStackList() is invoked
	 * \return true on success, false otherwise
	 */
	virtual bool ListFrames() = 0;
	
	/**
	 * \brief set the frame to be the active frame
	 * \param frame frame to set active
	 * \return true on success, false otherwise
	 */
	virtual bool SetFrame(int frame) = 0;
	/**
	 * \brief return list of threads. 
	 * \param threads [output]
	 * \return true on success, false otherwise
	 */
	virtual bool ListThreads(ThreadEntryArray &threads) = 0;
	/**
	 * \brief select threadId to be active
	 * \param threadId thread id
	 * \return true on success, false otherwise
	 */
	virtual bool SelectThread(long threadId) = 0;
	/**
	 * \brief the Poke() method is called at Idle() time by the application so the debugger can read the actual debugger process output and process it
	 */
	virtual void Poke() = 0;
	/**
	 * \brief return tip for a give expression
	 * \param expression expression to evaluate
	 * \param evaluated evaluated value from the debugger
	 * \return 
	 */
	virtual bool GetTip(const wxString &expression, wxString &evaluated) = 0;
	
	/**
	 * \brief resolve expression and return its actual type
	 * \param expression expression to evaluate
	 * \param type the type [output]
	 * \return true on success, false otherwise
	 */
	virtual bool ResolveType(const wxString &expression, wxString &type) = 0;
	
	//We provide two ways of evulating an expressions:
	//The short one, which returns a string, and long one
	//which returns a tree of the result
	virtual bool EvaluateExpressionToString(const wxString &expression, const wxString &format) = 0;
	virtual bool EvaluateExpressionToTree(const wxString &expression) = 0;
	
	/**
	 * \brief a request to display memory from address -> address + count. This is a synchronous call
	 * \param address starting address
	 * \param count address range
	 * \param output [output] string containing the formatted result
	 * \return true on success, false otherwise
	 */
	virtual bool WatchMemory(const wxString &address, size_t count, wxString &output) = 0;
};


//-----------------------------------------------------------
// Each debugger module must implement these two functions
//-----------------------------------------------------------
typedef IDebugger*    (*GET_DBG_CREATE_FUNC)();
typedef DebuggerInfo  (*GET_DBG_INFO_FUNC)();

#endif //DEBUGGER_H
