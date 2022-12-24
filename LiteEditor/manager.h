//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : manager.h
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
#ifndef MANAGER_H
#define MANAGER_H

#include "async_executable_cmd.h"
#include "breakpointsmgr.h"
#include "clDebuggerTerminal.h"
#include "clKeyboardManager.h"
#include "cl_command_event.h"
#include "ctags_manager.h"
#include "debuggerobserver.h"
#include "filehistory.h"
#include "perspectivemanager.h"
#include "queuecommand.h"
#include "shell_command.h"
#include "singleton.h"
#include "workspace.h"

#include <list>
#include <map>
#include <wx/event.h>

class clEditor;
class IProcess;

// ====================================================================
// The Manager class
// ====================================================================

extern const wxEventType wxEVT_CMD_RESTART_CODELITE;

class DisplayVariableDlg;

class DbgStackInfo
{
public:
    size_t depth;
    wxString func;

public:
    DbgStackInfo()
        : depth(wxString::npos)
        , func(wxT(""))
    {
    }

    ~DbgStackInfo() { Clear(); }

    void Clear()
    {
        func.Clear();
        depth = wxString::npos;
    }

    bool operator==(const DbgStackInfo& rhs) { return func == rhs.func && depth == rhs.depth; }

    bool operator!=(const DbgStackInfo& rhs) { return func != rhs.func || depth != rhs.depth; }

    bool IsValid() const { return !func.IsEmpty() && depth != wxString::npos; }
};

class Manager : public wxEvtHandler, public IDebuggerObserver
{
public:
    // Virtual Directory 'Create' error code
    enum { VD_OK, VD_EXISTS, VD_ERROR };

protected:
    friend class Singleton<Manager>;

    wxString m_installDir;
    wxString m_startupDir;
    wxString m_originalCwd;
    FileHistory m_recentWorkspaces;
    ShellCommand* m_shellProcess;
    IProcess* m_programProcess;
    BreakptMgr* m_breakptsmgr;
    bool m_isShutdown;
    bool m_workspceClosing;
    bool m_dbgCanInteract;
    bool m_useTipWin;
    long m_tipWinPos;
    int m_frameLineno;
    std::list<QueueCommand> m_buildQueue;
    wxArrayString m_dbgWatchExpressions;
    DisplayVariableDlg* m_watchDlg;
    bool m_retagInProgress;
    bool m_repositionEditor; // flag used for debugging, should editor be repositioned after user updates like "add
                             // watch"
    DbgStackInfo m_dbgCurrentFrameInfo;
    PerspectiveManager m_perspectiveManager;
    clDebuggerTerminalPOSIX m_debuggerTerminal;

protected:
    Manager(void);
    virtual ~Manager(void);
    void OnHideGdbTooltip(clCommandEvent& event);

    //--------------------------- Global State -----------------------------
public:
    DisplayVariableDlg* GetDebuggerTip();

    PerspectiveManager& GetPerspectiveManager() { return m_perspectiveManager; }

    void SetRetagInProgress(bool retagInProgress) { this->m_retagInProgress = retagInProgress; }
    bool GetRetagInProgress() const { return m_retagInProgress; }

    const wxString& GetOriginalCwd() const { return m_originalCwd; }
    void SetOriginalCwd(const wxString& path) { m_originalCwd = path; }

    const wxString& GetStartupDirectory() const { return m_startupDir; }
    void SetStartupDirectory(const wxString& path) { m_startupDir = path; }

    const wxString& GetInstallDir() const { return m_installDir; }
    void SetInstallDir(const wxString& dir) { m_installDir = dir; }

    bool IsShutdownInProgress() const { return m_isShutdown; }
    void SetShutdownInProgress(bool b) { m_isShutdown = b; }

    bool GetRepositionEditor() const { return m_repositionEditor; }
    void SetRepositionEditor(bool b) { m_repositionEditor = b; }

    void OnRestart(clCommandEvent& event);
    void OnCmdRestart(wxCommandEvent& event);
    void GenerateCompileCommands();
    void OnFindInFilesDismissed(clFindInFilesEvent& event);
    void OnFindInFilesShowing(clFindInFilesEvent& event);
    void OnUpdateDebuggerActiveView(clDebugEvent& event);
    void OnDebuggerAtFileLine(clDebugEvent& event);
    void OnDebuggerSetMemory(clDebugEvent& event);
    void OnDebuggerStopped(clDebugEvent& event);
    void OnDebuggerStopping(clDebugEvent& event);

protected:
    void DoRestartCodeLite();
    void InstallClangTools();

    //--------------------------- Workspace Loading -----------------------------
public:
    /*!
     * \brief
     * check if a workspace is open
     *
     * \returns
     * true if a workspace is open
     */
    bool IsWorkspaceOpen() const;

    const bool& IsWorkspaceClosing() const { return m_workspceClosing; }

    /*!
     * \brief
     * Create a workspace with a given name and path
     *
     * \param name
     * workspace name
     *
     * \param path
     * workspace path
     *
     */
    void CreateWorkspace(const wxString& name, const wxString& path);

    /**
     * Open an existing workspace by path
     */
    void OpenWorkspace(const wxString& path);

    /**
     * \brief close the currently opened workspace and reload it without saving any modifications made to it, if no
     * workspace is opened, this functiond does anything
     */
    void ReloadWorkspace();

    /**
     * Close the current workspace and save all
     * changes
     */
    void CloseWorkspace();

    /**
     * \brief clear the recent workspaces list
     */
    void ClearWorkspaceHistory();

    /**
     * Return list of recently opened workspaces from the configuration file
     * \param files [output]
     */
    void GetRecentlyOpenedWorkspaces(wxArrayString& files);

    /**
     * return the FileHistory object that holds the recently opened
     * workspace data
     */
    FileHistory& GetRecentlyOpenedWorkspacesClass() { return m_recentWorkspaces; }

    /**
     * @brief update the C++ parser search / exclude paths with the global paths
     * and the workspace specifc ones
     * @return true if the paths were modified, false otherwise
     */
    void UpdateParserPaths(bool notify);

protected:
    void DoSetupWorkspace(const wxString& path);

    void OnAddWorkspaceToRecentlyUsedList(wxCommandEvent& e);

    /**
     * @brief a project was renamed, reload the workspace
     */
    void OnProjectRenamed(clCommandEvent& event);
    //--------------------------- Workspace Projects Mgmt -----------------------------
public:
    /**
     * @brief add 'fileName' to the list of recently used workspaces
     */
    void AddToRecentlyOpenedWorkspaces(const wxString& fileName);

    /**
     * @brief create an empty project
     */
    void CreateProject(ProjectData& data, const wxString& workspaceFolder);

    /**
     * Add an existing project to the workspace. If no workspace is open,
     * this function does nothing
     * \param path project file path name to add
     */
    void AddProject(const wxString& path);

    /**
     * Checks the active project for file entries that no longer exist on the filesystem,
     * and for existing files that are missing from the project
     * \param projectName the name of the project to reconcile. If "" do the currently-active project
     */
    void ReconcileProject(const wxString& projectName = "");

    /**
     * Import a MS Solution file and open it in the editor
     * \param path path to the .sln file
     */
    void ImportMSVSSolution(const wxString& path, const wxString& defaultCompiler);

    /**
     * Remove the a project from the workspace
     * \param name project name to remove
     * \return true on success false otherwise
     */
    bool RemoveProject(const wxString& name, bool notify);

    /**
     * Return all project names under this workspace
     */
    void GetProjectList(wxArrayString& list);

    /**
     * find project by name
     */
    ProjectPtr GetProject(const wxString& name) const;

    /**
     * \return active project name
     */
    wxString GetActiveProjectName();

    /**
     * Set project as active
     * \param name project name to set as active
     */
    void SetActiveProject(const wxString& name);

    /**
     * Return the workspace build matrix
     */
    BuildMatrixPtr GetWorkspaceBuildMatrix() const;

    /**
     * Set or update the workspace build matrix
     */
    void SetWorkspaceBuildMatrix(BuildMatrixPtr matrix);

    /**
     * @brief user modified the project settings dialog
     */
    void OnProjectSettingsModified(clProjectSettingsEvent& event);

    //--------------------------- Workspace Files Mgmt -----------------------------
public:
    /**
     * return list of files in absolute path of the whole workspace
     */
    void GetWorkspaceFiles(wxArrayString& files);

    /**
     * @brief return list of files in a form of std::set
     */
    void GetWorkspaceFiles(std::set<wxString>& files);

    /**
     * return list of files that are part of the workspace
     */
    void GetWorkspaceFiles(std::vector<wxFileName>& files, bool absPath = false);

    /**
     * check if a file is part of the workspace
     * \param fileName the file name in absolute path
     */
    bool IsFileInWorkspace(const wxString& fileName);

    /**
     * Search for (non-absolute) file in the workspace
     */
    wxFileName FindFile(const wxString& fileName, const wxString& project = wxEmptyString);

    /**
     * retag workspace
     */
    void RetagWorkspace(TagsManager::RetagType type);

    /**
     * \brief retag a given file
     * \param filename
     */
    void RetagFile(const wxString& filename);

    /**
     * \brief Launch the ParseThread to update the preprocessor vizualisation
     * \param filename
     */
    void UpdatePreprocessorFile(clEditor* editor);

protected:
    wxFileName FindFile(const wxArrayString& files, const wxFileName& fn);

    //--------------------------- Project Files Mgmt -----------------------------
public:
    /**
     * Add new virtual directory to the workspace.
     * \param virtualDirFullPath a dot separated string of the new virtual directory full path up to the parent project
     *        for example: to add a new VD name VD3 under: Project1->VD1->VD2 path should contain: Project1.VD1.VD2.VD3
     */
    int AddVirtualDirectory(const wxString& virtualDirFullPath, bool createIt);

    /**
     * Remove virtual directory from the workspace.
     * \param virtualDirFullPath a dot separated string of the virtual directory to be removed
     */
    void RemoveVirtualDirectory(const wxString& virtualDirFullPath);

    /**
     * Create new file on the disk and open it in the main editor
     * \param fileName file full path (including directories)
     * \param vdFullPath path of the virtual directory
     */
    bool AddNewFileToProject(const wxString& fileName, const wxString& vdFullPath, bool openIt = true);

    /**
     * Add an existing file to workspace
     * \param fileName file full path (including directories)
     * \param vdFullPath path of the virtual directory
     */
    bool AddFileToProject(const wxString& fileName, const wxString& vdFullPath, bool openIt = false);

    /**
     * \brief
     * \param files
     * \param vdFullPath
     * \param actualAdded
     * \return
     */
    void AddFilesToProject(const wxArrayString& files, const wxString& vdFullPath, wxArrayString& actualAdded);

    /**
     * remove file from the workspace
     * \param fileName the full path of the file to be removed
     * \param vdFullPath the files' virtual directory path (including project)
     * \param fullpathRemoved [output] set the full path of the file removed
     * \param notify if set to true, this function will also fire the wxEVT_PROJ_FILE_REMOVED event
     */
    bool RemoveFile(const wxString& fileName, const wxString& vdFullPath, wxString& fullpathRemoved,
                    bool notify = true);

    /**
     * remove file from the workspace
     * \param origName the full path of the file to be renamed
     * \param newName the full path the file must be renamed to
     * \param vdFullPath the files' virtual directory path (including project)
     */
    bool RenameFile(const wxString& origName, const wxString& newName, const wxString& vdFullPath);

    bool MoveFileToVD(const wxString& fileName, const wxString& srcVD, const wxString& targetVD);

    /**
     * Rebuild the database by removing all entries from the database
     * that belongs to a given project, and then re-index all files
     * \param projectName project to re-tag
     */
    void RetagProject(const wxString& projectName, bool quickRetag);

    /**
     * return list of files in absolute path of a given project
     * \param project project name
     */
    void GetProjectFiles(const wxString& project, wxArrayString& files);

    /**
     * @brief return list of files belonged the active project (same as running: GetProjectFiles(GetActiveProjectName(),
     * files)
     */
    void GetActiveProjectFiles(wxArrayString& files);

    /**
     * @brief return the currently opened file's project files
     */
    void GetActiveFileProjectFiles(wxArrayString& files);

    /**
     * @brief return the project name that 'fullPathFileName' belongs to. if 2 matches are found, return
     * the first one, or empty string if no match is found
     * \param fullPathFileName the filepath to search with
     * \param caseSensitive do a case-sensitive search
     */
    wxString GetProjectNameByFile(const wxString& fullPathFileName, bool caseSensitive = false);
    /**
     * @brief return the project name that 'fullPathFileName' belongs to. if 2 matches are found, return
     * the first one, or empty string if no match is found
     * \param fullPathFileName the filepath to search with
     * \param caseSensitive do a case-sensitive search
     */
    wxString GetProjectNameByFile(wxString& fullPathFileName, bool caseSensitive = false);

    //--------------------------- Project Settings Mgmt -----------------------------
public:
    /**
     * @brief if a workspace is opened, return the current build configuration
     * of the active project
     * @return active build configuration or NULL
     */
    BuildConfigPtr GetCurrentBuildConf();

    /**
     * Return a project working directory
     * \param project project name
     */
    wxString GetProjectCwd(const wxString& project) const;

    /**
     * Return project settings by name
     * \param projectName project name
     * \return project settings smart prt
     */
    ProjectSettingsPtr GetProjectSettings(const wxString& projectName) const;

    /**
     * Set project settings
     * \param projectName project name
     * \param settings settings to update
     */
    void SetProjectSettings(const wxString& projectName, ProjectSettingsPtr settings);

    /**
     * Set project global settings
     * \param projectName project name
     * \param settings global settings to update
     */
    void SetProjectGlobalSettings(const wxString& projectName, BuildConfigCommonPtr settings);

    /**
     * \brief return the project excution command as it appears in the project settings
     * \param projectName
     * \param wd the working directory that the command should be running from
     * \param considerPauseWhenExecuting when set to true (default) CodeLite will take into consideration the value set
     * in the project
     * settings 'Pause when execution ends'
     * \return project execution command or wxEmptyString if the project does not exist
     */
    wxString GetProjectExecutionCommand(const wxString& projectName, wxString& wd,
                                        bool considerPauseWhenExecuting = true);

    bool DoFindDockInfo(const wxString& saved_perspective, const wxString& dock_name, wxString& dock_info);

    //--------------------------- Top Level Pane Management -----------------------------
public:
    /**
     * \brief test if pane_name is resides in the wxAuiManager and is visible
     * \param pane_name pane name to search for
     * \return true on success (exist in the AUI manager and visible), false otherwise
     */
    bool IsPaneVisible(const wxString& pane_name);

    /**
     * Show output pane and set focus to focusWin
     * \param focusWin tab name to set the focus
     * \return return true if the output pane was hidden and this function forced it to appear. false if the window was
     * already
     * shown and nothing needed to be done
     */
    bool ShowOutputPane(wxString focusWin = wxEmptyString, bool commit = true);

    /**
     * Show the debugger pane
     */
    void ShowDebuggerPane(bool commit = true);

    /**
     * Show the workspace pane and set focus to focusWin
     * \param focusWin tab name to set the focus
     */
    void ShowWorkspacePane(wxString focusWin = wxEmptyString, bool commit = true);

    /**
     * Hide pane
     */
    void HidePane(const wxString& paneName, bool commit = true);

    /**
     * Hide/Show all panes. This function saves the current prespective and
     * then hides all panes, when called again, all panes are restored
     */
    void TogglePanes();

    //--------------------------- Menu and Accelerator Mmgt -----------------------------
public:
    /**
     * \brief update the menu bar accelerators
     */
    void UpdateMenuAccelerators(wxFrame* frame = NULL);

    //--------------------------- Run Program (No Debug) -----------------------------
public:
    /**
     * return true a child program is running
     */
    bool IsProgramRunning() const;

    /**
     * Execute the project with no debugger
     */
    void ExecuteNoDebug(const wxString& projectName);

    /**
     * Kill child program which is running
     */
    void KillProgram();

protected:
    void OnProcessEnd(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);
    void OnBuildEnded(clBuildEvent& event);

    /**
     * @brief react to a build starting event
     */
    void OnBuildStarting(clBuildEvent& event);

    //--------------------------- Debugger Support -----------------------------
protected:
    void DoUpdateDebuggerTabControl(wxWindow* curpage);
    bool DebuggerPaneWasShown;

public:
    BreakptMgr* GetBreakpointsMgr() { return m_breakptsmgr; }

    /**
     * @brief start a terminal for the debugger and return its TTY
     * @param title terminal title
     * @param tty [output] the terminal TTY
     * @return true on success, false otherwise
     */
    bool StartTTY(const wxString& title, wxString& tty);

    void UpdateDebuggerPane();

    void SetMemory(const wxString& address, size_t count, const wxString& hex_value);

    /**
     * Stores the debugger pane status when the debug session started
     */
    void SetDebuggerPaneOriginallyVisible(bool shown) { DebuggerPaneWasShown = shown; }

    /**
     * Returns true if the debugger pane was already shown when the debug session started
     */
    bool GetDebuggerPaneOriginallyVisible() const { return DebuggerPaneWasShown; }

    //---------------------------------------------------
    // Debugging API
    //---------------------------------------------------

    void DbgStart(long pid = wxNOT_FOUND);
    void DbgStop();
    void DbgContinue();
    void DbgMarkDebuggerLine(const wxString& fileName, int lineno);
    void DbgUnMarkDebuggerLine();
    void DbgDoSimpleCommand(int cmd);
    void DbgSetFrame(int frame, int lineno);
    void DbgSetThread(long threadId);
    bool DbgCanInteract();
    void DbgClearWatches();
    void DbgRestoreWatches();
    DbgStackInfo DbgGetCurrentFrameInfo() { return m_dbgCurrentFrameInfo; }

    //---------------------------------------------------
    // Internal implementaion for various debugger events
    //---------------------------------------------------

    void UpdateAddLine(const wxString& line, const bool OnlyIfLoggingOn = false);
    void UpdateFileLine(const wxString& file, int lineno, bool repositionEditor = true);
    void UpdateGotControl(const DebuggerEventData& e);
    void UpdateLostControl();
    void UpdateRemoteTargetConnected(const wxString& line);
    void UpdateTypeReolsved(const wxString& expression, const wxString& type);
    void UpdateAsciiViewer(const wxString& expression, const wxString& tip);

    //---------------------------------------------------
    // Handle debugger event
    //---------------------------------------------------

    void DebuggerUpdate(const DebuggerEventData& event);
    void DoShowQuickWatchDialog(const DebuggerEventData& event);

    //--------------------------- Build Management -----------------------------
public:
    /**
     * return true if a compilation is in process (either clean or build)
     */
    bool IsBuildInProgress() const;

    /**
     * Stop current build process
     */
    void StopBuild();

    /**
     * \brief add build job to the internal queue
     * \param buildInfo
     */
    void PushQueueCommand(const QueueCommand& buildInfo);

    /**
     * \brief process the next build job
     */
    void ProcessCommandQueue();

    /**
     * \brief build the entire workspace. This operation is equal to
     * manually right clicking on each project in the workspace and selecting
     * 'build'
     */
    void BuildWorkspace();

    /**
     * \brief clean the entire workspace. This operation is equal to
     * manually right clicking on each project in the workspace and selecting
     * 'clean'
     */
    void CleanWorkspace();

    /**
     * \brief clean, followed by buid of the entire workspace. This operation is equal to
     * manually right clicking on each project in the workspace and selecting
     * 'clean'
     */
    void RebuildWorkspace();

    /**
     * \brief when building using custom build, execute the makefile generation command provided by the user
     * \param project project to execute it for
     */
    void RunCustomPreMakeCommand(const wxString& project);

    /**
     * compile single file from a given
     * given project
     */
    void CompileFile(const wxString& project, const wxString& fileName, bool preprocessOnly = false);

    /**
     * return true if the last buid ended successfully
     */
    bool IsBuildEndedSuccessfully() const;

    /**
     * @brief return the currently active project && configuration
     * @param project [output]
     * @param conf [output]
     */
    void GetActiveProjectAndConf(wxString& project, wxString& conf);
    /**
     * @brief return true if debugger view is visible
     * This can be true incase the view is the selected tab in the debuggger pane notebook
     * or incase it is detached and visible
     */
    bool IsDebuggerViewVisible(const wxString& name);

    /**
     * @brief show the new project wizard.
     * @param workspaceFolder the new project will be placed inside this workspace folder.
     * If left empty (the default) place the new project directly under the workspace
     */
    void ShowNewProjectWizard(const wxString& workspaceFolder = wxEmptyString);

protected:
    void DoBuildProject(const QueueCommand& buildInfo);
    void DoCleanProject(const QueueCommand& buildInfo);
    void DoCustomBuild(const QueueCommand& buildInfo);
    void DoCmdWorkspace(int cmd);
    void DoSaveAllFilesBeforeBuild();
};

typedef Singleton<Manager> ManagerST;

#endif // MANAGER_H
