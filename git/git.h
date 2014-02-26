//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __git__
#define __git__

#include <wx/progdlg.h>

#include "plugin.h"
#include "asyncprocess.h"
#include "processreaderthread.h"
#include <queue>
#include <set>
#include <wx/progdlg.h>
#include "project.h" // wxStringSet_t
#include <map>
#include "overlaytool.h"
#include "cl_command_event.h"
#include "gitentry.h"

class gitAction
{
public:
    int action;
    wxString arguments;
    wxString workingDirectory;

public:
    gitAction() : action(0), arguments(""), workingDirectory("") {}
    gitAction(int act, const wxString &args) : action(act), arguments(args) {}
    ~gitAction() {}
};
class GitConsole;
class GitCommitListDlg;

class GitPlugin : public IPlugin
{
    typedef std::map<int, int> IntMap_t;
    enum {
        gitNone = 0,
        gitUpdateRemotes,
        gitListAll,
        gitListModified,
        gitListRemotes,
        gitAddFile,
        gitDeleteFile,
        gitDiffFile,
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
    };

    wxArrayString           m_localBranchList;
    wxArrayString           m_remoteBranchList;
    wxStringSet_t           m_trackedFiles;
    wxStringSet_t           m_modifiedFiles;
    bool                    m_addedFiles;
    wxArrayString           m_remotes;
    wxColour                m_colourTrackedFile;
    wxColour                m_colourDiffFile;
    wxString                m_pathGITExecutable;
    wxString                m_pathGITKExecutable;
    wxString                m_repositoryDirectory;
    wxString                m_currentBranch;
    std::queue<gitAction>   m_gitActionQueue;
    wxTimer                 m_progressTimer;
    wxString                m_progressMessage;
    wxString                m_commandOutput;
    bool                    m_bActionRequiresTreUpdate;
    IProcess *              m_process;
    wxEvtHandler *          m_eventHandler;
    wxWindow *              m_topWindow;
    clToolBar*              m_pluginToolbar;
    wxMenu*                 m_pluginMenu;
    GitImages               m_images;
    IntMap_t                m_treeImageMapping;
    int                     m_baseImageCount;
    GitConsole *            m_console;
    wxFileName              m_workspaceFilename;
    GitCommitListDlg *      m_commitListDlg;
    
private:
    void DoCreateTreeImages();
    void DoShowDiffViewer(const wxString &headFile, const wxString& fileName);
    
    void DoSetTreeItemImage(wxTreeCtrl* ctrl, const wxTreeItemId& item, OverlayTool::BmpType bmpType ) const;
    void InitDefaults();
    void AddDefaultActions();
    void LoadDefaultGitCommands(GitEntry& data, bool overwrite = false);
    void ProcessGitActionQueue(const wxString& commandString = "");
    void ColourFileTree(wxTreeCtrl *tree, const wxStringSet_t& files, OverlayTool::BmpType bmpType) const;
    void CreateFilesTreeIDsMap(std::map<wxString, wxTreeItemId>& IDs, bool ifmodified = false) const;
    
    /// Workspace management
    bool IsWorkspaceOpened() const;
    wxString GetWorkspaceName() const;
    wxFileName GetWorkspaceFileName() const;
    
    void FinishGitListAction(const gitAction& ga);
    void ListBranchAction(const gitAction& ga);
    void GetCurrentBranchAction(const gitAction& ga);
    void UpdateFileTree();

    void ShowProgress(const wxString& message, bool pulse = true);
    void HideProgress();
    void DoCleanup();
    void DoAddFiles(const wxArrayString &files);
    void DoResetFiles(const wxArrayString &files);
    void DoGetFileViewSelectedFiles( wxArrayString &files, bool relativeToRepo );
    void DoShowDiffsForFiles(const wxArrayString &files);
    
    DECLARE_EVENT_TABLE()
    
    // Event handlers
    void OnInitDone(wxCommandEvent& e);
    void OnProgressTimer(wxTimerEvent& Event);
    void OnProcessTerminated(wxCommandEvent &event);
    void OnProcessOutput(wxCommandEvent &event);

    void OnFileSaved(clCommandEvent& e);
    void OnFilesAddedToProject(clCommandEvent& e);
    void OnFilesRemovedFromProject(clCommandEvent& e);
    void OnWorkspaceLoaded(wxCommandEvent& e);
    void OnWorkspaceClosed(wxCommandEvent &e);
    void OnWorkspaceConfigurationChanged(wxCommandEvent &e);
    void OnSetGitRepoPath(wxCommandEvent &e);
    void OnSettings(wxCommandEvent &e);
    void OnFileDiffSelected(wxCommandEvent &e);
    void OnFileResetSelected(wxCommandEvent &e);
    void OnFileAddSelected(wxCommandEvent &e);
    void OnFileDeleteSelected(wxCommandEvent &e);
    void OnSwitchLocalBranch(wxCommandEvent &e);
    void OnSwitchRemoteBranch(wxCommandEvent &e);
    void OnCreateBranch(wxCommandEvent &e);
    void OnCommit(wxCommandEvent &e);
    void OnCommitList(wxCommandEvent& e);
    void OnShowDiffs(wxCommandEvent& e);
    void OnApplyPatch(wxCommandEvent& e);
    void OnPush(wxCommandEvent &e);
    void OnPull(wxCommandEvent &e);
    void OnResetRepository(wxCommandEvent &e);
    void OnStartGitk(wxCommandEvent& e);
    void OnStartGitkUI(wxUpdateUIEvent& e);
    void OnListModified(wxCommandEvent& e);
    void OnRefresh(wxCommandEvent& e);
    void OnGarbageColletion(wxCommandEvent& e);
#if 0
    void OnBisectStart(wxCommandEvent& e);
    void OnBisectGood(wxCommandEvent& e);
    void OnBisectBad(wxCommandEvent& e);
    void OnBisectReset(wxCommandEvent& e);
#endif
    void OnEnableGitRepoExists(wxUpdateUIEvent &e);
    void OnClone(wxCommandEvent &e);

public:
    GitPlugin(IManager *manager);
    ~GitPlugin();

    const wxString& GetRepositoryDirectory() const {
        return m_repositoryDirectory;
    }
    IProcess* GetProcess() {
        return m_process;
    }

    IManager* GetManager() {
        return m_mgr;
    }
    
    void ShowDiff(const wxArrayString &files) {
        DoShowDiffsForFiles(files);
    }
    
    void ApplyPatch(const wxString &filename, const wxString &extraFlags);
    void AddFiles(const wxArrayString &files) {
        DoAddFiles(files);
    }
    
    void RevertCommit(const wxString &commitId);
    
    void ResetFiles(const wxArrayString &files) {
        DoResetFiles(files);
    }

    void UndoAddFiles(const wxArrayString& files);

    void RefreshFileListView();

    //--------------------------------------------
    //Abstract methods
    //--------------------------------------------
    virtual clToolBar *CreateToolBar(wxWindow *parent);
    virtual void CreatePluginMenu(wxMenu *pluginsMenu);
    virtual void HookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnPlug();
};

#endif //git
