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
#include "envvar_table.h"
#include "wx/choice.h"
#include "wx/timer.h"
#include "parse_thread.h"
#include "tags_options_dlg.h"
#include <wx/html/htmlwin.h>
#include "debuggerpane.h"
#include "custom_notebook.h"
#include "mainbook.h"

// forward decls
class TagEntry;
class WorkspacePane;
class wxToolBar;
class Notebook;
class OpenWindowsPanel;
class WorkspaceTab;
class FileExplorer;
class OutputTabWindow;
class DockablePaneMenuManager;
class OutputViewControlBar;

//--------------------------------
// Helper class
//--------------------------------
extern const wxEventType wxEVT_UPDATE_STATUS_BAR;

struct StartPageData {
	wxString name;
	wxString file_path;
	wxString action;
};

class Frame : public wxFrame
{
	MainBook *                            m_mainBook;
	static Frame*                         m_theFrame;
	wxAuiManager                          m_mgr;
	OutputPane *                          m_outputPane;
	WorkspacePane *                       m_workspacePane;
	wxArrayString                         m_files;
	wxTimer *                             m_timer;
	std::map<int, wxString>               m_viewAsMap;
	TagsOptionsData                       m_tagsOptionsData;
	DebuggerPane *                        m_debuggerPane;
	wxToolBar *                           m_debuggerTb;
	bool                                  m_buildAndRun;
	GeneralInfo                           m_frameGeneralInfo;
	std::map<int, wxString>               m_toolbars;
	std::map<int, wxString>               m_panes;
    std::vector<std::map<int, wxString> > m_status;
	wxMenu *                              m_cppMenu;
	bool                                  m_highlightWord;
	DockablePaneMenuManager *             m_DPmenuMgr;
	OutputViewControlBar *                m_controlBar;
	wxPanel*                              m_mainPanel;

public:
	static Frame* Get();
	static void Initialize(bool loadLastSession);

	DockablePaneMenuManager *GetDockablePaneMenuManager() {
		return m_DPmenuMgr;
	}

	wxPanel *GetMainPanel() {return m_mainPanel;}

	virtual ~Frame(void);
	/**
	 * @brief set frame option flag
	 * @param set
	 * @param flag
	 */
	void SetFrameFlag(bool set, int flag);

	/**
	 * @brief return true if the word under the caret should be highlighted
	 * @return
	 */
	bool GetHighlightWord() {
		return m_highlightWord;
	}

	/**
	 * @brief Return language name by menu item id
	 * @param id
	 * @return
	 */
	wxString GetViewAsLanguageById(int id) const;

	/**
	 * @brief
	 * @param editor
	 */
	void SetFrameTitle(LEditor *editor);

	MainBook* GetMainBook() const {
		return m_mainBook;
	}

	/**
	 * Close the current file
	 */
	void CloseActiveFile();

	/**
	 * @return the output pane (the bottom pane)
	 */
	OutputPane *GetOutputPane() {
		return m_outputPane;
	}

	/**
	 * return the debugger pane
	 * @return
	 */
	DebuggerPane *GetDebuggerPane() {
		return m_debuggerPane;
	}

	/**
	 * @return the workspace pane (the one that contained the Symbol view & class view)
	 */
	WorkspacePane *GetWorkspacePane() {
		return m_workspacePane;
	}

	/**
	 * return the workspace tab pane
	 */
	WorkspaceTab *GetWorkspaceTab();

	/**
	 * return the file explorer pane
	 */
	FileExplorer *GetFileExplorer();

	/**
	 * @return return AUI docking manager
	 */
	wxAuiManager& GetDockingManager() {
		return m_mgr;
	}

	wxAuiManager* GetDockingManagerPtr() {
		return &m_mgr;
	}

	/**
	 * Return the debugger toolbar
	 */
	wxToolBar *GetDebuggerToolbar() {
		return m_debuggerTb;
	}

	/**
	 * Load session into LE
	 */
	void LoadSession(const wxString &sessionName);

	/**
	 * Compelete the main frame initialization
	 * this method is called immediatly after the
	 * main frame construction is over.
	 */
	void CompleteInitialization();

	void RegisterToolbar(int menuItemId, const wxString &name);
	void RegisterDockWindow(int menuItemId, const wxString &name);

	const GeneralInfo& GetFrameGeneralInfo() const {
		return m_frameGeneralInfo;
	}

	void OnSingleInstanceOpenFiles(wxCommandEvent &e);
	void OnSingleInstanceRaise(wxCommandEvent &e);

	/**
	 * @brief rebuild the give project
	 * @param projectName
	 */
	void RebuildProject(const wxString &projectName);

	/**
	 * @brief display the welcome page
	 */
	void ShowWelcomePage();

	/**
	 * @brief handle custom build targets events
	 */
	void OnBuildCustomTarget(wxCommandEvent &event);

    /**
     * @brief set a status message
     */
    void SetStatusMessage(const wxString &msg, int col, int id = wxID_ANY);

	/**
	 * @brief save the current IDE layout and session
	 */
	void SaveLayoutAndSession();

private:
	// make our frame's constructor private
	Frame(wxWindow *pParent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION | wxSYSTEM_MENU | wxRESIZE_BORDER | wxCLIP_CHILDREN);
	wxString CreateWorkspaceTable();
	wxString CreateFilesTable();

private:
	/**
	 * Construct all the GUI controls of the main frame. this function is called
	 * at construction time
	 */
	void CreateGUIControls(void);
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
	void DispatchCommandEvent(wxCommandEvent &event);
	void DispatchUpdateUIEvent(wxUpdateUIEvent &event);
	void CreateToolbars24();
	void CreateToolbars16();
	void ViewPaneUI(const wxString &paneName, wxUpdateUIEvent&event);
	void ViewPane(const wxString &paneName, wxCommandEvent &event);
	void CreateViewAsSubMenu();
	void CreateRecentlyOpenedFilesMenu();
	void CreateRecentlyOpenedWorkspacesMenu();
	void CreateWelcomePage();
	void CreateMenuBar();
	void UpgradeExternalDbExt();
	void AutoLoadExternalDb();
	void DoBuildExternalDatabase(const wxString &dir = wxEmptyString);

	void ReloadExternallyModifiedProjectFiles();

protected:
	//----------------------------------------------------
	// event handlers
	//----------------------------------------------------
	void OnIdle(wxIdleEvent &e);
    void OnBuildEnded(wxCommandEvent &event);
	void OnQuit(wxCommandEvent& WXUNUSED(event));
	void OnClose(wxCloseEvent &event);
	void OnAddSourceFile(wxCommandEvent& event);
	void OnBuildFromDatabase(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnSaveAs(wxCommandEvent& event);
	void OnFileReload(wxCommandEvent& event);
	void OnCompleteWord(wxCommandEvent& event);
	void OnFunctionCalltip(wxCommandEvent& event);
	void OnDeleteProject(wxCommandEvent& event);
	void OnBuildExternalDatabase(wxCommandEvent& event);
	void OnUseExternalDatabase(wxCommandEvent& event);
	void OnCloseExternalDatabase(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnFileNew(wxCommandEvent &event);
	void OnFileOpen(wxCommandEvent &event);
	void OnFileClose(wxCommandEvent &event);
	void OnFileCloseUI(wxUpdateUIEvent &event);
	void OnFileSaveAll(wxCommandEvent &event);
	void OnFileFindAndReplace(wxCommandEvent &event);
	void OnFileExistUpdateUI(wxUpdateUIEvent &event);
	void OnCompleteWordUpdateUI(wxUpdateUIEvent &event);
	void OnFunctionCalltipUI(wxUpdateUIEvent &event);
	void OnIncrementalSearch(wxCommandEvent &event);
	void OnViewToolbar(wxCommandEvent &event);
	void OnViewToolbarUI(wxUpdateUIEvent &event);
	void OnViewOptions(wxCommandEvent &event);
	void OnTogglePanes(wxCommandEvent &event);
	void OnProjectNewWorkspace(wxCommandEvent &event);
	void OnProjectNewProject(wxCommandEvent &event);
	void OnCreateWorkspace(wxCommandEvent &event);
	void OnReloadWorkspace(wxCommandEvent &event);
	void OnReloadWorkspaceUI(wxUpdateUIEvent &event);
	void OnSwitchWorkspace(wxCommandEvent &event);
	void OnCloseWorkspace(wxCommandEvent &event);
	void OnProjectAddProject(wxCommandEvent &event);
	void OnWorkspaceOpen(wxUpdateUIEvent &event);
	void OnAddEnvironmentVariable(wxCommandEvent &event);
	void OnAdvanceSettings(wxCommandEvent &event);
	void OnCtagsOptions(wxCommandEvent &event);
	void OnBuildProject(wxCommandEvent &event);
	void OnBuildAndRunProject(wxCommandEvent &event);
	void OnRebuildProject(wxCommandEvent &event);
	void OnRetagWorkspace(wxCommandEvent &event);
	void OnBuildProjectUI(wxUpdateUIEvent &event);
	void OnStopBuild(wxCommandEvent &event);
	void OnStopBuildUI(wxUpdateUIEvent &event);
	void OnStopExecutedProgram(wxCommandEvent &event);
	void OnStopExecutedProgramUI(wxUpdateUIEvent &event);
	void OnCleanProject(wxCommandEvent &event);
	void OnCleanProjectUI(wxUpdateUIEvent &event);
	void OnExecuteNoDebug(wxCommandEvent &event);
	void OnExecuteNoDebugUI(wxUpdateUIEvent &event);
	void OnTimer(wxTimerEvent &event);
	void OnFileCloseAll(wxCommandEvent &event);
	void OnFindResource(wxCommandEvent &event);
	void OnFindType(wxCommandEvent &event);
	void OnQuickOutline(wxCommandEvent &event);
	void OnImportMSVS(wxCommandEvent &e);
	void OnDebugAttach(wxCommandEvent &event);
	void OnCopyFilePath(wxCommandEvent &event);
	void OnCopyFilePathOnly(wxCommandEvent &event);
	void OnHighlightWord(wxCommandEvent &event);
	void OnShowNavBar(wxCommandEvent &e);
	void OnShowNavBarUI(wxUpdateUIEvent &e);
    void OnDetachTab(wxCommandEvent &e);
    void OnDetachTabUI(wxUpdateUIEvent &e);
	void OnOpenShellFromFilePath(wxCommandEvent &e);
	void OnQuickDebug(wxCommandEvent &e);
	void OnQuickDebugUI(wxUpdateUIEvent &e);
	void OnNextFiFMatch      (wxCommandEvent &e);
	void OnPreviousFiFMatch  (wxCommandEvent &e);
	void OnNextFiFMatchUI    (wxUpdateUIEvent &e);
	void OnPreviousFiFMatchUI(wxUpdateUIEvent &e);

	// this event is sent from the notebook container to the frame
	void OnFileClosing(NotebookEvent &event);
	void OnPageChanged(NotebookEvent &event);
	void OnPageClosed(NotebookEvent &event);

	//handle symbol tree events
	void OnAddSymbols(SymbolTreeEvent &event);
	void OnDeleteSymbols(SymbolTreeEvent &event);
	void OnUpdateSymbols(SymbolTreeEvent &event);
	void OnParsingThreadDone(wxCommandEvent &e);

	void OnRecentFile(wxCommandEvent &event);
	void OnRecentWorkspace(wxCommandEvent &event);
	void OnBackwardForward(wxCommandEvent &event);
	void OnBackwardForwardUI(wxUpdateUIEvent &event);

	void OnDebug(wxCommandEvent &e);
	void OnDebugUI(wxUpdateUIEvent &e);
	void OnDebugRestart(wxCommandEvent &e);
	void OnDebugRestartUI(wxUpdateUIEvent &e);
	void OnDebugStop(wxCommandEvent &e);
	void OnDebugStopUI(wxUpdateUIEvent &e);
	void OnDebugManageBreakpointsUI(wxUpdateUIEvent &e);
	void OnDebugCmd(wxCommandEvent &e);
	void OnDebugCmdUI(wxUpdateUIEvent &e);
	void OnDebuggerSettings(wxCommandEvent &e);
	void OnMenuOpen(wxMenuEvent &e);
	void OnLinkClicked(wxHtmlLinkEvent &e);
	void OnLoadLastSessionUI(wxUpdateUIEvent &event);
	void OnLoadLastSession(wxCommandEvent &event);
	void OnShowWelcomePage(wxCommandEvent &event);
	void OnShowWelcomePageUI(wxUpdateUIEvent &event);
	void OnLoadWelcomePage(wxCommandEvent &event);
	void OnLoadWelcomePageUI(wxUpdateUIEvent &event);
	void OnAppActivated(wxActivateEvent &event);
	void OnReloadExternallModified(wxCommandEvent &e);
	void OnCompileFile(wxCommandEvent &e);
	void OnCompileFileUI(wxUpdateUIEvent &e);
	void OnCloseAllButThis(wxCommandEvent &e);
	void OnWorkspaceMenuUI(wxUpdateUIEvent &e);
	void OnUpdateBuildRefactorIndexBar(wxCommandEvent &e);

	void OnBuildWorkspace(wxCommandEvent &e);
	void OnBuildWorkspaceUI(wxUpdateUIEvent &e);
	void OnCleanWorkspace(wxCommandEvent &e);
	void OnCleanWorkspaceUI(wxUpdateUIEvent &e);
	void OnReBuildWorkspace(wxCommandEvent &e);
	void OnReBuildWorkspaceUI(wxUpdateUIEvent &e);

	//EOL
	void OnConvertEol(wxCommandEvent &e);
	void OnViewDisplayEOL(wxCommandEvent &e);
	void OnViewDisplayEOL_UI(wxUpdateUIEvent &e);

	//Docking windows events
	void OnAuiManagerRender(wxAuiManagerEvent &e);
    void OnDockablePaneClosed(wxAuiManagerEvent &e);
	void OnViewPane(wxCommandEvent &event);
	void OnViewPaneUI(wxUpdateUIEvent &event);
	void OnDetachWorkspaceViewTab(wxCommandEvent &e);
	void OnDetachDebuggerViewTab(wxCommandEvent &e);
	void OnNewDetachedPane(wxCommandEvent &e);
	void OnDestroyDetachedPane(wxCommandEvent &e);

	void OnManagePlugins(wxCommandEvent &e);
	void OnManageTags(wxCommandEvent &e);
	void OnCppContextMenu(wxCommandEvent &e);

	void OnConfigureAccelerators(wxCommandEvent &e);
	void OnStartPageEvent(wxCommandEvent &e);
	void OnNewVersionAvailable(wxCommandEvent &e);
	void OnBatchBuild(wxCommandEvent &e);
	void OnBatchBuildUI(wxUpdateUIEvent &e);
	void OnSyntaxHighlight(wxCommandEvent &e);
	void OnShowWhitespaceUI(wxUpdateUIEvent &e);
	void OnShowWhitespace(wxCommandEvent &e);
	void OnShowFullScreen(wxCommandEvent &e);
	void OnSetStatusMessage(wxCommandEvent &e);
	void OnShowQuickFinder (wxCommandEvent &e);

// Any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()
};

#endif // LITEEDITOR_FRAME_H
