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
class GitPlugin : public IPlugin
{
    typedef std::map<int, int> IntMap_t;

    enum {
        gitNone = 0,
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
#if 0
        gitBisectStart,
        gitBisectGood,
        gitBisectBad,
        gitBisectReset,
#endif
    };

    wxArrayString m_localBranchList;
    wxArrayString m_remoteBranchList;
    wxStringSet_t  m_trackedFiles;
    wxStringSet_t  m_modifiedFiles;
    bool m_addedFiles;
    wxArrayString m_remotes;

    wxColour m_colourTrackedFile;
    wxColour m_colourDiffFile;
    wxString m_pathGITExecutable;
    wxString m_pathGITKExecutable;
    wxString m_repositoryDirectory;
    wxString m_currentBranch;
    std::queue<gitAction> m_gitActionQueue;

    wxTimer m_progressTimer;
    wxProgressDialog* m_progressDialog;
    wxString m_progressMessage;
    wxString m_commandOutput;
    bool m_bActionRequiresTreUpdate;
    IProcess * m_process;
    wxEvtHandler *m_eventHandler;
    wxWindow *m_topWindow;

    clToolBar* m_pluginToolbar;
    wxMenu* m_pluginMenu;
    GitImages m_images;
    IntMap_t m_treeImageMapping;
    int      m_baseImageCount;
    GitConsole *m_console;

private:
    void DoCreateTreeImages();
    void DoSetTreeItemImage(wxTreeCtrl* ctrl, const wxTreeItemId& item, OverlayTool::BmpType bmpType ) const;
    void InitDefaults();
    void AddDefaultActions();
    void ProcessGitActionQueue();
    void ColourFileTree(wxTreeCtrl *tree, const wxStringSet_t& files, OverlayTool::BmpType bmpType) const;
    void CreateFilesTreeIDsMap(std::map<wxString, wxTreeItemId>& IDs, bool ifmodified = false) const;

    void FinishGitListAction(const gitAction& ga);
    void ListBranchAction(const gitAction& ga);
    void GetCurrentBranchAction(const gitAction& ga);
    void UpdateFileTree();

    void ShowProgress(const wxString& message, bool pulse = true);
    void HideProgress();
    void DoCleanup();
    void DoAddFiles(const wxArrayString &files);
    void DoResetFiles(const wxArrayString &files);
    
    DECLARE_EVENT_TABLE();
    // Event handlers
    void OnInitDone(wxCommandEvent& e);
    void OnProgressTimer(wxTimerEvent& Event);
    void OnProcessTerminated(wxCommandEvent &event);
    void OnProcessOutput(wxCommandEvent &event);

    void OnFileSaved(wxCommandEvent& e);
    void OnFilesAddedToProject(wxCommandEvent& e);
    void OnFilesRemovedFromProject(wxCommandEvent& e);
    void OnWorkspaceLoaded(wxCommandEvent& e);
    void OnWorkspaceClosed(wxCommandEvent &e);
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
    void OnPush(wxCommandEvent &e);
    void OnPull(wxCommandEvent &e);
    void OnResetRepository(wxCommandEvent &e);
    void OnStartGitk(wxCommandEvent& e);
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

    IProcess* GetProcess() {
        return m_process;
    }
    
    IManager* GetManager() {
        return m_mgr;
    }
    
    void AddFiles(const wxArrayString &files) {
        DoAddFiles(files);
    }
    
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
