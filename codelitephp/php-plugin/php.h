//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : php.h
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

#ifndef __PHP__
#define __PHP__

#include "XDebugManager.h"
#include "php_event.h"
#include "plugin.h"
#include "plugin_settings.h"
#include <cl_command_event.h>
#include <wx/filename.h>
#include <wx/sharedptr.h>
#include "PhpSFTPHandler.h"

class EvalPane;
class LocalsView;
class PHPDebugPane;
class BrowserPanel;
class PHPWorkspaceView;
class wxMenuBar;

class PhpPlugin : public IPlugin
{
protected:
    PHPWorkspaceView* m_workspaceView;
    bool m_clangOldFlag;
    BrowserPanel* m_browser;
    wxString m_savedPerspective;

    /// Debugger panes
    PHPDebugPane* m_debuggerPane;
    LocalsView* m_xdebugLocalsView;
    EvalPane* m_xdebugEvalPane;
    bool m_showWelcomePage;
    bool m_toggleToolbar;
#if USE_SFTP
    PhpSFTPHandler::Ptr_t m_sftpHandler;
#endif //USE_SFTP

public:
    enum {
        wxID_PHP_SETTINGS = 2000,
        wxID_XDEBUG_SETTING,
        wxID_XDEBUG_DELETE_ALL_BREAKPOINTS,
        wxID_XDEBUG_SHOW_BREAKPOINTS_WINDOW,
        wxID_PHP_RUN_XDEBUG_DIAGNOSTICS,
    };

public:
    PhpPlugin(IManager* manager);
    ~PhpPlugin();
    void SafelyDetachAndDestroyPane(wxWindow* pane, const wxString& name);
    void EnsureAuiPaneIsVisible(const wxString& paneName, bool update = false);
    void FinalizeStartup();

    PHPDebugPane* GetDebuggerPane() { return m_debuggerPane; }

protected:
    bool IsWorkspaceViewDetached();
    void DoOpenWorkspace(const wxString& filename, bool createIfMissing = false, bool createProjectFromSources = false);
    void DoPlaceMenuBar(wxMenuBar* menuBar);
    void DoEnsureXDebugPanesVisible(const wxString& selectWindow = "");

public:
    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual clToolBar* CreateToolBar(wxWindow* parent);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnHookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();
    void RunXDebugDiagnostics();

    IManager* GetManager() { return m_mgr; }
    // Event handlers

    void SetEditorActive(IEditor* editor);

    //////////////////////////////////////////////
    // Code completion related events
    //////////////////////////////////////////////
    void OnShowQuickOutline(clCodeCompletionEvent& e);

    //////////////////////////////////////////////
    // Other common codelite events
    //////////////////////////////////////////////
    void OnNewWorkspace(clCommandEvent& e);
    void OnIsWorkspaceOpen(clCommandEvent& e);
    void OnCloseWorkspace(clCommandEvent& e);
    void OnOpenWorkspace(clCommandEvent& e);
    void OnReloadWorkspace(clCommandEvent& e);
    void OnOpenResource(wxCommandEvent& e);
    void OnGetWorkspaceFiles(wxCommandEvent& e);
    void OnGetCurrentFileProjectFiles(wxCommandEvent& e);
    void OnGetActiveProjectFiles(wxCommandEvent& e);
    void OnNewProject(clNewProjectEvent& e);
    void OnNewProjectFinish(clNewProjectEvent& e);
    void OnFindInFilesDismissed(clCommandEvent& e);
    void OnRunXDebugDiagnostics(wxCommandEvent& e);
    void OnMenuCommand(wxCommandEvent& e);
    void OnXDebugShowBreakpointsWindow(wxCommandEvent& e);
    void OnXDebugDeleteAllBreakpoints(clDebugEvent& e);
    void OnXDebugSettings(wxCommandEvent& e);
    void OnLoadURL(PHPEvent& e);
    void OnAllEditorsClosed(wxCommandEvent& e);
    void OnGoingDown(clCommandEvent& event);
    void OnDebugStarted(XDebugEvent& e);
    void OnDebugEnded(XDebugEvent& e);
    void OnFileSysetmUpdated(clFileSystemEvent& event);
    void OnSaveSession(clCommandEvent& event);
};

#endif // PHP
