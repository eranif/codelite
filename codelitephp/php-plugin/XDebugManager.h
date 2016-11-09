//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : XDebugManager.h
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

#ifndef XDEBUGMANAGER_H
#define XDEBUGMANAGER_H

#include <wx/event.h>
#include <cl_command_event.h>
#include <wx/socket.h>
#include <wx/sharedptr.h>
#include "XDebugCommandHandler.h"
#include "xdebugbreakpointsmgr.h"
#include "php_event.h"
#include <macros.h>
#include "php_project.h"
#include <wx/msgqueue.h>

class XDebugComThread;
class IEditor;
class wxStyledTextCtrl;
class PhpPlugin;
class XDebugManager;

// ----------------------------------------------
// ----------------------------------------------
// ----------------------------------------------

struct xInitStruct {
    wxString filename;
};

class XDebugManager : public wxEvtHandler
{
    friend class SocketServer;
    size_t TranscationId;
    XDebugCommandHandler::Map_t m_handlers;
    XDebugBreakpointsMgr m_breakpointsMgr;
    PhpPlugin* m_plugin;
    XDebugComThread* m_readerThread;
    bool m_connected;

public:
    typedef wxSharedPtr<XDebugManager> Ptr_t;

public:
    int GetPort() const;
    wxString GetHost() const;
    void CenterEditor(wxStyledTextCtrl* ctrl, int lineNo);
    
    void StartListener();
    
    void SetConnected(bool connected);
    bool IsConnected() const { return m_connected; }
    void DoSocketWrite(const wxString& command);
    /**
     * @brief refresh the current stack view (Locals & Callstack)
     * @param requestedStack
     */
    void DoRefreshDebuggerViews(int requestedStack = 0);

    void CloseDebugSession();
    void SendStopCommand();
    void SendRunCommand();

    /**
     * @brief request XDebug to expand propertyName
     * @param propertyName
     */
    void SendGetProperty(const wxString& propertyName);

    /**
     * @brief send dbgp command to xdebug
     */
    void SendDBGPCommand(const wxString& cmd);
    /**
     * @brief send an 'Eval' command to XDebug
     */
    void SendEvalCommand(const wxString& expression, int evalPurpose);

    /**
     * @brief highlight the current line with the debugger marker
     * If the marker is already on an another line, remove it
     */
    void SetDebuggerMarker(wxStyledTextCtrl* stc, int lineno);
    /**
     * @brief clear the debugger marker from all the open editors
     */
    void ClearDebuggerMarker();

    /**
     * @brief return all the file mapping for a given project
     * the mapping will include the xdebug mapping + SFTP mapping
     */
    wxStringMap_t GetFileMapping(PHPProject::Ptr_t pProject) const;

protected:
    void DoStartDebugger(bool ideInitiate = true);
    void DoStopDebugger();
    void DoRefreshBreakpointsMarkersForEditor(IEditor* editor);
    void DoHandleResponse(wxXmlNode* xml);
    void DoContinue();
    void DoApplyBreakpoints();
    void DoNegotiateFeatures();
    void DoDeleteBreakpoint(int bpid);
    xInitStruct ParseInitXML(wxXmlNode* init);

    // Handlers based on the tx id
    void AddHandler(XDebugCommandHandler::Ptr_t handler);
    XDebugCommandHandler::Ptr_t PopHandler(int transcationId);

    // Event handlers
    /**
     * @brief user initiated a Start debug OR continue command from
     */
    void OnDebugStartOrContinue(clDebugEvent& e);
    /**
     * @brief user asked to stop the debugger
     */
    void OnStopDebugger(clDebugEvent& e);
    /**
     * @brief set e.SetAnswer to true if an active XDebug session is currently in progress
     */
    void OnDebugIsRunning(clDebugEvent& e);
    /**
     * @brief user placed a breakpoint (either by the keyboard shortcut or by clicking on the margin)
     */
    void OnToggleBreakpoint(clDebugEvent& e);
    /**
     * @brief user clicked on the "Next" tool
     */
    void OnDebugNext(clDebugEvent& e);
    /**
     * @brief a "void" function that swallows events
     */
    void OnVoid(clDebugEvent& e);
    /**
     * @brief step into function
     */
    void OnDebugStepIn(clDebugEvent& e);
    /**
     * @brief step out of a function
     */
    void OnDebugStepOut(clDebugEvent& e);
    /**
     * @brief a tooltip is requested
     */
    void OnTooltip(clDebugEvent& e);
    /**
     * @brief can we interact with the debugger?
     */
    void OnCanInteract(clDebugEvent& e);

    /**
     * @brief XDebug stopped and the control is now passed to codelite
     * this is a good time to update the callstack and any watches we have
     */
    void OnGotFocusFromXDebug(XDebugEvent& e);

    /**
     * @brief xdebug stopped the debug session (for whatever reason)
     */
    void OnXDebugStopped(XDebugEvent& e);

    /**
     * @brief respond to use d-click an entry in the call stack
     */
    void OnStackTraceItemActivated(PHPEvent& e);

    /**
     * @brief respond to use d-click an entry in the breakpoint table
     */
    void OnBreakpointItemActivated(PHPEvent& e);

    /**
     * @brief user clicked the 'delete all' button in the breakpoints view
     * @param e
     */
    void OnDeleteAllBreakpoints(PHPEvent& e);
    /**
     * @brief user clicked the 'delete' bp button
     */
    void OnDeleteBreakpoint(PHPEvent& e);

    /**
     * @brief user updated the breakpoint view
     */
    void OnBreakpointsViewUpdated(XDebugEvent& e);

    /**
     * @brief display debugger tooltip
     */
    void OnShowTooltip(XDebugEvent& e);

    bool ProcessDebuggerMessage(const wxString& buffer);

    XDebugManager();

public:
    virtual ~XDebugManager();
    static XDebugManager& Get();
    static void Free();
    static void Initialize(PhpPlugin* plugin);

    /**
     * @brief return true if an active debug session is running
     */
    bool IsDebugSessionRunning() const;

    PhpPlugin* GetPlugin() { return m_plugin; }
    XDebugBreakpointsMgr& GetBreakpointsMgr() { return m_breakpointsMgr; }

    const XDebugBreakpointsMgr& GetBreakpointsMgr() const { return m_breakpointsMgr; }

    /**
     * @brief we got something on the socket - read it
     */
    void OnSocketInput(const std::string& reply);

    /**
     * @brief will be called once XDebug does not connect after 5 seconds
     */
    void XDebugNotConnecting();

    /**
     * @brief called when the comm thread is done
     */
    void OnCommThreadTerminated();
};

#endif // XDEBUGMANAGER_H
