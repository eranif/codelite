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

#include "AsyncProcess/asyncprocess.h"
#include "cl_command_event.h"
#include "consolefinder.h"
#include "debugger.h"
#include "ssh/ssh_account_info.h"

#include <vector>
#include <wx/event.h>
#include <wx/hashmap.h>
#include <wx/string.h>

// declare the debugger function creation
extern "C" IDebugger* CreateDebuggerGDB();
// declare the function that will be called by host application
// to retrieve the debugger initialization function
extern "C" const DebuggerInfo* GetDebuggerInfo();

class DbgCmdHandler;
class DbgCmdCLIHandler;
class IProcess;

using HandlersMap_t = std::map<wxString, DbgCmdHandler*>;

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

    // wrapper for convenience
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
    bool IsRecording() const override { return m_isRecording; }

public:
    DbgGdb();
    ~DbgGdb() override;

    //------ IDebugger ---------
    bool Start(const DebugSessionInfo& si, clEnvList_t* env_list) override;
    bool Attach(const DebugSessionInfo& si, clEnvList_t* env_list) override;
    bool Run(const wxString& args, const wxString& comm) override;
    bool Stop() override;
    bool Break(const clDebuggerBreakpoint& bp) override;
    bool SetEnabledState(double bid, const bool enable) override;
    bool SetIgnoreLevel(double bid, const int ignorecount) override;
    bool SetCondition(const clDebuggerBreakpoint& bp) override;
    bool SetCommands(const clDebuggerBreakpoint& bp) override;
    bool RemoveBreak(double bid) override;
    bool RemoveAllBreaks() override;
    bool StepIn() override;
    bool StepInInstruction() override;
    bool StepOut() override;
    bool Next() override;
    bool NextInstruction() override;
    bool Continue() override;
    bool QueryFileLine() override;
    bool Disassemble(const wxString& filename, int lineNumber) override;
    bool Interrupt() override;
    bool IsRunning() override;
    bool ExecuteCmd(const wxString& cmd) override;
    bool EvaluateExpressionToString(const wxString& expression, const wxString& format) override;
    bool QueryLocals() override;
    bool ListFrames() override;
    bool ListThreads() override;
    bool SelectThread(long threadId) override;
    bool SetFrame(int frame) override;
    void Poke() override;
    bool GetAsciiViewerContent(const wxString& dbgCommand, const wxString& expression) override;
    bool ResolveType(const wxString& expression, int userReason) override;
    bool WatchMemory(const wxString& address, size_t count, size_t columns) override;
    bool SetMemory(const wxString& address, size_t count, const wxString& hex_value) override;
    void SetDebuggerInformation(const DebuggerInformation& info) override;
    void BreakList() override;
    bool ListChildren(const wxString& name, int userReason) override;
    bool CreateVariableObject(const wxString& expression, bool persistent, int userReason) override;
    bool DeleteVariableObject(const wxString& name) override;
    bool EvaluateVariableObject(const wxString& name, int userReason) override;
    bool SetVariableObjectDisplayFormat(const wxString& name, DisplayFormat displayFormat) override;
    bool UpdateVariableObject(const wxString& name, int userReason) override;
    void AssignValue(const wxString& expression, const wxString& newValue) override;
    bool Jump(wxString filename, int line) override;
    bool ListRegisters() override;
    bool UpdateWatch(const wxString& name) override;
    void EnableReverseDebugging(bool b) override;
    void EnableRecording(bool b) override;
    bool IsReverseDebuggingEnabled() const override;

    /**
     * @brief restart the debugger (execute 'run')
     * @return true on success, false otherwise
     */
    bool Restart() override;

    // Event handlers
    void OnProcessEnd(clProcessEvent& e);
    void OnDataRead(clProcessEvent& e);
    void OnKillGDB(wxCommandEvent& e);
};
#endif // DBGINTERFACE_H
