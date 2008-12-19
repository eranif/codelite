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

class DbgCmdHandler {
protected:
	IDebuggerObserver *m_observer;

public:
	DbgCmdHandler(IDebuggerObserver *observer) : m_observer(observer){}
	virtual ~DbgCmdHandler(){}

	virtual bool ProcessOutput(const wxString &line) = 0;
};

/**
 * handles the 
 * -file-list-exec-source-file command
 */
class DbgCmdHandlerGetLine : public DbgCmdHandler {
public:
	DbgCmdHandlerGetLine(IDebuggerObserver *observer) : DbgCmdHandler(observer){}
	virtual ~DbgCmdHandlerGetLine(){}

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
class DbgCmdHandlerAsyncCmd : public DbgCmdHandler {
public:
	DbgCmdHandlerAsyncCmd(IDebuggerObserver *observer) : DbgCmdHandler(observer){}
	virtual ~DbgCmdHandlerAsyncCmd(){}

	virtual bool ProcessOutput(const wxString &line);
};

class DbgCmdHandlerRemoteDebugging : public DbgCmdHandler {
	IDebugger *m_debugger;
	
public:
	DbgCmdHandlerRemoteDebugging(IDebuggerObserver *observer, IDebugger *debugger) : DbgCmdHandler(observer), m_debugger(debugger)
	{}
	
	virtual ~DbgCmdHandlerRemoteDebugging(){}

	virtual bool ProcessOutput(const wxString &line);
};

class DbgCmdHandlerBp : public DbgCmdHandler {
	const BreakpointInfo m_bp;
	std::vector< BreakpointInfo > *m_bplist;
	int m_bpType; // BP_type_break by default
	static int m_debuggerID;
	
public:
	DbgCmdHandlerBp(IDebuggerObserver *observer, BreakpointInfo bp, std::vector< BreakpointInfo > *bplist, int bptype = BP_type_break) 
	: DbgCmdHandler(observer)
	, m_bp(bp) 
	, m_bplist(bplist)
	, m_bpType(bptype)
	{}
	
	static void StoreDebuggerID(const int debugger_id);	// Store the int that will become BreakpointInfo::debugger_id 
	static int RetrieveDebuggerID();
	virtual ~DbgCmdHandlerBp(){}
	virtual bool ProcessOutput(const wxString &line);
};

class DbgCmdHandlerLocals : public DbgCmdHandler {
public:
	enum {
		EvaluateExpression,
		Locals,
		This,
		FunctionArguments
	};
	
protected:
	void MakeTree(TreeNode<wxString, NodeData> *parent);
	void MakeTreeFromFrame(wxString &strline, TreeNode<wxString, NodeData> *parent);
	void MakeSubTree(TreeNode<wxString, NodeData> *parent);
	int m_evaluateExpression;
	wxString m_expression;

public:
	DbgCmdHandlerLocals(IDebuggerObserver *observer, int kind = Locals, const wxString &expr = wxEmptyString) 
		: DbgCmdHandler(observer)
		, m_evaluateExpression(kind)
		, m_expression(expr)
	{}

	virtual ~DbgCmdHandlerLocals(){}

	virtual bool ProcessOutput(const wxString &line);
};

// A Void Handler, which is here simply to ignore a reply from the debugger
class DbgCmdHandlerVarCreator : public DbgCmdHandler {
public:
	DbgCmdHandlerVarCreator(IDebuggerObserver *observer) : DbgCmdHandler(observer){}
	virtual ~DbgCmdHandlerVarCreator(){}
	virtual bool ProcessOutput(const wxString & line);
};

class DbgCmdHandlerEvalExpr : public DbgCmdHandler {
	wxString m_expression;
public:
	DbgCmdHandlerEvalExpr(IDebuggerObserver *observer, const wxString &expression) 
		: DbgCmdHandler(observer)
		, m_expression(expression)
	{}

	virtual ~DbgCmdHandlerEvalExpr(){}
	virtual bool ProcessOutput(const wxString & line);
	virtual const wxString & GetExpression() const {return m_expression;}
};

// handler -list-stack-frames command
class DbgCmdStackList : public DbgCmdHandler {
public:
	DbgCmdStackList(IDebuggerObserver *observer) : DbgCmdHandler(observer){}
	virtual ~DbgCmdStackList(){}
	virtual bool ProcessOutput(const wxString & line);
};

// handler -list-stack-frames command
class DbgCmdSelectFrame : public DbgCmdHandler {
public:
	DbgCmdSelectFrame(IDebuggerObserver *observer) : DbgCmdHandler(observer){}
	virtual ~DbgCmdSelectFrame(){}
	virtual bool ProcessOutput(const wxString & line);
};

// Used for Ignore etc
class DbgCmdDisplayOutput : public DbgCmdHandler {
public:
	DbgCmdDisplayOutput(IDebuggerObserver *observer) : DbgCmdHandler(observer){}
	virtual ~DbgCmdDisplayOutput(){}
	virtual bool ProcessOutput(const wxString & line);
};
#endif //DBGCMD_H

