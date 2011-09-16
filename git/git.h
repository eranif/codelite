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
#include "progress_dialog.h"

typedef struct gitAction {
	int action;
	wxString arguments;
} gitAction;

class GitPlugin : public IPlugin
{
	enum {
		gitNone = 0,
		gitListAll,
		gitListModified,
		gitListRemotes,
		gitAddFile,
		gitDeleteFile,
		gitDiffFile,
		gitDiffRepoCommit,
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
	};

	wxArrayString m_localBranchList;
	wxArrayString m_remoteBranchList;
	wxArrayString m_trackedFiles;
	wxArrayString m_modifiedFiles;
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
	clProgressDlg* m_progressDialog;
	wxString m_progressMessage;
	wxString m_commandOutput;
	bool m_bActionRequiresTreUpdate;
	IProcess * m_process;
	wxEvtHandler *m_topWindow;

	clToolBar* m_pluginToolbar;
	wxMenu* m_pluginMenu;

private:
	void InitDefaults();
	void AddDefaultActions();
	void ProcessGitActionQueue();
	void ColourFileTree(wxTreeCtrl *tree, const wxArrayString& files, const wxColour& colour) const;
	void CreateFilesTreeIDsMap(std::map<wxString, wxTreeItemId>& IDs, bool ifmodified = false) const;

	void FinishGitListAction(const gitAction& ga);
	void ListBranchAction(const gitAction& ga);
	void GetCurrentBranchAction(const gitAction& ga);
	void UpdateFileTree();

	void ShowProgress(const wxString& message, bool pulse = true);
	void HideProgress();
	void DoCleanup();
	
	DECLARE_EVENT_TABLE();
	// Event handlers
	void OnProgressTimer(wxTimerEvent& Event);
	void OnProcessTerminated(wxCommandEvent &event);
	void OnProcessOutput(wxCommandEvent &event);

	void OnFileSaved(wxCommandEvent& e);
	void OnFilesAddedToProject(wxCommandEvent& e);
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
	void OnPush(wxCommandEvent &e);
	void OnPull(wxCommandEvent &e);
	void OnResetRepository(wxCommandEvent &e);
	void OnStartGitk(wxCommandEvent& e);
	void OnListModified(wxCommandEvent& e);
	void OnRefresh(wxCommandEvent& e);
	void OnGarbageColletion(wxCommandEvent& e);
	void OnEnableGitRepoExists(wxUpdateUIEvent &e);
	
public:
	GitPlugin(IManager *manager);
	~GitPlugin();

	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual clToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
};

#endif //git
