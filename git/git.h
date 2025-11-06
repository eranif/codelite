//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : git.h
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

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __git__
#define __git__

#include "AsyncProcess/asyncprocess.h"
#include "AsyncProcess/processreaderthread.h"
#include "ai/ResponseCollector.hpp"
#include "clCodeLiteRemoteProcess.hpp"
#include "clResult.hpp"
#include "clTabTogglerHelper.h"
#include "cl_command_event.h"
#include "gitentry.h"
#include "gitui.h"
#include "ieditor.h"
#include "overlaytool.h"
#include "plugin.h"
#include "project.h" // wxStringSet_t

#include <map>
#include <optional>
#include <queue>
#include <set>
#include <vector>
#include <wx/progdlg.h>
#if USE_SFTP
#include "cl_ssh.h"
#endif

class clTreeCtrl;
class clCommandProcessor;
class GitCommitDlg;

#define GIT_TAB_NAME "Git"

class gitAction
{
public:
    int action = 0;
    wxString arguments;
    wxString workingDirectory;

public:
    gitAction() = default;
    gitAction(int act, const wxString& args)
        : action(act)
        , arguments(args)
    {
    }
    ~gitAction() = default;
};

class GitConsole;
class GitCommitListDlg;

struct GitCmd {
    wxString baseCommand;
    size_t processFlags;

    GitCmd(const wxString& cmd, size_t console)
        : baseCommand(cmd)
        , processFlags(console)
    {
    }
    GitCmd()
        : processFlags(IProcessCreateDefault)
    {
    }
    using Vec_t = std::vector<GitCmd>;
};

class GitPlugin : public IPlugin
{
    friend class GitConsole;
    friend class GitCommitListDlg;
    friend class GitCommitDlg;

    using IntMap_t = std::map<int, int>;
    enum {
        gitNone = 0,
        gitUpdateRemotes,
        gitListAll,
        gitListModified,
        gitListRemotes,
        gitAddFile,
        gitDeleteFile,
        gitDiffFile,
        gitDiffFileExternal,
        gitDiffRepoCommit,
        gitDiffRepoShow,
        gitResetFile,
        gitResetRepo,
        gitPull,
        gitPush,
        gitCommit,
        gitBranchCreate,
        gitBranchCurrent,
        gitBranchList,
        gitBranchListRemote,
        gitBranchSwitch,
        gitBranchSwitchRemote,
        gitCommitList,
        gitBlame,
        gitBlameSummary,
        gitRevlist,
        gitRebase,
        gitGarbageCollection,
        gitClone,
        gitStatus,
        gitUndoAdd,
        gitRmFiles,
        gitApplyPatch,
        gitRevertCommit,
        gitStash,
        gitStashPop,
        gitConfig,
    };

    wxArrayString m_localBranchList;
    wxArrayString m_remoteBranchList;
    wxStringSet_t m_trackedFiles;
    wxStringSet_t m_modifiedFiles;
    bool m_addedFiles;
    wxArrayString m_remotes;
    wxColour m_colourTrackedFile;
    wxColour m_colourDiffFile;
    wxString m_pathGITExecutable;
    wxString m_pathGITKExecutable;
    wxString m_repositoryDirectory;
    wxString m_userEnteredRepositoryDirectory;
    wxString m_currentBranch;
    std::list<gitAction> m_gitActionQueue;
    wxTimer m_progressTimer;
    wxString m_progressMessage;
    wxString m_commandOutput;
    bool m_bActionRequiresTreUpdate;
    IProcess* m_process;
    wxEvtHandler* m_eventHandler;
    clToolBar* m_pluginToolbar;
    wxMenu* m_pluginMenu;
    IntMap_t m_treeImageMapping;
    int m_baseImageCount;
    GitConsole* m_console;
    wxString m_workspace_file;
    GitCommitListDlg* m_commitListDlg;
    wxArrayString m_filesSelected;
    wxString m_selectedFolder;
    clCommandProcessor* m_commandProcessor;
    std::unordered_map<wxString, std::vector<wxString>>
        m_blameMap; // contains file: comment per line (extracted from the 'git blame' info)
    size_t m_configFlags = 0;
    wxString m_lastBlameMessage;
    bool m_isRemoteWorkspace = false;
    wxString m_remoteWorkspaceAccount;
    clCodeLiteRemoteProcess m_remoteProcess;
    wxString m_codeliteRemoteScriptPath;
    bool m_isEnabled = false;
    GitCommitDlg* m_commitDialog{nullptr};

#if USE_SFTP
    clSSH::Ptr_t m_ssh;
#endif

private:
    void StartCodeLiteRemote();
    void ClearCodeLiteRemoteInfo();
    void DoShowDiffViewer(const wxString& headFile, const wxString& fileName);
    void DoExecuteCommands(const GitCmd::Vec_t& commands, const wxString& workingDir);
    bool
    DoExecuteCommandSync(const wxString& command, wxString* commandOutput, const wxString& workingDir = wxEmptyString);

    void DoSetTreeItemImage(clTreeCtrl* ctrl, const wxTreeItemId& item, OverlayTool::BmpType bmpType) const;
    void InitDefaults();
    void AddDefaultActions();
    void LoadDefaultGitCommands(GitEntry& data, bool overwrite = false);
    void ProcessGitActionQueue();
    void ColourFileTree(clTreeCtrl* tree, const wxStringSet_t& files, OverlayTool::BmpType bmpType) const;
    void CreateFilesTreeIDsMap(std::map<wxString, wxTreeItemId>& IDs, bool ifmodified = false) const;
    void DoShowCommitDialog(const wxString& diff, wxString& commitArgs);
    void DoRefreshView(bool ensureVisible);

    /// Workspace management
    bool IsWorkspaceOpened() const;
    wxString GetCommitMessageFile() const;
    wxString FindRepositoryRoot(const wxString& starting_dir) const;
    void FinishGitListAction(const gitAction& ga);
    void ListBranchAction(const gitAction& ga);
    void GetCurrentBranchAction(const gitAction& ga);
    void UpdateFileTree();

    void ShowProgress(const wxString& message);
    void HideProgress();
    void DoCleanup();
    void DoAddFiles(const wxArrayString& files);
    void DoResetFiles(const wxArrayString& files);
    void DoShowDiffsForFiles(const wxArrayString& files, bool useFileAsBase = false);
    void DoSetRepoPath(const wxString& repo_path = wxEmptyString);
    void DoRecoverFromGitCommandError(bool clear_queue = true);
    void DoLoadBlameInfo(bool clearCache);
    void DoUpdateBlameInfo(const wxString& info, const wxString& fullpath);
    void DoAnyFileModified();
    DECLARE_EVENT_TABLE()

    // Event handlers
    void OnProgressTimer(wxTimerEvent& Event);
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);
    void OnFileMenu(clContextMenuEvent& event);
    void OnFolderMenu(clContextMenuEvent& event);

    void OnSideBarPageChanged(clCommandEvent& event);

    void OnFileCreated(clFileSystemEvent& event);
    void OnReplaceInFiles(clFileSystemEvent& event);
    void OnEditorChanged(wxCommandEvent& event);
    void OnFileSaved(clCommandEvent& e);
    void OnFileModifiedExternally(clFileSystemEvent& e);
    void OnFilesAddedToProject(clCommandEvent& e);
    void OnFilesRemovedFromProject(clCommandEvent& e);
    void OnWorkspaceLoaded(clWorkspaceEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& e);
    void OnWorkspaceConfigurationChanged(wxCommandEvent& e);
    void OnMainFrameTitle(clCommandEvent& e);
    void OnSettings(wxCommandEvent& e);
    void OnFileDiffSelected(wxCommandEvent& e);
    void OnFileResetSelected(wxCommandEvent& e);
    void OnFileAddSelected(wxCommandEvent& e);
    void OnSwitchLocalBranch(wxCommandEvent& e);
    void OnSwitchRemoteBranch(wxCommandEvent& e);
    void OnCreateBranch(wxCommandEvent& e);
    void OnCommit(wxCommandEvent& e);
    void OnCommitList(wxCommandEvent& e);
    void OnShowDiffs(wxCommandEvent& e);
    void OnApplyPatch(wxCommandEvent& e);
    void OnPush(wxCommandEvent& e);
    void OnRebase(wxCommandEvent& e);
    void OnPull(wxCommandEvent& e);
    void OnResetRepository(wxCommandEvent& e);
    void OnStartGitk(wxCommandEvent& e);
    void OnStartGitkUI(wxUpdateUIEvent& e);
    void OnListModified(wxCommandEvent& e);
    void OnGitBlame(wxCommandEvent& e);
    void OnRefresh(wxCommandEvent& e);
    void OnGarbageColletion(wxCommandEvent& e);
    void OnOpenMSYSGit(wxCommandEvent& e);
    void OnActiveProjectChanged(clProjectSettingsEvent& event);
    void OnFileCommitListSelected(wxCommandEvent& e);
    void OnFileGitBlame(wxCommandEvent& event);
    void OnAppActivated(wxCommandEvent& event);
    void OnUpdateNavBar(clCodeCompletionEvent& event);
    void OnEditorClosed(wxCommandEvent& event);
    void OnEnableGitRepoExists(wxUpdateUIEvent& e);
    void OnClone(wxCommandEvent& e);

    // Event handlers from folder context menu
    void OnFolderPullRebase(wxCommandEvent& event);
    void OnFolderCommit(wxCommandEvent& event);
    void OnFolderPush(wxCommandEvent& event);
    void OnFolderStash(wxCommandEvent& event);
    void OnFolderStashPop(wxCommandEvent& event);
    void OnFolderGitBash(wxCommandEvent& event);

    // Respond to local events
    void OnGitActionDone(clSourceControlEvent& event);
    bool HandleErrorsOnRemoteRepo(const wxString& output) const;

    // Remote callbacks
    void OnFindPath(clCommandEvent& event);
    void OpenURLInBrowser(const wxString& url);

public:
    GitPlugin(IManager* manager);
    ~GitPlugin() override = default;

    const wxString& GetRepositoryPath() const { return m_repositoryDirectory; }
    void WorkspaceClosed();

    bool IsRemoteWorkspace() const { return m_isRemoteWorkspace; }

    /**
     * @brief create git process and return the process handle
     */
    IProcess* AsyncRunGit(wxEvtHandler* handler,
                          const wxString& git_args,
                          size_t create_flags,
                          const wxString& working_directory,
                          bool logMessage = false);
    /**
     * @brief create a git process and direct the output to a callback
     */
    void AsyncRunGitWithCallback(const wxString& git_args,
                                 std::function<void(const wxString&)> callback,
                                 size_t create_flags,
                                 const wxString& working_directory,
                                 bool logMessage = false);
    /**
     * @brief is git enabled for the current workspace?
     */
    bool IsGitEnabled() const;

    /**
     * @brief open a file. The path is relative to the workspace location
     */
    IEditor* OpenFile(const wxString& relativePathFile);

    /**
     * @brief fetch the next 100 commits (skip 'skip' first commits)
     * and show them in the commit list dialog
     * @param skip number of first commits to skip
     */
    void FetchNextCommits(int skip, const wxString& args);

    GitConsole* GetConsole() { return m_console; }
    IProcess* GetProcess() { return m_process; }
    clCommandProcessor* GetFolderProcess() { return m_commandProcessor; }

    IManager* GetManager() { return m_mgr; }

    void ShowDiff(const wxArrayString& files) { DoShowDiffsForFiles(files); }
    void ShowExternalDiff(const wxString& file, const wxString& tool);

    void ApplyPatch(const wxString& filename, const wxString& extraFlags);
    void AddFiles(const wxArrayString& files) { DoAddFiles(files); }

    void RevertCommit(const wxString& commitId);

    void ResetFiles(const wxArrayString& files) { DoResetFiles(files); }

    void UndoAddFiles(const wxArrayString& files);

    void RefreshFileListView();
    /**
     * @brief display a message in the console
     * used by outside classes e.g. GitDiffDlg
     * @param message to display
     */
    void DisplayMessage(const wxString& message) const;

    void DoGitBlame(const wxString& args);      // Called by OnGitBlame or the git blame dialog
    wxString GetEditorRelativeFilepath() const; // Called by OnGitBlame or the git blame dialog

    /**
     * @brief simple git command executioin completed. Display its output etc
     */
    void OnCommandOutput(clCommandEvent& event);
    void OnCommandEnded(clCommandEvent& event);

    bool GenerateCommitMessage(const wxString& prompt);

    /// Return the commit log between range of commits. We split the log (line based), to match the `chunk_size`
    /// argument.
    clStatusOr<wxArrayString> FetchLogBetweenCommits(const wxString& start_commit,
                                                     const wxString& end_commit,
                                                     bool oneline = true,
                                                     size_t chunk_size = wxString::npos);

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    void CreateToolBar(clToolBarGeneric* toolbar) override;
    void CreatePluginMenu(wxMenu* pluginsMenu) override;
    void HookPopupMenu(wxMenu* menu, MenuType type) override;
    void UnPlug() override;
};

#endif // git
