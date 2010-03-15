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

#include "wx/string.h"
#include "wx/event.h"
#include "debuggerobserver.h"
#include "debugger.h"

class IDebugger;
class DbgGdb;

#define GDB_NEXT_TOKEN()\
	{\
		type = gdb_result_lex();\
		currentToken = _U(gdb_result_string.c_str());\
	}

#define GDB_ABORT(ch)\
	if(type != (int)ch){\
		break;\
	}

class DbgCmdHandler
{
protected:
	IDebuggerObserver *m_observer;

public:
	DbgCmdHandler(IDebuggerObserver *observer) : m_observer(observer) {}
	virtual ~DbgCmdHandler() {}

	virtual bool WantsErrors() const {
		return false;
	}

	virtual bool ProcessOutput(const wxString &line) = 0;
};

/**
 * handles the
 * -file-list-exec-source-file command
 */
class DbgCmdHandlerGetLine : public DbgCmdHandler
{
public:
	DbgCmdHandlerGetLine(IDebuggerObserver *observer) : DbgCmdHandler(observer) {}
	virtual ~DbgCmdHandlerGetLine() {}

	virtual bool ProcessOutput(const wxString &line);
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
public:
	DbgCmdHandlerAsyncCmd(IDebuggerObserver *observer) : DbgCmdHandler(observer) {}
	virtual ~DbgCmdHandlerAsyncCmd() {}

	virtual bool ProcessOutput(const wxString &line);
};

class DbgCmdHandlerRemoteDebugging : public DbgCmdHandler
{
	IDebugger *m_debugger;

public:
	DbgCmdHandlerRemoteDebugging(IDebuggerObserver *observer, IDebugger *debugger) : DbgCmdHandler(observer), m_debugger(debugger) {}

	virtual ~DbgCmdHandlerRemoteDebugging() {}

	virtual bool ProcessOutput(const wxString &line);
};

class DbgCmdHandlerBp : public DbgCmdHandler
{
	const BreakpointInfo           m_bp;
	std::vector< BreakpointInfo > *m_bplist;
	int                            m_bpType; // BP_type_break by default
	IDebugger*                     m_debugger;

public:
	DbgCmdHandlerBp(IDebuggerObserver *observer, IDebugger *debugger, BreakpointInfo bp, std::vector< BreakpointInfo > *bplist, int bptype = BP_type_break)
			: DbgCmdHandler(observer)
			, m_bp(bp)
			, m_bplist(bplist)
			, m_bpType(bptype)
			, m_debugger(debugger) {}

	virtual ~DbgCmdHandlerBp() {}
	virtual bool ProcessOutput(const wxString &line);
	virtual bool WantsErrors() const {
		return true;
	}
};

class DbgCmdHandlerLocals : public DbgCmdHandler
{
public:
	DbgCmdHandlerLocals(IDebuggerObserver *observer)
			: DbgCmdHandler(observer) {}
	virtual ~DbgCmdHandlerLocals() {}
	virtual bool ProcessOutput(const wxString &line);
};

class DbgCmdHandlerFuncArgs : public DbgCmdHandler
{
public:
	DbgCmdHandlerFuncArgs(IDebuggerObserver *observer)
			: DbgCmdHandler(observer) {}
	virtual ~DbgCmdHandlerFuncArgs() {}
	virtual bool ProcessOutput(const wxString &line);
};

// A Void Handler, which is here simply to ignore a reply from the debugger
class DbgCmdHandlerVarCreator : public DbgCmdHandler
{
public:
	DbgCmdHandlerVarCreator(IDebuggerObserver *observer) : DbgCmdHandler(observer) {}
	virtual ~DbgCmdHandlerVarCreator() {}
	virtual bool ProcessOutput(const wxString & line);
};

class DbgCmdHandlerEvalExpr : public DbgCmdHandler
{
	wxString m_expression;
public:
	DbgCmdHandlerEvalExpr(IDebuggerObserver *observer, const wxString &expression)
			: DbgCmdHandler(observer)
			, m_expression(expression) {}

	virtual ~DbgCmdHandlerEvalExpr() {}
	virtual bool ProcessOutput(const wxString & line);
	virtual const wxString & GetExpression() const {
		return m_expression;
	}
};

// handler -list-stack-frames command
class DbgCmdStackList : public DbgCmdHandler
{
public:
	DbgCmdStackList(IDebuggerObserver *observer) : DbgCmdHandler(observer) {}
	virtual ~DbgCmdStackList() {}
	virtual bool ProcessOutput(const wxString & line);
};

// handler -list-stack-frames command
class DbgCmdSelectFrame : public DbgCmdHandler
{
public:
	DbgCmdSelectFrame(IDebuggerObserver *observer) : DbgCmdHandler(observer) {}
	virtual ~DbgCmdSelectFrame() {}
	virtual bool ProcessOutput(const wxString & line);
};

// Used for Ignore etc
class DbgCmdDisplayOutput : public DbgCmdHandler
{
public:
	DbgCmdDisplayOutput(IDebuggerObserver *observer) : DbgCmdHandler(observer) {}
	virtual ~DbgCmdDisplayOutput() {}
	virtual bool ProcessOutput(const wxString & line);
};

class DbgCmdResolveTypeHandler : public DbgCmdHandler
{
	DbgGdb * m_debugger;
	wxString m_expression;

public:
	DbgCmdResolveTypeHandler(const wxString &expression, DbgGdb *debugger);

	virtual ~DbgCmdResolveTypeHandler() {}
	virtual bool ProcessOutput(const wxString & line);
};

class DbgCmdCLIHandler : public DbgCmdHandler
{
	wxString m_output;
	wxString m_commandId;

public:
	DbgCmdCLIHandler(IDebuggerObserver *observer) : DbgCmdHandler(observer) {}

	virtual ~DbgCmdCLIHandler() {}
	virtual bool ProcessOutput(const wxString & line);

	const wxString& GetOutput() const {
		return m_output;
	}

	void SetCommandId(const wxString& commandId) {
		this->m_commandId = commandId;
	}

	const wxString& GetCommandId() const {
		return m_commandId;
	}

	void Append( const wxString &text ) {
		m_output.Append(text + wxT("\n"));
	}
};

class DbgCmdGetTipHandler : public DbgCmdCLIHandler
{

	wxString m_expression;

public:
	DbgCmdGetTipHandler(IDebuggerObserver *observer, const wxString &expression) : DbgCmdCLIHandler(observer), m_expression(expression) {}

	virtual ~DbgCmdGetTipHandler() {}
	virtual bool ProcessOutput(const wxString & line);

};

// Callback for handling 'set condition' command
class DbgCmdSetConditionHandler : public DbgCmdHandler
{
	BreakpointInfo m_bp;
public:
	DbgCmdSetConditionHandler(IDebuggerObserver *observer, const BreakpointInfo& bp) : DbgCmdHandler(observer), m_bp(bp) {}
	virtual ~DbgCmdSetConditionHandler() {}

	virtual bool ProcessOutput(const wxString & line);
};

// Callback for handling parsing the -break-list output from
// the debugger
class DbgCmdBreakList : public DbgCmdHandler
{
public:
	DbgCmdBreakList(IDebuggerObserver *observer) : DbgCmdHandler(observer) {}
	virtual ~DbgCmdBreakList() {}

	virtual bool ProcessOutput(const wxString & line);
};

// Callback for handling threads info
// command
class DbgCmdListThreads : public DbgCmdCLIHandler
{
public:
	DbgCmdListThreads(IDebuggerObserver *observer) : DbgCmdCLIHandler(observer) {}
	virtual ~DbgCmdListThreads() {}

	virtual bool ProcessOutput(const wxString & line);
};

// Callback for handling the '-data-read-memory' command
class DbgCmdWatchMemory : public DbgCmdHandler
{
	wxString m_address;
	size_t   m_count;

public:
	DbgCmdWatchMemory(IDebuggerObserver *observer, const wxString &address, size_t count) : DbgCmdHandler(observer), m_address(address), m_count(count) {}
	virtual ~DbgCmdWatchMemory() {}

	virtual bool ProcessOutput(const wxString & line);
};

// Handle the 'CreateVariableObject' call
class DbgCmdCreateVarObj : public DbgCmdHandler
{
	wxString m_expression;
	int      m_userReason;
	DbgGdb * m_debugger;
	
public:
	DbgCmdCreateVarObj(IDebuggerObserver *observer, DbgGdb *gdb, const wxString &expression, int userReason)
			: DbgCmdHandler(observer)
			, m_debugger(gdb)
			, m_expression(expression)
			, m_userReason(userReason) {}

	virtual ~DbgCmdCreateVarObj() {}

	virtual bool ProcessOutput(const wxString & line);
};

// Handle the 'DbgCmdListChildren' call
class DbgCmdListChildren : public DbgCmdHandler
{
	wxString m_variable;
	int      m_userReason;
public:
	DbgCmdListChildren(IDebuggerObserver *observer, const wxString &variable, int userReason)
			: DbgCmdHandler(observer)
			, m_variable(variable)
			, m_userReason(userReason) {}

	virtual ~DbgCmdListChildren() {}

	virtual bool ProcessOutput(const wxString & line);
};

class DbgCmdEvalVarObj : public DbgCmdHandler
{
	wxString      m_variable;
	int           m_userReason;
	DisplayFormat m_displayFormat;
	
public:
	DbgCmdEvalVarObj(IDebuggerObserver *observer, const wxString &variable, DisplayFormat displayFormat, int userReason)
			: DbgCmdHandler  (observer)
			, m_variable     (variable)
			, m_displayFormat(displayFormat)
			, m_userReason   ( userReason ) {}

	virtual ~DbgCmdEvalVarObj() {}

	virtual bool ProcessOutput(const wxString & line);
};
#endif //DBGCMD_H
