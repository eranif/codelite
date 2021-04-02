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

#include "debugger.h"
#include "tree_node.h"
#include "wx/treectrl.h"
#include <wx/sharedptr.h>

struct NodeData {
    wxString name;
    wxTreeItemId itemId;
};

enum DebuggerReasons {
    DBG_BP_HIT,
    DBG_BP_ASSERTION_HIT,
    DBG_RECV_SIGNAL,
    DBG_RECV_SIGNAL_SIGABRT,
    DBG_RECV_SIGNAL_SIGTRAP,
    DBG_RECV_SIGNAL_EXC_BAD_ACCESS,
    DBG_RECV_SIGNAL_SIGSEGV,
    DBG_RECV_SIGNAL_SIGPIPE,
    DBG_END_STEPPING,
    DBG_EXITED_NORMALLY,
    DBG_EXIT_WITH_ERROR,
    DBG_DBGR_KILLED,
    DBG_CMD_ERROR,
    DBG_FUNC_FINISHED,
    DBG_UNKNOWN
};

enum DebuggerUpdateReason {
    DBG_UR_INVALID = -1,            // Invalid
    DBG_UR_GOT_CONTROL,             // Application gains the control back from the debugger
    DBG_UR_LOST_CONTROL,            // Appliecation lost control to the debugge, and it can not interact (atm) with it
    DBG_UR_ADD_LINE,                // Log line
    DBG_UR_BP_ADDED,                // Breakpoint was added
    DBG_UR_STOPPED,                 // Debugger stopped
    DBG_UR_LOCALS,                  // Local variables are available
    DBG_UR_FUNC_ARGS,               // Function arguments are available
    DBG_UR_EXPRESSION,              // A requested expression evaluation has completed and is available
    DBG_UR_REMOTE_TARGET_CONNECTED, // Remove target is now connected
    DBG_UR_RECONCILE_BPTS,          // Reconcile breakpoints is needed
    DBG_UR_BP_HIT,                  // Breakpoint was hit
    DBG_UR_TYPE_RESOLVED,           // The debugger has evaluated the a type
    DBG_UR_ASCII_VIEWER,            // Tip is available
    DBG_UR_WATCHMEMORY,             // Watch memory is available
    DBG_UR_LISTTHRAEDS,             // Threads list is available
    DBG_UR_LISTCHILDREN,            // Children list for a variable object is available
    DBG_UR_VARIABLEOBJ,             // Variable object was created
    DBG_UR_VARIABLEOBJCREATEERR,    // Variable object create error
    DBG_UR_EVALVARIABLEOBJ,         // Variable object has be evaluated
    DBG_UR_VAROBJUPDATE,            // An update to variable object
    DBG_UR_FRAMEDEPTH,              // Frame information
    DBG_UR_VARIABLEOBJUPDATEERR,    // Variable object update error
    DBG_UR_FUNCTIONFINISHED, // Function execution finished, there might be a return value to display in the Locals view
    DBG_UR_DEBUGGER_PID_VALID // The debugger's pid is now known, so it's possible e.g. to interrupt it. Used for
                              // disabling bps
};

enum UserReason { DBG_USERR_QUICKWACTH = 0, DBG_USERR_WATCHTABLE, DBG_USERR_LOCALS, DBG_USERR_LOCALS_INLINE };

class DebuggerEventData : public wxClientData
{
public:
    DebuggerUpdateReason m_updateReason; // Event reason - the reason why this event was sent
    // ==================================================
    // Available when the following UpdateReason are set:
    // ==================================================
    DebuggerReasons m_controlReason; // DBG_UR_GOT_CONTROL
    wxString m_file;                 //
    int m_line;                      //
    wxString m_text;                 // DBG_UR_ADD_LINE, DBG_UR_REMOTE_TARGET_CONNECTED, DBG_UR_ASCII_VIEWER
    int m_bpInternalId;              // DBG_UR_BP_ADDED
    int m_bpDebuggerId;              // DBG_UR_BP_ADDED, DBG_UR_BP_HIT
    LocalVariables m_locals;         // DBG_UR_LOCALS
    wxString m_expression; // DBG_UR_EXPRESSION, DBG_UR_TYPE_RESOLVED, DBG_UR_ASCII_VIEWER, DBG_UR_WATCHMEMORY,
                           // DBG_UR_VARIABLEOBJ
    // DBG_UR_EVALVARIABLEOBJ, DBG_UR_FUNCTIONFINISHED
    wxString m_evaluated;    // DBG_UR_EXPRESSION, DBG_UR_TYPE_RESOLVED, DBG_UR_WATCHMEMORY, DBG_UR_EVALVARIABLEOBJ
    StackEntryArray m_stack; // DBG_UR_UPDATE_STACK_LIST
    std::vector<clDebuggerBreakpoint> m_bpInfoList; // DBG_UR_RECONCILE_BPTS
    bool m_onlyIfLogging;                           // DBG_UR_ADD_LINE
    ThreadEntryArray m_threads;                     // DBG_UR_LISTTHRAEDS
    VariableObjChildren m_varObjChildren;           // DBG_UR_LISTCHILDREN
    VariableObject m_variableObject;                // DBG_UR_VARIABLEOBJ
    int m_userReason;       // User reason as provided in the calling API which triggered the DebuggerUpdate call
    StackEntry m_frameInfo; // DBG_UR_FRAMEINFO
    VariableObjectUpdateInfo m_varObjUpdateInfo; // DBG_UR_VAROBJUPDATE
    DisassembleEntryVec_t m_disassembleLines;    // None
    DbgRegistersVec_t m_registers;               // Sent with event wxEVT_DEBUGGER_LIST_REGISTERS
    DebuggerEventData()
        : m_updateReason(DBG_UR_INVALID)
        , m_controlReason(DBG_UNKNOWN)
        , m_file(wxEmptyString)
        , m_line(wxNOT_FOUND)
        , m_text(wxEmptyString)
        , m_bpInternalId(wxNOT_FOUND)
        , m_bpDebuggerId(wxNOT_FOUND)
        , m_expression(wxEmptyString)
        , m_evaluated(wxEmptyString)
        , m_onlyIfLogging(false)
        , m_userReason(wxNOT_FOUND)
    {
        m_stack.clear();
        m_bpInfoList.clear();
        m_threads.clear();
        m_varObjChildren.clear();
        m_registers.clear();
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
    virtual void DebuggerUpdate(const DebuggerEventData& event) = 0;

public:
    // For convinience
    /**
     * @brief this function is called when the debugger plugin got the control back from the debugger
     * @param reason the reason why the debugger gave the control to the plugin.
     * @sa DebuggerReasons
     */
    void UpdateGotControl(DebuggerReasons reason, const wxString& func = wxEmptyString)
    {
        DebuggerEventData e;
        e.m_updateReason = DBG_UR_GOT_CONTROL;
        e.m_controlReason = reason;
        e.m_frameInfo.function = func;
        DebuggerUpdate(e);
    }

    /**
     * @brief this function is called when the debugger plugin loses the control.
     */
    void UpdateLostControl()
    {
        DebuggerEventData e;
        e.m_updateReason = DBG_UR_LOST_CONTROL;
        DebuggerUpdate(e);
    }

    /**
     * @brief tells the observer to add line to the log view
     * @param line message to log
     */
    void UpdateAddLine(const wxString& line, const bool OnlyIfLoggingOn = false)
    {
        DebuggerEventData e;
        e.m_updateReason = DBG_UR_ADD_LINE;
        e.m_text = line;
        e.m_onlyIfLogging = OnlyIfLoggingOn;
        DebuggerUpdate(e);
    }

    /**
     * @brief notify the caller of the added breakpoint's debugger_id
     * @param the breakpoint's ids: internal and debugger
     */
    void UpdateBpAdded(const int internal_id, const int debugger_id)
    {
        DebuggerEventData e;
        e.m_updateReason = DBG_UR_BP_ADDED;
        e.m_bpInternalId = internal_id;
        e.m_bpDebuggerId = debugger_id;
        DebuggerUpdate(e);
    }

    /**
     * @brief notify that the debugger is stopped (not used)
     */
    void UpdateStopped()
    {
        DebuggerEventData e;
        e.m_updateReason = DBG_UR_STOPPED;
        DebuggerUpdate(e);
    }

    /**
     * @brief update the locals view
     * @param array containing the local variable
     */
    void UpdateLocals(const LocalVariables& locals)
    {
        DebuggerEventData e;
        e.m_updateReason = DBG_UR_LOCALS;
        e.m_userReason = DBG_USERR_LOCALS;
        e.m_locals = locals;
        DebuggerUpdate(e);
    }

    /**
     * @brief update the locals view
     * @param array containing the function arguments
     */
    void UpdateFunctionArguments(const LocalVariables& args)
    {
        DebuggerEventData e;
        e.m_updateReason = DBG_UR_FUNC_ARGS;
        e.m_userReason = DBG_USERR_LOCALS;
        e.m_locals = args;
        DebuggerUpdate(e);
    }

    /**
     * @brief an expression has been evaludated
     * @param expression the expression that the debugger was requested to evaluate
     * @param evaluated evaluated expression as string
     */
    void UpdateExpression(const wxString& expression, const wxString& evaluated)
    {
        DebuggerEventData e;
        e.m_updateReason = DBG_UR_EXPRESSION;
        e.m_expression = expression;
        e.m_evaluated = evaluated;
        DebuggerUpdate(e);
    }

    /**
     * @brief debugger connected to the remote target sucessfully
     * @param line debugger output
     */
    void UpdateRemoteTargetConnected(const wxString& line)
    {
        DebuggerEventData e;
        e.m_updateReason = DBG_UR_REMOTE_TARGET_CONNECTED;
        e.m_text = line;
        DebuggerUpdate(e);
    }

    /**
     * @brief Tell the manager that the debugger is now interruptable
     */
    void DebuggerPidValid()
    {
        DebuggerEventData e;
        e.m_updateReason = DBG_UR_DEBUGGER_PID_VALID;
        DebuggerUpdate(e);
    }

    /**
     * @brief Tells the breakpoints-manager which breakpoint was just hit
     * @param The breakpoint's ID
     */
    void UpdateBpHit(int id)
    {
        DebuggerEventData e;
        e.m_updateReason = DBG_UR_BP_HIT;
        e.m_bpDebuggerId = id;
        DebuggerUpdate(e);
    }

    /**
     * @brief update the observer with tip for expression
     * @param expression
     * @param tip
     */
    void UpdateAsciiViewer(const wxString& expression, const wxString& tip)
    {
        DebuggerEventData e;
        e.m_updateReason = DBG_UR_ASCII_VIEWER;
        e.m_expression = expression;
        e.m_text = tip;
        DebuggerUpdate(e);
    }
};

#endif // DEBUGGER_OBSERVER_H
