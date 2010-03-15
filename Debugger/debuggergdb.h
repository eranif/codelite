//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debuggergdb.h
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
#ifndef DBGINTERFACE_H
#define DBGINTERFACE_H

#include "wx/string.h"
#include "wx/event.h"
#include "list"
#include "debugger.h"
#include <wx/hashmap.h>
#include "consolefinder.h"

#ifdef MSVC_VER
//declare the debugger function creation
extern "C++" IDebugger *CreateDebuggerGDB();
//declare the function that will be called by host application
//to retrieve the debugger initialization function
extern "C++" DebuggerInfo GetDebuggerInfo();
#else
//declare the debugger function creation
extern "C" IDebugger *CreateDebuggerGDB();
//declare the function that will be called by host application
//to retrieve the debugger initialization function
extern "C" DebuggerInfo GetDebuggerInfo();
#endif

class DbgCmdHandler;
class DbgCmdCLIHandler;
class IProcess;

WX_DECLARE_STRING_HASH_MAP(DbgCmdHandler*, HandlersMap);

class DbgGdb : public wxEvtHandler, public IDebugger
{
	HandlersMap                 m_handlers;
	long                        m_debuggeePid;
	ConsoleFinder               m_consoleFinder;
	std::vector<BreakpointInfo> m_bpList;
	DbgCmdCLIHandler*           m_cliHandler;
	IProcess*                   m_gdbProcess;
	wxArrayString               m_gdbOutputArr;

protected:
	void           RegisterHandler(const wxString &id, DbgCmdHandler *cmd);
	DbgCmdHandler *PopHandler(const wxString &id);
	void           EmptyQueue();
	bool           FilterMessage(const wxString &msg);
	bool           DoGetNextLine(wxString &line);
	//wrapper for convinience
	void DoProcessAsyncCommand(wxString &line, wxString &id);
	
protected:
	bool               DoLocateGdbExecutable(const wxString &debuggerPath, wxString &dbgExeName);
	bool               DoInitializeGdb      (const std::vector<BreakpointInfo> &bpList, const wxArrayString &cmds);
	void               SetCliHandler        (DbgCmdCLIHandler *handler);
	DbgCmdCLIHandler * GetCliHandler        ();

public:
	bool WriteCommand  (const wxString &command, DbgCmdHandler *   cmd);
	bool ExecCLICommand(const wxString &command, DbgCmdCLIHandler* cmd);
	void SetBreakpoints();

public:
	DbgGdb();
	virtual ~DbgGdb();

	//------ IDebugger ---------
	virtual bool Start(const wxString &debuggerPath, const wxString &exeName, const wxString &cwd, const std::vector<BreakpointInfo> &bpList, const wxArrayString &cmds);
	virtual bool Start(const wxString &debuggerPath, const wxString &exeName, int pid, const std::vector<BreakpointInfo> &bpList, const wxArrayString &cmds);
	virtual bool Start(const wxString &exeName, const wxString &cwd, const std::vector<BreakpointInfo> &bpList, const wxArrayString &cmds);
	virtual bool Run(const wxString &args, const wxString &comm);
	virtual bool Stop();
	virtual bool Break(const BreakpointInfo& bp);
	virtual bool SetEnabledState(const int bid, const bool enable);
	virtual bool SetIgnoreLevel(const int bid, const int ignorecount);
	virtual bool SetCondition(const BreakpointInfo& bp);
	virtual bool SetCommands(const BreakpointInfo& bp);
	virtual bool RemoveBreak(int bid);
	virtual bool RemoveAllBreaks();
	virtual bool StepIn();
	virtual bool StepOut();
	virtual bool Next();
	virtual bool Continue();
	virtual bool QueryFileLine();
	virtual bool Interrupt();
	virtual bool IsRunning();
	virtual bool ExecuteCmd(const wxString &cmd);
	virtual bool EvaluateExpressionToString(const wxString &expression, const wxString &format);
	virtual bool QueryLocals();
	virtual bool ListFrames();
	virtual bool ListThreads();
	virtual bool SelectThread(long threadId);
	virtual bool SetFrame(int frame);
	virtual void Poke();
	virtual bool GetAsciiViewerContent(const wxString &dbgCommand, const wxString &expression);
	virtual bool ResolveType(const wxString &expression);
	virtual bool WatchMemory(const wxString &address, size_t count);
	virtual bool SetMemory(const wxString &address, size_t count, const wxString &hex_value);
	virtual void SetDebuggerInformation(const DebuggerInformation &info);
	virtual void BreakList();
	virtual bool ListChildren(const wxString &name, int userReason);
	virtual bool CreateVariableObject(const wxString &expression, int userReason);
	virtual bool DeleteVariableObject(const wxString &name);
	virtual bool EvaluateVariableObject(const wxString &name, DisplayFormat displayFormat, int userReason);

	// Event handlers
	DECLARE_EVENT_TABLE()
	void OnProcessEnd(wxCommandEvent &e);
	void OnDataRead  (wxCommandEvent &e);
};
#endif //DBGINTERFACE_H
