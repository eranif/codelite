
//////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include <wx/tokenzr.h>
#include <wx/file.h>
#include <wx/xrc/xmlres.h>
#include <stack>

#include "globals.h"

#include "virtualdirtreectrl.h"
#include "workspace.h"

#include "gitentry.h"
#include "gitCommitDlg.h"
#include "gitCommitListDlg.h"
#include "gitDiffDlg.h"
#include "gitLogDlg.h"
#include "gitSettingsDlg.h"
#include "git.h"

//#include "icons/menuadd.xpm"
#include "icons/giggle.xpm"
#include "icons/menubranch.xpm"
#include "icons/menucheckout.xpm"
#include "icons/menucleanup.xpm"
#include "icons/menucommit.xpm"
#include "icons/menudelete.xpm"
#include "icons/menudiff.xpm"
#include "icons/menuexport.xpm"
#include "icons/menuimport.xpm"
#include "icons/menulog.xpm"
#include "icons/menuswitch.xpm"
#include "icons/icon_git.xpm"

static GitPlugin* thePlugin = NULL;

#ifdef __WXMSW__
#    define XPM_BITMAP(name) wxBitmap(name##_xpm, wxBITMAP_TYPE_XPM)
#else
#    define XPM_BITMAP(name) wxBitmap( (const char**) name##_xpm )
#endif

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new GitPlugin(manager);
	}
	return thePlugin;
}
/*******************************************************************************/
extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxT("René Kraus"));
	info.SetName(wxT("git"));
	info.SetDescription(wxT("Simple GIT plugin"));
	info.SetVersion(wxT("v1.0.3"));
	return info;
}
/*******************************************************************************/
extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

BEGIN_EVENT_TABLE(GitPlugin, wxEvtHandler)
	EVT_TIMER(wxID_ANY, GitPlugin::OnProgressTimer)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  GitPlugin::OnProcessOutput)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, GitPlugin::OnProcessTerminated)
END_EVENT_TABLE()

GitPlugin::GitPlugin(IManager *manager)
	: IPlugin(manager)
	, m_colourTrackedFile(wxT("DARK GREEN"))
	, m_colourDiffFile(wxT("MAROON"))
#ifdef __WXGTK__
	, m_pathGITExecutable(_("/usr/bin/git"))
	, m_pathGITKExecutable(_("/usr/bin/gitk"))
#else
	, m_pathGITExecutable(_("git"))
	, m_pathGITKExecutable(_("gitk"))
#endif
	, m_progressDialog(NULL)
	, m_bActionRequiresTreUpdate(false)
	, m_process(NULL)
	, m_topWindow(NULL)
	, m_pluginToolbar(NULL)
	, m_pluginMenu(NULL)
{
	m_longName = wxT("GIT plugin");
	m_shortName = wxT("git");
	m_topWindow = m_mgr->GetTheApp();

	m_topWindow->Connect( wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(GitPlugin::OnWorkspaceLoaded), NULL, this);
	m_topWindow->Connect( wxEVT_FILE_SAVED, wxCommandEventHandler(GitPlugin::OnFileSaved), NULL, this);
	m_topWindow->Connect( wxEVT_PROJ_FILE_ADDED, wxCommandEventHandler(GitPlugin::OnFilesAddedToProject), NULL, this);

	m_progressTimer.SetOwner(this);
}
/*******************************************************************************/
GitPlugin::~GitPlugin()
{
	delete m_progressDialog;
}
/*******************************************************************************/
clToolBar *GitPlugin::CreateToolBar(wxWindow *parent)
{
	wxUnusedVar(parent);

	if(m_mgr->AllowToolbar()) {
		int size = m_mgr->GetToolbarIconSize();
		m_pluginToolbar = new clToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);
		m_pluginToolbar->SetToolBitmapSize(wxSize(size,size));
		m_pluginToolbar->AddTool(ID_PULL, wxT("Pull"), XPM_BITMAP(menucheckout), wxT("Pull remote changes"));
		m_pluginToolbar->AddSeparator();
		m_pluginToolbar->AddTool(ID_COMMIT, wxT("Commit"), XPM_BITMAP(menucommit), wxT("Commit local changes"));
		m_pluginToolbar->AddTool(ID_PUSH, wxT("Push"), XPM_BITMAP(menuimport), wxT("Push local changes"));
		m_pluginToolbar->AddSeparator();
		m_pluginToolbar->AddTool(ID_RESET_REPO, wxT("Reset"), XPM_BITMAP(menucleanup), wxT("Reset repository"));
		m_pluginToolbar->AddSeparator();
		m_pluginToolbar->AddTool(ID_CREATE_BRANCH, wxT("Create branch"), XPM_BITMAP(menubranch), wxT("Create local branch"));
		m_pluginToolbar->AddTool(ID_SWITCH_BRANCH, wxT("Local branch"), XPM_BITMAP(menuswitch), wxT("Switch to local branch"));
		m_pluginToolbar->AddTool(ID_SWITCH_BRANCH_REMOTE, wxT("Remote branch"), XPM_BITMAP(menuexport), wxT("Init and switch to remote branch"));
		m_pluginToolbar->AddSeparator();
		m_pluginToolbar->AddTool(ID_COMMIT_LIST, wxT("Log"), XPM_BITMAP(menulog), wxT("Browse commit history"));
		m_pluginToolbar->AddTool(ID_GITK, wxT("gitk"), XPM_BITMAP(giggle), wxT("Start gitk"));
		m_pluginToolbar->Realize();

		EnableMenuAndToolBar(false);
		return m_pluginToolbar;
	}
	return NULL;
}
/*******************************************************************************/
void GitPlugin::CreatePluginMenu(wxMenu *pluginsMenu)
{
	m_pluginMenu = new wxMenu();
	wxMenuItem *item( NULL );

	item = new wxMenuItem( m_pluginMenu, ID_PULL, wxT( "Pull remote changes"));
	item->SetBitmap(XPM_BITMAP(menucheckout));
	m_pluginMenu->Append( item );
	m_pluginMenu->AppendSeparator();

	item = new wxMenuItem( m_pluginMenu, ID_COMMIT, wxT( "Commit local changes"));
	item->SetBitmap(XPM_BITMAP(menucommit));
	m_pluginMenu->Append( item );
	item = new wxMenuItem( m_pluginMenu, ID_PUSH, wxT( "Push local commits"));
	item->SetBitmap(XPM_BITMAP(menuimport));
	m_pluginMenu->Append( item );
	m_pluginMenu->AppendSeparator();

	item = new wxMenuItem( m_pluginMenu, ID_RESET_REPO, wxT( "Reset current repository" ));
	item->SetBitmap(XPM_BITMAP(menucleanup));
	m_pluginMenu->Append( item );
	m_pluginMenu->AppendSeparator();
	item = new wxMenuItem( m_pluginMenu, ID_CREATE_BRANCH, wxT( "Create local branch" ) );
	item->SetBitmap(XPM_BITMAP(menubranch));
	m_pluginMenu->Append( item );
	item = new wxMenuItem( m_pluginMenu, ID_SWITCH_BRANCH, wxT( "Switch local branch" ), wxT( "Switch local branch" ), wxITEM_NORMAL );
	item->SetBitmap(XPM_BITMAP(menuswitch));
	m_pluginMenu->Append( item );

	item = new wxMenuItem( m_pluginMenu, ID_SWITCH_BRANCH_REMOTE, wxT( "Switch remote branch" ), wxT( "Switch remote branch" ), wxITEM_NORMAL );
	item->SetBitmap(XPM_BITMAP(menuexport));
	m_pluginMenu->Append( item );

	m_pluginMenu->AppendSeparator();
	item = new wxMenuItem( m_pluginMenu, ID_COMMIT_LIST, wxT( "List commits" ), wxT( "List commits" ), wxITEM_NORMAL );
	item->SetBitmap(XPM_BITMAP(giggle));
	m_pluginMenu->Append( item );
	m_pluginMenu->AppendSeparator();
	item = new wxMenuItem( m_pluginMenu, ID_LIST_MODIFIED, wxT( "List modified files" ), wxT( "List modified files" ), wxITEM_NORMAL );
	m_pluginMenu->Append( item );
	item = new wxMenuItem( m_pluginMenu, ID_GITK, wxT( "Start gitk" ), wxT( "Start gitk" ), wxITEM_NORMAL );
	item->SetBitmap(XPM_BITMAP(giggle));
	m_pluginMenu->Append( item );

	m_pluginMenu->AppendSeparator();
	item = new wxMenuItem( m_pluginMenu, ID_GARBAGE_COLLETION, wxT( "Clean git database (garbage collection)" ));
	m_pluginMenu->Append( item );
	item = new wxMenuItem( m_pluginMenu, ID_REFRESH, wxT( "Refresh git file list" ), wxT( "Refresh file lists" ), wxITEM_NORMAL );
	m_pluginMenu->Append( item );
	m_pluginMenu->AppendSeparator();
	item = new wxMenuItem( m_pluginMenu, ID_SETTINGS, wxT( "GIT plugin settings" ), wxT( "Set GIT executable path" ), wxITEM_NORMAL );
	m_pluginMenu->Append( item );
	item = new wxMenuItem( m_pluginMenu, ID_SETGITREPO, wxT( "Set GIT repository path" ), wxT( "Set GIT repository path" ), wxITEM_NORMAL );
	item->SetBitmap(XPM_BITMAP(menulog));
	m_pluginMenu->Append( item );

	item = new wxMenuItem(pluginsMenu, wxID_ANY, wxT("Git"));
	item->SetSubMenu( m_pluginMenu);
	item->SetBitmap(XPM_BITMAP(icon_git));
	pluginsMenu->Append(item);

	m_topWindow->Connect( ID_SETGITREPO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnSetGitRepoPath ), NULL, this );
	m_topWindow->Connect( ID_SETTINGS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnSettings ), NULL, this );
	m_topWindow->Connect( ID_SWITCH_BRANCH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnSwitchLocalBranch ), NULL, this );
	m_topWindow->Connect( ID_SWITCH_BRANCH_REMOTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnSwitchRemoteBranch ), NULL, this );
	m_topWindow->Connect( ID_CREATE_BRANCH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnCreateBranch ), NULL, this );
	m_topWindow->Connect( ID_PULL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnPull ), NULL, this );
	m_topWindow->Connect( ID_COMMIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnCommit ), NULL, this );
	m_topWindow->Connect( ID_COMMIT_LIST, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnCommitList ), NULL, this );
	m_topWindow->Connect( ID_PUSH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnPush ), NULL, this );
	m_topWindow->Connect( ID_RESET_REPO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnResetRepository ), NULL, this );
	m_topWindow->Connect( ID_GITK, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnStartGitk ), NULL, this );
	m_topWindow->Connect( ID_LIST_MODIFIED, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnListModified ), NULL, this );
	m_topWindow->Connect( ID_REFRESH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnRefresh ), NULL, this );
	m_topWindow->Connect( ID_GARBAGE_COLLETION, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnGarbageColletion ), NULL, this );

	EnableMenuAndToolBar(false);
}
/*******************************************************************************/
void GitPlugin::HookPopupMenu(wxMenu *menu, MenuType type)
{
	if(type == MenuTypeFileView_File) {
		menu->AppendSeparator();
		wxMenuItem* item = new wxMenuItem(menu, ID_ADD_FILE,wxT("Git: Add file"));
		item->SetBitmap(XPM_BITMAP(menudelete));
		menu->Append(item);
		m_topWindow->Connect( ID_ADD_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnFileAddSelected), NULL, this );

		//item = new wxMenuItem(menu, ID_DELETE_FILE,wxT("Git: Delete file"));
		//item->SetBitmap(XPM_BITMAP(menudelete));
		//menu->Append(item);
		//m_topWindow->Connect( ID_DELETE_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnFileDeleteSelected), NULL, this );

		item = new wxMenuItem(menu, ID_RESET_FILE,wxT("Gi:t Reset file"));
		item->SetBitmap(XPM_BITMAP(menudelete));
		menu->Append(item);
		m_topWindow->Connect( ID_RESET_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnFileResetSelected), NULL, this );

		item = new wxMenuItem(menu, ID_DIFF_FILE,wxT("Git: Show file diff"));
		item->SetBitmap(XPM_BITMAP(menudiff));
		menu->Append(item);
		m_topWindow->Connect( ID_DIFF_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnFileDiffSelected), NULL, this );

	}
}
/*******************************************************************************/
void GitPlugin::UnPlug()
{
	/*MENU*/
	m_topWindow->Disconnect( ID_SETGITREPO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnSetGitRepoPath ), NULL, this );
	m_topWindow->Disconnect( ID_SETTINGS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnSettings ), NULL, this );
	m_topWindow->Disconnect( ID_SWITCH_BRANCH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnSwitchLocalBranch ), NULL, this );
	m_topWindow->Disconnect( ID_SWITCH_BRANCH_REMOTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnSwitchRemoteBranch ), NULL, this);
	m_topWindow->Disconnect( ID_CREATE_BRANCH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnCreateBranch ), NULL, this );
	m_topWindow->Disconnect( ID_PULL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnPull ), NULL, this );
	m_topWindow->Disconnect( ID_COMMIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnCommit ), NULL, this );
	m_topWindow->Disconnect( ID_COMMIT_LIST, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnCommitList ), NULL, this );
	m_topWindow->Disconnect( ID_PUSH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnPush ), NULL, this );
	m_topWindow->Disconnect( ID_RESET_REPO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnResetRepository ), NULL, this );
	m_topWindow->Disconnect( ID_GITK, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnStartGitk ), NULL, this );
	m_topWindow->Disconnect( ID_LIST_MODIFIED, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnListModified ), NULL, this );
	m_topWindow->Disconnect( ID_REFRESH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnRefresh ), NULL, this );
	m_topWindow->Disconnect( ID_GARBAGE_COLLETION, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnGarbageColletion ), NULL, this );
	/*SYSTEM*/
	m_topWindow->Disconnect( wxEVT_FILE_SAVED, wxCommandEventHandler(GitPlugin::OnFileSaved), NULL, this);
	m_topWindow->Disconnect( wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(GitPlugin::OnWorkspaceLoaded), NULL, this);
	m_topWindow->Disconnect( wxEVT_PROJ_FILE_ADDED, wxCommandEventHandler(GitPlugin::OnFilesAddedToProject), NULL, this);
	/*Context Menu*/
	m_topWindow->Disconnect( ID_ADD_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnFileAddSelected), NULL, this );
	//m_topWindow->Disconnect( ID_DELETE_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnFileDeleteSelected), NULL, this );
	m_topWindow->Disconnect( ID_RESET_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnFileResetSelected), NULL, this );
	m_topWindow->Disconnect( ID_DIFF_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnFileDiffSelected), NULL, this );
}
/*******************************************************************************/
void GitPlugin::OnSetGitRepoPath(wxCommandEvent& e)
{
	wxUnusedVar(e);
	if(!m_mgr->GetWorkspace()) {
		wxMessageBox(_("No active workspace found!\n"
						"Setting a repository path relies on an sctive workspace."),
		             _("Missing workspace"),
		             wxICON_ERROR);
		return;
	}

	wxString workspaceName = m_mgr->GetWorkspace()->GetName();
	
	// use the current repository as the starting path 
	// if current repository is empty, use the current workspace path
	wxString startPath = m_repositoryDirectory;
	if(startPath.IsEmpty()) {
		startPath = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath();
	}
	
	const wxString& dir = wxDirSelector(wxT("Select git root directory for this workspace"), startPath);
	if (m_repositoryDirectory != dir ) {
		m_repositoryDirectory = dir;

		GitEntry data;
		m_mgr->GetConfigTool()->ReadObject(wxT("GitData"), &data);
		data.SetEntry(workspaceName, dir);
		m_mgr->GetConfigTool()->WriteObject(wxT("GitData"), &data);

		if(!dir.IsEmpty()) {
			EnableMenuAndToolBar(true);
			AddDefaultActions();
			ProcessGitActionQueue();
		} else {
			EnableMenuAndToolBar(false);
		}
	}
}
/*******************************************************************************/
void GitPlugin::OnSettings(wxCommandEvent &e)
{
	GitSettingsDlg dlg(m_mgr->GetTheApp()->GetTopWindow(),
	                   m_colourTrackedFile, m_colourDiffFile,
	                   m_pathGITExecutable, m_pathGITKExecutable);
	if(dlg.ShowModal() == wxID_OK) {
		GitEntry data;
		m_mgr->GetConfigTool()->ReadObject(wxT("GitData"), &data);
		data.SetTrackedFileColour(dlg.GetTrackedFileColour());
		data.SetDiffFileColour(dlg.GetDiffFileColour());
		data.SetGITExecutablePath(dlg.GetGITExecutablePath());
		data.SetGITKExecutablePath(dlg.GetGITKExecutablePath());
		m_mgr->GetConfigTool()->WriteObject(wxT("GitData"), &data);
		m_colourTrackedFile = dlg.GetTrackedFileColour();
		m_colourDiffFile = dlg.GetDiffFileColour();
		m_pathGITExecutable = dlg.GetGITExecutablePath();
		m_pathGITKExecutable = dlg.GetGITKExecutablePath();

		AddDefaultActions();
		ProcessGitActionQueue();
	}
}
/*******************************************************************************/
void GitPlugin::OnFileAddSelected(wxCommandEvent &e)
{
	wxUnusedVar(e);
	TreeItemInfo info = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
	wxString path = info.m_fileName.GetFullPath();
	if(m_trackedFiles.Index(path) != wxNOT_FOUND) {
		wxMessageBox(wxT("File is already part of the index..."));
		return;
	}
	m_addedFiles = true;
	path.Replace(m_repositoryDirectory,wxT(""));
	if(path.StartsWith(wxT("/")))
		path.Remove(0,1);
	gitAction ga = {gitAddFile,path};
	m_gitActionQueue.push(ga);
	AddDefaultActions();
	ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnFileDeleteSelected(wxCommandEvent &e)
{
	//Experimental
	/*
	wxUnusedVar(e);
	TreeItemInfo info = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
	wxString path = info.m_fileName.GetFullPath();
	if(wxMessageBox(wxT("Really delete file ")+path+wxT(" from the index (not from disk)?")
	                , wxT("Confirm file deletion"), wxYES_NO) == wxNO)
	  return;
	path.Replace(m_repositoryDirectory,wxT(""));
	if(path.StartsWith(wxT("/")))
	  path.Remove(0,1);
	gitAction ga = {gitDeleteFile,path};
	m_gitActionQueue.push(ga);
	AddDefaultActions();
	ProcessGitActionQueue();
	*/
}
/*******************************************************************************/
void GitPlugin::OnFileDiffSelected(wxCommandEvent &e)
{
	wxUnusedVar(e);
	TreeItemInfo info = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
	wxString path = info.m_fileName.GetFullPath();
	if(m_modifiedFiles.Index(path) == wxNOT_FOUND) {
		wxMessageBox(wxT("File is not modified, there is no diff..."));
		return;
	}
	path.Replace(m_repositoryDirectory,wxT(""));
	if(path.StartsWith(wxT("/")))
		path.Remove(0,1);
	gitAction ga = {gitDiffFile,path};
	m_gitActionQueue.push(ga);
	ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnFileResetSelected(wxCommandEvent &e)
{
	wxUnusedVar(e);
	TreeItemInfo info = m_mgr->GetSelectedTreeItemInfo(TreeFileView);

	if(wxMessageBox(wxT("Really reset file ")+info.m_text
	                +wxT("?\nAll changes to the file will be lost!"),
	                wxT("Confirm reset"), wxYES_NO) == wxNO) {
		return;
	}
	gitAction ga = {gitResetFile,info.m_fileName.GetFullPath()};
	m_gitActionQueue.push(ga);
	ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnSwitchLocalBranch(wxCommandEvent &e)
{
	wxUnusedVar(e);
	if(m_modifiedFiles.GetCount() != 0) {
		wxMessageBox(wxT("Modified files found! Commit them first before switching branches..."));
		return;
	}

	if(m_localBranchList.GetCount() == 0) {
		wxMessageBox(wxT("No other local branches found!"));
		return;
	}

	wxString message = wxT("Select branch (current is ");
	message << m_currentBranch << wxT(")");

	wxString selection = wxGetSingleChoice(message, wxT("Switch branch"),m_localBranchList);

	if(selection.IsEmpty())
		return;

	gitAction ganew = {gitBranchSwitch,selection};
	m_gitActionQueue.push(ganew);
	AddDefaultActions();
	m_mgr->SaveAll();
	ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnSwitchRemoteBranch(wxCommandEvent &e)
{
	wxUnusedVar(e);
	if(m_modifiedFiles.GetCount() != 0) {
		wxMessageBox(wxT("Modified files found! Commit them first before switching branches..."));
		return;
	}
	if(m_remoteBranchList.GetCount() == 0) {
		wxMessageBox(wxT("No remote branches found!"));
		return;
	}
	wxString message = wxT("Select remote branch (current is ");
	message << m_currentBranch << wxT(")");

	wxString selection = wxGetSingleChoice(message, wxT("Switch to remote branch"),
	                                       m_remoteBranchList);

	if(selection.IsEmpty())
		return;

	wxString localBranch = selection;
	localBranch.Replace(wxT("origin/"), wxT(""));
	localBranch = wxGetTextFromUser(wxT("Specify the name for the local branch"),
	                                wxT("Branch name"), localBranch);
	if(localBranch.IsEmpty())
		return;

	gitAction ganew = {gitBranchSwitchRemote,localBranch + wxT(" ") + selection};
	m_gitActionQueue.push(ganew);

	AddDefaultActions();
	m_mgr->SaveAll();
	ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnCreateBranch(wxCommandEvent &e)
{
	wxUnusedVar(e);
	wxString newBranch = wxGetTextFromUser(wxT("Specify the name of the new branch"),
	                                       wxT("Branch name"));
	if(newBranch.IsEmpty())
		return;

	gitAction ga = {gitBranchCreate,newBranch};
	m_gitActionQueue.push(ga);

	if(wxMessageBox(wxT("Switch to new branch once it is created?"),
	                wxT("Switch to new branch"), wxYES_NO) == wxYES) {
		ga.action = gitBranchSwitch;
		ga.arguments = newBranch;
		m_gitActionQueue.push(ga);
		AddDefaultActions();
		m_mgr->SaveAll();
	}

	ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnCommit(wxCommandEvent &e)
{
	wxUnusedVar(e);
	if(m_modifiedFiles.GetCount() == 0
	   && !m_addedFiles) {
		wxMessageBox(wxT("No modified files found, nothing to commit..."));
		return;
	}
	gitAction ga = {gitDiffRepoCommit,wxT("")};
	m_gitActionQueue.push(ga);
	ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnCommitList(wxCommandEvent &e)
{
	wxUnusedVar(e);
	gitAction ga = {gitCommitList,wxT("")};
	m_gitActionQueue.push(ga);
	ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnPush(wxCommandEvent &e)
{
	wxUnusedVar(e);
	if(m_remotes.GetCount() == 0) {
		wxMessageBox(wxT("No remotes found, can't push!"));
		return;
	}

	if(wxMessageBox(wxT("Push all local commits?"),
	                wxT("Push changes"), wxYES_NO) == wxYES) {
		wxString remote = m_remotes[0];
		if(m_remotes.GetCount() > 1) {
			remote = wxGetSingleChoice(wxT("Select remote to push to."),
			                           wxT("Select remote"),m_remotes);
			if(remote.IsEmpty()) {
				return;
			}
		}
		gitAction ga = {gitPush, remote + wxT(" ") + m_currentBranch};
		m_gitActionQueue.push(ga);
		ProcessGitActionQueue();
	}
}
/*******************************************************************************/
void GitPlugin::OnPull(wxCommandEvent &e)
{
	wxUnusedVar(e);
	if(wxMessageBox(wxT("Save all changes and pull remote changes?"),
	                wxT("Pull remote changes"), wxYES_NO) == wxYES) {
		m_mgr->SaveAll();
		gitAction ga = {gitPull,wxT("")};
		m_gitActionQueue.push(ga);
		AddDefaultActions();
		ProcessGitActionQueue();
	}
}
/*******************************************************************************/
void GitPlugin::OnResetRepository(wxCommandEvent &e)
{
	wxUnusedVar(e);
	if(wxMessageBox(wxT("Are you sure that you want to discard all local changes?"),
	                wxT("Reset repository"), wxYES_NO) == wxYES) {
		gitAction ga = {gitResetRepo,wxT("")};
		m_gitActionQueue.push(ga);
		AddDefaultActions();
		ProcessGitActionQueue();
	}
}
/*******************************************************************************/
void GitPlugin::OnStartGitk(wxCommandEvent& e)
{
	wxUnusedVar(e);
	wxString oldCWD = wxGetCwd();
	wxSetWorkingDirectory(m_repositoryDirectory);
	wxExecute(m_pathGITKExecutable);
	wxSetWorkingDirectory(oldCWD);
}
/*******************************************************************************/
void GitPlugin::OnListModified(wxCommandEvent& e)
{
	wxUnusedVar(e);
	wxArrayString choices;
	std::map<wxString, wxTreeItemId>::const_iterator it;
	for(it = m_modifiedIDs.begin(); it != m_modifiedIDs.end(); ++it) {
		if(it->second.IsOk())
			choices.Add(it->first);
	}

	if(choices.GetCount() == 0)
		return;

	wxString choice =wxGetSingleChoice(wxT("Jump to modifed file"),
	                                   wxT("Modifed files"),
	                                   choices);
	if(!choice.IsEmpty()) {
		wxTreeItemId id = m_modifiedIDs[choice];
		if(id.IsOk()) {
			m_mgr->GetTree(TreeFileView)->EnsureVisible(id);
			m_mgr->GetTree(TreeFileView)->SelectItem(id);
		}
	}

}
/*******************************************************************************/
void GitPlugin::OnRefresh(wxCommandEvent& e)
{
	wxUnusedVar(e);
	AddDefaultActions();
	ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnGarbageColletion(wxCommandEvent& e)
{
	wxUnusedVar(e);
	gitAction ga = {gitGarbageCollection,wxT("")};
	m_gitActionQueue.push(ga);
	ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnFileSaved(wxCommandEvent& e)
{
	wxUnusedVar(e);
	std::map<wxString, wxTreeItemId>::const_iterator it;
	for(it = m_modifiedIDs.begin(); it != m_modifiedIDs.end(); ++it) {
		if(!it->second.IsOk()) {
			wxLogMessage(wxT("GIT: Stored item not found in tree, rebuilding item IDs"));
			gitAction ga = {gitListAll,wxT("")};
			m_gitActionQueue.push(ga);
			break;
		}
		m_mgr->GetTree(TreeFileView)->SetItemTextColour(it->second,m_colourTrackedFile);
	}

	gitAction ga = {gitListModified,wxT("")};
	m_gitActionQueue.push(ga);
	ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnFilesAddedToProject(wxCommandEvent& e)
{
	wxUnusedVar(e);
	wxLogMessage(wxT("GIT: Files added to project, updating file list"));
	gitAction ga = {gitListAll,wxT("")};
	m_gitActionQueue.push(ga);
	ga.action = gitListModified;
	m_gitActionQueue.push(ga);
	ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnWorkspaceLoaded(wxCommandEvent& e)
{
	InitDefaults();
	e.Skip();
}
/*******************************************************************************/
void GitPlugin::ProcessGitActionQueue()
{
	if(m_repositoryDirectory.IsEmpty()) {
		m_gitActionQueue.pop();
		return;
	}

	if(m_process) {
		wxLogMessage(wxT("GIT: Process already running"));
		return;
	}
	if(m_gitActionQueue.size() == 0)
		return;

	gitAction ga = m_gitActionQueue.front();

	wxString command = m_pathGITExecutable;
	switch(ga.action) {
	case gitListAll:
		wxLogMessage(wxT("GIT: Listing files in git repository"));
		command << wxT(" --no-pager ls-files");
		//ShowProgress(wxT("Listing files in git repository"));
		break;
	case gitListModified:
		wxLogMessage(wxT("GIT: Listing modified files in git repository"));
		command << wxT(" --no-pager ls-files -m");
		//ShowProgress(wxT("Listing modified files in git repository"));
		break;
	case gitListRemotes:
		wxLogMessage(wxT("GIT: Listing remotes"));
		command << wxT(" --no-pager remote");
		///ShowProgress(wxT("Listing remotes"));
		break;
	case gitAddFile:
		wxLogMessage(wxT("GIT: Add file ") + ga.arguments);
		command << wxT(" --no-pager add ") << ga.arguments;
		break;
	case gitDeleteFile:
		wxLogMessage(wxT("GIT: Delete file ") + ga.arguments);
		command << wxT(" --no-pager update-index --remove --force-remove ") << ga.arguments;
		break;
	case gitDiffFile:
		wxLogMessage(wxT("GIT: Diff file ") + ga.arguments);
		command << wxT(" --no-pager diff --no-color ") << ga.arguments;
		break;
	case gitDiffRepoCommit:
		wxLogMessage(wxT("GIT: Diff repo HEAD"));
		command << wxT(" --no-pager diff --no-color HEAD");
		ShowProgress(wxT("Obtaining diffs for modified files..."));
		break;
	case gitResetFile:
		wxLogMessage(wxT("GIT: Reset file ") + ga.arguments);
		command << wxT(" --no-pager checkout ") << ga.arguments;
		break;
	case gitPull:
		wxLogMessage(wxT("GIT: Pull remote changes"));
		ShowProgress(wxT("Obtaining remote changes"), false);
		command << wxT(" --no-pager pull --log");
		break;
	case gitPush:
		wxLogMessage(wxT("GIT: Push local changes (")+ ga.arguments+wxT(")"));
		command << wxT(" --no-pager push ") << ga.arguments;
		ShowProgress(wxT("Pushing local changes..."), false);
		break;
	case gitCommit:
		wxLogMessage(wxT("GIT: Commit local changes (")+ ga.arguments+wxT(")"));
		command << wxT(" --no-pager commit ") << ga.arguments;
		ShowProgress(wxT("Commiting local changes..."));
		break;
	case gitResetRepo:
		wxLogMessage(wxT("GIT: Reset repository"));
		command << wxT(" --no-pager reset --hard");
		ShowProgress(wxT("Resetting local repository..."));
		break;
	case gitBranchCreate:
		wxLogMessage(wxT("GIT: Create local branch ")+ ga.arguments);
		command << wxT(" --no-pager branch ") << ga.arguments;
		ShowProgress(wxT("Creating local branch..."));
		break;
	case gitBranchCurrent:
		wxLogMessage(wxT("GIT: Get current branch"));
		command << wxT(" --no-pager branch --no-color");
		break;
	case gitBranchList:
		wxLogMessage(wxT("GIT: List local branches"));
		command << wxT(" --no-pager branch --no-color");
		break;
	case gitBranchListRemote:
		wxLogMessage(wxT("GIT: List remote branches"));
		command << wxT(" --no-pager branch -r --no-color");
		break;
	case gitBranchSwitch:
		wxLogMessage(wxT("GIT: Switch to local branch ")+ ga.arguments);
		ShowProgress(wxT("Switching to local branch ")+ ga.arguments, false);
		command << wxT(" --no-pager checkout ") << ga.arguments;
		break;
	case gitBranchSwitchRemote:
		wxLogMessage(wxT("GIT: Switch to remote branch ")+ ga.arguments);
		ShowProgress(wxT("Switching to remote branch ")+ ga.arguments, false);
		command << wxT(" --no-pager checkout -b ") << ga.arguments;
		break;
	case gitCommitList:
		wxLogMessage(wxT("Listing commits.."));
		ShowProgress(wxT("Fetching commit list"));
		command << wxT(" --no-pager log --pretty=\"%h|%s|%cn|%ci\"");
		break;
	case gitRebase:
		wxLogMessage(wxT("GIT: Rebasing.."));
		ShowProgress(wxT("Rebase with ")+ga.arguments+wxT(".."));
		command << wxT(" --no-pager rebase ") << ga.arguments;
		break;
	case gitGarbageCollection:
		wxLogMessage(wxT("GIT: Clean database.."));
		ShowProgress(wxT("Cleaning git database. This may take some time..."),false);
		command << wxT(" --no-pager gc");
		break;
	default:
		wxLogMessage(wxT("Unknown git action"));
		return;
	}
	wxLogMessage(wxT("Git: %s"), command.c_str());
	m_process = CreateAsyncProcess(this, command, IProcessCreateDefault, m_repositoryDirectory);
}

/*******************************************************************************/
void GitPlugin::FinishGitListAction(const gitAction& ga)
{
	if(!m_mgr->GetWorkspace())
		return;
	wxArrayString gitFileList = wxStringTokenize(m_commandOutput, wxT("\n"), wxTOKEN_STRTOK);

	for(unsigned i=0; i < gitFileList.GetCount(); ++i) {
		wxFileName fname(gitFileList[i]);
		fname.MakeAbsolute(m_repositoryDirectory);
		gitFileList[i] = fname.GetFullPath();
	}

	if(ga.action == gitListAll) {
		std::map<wxString, wxTreeItemId> d;
		ColourFileTree(m_mgr->GetTree(TreeFileView), gitFileList, m_colourTrackedFile,
		               d, false);
		m_trackedFiles = gitFileList;
	} else if(ga.action == gitListModified) {
		wxArrayString toColour;
		for(unsigned i=0; i < gitFileList.GetCount(); ++i) {
			wxTreeItemId id = m_modifiedIDs[gitFileList[i]];
			if(id.IsOk()) {
				m_mgr->GetTree(TreeFileView)->SetItemTextColour(id, m_colourDiffFile);
			} else {
				toColour.Add(gitFileList[i]);
			}
		}
		if(toColour.GetCount() != 0)
			ColourFileTree(m_mgr->GetTree(TreeFileView), toColour, m_colourDiffFile,
			               m_modifiedIDs, true);
		m_modifiedFiles = gitFileList;
	}
}
/*******************************************************************************/
void GitPlugin::ListBranchAction(const gitAction& ga)
{
	wxArrayString gitList = wxStringTokenize(m_commandOutput, wxT("\n"));
	if(gitList.GetCount()==0)
		return;

	wxArrayString branchList;
	for(unsigned i=0; i < gitList.GetCount(); ++i) {
		if(!gitList[i].Contains(wxT("->"))) {
			branchList.Add(gitList[i].Mid(2));
		}
	}
	
	if(branchList.Index(m_currentBranch) != wxNOT_FOUND) {
		branchList.Remove(m_currentBranch);
	}

	if(ga.action == gitBranchList) {
		m_localBranchList = branchList;
	} else if(ga.action == gitBranchListRemote) {
		m_remoteBranchList = branchList;
	}
}
/*******************************************************************************/
void GitPlugin::GetCurrentBranchAction(const gitAction& ga)
{
	wxArrayString gitList = wxStringTokenize(m_commandOutput, wxT("\n"));
	if(gitList.GetCount()==0)
		return;

	m_currentBranch.Empty();
	for(unsigned i=0; i < gitList.GetCount(); ++i) {
		if(gitList[i].StartsWith(wxT("*"))) {
			m_currentBranch = gitList[i].Mid(2);
			break;
		}
	}

	if(!m_currentBranch.IsEmpty()) {
		wxLogMessage(wxT("GIT: Current branch ")+m_currentBranch);
		m_mgr->GetDockingManager()->GetPane( wxT("Workspace View") ).Caption( wxT("Workspace View [")+m_currentBranch+wxT("]"));
		m_mgr->GetDockingManager()->Update();
	}
}
/*******************************************************************************/
void GitPlugin::UpdateFileTree()
{

	if(wxMessageBox(wxT("Do you want to start importing new / updating changed files?"),
	                wxT("Import files"), wxYES_NO) == wxNO) {
		return;
	}

	wxLogMessage(wxT("GIT: Start updating file tree"));

	wxString projectName = m_mgr->GetWorkspace()->GetActiveProjectName();
	wxString error = wxT("Error obtaining project");
	ProjectPtr proj =  m_mgr->GetWorkspace()->FindProjectByName(projectName,error);

	proj->BeginTranscation();
	wxString path = m_repositoryDirectory;
	if (path.EndsWith(wxT("/")) || path.EndsWith(wxT("\\"))) {
		path.RemoveLast();
	}
	wxFileName rootPath(path);

	wxArrayString gitfiles = wxStringTokenize(m_commandOutput, wxT("\n"));
	wxArrayString files;

	wxProgressDialog *prgDlg = new wxProgressDialog (wxT("Importing files ..."),
	        wxT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"),
	        (int)gitfiles.GetCount()+2, NULL, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE);
	prgDlg->GetSizer()->Fit(prgDlg);
	prgDlg->Layout();
	prgDlg->Centre();

	wxString filespec = wxT("*.cpp;*.hpp;*.c;*.h;*.ui;*.py;*.txt");
	bool extlessFiles(true);
	wxStringTokenizer tok(filespec, wxT(";"));
	std::set<wxString> specMap;
	while ( tok.HasMoreTokens() ) {
		wxString v = tok.GetNextToken().AfterLast(wxT('*'));
		v = v.AfterLast(wxT('.')).MakeLower();
		specMap.insert( v );
	}

	//filter non interesting files
	for (size_t i=0; i<gitfiles.GetCount(); i++) {

		prgDlg->Update((int)i, wxT("Checking file ")+ gitfiles.Item(i));
		wxFileName fn(m_repositoryDirectory+wxT("/")+gitfiles.Item(i));

		/* always excluded by default */
		wxString filepath = fn.GetPath();
		if( filepath.Contains(wxT(".svn"))           || filepath.Contains(wxT(".cvs"))           ||
		    filepath.Contains(wxT(".arch-ids"))      || filepath.Contains(wxT("arch-inventory")) ||
		    filepath.Contains(wxT("autom4te.cache")) || filepath.Contains(wxT("BitKeeper"))      ||
		    filepath.Contains(wxT(".bzr"))           || filepath.Contains(wxT(".bzrignore"))     ||
		    filepath.Contains(wxT("CVS"))            || filepath.Contains(wxT(".cvsignore"))     ||
		    filepath.Contains(wxT("_darcs"))         || filepath.Contains(wxT(".deps"))          ||
		    filepath.Contains(wxT("EIFGEN"))         || filepath.Contains(wxT(".git"))           ||
		    filepath.Contains(wxT(".gitignore"))     ||
		    filepath.Contains(wxT(".hg"))            || filepath.Contains(wxT("PENDING"))        ||
		    filepath.Contains(wxT("RCS"))            || filepath.Contains(wxT("RESYNC"))         ||
		    filepath.Contains(wxT("SCCS"))           || filepath.Contains(wxT("{arch}"))) {
			continue;
		}

		if ( specMap.empty() ) {
			files.Add(gitfiles.Item(i));
		} else if (fn.GetExt().IsEmpty() & extlessFiles) {
			files.Add(gitfiles.Item(i));
		} else if (specMap.find(fn.GetExt().MakeLower()) != specMap.end()) {
			files.Add(gitfiles.Item(i));
		}
	}

	prgDlg->Update(gitfiles.GetCount()+1, wxT("Adding files..."));
	// get list of files
	std::vector<wxFileName> vExistingFiles;
	wxArrayString existingFiles;

	proj->GetFiles(vExistingFiles, true);
	for (size_t i=0; i<vExistingFiles.size(); i++) {
		existingFiles.Add(vExistingFiles.at(i).GetFullPath());
	}

	for (size_t i=0; i<files.GetCount(); i++) {
		wxFileName fn(m_repositoryDirectory+wxT("/")+files.Item(i));

		// if the file already exist, skip it
		if (existingFiles.Index(fn.GetFullPath()) != wxNOT_FOUND) {
			continue;
		}

		wxString fullpath = fn.GetFullPath();
		fn.MakeRelativeTo(path);

		wxString relativePath = fn.GetPath();
		relativePath.Replace(wxT("/"), wxT(":"));
		relativePath.Replace(wxT("\\"), wxT(":"));

		if (relativePath.IsEmpty()) {
			//the file is probably under the root, add it under
			//a virtual directory with the name of the target
			//root folder
			relativePath = rootPath.GetName();
		}
		relativePath.Append(wxT(":"));

		bool createAndAdd = false;
		wxArrayString dummyFiles;
		proj->GetFilesByVirtualDir(relativePath, dummyFiles);
		if(dummyFiles.GetCount() != 0 ) {
			createAndAdd = true;
		} else {
			proj->GetFilesByVirtualDir(relativePath.Left(relativePath.Find(wxT(":"))), dummyFiles);
			if(dummyFiles.GetCount() != 0) {
				createAndAdd = true;
			}
		}

		if(createAndAdd) {
			wxLogMessage(wxT("GIT: Adding ")+fullpath
			             +wxT(" to ")+ relativePath);
			proj->CreateVirtualDir(relativePath, true);
			proj->FastAddFile(fullpath, relativePath);

		}
	}
	prgDlg->Update(gitfiles.GetCount()+2, wxT("Finished adding files..."));
	prgDlg->Destroy();
	proj->CommitTranscation();
	wxLogMessage(wxT("GIT: Finished updating file tree"));

	m_mgr->ReloadWorkspace();
}
/*******************************************************************************/
void GitPlugin::OnProcessTerminated(wxCommandEvent &event)
{
	HideProgress();

	int exitCode = 0;
	if(m_process) {
		int pid = m_process->GetPid();
		IProcess::GetProcessExitCode(pid, exitCode);
	}
	m_commandOutput.Replace(wxT("\r"), wxT(""));

	if(exitCode != 0 || m_commandOutput.StartsWith(wxT("fatal"))
	   || m_commandOutput.StartsWith(wxT("error"))) {
		wxString msg = wxT("There was a problem while performing a git action.\n"
		                   "Last command output:\n");
		msg << m_commandOutput;
		msg << wxT("\nExit code: ") << exitCode;
		wxMessageBox(msg, wxT("git error"),wxICON_ERROR);
		//Last action failed, clear queue
		while(!m_gitActionQueue.empty()) {
			m_gitActionQueue.pop();
		}

		if (m_process) {
			delete m_process;
			m_process = NULL;
		}
		m_commandOutput.Clear();
		return;
	}

	gitAction ga = m_gitActionQueue.front();
	if(ga.action == gitListAll
	   || ga.action == gitListModified) {
		if(ga.action == gitListAll
		   && m_bActionRequiresTreUpdate) {
			if(m_commandOutput.Lower().Contains(_("created")))
				UpdateFileTree();
		}
		m_bActionRequiresTreUpdate = false;
		FinishGitListAction(ga);
	} else if(ga.action == gitListRemotes ) {
		wxArrayString gitList = wxStringTokenize(m_commandOutput, wxT("\n"));
		m_remotes = gitList;
	} else if(ga.action == gitDiffFile ) {
		GitDiffDlg dlg(m_mgr->GetTheApp()->GetTopWindow());
		dlg.SetDiff(m_commandOutput);
		dlg.ShowModal();
	} else if(ga.action == gitDiffRepoCommit ) {
		GitCommitDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_repositoryDirectory);
		dlg.AppendDiff(m_commandOutput);
		if(dlg.ShowModal() == wxID_OK) {
			wxString message = dlg.GetCommitMessage();
			if(!message.IsEmpty()) {
				wxArrayString files = dlg.GetSelectedFiles();
				wxString arg = wxT("-m \"");
				arg << message;
				arg << wxT("\" ");
				if(files.GetCount() != 0) {
					for(unsigned i=0; i < files.GetCount(); ++i)
						arg << files[i] << wxT(" ");
				}
				gitAction ga = {gitCommit,arg};
				m_gitActionQueue.push(ga);
				AddDefaultActions();
			} else {
				wxMessageBox(wxT("No commit message given, aborting..."));
			}
		}
	} else if(ga.action == gitResetFile ) {
		wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT);
		m_mgr->GetTheApp()->GetTopWindow()->GetEventHandler()->AddPendingEvent(e);

		gitAction ga = {gitListAll,wxT("")};
		m_gitActionQueue.push(ga);
		ga.action = gitListModified;
		m_gitActionQueue.push(ga);
	} else if(ga.action == gitBranchCurrent ) {
		GetCurrentBranchAction(ga);
	} else if(ga.action == gitBranchList || ga.action == gitBranchListRemote ) {
		ListBranchAction(ga);
	} else if(ga.action == gitBranchSwitch
	          || ga.action == gitBranchSwitchRemote
	          || ga.action == gitPull
	          || ga.action == gitResetRepo) {
		if(ga.action == gitPull) {
			if(m_commandOutput.Contains(wxT("Already"))) {
				wxMessageBox(wxT("Nothing to pull, already up-to-date."));
			} else {
				wxString log =  m_commandOutput.Mid(m_commandOutput.Find(wxT("From ")));
				GitLogDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), wxT("Pull log"));
				dlg.SetLog(log);
				dlg.ShowModal();
				if(m_commandOutput.Contains(wxT("Merge made by"))) {
					if(wxMessageBox(wxT("Merged after pull. Rebase?"),wxT("Rebase"), wxYES_NO) == wxYES) {
						wxString selection;
						if(m_remotes.GetCount() > 1) {
							selection = wxGetSingleChoice(wxT("Rebase with what branch?"), wxT("Rebase"),m_remoteBranchList);
						} else {
							selection = m_remotes[0]+wxT("/")+m_currentBranch;
							if(wxMessageBox(wxT("Rebase with ")+selection+wxT("?"),wxT("Rebase"), wxYES_NO) == wxNO)
								selection.Empty();
						}

						if(selection.IsEmpty())
							return;

						gitAction ga = {gitRebase,selection};
						m_gitActionQueue.push(ga);
					}
				} else if(m_commandOutput.Contains(wxT("CONFLICT"))) {
					if(wxMessageBox(wxT("There was a conflict during merge.\n"
					                    "Please resolve conflicts and commit by hand.\n"
					                    "After resolving conflicts, be sure to reload the current project.\n\n"
					                    "Would you like to start \'git mergetool\'?"),
					                wxT("Conflict found during merge"),
					                wxYES_NO) == wxYES) {
						wxString oldCWD = wxGetCwd();
						wxSetWorkingDirectory(m_repositoryDirectory);
						wxExecute(m_pathGITExecutable+wxT(" mergetool -y"));
						wxSetWorkingDirectory(oldCWD);
					}
				}
				if(m_commandOutput.Contains(wxT("Updating")))
					m_bActionRequiresTreUpdate = true;
			}
		} else if(ga.action == gitBranchSwitch
		          || ga.action == gitBranchSwitchRemote) {
			if(wxMessageBox(wxT("Switched branch. Do you want to clean the current build?"),
			                wxT("Clean build after branch switch"), wxYES_NO) == wxYES) {
				wxString conf, projectName;
				projectName = m_mgr->GetWorkspace()->GetActiveProjectName();

				// get the selected configuration to be built
				BuildConfigPtr bldConf = m_mgr->GetWorkspace()->GetProjBuildConf(projectName, wxEmptyString);
				if (bldConf) {
					conf = bldConf->GetName();
				}

				QueueCommand buildInfo(projectName, conf, false, QueueCommand::Clean);
				if (bldConf && bldConf->IsCustomBuild()) {
					buildInfo.SetKind(QueueCommand::CustomBuild);
					buildInfo.SetCustomBuildTarget(wxT("Clean"));
				}
				m_mgr->PushQueueCommand(buildInfo);
				m_mgr->ProcessCommandQueue();
			}
			//update the tree
			m_modifiedIDs.clear();
			gitAction ga = {gitListAll,wxT("")};
			m_gitActionQueue.push(ga);
			ga.action = gitListModified;
			m_gitActionQueue.push(ga);
		}
	} else if(ga.action == gitCommitList) {
		GitCommitListDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_repositoryDirectory);
		dlg.SetCommitList(m_commandOutput);
		dlg.ShowModal();
	} else if(ga.action == gitCommitList) {
		m_addedFiles = false;
	}

	if (m_process) {
		delete m_process;
		m_process = NULL;
	}
	m_commandOutput.Clear();
	m_gitActionQueue.pop();
	ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnProcessOutput(wxCommandEvent &event)
{
	ProcessEventData *ped = (ProcessEventData*)event.GetClientData();
	if( ped ) {
		wxString output = ped->GetData();
		m_commandOutput.Append(output);

		if(m_progressDialog && m_progressDialog->IsShown()) {
			wxString message = output.Left(output.Find(':'));
			int percent = output.Find('%',true);
			if(percent != wxNOT_FOUND) {
				wxString number = output.Mid(percent-3,3);
				number.Trim(false);
				unsigned long current;
				if(number.ToULong(&current)) {
					message.Prepend(m_progressMessage+wxT("\nStatus: "));
					m_progressDialog->Update(current, message);
				}
			}
		}
		delete ped;
	}
}
/*******************************************************************************/
void GitPlugin::InitDefaults()
{
	if(!m_mgr->GetWorkspace()) {
		return;
	}

	wxString workspaceName = m_mgr->GetWorkspace()->GetName();

	GitEntry data;
	m_mgr->GetConfigTool()->ReadObject(wxT("GitData"), &data);

	if(data.GetTrackedFileColour().IsOk()) {
		m_colourTrackedFile = data.GetTrackedFileColour();
	}
	if(data.GetDiffFileColour().IsOk()) {
		m_colourDiffFile = data.GetDiffFileColour();
	}
	if(!data.GetGITExecutablePath().IsEmpty()) {
		m_pathGITExecutable = data.GetGITExecutablePath();
	}
	if(!data.GetGITKExecutablePath().IsEmpty()) {
		m_pathGITKExecutable = data.GetGITKExecutablePath();
	}

	wxString repoPath = data.GetPath(workspaceName);

	if(!repoPath.IsEmpty() && wxFileName::DirExists(repoPath + wxFileName::GetPathSeparator() + wxT(".git"))) {
		m_repositoryDirectory = repoPath;
		EnableMenuAndToolBar(true);
		
	} else {
		EnableMenuAndToolBar(false);
		m_repositoryDirectory.Empty();
		m_mgr->GetDockingManager()->GetPane( wxT("Workspace View") ).
		Caption( wxT("Workspace View"));
		m_mgr->GetDockingManager()->Update();
	}

	if(!m_repositoryDirectory.IsEmpty()) {
		AddDefaultActions();
		ProcessGitActionQueue();
	}
}

/*******************************************************************************/
void GitPlugin::EnableMenuAndToolBar(bool enable)
{
	if(m_pluginToolbar) {
		m_pluginToolbar->EnableTool(ID_PULL, enable);
		m_pluginToolbar->EnableTool(ID_COMMIT, enable);
		m_pluginToolbar->EnableTool(ID_PUSH, enable);
		m_pluginToolbar->EnableTool(ID_RESET_REPO, enable);
		m_pluginToolbar->EnableTool(ID_CREATE_BRANCH, enable);
		m_pluginToolbar->EnableTool(ID_SWITCH_BRANCH, enable);
		m_pluginToolbar->EnableTool(ID_SWITCH_BRANCH_REMOTE, enable);
		m_pluginToolbar->EnableTool(ID_COMMIT_LIST, enable);
		m_pluginToolbar->EnableTool(ID_GITK, enable);
	}

	if(m_pluginMenu) {
		m_pluginMenu->Enable(ID_PULL, enable);
		m_pluginMenu->Enable(ID_COMMIT, enable);
		m_pluginMenu->Enable(ID_PUSH, enable);
		m_pluginMenu->Enable(ID_RESET_REPO, enable);
		m_pluginMenu->Enable(ID_CREATE_BRANCH, enable);
		m_pluginMenu->Enable(ID_SWITCH_BRANCH, enable);
		m_pluginMenu->Enable(ID_SWITCH_BRANCH_REMOTE, enable);
		m_pluginMenu->Enable(ID_COMMIT_LIST, enable);
		m_pluginMenu->Enable(ID_GITK, enable);
		m_pluginMenu->Enable(ID_LIST_MODIFIED, enable);
		m_pluginMenu->Enable(ID_GARBAGE_COLLETION, enable);
		m_pluginMenu->Enable(ID_LIST_MODIFIED, enable);
		m_pluginMenu->Enable(ID_REFRESH, enable);
	}
}
/*******************************************************************************/
void GitPlugin::AddDefaultActions()
{
	gitAction ga = {gitBranchCurrent,wxT("")};
	m_gitActionQueue.push(ga);
	ga.action = gitListAll;
	m_gitActionQueue.push(ga);
	ga.action = gitListModified;
	m_gitActionQueue.push(ga);
	ga.action = gitBranchList;
	m_gitActionQueue.push(ga);
	ga.action = gitBranchListRemote;
	m_gitActionQueue.push(ga);
	ga.action = gitListRemotes;
	m_gitActionQueue.push(ga);
}
/*******************************************************************************/
void GitPlugin::ColourFileTree(wxTreeCtrl *tree, const wxArrayString& files,
                               const wxColour& colour,
                               std::map<wxString, wxTreeItemId>& cIDs,
                               bool storeIds)
{
	std::stack<wxTreeItemId> items;
	if (tree->GetRootItem().IsOk())
		items.push(tree->GetRootItem());

	while (!items.empty()) {
		wxTreeItemId next = items.top();
		items.pop();

		if (next != tree->GetRootItem()) {
			FilewViewTreeItemData *data = static_cast<FilewViewTreeItemData*>( tree->GetItemData( next ) );
			const wxString& path = data->GetData().GetFile();
			if(!path.IsEmpty() && files.Index(path) != wxNOT_FOUND) {
				tree->SetItemTextColour(next, colour);
				if(storeIds)
					cIDs[path] = next;
			}
		}

		wxTreeItemIdValue cookie;
		wxTreeItemId nextChild = tree->GetFirstChild(next, cookie);
		while (nextChild.IsOk()) {
			items.push(nextChild);
			nextChild = tree->GetNextSibling(nextChild);
		}
	}
}
/*******************************************************************************/
void GitPlugin::OnProgressTimer(wxTimerEvent& Event)
{
	if(m_progressDialog->IsShown())
		m_progressDialog->Pulse();
}
/*******************************************************************************/
void GitPlugin::ShowProgress(const wxString& message, bool pulse)
{
	if(!m_progressDialog) {
		m_progressDialog = new wxProgressDialog(wxT("Git progress"),
		                                        wxT("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"),
		                                        101);
		m_progressDialog->SetIcon(wxICON(icon_git));
	}
	m_progressDialog->CenterOnScreen();
	m_progressDialog->Show();
	if(pulse) {
		m_progressDialog->Pulse(message);
		m_progressTimer.Start(50);
	} else {
		m_progressMessage = message;
		m_progressDialog->Update(0, message);
	}
}
/*******************************************************************************/
void GitPlugin::HideProgress()
{
	if(m_progressDialog) {
		m_progressDialog->Hide();
		m_progressTimer.Stop();
	}
}
