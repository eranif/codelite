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
	 * @brief this function is called when the debugger plugin got the control back from the debugger
	 * @param reason the reason why the debugger gave the control to the plugin.
	 * @sa DebuggerReasons
	 */
	virtual void UpdateGotControl(DebuggerReasons reason) = 0;

	/**
	 * @brief this function is called when the debugger plugin loses the control.
	 */
	virtual void UpdateLostControl() = 0;

	/**
	 * @brief update the view to file and line number
	 * @param file full path to the current file
	 * @param lineno the line number
	 */
	virtual void UpdateFileLine(const wxString &file, int lineno) = 0;

	/**
	 * @brief tells the observer to add line to the log view
	 * @param line message to log
	 */
	virtual void UpdateAddLine(const wxString &line) = 0;

	/**
	 * @brief notify the caller of the added breakpoint's debugger_id
	 * @param the breakpoint's ids: internal and debugger
	 */
	virtual void UpdateBpAdded(const int internal_id, const int debugger_id) = 0;

	/**
	 * @brief notify that the debugger is stopped (not used)
	 */
	virtual void UpdateStopped() = 0;

	/**
	 * @brief update the locals view
	 * @param tree tree data strcuture which represents the local variables
	 * @sa TreeNode
	 */
	virtual void UpdateLocals(TreeNode<wxString, NodeData> *tree) = 0;

	/**
	 * @brief an expression has been evaludated
	 * @param expression the expression that the debugger was requested to evaluate
	 * @param evaluated evaluated expression as string
	 */
	virtual void UpdateExpression(const wxString &expression, const wxString &evaluated) = 0;

	/**
	 * @brief update the quick watch view.
	 * @param expression expression that CodeLite requested to quick view
	 * @param tree a tree representing the evaluated expression
	 */
	virtual void UpdateQuickWatch(const wxString &expression, TreeNode<wxString, NodeData> *tree) = 0;

	/**
	 * @brief update the call stack
	 * @param stackArray an array of StackEntry items
	 */
	virtual void UpdateStackList(const StackEntryArray &stackArray) = 0;

	/**
	 * @brief debugger connected to the remote target sucessfully
	 * @param line debugger output
	 */
	virtual void UpdateRemoteTargetConnected(const wxString &line) = 0;
};

#endif //DEBUGGER_OBSERVER_H
