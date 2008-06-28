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

	// Debugger operations
	virtual bool Start(const wxString &debuggerPath, const wxString &exeName, const wxString &cwd, const std::vector<BreakpointInfo> &bpList) = 0;
	virtual bool Start(const wxString &exeName, const wxString &cwd, const std::vector<BreakpointInfo> &bpList) = 0;
	virtual bool Start(const wxString &debuggerPath, const wxString &exeName, int pid, const std::vector<BreakpointInfo> &bpList) = 0;
	virtual bool Run(const wxString &args) = 0;
	virtual bool Stop() = 0;
	virtual bool Interrupt() = 0;
	virtual bool IsRunning() = 0;
	virtual bool Next() = 0;
	virtual bool Continue() = 0;
	virtual bool StepIn() = 0;
	virtual bool StepOut() = 0;
	virtual bool Break(const wxString &file, long lineno) = 0;
	virtual bool RemoveBreak(const wxString &file, long lineno) = 0;
	virtual bool RemoveBreak(int bid) = 0;
	virtual bool RemoveAllBreaks() = 0;
	virtual bool QueryFileLine() = 0;
	virtual bool ExecuteCmd(const wxString &cmd) = 0;
	virtual bool ExecSyncCmd(const wxString &command, wxString &output) = 0;
	virtual bool QueryLocals() = 0;
	virtual bool ListFrames() = 0;
	virtual bool SetFrame(int frame) = 0;
	virtual void SetDebuggerInformation(const DebuggerInformation& info) {
		m_info = info;
	}
	virtual bool ListThreads(ThreadEntryArray &threads) = 0;
	virtual bool SelectThread(long threadId) = 0;
	virtual void Poke() = 0;
	//We provide two ways of evulating an expressions:
	//The short one, which returns a string, and long one
	//which returns a tree of the result
	virtual bool EvaluateExpressionToString(const wxString &expression, const wxString &format) = 0;
	virtual bool EvaluateExpressionToTree(const wxString &expression) = 0;
};


//-----------------------------------------------------------
// Each debugger module must implement these two functions
//-----------------------------------------------------------
typedef IDebugger*    (*GET_DBG_CREATE_FUNC)();
typedef DebuggerInfo  (*GET_DBG_INFO_FUNC)();

#endif //DEBUGGER_H
