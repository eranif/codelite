//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : dbgcmd.h
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
#ifndef DBGCMD_H
#define DBGCMD_H

#include "debugger.h"
#include "debuggerobserver.h"
#include "wx/event.h"
#include "wx/string.h"

class IDebugger;
class DbgGdb;

#define GDB_NEXT_TOKEN()                              \
    {                                                 \
        type = gdb_result_lex();                      \
        currentToken = _U(gdb_result_string.c_str()); \
    }

#define GDB_ABORT(ch)     \
    if(type != (int)ch) { \
        break;            \
    }

class DbgCmdHandler
{
protected:
    IDebuggerObserver* m_observer;

public:
    DbgCmdHandler(IDebuggerObserver* observer)
        : m_observer(observer)
    {
    }
    virtual ~DbgCmdHandler() {}

    virtual bool WantsErrors() const { return false; }

    virtual bool ProcessOutput(const wxString& line) = 0;
};

/**
 * handles the
 * -file-list-exec-source-file command
 */
class DbgCmdHandlerGetLine : public DbgCmdHandler
{
    DbgGdb* m_gdb;

public:
    DbgCmdHandlerGetLine(IDebuggerObserver* observe, DbgGdb* gdbr)
        : DbgCmdHandler(observe)
        , m_gdb(gdbr)
    {
    }

    virtual ~DbgCmdHandlerGetLine() {}

    virtual bool ProcessOutput(const wxString& line);
};

class DbgCmdHandlerDisasseble : public DbgCmdHandler
{
    DbgGdb* m_gdb;

public:
    DbgCmdHandlerDisasseble(IDebuggerObserver* observe, DbgGdb* gdbr)
        : DbgCmdHandler(observe)
        , m_gdb(gdbr)
    {
    }

    virtual ~DbgCmdHandlerDisasseble() {}

    virtual bool ProcessOutput(const wxString& line);
};

class DbgCmdHandlerDisassebleCurLine : public DbgCmdHandler
{
    DbgGdb* m_gdb;

public:
    DbgCmdHandlerDisassebleCurLine(IDebuggerObserver* observe, DbgGdb* gdbr)
        : DbgCmdHandler(observe)
        , m_gdb(gdbr)
    {
    }

    virtual ~DbgCmdHandlerDisassebleCurLine() {}

    virtual bool ProcessOutput(const wxString& line);
};

/**
 * @class DbgCmdHandlerStackInfo
 * handles the -stack-info-frame command
 */
class DbgCmdHandlerStackDepth : public DbgCmdHandler
{
    DbgGdb* m_gdb;

public:
    DbgCmdHandlerStackDepth(IDebuggerObserver* observe, DbgGdb* gdbr)
        : DbgCmdHandler(observe)
        , m_gdb(gdbr)
    {
    }

    virtual ~DbgCmdHandlerStackDepth() {}

    virtual bool ProcessOutput(const wxString& line);
};

/**
 * this handler, handles the following commands:
 * -exec-run
 * -exec-continue
 * -exec-step
 * -exec-next
 * -exec-finish
 */
class DbgCmdHandlerAsyncCmd : public DbgCmdHandler
{
protected:
    DbgGdb* m_gdb;

public:
    DbgCmdHandlerAsyncCmd(IDebuggerObserver* observer, DbgGdb* gdb)
        : DbgCmdHandler(observer)
        , m_gdb(gdb)
    {
    }
    virtual ~DbgCmdHandlerAsyncCmd() {}

    void UpdateGotControl(DebuggerReasons reason, const wxString& func);
    virtual bool ProcessOutput(const wxString& line);
};

class DbgCmdHandlerExecRun : public DbgCmdHandlerAsyncCmd
{
public:
    DbgCmdHandlerExecRun(IDebuggerObserver* observer, DbgGdb* gdb)
        : DbgCmdHandlerAsyncCmd(observer, gdb)
    {
    }

    virtual ~DbgCmdHandlerExecRun() {}
    virtual bool ProcessOutput(const wxString& line);
    virtual bool WantsErrors() const { return true; }
};

class DbgCmdHandlerRemoteDebugging : public DbgCmdHandler
{
    IDebugger* m_debugger;

public:
    DbgCmdHandlerRemoteDebugging(IDebuggerObserver* observer, IDebugger* debugger)
        : DbgCmdHandler(observer)
        , m_debugger(debugger)
    {
    }

    virtual ~DbgCmdHandlerRemoteDebugging() {}

    virtual bool ProcessOutput(const wxString& line);
};

class DbgCmdHandlerBp : public DbgCmdHandler
{
    const clDebuggerBreakpoint m_bp;
    std::vector<clDebuggerBreakpoint>* m_bplist;
    int m_bpType; // BP_type_break by default
    DbgGdb* m_debugger;

public:
    DbgCmdHandlerBp(IDebuggerObserver* observer, DbgGdb* debugger, clDebuggerBreakpoint bp,
                    std::vector<clDebuggerBreakpoint>* bplist, int bptype = BP_type_break)
        : DbgCmdHandler(observer)
        , m_bp(bp)
        , m_bplist(bplist)
        , m_bpType(bptype)
        , m_debugger(debugger)
    {
    }

    virtual ~DbgCmdHandlerBp() {}
    virtual bool ProcessOutput(const wxString& line);
    virtual bool WantsErrors() const { return true; }
};

class DbgCmdHandlerLocals : public DbgCmdHandler
{
public:
    DbgCmdHandlerLocals(IDebuggerObserver* observer)
        : DbgCmdHandler(observer)
    {
    }
    virtual ~DbgCmdHandlerLocals() {}
    virtual bool ProcessOutput(const wxString& line);
};

// A Void Handler, which is here simply to ignore a reply from the debugger
class DbgCmdHandlerVarCreator : public DbgCmdHandler
{
public:
    DbgCmdHandlerVarCreator(IDebuggerObserver* observer)
        : DbgCmdHandler(observer)
    {
    }
    virtual ~DbgCmdHandlerVarCreator() {}
    virtual bool ProcessOutput(const wxString& line);
};

class DbgCmdHandlerEvalExpr : public DbgCmdHandler
{
    wxString m_expression;

public:
    DbgCmdHandlerEvalExpr(IDebuggerObserver* observer, const wxString& expression)
        : DbgCmdHandler(observer)
        , m_expression(expression)
    {
    }

    virtual ~DbgCmdHandlerEvalExpr() {}
    virtual bool ProcessOutput(const wxString& line);
    virtual const wxString& GetExpression() const { return m_expression; }
};

// handler -list-stack-frames command
class DbgCmdStackList : public DbgCmdHandler
{
public:
    DbgCmdStackList(IDebuggerObserver* observer)
        : DbgCmdHandler(observer)
    {
    }
    virtual ~DbgCmdStackList() {}
    virtual bool ProcessOutput(const wxString& line);
};

// handler -list-stack-frames command
class DbgCmdSelectFrame : public DbgCmdHandler
{
public:
    DbgCmdSelectFrame(IDebuggerObserver* observer)
        : DbgCmdHandler(observer)
    {
    }
    virtual ~DbgCmdSelectFrame() {}
    virtual bool ProcessOutput(const wxString& line);
};

// Used for Ignore etc
class DbgCmdDisplayOutput : public DbgCmdHandler
{
public:
    DbgCmdDisplayOutput(IDebuggerObserver* observer)
        : DbgCmdHandler(observer)
    {
    }
    virtual ~DbgCmdDisplayOutput() {}
    virtual bool ProcessOutput(const wxString& line);
};

class DbgCmdResolveTypeHandler : public DbgCmdHandler
{
    DbgGdb* m_debugger;
    wxString m_expression;
    int m_userReason;

public:
    DbgCmdResolveTypeHandler(const wxString& expression, DbgGdb* debugger, int userReason);

    virtual ~DbgCmdResolveTypeHandler() {}
    virtual bool ProcessOutput(const wxString& line);
    virtual bool WantsErrors() const { return true; }
};

class DbgCmdCLIHandler : public DbgCmdHandler
{
    wxString m_output;
    wxString m_commandId;

public:
    DbgCmdCLIHandler(IDebuggerObserver* observer)
        : DbgCmdHandler(observer)
    {
    }

    virtual ~DbgCmdCLIHandler() {}
    virtual bool ProcessOutput(const wxString& line);

    const wxString& GetOutput() const { return m_output; }

    void SetCommandId(const wxString& commandId) { this->m_commandId = commandId; }

    const wxString& GetCommandId() const { return m_commandId; }

    void Append(const wxString& text) { m_output.Append(text + wxT("\n")); }
};

class DbgCmdGetTipHandler : public DbgCmdCLIHandler
{

    wxString m_expression;

public:
    DbgCmdGetTipHandler(IDebuggerObserver* observer, const wxString& expression)
        : DbgCmdCLIHandler(observer)
        , m_expression(expression)
    {
    }

    virtual ~DbgCmdGetTipHandler() {}
    virtual bool ProcessOutput(const wxString& line);
};

// Callback for handling 'set condition' command
class DbgCmdSetConditionHandler : public DbgCmdHandler
{
    clDebuggerBreakpoint m_bp;

public:
    DbgCmdSetConditionHandler(IDebuggerObserver* observer, const clDebuggerBreakpoint& bp)
        : DbgCmdHandler(observer)
        , m_bp(bp)
    {
    }
    virtual ~DbgCmdSetConditionHandler() {}

    virtual bool ProcessOutput(const wxString& line);
};

// Callback for handling parsing the -break-list output from
// the debugger
class DbgCmdBreakList : public DbgCmdHandler
{
    DbgGdb* m_gdb = nullptr;

public:
    DbgCmdBreakList(DbgGdb* gdb)
        : DbgCmdHandler(nullptr)
        , m_gdb(gdb)
    {
    }
    virtual ~DbgCmdBreakList() {}

    virtual bool ProcessOutput(const wxString& line);
};

// Callback for handling threads info
// command
class DbgCmdListThreads : public DbgCmdHandler
{
public:
    DbgCmdListThreads(IDebuggerObserver* observer)
        : DbgCmdHandler(observer)
    {
    }
    virtual ~DbgCmdListThreads() {}

    virtual bool ProcessOutput(const wxString& line);
};

// Callback for handling the '-data-read-memory' command
class DbgCmdWatchMemory : public DbgCmdHandler
{
    wxString m_address;
    size_t m_count;
    size_t m_columns;

public:
    DbgCmdWatchMemory(IDebuggerObserver* observer, const wxString& address, size_t count, size_t columns)
        : DbgCmdHandler(observer)
        , m_address(address)
        , m_count(count)
        , m_columns(columns)
    {
    }
    virtual ~DbgCmdWatchMemory() {}
    virtual bool ProcessOutput(const wxString& line);
};

// Handle the 'CreateVariableObject' call
class DbgCmdCreateVarObj : public DbgCmdHandler
{
    wxString m_expression;
    int m_userReason;
    DbgGdb* m_debugger;

public:
    DbgCmdCreateVarObj(IDebuggerObserver* observer, DbgGdb* gdb, const wxString& expression, int userReason)
        : DbgCmdHandler(observer)
        , m_expression(expression)
        , m_userReason(userReason)
        , m_debugger(gdb)
    {
    }

    /**
     * @brief we want to handle error ourselves
     */
    virtual bool WantsErrors() const { return true; }

    virtual ~DbgCmdCreateVarObj() {}

    virtual bool ProcessOutput(const wxString& line);
};

// Handle the 'DbgCmdListChildren' call
class DbgCmdListChildren : public DbgCmdHandler
{
    wxString m_variable;
    int m_userReason;

public:
    DbgCmdListChildren(IDebuggerObserver* observer, const wxString& variable, int userReason)
        : DbgCmdHandler(observer)
        , m_variable(variable)
        , m_userReason(userReason)
    {
    }

    virtual ~DbgCmdListChildren() {}

    virtual bool ProcessOutput(const wxString& line);
};

class DbgCmdEvalVarObj : public DbgCmdHandler
{
    wxString m_variable;
    int m_userReason;

public:
    DbgCmdEvalVarObj(IDebuggerObserver* observer, const wxString& variable, int userReason)
        : DbgCmdHandler(observer)
        , m_variable(variable)
        , m_userReason(userReason)
    {
    }

    virtual ~DbgCmdEvalVarObj() {}

    virtual bool ProcessOutput(const wxString& line);
};

class DbgFindMainBreakpointIdHandler : public DbgCmdHandler
{
    DbgGdb* m_debugger;

public:
    DbgFindMainBreakpointIdHandler(IDebuggerObserver* observer, DbgGdb* debugger)
        : DbgCmdHandler(observer)
        , m_debugger(debugger)
    {
    }

    virtual ~DbgFindMainBreakpointIdHandler() {}

    virtual bool ProcessOutput(const wxString& line);
};

class DbgVarObjUpdate : public DbgCmdHandler
{
    wxString m_variableName;
    DbgGdb* m_debugger;
    int m_userReason;

public:
    DbgVarObjUpdate(IDebuggerObserver* observer, DbgGdb* debugger, const wxString& name, int userReason)
        : DbgCmdHandler(observer)
        , m_variableName(name)
        , m_debugger(debugger)
        , m_userReason(userReason)
    {
    }

    virtual ~DbgVarObjUpdate() {}

    virtual bool ProcessOutput(const wxString& line);
    virtual bool WantsErrors() { return true; }
};

// Callback for handling jump command questions
// command
class DbgCmdJumpHandler : public DbgCmdCLIHandler
{
public:
    DbgCmdJumpHandler(IDebuggerObserver* observer)
        : DbgCmdCLIHandler(observer)
    {
    }
    virtual ~DbgCmdJumpHandler() {}

    virtual bool ProcessOutput(const wxString& line);
};

class DbgCmdStopHandler : public DbgCmdHandler
{
public:
    DbgCmdStopHandler(IDebuggerObserver* observer)
        : DbgCmdHandler(observer)
    {
    }
    virtual ~DbgCmdStopHandler() {}

    virtual bool ProcessOutput(const wxString& line);
};

class DbgCmdRecordHandler : public DbgCmdHandler
{
    DbgGdb* m_gdb;

public:
    DbgCmdRecordHandler(IDebuggerObserver* observer, DbgGdb* gdb)
        : DbgCmdHandler(observer)
        , m_gdb(gdb)
    {
    }
    virtual ~DbgCmdRecordHandler() {}

    virtual bool ProcessOutput(const wxString& line);
};

// +++-----------------------------
// DbgCmdHandlerRegisterNames
// +++-----------------------------

class DbgCmdHandlerRegisterNames : public DbgCmdHandler
{
    DbgGdb* m_gdb;
    std::map<int, wxString> m_numberToName;

public:
    DbgCmdHandlerRegisterNames(IDebuggerObserver* observer, DbgGdb* gdbr)
        : DbgCmdHandler(observer)
        , m_gdb(gdbr)
    {
    }

    virtual ~DbgCmdHandlerRegisterNames() {}

    virtual bool ProcessOutput(const wxString& line);
};

// +++-----------------------------
// DbgCmdHandlerRegisterValues
// +++-----------------------------
class DbgCmdHandlerRegisterValues : public DbgCmdHandler
{
    DbgGdb* m_gdb;
    std::map<int, wxString> m_numberToName;

public:
    DbgCmdHandlerRegisterValues(IDebuggerObserver* observer, DbgGdb* gdbr, const std::map<int, wxString>& numberToName)
        : DbgCmdHandler(observer)
        , m_gdb(gdbr)
    {
        m_numberToName = numberToName;
    }

    virtual ~DbgCmdHandlerRegisterValues() {}

    virtual bool ProcessOutput(const wxString& line);
};
#endif // DBGCMD_H
