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

#include "asyncprocess.h"
#include "clBreakpointsStore.hpp"
#include "clModuleLogger.hpp"
#include "cl_command_event.h"
#include "dap/Client.hpp"
#include "plugin.h"

#include <wx/stc/stc.h>

class DAPMainView;
class DebugAdapterClient : public IPlugin
{
    dap::Client m_client;
    wxString m_defaultPerspective;
    clModuleLogger LOG;
    wxFileName m_breakpoints_file;
    bool m_need_to_set_breakpoints = false;

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
    void StartAndConnectToDapServer(const wxString& exepath, const wxString& args, const wxString& working_directory,
                                    const clEnvList_t& env);
    /// loads all the breakpoints from the store for a given file and
    /// pass them to the debugger. If the debugger, is not running, do
    /// nothing
    void ApplyBreakpoints(const wxString& path);

    /// Place breakpoint markers for a given editor
    void RefreshBreakpointsMarkersForEditor(IEditor* editor);

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

    // DAP events
    void OnDapExited(DAPEvent& event);
    void OnLaunchResponse(DAPEvent& event);
    void OnInitializedEvent(DAPEvent& event);
    void OnStoppedEvent(DAPEvent& event);
    void OnThreadsResponse(DAPEvent& event);
    void OnStackTraceResponse(DAPEvent& event);
    void OnSetFunctionBreakpointResponse(DAPEvent& event);
    void OnSetSourceBreakpointResponse(DAPEvent& event);

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
