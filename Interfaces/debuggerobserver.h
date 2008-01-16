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
	DBG_RECV_SIGNAL,
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
 * \ingroup Interfaces
 * Defines the observer interface for classes who whishes to 
 * observer the debugger behavior
 * \version 1.0
 * first version
 *
 * \date 09-17-2007
 *
 * \author Eran
 *
 *
 */
class IDebuggerObserver
{
public:
	IDebuggerObserver(){};
	virtual ~IDebuggerObserver(){};

	virtual void UpdateGotControl(DebuggerReasons reason) = 0;
	virtual void UpdateLostControl() = 0;
	virtual void UpdateFileLine(const wxString &file, int lineno) = 0;
	virtual void UpdateAddLine(const wxString &line) = 0;
	virtual void UpdateBpAdded() = 0;
	virtual void UpdateStopped() = 0;
	virtual void UpdateLocals(TreeNode<wxString, NodeData> *tree) = 0;
	virtual void UpdateExpression(const wxString &expression, const wxString &evaluated) = 0;
	virtual void UpdateQuickWatch(const wxString &expression, TreeNode<wxString, NodeData> *tree) = 0;
	virtual void UpdateStackList(const StackEntryArray &stackArray) = 0;
};

#endif //DEBUGGER_OBSERVER_H
