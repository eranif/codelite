//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debuggerobserver.h
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
 #ifndef DEBUGGER_OBSERVER_H
#define DEBUGGER_OBSERVER_H

#include "wx/treectrl.h"
#include "tree_node.h"
#include "debugger.h"

struct NodeData
{
	wxString name;
	wxTreeItemId itemId;
};

enum DebuggerReasons
{
	DBG_BP_HIT,
	DBG_BP_ASSERTION_HIT,
	DBG_RECV_SIGNAL,
	DBG_RECV_SIGNAL_SIGABRT,
	DBG_RECV_SIGNAL_EXC_BAD_ACCESS,
	DBG_RECV_SIGNAL_SIGSEGV,
	DBG_END_STEPPING,
	DBG_EXITED_NORMALLY,
	DBG_DBGR_KILLED,
	DBG_CMD_ERROR,
	DBG_FUNC_FINISHED,
	DBG_UNKNOWN
};

enum DebuggerUpdateReason
{
	DBG_UR_INVALID = -1,            // Invalid
	DBG_UR_GOT_CONTROL,             // Application gains the control back from the debugger
	DBG_UR_LOST_CONTROL,            // Appliecation lost control to the debugge, and it can not interact (atm) with it
	DBG_UR_FILE_LINE,               // The debugger is at file / line
	DBG_UR_ADD_LINE,                // Log line
	DBG_UR_BP_ADDED,                // Breakpoint was added
	DBG_UR_STOPPED,                 // Debugger stopped
	DBG_UR_LOCALS,                  // Local variables are available
	DBG_UR_EXPRESSION,              // A requested expression evaluation has completed and is available
	DBG_UR_UPDATE_STACK_LIST,       // Stack List is available
	DBG_UR_REMOTE_TARGET_CONNECTED, // Remove target is now connected
	DBG_UR_RECONCILE_BPTS,          // Reconcile breakpoints is needed
	DBG_UR_BP_HIT,                  // Breakpoint was hit
	DBG_UR_TYPE_RESOLVED,           // The debugger has evaluated the a type
	DBG_UR_TIP,                     // Tip is available
	DBG_UR_WATCHMEMORY,             // Watch memory is available
	DBG_UR_LISTTHRAEDS,             // Threads list is available
	DBG_UR_LISTCHILDREN,            // Children list for a variable object is available
	DBG_UR_VARIABLEOBJ,             // Variable object was created
	DBG_UR_EVALVARIABLEOBJ          // Variable object has be evaluated
};

struct DebuggerEvent {
	DebuggerUpdateReason          m_updateReason;  // Event reason - the reason why this event was sent
												   // ==================================================
												   // Available when the following UpdateReason are set:
												   // ==================================================
	DebuggerReasons               m_controlReason; // DBG_UR_GOT_CONTROL
	wxString                      m_file;          // DBG_UR_FILE_LINE
	int                           m_line;          // DBG_UR_FILE_LINE
	wxString                      m_text;          // DBG_UR_ADD_LINE, DBG_UR_REMOTE_TARGET_CONNECTED, DBG_UR_TIP
	int                           m_bpInternalId;  // DBG_UR_BP_ADDED
	int                           m_bpDebuggerId;  // DBG_UR_BP_ADDED, DBG_UR_BP_HIT
	TreeNode<wxString, NodeData> *m_tree;          // DBG_UR_LOCALS
	wxString                      m_expression;    // DBG_UR_EXPRESSION, DBG_UR_TYPE_RESOLVED, DBG_UR_TIP, DBG_UR_WATCHMEMORY, DBG_UR_VARIABLEOBJ
												   // DBG_UR_EVALVARIABLEOBJ
	wxString                      m_evaluated;     // DBG_UR_EXPRESSION, DBG_UR_TYPE_RESOLVED, DBG_UR_WATCHMEMORY, DBG_UR_EVALVARIABLEOBJ
	StackEntryArray               m_stack;         // DBG_UR_UPDATE_STACK_LIST
	std::vector<BreakpointInfo>   m_bpInfoList;    // DBG_UR_RECONCILE_BPTS
	bool                          m_onlyIfLogging; // DBG_UR_ADD_LINE
	ThreadEntryArray              m_threads;       // DBG_UR_LISTTHRAEDS
	VariableObjChildren           m_varObjChildren;// DBG_UR_LISTCHILDREN
	VariableObject                m_variableObject;// DBG_UR_VARIABLEOBJ

	DebuggerEvent()
		: m_updateReason  (DBG_UR_INVALID)
		, m_controlReason (DBG_UNKNOWN   )
		, m_file          (wxEmptyString )
		, m_line          (wxNOT_FOUND   )
		, m_text          (wxEmptyString )
		, m_bpInternalId  (wxNOT_FOUND   )
		, m_bpDebuggerId  (wxNOT_FOUND   )
		, m_tree          (NULL          )
		, m_expression    (wxEmptyString )
		, m_evaluated     (wxEmptyString )
		, m_onlyIfLogging (false         )
	{
		m_stack.clear();
		m_bpInfoList.clear();
		m_threads.clear();
		m_varObjChildren.clear();
	}
};

/**
 * \brief Defines the observer interface for classes who whishes to receive notifications from the debugger.
 * To set your self as the observer, one should call: IDebugger::SetObserver() method
 * \date 09-17-2007
 * \author Eran
 */
class IDebuggerObserver
{
public:
	IDebuggerObserver(){};
	virtual ~IDebuggerObserver(){};

	/**
	 * @brief the reporting method of the debugger. Must be implemented by any 'DebuggerObserver'
	 * @param event struct containing the update reason along with additional information per update type
	 */
	virtual void DebuggerUpdate( const DebuggerEvent &event ) = 0;

public:
	// For convinience
	/**
	 * @brief this function is called when the debugger plugin got the control back from the debugger
	 * @param reason the reason why the debugger gave the control to the plugin.
	 * @sa DebuggerReasons
	 */
	void UpdateGotControl(DebuggerReasons reason) {
		DebuggerEvent e;
		e.m_updateReason  = DBG_UR_GOT_CONTROL;
		e.m_controlReason = reason;
		DebuggerUpdate( e );
	}

	/**
	 * @brief this function is called when the debugger plugin loses the control.
	 */
	void UpdateLostControl() {
		DebuggerEvent e;
		e.m_updateReason  = DBG_UR_LOST_CONTROL;
		DebuggerUpdate( e );
	}

	/**
	 * @brief update the view to file and line number
	 * @param file full path to the current file
	 * @param lineno the line number
	 */
	void UpdateFileLine(const wxString &file, int lineno){
		DebuggerEvent e;
		e.m_updateReason  = DBG_UR_FILE_LINE;
		e.m_file = file;
		e.m_line = lineno;
		DebuggerUpdate( e );
	}

	/**
	 * @brief tells the observer to add line to the log view
	 * @param line message to log
	 */
	void UpdateAddLine(const wxString &line, const bool OnlyIfLoggingOn = false) {
		DebuggerEvent e;
		e.m_updateReason  = DBG_UR_ADD_LINE;
		e.m_text = line;
		e.m_onlyIfLogging = OnlyIfLoggingOn;
		DebuggerUpdate( e );
	}

	/**
	 * @brief notify the caller of the added breakpoint's debugger_id
	 * @param the breakpoint's ids: internal and debugger
	 */
	void UpdateBpAdded(const int internal_id, const int debugger_id) {
		DebuggerEvent e;
		e.m_updateReason = DBG_UR_BP_ADDED;
		e.m_bpInternalId = internal_id;
		e.m_bpDebuggerId = debugger_id;
		DebuggerUpdate( e );
	}

	/**
	 * @brief notify that the debugger is stopped (not used)
	 */
	void UpdateStopped() {
		DebuggerEvent e;
		e.m_updateReason = DBG_UR_STOPPED;
		DebuggerUpdate( e );
	}

	/**
	 * @brief update the locals view
	 * @param tree tree data strcuture which represents the local variables
	 * @sa TreeNode
	 */
	void UpdateLocals(TreeNode<wxString, NodeData> *tree) {
		DebuggerEvent e;
		e.m_updateReason = DBG_UR_LOCALS;
		e.m_tree = tree;
		DebuggerUpdate( e );
	}

	/**
	 * @brief an expression has been evaludated
	 * @param expression the expression that the debugger was requested to evaluate
	 * @param evaluated evaluated expression as string
	 */
	void UpdateExpression(const wxString &expression, const wxString &evaluated) {
		DebuggerEvent e;
		e.m_updateReason = DBG_UR_EXPRESSION;
		e.m_expression = expression;
		e.m_evaluated= evaluated;
		DebuggerUpdate( e );
	}

	/**
	 * @brief update the call stack
	 * @param stackArray an array of StackEntry items
	 */
	void UpdateStackList(const StackEntryArray &stackArray) {
		DebuggerEvent e;
		e.m_updateReason = DBG_UR_UPDATE_STACK_LIST;
		e.m_stack = stackArray;
		DebuggerUpdate( e );
	}

	/**
	 * @brief debugger connected to the remote target sucessfully
	 * @param line debugger output
	 */
	void UpdateRemoteTargetConnected(const wxString &line) {
		DebuggerEvent e;
		e.m_updateReason = DBG_UR_REMOTE_TARGET_CONNECTED;
		e.m_text = line;
		DebuggerUpdate( e );
	}

	/**
	 * @brief Update the breakpoints-manager's info with what the debugger really contains
	 * @param vector of breakpoints acquired from -break-list
	 */
	void ReconcileBreakpoints(std::vector<BreakpointInfo>& li) {
		DebuggerEvent e;
		e.m_updateReason = DBG_UR_RECONCILE_BPTS;
		e.m_bpInfoList = li;
		DebuggerUpdate( e );
	}

	/**
	 * @brief Tells the breakpoints-manager which breakpoint was just hit
	 * @param The breakpoint's ID
	 */
	void UpdateBpHit(int id) {
		DebuggerEvent e;
		e.m_updateReason = DBG_UR_BP_HIT;
		e.m_bpDebuggerId = id;
		DebuggerUpdate( e );
	}

	/**
	 * @brief updates the observer that a request to resolve 'expression' is completed
	 * and the type can be examined in 'type'
	 * @param expression
	 * @param type
	 */
	void UpdateTypeReolsved(const wxString &expression, const wxString &type) {
		DebuggerEvent e;
		e.m_updateReason = DBG_UR_TYPE_RESOLVED;
		e.m_expression = expression;
		e.m_evaluated = type;
		DebuggerUpdate( e );
	}

	/**
	 * @brief update the observer with tip for expression
	 * @param expression
	 * @param tip
	 */
	void UpdateTip (const wxString &expression, const wxString &tip) {
		DebuggerEvent e;
		e.m_updateReason = DBG_UR_TIP;
		e.m_expression = expression;
		e.m_text = tip;
		DebuggerUpdate( e );
	}
};

#endif //DEBUGGER_OBSERVER_H
