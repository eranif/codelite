//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : frame.h
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
#ifndef LITEEDITOR_FRAME_H
#define LITEEDITOR_FRAME_H

#include "wx/combobox.h"
#include "generalinfo.h"
#include <wx/dcbuffer.h>
#include <wx/process.h>
#include "wx/aui/aui.h"
#include "wx/frame.h"
#include "cl_editor.h"
#include "output_pane.h"
#include "cl_process.h"
#include "EnvironmentVariablesDlg.h"
#include "wx/choice.h"
#include "wx/timer.h"
#include "parse_thread.h"
#include "tags_options_dlg.h"
#include <wx/html/htmlwin.h>
#include "debuggerpane.h"
#include "Notebook.h"
#include "mainbook.h"
#include <set>
#include "theme_handler.h"
#include "cl_command_event.h"
#include "ZombieReaperPOSIX.h"
#include "macros.h"
#include <wx/splash.h>
#include "clStatusBar.h"
#include <wx/cmndata.h>
#include "clDockingManager.h"
#include "clMainFrameHelper.h"

// forward decls
class WebUpdateJob;
class CodeLiteApp;
class clSingleInstanceThread;
class wxCustomStatusBar;
class TagEntry;
class WorkspacePane;
class wxToolBar;
class OpenWindowsPanel;
class WorkspaceTab;
class FileExplorer;
class OutputTabWindow;
class DockablePaneMenuManager;
class MyMenuBar;
class clSplashScreen;

//--------------------------------
// Helper class
//--------------------------------
extern const wxEventType wxEVT_LOAD_PERSPECTIVE;
extern const wxEventType wxEVT_REFRESH_PERSPECTIVE_MENU;
extern const wxEventType wxEVT_ACTIVATE_EDITOR;

struct StartPageData
{
    wxString name;
    wxString file_path;
    wxString action;
};

class clMainFrame : public wxFrame
{
    MainBook* m_mainBook;
    static clMainFrame* m_theFrame;
    clDockingManager m_mgr;
    OutputPane* m_outputPane;
    WorkspacePane* m_workspacePane;
    wxArrayString m_files;
    wxTimer* m_timer;
    std::map<int, wxString> m_viewAsMap;
    TagsOptionsData m_tagsOptionsData;
    DebuggerPane* m_debuggerPane;
    bool m_buildAndRun;
    GeneralInfo m_frameGeneralInfo;
    std::map<int, wxString> m_toolbars;
    std::map<int, wxString> m_panes;
    wxMenu* m_cppMenu;
    bool m_highlightWord;
    DockablePaneMenuManager* m_DPmenuMgr;
    wxPanel* m_mainPanel;
    wxString m_codeliteDownloadPageURL;
    wxString m_defaultLayout;
    bool m_workspaceRetagIsRequired;
    bool m_loadLastSession;
    wxSizer* m_horzSizer;
    MyMenuBar* m_myMenuBar;
    wxMenu* m_bookmarksDropDownMenu;
    ThemeHandler m_themeHandler;

#ifndef __WXMSW__
    ZombieReaperPOSIX m_zombieReaper;
#endif

#ifdef __WXGTK__
    bool m_isWaylandSession;
#endif

    // Maintain a set of core toolbars (i.e. toolbars not owned by any plugin)
    wxStringSet_t m_coreToolbars;
    clStatusBar* m_statusBar;
    clSingleInstanceThread* m_singleInstanceThread;
    bool m_toggleToolBar;

    // Printing
    wxPrintDialogData m_printDlgData;
    wxToolBar* m_mainToolBar;
    clMainFrameHelper::Ptr_t m_frameHelper;
    WebUpdateJob *m_webUpdate;
    
public:
    static bool m_initCompleted;

protected:
    bool IsEditorEvent(wxEvent& event);
    void DoCreateBuildDropDownMenu(wxMenu* menu);
    void DoShowToolbars(bool show, bool update = true);
    void InitializeLogo();

public:
    static clMainFrame* Get();
    static void Initialize(bool loadLastSession);

    /**
     * @brief goto anything..
     */
    void OnGotoAnything(wxCommandEvent& e);

    /**
     * @brief Return CodeLite App object
     */
    CodeLiteApp* GetTheApp();

    /**
     * @brief return the status bar
     */
    clStatusBar* GetStatusBar() { return m_statusBar; }

    /**
     * @brief update the parser (code completion) search paths using the
     * default compiler as the input provider
     */
    void UpdateParserSearchPathsFromDefaultCompiler();

    DockablePaneMenuManager* GetDockablePaneMenuManager() { return m_DPmenuMgr; }
    //--------------------- debuger---------------------------------
    /**
     * @brief launch TTY
     */
    wxString StartTTY(const wxString& title);

    //---------------------------------------------------------------

    /**
     * \brief update various AUI parameters like Output Pane View caption visible and other
     */
    void UpdateAUI();

    /**
     * @brief set the AUI manager flags as dictated from the configuration
     */
    void SetAUIManagerFlags();

    /**
     * @brief update the environment status message:
     * which displays to the user the current environment set used + the active builder
     */
    void SelectBestEnvSet();

    virtual ~clMainFrame(void);
    /**
     * @brief set frame option flag
     * @param set
     * @param flag
     */
    void SetFrameFlag(bool set, int flag);

    /**
     * @brief update the tags options data on the disc as well as
     * the parser thread
     * @param tod
     */
    void UpdateTagsOptions(const TagsOptionsData& tod);

    /**
     * @brief return the current tags options data
     * @return
     */
    TagsOptionsData& GetTagsOptions() { return m_tagsOptionsData; }

    /**
     * @brief return true if the word under the caret should be highlighted
     * @return
     */
    bool GetHighlightWord() { return m_highlightWord; }

    /**
     * @brief
     * @param editor
     */
    void SetFrameTitle(LEditor* editor);

    MainBook* GetMainBook() const { return m_mainBook; }

    /**
     * @return the output pane (the bottom pane)
     */
    OutputPane* GetOutputPane() { return m_outputPane; }

    /**
     * return the debugger pane
     * @return
     */
    DebuggerPane* GetDebuggerPane() { return m_debuggerPane; }

    /**
     * @return the workspace pane (the one that contained the Symbol view & class view)
     */
    WorkspacePane* GetWorkspacePane() { return m_workspacePane; }

    /**
     * return the workspace tab pane
     */
    WorkspaceTab* GetWorkspaceTab();

    /**
     * return the file explorer pane
     */
    FileExplorer* GetFileExplorer();

    /**
     * @return return AUI docking manager
     */
    wxAuiManager& GetDockingManager() { return m_mgr; }

    wxAuiManager* GetDockingManagerPtr() { return &m_mgr; }

    /**
     * Load session into LE
     */
    void LoadSession(const wxString& sessionName);

    /**
     * Compelete the main frame initialization
     * this method is called immediatly after the
     * main frame construction is over.
     */
    void CompleteInitialization();

    void RegisterToolbar(int menuItemId, const wxString& name);
    void RegisterDockWindow(int menuItemId, const wxString& name);

    const GeneralInfo& GetFrameGeneralInfo() const { return m_frameGeneralInfo; }

    void OnSingleInstanceOpenFiles(clCommandEvent& e);
    void OnSingleInstanceRaise(clCommandEvent& e);

    /**
     * @brief rebuild the give project
     * @param projectName
     */
    void RebuildProject(const wxString& projectName);

    /**
     * @brief display the welcome page
     */
    void ShowWelcomePage();

    /**
     * @brief handle custom build targets events
     */
    void OnBuildCustomTarget(wxCommandEvent& event);

    /**
     * @brief save the current IDE layout and session
     */
    bool SaveLayoutAndSession();

    /**
     * @brief save settings (such as whether to show the splashscreen) which should be saved
     * regardless of whether SaveLayoutAndSession is called
     */
    void SaveGeneralSettings();

    /**
     * @brief create the recently-opened-workspaces menu
     */
    void CreateRecentlyOpenedWorkspacesMenu();
    void DoSuggestRestart();

    void Bootstrap();

#ifdef __WXGTK__
    bool GetIsWaylandSession() const { return m_isWaylandSession; }
#endif

private:
    // make our frame's constructor private
    clMainFrame(wxWindow* pParent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                long style = wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION | wxSYSTEM_MENU |
                             wxRESIZE_BORDER | wxCLIP_CHILDREN);
    wxString CreateWorkspaceTable();
    wxString CreateFilesTable();
    void StartTimer();

private:
    /**
     * Construct all the GUI controls of the main frame. this function is called
     * at construction time
     */
    void CreateGUIControls();
    /**
     * \brief update the path & name of the build tool
     * on windows, try to locate make, followed by mingw32-make
     */
    void UpdateBuildTools();
    /**
     * Helper function that prompt user with a simple wxTextEntry dialog
     * @param msg message to display to user
     * @return user's string or wxEmptyString if 'Cancel' pressed.
     */
    void DispatchCommandEvent(wxCommandEvent& event);
    void DispatchUpdateUIEvent(wxUpdateUIEvent& event);

    void OnSplitSelection(wxCommandEvent& event);
    void OnSplitSelectionUI(wxUpdateUIEvent& event);

    /// Toolbar management
    void CreateToolbars24();
    void CreateToolbars16();
    void CreateNativeToolbar24();
    void CreateNativeToolbar16();
    void ToggleToolBars(bool all);

    void SetToolBar(wxToolBar* tb);

    void ViewPaneUI(const wxString& paneName, wxUpdateUIEvent& event);
    void CreateRecentlyOpenedFilesMenu();
    void CreateWelcomePage();
    bool ReloadExternallyModifiedProjectFiles();
    void DoEnableWorkspaceViewFlag(bool enable, int flag);
    void DoUpdatePerspectiveMenu();
    bool IsWorkspaceViewFlagEnabled(int flag);
    /**
     * @brief show the startup wizard
     * @return true if a restart is needed
     */
    bool StartSetupWizard();

    void DoShowCaptions(bool show);

public:
    void ViewPane(const wxString& paneName, bool checked);
    void ShowOrHideCaptions();
    wxToolBar* GetMainToolBar() const { return m_mainToolBar; }

protected:
    //----------------------------------------------------
    // event handlers
    //----------------------------------------------------
    void OnDebugStarted(clDebugEvent& event);
    void OnDebugEnded(clDebugEvent& event);

    void OnRestoreDefaultLayout(wxCommandEvent& e);
    void OnIdle(wxIdleEvent& e);
    void OnBuildEnded(clCommandEvent& event);
    void OnQuit(wxCommandEvent& WXUNUSED(event));
    void OnClose(wxCloseEvent& event);

    void OnSave(wxCommandEvent& event);
    void OnDuplicateTab(wxCommandEvent& event);
    void OnFileSaveUI(wxUpdateUIEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnFileReload(wxCommandEvent& event);
    void OnFileLoadTabGroup(wxCommandEvent& event);
    void OnNativeTBUnRedoDropdown(wxCommandEvent& event);
    void OnTBUnRedo(wxAuiToolBarEvent& event);
    void OnCodeComplete(wxCommandEvent& event);
    void OnWordComplete(wxCommandEvent& event);
    void OnCompleteWordRefreshList(wxCommandEvent& event);
    void OnFunctionCalltip(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnCheckForUpdate(wxCommandEvent& e);
    void OnRunSetupWizard(wxCommandEvent& e);
    void OnFileNew(wxCommandEvent& event);
    void OnFileOpen(wxCommandEvent& event);
    void OnFileOpenFolder(wxCommandEvent& event);
    void OnFileClose(wxCommandEvent& event);
    void OnFileCloseUI(wxUpdateUIEvent& event);
    void OnFileSaveAll(wxCommandEvent& event);
    void OnFileSaveTabGroup(wxCommandEvent& event);
    void OnFileExistUpdateUI(wxUpdateUIEvent& event);
    void OnCopyFilePathRelativeToWorkspaceUI(wxUpdateUIEvent& event);
    void OnFileSaveAllUI(wxUpdateUIEvent& event);
    void OnCompleteWordUpdateUI(wxUpdateUIEvent& event);
    void OnFunctionCalltipUI(wxUpdateUIEvent& event);
    void OnIncrementalSearch(wxCommandEvent& event);
    void OnIncrementalReplace(wxCommandEvent& event);
    void OnIncrementalSearchUI(wxUpdateUIEvent& event);
    void OnViewToolbar(wxCommandEvent& event);
    void OnViewToolbarUI(wxUpdateUIEvent& event);
    void OnPrint(wxCommandEvent& event);
    void OnPageSetup(wxCommandEvent& event);
    void OnRecentWorkspaceUI(wxUpdateUIEvent& e);

    void OnViewOptions(wxCommandEvent& event);
    void OnToggleMainTBars(wxCommandEvent& event);
    void OnTogglePluginTBars(wxCommandEvent& event);
    void OnTogglePanes(wxCommandEvent& event);
    void OnToggleMinimalView(wxCommandEvent& event);
    void OnToggleMinimalViewUI(wxUpdateUIEvent& event);
    void OnShowStatusBar(wxCommandEvent& event);
    void OnShowStatusBarUI(wxUpdateUIEvent& event);
    void OnShowToolbar(wxCommandEvent& event);
    void OnShowToolbarUI(wxUpdateUIEvent& event);
    void OnShowTabBar(wxCommandEvent& event);
    void OnShowTabBarUI(wxUpdateUIEvent& event);
    void OnProjectNewWorkspace(wxCommandEvent& event);
    void OnProjectNewProject(wxCommandEvent& event);
    void OnReloadWorkspace(wxCommandEvent& event);
    void OnReloadWorkspaceUI(wxUpdateUIEvent& event);
    void OnSwitchWorkspace(wxCommandEvent& event);
    void OnSwitchWorkspaceUI(wxUpdateUIEvent& event);
    void OnNewWorkspaceUI(wxUpdateUIEvent& event);
    void OnCloseWorkspace(wxCommandEvent& event);
    void OnProjectAddProject(wxCommandEvent& event);
    void OnReconcileProject(wxCommandEvent& event);
    void OnWorkspaceOpen(wxUpdateUIEvent& event);
    void OnNewProjectUI(wxUpdateUIEvent& event);
    void OnRetagWorkspaceUI(wxUpdateUIEvent& event);
    void OnAddEnvironmentVariable(wxCommandEvent& event);
    void OnAdvanceSettings(wxCommandEvent& event);
    void OnCtagsOptions(wxCommandEvent& event);
    void OnBuildProject(wxCommandEvent& event);
    void OnBuildProjectOnly(wxCommandEvent& event);
    void OnShowAuiBuildMenu(wxAuiToolBarEvent& e);
    void OnBuildAndRunProject(wxCommandEvent& event);
    void OnRebuildProject(wxCommandEvent& event);
    void OnRetagWorkspace(wxCommandEvent& event);
    void OnBuildProjectUI(wxUpdateUIEvent& event);
    void OnStopBuild(wxCommandEvent& event);
    void OnStopBuildUI(wxUpdateUIEvent& event);
    void OnStopExecutedProgram(wxCommandEvent& event);
    void OnStopExecutedProgramUI(wxUpdateUIEvent& event);
    void OnCleanProject(wxCommandEvent& event);
    void OnCleanProjectOnly(wxCommandEvent& event);
    void OnCleanProjectUI(wxUpdateUIEvent& event);
    void OnExecuteNoDebug(wxCommandEvent& event);
    void OnExecuteNoDebugUI(wxUpdateUIEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnFileCloseAll(wxCommandEvent& event);
    void OnQuickOutline(wxCommandEvent& event);
    void OnImportMSVS(wxCommandEvent& e);
    void OnDebugAttach(wxCommandEvent& event);
    void OnCopyFilePath(wxCommandEvent& event);
    void OnCopyFilePathRelativeToWorkspace(wxCommandEvent& event);
    void OnCopyFilePathOnly(wxCommandEvent& event);
    void OnCopyFileName(wxCommandEvent& event);
    void OnHighlightWord(wxCommandEvent& event);
    void OnShowNavBar(wxCommandEvent& e);
    void OnShowNavBarUI(wxUpdateUIEvent& e);
    void OnOpenShellFromFilePath(wxCommandEvent& e);
    void OnOpenFileExplorerFromFilePath(wxCommandEvent& e);
    void OnDetachEditor(wxCommandEvent& e);
    void OnMarkEditorReadonly(wxCommandEvent& e);
    void OnMarkEditorReadonlyUI(wxUpdateUIEvent& e);
    void OnDetachEditorUI(wxUpdateUIEvent& e);
    void OnQuickDebug(wxCommandEvent& e);
    void OnQuickDebugUI(wxUpdateUIEvent& e);
    void OnDebugCoreDump(wxCommandEvent& e);
    void OnNextFiFMatch(wxCommandEvent& e);
    void OnPreviousFiFMatch(wxCommandEvent& e);
    void OnNextFiFMatchUI(wxUpdateUIEvent& e);
    void OnPreviousFiFMatchUI(wxUpdateUIEvent& e);
    void OnGrepWord(wxCommandEvent& e);
    void OnGrepWordUI(wxUpdateUIEvent& e);
    void OnThemeChanged(wxCommandEvent& e);
    void OnEnvironmentVariablesModified(clCommandEvent& e);

    // handle symbol tree events
    void OnParsingThreadMessage(wxCommandEvent& e);
    void OnDatabaseUpgrade(wxCommandEvent& e);
    void OnDatabaseUpgradeInternally(wxCommandEvent& e);
    void OnRefreshPerspectiveMenu(wxCommandEvent& e);
    void OnClearTagsCache(wxCommandEvent& e);
    void OnRetaggingCompelted(wxCommandEvent& e);
    void OnRetaggingProgress(wxCommandEvent& e);

    void OnRecentFile(wxCommandEvent& event);
    void OnRecentWorkspace(wxCommandEvent& event);
    void OnBackwardForward(wxCommandEvent& event);
    void OnBackwardForwardUI(wxUpdateUIEvent& event);

    void OnShowDebuggerWindow(wxCommandEvent& e);
    void OnShowDebuggerWindowUI(wxUpdateUIEvent& e);
    void OnDebug(wxCommandEvent& e);
    void OnDebugUI(wxUpdateUIEvent& e);
    void OnDebugRestart(wxCommandEvent& e);
    void OnDebugRestartUI(wxUpdateUIEvent& e);
    void OnDebugStop(wxCommandEvent& e);
    void OnDebugStopUI(wxUpdateUIEvent& e);
    void OnDebugManageBreakpointsUI(wxUpdateUIEvent& e);
    void OnDebugCmd(wxCommandEvent& e);
    void OnToggleReverseDebugging(wxCommandEvent& e);
    void OnToggleReverseDebuggingRecording(wxCommandEvent& e);
    void OnToggleReverseDebuggingUI(wxUpdateUIEvent& e);
    void OnToggleReverseDebuggingRecordingUI(wxUpdateUIEvent& e);
    void OnDebugCmdUI(wxUpdateUIEvent& e);

    void OnDebugRunToCursor(wxCommandEvent& e);
    void OnDebugJumpToCursor(wxCommandEvent& e);

    // Special UI handlers for debugger events
    void OnDebugStepInstUI(wxUpdateUIEvent& e);
    void OnDebugJumpToCursorUI(wxUpdateUIEvent& e);
    void OnDebugRunToCursorUI(wxUpdateUIEvent& e);
    void OnDebugInterruptUI(wxUpdateUIEvent& e);
    void OnDebugShowCursorUI(wxUpdateUIEvent& e);

    void OnDebuggerSettings(wxCommandEvent& e);
    void OnLinkClicked(wxHtmlLinkEvent& e);
    void OnLoadSession(wxCommandEvent& e);
    void OnShowWelcomePage(wxCommandEvent& event);
    void OnShowWelcomePageUI(wxUpdateUIEvent& event);
    void OnLoadWelcomePage(wxCommandEvent& event);
    void OnLoadWelcomePageUI(wxUpdateUIEvent& event);
    void OnAppActivated(wxActivateEvent& event);
    void OnReloadExternallModified(wxCommandEvent& e);
    void OnReloadExternallModifiedNoPrompt(wxCommandEvent& e);
    void OnCompileFile(wxCommandEvent& e);
    void OnCompileFileUI(wxUpdateUIEvent& e);
    void OnCloseAllButThis(wxCommandEvent& e);
    void OnCloseTabsToTheRight(wxCommandEvent& e);
    void OnWorkspaceMenuUI(wxUpdateUIEvent& e);
    void OnUpdateBuildRefactorIndexBar(wxCommandEvent& e);
    void OnUpdateNumberOfBuildProcesses(wxCommandEvent& e);
    void OnBuildWorkspace(wxCommandEvent& e);
    void OnBuildWorkspaceUI(wxUpdateUIEvent& e);
    void OnCleanWorkspace(wxCommandEvent& e);
    void OnCleanWorkspaceUI(wxUpdateUIEvent& e);
    void OnReBuildWorkspace(wxCommandEvent& e);
    void OnReBuildWorkspaceUI(wxUpdateUIEvent& e);

    // Perspectives management
    void OnChangePerspective(wxCommandEvent& e);
    void OnChangePerspectiveUI(wxUpdateUIEvent& e);
    void OnManagePerspectives(wxCommandEvent& e);
    void OnSaveLayoutAsPerspective(wxCommandEvent& e);

    // EOL
    void OnConvertEol(wxCommandEvent& e);
    void OnViewDisplayEOL(wxCommandEvent& e);
    void OnViewWordWrap(wxCommandEvent& e);
    void OnViewWordWrapUI(wxUpdateUIEvent& e);
    void OnViewDisplayEOL_UI(wxUpdateUIEvent& e);

    // Docking windows events
    void OnAuiManagerRender(wxAuiManagerEvent& e);
    void OnDockablePaneClosed(wxAuiManagerEvent& e);
    void OnViewPane(wxCommandEvent& event);
    void OnViewPaneUI(wxUpdateUIEvent& event);
    void OnDetachWorkspaceViewTab(wxCommandEvent& e);
    void OnHideWorkspaceViewTab(wxCommandEvent& e);
    void OnDetachDebuggerViewTab(wxCommandEvent& e);
    void OnNewDetachedPane(wxCommandEvent& e);
    void OnDestroyDetachedPane(wxCommandEvent& e);

    void OnManagePlugins(wxCommandEvent& e);
    void OnCppContextMenu(wxCommandEvent& e);

    void OnConfigureAccelerators(wxCommandEvent& e);
    void OnStartPageEvent(wxCommandEvent& e);
    void OnNewVersionAvailable(wxCommandEvent& e);
    void OnVersionCheckError(wxCommandEvent& e);
    void OnGotoCodeLiteDownloadPage(wxCommandEvent& e);
    void OnBatchBuild(wxCommandEvent& e);
    void OnBatchBuildUI(wxUpdateUIEvent& e);
    void OnSyntaxHighlight(wxCommandEvent& e);
    void OnShowWhitespaceUI(wxUpdateUIEvent& e);
    void OnShowWhitespace(wxCommandEvent& e);
    void OnNextTab(wxCommandEvent& e);
    void OnPrevTab(wxCommandEvent& e);
    void OnNextPrevTab_UI(wxUpdateUIEvent& e);
    void OnShowFullScreen(wxCommandEvent& e);
    void OnFindResourceXXX(wxCommandEvent& e);
    void OnShowActiveProjectSettings(wxCommandEvent& e);
    void OnShowActiveProjectSettingsUI(wxUpdateUIEvent& e);
    void OnLoadPerspective(wxCommandEvent& e);
    void OnWorkspaceSettings(wxCommandEvent& e);
    void OnWorkspaceEditorPreferences(wxCommandEvent& e);
    void OnParserThreadReady(wxCommandEvent& e);

    // Clang
    void OnPchCacheStarted(wxCommandEvent& e);
    void OnPchCacheEnded(wxCommandEvent& e);

    // Misc
    void OnActivateEditor(wxCommandEvent& e);
    void OnActiveEditorChanged(wxCommandEvent& e);
    void OnUpdateCustomTargetsDropDownMenu(wxCommandEvent& e);
    void OnWorkspaceLoaded(wxCommandEvent& e);
    void OnRefactoringCacheStatus(wxCommandEvent& e);
    void OnWorkspaceClosed(wxCommandEvent& e);
    void OnChangeActiveBookmarkType(wxCommandEvent& e);
    void OnShowBookmarkMenu(wxAuiToolBarEvent& e);
    void OnSettingsChanged(wxCommandEvent& e);
    void OnEditMenuOpened(wxMenuEvent& e);
    void OnProjectRenamed(clCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif // LITEEDITOR_FRAME_H
