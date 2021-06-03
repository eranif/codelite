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

#include "asyncprocess.h"
#include "cl_command_event.h"
#include "consolefinder.h"
#include "debugger.h"
#include "list"
#include "ssh_account_info.h"
#include "wx/event.h"
#include "wx/string.h"
#include <wx/hashmap.h>

#ifdef MSVC_VER
// declare the debugger function creation
extern "C++" IDebugger* CreateDebuggerGDB();
// declare the function that will be called by host application
// to retrieve the debugger initialization function
extern "C++" DebuggerInfo GetDebuggerInfo();
#else
// declare the debugger function creation
extern "C" IDebugger* CreateDebuggerGDB();
// declare the function that will be called by host application
// to retrieve the debugger initialization function
extern "C" DebuggerInfo GetDebuggerInfo();
#endif

class DbgCmdHandler;
class DbgCmdCLIHandler;
class IProcess;

typedef std::map<wxString, DbgCmdHandler*> HandlersMap_t;

extern const wxEventType wxEVT_GDB_STOP_DEBUGGER;

class DbgGdb : public wxEvtHandler, public IDebugger
{
    HandlersMap_t m_handlers;
    long m_debuggeePid;
    ConsoleFinder m_consoleFinder;
    std::vector<clDebuggerBreakpoint> m_bpList;
    DbgCmdCLIHandler* m_cliHandler;
    IProcess* m_gdbProcess;
    wxArrayString m_gdbOutputArr;
    wxString m_gdbOutputIncompleteLine;
    bool m_break_at_main;
    bool m_attachedMode;
    bool m_goingDown;
    bool m_reverseDebugging;
    wxStringSet_t m_reversableCommands;
    bool m_isRecording;

public:
    int m_internalBpId;

protected:
    void RegisterHandler(const wxString& id, DbgCmdHandler* cmd);
    DbgCmdHandler* PopHandler(const wxString& id);
    void EmptyQueue();
    bool FilterMessage(const wxString& msg);
    bool DoGetNextLine(wxString& line);
    void DoCleanup();

    // wrapper for convinience
    void DoProcessAsyncCommand(wxString& line, wxString& id);

protected:
    bool DoLocateGdbExecutable(const wxString& debuggerPath, wxString& dbgExeName, const DebugSessionInfo& sessionInfo);
    bool DoInitializeGdb(const DebugSessionInfo& sessionInfo);
    void SetCliHandler(DbgCmdCLIHandler* handler);
    DbgCmdCLIHandler* GetCliHandler();

public:
    bool WriteCommand(const wxString& command, DbgCmdHandler* cmd);
    bool ExecCLICommand(const wxString& command, DbgCmdCLIHandler* cmd);
    void SetBreakpoints();
    void SetInternalMainBpID(int bpId);
    void SetShouldBreakAtMain(bool break_at_main) { m_break_at_main = break_at_main; }
    bool GetShouldBreakAtMain() const { return m_break_at_main; }
    void GetDebugeePID(const wxString& line);

    void SetGoingDown(bool goingDown) { this->m_goingDown = goingDown; }
    bool IsGoingDown() const { return m_goingDown; }

    const std::vector<clDebuggerBreakpoint>& GetBpList() const { return m_bpList; }

    void SetIsRecording(bool isRecording) { this->m_isRecording = isRecording; }
    bool IsRecording() const { return m_isRecording; }

public:
    DbgGdb();
    virtual ~DbgGdb();

    //------ IDebugger ---------
    virtual bool Start(const DebugSessionInfo& si, clEnvList_t* env_list);
    virtual bool Attach(const DebugSessionInfo& si, clEnvList_t* env_list);
    virtual bool Run(const wxString& args, const wxString& comm);
    virtual bool Stop();
    virtual bool Break(const clDebuggerBreakpoint& bp);
    virtual bool SetEnabledState(double bid, const bool enable);
    virtual bool SetIgnoreLevel(double bid, const int ignorecount);
    virtual bool SetCondition(const clDebuggerBreakpoint& bp);
    virtual bool SetCommands(const clDebuggerBreakpoint& bp);
    virtual bool RemoveBreak(double bid);
    virtual bool RemoveAllBreaks();
    virtual bool StepIn();
    virtual bool StepInInstruction();
    virtual bool StepOut();
    virtual bool Next();
    virtual bool NextInstruction();
    virtual bool Continue();
    virtual bool QueryFileLine();
    virtual bool Disassemble(const wxString& filename, int lineNumber);
    virtual bool Interrupt();
    virtual bool IsRunning();
    virtual bool ExecuteCmd(const wxString& cmd);
    virtual bool EvaluateExpressionToString(const wxString& expression, const wxString& format);
    virtual bool QueryLocals();
    virtual bool ListFrames();
    virtual bool ListThreads();
    virtual bool SelectThread(long threadId);
    virtual bool SetFrame(int frame);
    virtual void Poke();
    virtual bool GetAsciiViewerContent(const wxString& dbgCommand, const wxString& expression);
    virtual bool ResolveType(const wxString& expression, int userReason);
    virtual bool WatchMemory(const wxString& address, size_t count, size_t columns);
    virtual bool SetMemory(const wxString& address, size_t count, const wxString& hex_value);
    virtual void SetDebuggerInformation(const DebuggerInformation& info);
    virtual void BreakList();
    virtual bool ListChildren(const wxString& name, int userReason);
    virtual bool CreateVariableObject(const wxString& expression, bool persistent, int userReason);
    virtual bool DeleteVariableObject(const wxString& name);
    virtual bool EvaluateVariableObject(const wxString& name, int userReason);
    virtual bool SetVariableObbjectDisplayFormat(const wxString& name, DisplayFormat displayFormat);
    virtual bool UpdateVariableObject(const wxString& name, int userReason);
    virtual void AssignValue(const wxString& expression, const wxString& newValue);
    virtual bool Jump(wxString filename, int line);
    virtual bool ListRegisters();
    virtual bool UpdateWatch(const wxString& name);
    virtual void EnableReverseDebugging(bool b);
    virtual void EnableRecording(bool b);
    virtual bool IsReverseDebuggingEnabled() const;

    /**
     * @brief restart the debugger (execute 'run')
     * @return true on success, false otherwise
     */
    virtual bool Restart();

    // Event handlers
    void OnProcessEnd(clProcessEvent& e);
    void OnDataRead(clProcessEvent& e);
    void OnKillGDB(wxCommandEvent& e);
};
#endif // DBGINTERFACE_H
