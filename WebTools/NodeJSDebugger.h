//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSDebugger.h
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

#ifndef NODEJSDEBUGGER_H
#define NODEJSDEBUGGER_H

#include <wx/event.h>
#include "cl_command_event.h"
#include <wx/socket.h>
#include "NodeJSSocket.h"
#include "NodeJSDebuggerBreakpointManager.h"
#include "TerminalEmulator.h"
#include "NodeJS.h"

class NodeJSDebuggerTooltip;
struct NodeJSDebuggerException
{
    wxString message;
    wxString script;
    int line;
    int column;

    NodeJSDebuggerException()
        : line(wxNOT_FOUND)
        , column(wxNOT_FOUND)
    {
    }
};

class NodeJSDebugger : public wxEvtHandler
{
    NodeJSSocket::Ptr_t m_socket;
    TerminalEmulator m_node;
    NodeJSBptManager m_bptManager;
    bool m_canInteract;
    wxStringSet_t m_tempFiles;
    NodeJSDebuggerTooltip* m_tooltip;

public:
    typedef SmartPtr<NodeJSDebugger> Ptr_t;

protected:
    // Debugger event handlers
    void OnDebugStart(clDebugEvent& event);
    void OnDebugContinue(clDebugEvent& event);
    void OnStopDebugger(clDebugEvent& event);
    void OnDebugIsRunning(clDebugEvent& event);
    void OnToggleBreakpoint(clDebugEvent& event);
    void OnDebugNext(clDebugEvent& event);
    void OnVoid(clDebugEvent& event);
    void OnDebugStepIn(clDebugEvent& event);
    void OnDebugStepOut(clDebugEvent& event);
    void OnTooltip(clDebugEvent& event);
    void OnCanInteract(clDebugEvent& event);
    void OnAttach(clDebugEvent& event);

    // CodeLite events
    void OnWorkspaceOpened(wxCommandEvent& event);
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnEditorChanged(wxCommandEvent& event);

    // Process event
    void OnNodeTerminated(clCommandEvent& event);
    void OnNodeOutput(clCommandEvent& event);
    void OnHighlightLine(clDebugEvent& event);
    void OnEvalExpression(clDebugEvent& event);

    // The tip needs to be destroyed
    void OnDestroyTip(clCommandEvent& event);

protected:
    void DoHighlightLine(const wxString& filename, int lineNo);
    void DoDeleteTempFiles(const wxStringSet_t& files);

public:
    NodeJSDebugger();
    virtual ~NodeJSDebugger();
    bool IsConnected();
    void ShowTooltip(const wxString& expression, const wxString& jsonOutput);

    void AddTempFile(const wxString& filename) { m_tempFiles.insert(filename); }

    void SetDebuggerMarker(IEditor* editor, int lineno);
    void ClearDebuggerMarker();

    void SetCanInteract(bool canInteract);
    bool IsCanInteract() const { return m_canInteract; }

    //--------------------------------------------------
    // API
    //--------------------------------------------------
    /**
     * @brief start the debugger using the given command
     */
    void StartDebugger(const wxString& command, const wxString& workingDirectory);

    /**
     * @brief delete breakpoint from NodeJS. This function does not updat the breakpoint manager
     * nor it does not update the UI
     */
    void DeleteBreakpoint(const NodeJSBreakpoint& bp);

    /**
     * @brief set a breakpoint in nodejs debugger
     */
    void SetBreakpoint(const NodeJSBreakpoint& bp);

    /**
     * @brief set all breakpoints
     */
    void SetBreakpoints();

    /**
     * @brief continue execution
     */
    void Continue();

    /**
     * @brief break on exception
     */
    void BreakOnException(bool b = true);

    /**
     * @brief request for callstack
     */
    void Callstack();

    /**
     * @brief select new frame (usually by d-clicking a frame entry in the callstack view)
     */
    void SelectFrame(int frameId);

    /**
     * @brief load the content of a given file name
     */
    void GetCurrentFrameSource(const wxString& filename, int line);

    /**
     * @brief The request lookup is used to lookup objects based on their handle
     */
    void Lookup(const std::vector<int>& handles, eNodeJSContext context);

    //--------------------------------------------------
    // API END
    //--------------------------------------------------

    /**
     * @brief return the breakpoints manager
     */
    NodeJSBptManager* GetBreakpointsMgr() { return &m_bptManager; }

    /**
     * @brief nodejs debugger socket connected
     */
    void ConnectionEstablished();
    /**
     * @brief nodejs debugger socket disconnected
     */
    void ConnectionLost(const wxString& errmsg);

    /**
     * @brief could not connect to NodeJS
     */
    void ConnectError(const wxString& errmsg);

    /**
     * @brief this function is called by the various handlers whenever the CodeLite
     * gets the control from NodeJS
     */
    void GotControl(bool requestBacktrace);

    /**
     * @brief the execution in the VM has stopped due to an exception
     */
    void ExceptionThrown(const NodeJSDebuggerException& exc);
};

#endif // NODEJSDEBUGGER_H
