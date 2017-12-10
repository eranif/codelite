//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : git.cpp
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

#include "event_notifier.h"
#include <stack>
#include <wx/artprov.h>
#include <wx/file.h>
#include <wx/tokenzr.h>
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>

#include "globals.h"

#include "workspace.h"

#include "git.h"
#include "gitCommitDlg.h"
#include "gitCommitListDlg.h"
#include "gitDiffDlg.h"
#include "gitFileDiffDlg.h"
#include "gitSettingsDlg.h"
#include "gitentry.h"

#include "DiffSideBySidePanel.h"
#include "GitApplyPatchDlg.h"
#include "GitConsole.h"
#include "GitLocator.h"
#include "bitmap_loader.h"
#include "clCommandProcessor.h"
#include "clStatusBar.h"
#include "clWorkspaceManager.h"
#include "dirsaver.h"
#include "environmentconfig.h"
#include "file_logger.h"
#include "gitBlameDlg.h"
#include "gitCloneDlg.h"
#include "icons/icon_git.xpm"
#include "overlaytool.h"
#include "project.h"
#include <wx/ffile.h>
#include <wx/msgdlg.h>
#include <wx/sstream.h>
#include <wx/utils.h>
#include "clDiffFrame.h"

#ifdef __WXGTK__
#include <sys/wait.h>
#endif

static GitPlugin* thePlugin = NULL;
#define GIT_MESSAGE(...) m_console->AddText(wxString::Format(__VA_ARGS__));
#define GIT_MESSAGE1(...) \
    if(m_console->IsVerbose()) { m_console->AddText(wxString::Format(__VA_ARGS__)); }

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) { thePlugin = new GitPlugin(manager); }
    return thePlugin;
}
/*******************************************************************************/
CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("René Kraus"));
    info.SetName(wxT("Git"));
    info.SetDescription(_("Simple GIT plugin"));
    info.SetVersion(wxT("v1.1.0"));
    return &info;
}
/*******************************************************************************/
CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

/*******************************************************************************/
// Helper that returns the last-used selection for those git commands with several alternative options
wxString GetAnyDefaultCommand(const wxString& gitCommand)
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    GitCommandsEntries& ce = data.GetGitCommandsEntries(gitCommand);
    return ce.GetDefaultCommand();
}
/*******************************************************************************/

BEGIN_EVENT_TABLE(GitPlugin, wxEvtHandler)
EVT_TIMER(wxID_ANY, GitPlugin::OnProgressTimer)
END_EVENT_TABLE()

GitPlugin::GitPlugin(IManager* manager)
    : IPlugin(manager)
    , m_colourTrackedFile(wxT("DARK GREEN"))
    , m_colourDiffFile(wxT("MAROON"))
#ifdef __WXGTK__
    , m_pathGITExecutable(wxT("/usr/bin/git"))
    , m_pathGITKExecutable(wxT("/usr/bin/gitk"))
#else
    , m_pathGITExecutable(wxT("git"))
    , m_pathGITKExecutable(wxT("gitk"))
#endif
    , m_bActionRequiresTreUpdate(false)
    , m_process(NULL)
    , m_eventHandler(NULL)
    , m_topWindow(NULL)
    , m_pluginToolbar(NULL)
    , m_pluginMenu(NULL)
    , m_commitListDlg(NULL)
    , m_commandProcessor(NULL)
    , m_gitBlameDlg(NULL)
{
    m_longName = _("GIT plugin");
    m_shortName = wxT("Git");
    m_eventHandler = m_mgr->GetTheApp();

    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &GitPlugin::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &GitPlugin::OnProcessTerminated, this);

    EventNotifier::Get()->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(GitPlugin::OnInitDone), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(GitPlugin::OnWorkspaceLoaded), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(GitPlugin::OnWorkspaceClosed), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVED, clCommandEventHandler(GitPlugin::OnFileSaved), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PROJ_FILE_ADDED, clCommandEventHandler(GitPlugin::OnFilesAddedToProject), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_PROJ_FILE_REMOVED, clCommandEventHandler(GitPlugin::OnFilesRemovedFromProject),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CONFIG_CHANGED,
                                  wxCommandEventHandler(GitPlugin::OnWorkspaceConfigurationChanged), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CL_FRAME_TITLE, clCommandEventHandler(GitPlugin::OnMainFrameTitle), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FILE, &GitPlugin::OnFileMenu, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &GitPlugin::OnFolderMenu, this);
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_PROJECT_CHANGED, &GitPlugin::OnActiveProjectChanged, this);

    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderPullRebase, this, XRCID("git_pull_rebase_folder"));
    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderCommit, this, XRCID("git_commit_folder"));
    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderPush, this, XRCID("git_push_folder"));
    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderStash, this, XRCID("git_stash_folder"));
    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderStashPop, this, XRCID("git_stash_pop_folder"));
    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderGitBash, this, XRCID("git_bash_folder"));

    // Connect the file context menu event handlers
    m_eventHandler->Connect(XRCID("git_add_file"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnFileAddSelected), NULL, this);
    m_eventHandler->Connect(XRCID("git_reset_file"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnFileResetSelected), NULL, this);
    m_eventHandler->Connect(XRCID("git_diff_file"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnFileDiffSelected), NULL, this);
    m_eventHandler->Bind(wxEVT_MENU, &GitPlugin::OnFileGitBlame, this, XRCID("git_blame_file"));

    // Add the console
    m_console = new GitConsole(m_mgr->GetOutputPaneNotebook(), this);
    m_mgr->GetOutputPaneNotebook()->AddPage(m_console, _("Git"), false, m_mgr->GetStdIcons()->LoadBitmap("git"));
    m_tabToggler.reset(new clTabTogglerHelper(_("Git"), m_console, "", NULL));
    m_tabToggler->SetOutputTabBmp(m_mgr->GetStdIcons()->LoadBitmap("git"));

    m_progressTimer.SetOwner(this);
}
/*******************************************************************************/
GitPlugin::~GitPlugin() { delete m_gitBlameDlg; }

/*******************************************************************************/
clToolBar* GitPlugin::CreateToolBar(wxWindow* parent)
{
    wxUnusedVar(parent);
    return NULL;
}

/*******************************************************************************/
void GitPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    m_pluginMenu = new wxMenu();
    wxMenuItem* item(NULL);

    BitmapLoader* bmps = m_mgr->GetStdIcons();
    item = new wxMenuItem(m_pluginMenu, XRCID("git_pull"), _("Pull remote changes"));
    item->SetBitmap(bmps->LoadBitmap("pull"));
    m_pluginMenu->Append(item);
    m_pluginMenu->AppendSeparator();

    item = new wxMenuItem(m_pluginMenu, XRCID("git_commit"), _("Commit local changes"));
    item->SetBitmap(bmps->LoadBitmap("git-commit"));
    m_pluginMenu->Append(item);
    item = new wxMenuItem(m_pluginMenu, XRCID("git_push"), _("Push local commits"));
    item->SetBitmap(bmps->LoadBitmap("up"));
    m_pluginMenu->Append(item);
    m_pluginMenu->AppendSeparator();

    item = new wxMenuItem(m_pluginMenu, XRCID("git_reset_repository"), _("Reset current repository"));
    item->SetBitmap(bmps->LoadBitmap("clean"));
    m_pluginMenu->Append(item);
    m_pluginMenu->AppendSeparator();
    item = new wxMenuItem(m_pluginMenu, XRCID("git_create_branch"), _("Create local branch"));
    item->SetBitmap(bmps->LoadBitmap("plus"));
    m_pluginMenu->Append(item);
    item = new wxMenuItem(m_pluginMenu, XRCID("git_switch_branch"), _("Switch local branch"), _("Switch local branch"),
                          wxITEM_NORMAL);
    item->SetBitmap(bmps->LoadBitmap("split"));
    m_pluginMenu->Append(item);

    item = new wxMenuItem(m_pluginMenu, XRCID("git_switch_to_remote_branch"), _("Switch remote branch"),
                          _("Switch remote branch"), wxITEM_NORMAL);
    m_pluginMenu->Append(item);

    m_pluginMenu->AppendSeparator();
    item = new wxMenuItem(m_pluginMenu, XRCID("git_browse_commit_list"), _("List commits"), _("List commits"),
                          wxITEM_NORMAL);
    item->SetBitmap(bmps->LoadBitmap("tasks"));
    m_pluginMenu->Append(item);
    item =
        new wxMenuItem(m_pluginMenu, XRCID("git_commit_diff"), _("Show current diffs"), _("Show diffs"), wxITEM_NORMAL);
    item->SetBitmap(bmps->LoadBitmap("diff"));
    m_pluginMenu->Append(item);
    item = new wxMenuItem(m_pluginMenu, XRCID("git_blame"), _("Show git blame"), _("Show blame"), wxITEM_NORMAL);
    item->SetBitmap(bmps->LoadBitmap("finger"));
    m_pluginMenu->Append(item);
    item = new wxMenuItem(m_pluginMenu, XRCID("git_apply_patch"), _("Apply Patch"), _("Apply Patch"), wxITEM_NORMAL);
    item->SetBitmap(bmps->LoadBitmap("patch"));
    m_pluginMenu->Append(item);

    m_pluginMenu->AppendSeparator();
    item = new wxMenuItem(m_pluginMenu, XRCID("git_list_modified"), _("List modified files"), _("List modified files"),
                          wxITEM_NORMAL);
    item->SetBitmap(bmps->LoadBitmap("marker"));
    m_pluginMenu->Append(item);

    item = new wxMenuItem(m_pluginMenu, XRCID("git_start_gitk"), _("Start gitk"), _("Start gitk"), wxITEM_NORMAL);
    item->SetBitmap(bmps->LoadBitmap("debugger_start"));
    m_pluginMenu->Append(item);

    m_pluginMenu->AppendSeparator();
    item = new wxMenuItem(m_pluginMenu, XRCID("git_garbage_collection"), _("Clean git database (garbage collection)"));
    item->SetBitmap(bmps->LoadBitmap("clean"));
    m_pluginMenu->Append(item);

    item = new wxMenuItem(m_pluginMenu, XRCID("git_refresh"), _("Refresh git file list"), _("Refresh file lists"),
                          wxITEM_NORMAL);
    item->SetBitmap(bmps->LoadBitmap("debugger_restart"));
    m_pluginMenu->Append(item);
    m_pluginMenu->AppendSeparator();
    item = new wxMenuItem(m_pluginMenu, XRCID("git_settings"), _("GIT plugin settings"), _("GIT plugin settings"),
                          wxITEM_NORMAL);
    item->SetBitmap(bmps->LoadBitmap("cog"));
    m_pluginMenu->Append(item);
    item = new wxMenuItem(m_pluginMenu, XRCID("git_set_repository"), _("Set GIT repository path"),
                          _("Set GIT repository path"), wxITEM_NORMAL);
    item->SetBitmap(bmps->LoadBitmap("folder"));
    m_pluginMenu->Append(item);
    item = new wxMenuItem(m_pluginMenu, XRCID("git_clone"), _("Clone a git repository"), _("Clone a git repository"),
                          wxITEM_NORMAL);
    item->SetBitmap(bmps->LoadBitmap("copy"));
    m_pluginMenu->Append(item);

    item = new wxMenuItem(pluginsMenu, wxID_ANY, wxT("Git"));
    item->SetSubMenu(m_pluginMenu);
    item->SetBitmap(bmps->LoadBitmap("git"));
    pluginsMenu->Append(item);

    m_eventHandler->Bind(wxEVT_COMMAND_MENU_SELECTED, &GitPlugin::OnOpenMSYSGit, this, XRCID("git_msysgit"));
    m_eventHandler->Connect(XRCID("git_set_repository"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnSetGitRepoPath), NULL, this);
    m_eventHandler->Connect(XRCID("git_settings"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnSettings), NULL, this);
    m_eventHandler->Connect(XRCID("git_switch_branch"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnSwitchLocalBranch), NULL, this);
    m_eventHandler->Connect(XRCID("git_switch_to_remote_branch"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnSwitchRemoteBranch), NULL, this);
    m_eventHandler->Connect(XRCID("git_create_branch"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnCreateBranch), NULL, this);
    m_eventHandler->Connect(XRCID("git_pull"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GitPlugin::OnPull),
                            NULL, this);
    m_eventHandler->Connect(XRCID("git_clone"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GitPlugin::OnClone),
                            NULL, this);
    m_eventHandler->Connect(XRCID("git_commit"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnCommit), NULL, this);
    m_eventHandler->Connect(XRCID("git_browse_commit_list"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnCommitList), NULL, this);
    m_eventHandler->Connect(XRCID("git_commit_diff"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnShowDiffs), NULL, this);
    m_eventHandler->Connect(XRCID("git_blame"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnGitBlame), NULL, this);
    m_eventHandler->Connect(XRCID("git_apply_patch"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnApplyPatch), NULL, this);
    m_eventHandler->Connect(XRCID("git_push"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GitPlugin::OnPush),
                            NULL, this);
    m_eventHandler->Connect(XRCID("git_rebase"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnRebase), NULL, this);
    m_eventHandler->Connect(XRCID("git_reset_repository"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnResetRepository), NULL, this);
    m_eventHandler->Connect(XRCID("git_start_gitk"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnStartGitk), NULL, this);
    m_eventHandler->Connect(XRCID("git_list_modified"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnListModified), NULL, this);
    m_eventHandler->Connect(XRCID("git_refresh"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnRefresh), NULL, this);
    m_eventHandler->Connect(XRCID("git_garbage_collection"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GitPlugin::OnGarbageColletion), NULL, this);
#if 0
    m_eventHandler->Connect( XRCID("git_bisect_start"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnBisectStart ), NULL, this );
    m_eventHandler->Connect( XRCID("git_bisect_good"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnBisectGood ), NULL, this );
    m_eventHandler->Connect( XRCID("git_bisect_bad"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnBisectBad ), NULL, this );
    m_eventHandler->Connect( XRCID("git_bisect_reset"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( GitPlugin::OnBisectReset ), NULL, this );
#endif
    m_eventHandler->Connect(XRCID("git_switch_branch"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(GitPlugin::OnEnableGitRepoExists), NULL, this);
    m_eventHandler->Connect(XRCID("git_switch_to_remote_branch"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(GitPlugin::OnEnableGitRepoExists), NULL, this);
    m_eventHandler->Connect(XRCID("git_create_branch"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(GitPlugin::OnEnableGitRepoExists), NULL, this);
    m_eventHandler->Connect(XRCID("git_pull"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(GitPlugin::OnEnableGitRepoExists), NULL, this);
    m_eventHandler->Connect(XRCID("git_commit"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(GitPlugin::OnEnableGitRepoExists), NULL, this);
    m_eventHandler->Connect(XRCID("git_browse_commit_list"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(GitPlugin::OnEnableGitRepoExists), NULL, this);
    m_eventHandler->Connect(XRCID("git_commit_diff"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(GitPlugin::OnEnableGitRepoExists), NULL, this);
    m_eventHandler->Connect(XRCID("git_apply_patch"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(GitPlugin::OnEnableGitRepoExists), NULL, this);
    m_eventHandler->Connect(XRCID("git_push"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(GitPlugin::OnEnableGitRepoExists), NULL, this);
    m_eventHandler->Connect(XRCID("git_rebase"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(GitPlugin::OnEnableGitRepoExists), NULL, this);
    m_eventHandler->Connect(XRCID("git_reset_repository"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(GitPlugin::OnEnableGitRepoExists), NULL, this);
    m_eventHandler->Connect(XRCID("git_start_gitk"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GitPlugin::OnStartGitkUI),
                            NULL, this);
    m_eventHandler->Connect(XRCID("git_list_modified"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(GitPlugin::OnEnableGitRepoExists), NULL, this);
    m_eventHandler->Connect(XRCID("git_refresh"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(GitPlugin::OnEnableGitRepoExists), NULL, this);
    m_eventHandler->Connect(XRCID("git_garbage_collection"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(GitPlugin::OnEnableGitRepoExists), NULL, this);
    m_eventHandler->Connect(XRCID("git_blame"), wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(GitPlugin::OnEnableGitRepoExists), NULL, this);
}

/*******************************************************************************/
void GitPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

/*******************************************************************************/
void GitPlugin::UnPlug()
{
    // before this plugin is un-plugged we must remove the tab we added
    for(size_t i = 0; i < m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
        if(m_console == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
            m_mgr->GetOutputPaneNotebook()->RemovePage(i);
            m_console->Destroy();
            break;
        }
    }

    /*MENU*/
    m_eventHandler->Unbind(wxEVT_COMMAND_MENU_SELECTED, &GitPlugin::OnOpenMSYSGit, this, XRCID("git_msysgit"));
    m_eventHandler->Disconnect(XRCID("git_set_repository"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnSetGitRepoPath), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_settings"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnSettings), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_switch_branch"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnSwitchLocalBranch), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_switch_to_remote_branch"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnSwitchRemoteBranch), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_create_branch"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnCreateBranch), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_pull"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GitPlugin::OnPull),
                               NULL, this);
    m_eventHandler->Disconnect(XRCID("git_commit"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnCommit), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_browse_commit_list"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnCommitList), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_commit_diff"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnShowDiffs), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_blame"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnGitBlame), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_apply_patch"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnApplyPatch), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_push"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GitPlugin::OnPush),
                               NULL, this);
    m_eventHandler->Disconnect(XRCID("git_reset_repository"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnResetRepository), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_start_gitk"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnStartGitk), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_list_modified"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnListModified), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_refresh"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnRefresh), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_garbage_collection"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnGarbageColletion), NULL, this);
    m_eventHandler->Unbind(wxEVT_MENU, &GitPlugin::OnFileGitBlame, this, XRCID("git_blame_file"));

    /*SYSTEM*/
    EventNotifier::Get()->Disconnect(wxEVT_INIT_DONE, wxCommandEventHandler(GitPlugin::OnInitDone), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED, clCommandEventHandler(GitPlugin::OnFileSaved), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(GitPlugin::OnWorkspaceLoaded), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_PROJ_FILE_ADDED, clCommandEventHandler(GitPlugin::OnFilesAddedToProject),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CONFIG_CHANGED,
                                     wxCommandEventHandler(GitPlugin::OnWorkspaceConfigurationChanged), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_PROJECT_CHANGED, &GitPlugin::OnActiveProjectChanged, this);

    /*Context Menu*/
    m_eventHandler->Disconnect(XRCID("git_add_file"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnFileAddSelected), NULL, this);
    // m_eventHandler->Disconnect( ID_DELETE_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(
    // GitPlugin::OnFileDeleteSelected), NULL, this );
    m_eventHandler->Disconnect(XRCID("git_reset_file"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnFileResetSelected), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_diff_file"), wxEVT_COMMAND_MENU_SELECTED,
                               wxCommandEventHandler(GitPlugin::OnFileDiffSelected), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FILE, &GitPlugin::OnFileMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FOLDER, &GitPlugin::OnFolderMenu, this);
    wxTheApp->Unbind(wxEVT_MENU, &GitPlugin::OnFolderPullRebase, this, XRCID("git_pull_rebase_folder"));
    wxTheApp->Unbind(wxEVT_MENU, &GitPlugin::OnFolderCommit, this, XRCID("git_commit_folder"));
    wxTheApp->Unbind(wxEVT_MENU, &GitPlugin::OnFolderPush, this, XRCID("git_push_folder"));
    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderStash, this, XRCID("git_stash_folder"));
    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderStashPop, this, XRCID("git_stash_pop_folder"));
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &GitPlugin::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &GitPlugin::OnProcessTerminated, this);
    m_tabToggler.reset(NULL);
}

/*******************************************************************************/
void GitPlugin::OnSetGitRepoPath(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoSetRepoPath();
}

void GitPlugin::DoSetRepoPath(const wxString& repoPath, bool promptUser)
{
    wxString dir = repoPath;

    // Sanity
    if(dir.IsEmpty() && !promptUser) { return; }

    if(dir.IsEmpty() && promptUser) {
        // use the current repository as the starting path
        // if current repository is empty, use the current workspace path
        wxString startPath = m_repositoryDirectory;
        if(startPath.IsEmpty()) { startPath = GetWorkspaceFileName().GetPath(); }

        dir = ::wxDirSelector(_("Select git root directory"), startPath);
        if(dir.empty()) {
            return; // The user probably pressed Cancel
        }
    }

    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    wxString workspaceName(GetWorkspaceFileName().GetName());
    // Get the project name (can be PHP, JS or C++)
    wxString projectName = clWorkspaceManager::Get().IsWorkspaceOpened()
                               ? clWorkspaceManager::Get().GetWorkspace()->GetActiveProjectName()
                               : "";
    wxString lastRepoPath = data.GetProjectLastRepoPath(workspaceName, projectName);

    if(!dir.IsEmpty()) {
        // we were given a path, scan the folder and its parent
        // searching for .git folder, when we find one - its our git root
        // folder
        wxFileName fnDir(dir, "");
        while(fnDir.GetDirCount()) {
            wxFileName gitDotFolder(fnDir.GetPath(), "");
            gitDotFolder.AppendDir(".git");
            if(gitDotFolder.DirExists()) {
                // a match
                gitDotFolder.RemoveLastDir(); // Remove the .git folder
                dir = gitDotFolder.GetPath();
                break;
            }
            fnDir.RemoveLastDir();
        }

        if(!fnDir.GetDirCount()) {
            // Not found. Use any previously-set repo path outside the project
            dir = lastRepoPath;
            if(dir.empty()) { return; }
        }

        m_repositoryDirectory = dir;
        data.SetProjectLastRepoPath(workspaceName, projectName, m_repositoryDirectory);
        conf.WriteItem(&data);
        conf.Save();

        GIT_MESSAGE("Git repo path is now set to '%s'", m_repositoryDirectory);

        // Update the status bar icon to reflect that we are using "Git"
        clStatusBar* sb = m_mgr->GetStatusBar();
        if(sb) {
            sb->SetSourceControlBitmap(m_mgr->GetStdIcons()->LoadBitmap("git"), "Git",
                                       _("Using Git\nClick to open the Git view"));
        }
        AddDefaultActions();
        ProcessGitActionQueue();
    }
}

/*******************************************************************************/
void GitPlugin::OnSettings(wxCommandEvent& e)
{
    GitSettingsDlg dlg(m_topWindow, m_repositoryDirectory);
    if(dlg.ShowModal() == wxID_OK) {

        // update the paths
        clConfig conf("git.conf");
        GitEntry data;
        conf.ReadItem(&data);

        m_pathGITExecutable = data.GetGITExecutablePath();
        m_pathGITKExecutable = data.GetGITKExecutablePath();

        GIT_MESSAGE("git executable is now set to: %s", m_pathGITExecutable.c_str());
        GIT_MESSAGE("gitk executable is now set to: %s", m_pathGITKExecutable.c_str());
        AddDefaultActions();
        ProcessGitActionQueue();
    }
}

/*******************************************************************************/
void GitPlugin::OnFileAddSelected(wxCommandEvent& e)
{
    wxUnusedVar(e);

    wxArrayString files;
    files.swap(m_filesSelected);
    if(files.IsEmpty()) return;

    // Make the git console visible
    m_mgr->ShowOutputPane("Git");

    wxString workingDir;
    workingDir = wxFileName(files.Item(0)).GetPath();

    // Pepare the command:
    // git add --no-pager
    wxString cmd = "add";
    for(size_t i = 0; i < files.size(); ++i) {
        wxFileName fn(files.Item(i));
        fn.MakeRelativeTo(workingDir);
        wxString filename = fn.GetFullPath(wxPATH_UNIX);
        ::WrapWithQuotes(filename);
        cmd << " " << filename;
    }

    wxString commandOutput;
    DoExecuteCommandSync(cmd, workingDir, commandOutput);
    GetConsole()->AddRawText(commandOutput);
    RefreshFileListView();
}

/*******************************************************************************/
void GitPlugin::OnFileDeleteSelected(wxCommandEvent& e) { RefreshFileListView(); }

/*******************************************************************************/
void GitPlugin::OnFileDiffSelected(wxCommandEvent& e)
{
    wxUnusedVar(e);

    wxArrayString files;
    files.swap(m_filesSelected);
    if(files.IsEmpty()) return;

    // Make the git console visible
    m_mgr->ShowOutputPane("Git");

    wxString workingDir;
    workingDir = wxFileName(files.Item(0)).GetPath();

    for(size_t i = 0; i < files.size(); ++i) {
        // Pepare the command:
        // git add --no-pager
        wxString cmd = "show HEAD:";
        wxFileName fn(files.Item(i));
        fn.MakeRelativeTo(workingDir);
        wxString filename = fn.GetFullPath(wxPATH_UNIX);
        if(!filename.StartsWith(".")) { filename.Prepend("./"); }
        ::WrapWithQuotes(filename);
        cmd << filename;

        // We need to run this command per file
        wxString commandOutput;
        DoExecuteCommandSync(cmd, workingDir, commandOutput);
        if(!commandOutput.IsEmpty()) { DoShowDiffViewer(commandOutput, files.Item(i)); }
    }
}

/*******************************************************************************/
void GitPlugin::OnFileResetSelected(wxCommandEvent& e)
{
    wxUnusedVar(e);

    wxArrayString files;
    files.swap(m_filesSelected);
    if(files.IsEmpty()) return;

    // Make the git console visible
    m_mgr->ShowOutputPane("Git");

    wxString workingDir;
    workingDir = wxFileName(files.Item(0)).GetPath();

    // Pepare the command:
    // git add --no-pager
    wxString cmd = "checkout";
    for(size_t i = 0; i < files.size(); ++i) {
        wxFileName fn(files.Item(i));
        fn.MakeRelativeTo(workingDir);
        wxString filename = fn.GetFullPath(wxPATH_UNIX);
        ::WrapWithQuotes(filename);
        cmd << " " << filename;
    }

    wxString commandOutput;
    DoExecuteCommandSync(cmd, workingDir, commandOutput);
    GetConsole()->AddRawText(commandOutput);

    // Reload externally modified files
    EventNotifier::Get()->PostReloadExternallyModifiedEvent();
    RefreshFileListView();
}

/*******************************************************************************/
void GitPlugin::OnSwitchLocalBranch(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(!m_modifiedFiles.empty()) {
        wxMessageBox(_("Modified files found! Commit them first before switching branches..."), wxT("CodeLite"),
                     wxICON_ERROR | wxOK, m_topWindow);
        return;
    }

    if(m_localBranchList.GetCount() == 0) {
        wxMessageBox(_("No other local branches found."), wxT("CodeLite"), wxICON_INFORMATION | wxOK, m_topWindow);
        return;
    }

    wxString message = _("Select branch (current is ");
    message << m_currentBranch << wxT(")");

    wxString selection = wxGetSingleChoice(message, _("Switch branch"), m_localBranchList, m_topWindow);

    if(selection.IsEmpty()) return;

    gitAction ganew(gitBranchSwitch, selection);
    m_gitActionQueue.push_back(ganew);
    AddDefaultActions();
    m_mgr->SaveAll();
    ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnSwitchRemoteBranch(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(!m_modifiedFiles.empty()) {
        wxMessageBox(_("Modified files found! Commit them first before switching branches..."), wxT("CodeLite"),
                     wxICON_ERROR | wxOK, m_topWindow);
        return;
    }
    if(m_remoteBranchList.GetCount() == 0) {
        wxMessageBox(_("No remote branches found."), wxT("CodeLite"), wxICON_INFORMATION | wxOK, m_topWindow);
        return;
    }
    wxString message = _("Select remote branch (current is ");
    message << m_currentBranch << wxT(")");

    wxString selection = wxGetSingleChoice(message, _("Switch to remote branch"), m_remoteBranchList, m_topWindow);

    if(selection.IsEmpty()) return;

    wxString localBranch = selection;
    localBranch.Replace(wxT("origin/"), wxT(""));
    localBranch =
        wxGetTextFromUser(_("Specify the name for the local branch"), _("Branch name"), localBranch, m_topWindow);
    if(localBranch.IsEmpty()) return;

    gitAction ganew(gitBranchSwitchRemote, localBranch + wxT(" ") + selection);
    m_gitActionQueue.push_back(ganew);

    AddDefaultActions();
    m_mgr->SaveAll();
    ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnCreateBranch(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxString newBranch =
        wxGetTextFromUser(_("Specify the name of the new branch"), _("Branch name"), wxT(""), m_topWindow);
    if(newBranch.IsEmpty()) return;

    gitAction ga(gitBranchCreate, newBranch);
    m_gitActionQueue.push_back(ga);

    if(wxMessageBox(_("Switch to new branch once it is created?"), _("Switch to new branch"), wxYES_NO, m_topWindow) ==
       wxYES) {
        ga.action = gitBranchSwitch;
        ga.arguments = newBranch;
        m_gitActionQueue.push_back(ga);
        AddDefaultActions();
        m_mgr->SaveAll();
    }

    ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnCommit(wxCommandEvent& e)
{
    wxUnusedVar(e);
    gitAction ga(gitDiffRepoCommit, wxT(""));
    m_gitActionQueue.push_back(ga);
    m_mgr->ShowOutputPane("Git");
    ProcessGitActionQueue();
}

/*******************************************************************************/
void GitPlugin::OnCommitList(wxCommandEvent& e)
{
    wxUnusedVar(e);
    gitAction ga(gitCommitList, wxT(""));
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();
}

/*******************************************************************************/

void GitPlugin::OnShowDiffs(wxCommandEvent& e)
{
    wxUnusedVar(e);
/*    gitAction ga(gitDiffRepoShow, wxT(""));
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();*/
    GitDiffDlg dlg(m_topWindow, m_repositoryDirectory, this);
    dlg.ShowModal();
}

/*******************************************************************************/
void GitPlugin::OnApplyPatch(wxCommandEvent& e)
{
    wxUnusedVar(e);
    GitApplyPatchDlg dlg(m_topWindow);
    if(dlg.ShowModal() == wxID_OK) { ApplyPatch(dlg.GetPatchFile(), dlg.GetExtraFlags()); }
}
/*******************************************************************************/
void GitPlugin::OnPush(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(m_remotes.GetCount() == 0) {
        wxMessageBox(_("No remotes found, can't push!"), wxT("CodeLite"), wxICON_ERROR | wxOK, m_topWindow);
        return;
    }

    wxStandardID res = ::PromptForYesNoDialogWithCheckbox(_("Push all local commits?"), "GitPromptBeforePush");
    if(res == wxID_YES) {
        wxString remote = m_remotes[0];
        if(m_remotes.GetCount() > 1) {
            remote = wxGetSingleChoice(_("Select remote to push to."), _("Select remote"), m_remotes, m_topWindow);
            if(remote.IsEmpty()) { return; }
        }
        gitAction ga(gitPush, remote + wxT(" ") + m_currentBranch);
        m_gitActionQueue.push_back(ga);
        m_mgr->ShowOutputPane("Git");
        ProcessGitActionQueue();
    }
}

/*******************************************************************************/
void GitPlugin::OnPull(wxCommandEvent& e)
{
    wxString argumentString = e.GetString(); // This might be user-specified e.g. pull --rebase
    if(argumentString.empty()) { argumentString = GetAnyDefaultCommand("git_pull"); }
    argumentString.Replace("pull", "");
    argumentString.Trim(false);

    wxStandardID res =
        ::PromptForYesNoDialogWithCheckbox(_("Save all changes and pull remote changes?"), "GitPullRemoteChanges");
    if(res == wxID_YES) {
        m_mgr->SaveAll();
        if(m_console->IsDirty()) {
            gitAction ga(gitStash, wxT(""));
            m_gitActionQueue.push_back(ga);
        }
        {
            gitAction ga(gitPull, argumentString);
            m_gitActionQueue.push_back(ga);
        }
        if(m_console->IsDirty()) {
            gitAction ga(gitStashPop, wxT(""));
            m_gitActionQueue.push_back(ga);
        }
        AddDefaultActions();
        m_mgr->ShowOutputPane("Git");
        ProcessGitActionQueue();
    }
}

/*******************************************************************************/
void GitPlugin::OnResetRepository(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(wxMessageBox(_("Are you sure that you want to discard all local changes?"), _("Reset repository"), wxYES_NO,
                    m_topWindow) == wxYES) {
        gitAction ga(gitResetRepo, wxT(""));
        m_gitActionQueue.push_back(ga);
        AddDefaultActions();
        ProcessGitActionQueue();
    }
}
/*******************************************************************************/
void GitPlugin::OnStartGitk(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DirSaver ds;

    ::wxSetWorkingDirectory(m_repositoryDirectory);
    ::wxExecute(m_pathGITKExecutable);
}

/*******************************************************************************/
void GitPlugin::OnListModified(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxArrayString choices;
    std::map<wxString, wxTreeItemId>::const_iterator it;

    // First get a map of the filepaths/treeitemids of modified files
    std::map<wxString, wxTreeItemId> modifiedIDs;
    CreateFilesTreeIDsMap(modifiedIDs, true);

    for(it = modifiedIDs.begin(); it != modifiedIDs.end(); ++it) {
        if(it->second.IsOk()) choices.Add(it->first);
    }

    if(choices.GetCount() == 0) return;

    wxString choice = wxGetSingleChoice(_("Jump to modifed file"), _("Modifed files"), choices, m_topWindow);
    if(!choice.IsEmpty()) {
        wxTreeItemId id = modifiedIDs[choice];
        if(id.IsOk()) {
            m_mgr->GetTree(TreeFileView)->EnsureVisible(id);
            m_mgr->GetTree(TreeFileView)->SelectItem(id);
        }
    }
}

/*******************************************************************************/
void GitPlugin::OnGitBlame(wxCommandEvent& e)
{
    wxUnusedVar(e);

    wxString filepath = GetEditorRelativeFilepath();
    if(!filepath.empty()) { DoGitBlame(filepath); }
}
/*******************************************************************************/
wxString GitPlugin::GetEditorRelativeFilepath() const // Called by OnGitBlame or the git blame dialog
{
    IEditor* current = m_mgr->GetActiveEditor();
    if(!current || m_repositoryDirectory.empty()) { return ""; }

    // We need to be symlink-aware here on Linux, so use CLRealPath
    wxString realfilepath = CLRealPath(current->GetFileName().GetFullPath());
    wxFileName fn(realfilepath);
    fn.MakeRelativeTo(CLRealPath(m_repositoryDirectory));

    return fn.GetFullPath();
}
/*******************************************************************************/
void GitPlugin::DoGitBlame(const wxString& args) // Called by OnGitBlame or the git blame dialog
{
    gitAction ga(gitBlame, args);
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnGitBlameRevList(const wxString& arg, const wxString& filepath,
                                  const wxString& commit) // Called by the git blame dialog
{
    wxString cmt(commit);
    if(cmt.empty()) { cmt = "HEAD"; }
    wxString args = arg + ' ' + cmt + " -- " + filepath;

    gitAction ga(gitRevlist, args);
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();
}
/*******************************************************************************/

void GitPlugin::OnRefresh(wxCommandEvent& e)
{
    wxUnusedVar(e);
    gitAction ga(gitListAll, wxT(""));
    m_gitActionQueue.push_back(ga);
    AddDefaultActions();
    m_mgr->ShowOutputPane("Git");
    ProcessGitActionQueue();
}
/*******************************************************************************/
void GitPlugin::OnGarbageColletion(wxCommandEvent& e)
{
    wxUnusedVar(e);
    gitAction ga(gitGarbageCollection, wxT(""));
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();
}

/*******************************************************************************/
#if 0
void GitPlugin::OnBisectStart(wxCommandEvent& e)
{
    m_pluginToolbar->EnableTool(XRCID("git_bisect_start"),false);
    m_pluginToolbar->EnableTool(XRCID("git_bisect_good"),true);
    m_pluginToolbar->EnableTool(XRCID("git_bisect_bad"),true);
    m_pluginToolbar->EnableTool(XRCID("git_bisect_reset"),true);
}

/*******************************************************************************/

void GitPlugin::OnBisectGood(wxCommandEvent& e)
{
}

/*******************************************************************************/

void GitPlugin::OnBisectBad(wxCommandEvent& e)
{
}

/*******************************************************************************/

void GitPlugin::OnBisectReset(wxCommandEvent& e)
{
    m_pluginToolbar->EnableTool(XRCID("git_bisect_start"),true);
    m_pluginToolbar->EnableTool(XRCID("git_bisect_good"),false);
    m_pluginToolbar->EnableTool(XRCID("git_bisect_bad"),false);
    m_pluginToolbar->EnableTool(XRCID("git_bisect_reset"),false);
}
#endif
/*******************************************************************************/
void GitPlugin::OnFileSaved(clCommandEvent& e)
{
    e.Skip();
    std::map<wxString, wxTreeItemId>::const_iterator it;

    // First get an up to date map of the filepaths/treeitemids of modified files
    // (Trying to cache these results in segfaults when the tree has been modified)
    std::map<wxString, wxTreeItemId> modifiedIDs;
    CreateFilesTreeIDsMap(modifiedIDs, true);

    for(it = modifiedIDs.begin(); it != modifiedIDs.end(); ++it) {
        if(!it->second.IsOk()) {
            GIT_MESSAGE(wxT("Stored item not found in tree, rebuilding item IDs"));
            gitAction ga(gitListAll, wxT(""));
            m_gitActionQueue.push_back(ga);
            break;
        }
        DoSetTreeItemImage(m_mgr->GetTree(TreeFileView), it->second, OverlayTool::Bmp_Modified);
    }

    gitAction ga(gitListModified, wxT(""));
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();
    RefreshFileListView();
}

/*******************************************************************************/
void GitPlugin::OnFilesAddedToProject(clCommandEvent& e)
{
    e.Skip();

    const wxArrayString& files = e.GetStrings();
    if(!files.IsEmpty() && !m_repositoryDirectory.IsEmpty()) {
        GIT_MESSAGE(wxT("Files added to project, updating file list"));
        DoAddFiles(files);
        RefreshFileListView();
    }
}

/*******************************************************************************/
void GitPlugin::OnFilesRemovedFromProject(clCommandEvent& e)
{
    e.Skip();
    RefreshFileListView(); // in git world, deleting a file is enough
}

/*******************************************************************************/
void GitPlugin::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
    m_workspaceFilename = e.GetString();
    DoCleanup();
    InitDefaults();
    RefreshFileListView();

    // Try to set the repo to the workspace path
    DoSetRepoPath(GetWorkspaceFileName().GetPath(), false);
}

/*******************************************************************************/
void GitPlugin::OnInitDone(wxCommandEvent& e)
{
    e.Skip();
    m_topWindow = m_mgr->GetTheApp()->GetTopWindow();
}
/*******************************************************************************/
void GitPlugin::ProcessGitActionQueue()
{
    if(m_gitActionQueue.size() == 0) return;

    // Sanity:
    // if there is no repo and the command is not 'clone'
    // return
    gitAction ga = m_gitActionQueue.front();
    if(m_repositoryDirectory.IsEmpty() && ga.action != gitClone) {
        m_gitActionQueue.pop_front();
        return;
    }

    if(m_process) { return; }

    wxString command = m_pathGITExecutable;

    // Wrap the executable with quotes if needed
    command.Trim().Trim(false);
    ::WrapWithQuotes(command);

    switch(ga.action) {
    case gitStash:
        command << " stash";
        GIT_MESSAGE("Git repository is ditry, stashing local changes before pulling...");
        GIT_MESSAGE("%s", command);
        break;

    case gitStashPop:
        command << " stash pop";

        GIT_MESSAGE("%s", command);
        break;

    case gitRevertCommit:
        command << " revert --no-commit " << ga.arguments;
        GIT_MESSAGE("%s", command);
        break;

    case gitApplyPatch:
        command << " apply --whitespace=nowarn --ignore-whitespace " << ga.arguments;
        GIT_MESSAGE("%s", command.c_str());
        break;

    case gitRmFiles:
        command << " --no-pager rm --force " << ga.arguments;
        GIT_MESSAGE("%s", command.c_str());
        break;

    case gitClone:
        command << wxT(" --no-pager clone ") << ga.arguments;
        GIT_MESSAGE("%s", command.c_str());
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitStatus:
        command << " --no-pager status -s";
        GIT_MESSAGE1("%s", command.c_str());
        GIT_MESSAGE1(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitListAll:
        GIT_MESSAGE1(wxT("Listing files in git repository"));
        command << wxT(" --no-pager ls-files");
        GIT_MESSAGE1(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitListModified:
        GIT_MESSAGE1(wxT("Listing modified files in git repository"));
        command << wxT(" --no-pager ls-files -m");
        GIT_MESSAGE1(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitUpdateRemotes:
        GIT_MESSAGE1(wxT("Updating remotes"));
        command << wxT(" --no-pager remote update");
        break;

    case gitListRemotes:
        GIT_MESSAGE1(wxT("Listing remotes"));
        command << wxT(" --no-pager remote");
        GIT_MESSAGE1(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitAddFile:
        GIT_MESSAGE(wxT("Add file ") + ga.arguments);
        command << wxT(" --no-pager add ") << ga.arguments;
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitDeleteFile:
        GIT_MESSAGE(wxT("Delete file ") + ga.arguments);
        command << wxT(" --no-pager update-index --remove --force-remove ") << ga.arguments;
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitDiffFile:
        GIT_MESSAGE1(wxT("Diff file: ") + ga.arguments);
        command << wxT(" --no-pager show HEAD:") << ga.arguments;
        GIT_MESSAGE(wxT("%s. (Repo path: %s)"), command, m_repositoryDirectory);
        break;

    case gitDiffRepoCommit:
        command << wxT(" --no-pager diff --no-color HEAD");
        GIT_MESSAGE(wxT("%s"), command.c_str());
        ShowProgress(wxT("Obtaining diffs for modified files..."));
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitDiffRepoShow:
        command << wxT(" --no-pager diff --no-color HEAD");
        GIT_MESSAGE(wxT("%s"), command.c_str());
        ShowProgress(wxT("Obtaining diffs for modified files..."));
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitResetFile:
        GIT_MESSAGE(wxT("Reset file ") + ga.arguments);
        command << wxT(" --no-pager checkout ") << ga.arguments;
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitUndoAdd:
        GIT_MESSAGE(wxT("Reset file ") + ga.arguments);
        command << wxT(" --no-pager reset ") << ga.arguments;
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitPull:
        GIT_MESSAGE(wxT("Pull remote changes"));
        ShowProgress(wxT("Obtaining remote changes"), false);
        command << " --no-pager pull " << ga.arguments;
        command << " --log";
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitPush:
        GIT_MESSAGE(wxT("Push local changes (") + ga.arguments + wxT(")"));
        command << wxT(" --no-pager push ") << ga.arguments;
        ShowProgress(wxT("Pushing local changes..."), false);
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitCommit:
        GIT_MESSAGE(wxT("Commit local changes (") + ga.arguments + wxT(")"));
        command << wxT(" --no-pager commit ") << ga.arguments;
        ShowProgress(wxT("Committing local changes..."));
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitResetRepo:
        GIT_MESSAGE(wxT("Reset repository"));
        command << wxT(" --no-pager reset --hard");
        ShowProgress(wxT("Resetting local repository..."));
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitBranchCreate:
        GIT_MESSAGE(wxT("Create local branch ") + ga.arguments);
        command << wxT(" --no-pager branch ") << ga.arguments;
        ShowProgress(wxT("Creating local branch..."));
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitBranchCurrent:
        GIT_MESSAGE(wxT("Get current branch"));
        command << wxT(" --no-pager branch --no-color");
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitBranchList:
        GIT_MESSAGE1(wxT("List local branches"));
        command << wxT(" --no-pager branch --no-color");
        GIT_MESSAGE1(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitBranchListRemote:
        GIT_MESSAGE1(wxT("List remote branches"));
        command << wxT(" --no-pager branch -r --no-color");
        GIT_MESSAGE1(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitBranchSwitch:
        GIT_MESSAGE(wxT("Switch to local branch ") + ga.arguments);
        ShowProgress(wxT("Switching to local branch ") + ga.arguments, false);
        command << wxT(" --no-pager checkout ") << ga.arguments;
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitBranchSwitchRemote:
        GIT_MESSAGE(wxT("Switch to remote branch ") + ga.arguments);
        ShowProgress(wxT("Switching to remote branch ") + ga.arguments, false);
        command << wxT(" --no-pager checkout -b ") << ga.arguments;
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitCommitList:
        GIT_MESSAGE(wxT("Listing commits..."));
        ShowProgress(wxT("Fetching commit list"));
        // hash @ author-name @ date @ subject
        command << wxT(" --no-pager log --pretty=\"%h@%an@%ci@%s\" -n 100 ") << ga.arguments;
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitBlame:
        GIT_MESSAGE("Git blame...");
        command << " --no-pager blame --line-porcelain " << ga.arguments;
        GIT_MESSAGE("Git blame: %s", command);
        break;

    case gitRevlist:
        command << " --no-pager rev-list " << ga.arguments;
        GIT_MESSAGE("Git rev-list: %s", command);
        break;

    case gitRebase:
        GIT_MESSAGE(wxT("Rebasing.."));
        ShowProgress(wxT("Rebase with ") + ga.arguments + wxT(".."));
        command << wxT(" --no-pager rebase ") << ga.arguments;
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    case gitGarbageCollection:
        GIT_MESSAGE(wxT("Clean database.."));
        ShowProgress(wxT("Cleaning git database. This may take some time..."), false);
        command << wxT(" --no-pager gc");
        GIT_MESSAGE(wxT("%s. Repo path: %s"), command.c_str(), m_repositoryDirectory.c_str());
        break;

    default:
        GIT_MESSAGE(wxT("Unknown git action"));
        return;
    }

    IProcessCreateFlags createFlags;
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

#ifdef __WXMSW__
    if(ga.action == gitClone || ga.action == gitPush || ga.action == gitPull) {
        createFlags =
            data.GetFlags() & GitEntry::Git_Show_Terminal ? IProcessCreateConsole : IProcessCreateWithHiddenConsole;

    } else {
        createFlags = IProcessCreateWithHiddenConsole;
    }
#else
    createFlags = IProcessCreateWithHiddenConsole;

#endif

    // Set locale to english
    wxStringMap_t om;
    om.insert(std::make_pair("LC_ALL", "C"));
    om.insert(std::make_pair("GIT_MERGE_AUTOEDIT", "no"));

#ifdef __WXMSW__
    wxString homeDir;
    if(wxGetEnv("USERPROFILE", &homeDir)) { om.insert(std::make_pair("HOME", homeDir)); }
#endif
    EnvSetter es(&om);

    m_process = ::CreateAsyncProcess(this, command, createFlags,
                                     ga.workingDirectory.IsEmpty() ? m_repositoryDirectory : ga.workingDirectory);
    if(!m_process) {
        GIT_MESSAGE(wxT("Failed to execute git command!"));
        DoRecoverFromGitCommandError();
    }
}

/*******************************************************************************/
void GitPlugin::FinishGitListAction(const gitAction& ga)
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    if(!(data.GetFlags() & GitEntry::Git_Colour_Tree_View)) return;

    wxArrayString tmpArray = wxStringTokenize(m_commandOutput, wxT("\n"), wxTOKEN_STRTOK);

    // Convert path to absolute
    for(unsigned i = 0; i < tmpArray.GetCount(); ++i) {
        wxFileName fname(tmpArray[i]);
        fname.MakeAbsolute(m_repositoryDirectory);
        tmpArray[i] = fname.GetFullPath();
    }

    // convert the array to set for performance
    wxStringSet_t gitFileSet;
    gitFileSet.insert(tmpArray.begin(), tmpArray.end());

    if(ga.action == gitListAll) {
        m_mgr->SetStatusMessage(_("Colouring tracked git files..."), 0);
        ColourFileTree(m_mgr->GetTree(TreeFileView), gitFileSet, OverlayTool::Bmp_OK);
        m_trackedFiles.swap(gitFileSet);

    } else if(ga.action == gitListModified) {
        m_mgr->SetStatusMessage(_("Colouring modifed git files..."), 0);
        // Reset modified files
        ColourFileTree(m_mgr->GetTree(TreeFileView), m_modifiedFiles, OverlayTool::Bmp_OK);
        // First get an up to date map of the filepaths/treeitemids
        // (Trying to cache these results in segfaults when the tree has been modified)
        std::map<wxString, wxTreeItemId> IDs;
        CreateFilesTreeIDsMap(IDs);

        // Now filter using the list of modified files, gitFileList, to find which IDs to colour differently
        wxStringSet_t toColour;
        wxStringSet_t::const_iterator iter = gitFileSet.begin();
        for(; iter != gitFileSet.end(); ++iter) {
            wxTreeItemId id = IDs[(*iter)];
            if(id.IsOk()) {
                DoSetTreeItemImage(m_mgr->GetTree(TreeFileView), id, OverlayTool::Bmp_Modified);

            } else {
                toColour.insert(*iter);
            }
        }

        if(!toColour.empty()) { ColourFileTree(m_mgr->GetTree(TreeFileView), toColour, OverlayTool::Bmp_Modified); }

        // Finally, cache the modified-files list: it's used in other functions
        m_modifiedFiles.swap(gitFileSet);
    }
    m_mgr->SetStatusMessage("", 0);
}

/*******************************************************************************/
void GitPlugin::ListBranchAction(const gitAction& ga)
{
    wxArrayString gitList = wxStringTokenize(m_commandOutput, wxT("\n"));
    if(gitList.GetCount() == 0) return;

    wxArrayString branchList;
    for(unsigned i = 0; i < gitList.GetCount(); ++i) {
        if(!gitList[i].Contains(wxT("->"))) { branchList.Add(gitList[i].Mid(2)); }
    }

    if(branchList.Index(m_currentBranch) != wxNOT_FOUND) { branchList.Remove(m_currentBranch); }

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
    if(gitList.GetCount() == 0) return;

    m_currentBranch.Empty();
    for(unsigned i = 0; i < gitList.GetCount(); ++i) {
        if(gitList[i].StartsWith(wxT("*"))) {
            m_currentBranch = gitList[i].Mid(2);
            break;
        }
    }

    if(!m_currentBranch.IsEmpty()) {
        GIT_MESSAGE(wxT("Current branch ") + m_currentBranch);
        m_mgr->GetDockingManager()
            ->GetPane(wxT("Workspace View"))
            .Caption(wxT("Workspace View [") + m_currentBranch + wxT("]"));
        m_mgr->GetDockingManager()->Update();
    }
}
/*******************************************************************************/
void GitPlugin::UpdateFileTree()
{
    if(!m_mgr->GetWorkspace()->IsOpen()) { return; }

    if(wxMessageBox(_("Do you want to start importing new / updating changed files?"), _("Import files"), wxYES_NO,
                    m_topWindow) == wxNO) {
        return;
    }

    wxString projectName = m_mgr->GetWorkspace()->GetActiveProjectName();
    wxString error = _("Error obtaining project");
    ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(projectName, error);

    proj->BeginTranscation();
    wxString path = m_repositoryDirectory;
    if(path.EndsWith(wxT("/")) || path.EndsWith(wxT("\\"))) { path.RemoveLast(); }
    wxFileName rootPath(path);

    wxArrayString gitfiles = wxStringTokenize(m_commandOutput, wxT("\n"));
    wxArrayString files;

    // clProgressDlg *prgDlg = new clProgressDlg (m_topWindow, _("Importing files ..."), wxT(""),
    // (int)gitfiles.GetCount()+2);
    wxProgressDialog* prgDlg =
        new wxProgressDialog(_("Importing files ..."), wxT(""), (int)gitfiles.GetCount() + 2, m_topWindow);
    wxString filespec = wxT("*.cpp;*.hpp;*.c;*.h;*.ui;*.py;*.txt");
    bool extlessFiles(true);
    wxStringTokenizer tok(filespec, wxT(";"));
    std::set<wxString> specMap;
    while(tok.HasMoreTokens()) {
        wxString v = tok.GetNextToken().AfterLast(wxT('*'));
        v = v.AfterLast(wxT('.')).MakeLower();
        specMap.insert(v);
    }

    // filter non interesting files
    for(size_t i = 0; i < gitfiles.GetCount(); i++) {

        prgDlg->Update((int)i, _("Checking file ") + gitfiles.Item(i));
        wxFileName fn(m_repositoryDirectory + wxT("/") + gitfiles.Item(i));

        /* always excluded by default */
        wxString filepath = fn.GetPath();
        if(filepath.Contains(wxT(".svn")) || filepath.Contains(wxT(".cvs")) || filepath.Contains(wxT(".arch-ids")) ||
           filepath.Contains(wxT("arch-inventory")) || filepath.Contains(wxT("autom4te.cache")) ||
           filepath.Contains(wxT("BitKeeper")) || filepath.Contains(wxT(".bzr")) ||
           filepath.Contains(wxT(".bzrignore")) || filepath.Contains(wxT("CVS")) ||
           filepath.Contains(wxT(".cvsignore")) || filepath.Contains(wxT("_darcs")) ||
           filepath.Contains(wxT(".deps")) || filepath.Contains(wxT("EIFGEN")) || filepath.Contains(wxT(".git")) ||
           filepath.Contains(wxT(".gitignore")) || filepath.Contains(wxT(".hg")) || filepath.Contains(wxT("PENDING")) ||
           filepath.Contains(wxT("RCS")) || filepath.Contains(wxT("RESYNC")) || filepath.Contains(wxT("SCCS")) ||
           filepath.Contains(wxT("{arch}"))) {
            continue;
        }

        if(specMap.empty()) {
            files.Add(gitfiles.Item(i));
        } else if(fn.GetExt().IsEmpty() & extlessFiles) {
            files.Add(gitfiles.Item(i));
        } else if(specMap.find(fn.GetExt().MakeLower()) != specMap.end()) {
            files.Add(gitfiles.Item(i));
        }
    }

    prgDlg->Update(gitfiles.GetCount() + 1, _("Adding files..."));
    for(size_t i = 0; i < files.GetCount(); i++) {
        wxFileName fn(m_repositoryDirectory + wxT("/") + files.Item(i));

        // if the file already exist, skip it
        if(proj->IsFileExist(fn.GetFullPath())) { continue; }

        wxString fullpath = fn.GetFullPath();
        fn.MakeRelativeTo(path);

        wxString relativePath = fn.GetPath();
        relativePath.Replace(wxT("/"), wxT(":"));
        relativePath.Replace(wxT("\\"), wxT(":"));

        if(relativePath.IsEmpty()) {
            // the file is probably under the root, add it under
            // a virtual directory with the name of the target
            // root folder
            relativePath = rootPath.GetName();
        }
        relativePath.Append(wxT(":"));

        bool createAndAdd = false;
        wxArrayString dummyFiles;
        proj->GetFilesByVirtualDir(relativePath, dummyFiles);
        if(dummyFiles.GetCount() != 0) {
            createAndAdd = true;
        } else {
            proj->GetFilesByVirtualDir(relativePath.Left(relativePath.Find(wxT(":"))), dummyFiles);
            if(dummyFiles.GetCount() != 0) { createAndAdd = true; }
        }

        if(createAndAdd) {
            proj->CreateVirtualDir(relativePath, true);
            proj->FastAddFile(fullpath, relativePath);
        }
    }
    prgDlg->Update(gitfiles.GetCount() + 2, _("Finished adding files..."));
    prgDlg->Destroy();
    proj->CommitTranscation();

    m_mgr->ReloadWorkspace();
}

/*******************************************************************************/
void GitPlugin::OnProcessTerminated(clProcessEvent& event)
{
    HideProgress();
    if(m_gitActionQueue.empty()) return;

    gitAction ga = m_gitActionQueue.front();
    if(ga.action != gitDiffFile) {
        // Dont manipulate the output if its a diff...
        m_commandOutput.Replace(wxT("\r"), wxT(""));
    }

    if(m_commandOutput.StartsWith(wxT("fatal")) || m_commandOutput.StartsWith(wxT("error"))) {
        wxString msg = _("There was a problem while performing a git action.\n"
                         "Last command output:\n");
        msg << m_commandOutput;
        wxMessageBox(msg, _("git error"), wxICON_ERROR | wxOK, m_topWindow);
        // Last action failed, clear queue
        DoRecoverFromGitCommandError();
        return;
    }

    if(ga.action == gitListAll || ga.action == gitListModified || ga.action == gitResetRepo) {
        if(ga.action == gitListAll && m_bActionRequiresTreUpdate) {
            if(m_commandOutput.Lower().Contains(_("created"))) UpdateFileTree();
        }
        m_bActionRequiresTreUpdate = false;
        FinishGitListAction(ga);

    } else if(ga.action == gitStatus) {
        m_console->UpdateTreeView(m_commandOutput);
        FinishGitListAction(ga);

    } else if(ga.action == gitListRemotes) {
        wxArrayString gitList = wxStringTokenize(m_commandOutput, wxT("\n"));
        m_remotes = gitList;

    } else if(ga.action == gitDiffFile) {

        // Show the diff in the diff-viewer
        DoShowDiffViewer(m_commandOutput, ga.arguments);

    } else if(ga.action == gitDiffRepoCommit) {
        wxString commitArgs;
        DoShowCommitDialog(m_commandOutput, commitArgs);
        if(!commitArgs.IsEmpty()) {
            gitAction ga(gitCommit, commitArgs);
            m_gitActionQueue.push_back(ga);
            AddDefaultActions();
        }

    } else if(ga.action == gitBlame) {
        if(!m_gitBlameDlg) { m_gitBlameDlg = new GitBlameDlg(m_topWindow, this); }
        m_gitBlameDlg->SetBlame(m_commandOutput, ga.arguments);
        m_gitBlameDlg->Show();
        m_gitBlameDlg->SetFocus();

    } else if(ga.action == gitRevlist) {
        if(m_gitBlameDlg) { m_gitBlameDlg->OnRevListOutput(m_commandOutput, ga.arguments); }

    } else if(ga.action == gitDiffRepoShow) {
        // This is now dealt with by GitDiffDlg itself
//        GitDiffDlg dlg(m_topWindow, m_repositoryDirectory, this);
//        dlg.SetDiff(m_commandOutput);
//        dlg.ShowModal();

    } else if(ga.action == gitResetFile || ga.action == gitApplyPatch) {
        EventNotifier::Get()->PostReloadExternallyModifiedEvent(true);

        gitAction newAction;
        newAction.action = gitListModified;
        m_gitActionQueue.push_back(newAction);

    } else if(ga.action == gitBranchCurrent) {
        GetCurrentBranchAction(ga);

    } else if(ga.action == gitBranchList || ga.action == gitBranchListRemote) {
        ListBranchAction(ga);

    } else if(ga.action == gitBranchSwitch || ga.action == gitBranchSwitchRemote || ga.action == gitPull ||
              ga.action == gitResetRepo) {
        if(ga.action == gitPull) {
            if(m_commandOutput.Contains(wxT("Already"))) {
                wxMessageBox(_("Nothing to pull, already up-to-date."), wxT("CodeLite"), wxICON_INFORMATION | wxOK,
                             m_topWindow);
            } else {

                wxString log = m_commandOutput.Mid(m_commandOutput.Find(wxT("From")));
                // Write the pull log to the console
                m_console->AddText(wxString() << "\n===============\nPull Log\n===============\n" << log << "\n");

                if(m_commandOutput.Contains(wxT("Merge made by"))) {
                    if(wxMessageBox(_("Merged after pull. Rebase?"), _("Rebase"), wxYES_NO, m_topWindow) == wxYES) {
                        wxString selection;
                        if(m_remotes.GetCount() > 1) {
                            selection = wxGetSingleChoice(_("Rebase with what branch?"), _("Rebase"),
                                                          m_remoteBranchList, m_topWindow);
                        } else {
                            selection = m_remotes[0] + wxT("/") + m_currentBranch;
                            if(wxMessageBox(_("Rebase with ") + selection + wxT("?"), _("Rebase"), wxYES_NO,
                                            m_topWindow) == wxNO)
                                selection.Empty();
                        }

                        if(selection.IsEmpty()) return;

                        gitAction ga(gitRebase, selection);
                        m_gitActionQueue.push_back(ga);
                    }
                } else if(m_commandOutput.Contains(wxT("CONFLICT"))) {
                    wxMessageBox(wxT("There was a conflict during merge.\n"
                                     "Please resolve conflicts and commit by hand.\n"
                                     "After resolving conflicts, be sure to reload the current project."),
                                 _("Conflict found during merge"), wxOK, m_topWindow);
                }
                if(m_commandOutput.Contains(wxT("Updating"))) m_bActionRequiresTreUpdate = true;

                if(m_bActionRequiresTreUpdate) {
                    // Post event about file system updated
                    clFileSystemEvent fsEvent(wxEVT_FILE_SYSTEM_UPDATED);
                    fsEvent.SetPath(m_repositoryDirectory);
                    EventNotifier::Get()->AddPendingEvent(fsEvent);
                }
            }

        } else if(ga.action == gitBranchSwitch || ga.action == gitBranchSwitchRemote) {
            // update the tree
            gitAction ga(gitListAll, wxT(""));
            m_gitActionQueue.push_back(ga);
            ga.action = gitListModified;
            m_gitActionQueue.push_back(ga);
        }

        // Reload files if needed
        EventNotifier::Get()->PostReloadExternallyModifiedEvent(true);

    } else if(ga.action == gitCommitList) {
        if(m_commitListDlg) {
            m_commitListDlg->SetCommitList(m_commandOutput);
        } else {
            m_commitListDlg = new GitCommitListDlg(m_topWindow, m_repositoryDirectory, this);
            m_commitListDlg->SetCommitList(m_commandOutput);
        }
        m_commitListDlg->Show();

    } else if(ga.action == gitRevertCommit) {
        AddDefaultActions();
    }

    if(ga.action == gitResetRepo || ga.action == gitResetFile) {
        // Reload externally modified files
        CL_DEBUG("Git: posting a 'reload externally modified files' event");
        EventNotifier::Get()->PostReloadExternallyModifiedEvent(true);
    }

    wxDELETE(m_process);
    m_commandOutput.Clear();
    m_gitActionQueue.pop_front();

#ifdef __WXGTK__
    int statLoc;
    ::waitpid(-1, &statLoc, WNOHANG);
#endif

    ProcessGitActionQueue();
}

/*******************************************************************************/
void GitPlugin::OnProcessOutput(clProcessEvent& event)
{
    wxString output = event.GetOutput();
    gitAction ga;
    if(!m_gitActionQueue.empty()) { ga = m_gitActionQueue.front(); }

    if(m_console->IsVerbose() || ga.action == gitPush || ga.action == gitPull) m_console->AddRawText(output);
    m_commandOutput.Append(output);

    // Handle password required
    wxString tmpOutput = output;
    tmpOutput.Trim().Trim(false);
    tmpOutput.MakeLower();

    if(ga.action != gitDiffRepoCommit && ga.action != gitDiffFile && ga.action != gitCommitList &&
       ga.action != gitDiffRepoShow && ga.action != gitBlame && ga.action != gitRevlist)

    {
        if(tmpOutput.Contains("username for")) {
            // username is required
            wxString username = ::wxGetTextFromUser(output);
            if(username.IsEmpty()) {
                m_process->Terminate();
            } else {
                m_process->WriteToConsole(username);
            }

        } else if(tmpOutput.Contains("commit-msg hook failure") || tmpOutput.Contains("pre-commit hook failure")) {
            m_process->Terminate();
            ::wxMessageBox(output, "Git", wxICON_ERROR | wxCENTER | wxOK, EventNotifier::Get()->TopFrame());

        } else if(tmpOutput.Contains("*** please tell me who you are")) {
            m_process->Terminate();
            ::wxMessageBox(output, "Git", wxICON_ERROR | wxCENTER | wxOK, EventNotifier::Get()->TopFrame());

        } else if(tmpOutput.EndsWith("password:") || tmpOutput.Contains("password for")) {

            // Password is required
            wxString pass = ::wxGetPasswordFromUser(output);
            if(pass.IsEmpty()) {

                // No point on continuing
                m_process->Terminate();

            } else {

                // write the password
                m_process->WriteToConsole(pass);
            }
        } else if((tmpOutput.Contains("the authenticity of host") && tmpOutput.Contains("can't be established")) ||
                  tmpOutput.Contains("key fingerprint")) {
            if(::wxMessageBox(tmpOutput, _("Are you sure you want to continue connecting"),
                              wxYES_NO | wxCENTER | wxICON_QUESTION) == wxYES) {
                m_process->WriteToConsole("yes");

            } else {
                m_process->Terminate();
            }
        }
    }

    if(m_console->IsProgressShown()) {
        wxString message = output.Left(output.Find(':'));
        int percent = output.Find('%', true);
        if(percent != wxNOT_FOUND) {

            wxString number = output.Mid(percent - 3, 3);
            number.Trim(false);
            unsigned long current;
            if(number.ToULong(&current)) {
                message.Prepend(m_progressMessage + " Status: ");
                m_console->UpdateProgress(current, message);
            }
        }
    }
}

/*******************************************************************************/
void GitPlugin::InitDefaults()
{
    DoCreateTreeImages();
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    if(data.GetTrackedFileColour().IsOk()) { m_colourTrackedFile = data.GetTrackedFileColour(); }
    if(data.GetDiffFileColour().IsOk()) { m_colourDiffFile = data.GetDiffFileColour(); }
    if(!data.GetGITExecutablePath().IsEmpty()) { m_pathGITExecutable = data.GetGITExecutablePath(); }
    if(!data.GetGITKExecutablePath().IsEmpty()) { m_pathGITKExecutable = data.GetGITKExecutablePath(); }

    LoadDefaultGitCommands(data); // Always do this, in case of new entries
    conf.WriteItem(&data);
    conf.Save();

    wxString repoPath;
    if(IsWorkspaceOpened()) {
        repoPath = data.GetPath(GetWorkspaceName());
    } else {
        repoPath = ::wxGetCwd();
    }

    if(!repoPath.IsEmpty() && wxFileName::DirExists(repoPath + wxFileName::GetPathSeparator() + wxT(".git"))) {
        m_repositoryDirectory = repoPath;

    } else {
        DoCleanup();
    }

    if(!m_repositoryDirectory.IsEmpty()) {
        GIT_MESSAGE(wxT("intializing git on %s"), m_repositoryDirectory.c_str());
#if 0
        m_pluginToolbar->EnableTool(XRCID("git_bisect_start"),true);
        m_pluginToolbar->EnableTool(XRCID("git_bisect_good"),false);
        m_pluginToolbar->EnableTool(XRCID("git_bisect_bad"),false);
        m_pluginToolbar->EnableTool(XRCID("git_bisect_reset"),false);
#endif
        gitAction ga(gitListAll, wxT(""));
        m_gitActionQueue.push_back(ga);
        AddDefaultActions();
        ProcessGitActionQueue();
    }
}

/*******************************************************************************/
void GitPlugin::AddDefaultActions()
{
    gitAction ga(gitBranchCurrent, wxT(""));
    m_gitActionQueue.push_back(ga);

    //    ga.action = gitListAll;
    //    m_gitActionQueue.push_back(ga);

    ga.action = gitListModified;
    m_gitActionQueue.push_back(ga);

    // ga.action = gitUpdateRemotes;
    // m_gitActionQueue.push_back(ga);

    ga.action = gitBranchList;
    m_gitActionQueue.push_back(ga);

    ga.action = gitBranchListRemote;
    m_gitActionQueue.push_back(ga);

    ga.action = gitListRemotes;
    m_gitActionQueue.push_back(ga);

    ga.action = gitStatus;
    m_gitActionQueue.push_back(ga);
}

/*******************************************************************************/
void GitPlugin::ColourFileTree(wxTreeCtrl* tree, const wxStringSet_t& files, OverlayTool::BmpType bmpType) const
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    if(!(data.GetFlags() & GitEntry::Git_Colour_Tree_View)) return;

    std::stack<wxTreeItemId> items;
    if(tree->GetRootItem().IsOk()) items.push(tree->GetRootItem());

    while(!items.empty()) {
        wxTreeItemId next = items.top();
        items.pop();

        if(next != tree->GetRootItem()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(tree->GetItemData(next));
            const wxString& path = data->GetData().GetFile();
            if(!path.IsEmpty() && files.count(path)) { DoSetTreeItemImage(tree, next, bmpType); }
        }

        wxTreeItemIdValue cookie;
        wxTreeItemId nextChild = tree->GetFirstChild(next, cookie);
        while(nextChild.IsOk()) {
            items.push(nextChild);
            nextChild = tree->GetNextSibling(nextChild);
        }
    }
}

/*******************************************************************************/

void GitPlugin::CreateFilesTreeIDsMap(std::map<wxString, wxTreeItemId>& IDs, bool ifmodified /*=false*/) const
{
    wxTreeCtrl* tree = m_mgr->GetTree(TreeFileView);
    if(!tree) { return; }

    IDs.clear();

    std::stack<wxTreeItemId> items;
    if(tree->GetRootItem().IsOk()) items.push(tree->GetRootItem());

    while(!items.empty()) {
        wxTreeItemId next = items.top();
        items.pop();

        if(next != tree->GetRootItem()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(tree->GetItemData(next));
            const wxString& path = data->GetData().GetFile();
            if(!path.IsEmpty()) {
                // If m_modifiedFiles has already been filled, only include files listed there
                if(!ifmodified || m_modifiedFiles.count(path)) { IDs[path] = next; }
            }
        }

        wxTreeItemIdValue cookie;
        wxTreeItemId nextChild = tree->GetFirstChild(next, cookie);
        while(nextChild.IsOk()) {
            items.push(nextChild);
            nextChild = tree->GetNextSibling(nextChild);
        }
    }
}

/*******************************************************************************/
void GitPlugin::OnProgressTimer(wxTimerEvent& Event)
{
    if(m_console->IsProgressShown()) m_console->PulseProgress();
}

/*******************************************************************************/
void GitPlugin::ShowProgress(const wxString& message, bool pulse)
{
    m_console->ShowProgress(message, pulse);

    if(pulse) {
        m_progressTimer.Start(50);

    } else {
        m_progressMessage = message;
    }
}

/*******************************************************************************/
void GitPlugin::HideProgress()
{
    m_console->HideProgress();
    m_progressTimer.Stop();
}

void GitPlugin::OnEnableGitRepoExists(wxUpdateUIEvent& e) { e.Enable(m_repositoryDirectory.IsEmpty() == false); }

void GitPlugin::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    StoreWorkspaceRepoDetails();
    WorkspaceClosed();
}

void GitPlugin::DoCleanup()
{
    m_gitActionQueue.clear();
    m_repositoryDirectory.Clear();
    m_remotes.Clear();
    m_localBranchList.Clear();
    m_remoteBranchList.Clear();
    m_trackedFiles.clear();
    m_modifiedFiles.clear();
    m_addedFiles = false;
    m_progressMessage.Clear();
    m_commandOutput.Clear();
    m_bActionRequiresTreUpdate = false;
    wxDELETE(m_process);
    m_mgr->GetDockingManager()->GetPane(wxT("Workspace View")).Caption(wxT("Workspace View"));
    m_mgr->GetDockingManager()->Update();
    m_filesSelected.Clear();
    m_selectedFolder.Clear();
}

void GitPlugin::DoCreateTreeImages()
{
// We update the tree view with new icons:
// each icon will get an additional of 2 icons:
// modified / OK
// the index will be: m_baseImageCount + img-base + 1 => OK
//                    m_baseImageCount + img-base + 2 => Modified
#if 0
    if(m_treeImageMapping.empty()) {
        wxTreeCtrl* tree = m_mgr->GetTree(TreeFileView);

        // Create 2 sets: modified & normal
        wxImageList* il = tree->GetImageList();
        m_baseImageCount = il->GetImageCount();

        for(int i = 0; i < m_baseImageCount; ++i) {
            // we also keep a mapping of the new image to its base image
            // The ordeer of adding the images is important since we will use this enumerators (OverlayTool::Bmp_OK etc)
            // to choose the correct
            // image when colouring the tree
            m_treeImageMapping.insert(
                std::make_pair(il->Add(OverlayTool::Get().CreateBitmap(il->GetBitmap(i), OverlayTool::Bmp_OK)), i));
            m_treeImageMapping.insert(std::make_pair(
                il->Add(OverlayTool::Get().CreateBitmap(il->GetBitmap(i), OverlayTool::Bmp_Modified)), i));
            m_treeImageMapping.insert(std::make_pair(i, i));
        }
    }
#endif
}

void GitPlugin::DoSetTreeItemImage(wxTreeCtrl* ctrl, const wxTreeItemId& item, OverlayTool::BmpType bmpType) const
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    if(!(data.GetFlags() & GitEntry::Git_Colour_Tree_View)) return;

    // get the base image first
    int curImgIdx = ctrl->GetItemImage(item);
    if(m_treeImageMapping.count(curImgIdx)) {
        int baseImg = m_treeImageMapping.find(curImgIdx)->second;

        // now get the new image index based on the following:
        // baseCount + (imgIdx * bitmapCount) + BmpType
        int newImg = m_baseImageCount + (baseImg * 2) + bmpType;

        // the below condition should never met, but I am paranoid..
        if(ctrl->GetImageList()->GetImageCount() > newImg) {
            ctrl->SetItemImage(item, newImg, wxTreeItemIcon_Selected);
            ctrl->SetItemImage(item, newImg, wxTreeItemIcon_Normal);
        }
    }
}

void GitPlugin::OnClone(wxCommandEvent& e)
{
    gitCloneDlg dlg(wxTheApp->GetTopWindow());
    if(dlg.ShowModal() == wxID_OK) {
        gitAction ga;
        ga.action = gitClone;
        ga.arguments = dlg.GetCloneURL();
        ga.workingDirectory = dlg.GetTargetDirectory();
        m_gitActionQueue.push_back(ga);
        ProcessGitActionQueue();
        RefreshFileListView();
    }
}

void GitPlugin::DoAddFiles(const wxArrayString& files)
{
    m_addedFiles = true;

    wxString filesToAdd;
    for(size_t i = 0; i < files.GetCount(); ++i) {
        wxFileName fn(files.Item(i));
        wxString file = fn.GetFullPath();
        ::WrapWithQuotes(file);
        filesToAdd << file << " ";
    }

    gitAction ga(gitAddFile, filesToAdd);
    m_gitActionQueue.push_back(ga);

    AddDefaultActions();
    ProcessGitActionQueue();
}

void GitPlugin::DoResetFiles(const wxArrayString& files)
{
    wxString filesToDelete;
    for(size_t i = 0; i < files.GetCount(); ++i) {
        wxFileName fn(files.Item(i));
        wxString file = fn.GetFullPath();
        ::WrapWithQuotes(file);
        filesToDelete << file << " ";
    }

    gitAction ga(gitResetFile, filesToDelete);
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();
    AddDefaultActions();
    RefreshFileListView();
}

void GitPlugin::UndoAddFiles(const wxArrayString& files)
{
    wxString filesToDelete;
    for(size_t i = 0; i < files.GetCount(); ++i) {
        wxFileName fn(files.Item(i));
        if(fn.IsAbsolute()) fn.MakeAbsolute(m_repositoryDirectory);
        filesToDelete << "\"" << fn.GetFullPath() << "\" ";
    }

    gitAction ga(gitUndoAdd, filesToDelete);
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();

    RefreshFileListView();
}

void GitPlugin::RefreshFileListView()
{
    gitAction ga;
    ga.action = gitStatus;
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();
}

void GitPlugin::DoGetFileViewSelectedFiles(wxArrayString& files, bool relativeToRepo)
{
    files.Clear();
    wxTreeCtrl* tree = m_mgr->GetTree(TreeFileView);
    if(!tree) return;

    wxArrayTreeItemIds items;
    tree->GetSelections(items);

    for(size_t i = 0; i < items.GetCount(); ++i) {
        FilewViewTreeItemData* itemData = dynamic_cast<FilewViewTreeItemData*>(tree->GetItemData(items.Item(i)));
        if(itemData && itemData->GetData().GetKind() == ProjectItem::TypeFile) {
            // we got a file
            wxFileName fn(itemData->GetData().GetFile());
            if(relativeToRepo && fn.IsAbsolute()) { fn.MakeRelativeTo(m_repositoryDirectory); }

            wxString filename = fn.GetFullPath();
            if(filename.Contains(" ")) { filename.Prepend("\"").Append("\""); }
            files.Add(filename);
        }
    }
}

void GitPlugin::DoShowDiffsForFiles(const wxArrayString& files, bool useFileAsBase)
{
    for(size_t i = 0; i < files.GetCount(); ++i) {

        // and finally, perform the action
        // File name should be relative to the repo
        wxFileName fn(files.Item(i));
        fn.MakeRelativeTo(m_repositoryDirectory);
        gitAction ga(gitDiffFile, fn.GetFullPath(wxPATH_UNIX));
        m_gitActionQueue.push_back(ga);
    }

    ProcessGitActionQueue();
}

void GitPlugin::OnStartGitkUI(wxUpdateUIEvent& e)
{
    e.Enable(!m_repositoryDirectory.IsEmpty() && !m_pathGITKExecutable.IsEmpty());
}

void GitPlugin::ApplyPatch(const wxString& filename, const wxString& extraFlags)
{
    gitAction ga(gitApplyPatch, wxString() << extraFlags << " \"" << filename << "\" ");
    m_gitActionQueue.push_back(ga);

    // Trigger a refresh
    gitAction gaStatus(gitStatus, "");
    m_gitActionQueue.push_back(gaStatus);

    ProcessGitActionQueue();
}

void GitPlugin::OnWorkspaceConfigurationChanged(wxCommandEvent& e)
{
    e.Skip();

    // Trigger a refresh
    gitAction gaStatus(gitStatus, "");
    m_gitActionQueue.push_back(gaStatus);
    ProcessGitActionQueue();
}

wxString GitPlugin::GetWorkspaceName() const { return m_workspaceFilename.GetName(); }

bool GitPlugin::IsWorkspaceOpened() const { return m_workspaceFilename.IsOk(); }

wxFileName GitPlugin::GetWorkspaceFileName() const { return m_workspaceFilename; }

void GitPlugin::RevertCommit(const wxString& commitId)
{
    gitAction ga(gitRevertCommit, commitId);
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();
}

void GitPlugin::LoadDefaultGitCommands(GitEntry& data, bool overwrite /*= false*/)
{
    static const char* commands[] = { //  ID_String|MenuLabel,Command;MenuLabel,Command; ...
                                      "git_pull|git pull,pull;git pull --rebase,pull --rebase",
                                      "git_rebase|git rebase,git rebase;git rebase --continue,rebase --continue"
    };

    const size_t items = sizeof(commands) / sizeof(char*);
    for(size_t n = 0; n < items; ++n) {
        wxString item = commands[n];

        wxString name = item.BeforeFirst('|');
        if(name.empty() || name.Len() == item.Len()) {
            wxASSERT(name.empty() || name.Len() == item.Len());
            continue;
        }

        if(!overwrite && !data.GetGitCommandsEntries(name).GetCommands().empty()) { continue; }

        GitCommandsEntries gce(name);
        vGitLabelCommands_t commandEntries;
        wxArrayString entries = wxStringTokenize(item.AfterFirst('|'), ";");

        for(size_t entry = 0; entry < entries.GetCount(); ++entry) {
            wxString label = entries.Item(entry).BeforeFirst(',');
            wxString command = entries.Item(entry).AfterFirst(',');
            wxASSERT(!label.empty() && !command.empty());
            if(!label.empty() && !command.empty()) { commandEntries.push_back(GitLabelCommand(label, command)); }
        }

        gce.SetCommands(commandEntries);
        data.DeleteGitCommandsEntry(name);
        data.AddGitCommandsEntry(gce, name);
    }
}

void GitPlugin::DoShowDiffViewer(const wxString& headFile, const wxString& fileName)
{
    // Write the content of the head file to a temporary file
    wxFileName tmpFile(wxFileName::CreateTempFileName("gittmp"));
    wxFileName fnWorkingCopy(fileName);
    fnWorkingCopy.MakeAbsolute(m_repositoryDirectory);

    tmpFile.SetExt(wxFileName(fileName).GetExt());
    wxString tmpFilePath = tmpFile.GetFullPath();
    wxFFile fp(tmpFilePath, "w+b");
    if(fp.IsOpened()) {
        fp.Write(headFile);
        fp.Close();
    }
    
    // Show diff frame
    DiffSideBySidePanel::FileInfo l(tmpFilePath, _("HEAD version"), true);
    l.deleteOnExit = true;
    DiffSideBySidePanel::FileInfo r(fnWorkingCopy.GetFullPath(), _("Working copy"), false);
    clDiffFrame* diffView = new clDiffFrame(EventNotifier::Get()->TopFrame(), l, r, true);
    diffView->Show();
}

void GitPlugin::OnRebase(wxCommandEvent& e)
{
    wxString argumentString = e.GetString(); // This might be user-specified e.g. rebase --continue
    if(argumentString.empty()) { argumentString = GetAnyDefaultCommand("git_rebase"); }
    argumentString.Replace("rebase", "");
    argumentString.Trim(false);

    wxStandardID res = ::PromptForYesNoDialogWithCheckbox(_("Save all changes and rebase?"), "GitRebaseChanges");
    if(res == wxID_YES) {
        m_mgr->SaveAll();
        if(m_console->IsDirty()) {
            gitAction ga(gitStash, wxT(""));
            m_gitActionQueue.push_back(ga);
        }
        {
            gitAction ga(gitRebase, argumentString);
            m_gitActionQueue.push_back(ga);
        }
        if(m_console->IsDirty()) {
            gitAction ga(gitStashPop, wxT(""));
            m_gitActionQueue.push_back(ga);
        }
        AddDefaultActions();
        ProcessGitActionQueue();
    }
}

void GitPlugin::OnMainFrameTitle(clCommandEvent& e)
{
    // By default - skip it
    e.Skip();
    if(!m_currentBranch.IsEmpty() && !m_repositoryDirectory.IsEmpty()) {
        wxString newTitle;
        newTitle << e.GetString() << wxT(" - [git: ") << m_currentBranch << "]";
        e.SetString(newTitle);
        e.Skip(false);
    }
}

void GitPlugin::DoRecoverFromGitCommandError()
{
    // Last action failed, clear queue
    while(!m_gitActionQueue.empty()) {
        m_gitActionQueue.pop_front();
    }

    wxDELETE(m_process);
    m_commandOutput.Clear();
}

void GitPlugin::OnFileMenu(clContextMenuEvent& event)
{
    event.Skip();
    wxMenu* menu = new wxMenu();
    wxMenu* parentMenu = event.GetMenu();
    m_filesSelected = event.GetStrings();

    BitmapLoader* bmps = m_mgr->GetStdIcons();
    wxMenuItem* item = new wxMenuItem(menu, XRCID("git_add_file"), _("Add file"));
    item->SetBitmap(bmps->LoadBitmap("plus"));
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("git_reset_file"), _("Reset file"));
    item->SetBitmap(bmps->LoadBitmap("clean"));
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("git_diff_file"), _("Show file diff"));
    item->SetBitmap(bmps->LoadBitmap("diff"));
    menu->Append(item);

    menu->AppendSeparator();
    item = new wxMenuItem(menu, XRCID("git_blame_file"), _("Show Git Blame"));
    item->SetBitmap(bmps->LoadBitmap("finger"));
    menu->Append(item);

    item = new wxMenuItem(parentMenu, wxID_ANY, _("Git"), "", wxITEM_NORMAL, menu);
    item->SetBitmap(bmps->LoadBitmap("git"));
    parentMenu->AppendSeparator();
    parentMenu->Append(item);
}

void GitPlugin::OnOpenMSYSGit(wxCommandEvent& e)
{
    GitEntry entry;
    wxString bashcommand = entry.Load().GetGitShellCommand();
    if(!bashcommand.IsEmpty()) {
        DirSaver ds;
        IEditor* editor = m_mgr->GetActiveEditor();
        if(editor) { ::wxSetWorkingDirectory(editor->GetFileName().GetPath()); }
        ::WrapInShell(bashcommand);
        ::wxExecute(bashcommand);
    } else {
        ::wxMessageBox(_("Don't know how to start MSYSGit..."), "Git", wxICON_WARNING | wxOK | wxCENTER);
    }
}

void GitPlugin::OnFolderMenu(clContextMenuEvent& event)
{
    event.Skip();
    wxMenu* menu = new wxMenu();
    wxMenu* parentMenu = event.GetMenu();
    m_selectedFolder = event.GetPath();

    BitmapLoader* bmps = m_mgr->GetStdIcons();
    wxMenuItem* item = new wxMenuItem(menu, XRCID("git_pull_rebase_folder"), _("Pull remote changes"));
    item->SetBitmap(bmps->LoadBitmap("pull"));
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("git_commit_folder"), _("Commit"));
    item->SetBitmap(bmps->LoadBitmap("git-commit"));
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("git_push_folder"), _("Push"));
    item->SetBitmap(bmps->LoadBitmap("up"));
    menu->Append(item);

    menu->AppendSeparator();

    item = new wxMenuItem(menu, XRCID("git_stash_folder"), _("Stash"));
    item->SetBitmap(bmps->LoadBitmap("down"));
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("git_stash_pop_folder"), _("Stash pop"));
    item->SetBitmap(bmps->LoadBitmap("up"));
    menu->Append(item);

#ifdef __WXMSW__
    menu->AppendSeparator();
    item = new wxMenuItem(menu, XRCID("git_bash_folder"), _("Open git bash"));
    item->SetBitmap(bmps->LoadBitmap("console"));
    menu->Append(item);
#endif

    item = new wxMenuItem(parentMenu, wxID_ANY, _("Git"), "", wxITEM_NORMAL, menu);
    item->SetBitmap(bmps->LoadBitmap("git"));
    parentMenu->AppendSeparator();
    parentMenu->Append(item);
}

void GitPlugin::OnFolderPullRebase(wxCommandEvent& event)
{
    // Just perform a 'pull --rebase'
    // if an error occurs, let the user handle it first
    GitCmd::Vec_t commands;
    commands.push_back(GitCmd("pull --rebase", IProcessCreateConsole));
    DoExecuteCommands(commands, m_selectedFolder);
    m_selectedFolder.Clear();
}

void GitPlugin::OnCommandEnded(clCommandEvent& event)
{
    m_commandProcessor->Unbind(wxEVT_COMMAND_PROCESSOR_OUTPUT, &GitPlugin::OnCommandOutput, this);
    m_commandProcessor->Unbind(wxEVT_COMMAND_PROCESSOR_ENDED, &GitPlugin::OnCommandEnded, this);
    m_commandProcessor = NULL;

    // Perform a tree refresh
    wxCommandEvent dummy;
    OnRefresh(dummy);
}

void GitPlugin::OnCommandOutput(clCommandEvent& event)
{
    m_console->AddText(event.GetString());
    wxString processOutput = event.GetString();
    processOutput.MakeLower();
    if(processOutput.Contains("username for")) {
        wxString user = ::wxGetTextFromUser(event.GetString(), "Git");
        if(!user.IsEmpty()) { event.SetString(user); }
    }
    if(processOutput.Contains("password for")) {
        wxString pass = ::wxGetPasswordFromUser(event.GetString(), "Git");
        if(!pass.IsEmpty()) { event.SetString(pass); }
    } else if(processOutput.Contains("fatal:") || processOutput.Contains("not a git repository")) {
        // prompt the user for the error
        ::wxMessageBox(processOutput, "Git", wxICON_WARNING | wxCENTER | wxOK);
    }
}

void GitPlugin::DoExecuteCommands(const GitCmd::Vec_t& commands, const wxString& workingDir)
{
    if(commands.empty()) return;

    if(m_commandProcessor) {
        // another command is already running, don't do anything
        return;
    }

    wxString command = m_pathGITExecutable;
    // Wrap the executable with quotes if needed
    command.Trim().Trim(false);
    ::WrapWithQuotes(command);
    command << " --no-pager ";
    m_commandProcessor =
        new clCommandProcessor(command + commands.at(0).baseCommand, workingDir, commands.at(0).processFlags);
    clCommandProcessor* cur = m_commandProcessor;
    for(size_t i = 1; i < commands.size(); ++i) {
        clCommandProcessor* next =
            new clCommandProcessor(command + commands.at(i).baseCommand, workingDir, commands.at(i).processFlags);
        cur = cur->Link(next);
    }
    m_commandProcessor->Bind(wxEVT_COMMAND_PROCESSOR_OUTPUT, &GitPlugin::OnCommandOutput, this);
    m_commandProcessor->Bind(wxEVT_COMMAND_PROCESSOR_ENDED, &GitPlugin::OnCommandEnded, this);
    m_mgr->ShowOutputPane("Git");
    m_commandProcessor->ExecuteCommand();
}

void GitPlugin::DoShowCommitDialog(const wxString& diff, wxString& commitArgs)
{
    wxString lastCommitString, commitHistory;
    // Find the title/body message of the previous commit, in case the user wants to amend it
    DoExecuteCommandSync("log -1 --pretty=format:\"%B\"", m_repositoryDirectory, lastCommitString);
    // Similarly the # and title of the last 100 commits for the commit history
    DoExecuteCommandSync("log -100 --abbrev-commit --pretty=oneline", m_repositoryDirectory, commitHistory);

    commitArgs.Clear();
    GitCommitDlg dlg(m_topWindow, this, m_repositoryDirectory);
    dlg.AppendDiff(diff);
    dlg.SetPreviousCommitMessage(lastCommitString);
    dlg.SetHistory(commitHistory);
    if(dlg.ShowModal() == wxID_OK) {
        if(dlg.GetSelectedFiles().IsEmpty() && !dlg.IsAmending()) return;
        wxString message = dlg.GetCommitMessage();
        if(!message.IsEmpty() || dlg.IsAmending()) {

            // amending?
            if(dlg.IsAmending()) { commitArgs << " --amend "; }

            // Add the message
            if(!message.IsEmpty()) {
                commitArgs << "-m \"";
                commitArgs << message;
                commitArgs << "\" ";

            } else {
                // we are amending previous commit, use the previous commit message
                // by passing the --no-edit switch
                commitArgs << " --no-edit ";
            }
            wxArrayString selectedFiles = dlg.GetSelectedFiles();
            for(unsigned i = 0; i < selectedFiles.GetCount(); ++i)
                commitArgs << selectedFiles.Item(i) << wxT(" ");

        } else {
            wxMessageBox(_("No commit message given, aborting."), wxT("CodeLite"), wxICON_ERROR | wxOK, m_topWindow);
        }
    }
}

void GitPlugin::OnFolderCommit(wxCommandEvent& event)
{
    // 1. Get diff output
    wxString diff;
    bool res = DoExecuteCommandSync("diff --no-color HEAD", m_selectedFolder, diff);
    if(!diff.IsEmpty()) {
        wxString commitArgs;
        DoShowCommitDialog(diff, commitArgs);
        if(!commitArgs.IsEmpty()) {
            GitCmd::Vec_t commands;
            commands.push_back(GitCmd("commit " + commitArgs, IProcessCreateDefault));
            DoExecuteCommands(commands, m_selectedFolder);
        }
    } else if(res) {
        ::wxMessageBox(_("All files are up-to-date!"), "CodeLite");
    }
}

bool GitPlugin::DoExecuteCommandSync(const wxString& command, const wxString& workingDir, wxString& commandOutput)
{
    commandOutput.Clear();
    wxString git = m_pathGITExecutable;
    // Wrap the executable with quotes if needed
    git.Trim().Trim(false);
    ::WrapWithQuotes(git);
    git << " --no-pager ";
    git << command;

    GetConsole()->AddRawText("[" + workingDir + "] " + git + "\n");
    IProcess::Ptr_t gitProc(::CreateSyncProcess(git, IProcessCreateSync, workingDir));
    if(gitProc) {
        gitProc->WaitForTerminate(commandOutput);
    } else {
        return false;
    }

    const wxString lcOutput = commandOutput.Lower();
    if(lcOutput.Contains("fatal:") || lcOutput.Contains("not a git repository")) {
        // prompt the user for the error
        ::wxMessageBox(commandOutput, "Git", wxICON_WARNING | wxCENTER | wxOK);
        commandOutput.clear();
        return false;
    }
    return true;
}

void GitPlugin::OnFolderPush(wxCommandEvent& event)
{
    GitCmd::Vec_t commands;
    commands.push_back(GitCmd("push", IProcessCreateConsole));
    DoExecuteCommands(commands, m_selectedFolder);
}

void GitPlugin::OnFolderStash(wxCommandEvent& event)
{
    GitCmd::Vec_t commands;
    commands.push_back(GitCmd("stash", IProcessCreateDefault));
    DoExecuteCommands(commands, m_selectedFolder);
}

void GitPlugin::OnFolderStashPop(wxCommandEvent& event)
{
    GitCmd::Vec_t commands;
    commands.push_back(GitCmd("stash pop", IProcessCreateDefault));
    DoExecuteCommands(commands, m_selectedFolder);
}

void GitPlugin::OnFolderGitBash(wxCommandEvent& event)
{
    GitLocator locator;
    wxString bashcommand;
    if(locator.MSWGetGitShellCommand(bashcommand)) {
        DirSaver ds;
        ::wxSetWorkingDirectory(m_selectedFolder);
        ::WrapInShell(bashcommand);
        ::wxExecute(bashcommand);
    } else {
        ::wxMessageBox(_("Don't know how to start MSYSGit..."), "Git", wxICON_WARNING | wxOK | wxCENTER);
    }
}

void GitPlugin::OnActiveProjectChanged(clProjectSettingsEvent& event)
{
    event.Skip();
    // Clear any stale repo data, otherwise it looks as if there's a valid git
    // repo when it actually belongs to a different project
    DoCleanup();
    m_console->UpdateTreeView("");

    wxFileName projectFile(event.GetFileName());
    DoSetRepoPath(projectFile.GetPath(), false);
}

void GitPlugin::StoreWorkspaceRepoDetails()
{
    // store the GIT entry data
    if(IsWorkspaceOpened()) {

        clConfig conf("git.conf");
        GitEntry data;
        conf.ReadItem(&data);
        data.SetEntry(GetWorkspaceName(), m_repositoryDirectory);
        conf.WriteItem(&data);
    }
}

void GitPlugin::WorkspaceClosed()
{
    // Clearn any saved data from the current workspace
    // git commands etc
    DoCleanup();
    m_workspaceFilename.Clear();
}

void GitPlugin::FetchNextCommits(int skip, const wxString& args)
{
    wxString skipCommits;
    skipCommits << " --skip=" << skip;
    gitAction ga(gitCommitList, args + skipCommits);
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();
}

void GitPlugin::OnFileGitBlame(wxCommandEvent& event)
{
    // Sanity
    if(m_filesSelected.IsEmpty() || m_repositoryDirectory.empty()) return;

    // We need to be symlink-aware here on Linux, so use CLRealPath
    wxString realfilepath = CLRealPath(m_filesSelected.Item(0));
    wxFileName fn(realfilepath);
    fn.MakeRelativeTo(CLRealPath(m_repositoryDirectory));

    DoGitBlame(fn.GetFullPath());
}

void GitPlugin::DisplayMessage(const wxString& message) const
{
    if (!message.empty()) {
        GIT_MESSAGE(message);
    }
}
