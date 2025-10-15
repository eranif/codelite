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
#pragma once

#include "AsyncProcess/asyncprocess.h"
#include "BreakpointsHelper.hpp"
#include "DAPDebuggerPane.h"
#include "DAPOutputPane.hpp"
#include "DAPTextView.h"
#include "DebugSession.hpp"
#include "RunInTerminalHelper.hpp"
#include "SessionBreakpoints.hpp"
#include "clDapSettingsStore.hpp"
#include "clModuleLogger.hpp"
#include "cl_command_event.h"
#include "dap/Client.hpp"
#include "dap/DAPEvent.hpp"
#include "plugin.h"
#include "ssh/ssh_account_info.h"

#include <vector>
#include <wx/msgqueue.h>
#include <wx/stc/stc.h>

class DAPMainView;
class DAPTooltip;
class DAPConsoleOutput;
class DAPModuleView;
class DAPBreakpointsView;
class IProcess;
class DAPWatchesView;

struct DapProcess {
    DapProcess(IProcess* process)
    {
        m_process.reset(process);
        if (m_process) {
            m_process->SetHardKill(true);
        }
    }

    bool IsOk() const { return m_process != nullptr; }

    void Terminate()
    {
        if (m_process) {
            m_process->Terminate();
        }
    }

    bool Write(const std::string& buffer)
    {
        if (m_process) {
            return m_process->WriteRaw(buffer);
        }
        return false;
    }

    bool IsRedirect() const { return m_process && m_process->IsRedirect(); }
    wxMessageQueue<std::string>& Queue() { return m_readQueue; }
    typedef std::shared_ptr<DapProcess> Ptr_t;

private:
    IProcess::Ptr_t m_process = nullptr;
    wxMessageQueue<std::string> m_readQueue;
};

class DebugAdapterClient : public IPlugin
{
    dap::Client m_client;
    wxString m_defaultPerspective;
    DebugSession m_session;
    clDapSettingsStore m_dap_store;
    RunInTerminalHelper m_terminal_helper;
    wxFileName m_breakpointsFile;
    BreakpointsHelper* m_breakpointsHelper = nullptr;
    SessionBreakpoints m_sessionBreakpoints;
    DapProcess::Ptr_t m_dap_server;

    /// ------------------------------------
    /// UI elements
    /// ------------------------------------
    DAPTextView* m_textView = nullptr;
    DAPTooltip* m_tooltip = nullptr;
    DAPDebuggerPane* m_debuggerPane = nullptr;
    bool m_raisOnBpHit = true;
    bool m_isPerspectiveLoaded = false;

    friend class LLDBTooltip;

private:
    wxString ReplacePlaceholders(const wxString& str) const;
    void UpdateWatches();
    void HideDebuggerUI();
    void InitializeUI();
    void LoadPerspective();
    void ShowPane(const wxString& paneName, bool show);
    void ClearDebuggerMarker();
    void DoCleanup();
    void StartAndConnectToDapServer();
    bool StartSocketDap();
    dap::Transport* StartStdioDap();
    void RefreshBreakpointsView();
    bool InitialiseSession(const DapEntry& dap_server, const wxString& exepath, const wxString& args,
                           const wxString& working_directory, const wxString& ssh_account, const clEnvList_t& env);

    clEnvList_t PrepareEnvForFileSystemWorkspace(const DapEntry& dap_server, bool resolve_vars);

    /// stop the dap server if it is running and send DEBUG_STOP event
    void StopProcess();
    void RestoreUI();

    wxString NormaliseReceivedPath(const wxString& path) const;
    void RegisterDebuggers();
    /**
     * @brief return true of the debugger identified by "name"
     * is owned by this plugin
     */
    bool IsDebuggerOwnedByPlugin(const wxString& name) const;

    void DestroyTooltip();
    DAPMainView* GetThreadsView() const { return m_debuggerPane->GetMainView(); }
    DAPBreakpointsView* GetBreakpointsView() const { return m_debuggerPane->GetBreakpointsView(); }
    DAPWatchesView* GetWatchesView() const { return m_debuggerPane->GetWatchesView(); }
    DAPOutputPane* GetOutputView() const { return m_debuggerPane->GetOutputView(); }

public:
    DebugAdapterClient(IManager* manager);
    ~DebugAdapterClient() override = default;

    IManager* GetManager() { return m_mgr; }

protected:
    // Other CodeLite events
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnInitDone(wxCommandEvent& event);
    void OnFileLoaded(clCommandEvent& event);
    void OnPageClosing(wxNotifyEvent& event);

    // UI debugger events
    void OnDebugStart(clDebugEvent& event);
    void OnDebugContinue(clDebugEvent& event);
    void OnDebugNext(clDebugEvent& event);
    void OnDebugStop(clDebugEvent& event);
    void OnDebugIsRunning(clDebugEvent& event);
    void OnDebugCanInteract(clDebugEvent& event);
    void OnDebugStepIn(clDebugEvent& event);
    void OnDebugStepOut(clDebugEvent& event);
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
    void OnDestroyTip(clCommandEvent& event);

    void OnAddWatch(wxCommandEvent& event);
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);
    void OnIdle(wxIdleEvent& event);

    // DAP events
    void OnDapExited(DAPEvent& event);
    void OnDapLaunchResponse(DAPEvent& event);
    void OnDapInitializedEvent(DAPEvent& event);
    void OnDapInitializeResponse(DAPEvent& event);
    void OnDapStoppedEvent(DAPEvent& event);
    void OnDapThreadsResponse(DAPEvent& event);
    void OnDapStackTraceResponse(DAPEvent& event);
    void OnDapSetFunctionBreakpointResponse(DAPEvent& event);
    void OnDapSetSourceBreakpointResponse(DAPEvent& event);
    void OnDapBreakpointEvent(DAPEvent& event);
    void OnDapRunInTerminal(DAPEvent& event);
    void OnDapLog(DAPEvent& event);
    void OnDapScopesResponse(DAPEvent& event);
    void OnDapVariablesResponse(DAPEvent& event);
    void OnDapOutputEvent(DAPEvent& event);
    void OnDapModuleEvent(DAPEvent& event);

public:
    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    void CreateToolBar(clToolBarGeneric* toolbar) override;
    void CreatePluginMenu(wxMenu* pluginsMenu) override;
    void HookPopupMenu(wxMenu* menu, MenuType type) override;
    void UnPlug() override;

    // expose these API calls to the views can use it
    dap::Client& GetClient() { return m_client; }
    int GetCurrentFrameId() const;
    void LoadFile(const dap::Source& sourceId, int line_number);
};
