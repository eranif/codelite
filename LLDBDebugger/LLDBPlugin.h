//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBPlugin.h
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

#ifndef __LLDBDebugger__
#define __LLDBDebugger__

#include "plugin.h"
#include "cl_command_event.h"
#include <wx/stc/stc.h>

#include "LLDBProtocol/LLDBConnector.h"
#include "LLDBProtocol/LLDBEvent.h"
#include "LLDBProtocol/LLDBRemoteConnectReturnObject.h"
#include "clDebuggerTerminal.h"

class LLDBTooltip;
class LLDBThreadsView;
class LLDBLocalsView;
class LLDBOutputView;
class LLDBCallStackPane;
class LLDBPlugin : public IPlugin
{
    LLDBConnector m_connector;
    wxString m_defaultPerspective;

    /// ------------------------------------
    /// UI elements
    /// ------------------------------------
    LLDBCallStackPane* m_callstack;
    LLDBOutputView* m_breakpointsView;
    LLDBLocalsView* m_localsView;
    LLDBThreadsView* m_threadsView;
    clDebuggerTerminalPOSIX m_debuggerTerminal;
    bool m_stopReasonPrompted;
    bool m_raisOnBpHit;
    LLDBTooltip* m_tooltip;
    bool m_isPerspectiveLoaded;
    bool m_showThreadNames;
    bool m_showFileNamesOnly;

    friend class LLDBTooltip;

public:
    LLDBPlugin(IManager* manager);
    ~LLDBPlugin();

    LLDBConnector* GetLLDB() { return &m_connector; }

    IManager* GetManager() { return m_mgr; }

    /**
     * @brief Should thread name column be shown in thread pane?
     */
    bool ShowThreadNames() const;

    /**
     * @brief Maybe convert a path to filename only for display.
     */
    wxString GetFilenameForDisplay(const wxString& fileName) const;

private:
    void TerminateTerminal();
    void SetupPivotFolder(const LLDBConnectReturnObject& ret);
    void ClearDebuggerMarker();
    void SetDebuggerMarker(wxStyledTextCtrl* stc, int lineno);
    void LoadLLDBPerspective();
    void ShowLLDBPane(const wxString& paneName, bool show = true);
    void InitializeUI();
    void DestroyUI();
    void DoCleanup();
    bool DoInitializeDebugger(clDebugEvent& event, bool redirectOutput, const wxString& terminalTitle);
    void DestroyTooltip();

protected:
    // Other codelite events
    void OnWorkspaceLoaded(wxCommandEvent& event);
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnInitDone(wxCommandEvent& event);

    // UI debugger events
    void OnIsDebugger(clDebugEvent& event);
    void OnDebugStart(clDebugEvent& event);
    void OnDebugContinue(clDebugEvent& event);
    void OnDebugNext(clDebugEvent& event);
    void OnDebugStop(clDebugEvent& event);
    void OnDebugIsRunning(clDebugEvent& event);
    void OnDebugCanInteract(clDebugEvent& event);
    void OnDebugStepIn(clDebugEvent& event);
    void OnDebugStepOut(clDebugEvent& event);
    void OnToggleBreakpoint(clDebugEvent& event);
    void OnToggleInterrupt(clDebugEvent& event);
    void OnDebugTooltip(clDebugEvent& event);
    void OnDebugQuickDebug(clDebugEvent& event);
    void OnDebugCoreFile(clDebugEvent& event);
    void OnDebugAttachToProcess(clDebugEvent& event);
    void OnDebugDeleteAllBreakpoints(clDebugEvent& event);
    void OnDebugEnableAllBreakpoints(clDebugEvent& event);
    void OnDebugDisableAllBreakpoints(clDebugEvent& event);
    void OnDebugNextInst(clDebugEvent& event);
    void OnDebugVOID(clDebugEvent& event);
    void OnDebugShowCursor(clDebugEvent& event);

    void OnBuildStarting(clBuildEvent& event);
    void OnRunToCursor(wxCommandEvent& event);
    void OnJumpToCursor(wxCommandEvent& event);

    void OnAddWatch(wxCommandEvent& event);

    // LLDB events
    void OnLLDBCrashed(LLDBEvent& event);
    void OnLLDBStarted(LLDBEvent& event);
    void OnLLDBExited(LLDBEvent& event);
    void OnLLDBStopped(LLDBEvent& event);
    void OnLLDBStoppedOnEntry(LLDBEvent& event);
    void OnLLDBRunning(LLDBEvent& event);
    void OnLLDBDeletedAllBreakpoints(LLDBEvent& event);
    void OnLLDBBreakpointsUpdated(LLDBEvent& event);
    void OnLLDBExpressionEvaluated(LLDBEvent& event);
    void OnLLDBLaunchSuccess(LLDBEvent& event);
    void OnDestroyTip(clCommandEvent& e);

public:
    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();
};

#endif // LLDBDebuggerPlugin
