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

#include "RunInTerminalHelper.hpp"
#include "asyncprocess.h"
#include "clDapSettingsStore.hpp"
#include "clModuleLogger.hpp"
#include "cl_command_event.h"
#include "dap/Client.hpp"
#include "plugin.h"
#include "ssh_account_info.h"

#include <vector>
#include <wx/stc/stc.h>

class DAPMainView;
class IProcess;

struct DebugSession {
    std::vector<wxString> command;
    wxString working_directory;
    clEnvList_t environment;
    bool need_to_set_breakpoints = false;
    bool debug_over_ssh = false;
    SSHAccountInfo ssh_acount;
    DapEntry dap_server;

    void Clear()
    {
        need_to_set_breakpoints = false;
        working_directory.clear();
        debug_over_ssh = false;
        ssh_acount = {};
        command.clear();
        environment.clear();
        dap_server = {};
    }
};

class DebugAdapterClient : public IPlugin
{
    dap::Client m_client;
    clModuleLogger LOG;
    wxString m_defaultPerspective;
    DebugSession m_session;
    clDapSettingsStore m_dap_store;
    IProcess* m_dap_server = nullptr;
    RunInTerminalHelper m_terminal_helper;

    /// ------------------------------------
    /// UI elements
    /// ------------------------------------
    DAPMainView* m_threadsView = nullptr;
    bool m_raisOnBpHit;
    bool m_isPerspectiveLoaded;
    bool m_showThreadNames;
    bool m_showFileNamesOnly;

    friend class LLDBTooltip;

private:
    void DestroyUI();
    void InitializeUI();
    void LoadPerspective();
    void ShowPane(const wxString& paneName, bool show);
    void ClearDebuggerMarker();
    void SetDebuggerMarker(wxStyledTextCtrl* stc, int lineno);
    void DoCleanup();
    void StartAndConnectToDapServer(const DapEntry& dap_server, const wxString& exepath, const wxString& args,
                                    const wxString& working_directory, const clEnvList_t& env);
    bool LaunchProcess(const DapEntry& dap_entry);

    /// stop the dap server if it is running and send DEBUG_STOP event
    void StopProcess();

    /// loads all the breakpoints from the store for a given file and
    /// pass them to the debugger. If the debugger, is not running, do
    /// nothing
    void ApplyBreakpoints(const wxString& path);
    void RestoreUI();

    /// Place breakpoint markers for a given editor
    void RefreshBreakpointsMarkersForEditor(IEditor* editor);
    wxString NormalisePath(const wxString& path) const;
    void RegisterDebuggers();
    /**
     * @brief return true of the debugger identified by "name"
     * is owned by this plugin
     */
    bool IsDebuggerOwnedByPlugin(const wxString& name) const;

public:
    DebugAdapterClient(IManager* manager);
    ~DebugAdapterClient();

    IManager* GetManager() { return m_mgr; }

    /**
     * @brief Should thread name column be shown in thread pane?
     */
    bool ShowThreadNames() const;

    /**
     * @brief Maybe convert a path to filename only for display.
     */
    wxString GetFilenameForDisplay(const wxString& fileName) const;

protected:
    // Other codelite events
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnInitDone(wxCommandEvent& event);
    void OnFileLoaded(clCommandEvent& event);

    // UI debugger events
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
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);

    // DAP events
    void OnDapExited(DAPEvent& event);
    void OnLaunchResponse(DAPEvent& event);
    void OnInitializedEvent(DAPEvent& event);
    void OnInitializeResponse(DAPEvent& event);
    void OnStoppedEvent(DAPEvent& event);
    void OnThreadsResponse(DAPEvent& event);
    void OnStackTraceResponse(DAPEvent& event);
    void OnSetFunctionBreakpointResponse(DAPEvent& event);
    void OnSetSourceBreakpointResponse(DAPEvent& event);
    void OnRunInTerminal(DAPEvent& event);
    void OnDapLog(DAPEvent& event);

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
