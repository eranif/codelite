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

#include "git.h"

#include "DiffSideBySidePanel.h"
#include "GitApplyPatchDlg.h"
#include "GitConsole.h"
#include "GitLocator.h"
#include "GitUserEmailDialog.h"
#include "bitmap_loader.h"
#include "clCommandProcessor.h"
#include "clDiffFrame.h"
#include "clEditorBar.h"
#include "clSFTPManager.hpp"
#include "clStatusBar.h"
#include "clTempFile.hpp"
#include "clWorkspaceManager.h"
#include "dirsaver.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "gitBlameDlg.h"
#include "gitCloneDlg.h"
#include "gitCommitDlg.h"
#include "gitCommitListDlg.h"
#include "gitDiffDlg.h"
#include "gitFileDiffDlg.h"
#include "gitSettingsDlg.h"
#include "gitentry.h"
#include "globals.h"
#include "overlaytool.h"
#include "project.h"
#include "workspace.h"

#include <stack>
#include <unordered_set>
#include <wx/artprov.h>
#include <wx/ffile.h>
#include <wx/file.h>
#include <wx/msgdlg.h>
#include <wx/sstream.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>

#ifdef __WXGTK__
#include <sys/wait.h>
#endif

static GitPlugin* thePlugin = NULL;
namespace
{
wxString GetDirFromPath(const wxString& path)
{
    wxString p = path;
    p.Replace("\\", "/");
    return p.BeforeLast('/');
}
} // namespace

#define GIT_MESSAGE_IF(cond, ...)                          \
    if(cond) {                                             \
        m_console->AddText(wxString::Format(__VA_ARGS__)); \
    }
#define GIT_MESSAGE(...) GIT_MESSAGE_IF(true, __VA_ARGS__)
#define GIT_MESSAGE1(...) GIT_MESSAGE_IF(m_configFlags& GitEntry::Git_Verbose_Log, __VA_ARGS__)

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new GitPlugin(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("René Kraus"));
    info.SetName(wxT("Git"));
    info.SetDescription(_("Simple GIT plugin"));
    info.SetVersion(wxT("v1.1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

// Helper that returns the last-used selection for those git commands with several alternative options
wxString GetAnyDefaultCommand(const wxString& gitCommand)
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    GitCommandsEntries& ce = data.GetGitCommandsEntries(gitCommand);
    return ce.GetDefaultCommand();
}

BEGIN_EVENT_TABLE(GitPlugin, wxEvtHandler)
EVT_TIMER(wxID_ANY, GitPlugin::OnProgressTimer)
END_EVENT_TABLE()

GitPlugin::GitPlugin(IManager* manager)
    : IPlugin(manager)
    , m_colourTrackedFile(wxT("DARK GREEN"))
    , m_colourDiffFile(wxT("MAROON"))
    , m_pathGITExecutable(wxT("git"))
    , m_pathGITKExecutable(wxT("gitk"))
    , m_bActionRequiresTreUpdate(false)
    , m_process(NULL)
    , m_eventHandler(NULL)
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

    EventNotifier::Get()->Bind(wxEVT_CC_UPDATE_NAVBAR, &GitPlugin::OnUpdateNavBar, this);

    EventNotifier::Get()->Bind(wxEVT_FILE_CREATED, &GitPlugin::OnFileCreated, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &GitPlugin::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &GitPlugin::OnWorkspaceClosed, this);
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
    EventNotifier::Get()->Bind(wxEVT_CODELITE_MAINFRAME_GOT_FOCUS, &GitPlugin::OnAppActivated, this);
    EventNotifier::Get()->Bind(wxEVT_FILES_MODIFIED_REPLACE_IN_FILES, &GitPlugin::OnReplaceInFiles, this);
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &GitPlugin::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CLOSING, &GitPlugin::OnEditorClosed, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_MODIFIED_EXTERNALLY, &GitPlugin::OnFileModifiedExternally, this);

    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderPullRebase, this, XRCID("git_pull_rebase_folder"));
    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderCommit, this, XRCID("git_commit_folder"));
    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderPush, this, XRCID("git_push_folder"));
    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderStash, this, XRCID("git_stash_folder"));
    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderStashPop, this, XRCID("git_stash_pop_folder"));
    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderGitBash, this, XRCID("git_bash_folder"));

    // Connect the file context menu event handlers
    m_eventHandler->Connect(XRCID("git_add_file"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnFileAddSelected),
                            NULL, this);
    m_eventHandler->Connect(XRCID("git_reset_file"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnFileResetSelected),
                            NULL, this);
    m_eventHandler->Connect(XRCID("git_diff_file"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnFileDiffSelected),
                            NULL, this);
    m_eventHandler->Bind(wxEVT_MENU, &GitPlugin::OnFileGitBlame, this, XRCID("git_blame_file"));

    // Respond to our own events
    EventNotifier::Get()->Bind(wxEVT_SOURCE_CONTROL_PUSHED, &GitPlugin::OnGitActionDone, this);
    EventNotifier::Get()->Bind(wxEVT_SOURCE_CONTROL_COMMIT_LOCALLY, &GitPlugin::OnGitActionDone, this);
    EventNotifier::Get()->Bind(wxEVT_SOURCE_CONTROL_PULLED, &GitPlugin::OnGitActionDone, this);
    EventNotifier::Get()->Bind(wxEVT_SOURCE_CONTROL_RESET_FILES, &GitPlugin::OnGitActionDone, this);

    // Add the console
    auto images = m_mgr->GetOutputPaneNotebook()->GetBitmaps();
    m_console = new GitConsole(m_mgr->GetOutputPaneNotebook(), this);
    m_mgr->GetOutputPaneNotebook()->AddPage(m_console, _("Git"), false, images->Add("git"));
    m_tabToggler.reset(new clTabTogglerHelper(_("Git"), m_console, "", NULL));
    m_tabToggler->SetOutputTabBmp(images->Add("git"));

    m_progressTimer.SetOwner(this);

    m_remoteProcess.Bind(wxEVT_CODELITE_REMOTE_FINDPATH, &GitPlugin::OnFindPath, this);
    m_remoteProcess.Bind(wxEVT_CODELITE_REMOTE_FINDPATH_DONE, &GitPlugin::OnFindPath, this);

    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    m_configFlags = data.GetFlags();
}

GitPlugin::~GitPlugin() { wxDELETE(m_gitBlameDlg); }

void GitPlugin::CreateToolBar(clToolBar* toolbar) { wxUnusedVar(toolbar); }

void GitPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    m_pluginMenu = new wxMenu();
    wxMenuItem* item(NULL);

    BitmapLoader* bmps = m_mgr->GetStdIcons();
    item = new wxMenuItem(m_pluginMenu, XRCID("git_pull"), _("Pull remote changes"));
    item->SetBitmap(bmps->LoadBitmap("pull"));
    m_pluginMenu->Append(item);
    m_pluginMenu->AppendSeparator();

    item = new wxMenuItem(m_pluginMenu, XRCID("git_commit"), _("Commit changes"));
    item->SetBitmap(bmps->LoadBitmap("git-commit"));
    m_pluginMenu->Append(item);
    item = new wxMenuItem(m_pluginMenu, XRCID("git_push"), _("Push changes"));
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
    item = new wxMenuItem(m_pluginMenu, XRCID("git_clone"), _("Clone a git repository"), _("Clone a git repository"),
                          wxITEM_NORMAL);
    item->SetBitmap(bmps->LoadBitmap("copy"));
    m_pluginMenu->Append(item);

    item = new wxMenuItem(pluginsMenu, wxID_ANY, wxT("Git"));
    item->SetSubMenu(m_pluginMenu);
    item->SetBitmap(bmps->LoadBitmap("git"));
    pluginsMenu->Append(item);

    m_eventHandler->Bind(wxEVT_MENU, &GitPlugin::OnOpenMSYSGit, this, XRCID("git_msysgit"));
    m_eventHandler->Connect(XRCID("git_settings"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnSettings), NULL,
                            this);
    m_eventHandler->Connect(XRCID("git_switch_branch"), wxEVT_MENU,
                            wxCommandEventHandler(GitPlugin::OnSwitchLocalBranch), NULL, this);
    m_eventHandler->Connect(XRCID("git_switch_to_remote_branch"), wxEVT_MENU,
                            wxCommandEventHandler(GitPlugin::OnSwitchRemoteBranch), NULL, this);
    m_eventHandler->Connect(XRCID("git_create_branch"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnCreateBranch),
                            NULL, this);
    m_eventHandler->Connect(XRCID("git_pull"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnPull), NULL, this);
    m_eventHandler->Connect(XRCID("git_clone"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnClone), NULL, this);
    m_eventHandler->Connect(XRCID("git_commit"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnCommit), NULL, this);
    m_eventHandler->Connect(XRCID("git_browse_commit_list"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnCommitList),
                            NULL, this);
    m_eventHandler->Connect(XRCID("git_commit_diff"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnShowDiffs), NULL,
                            this);
    m_eventHandler->Connect(XRCID("git_blame"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnGitBlame), NULL, this);
    m_eventHandler->Connect(XRCID("git_apply_patch"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnApplyPatch), NULL,
                            this);
    m_eventHandler->Connect(XRCID("git_push"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnPush), NULL, this);
    m_eventHandler->Connect(XRCID("git_rebase"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnRebase), NULL, this);
    m_eventHandler->Connect(XRCID("git_reset_repository"), wxEVT_MENU,
                            wxCommandEventHandler(GitPlugin::OnResetRepository), NULL, this);
    m_eventHandler->Connect(XRCID("git_start_gitk"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnStartGitk), NULL,
                            this);
    m_eventHandler->Connect(XRCID("git_list_modified"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnListModified),
                            NULL, this);
    m_eventHandler->Connect(XRCID("git_refresh"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnRefresh), NULL, this);
    m_eventHandler->Connect(XRCID("git_garbage_collection"), wxEVT_MENU,
                            wxCommandEventHandler(GitPlugin::OnGarbageColletion), NULL, this);
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

void GitPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void GitPlugin::UnPlug()
{
    ClearCodeLiteRemoteInfo();
    // before this plugin is un-plugged we must remove the tab we added
    for(size_t i = 0; i < m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
        if(m_console == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
            m_mgr->GetOutputPaneNotebook()->RemovePage(i);
            m_console->Destroy();
            break;
        }
    }

    EventNotifier::Get()->Unbind(wxEVT_FILE_CREATED, &GitPlugin::OnFileCreated, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &GitPlugin::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CLOSING, &GitPlugin::OnEditorClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_MODIFIED_EXTERNALLY, &GitPlugin::OnFileModifiedExternally, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_UPDATE_NAVBAR, &GitPlugin::OnUpdateNavBar, this);

    EventNotifier::Get()->Unbind(wxEVT_SOURCE_CONTROL_PUSHED, &GitPlugin::OnGitActionDone, this);
    EventNotifier::Get()->Unbind(wxEVT_SOURCE_CONTROL_COMMIT_LOCALLY, &GitPlugin::OnGitActionDone, this);
    EventNotifier::Get()->Unbind(wxEVT_SOURCE_CONTROL_PULLED, &GitPlugin::OnGitActionDone, this);
    EventNotifier::Get()->Unbind(wxEVT_SOURCE_CONTROL_RESET_FILES, &GitPlugin::OnGitActionDone, this);

    /*MENU*/
    m_eventHandler->Unbind(wxEVT_MENU, &GitPlugin::OnOpenMSYSGit, this, XRCID("git_msysgit"));
    m_eventHandler->Disconnect(XRCID("git_settings"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnSettings), NULL,
                               this);
    m_eventHandler->Disconnect(XRCID("git_switch_branch"), wxEVT_MENU,
                               wxCommandEventHandler(GitPlugin::OnSwitchLocalBranch), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_switch_to_remote_branch"), wxEVT_MENU,
                               wxCommandEventHandler(GitPlugin::OnSwitchRemoteBranch), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_create_branch"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnCreateBranch),
                               NULL, this);
    m_eventHandler->Disconnect(XRCID("git_pull"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnPull), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_commit"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnCommit), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_browse_commit_list"), wxEVT_MENU,
                               wxCommandEventHandler(GitPlugin::OnCommitList), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_commit_diff"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnShowDiffs),
                               NULL, this);
    m_eventHandler->Disconnect(XRCID("git_blame"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnGitBlame), NULL,
                               this);
    m_eventHandler->Disconnect(XRCID("git_apply_patch"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnApplyPatch),
                               NULL, this);
    m_eventHandler->Disconnect(XRCID("git_push"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnPush), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_reset_repository"), wxEVT_MENU,
                               wxCommandEventHandler(GitPlugin::OnResetRepository), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_start_gitk"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnStartGitk), NULL,
                               this);
    m_eventHandler->Disconnect(XRCID("git_list_modified"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnListModified),
                               NULL, this);
    m_eventHandler->Disconnect(XRCID("git_refresh"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnRefresh), NULL,
                               this);
    m_eventHandler->Disconnect(XRCID("git_garbage_collection"), wxEVT_MENU,
                               wxCommandEventHandler(GitPlugin::OnGarbageColletion), NULL, this);
    m_eventHandler->Unbind(wxEVT_MENU, &GitPlugin::OnFileGitBlame, this, XRCID("git_blame_file"));

    /*SYSTEM*/
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED, clCommandEventHandler(GitPlugin::OnFileSaved), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &GitPlugin::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_PROJ_FILE_ADDED, &GitPlugin::OnFilesAddedToProject, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CONFIG_CHANGED,
                                     wxCommandEventHandler(GitPlugin::OnWorkspaceConfigurationChanged), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_PROJECT_CHANGED, &GitPlugin::OnActiveProjectChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CODELITE_MAINFRAME_GOT_FOCUS, &GitPlugin::OnAppActivated, this);
    EventNotifier::Get()->Unbind(wxEVT_FILES_MODIFIED_REPLACE_IN_FILES, &GitPlugin::OnReplaceInFiles, this);

    /*Context Menu*/
    m_eventHandler->Disconnect(XRCID("git_add_file"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnFileAddSelected),
                               NULL, this);
    // m_eventHandler->Disconnect( ID_DELETE_FILE, wxEVT_MENU, wxCommandEventHandler(
    // GitPlugin::OnFileDeleteSelected), NULL, this );
    m_eventHandler->Disconnect(XRCID("git_reset_file"), wxEVT_MENU,
                               wxCommandEventHandler(GitPlugin::OnFileResetSelected), NULL, this);
    m_eventHandler->Disconnect(XRCID("git_diff_file"), wxEVT_MENU, wxCommandEventHandler(GitPlugin::OnFileDiffSelected),
                               NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FILE, &GitPlugin::OnFileMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FOLDER, &GitPlugin::OnFolderMenu, this);
    wxTheApp->Unbind(wxEVT_MENU, &GitPlugin::OnFolderPullRebase, this, XRCID("git_pull_rebase_folder"));
    wxTheApp->Unbind(wxEVT_MENU, &GitPlugin::OnFolderCommit, this, XRCID("git_commit_folder"));
    wxTheApp->Unbind(wxEVT_MENU, &GitPlugin::OnFolderPush, this, XRCID("git_push_folder"));
    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderStash, this, XRCID("git_stash_folder"));
    wxTheApp->Bind(wxEVT_MENU, &GitPlugin::OnFolderStashPop, this, XRCID("git_stash_pop_folder"));
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &GitPlugin::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &GitPlugin::OnProcessTerminated, this);

    m_remoteProcess.Unbind(wxEVT_CODELITE_REMOTE_FINDPATH, &GitPlugin::OnFindPath, this);
    m_remoteProcess.Unbind(wxEVT_CODELITE_REMOTE_FINDPATH_DONE, &GitPlugin::OnFindPath, this);

    m_tabToggler.reset(NULL);
}

void GitPlugin::OnSetGitRepoPath(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoSetRepoPath();
}

void GitPlugin::DoSetRepoPath(const wxString& repo_path)
{
    if(repo_path.empty()) {
        if(!m_userEnteredRepositoryDirectory.empty()) {
            m_repositoryDirectory = m_userEnteredRepositoryDirectory;
        } else {
            m_repositoryDirectory = FindRepositoryRoot(GetDirFromPath(m_workspace_file));
        }
    } else {
        m_repositoryDirectory = repo_path;
    }

    if(IsGitEnabled()) {
        wxBitmap bmp = clGetManager()->GetStdIcons()->LoadBitmap("git");
        clGetManager()->GetStatusBar()->SetSourceControlBitmap(bmp, "Git", _("Using git\nClick to open the git view"));
    }
    AddDefaultActions();
    ProcessGitActionQueue();
}

void GitPlugin::OnSettings(wxCommandEvent& e)
{
    wxString projectNameHash;
    if(!m_isRemoteWorkspace) {
        wxString workspaceName = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetName();
        wxString projectName = m_mgr->GetWorkspace()->GetActiveProjectName();
        if(!workspaceName.empty() && !projectName.empty()) {
            projectNameHash << workspaceName << '-' << projectName;
        }
    }

    GitSettingsDlg dlg(EventNotifier::Get()->TopFrame(), m_repositoryDirectory, m_userEnteredRepositoryDirectory,
                       projectNameHash);
    int retValue = dlg.ShowModal();

    if(retValue == wxID_OK || retValue == wxID_REFRESH) {
        if(retValue == wxID_REFRESH) {
            // An unusual git repo path was entered (or a previously-entered one removed)
            m_userEnteredRepositoryDirectory = dlg.GetNewGitRepoPath();
            m_repositoryDirectory = m_userEnteredRepositoryDirectory;
            DoSetRepoPath(m_repositoryDirectory);
            CallAfter(&GitPlugin::DoRefreshView, false);
        }
        // update the paths
        clConfig conf("git.conf");
        GitEntry data;
        conf.ReadItem(&data);
        m_configFlags = data.GetFlags();

        m_pathGITExecutable = data.GetGITExecutablePath();
        m_pathGITKExecutable = data.GetGITKExecutablePath();

        GIT_MESSAGE("git executable is now set to: %s", m_pathGITExecutable.c_str());
        GIT_MESSAGE("gitk executable is now set to: %s", m_pathGITKExecutable.c_str());

        // clear any status bar information
        clGetManager()->GetNavigationBar()->ClearLabel();
        AddDefaultActions();
        ProcessGitActionQueue();

        DoLoadBlameInfo(true);
    }
}

void GitPlugin::OnFileAddSelected(wxCommandEvent& e)
{
    wxUnusedVar(e);

    wxArrayString files;
    files.swap(m_filesSelected);
    if(files.IsEmpty())
        return;

    // Make the git console visible
    m_mgr->ShowOutputPane("Git");

    wxString workingDir = wxFileName(files.Item(0)).GetPath(wxPATH_UNIX);
    if(!GetRepositoryPath().empty()) {
        workingDir = GetRepositoryPath();
    }

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
    DoExecuteCommandSync(cmd, &commandOutput, workingDir);
    GetConsole()->AddText(commandOutput);
    RefreshFileListView();
}

void GitPlugin::OnFileDeleteSelected(wxCommandEvent& e) { RefreshFileListView(); }

void GitPlugin::OnFileDiffSelected(wxCommandEvent& e)
{
    wxUnusedVar(e);

    wxArrayString files;
    files.swap(m_filesSelected);
    if(files.IsEmpty())
        return;

    // Make the git console visible
    m_mgr->ShowOutputPane("Git");

    for(const wxString& filename : files) {
        // Pepare the command:
        // git add --no-pager
        wxString cmd = "show HEAD:";

        wxString filenameEscaped = filename;
        ::WrapWithQuotes(filenameEscaped);
        cmd << filenameEscaped;

        // We need to run this command per file
        wxString commandOutput;
        DoExecuteCommandSync(cmd, &commandOutput);
        if(!commandOutput.IsEmpty()) {
            DoShowDiffViewer(commandOutput, filename);
        }
    }
}

void GitPlugin::OnFileResetSelected(wxCommandEvent& e)
{
    wxUnusedVar(e);

    wxArrayString files;
    files.swap(m_filesSelected);
    if(files.IsEmpty())
        return;

    // Make the git console visible
    m_mgr->ShowOutputPane("Git");

    wxString workingDir = wxFileName(files.Item(0)).GetPath(wxPATH_UNIX);
    if(!GetRepositoryPath().empty()) {
        workingDir = GetRepositoryPath();
    }

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
    DoExecuteCommandSync(cmd, &commandOutput, workingDir);
    GetConsole()->AddText(commandOutput);

    // Reload externally modified files
    EventNotifier::Get()->PostReloadExternallyModifiedEvent();
    RefreshFileListView();
}

void GitPlugin::OnSwitchLocalBranch(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(!m_modifiedFiles.empty()) {
        wxMessageBox(_("Modified files found! Commit them first before switching branches..."), wxT("CodeLite"),
                     wxICON_ERROR | wxOK, EventNotifier::Get()->TopFrame());
        return;
    }

    if(m_localBranchList.GetCount() == 0) {
        wxMessageBox(_("No other local branches found."), wxT("CodeLite"), wxICON_INFORMATION | wxOK,
                     EventNotifier::Get()->TopFrame());
        return;
    }

    wxString message = _("Select branch (current is ");
    message << m_currentBranch << wxT(")");

    wxString selection =
        wxGetSingleChoice(message, _("Switch branch"), m_localBranchList, EventNotifier::Get()->TopFrame());

    if(selection.IsEmpty())
        return;

    gitAction ganew(gitBranchSwitch, selection);
    m_gitActionQueue.push_back(ganew);
    AddDefaultActions();
    m_mgr->SaveAll();
    ProcessGitActionQueue();
}

void GitPlugin::OnSwitchRemoteBranch(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(!m_modifiedFiles.empty()) {
        wxMessageBox(_("Modified files found! Commit them first before switching branches..."), wxT("CodeLite"),
                     wxICON_ERROR | wxOK, EventNotifier::Get()->TopFrame());
        return;
    }
    if(m_remoteBranchList.GetCount() == 0) {
        wxMessageBox(_("No remote branches found."), wxT("CodeLite"), wxICON_INFORMATION | wxOK,
                     EventNotifier::Get()->TopFrame());
        return;
    }
    wxString message = _("Select remote branch (current is ");
    message << m_currentBranch << wxT(")");

    wxString selection =
        wxGetSingleChoice(message, _("Switch to remote branch"), m_remoteBranchList, EventNotifier::Get()->TopFrame());

    if(selection.IsEmpty())
        return;

    wxString localBranch = selection;
    localBranch.Replace(wxT("origin/"), wxT(""));
    localBranch = wxGetTextFromUser(_("Specify the name for the local branch"), _("Branch name"), localBranch,
                                    EventNotifier::Get()->TopFrame());
    if(localBranch.IsEmpty())
        return;

    gitAction ganew(gitBranchSwitchRemote, localBranch + wxT(" ") + selection);
    m_gitActionQueue.push_back(ganew);

    AddDefaultActions();
    m_mgr->SaveAll();
    ProcessGitActionQueue();
}

void GitPlugin::OnCreateBranch(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxString newBranch = wxGetTextFromUser(_("Specify the name of the new branch"), _("Branch name"), wxT(""),
                                           EventNotifier::Get()->TopFrame());
    if(newBranch.IsEmpty())
        return;

    gitAction ga(gitBranchCreate, newBranch);
    m_gitActionQueue.push_back(ga);

    if(wxMessageBox(_("Switch to new branch once it is created?"), _("Switch to new branch"), wxYES_NO,
                    EventNotifier::Get()->TopFrame()) == wxYES) {
        ga.action = gitBranchSwitch;
        ga.arguments = newBranch;
        m_gitActionQueue.push_back(ga);
        AddDefaultActions();
        m_mgr->SaveAll();
    }

    ProcessGitActionQueue();
}

void GitPlugin::OnCommit(wxCommandEvent& e)
{
    wxUnusedVar(e);
    gitAction ga(gitDiffRepoCommit, wxT(""));
    m_gitActionQueue.push_back(ga);
    m_mgr->ShowOutputPane("Git");
    ProcessGitActionQueue();
}

void GitPlugin::OnCommitList(wxCommandEvent& e)
{
    wxUnusedVar(e);
    gitAction ga(gitCommitList, wxT(""));
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();
}

void GitPlugin::OnShowDiffs(wxCommandEvent& e)
{
    wxUnusedVar(e);
    GitDiffDlg dlg(EventNotifier::Get()->TopFrame(), m_repositoryDirectory, this);
    dlg.ShowModal();
}

void GitPlugin::OnApplyPatch(wxCommandEvent& e)
{
    wxUnusedVar(e);
    GitApplyPatchDlg dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        ApplyPatch(dlg.GetPatchFile(), dlg.GetExtraFlags());
    }
}

void GitPlugin::OnPush(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(m_remotes.GetCount() == 0) {
        wxMessageBox(_("No remotes found, can't push!"), wxT("CodeLite"), wxICON_ERROR | wxOK,
                     EventNotifier::Get()->TopFrame());
        return;
    }

    wxStandardID res = ::PromptForYesNoDialogWithCheckbox(_("Push all local commits?"), "GitPromptBeforePush");
    if(res == wxID_YES) {
        // wxString remote = m_remotes[0];
        // if(m_remotes.GetCount() > 1) {
        //     remote = wxGetSingleChoice(_("Select remote to push to."), _("Select remote"), m_remotes,
        //     EventNotifier::Get()->TopFrame()); if(remote.IsEmpty()) {
        //         return;
        //     }
        // }
        gitAction ga(gitPush, /*remote + wxT(" ") + m_currentBranch*/ wxEmptyString);
        m_gitActionQueue.push_back(ga);
        m_mgr->ShowOutputPane("Git");
        ProcessGitActionQueue();
    }
}

void GitPlugin::OnPull(wxCommandEvent& e)
{
    wxString argumentString = e.GetString(); // This might be user-specified e.g. pull --rebase
    if(argumentString.empty()) {
        argumentString = GetAnyDefaultCommand("git_pull");
    }
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

void GitPlugin::OnResetRepository(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(wxMessageBox(_("Are you sure that you want to discard all local changes?"), _("Reset repository"),
                    wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxICON_WARNING,
                    EventNotifier::Get()->TopFrame()) == wxYES) {
        gitAction ga(gitResetRepo, wxT(""));
        m_gitActionQueue.push_back(ga);
        AddDefaultActions();
        ProcessGitActionQueue();
    }
}

void GitPlugin::OnStartGitk(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DirSaver ds;

    ::wxSetWorkingDirectory(m_repositoryDirectory);
    ::wxExecute(m_pathGITKExecutable);
}

void GitPlugin::OnListModified(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxArrayString choices;
    std::map<wxString, wxTreeItemId>::const_iterator it;

    // First get a map of the filepaths/treeitemids of modified files
    std::map<wxString, wxTreeItemId> modifiedIDs;
    CreateFilesTreeIDsMap(modifiedIDs, true);

    for(it = modifiedIDs.begin(); it != modifiedIDs.end(); ++it) {
        if(it->second.IsOk())
            choices.Add(it->first);
    }

    if(choices.GetCount() == 0)
        return;

    wxString choice =
        wxGetSingleChoice(_("Jump to modified file"), _("Modified files"), choices, EventNotifier::Get()->TopFrame());
    if(!choice.IsEmpty()) {
        wxTreeItemId id = modifiedIDs[choice];
        if(id.IsOk()) {
            m_mgr->GetWorkspaceTree()->EnsureVisible(id);
            m_mgr->GetWorkspaceTree()->SelectItem(id);
        }
    }
}

void GitPlugin::OnGitBlame(wxCommandEvent& e)
{
    wxUnusedVar(e);

    wxString filepath = GetEditorRelativeFilepath();
    if(!filepath.empty()) {
        DoGitBlame(filepath);
    }
}

wxString GitPlugin::GetEditorRelativeFilepath() const // Called by OnGitBlame or the git blame dialog
{
    IEditor* current = m_mgr->GetActiveEditor();
    if(!current || m_repositoryDirectory.empty()) {
        return "";
    }

    // We need to be symlink-aware here on Linux, so use CLRealPath
    wxString realfilepath = CLRealPath(current->GetFileName().GetFullPath());
    wxFileName fn(realfilepath);
    fn.MakeRelativeTo(CLRealPath(m_repositoryDirectory));

    return fn.GetFullPath();
}

void GitPlugin::DoGitBlame(const wxString& args) // Called by OnGitBlame or the git blame dialog
{
    gitAction ga(gitBlame, args);
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();
}

void GitPlugin::OnGitBlameRevList(const wxString& arg, const wxString& filepath,
                                  const wxString& commit) // Called by the git blame dialog
{
    wxString cmt(commit);
    if(cmt.empty()) {
        cmt = "HEAD";
    }
    wxString args = arg + ' ' + cmt + " -- " + filepath;

    gitAction ga(gitRevlist, args);
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();
}

void GitPlugin::OnRefresh(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoRefreshView(true);
}

void GitPlugin::OnGarbageColletion(wxCommandEvent& e)
{
    wxUnusedVar(e);
    gitAction ga(gitGarbageCollection, wxT(""));
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();
}

void GitPlugin::OnFileModifiedExternally(clFileSystemEvent& e)
{
    e.Skip();
    DoAnyFileModified();
}

void GitPlugin::DoAnyFileModified()
{
    DoLoadBlameInfo(true);
    gitAction ga(gitListModified, wxT(""));
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();
    RefreshFileListView();
}
void GitPlugin::OnFileSaved(clCommandEvent& e)
{
    e.Skip();
    DoAnyFileModified();
}

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

void GitPlugin::OnFilesRemovedFromProject(clCommandEvent& e)
{
    e.Skip();
    RefreshFileListView(); // in git world, deleting a file is enough
}

void GitPlugin::ClearCodeLiteRemoteInfo()
{
    m_isRemoteWorkspace = false;
    m_remoteWorkspaceAccount.clear();
    m_remoteProcess.Stop();
    m_codeliteRemoteScriptPath.clear();
}

void GitPlugin::StartCodeLiteRemote()
{
    if(m_isRemoteWorkspace && !m_remoteWorkspaceAccount.empty()) {
        wxString workspace_dir = GetDirFromPath(m_workspace_file);
        wxString codelite_remote_script = workspace_dir + "/.codelite/codelite-remote";
        m_remoteProcess.StartInteractive(m_remoteWorkspaceAccount, codelite_remote_script, "git");

        // find the real git root folder
        m_remoteProcess.FindPath(workspace_dir + "/.git");
    }
}

void GitPlugin::OnWorkspaceLoaded(clWorkspaceEvent& e)
{
    e.Skip();
    ClearCodeLiteRemoteInfo();
    DoCleanup();

    m_workspace_file = e.GetString();
    m_isRemoteWorkspace = e.IsRemote();
    m_remoteWorkspaceAccount = e.GetRemoteAccount();
    StartCodeLiteRemote();
    InitDefaults();
    RefreshFileListView();

    // Try to set the repo, usually to the workspace path
    DoSetRepoPath();
    CallAfter(&GitPlugin::DoRefreshView, false);
}

void GitPlugin::ProcessGitActionQueue()
{
    if(m_gitActionQueue.size() == 0)
        return;

    // Sanity:
    // if there is no repo and the command is not 'clone'
    // return
    gitAction ga = m_gitActionQueue.front();
    if(m_repositoryDirectory.IsEmpty() && ga.action != gitClone) {
        m_gitActionQueue.pop_front();
        return;
    }

    if(m_process) {
        return;
    }

    wxString command_args;
    size_t createFlags = 0;
    bool log_message = false;
    switch(ga.action) {
    case gitBlameSummary: {
        wxString filepath = ga.arguments;
        ::WrapWithQuotes(filepath);
        command_args << "--no-pager blame --date=short " << filepath;
    } break;
    case gitStash:
        command_args << " stash";
        log_message = false;
        break;

    case gitStashPop:
        command_args << " stash pop";
        log_message = false;
        break;

    case gitRevertCommit:
        command_args << "revert --no-commit " << ga.arguments;
        log_message = true;
        break;

    case gitApplyPatch:
        command_args << "apply --whitespace=nowarn --ignore-whitespace " << ga.arguments;
        log_message = true;
        break;

    case gitRmFiles:
        command_args << "--no-pager rm --force " << ga.arguments;
        log_message = true;
        break;

    case gitClone:
        command_args << wxT("--no-pager clone ") << ga.arguments;
        log_message = true;
        break;

    case gitStatus:
        command_args << "--no-pager status -s";
        break;

    case gitListAll:
        command_args << wxT("--no-pager ls-files");
        break;

    case gitListModified:
        command_args << wxT("--no-pager ls-files -m");
        break;

    case gitUpdateRemotes:
        command_args << wxT("--no-pager remote update");
        break;

    case gitListRemotes:
        command_args << wxT("--no-pager remote");
        break;

    case gitAddFile:
        command_args << wxT("--no-pager add ") << ga.arguments;
        log_message = true;
        break;

    case gitDeleteFile:
        command_args << wxT("--no-pager update-index --remove --force-remove ") << ga.arguments;
        log_message = true;
        break;

    case gitDiffFile:
        command_args << wxT("--no-pager show HEAD:") << ga.arguments;
        break;

    case gitDiffRepoCommit:
        command_args << wxT("--no-pager diff --no-color HEAD");
        ShowProgress(wxT("Obtaining diffs for modified files..."));
        break;

    case gitDiffRepoShow:
        command_args << wxT("--no-pager diff --no-color HEAD");
        ShowProgress(wxT("Obtaining diffs for modified files..."));
        break;

    case gitResetFile:
        command_args << wxT("--no-pager checkout ") << ga.arguments;
        log_message = true;
        break;

    case gitUndoAdd:
        command_args << wxT("--no-pager reset ") << ga.arguments;
        log_message = true;
        break;

    case gitPull:
        ShowProgress(wxT("Obtaining remote changes"), false);
        command_args << "--no-pager pull " << ga.arguments;
        command_args << " --log";
        log_message = true;
        createFlags |= IProcessRawOutput;
        break;

    case gitPush:
        command_args << wxT("--no-pager push ") << ga.arguments;
        ShowProgress(wxT("Pushing local changes..."), false);
        log_message = true;
        createFlags |= IProcessRawOutput;
        break;

    case gitCommit:
        command_args << wxT("--no-pager commit ") << ga.arguments;
        ShowProgress(wxT("Committing local changes..."));
        log_message = true;
        createFlags |= IProcessRawOutput;
        break;

    case gitResetRepo:
        command_args << wxT("--no-pager reset --hard");
        ShowProgress(wxT("Resetting local repository..."));
        log_message = true;
        break;

    case gitBranchCreate:
        command_args << wxT("--no-pager branch ") << ga.arguments;
        ShowProgress(wxT("Creating local branch..."));
        log_message = true;
        break;

    case gitBranchCurrent:
        command_args << wxT("--no-pager branch --no-color");
        break;

    case gitBranchList:
        command_args << wxT("--no-pager branch --no-color");
        break;

    case gitBranchListRemote:
        command_args << wxT("--no-pager branch -r --no-color");
        break;

    case gitBranchSwitch:
        ShowProgress(wxT("Switching to local branch ") + ga.arguments, false);
        command_args << wxT("--no-pager checkout ") << ga.arguments;
        log_message = true;
        break;

    case gitBranchSwitchRemote:
        ShowProgress(wxT("Switching to remote branch ") + ga.arguments, false);
        command_args << wxT("--no-pager checkout -b ") << ga.arguments;
        log_message = true;
        createFlags |= IProcessRawOutput;
        break;

    case gitCommitList:
        ShowProgress(wxT("Fetching commit list"));
        // hash @ author-name @ date @ subject
        command_args << wxT("--no-pager log --pretty=\"%h@%an@%ci@%s\" -n 100 ") << ga.arguments;
        break;

    case gitBlame:
        command_args << "--no-pager blame --line-porcelain " << ga.arguments;
        log_message = true;
        break;

    case gitRevlist:
        command_args << "--no-pager rev-list " << ga.arguments;
        break;

    case gitRebase:
        ShowProgress(wxT("Rebase with ") + ga.arguments + wxT(".."));
        command_args << wxT("--no-pager rebase ") << ga.arguments;
        log_message = true;
        createFlags |= IProcessRawOutput;
        break;
    case gitConfig:
        command_args << wxT("--no-pager config ") << ga.arguments;
        break;
    case gitGarbageCollection:
        ShowProgress(wxT("Cleaning git database. This may take some time..."), false);
        log_message = true;
        command_args << wxT("--no-pager gc");
        break;

    default:
        GIT_MESSAGE(wxT("Unknown git action"));
        return;
    }

    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

#ifdef __WXMSW__
    if(ga.action == gitClone || ga.action == gitPush || ga.action == gitPull) {
        createFlags |=
            data.GetFlags() & GitEntry::Git_Show_Terminal ? IProcessCreateConsole : IProcessCreateWithHiddenConsole;

    } else {
        createFlags |= IProcessCreateWithHiddenConsole;
    }
#else
    createFlags |= IProcessCreateWithHiddenConsole;

#endif

    // Set locale to english
    wxStringMap_t om;
    om.insert({ "LC_ALL", "C" });
    om.insert({ "GIT_MERGE_AUTOEDIT", "no" });

#ifdef __WXMSW__
    wxString homeDir;
    if(wxGetEnv("USERPROFILE", &homeDir)) {
        om.insert(std::make_pair("HOME", homeDir));
    }
#endif
    EnvSetter es(&om);
    wxString workingDirectory = ga.workingDirectory.IsEmpty() ? m_repositoryDirectory : ga.workingDirectory;

    m_process = AsyncRunGit(this, command_args, createFlags | IProcessWrapInShell, workingDirectory, log_message);
    if(!m_process) {
        GIT_MESSAGE(wxT("Failed to execute git command!"));
        DoRecoverFromGitCommandError();
    }
}

void GitPlugin::FinishGitListAction(const gitAction& ga)
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    if(!(data.GetFlags() & GitEntry::Git_Colour_Tree_View))
        return;

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
        ColourFileTree(m_mgr->GetWorkspaceTree(), gitFileSet, OverlayTool::Bmp_OK);
        m_trackedFiles.swap(gitFileSet);

    } else if(ga.action == gitListModified) {
        m_mgr->SetStatusMessage(_("Colouring modified git files..."), 0);
        // Reset modified files
        ColourFileTree(m_mgr->GetWorkspaceTree(), m_modifiedFiles, OverlayTool::Bmp_OK);
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
                DoSetTreeItemImage(m_mgr->GetWorkspaceTree(), id, OverlayTool::Bmp_Modified);

            } else {
                toColour.insert(*iter);
            }
        }

        if(!toColour.empty()) {
            ColourFileTree(m_mgr->GetWorkspaceTree(), toColour, OverlayTool::Bmp_Modified);
        }

        // Finally, cache the modified-files list: it's used in other functions
        m_modifiedFiles.swap(gitFileSet);
    }
    m_mgr->SetStatusMessage("", 0);
}

void GitPlugin::ListBranchAction(const gitAction& ga)
{
    wxArrayString gitList = wxStringTokenize(m_commandOutput, wxT("\n"));
    if(gitList.GetCount() == 0)
        return;

    wxArrayString branchList;
    for(unsigned i = 0; i < gitList.GetCount(); ++i) {
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

void GitPlugin::GetCurrentBranchAction(const gitAction& ga)
{
    wxArrayString gitList = wxStringTokenize(m_commandOutput, wxT("\n"));
    if(gitList.GetCount() == 0)
        return;

    m_currentBranch.Empty();
    for(unsigned i = 0; i < gitList.GetCount(); ++i) {
        if(gitList[i].StartsWith(wxT("*"))) {
            m_currentBranch = gitList[i].Mid(2);
            break;
        }
    }

    if(!m_currentBranch.IsEmpty()) {
        GIT_MESSAGE1(wxT("Current branch ") + m_currentBranch);
        m_mgr->GetDockingManager()
            ->GetPane(wxT("Workspace View"))
            .Caption(_("Workspace View [") + m_currentBranch + wxT("]"));
        m_mgr->GetDockingManager()->Update();
    }
}

void GitPlugin::UpdateFileTree()
{
    if(!m_mgr->GetWorkspace()->IsOpen()) {
        return;
    }

    if(wxMessageBox(_("Do you want to start importing new / updating changed files?"), _("Import files"), wxYES_NO,
                    EventNotifier::Get()->TopFrame()) == wxNO) {
        return;
    }

    wxString projectName = m_mgr->GetWorkspace()->GetActiveProjectName();
    wxString error = _("Error obtaining project");
    ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(projectName, error);

    proj->BeginTranscation();
    wxString path = m_repositoryDirectory;
    if(path.EndsWith(wxT("/")) || path.EndsWith(wxT("\\"))) {
        path.RemoveLast();
    }
    wxFileName rootPath(path);

    wxArrayString gitfiles = wxStringTokenize(m_commandOutput, wxT("\n"));
    wxArrayString files;

    // clProgressDlg *prgDlg = new clProgressDlg (EventNotifier::Get()->TopFrame(), _("Importing files ..."), wxT(""),
    // (int)gitfiles.GetCount()+2);
    wxProgressDialog* prgDlg = new wxProgressDialog(_("Importing files ..."), wxT(""), (int)gitfiles.GetCount() + 2,
                                                    EventNotifier::Get()->TopFrame());
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
        if(proj->IsFileExist(fn.GetFullPath())) {
            continue;
        }

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
            if(dummyFiles.GetCount() != 0) {
                createAndAdd = true;
            }
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

void GitPlugin::OnProcessTerminated(clProcessEvent& event)
{
    HideProgress();
    if(m_gitActionQueue.empty())
        return;

    gitAction ga = m_gitActionQueue.front();
    if(ga.action != gitDiffFile) {
        // Dont manipulate the output if its a diff...
        m_commandOutput.Replace(wxT("\r"), wxT(""));
    }
    if (ga.action == gitDiffRepoCommit && m_commandOutput.StartsWith(wxT("fatal"))) {
        m_commandOutput.Clear();
        DoExecuteCommandSync("diff --no-color --cached", &m_commandOutput);
    }
    if(m_commandOutput.StartsWith(wxT("fatal")) || m_commandOutput.StartsWith(wxT("error"))) {
        // Last action failed, clear queue
        clDEBUG1() << "[git]" << m_commandOutput << clEndl;
        static std::unordered_set<int> recoverableActions = { gitBlameSummary };
        DoRecoverFromGitCommandError(recoverableActions.count(ga.action) == 0);
        GetConsole()->ShowLog();
        return;
    }

    switch(ga.action) {
    case gitBlameSummary: {
        DoUpdateBlameInfo(m_commandOutput, ga.arguments);
    } break;
    case gitPush: {
        clSourceControlEvent evt(wxEVT_SOURCE_CONTROL_PUSHED);
        evt.SetSourceControlName("git");
        EventNotifier::Get()->QueueEvent(evt.Clone());
    } break;
    case gitListAll:
    case gitListModified:
    case gitResetRepo: {
        if(ga.action == gitListAll && m_bActionRequiresTreUpdate) {
            if(m_commandOutput.Lower().Contains(_("created"))) {
                UpdateFileTree();
            }
        }
        m_bActionRequiresTreUpdate = false;
        FinishGitListAction(ga);
        if(ga.action == gitResetRepo) {
            // Reload files if needed
            EventNotifier::Get()->PostReloadExternallyModifiedEvent(true);
            // We also want to post reset event here
            clSourceControlEvent evt(wxEVT_SOURCE_CONTROL_RESET_FILES);
            evt.SetSourceControlName("git");
            EventNotifier::Get()->QueueEvent(evt.Clone());
        }
    } break;
    case gitStatus: {
        m_console->UpdateTreeView(m_commandOutput);
        FinishGitListAction(ga);
    } break;
    case gitListRemotes: {
        wxArrayString gitList = wxStringTokenize(m_commandOutput, wxT("\n"));
        m_remotes = gitList;
    } break;
    case gitDiffFile: {

        // Show the diff in the diff-viewer
        DoShowDiffViewer(m_commandOutput, ga.arguments);
    } break;
    case gitDiffRepoCommit: {
        wxString commitArgs;
        DoShowCommitDialog(m_commandOutput, commitArgs);
        if(!commitArgs.IsEmpty()) {
            gitAction ga(gitCommit, commitArgs);
            m_gitActionQueue.push_back(ga);
            AddDefaultActions();
        }
    } break;
    case gitBlame: {
        if(!m_gitBlameDlg) {
            m_gitBlameDlg = new GitBlameDlg(EventNotifier::Get()->TopFrame(), this);
        }
        m_gitBlameDlg->SetBlame(m_commandOutput, ga.arguments);
        m_gitBlameDlg->Show();
        m_gitBlameDlg->SetFocus();
    } break;
    case gitRevlist: {
        if(m_gitBlameDlg) {
            m_gitBlameDlg->OnRevListOutput(m_commandOutput, ga.arguments);
        }
    } break;
    case gitDiffRepoShow: {
        // This is now dealt with by GitDiffDlg itself
        //        GitDiffDlg dlg(EventNotifier::Get()->TopFrame(), m_repositoryDirectory, this);
        //        dlg.SetDiff(m_commandOutput);
        //        dlg.ShowModal();
    } break;
    case gitResetFile:
    case gitApplyPatch: {
        EventNotifier::Get()->PostReloadExternallyModifiedEvent(true);

        gitAction newAction;
        newAction.action = gitListModified;
        m_gitActionQueue.push_back(newAction);

        if(ga.action == gitResetFile) {
            // We also want to post reset event here
            clSourceControlEvent evt(wxEVT_SOURCE_CONTROL_RESET_FILES);
            evt.SetSourceControlName("git");
            EventNotifier::Get()->QueueEvent(evt.Clone());
        }

    } break;
    case gitBranchCurrent:
        GetCurrentBranchAction(ga);
        break;
    case gitBranchList:
    case gitBranchListRemote: {
        ListBranchAction(ga);
    } break;
    case gitBranchSwitch:
    case gitBranchSwitchRemote:
    case gitPull: {
        if(ga.action == gitPull) {
            if(m_commandOutput.Contains(wxT("Already"))) {
                // do nothing
            } else {
                wxString log = m_commandOutput.Mid(m_commandOutput.Find(wxT("From")));
                if(m_commandOutput.Contains(wxT("Merge made by"))) {
                    if(wxMessageBox(_("Merged after pull. Rebase?"), _("Rebase"), wxYES_NO,
                                    EventNotifier::Get()->TopFrame()) == wxYES) {
                        wxString selection;
                        if(m_remotes.GetCount() > 1) {
                            selection = wxGetSingleChoice(_("Rebase with what branch?"), _("Rebase"),
                                                          m_remoteBranchList, EventNotifier::Get()->TopFrame());
                        } else {
                            selection = m_remotes[0] + wxT("/") + m_currentBranch;
                            if(wxMessageBox(_("Rebase with ") + selection + wxT("?"), _("Rebase"), wxYES_NO,
                                            EventNotifier::Get()->TopFrame()) == wxNO)
                                selection.Empty();
                        }

                        if(selection.IsEmpty())
                            return;

                        gitAction ga(gitRebase, selection);
                        m_gitActionQueue.push_back(ga);
                    }
                } else if(m_commandOutput.Contains(wxT("CONFLICT"))) {
                    // Do nothing, will be coloured in the console view
                    GetConsole()->ShowLog();
                }
                if(m_commandOutput.Contains(wxT("Updating")))
                    m_bActionRequiresTreUpdate = true;

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

        clSourceControlEvent evt(wxEVT_SOURCE_CONTROL_PULLED);
        evt.SetSourceControlName("git");
        EventNotifier::Get()->QueueEvent(evt.Clone());

        // Reload files if needed
        EventNotifier::Get()->PostReloadExternallyModifiedEvent(true);
    } break;
    case gitCommitList: {
        if(!m_commitListDlg) {
            m_commitListDlg = new GitCommitListDlg(EventNotifier::Get()->TopFrame(), m_repositoryDirectory, this);
        }
        m_commitListDlg->SetCommitList(m_commandOutput);
        m_commitListDlg->Show();
    } break;
    case gitCommit: {
        clSourceControlEvent evt(wxEVT_SOURCE_CONTROL_COMMIT_LOCALLY);
        evt.SetSourceControlName("git");
        EventNotifier::Get()->QueueEvent(evt.Clone());
    } break;
    case gitRevertCommit: {
        // We also want to post reset event here
        clSourceControlEvent evt(wxEVT_SOURCE_CONTROL_RESET_FILES);
        evt.SetSourceControlName("git");
        EventNotifier::Get()->QueueEvent(evt.Clone());
        AddDefaultActions();
    } break;
    default:
        break;
    }

    if(ga.action == gitResetRepo || ga.action == gitResetFile) {
        // Reload externally modified files
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

bool GitPlugin::HandleErrorsOnRemoteRepo(const wxString& output) const
{
    if(!m_isRemoteWorkspace) {
        return false;
    }

    if(output.Contains("username for") || output.Contains("username for") ||
       output.Contains("commit-msg hook failure") || output.Contains("pre-commit hook failure") ||
       output.Contains("*** please tell me who you are") || output.EndsWith("password:") ||
       output.Contains("password for") || output.Contains("authentication failed") ||
       (output.Contains("the authenticity of host") && output.Contains("can't be established")) ||
       output.Contains("key fingerprint")) {
        ::wxMessageBox(_("Remote git error (needs to be handled manually on the remote server)\n") + output, "CodeLite",
                       wxICON_WARNING | wxOK | wxCANCEL | wxOK_DEFAULT);
        return true;
    }
    return false;
}

void GitPlugin::OnProcessOutput(clProcessEvent& event)
{
    wxString output = event.GetOutput();
    auto process = event.GetProcess();
    gitAction ga;
    if(!m_gitActionQueue.empty()) {
        ga = m_gitActionQueue.front();
    }

    if(ga.action == gitPush || ga.action == gitPull) {
        m_console->AddText(output);
    }
    m_commandOutput.Append(output);

    // Handle password required
    wxString tmpOutput = output;
    tmpOutput.Trim().Trim(false);
    tmpOutput.MakeLower();

    static std::unordered_set<int> exclude_commands = { gitDiffRepoCommit, gitDiffFile, gitCommitList,  gitDiffRepoShow,
                                                        gitBlame,          gitRevlist,  gitBlameSummary };
    if(process && exclude_commands.count(ga.action) == 0) {
        if(HandleErrorsOnRemoteRepo(tmpOutput)) {
            return;
        }

        if(tmpOutput.Contains("username for")) {
            // username is required
            wxString username = ::wxGetTextFromUser(output);
            if(username.IsEmpty()) {
                process->Terminate();
            } else {
                process->WriteToConsole(username);
            }

        } else if(tmpOutput.Contains("commit-msg hook failure") || tmpOutput.Contains("pre-commit hook failure")) {
            process->Terminate();

        } else if(tmpOutput.Contains("*** please tell me who you are")) {
            process->Terminate();
            GitUserEmailDialog userEmailDialog(EventNotifier::Get()->TopFrame());
            if(userEmailDialog.ShowModal() == wxID_OK) {
                wxString username = userEmailDialog.GetUsername();
                wxString emaiil = userEmailDialog.GetEmail();
                {
                    wxString args;
                    args << " user.email \"" << emaiil << "\"";
                    gitAction act(gitConfig, args);
                    m_gitActionQueue.push_back(act);
                }
                {
                    wxString args;
                    args << " user.name \"" << username << "\"";
                    gitAction act(gitConfig, args);
                    m_gitActionQueue.push_back(act);
                }
            }
        } else if(tmpOutput.EndsWith("password:") || tmpOutput.Contains("password for") ||
                  tmpOutput.Contains("authentication failed")) {

            // Password is required
            wxString pass = ::wxGetPasswordFromUser(output);
            if(pass.IsEmpty()) {

                // No point on continuing
                process->Terminate();

            } else {

                // write the password
                process->WriteToConsole(pass);
            }
        } else if((tmpOutput.Contains("the authenticity of host") && tmpOutput.Contains("can't be established")) ||
                  tmpOutput.Contains("key fingerprint")) {
            if(::wxMessageBox(tmpOutput, _("Are you sure you want to continue connecting"),
                              wxYES_NO | wxCENTER | wxICON_QUESTION) == wxYES) {
                process->WriteToConsole("yes");

            } else {
                process->Terminate();
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

void GitPlugin::InitDefaults()
{
    DoCreateTreeImages();
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

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

    LoadDefaultGitCommands(data); // Always do this, in case of new entries
    conf.WriteItem(&data);
    conf.Save();

    if(IsWorkspaceOpened()) {
        m_repositoryDirectory = GetRepositoryPath();

        // Load any unusual git-repo path
        wxString projectNameHash, UserEnteredRepoPath;
        if(!m_isRemoteWorkspace) {
            wxString workspaceName = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetName();
            wxString projectName = m_mgr->GetWorkspace()->GetActiveProjectName();
            if(!workspaceName.empty() && !projectName.empty()) {
                projectNameHash << workspaceName << '-' << projectName;
                m_userEnteredRepositoryDirectory = data.GetProjectUserEnteredRepoPath(projectNameHash);
                m_repositoryDirectory = m_userEnteredRepositoryDirectory;
            }
        }
    } else {
        DoCleanup();
    }

    if(!m_repositoryDirectory.IsEmpty()) {
        m_console->AddLine("Initializing git...");
        gitAction ga(gitListAll, wxT(""));
        m_gitActionQueue.push_back(ga);
        AddDefaultActions();
        ProcessGitActionQueue();
    }
}

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

void GitPlugin::ColourFileTree(clTreeCtrl* tree, const wxStringSet_t& files, OverlayTool::BmpType bmpType) const
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    if(!(data.GetFlags() & GitEntry::Git_Colour_Tree_View))
        return;

    std::stack<wxTreeItemId> items;
    if(tree->GetRootItem().IsOk())
        items.push(tree->GetRootItem());

    while(!items.empty()) {
        wxTreeItemId next = items.top();
        items.pop();

        if(next != tree->GetRootItem()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(tree->GetItemData(next));
            const wxString& path = data->GetData().GetFile();
            if(!path.IsEmpty() && files.count(path)) {
                DoSetTreeItemImage(tree, next, bmpType);
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

void GitPlugin::CreateFilesTreeIDsMap(std::map<wxString, wxTreeItemId>& IDs, bool ifmodified /*=false*/) const
{
    clTreeCtrl* tree = m_mgr->GetWorkspaceTree();
    if(!tree) {
        return;
    }

    IDs.clear();

    std::stack<wxTreeItemId> items;
    if(tree->GetRootItem().IsOk())
        items.push(tree->GetRootItem());

    while(!items.empty()) {
        wxTreeItemId next = items.top();
        items.pop();

        if(next != tree->GetRootItem()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(tree->GetItemData(next));
            const wxString& path = data->GetData().GetFile();
            if(!path.IsEmpty()) {
                // If m_modifiedFiles has already been filled, only include files listed there
                if(!ifmodified || m_modifiedFiles.count(path)) {
                    IDs[path] = next;
                }
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

void GitPlugin::OnProgressTimer(wxTimerEvent& Event)
{
    if(m_console->IsProgressShown())
        m_console->PulseProgress();
}

void GitPlugin::ShowProgress(const wxString& message, bool pulse)
{
    m_console->ShowProgress(message, pulse);

    if(pulse) {
        m_progressTimer.Start(50);

    } else {
        m_progressMessage = message;
    }
}

void GitPlugin::HideProgress()
{
    m_console->HideProgress();
    m_progressTimer.Stop();
}

void GitPlugin::OnEnableGitRepoExists(wxUpdateUIEvent& e) { e.Enable(m_repositoryDirectory.IsEmpty() == false); }

void GitPlugin::OnWorkspaceClosed(clWorkspaceEvent& e)
{
    e.Skip();
    m_blameMap.clear();
    WorkspaceClosed();
    m_lastBlameMessage.clear();
    ClearCodeLiteRemoteInfo();
    clGetManager()->GetStatusBar()->SetSourceControlBitmap(wxNullBitmap, "", "");
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
    // clear blame info
    m_blameMap.clear();
    clGetManager()->GetNavigationBar()->ClearLabel();
    m_lastBlameMessage.clear();
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
        wxTreeCtrl* tree = m_mgr->GetWorkspaceTree();

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

void GitPlugin::DoSetTreeItemImage(clTreeCtrl* ctrl, const wxTreeItemId& item, OverlayTool::BmpType bmpType) const
{
    wxUnusedVar(ctrl);
    wxUnusedVar(item);
    wxUnusedVar(bmpType);
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
    if(files.IsEmpty()) {
        return;
    }
    m_addedFiles = true;

    wxString filesToAdd;
    for(wxString file : files) {
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
    for(wxString file : files) {
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
    for(wxString file : files) {
        ::WrapWithQuotes(file);
        filesToDelete << file << " ";
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
    clTreeCtrl* tree = m_mgr->GetWorkspaceTree();
    if(!tree)
        return;

    wxArrayTreeItemIds items;
    tree->GetSelections(items);

    for(size_t i = 0; i < items.GetCount(); ++i) {
        FilewViewTreeItemData* itemData = dynamic_cast<FilewViewTreeItemData*>(tree->GetItemData(items.Item(i)));
        if(itemData && itemData->GetData().GetKind() == ProjectItem::TypeFile) {
            // we got a file
            wxFileName fn(itemData->GetData().GetFile());
            if(relativeToRepo && fn.IsAbsolute()) {
                fn.MakeRelativeTo(m_repositoryDirectory);
            }

            wxString filename = fn.GetFullPath();
            if(filename.Contains(" ")) {
                filename.Prepend("\"").Append("\"");
            }
            files.Add(filename);
        }
    }
}

void GitPlugin::DoShowDiffsForFiles(const wxArrayString& files, bool useFileAsBase)
{
    for(const wxString& file : files) {

        // and finally, perform the action
        // File name should be relative to the repo
        gitAction ga(gitDiffFile, file);
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

bool GitPlugin::IsWorkspaceOpened() const { return !m_workspace_file.empty(); }

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

        if(!overwrite && !data.GetGitCommandsEntries(name).GetCommands().empty()) {
            continue;
        }

        GitCommandsEntries gce(name);
        vGitLabelCommands_t commandEntries;
        wxArrayString entries = wxStringTokenize(item.AfterFirst('|'), ";");

        for(size_t entry = 0; entry < entries.GetCount(); ++entry) {
            wxString label = entries.Item(entry).BeforeFirst(',');
            wxString command = entries.Item(entry).AfterFirst(',');
            wxASSERT(!label.empty() && !command.empty());
            if(!label.empty() && !command.empty()) {
                commandEntries.push_back(GitLabelCommand(label, command));
            }
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

    wxString right_file;
    if(IsRemoteWorkspace()) {
#if USE_SFTP
        // download the file and store it locally
        wxFileName remote_file(GetRepositoryPath() + "/" + fileName);
        right_file =
            clSFTPManager::Get().Download(remote_file.GetFullPath(wxPATH_UNIX), m_remoteWorkspaceAccount).GetFullPath();
#else
        return;
#endif
    } else {
        wxFileName fnWorkingCopy(fileName);
        fnWorkingCopy.MakeAbsolute(m_repositoryDirectory);
        right_file = fnWorkingCopy.GetFullPath();
    }

    DiffSideBySidePanel::FileInfo r(right_file, _("Working copy"), false);
    clDiffFrame* diffView = new clDiffFrame(EventNotifier::Get()->TopFrame(), l, r, true);
    diffView->Show();
}

void GitPlugin::OnRebase(wxCommandEvent& e)
{
    wxString argumentString = e.GetString(); // This might be user-specified e.g. rebase --continue
    if(argumentString.empty()) {
        argumentString = GetAnyDefaultCommand("git_rebase");
    }
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

void GitPlugin::DoRecoverFromGitCommandError(bool clear_queue)
{
    if(clear_queue) {
        // Last action failed, clear queue
        while(!m_gitActionQueue.empty()) {
            m_gitActionQueue.pop_front();
        }
    } else {
        // do not clear the queue, but we must remove the top command
        // (the command that caused the latest failure)
        if(!m_gitActionQueue.empty()) {
            m_gitActionQueue.pop_front();
        }
    }
    wxDELETE(m_process);
    m_commandOutput.Clear();
    if(!clear_queue) {
        ProcessGitActionQueue();
    }
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
        if(editor) {
            ::wxSetWorkingDirectory(editor->GetFileName().GetPath());
        }
#ifndef __WXMSW__
        ::WrapInShell(bashcommand);
#endif
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
    DoRefreshView(false);
}

void GitPlugin::OnCommandOutput(clCommandEvent& event)
{
    m_console->AddText(event.GetString());
    wxString processOutput = event.GetString();
    processOutput.MakeLower();
    if(processOutput.Contains("username for")) {
        wxString user = ::wxGetTextFromUser(event.GetString(), "Git");
        if(!user.IsEmpty()) {
            event.SetString(user);
        }
    }
    if(processOutput.Contains("password for")) {
        wxString pass = ::wxGetPasswordFromUser(event.GetString(), "Git");
        if(!pass.IsEmpty()) {
            event.SetString(pass);
        }
    }
}

void GitPlugin::DoExecuteCommands(const GitCmd::Vec_t& commands, const wxString& workingDir)
{
    if(commands.empty())
        return;

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
    DoExecuteCommandSync("log -1 --pretty=format:\"%B\"", &lastCommitString);
    // Similarly the # and title of the last 100 commits for the commit history
    DoExecuteCommandSync("log -100 --abbrev-commit --pretty=oneline", &commitHistory);

    commitArgs.Clear();
    GitCommitDlg dlg(EventNotifier::Get()->TopFrame(), this, m_repositoryDirectory);
    dlg.AppendDiff(diff);
    dlg.SetPreviousCommitMessage(lastCommitString);
    dlg.SetHistory(commitHistory);
    if(dlg.ShowModal() == wxID_OK) {
        if(dlg.GetSelectedFiles().IsEmpty() && !dlg.IsAmending())
            return;
        wxString message = dlg.GetCommitMessage();
        if(!message.IsEmpty() || dlg.IsAmending()) {

            // amending?
            if(dlg.IsAmending()) {
                commitArgs << " --amend ";
            }

            // Add the message
            if(!message.IsEmpty()) {
                wxString messagefile = GetCommitMessageFile();
                ::WrapWithQuotes(messagefile);
                commitArgs << "--file=";
                commitArgs << messagefile << " ";

                if(IsRemoteWorkspace()) {
#if USE_SFTP
                    if(!clSFTPManager::Get().AwaitWriteFile(message, messagefile, m_remoteWorkspaceAccount)) {
                        m_console->AddText(_("ERROR: Failed to write commit message to file: ") + messagefile + "\n" +
                                           clSFTPManager::Get().GetLastError() + "\n");
                        return;
                    }
#else
                    return;
#endif
                } else {
                    if(!FileUtils::WriteFileContent(messagefile, message)) {
                        m_console->AddText(_("ERROR: Failed to write commit message to file: ") + messagefile + "\n");
                        return;
                    }
                }
            } else {
                // we are amending previous commit, use the previous commit message
                // by passing the --no-edit switch
                commitArgs << " --no-edit ";
            }
            wxArrayString selectedFiles = dlg.GetSelectedFiles();
            for(unsigned i = 0; i < selectedFiles.GetCount(); ++i)
                commitArgs << ::WrapWithQuotes(selectedFiles.Item(i)) << wxT(" ");

        } else {
            m_console->AddText(_("error: no commit message given, aborting"));
        }
    }
}

void GitPlugin::OnFolderCommit(wxCommandEvent& event)
{
    // 1. Get diff output
    wxString diff;
    bool res = DoExecuteCommandSync("diff --no-color HEAD", &diff, m_selectedFolder);
    if (diff.empty()) {
        DoExecuteCommandSync("diff --no-color --cached", &diff);
    }
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

bool GitPlugin::DoExecuteCommandSync(const wxString& command, wxString* commandOutput, const wxString& workingDir)
{
    commandOutput->Clear();
    if(!IsRemoteWorkspace()) {
        wxString git_command = m_pathGITExecutable;
        // Wrap the executable with quotes if needed
        git_command.Trim().Trim(false);
        ::WrapWithQuotes(git_command);
        git_command << " --no-pager ";
        git_command << command;

        GIT_MESSAGE(git_command);
        IProcess::Ptr_t gitProc(::CreateSyncProcess(git_command, IProcessCreateSync | IProcessWrapInShell, workingDir));
        if(gitProc) {
            gitProc->WaitForTerminate(*commandOutput);
        } else {
            return false;
        }

    } else {
        clEnvList_t env;
        wxString git_command = "git --no-pager ";
        git_command << command;
        GIT_MESSAGE(git_command);
        if(!m_remoteProcess.SyncExec(git_command, workingDir.empty() ? m_repositoryDirectory : workingDir, env,
                                     commandOutput)) {
            commandOutput->clear();
            return false;
        }
    }

    const wxString lcOutput = commandOutput->Lower();
    if(lcOutput.Contains("fatal:") || lcOutput.Contains("not a git repository")) {
        // prompt the user for the error
        commandOutput->clear();
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

    // Load any unusual git-repo path
    wxString projectNameHash;
    if(!m_isRemoteWorkspace) {
        wxString workspaceName = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetName();
        wxString projectName = m_mgr->GetWorkspace()->GetActiveProjectName();
        if(!workspaceName.empty() && !projectName.empty()) {
            projectNameHash << workspaceName << '-' << projectName;
            clConfig conf("git.conf");
            GitEntry data;
            conf.ReadItem(&data);
            m_userEnteredRepositoryDirectory = data.GetProjectUserEnteredRepoPath(projectNameHash);
        }
    }

    DoSetRepoPath(m_userEnteredRepositoryDirectory);
}

void GitPlugin::WorkspaceClosed()
{
    // Clearn any saved data from the current workspace
    // git commands etc
    DoCleanup();
    m_workspace_file.Clear();
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
    if(m_filesSelected.IsEmpty() || m_repositoryDirectory.empty())
        return;

    // We need to be symlink-aware here on Linux, so use CLRealPath
    wxString realfilepath = CLRealPath(m_filesSelected.Item(0));
    wxFileName fn(realfilepath);
    fn.MakeRelativeTo(CLRealPath(m_repositoryDirectory));

    DoGitBlame(fn.GetFullPath());
}

void GitPlugin::DisplayMessage(const wxString& message) const
{
    if(!message.empty()) {
        GIT_MESSAGE(message);
    }
}

void GitPlugin::DoRefreshView(bool ensureVisible)
{
    gitAction ga(gitListAll, wxT(""));
    m_gitActionQueue.push_back(ga);
    AddDefaultActions();
    if(ensureVisible) {
        m_mgr->ShowOutputPane("Git");
    }
    ProcessGitActionQueue();
}

void GitPlugin::OnAppActivated(wxCommandEvent& event)
{
    event.Skip();
    if(IsGitEnabled()) {
        CallAfter(&GitPlugin::DoRefreshView, false);
    }
}

bool GitPlugin::IsGitEnabled() const { return !m_repositoryDirectory.IsEmpty(); }

void GitPlugin::OnFileCreated(clFileSystemEvent& event)
{
    event.Skip();
    if(IsGitEnabled()) {
        // A file was created on the file system, add it to git if needed
        const wxArrayString& paths = event.GetPaths();
        DoAddFiles(paths);
        RefreshFileListView();
    }
}

void GitPlugin::OnReplaceInFiles(clFileSystemEvent& event)
{
    event.Skip();
    DoRefreshView(false);
}

void GitPlugin::OnEditorChanged(wxCommandEvent& event)
{
    event.Skip();
    // Git the basic git blame
    DoLoadBlameInfo(false);
}

void GitPlugin::DoLoadBlameInfo(bool clearCache)
{
    if(m_configFlags & GitEntry::Git_Hide_Blame_Status_Bar)
        return;

    if(!IsGitEnabled()) {
        return;
    }

    auto editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    // use the remote path if available
    wxString fullpath = editor->GetRemotePathOrLocal();
    if(m_blameMap.count(fullpath) && !clearCache) {
        return;
    }

    m_blameMap.erase(fullpath);
    gitAction ga(gitBlameSummary, fullpath);
    m_gitActionQueue.push_back(ga);
    ProcessGitActionQueue();
}

void GitPlugin::DoUpdateBlameInfo(const wxString& info, const wxString& fullpath)
{
    // parse the git blame output
    if(m_blameMap.count(fullpath)) {
        m_blameMap.erase(fullpath);
    }
    m_blameMap.insert({ fullpath, {} });
    auto& V = m_blameMap[fullpath];
    wxArrayString lines = ::wxStringTokenize(info, "\n", wxTOKEN_RET_DELIMS);
    V.reserve(lines.size());
    for(wxString& line : lines) {
        line = line.BeforeFirst(')');
        wxArrayString parts = ::wxStringTokenize(line, "\t )(", wxTOKEN_STRTOK);
        if(!parts.empty()) {
            // remove the line number part of the line
            parts.pop_back();
        }
        // build the line
        wxString comment;
        for(size_t i = 0; i < parts.size(); ++i) {
            wxString part = parts[i];
            if(i == (parts.size() - 1)) {
                part.Append(")").Prepend("(");
            } else if(i == 0) {
                part << ":";
            }
            comment << part << " ";
        }
        V.emplace_back(comment);
    }
}

void GitPlugin::OnUpdateNavBar(clCodeCompletionEvent& event)
{
    event.Skip();
    if(m_configFlags & GitEntry::Git_Hide_Blame_Status_Bar) {
        return;
    }

    auto editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    wxString fullpath = editor->GetRemotePathOrLocal();
    clDEBUG1() << "Checking blame info for file:" << fullpath << clEndl;
    auto where = m_blameMap.find(fullpath);

    if(where == m_blameMap.end()) {
        clDEBUG1() << "Could not get git blame for file:" << fullpath << clEndl;
        clGetManager()->GetNavigationBar()->ClearLabel();
        return;
    }

    size_t lineNumber = editor->GetCurrentLine();
    if(lineNumber < where->second.size()) {
        const wxString& newmsg = where->second[lineNumber];
        if(m_lastBlameMessage != newmsg) {
            m_lastBlameMessage = newmsg;
            clGetManager()->GetNavigationBar()->SetLabel(newmsg);
        }
    }
}

void GitPlugin::OnEditorClosed(wxCommandEvent& event)
{
    event.Skip();
    IEditor* editor = (IEditor*)event.GetClientData();
    CHECK_PTR_RET(editor);
    m_blameMap.erase(editor->GetFileName().GetFullPath());
    m_lastBlameMessage.clear();
}

void GitPlugin::OnGitActionDone(clSourceControlEvent& event)
{
    // whenever a git action is performed, we clear the blame info
    // and reload it for the current file
    event.Skip();
    m_blameMap.clear();
    m_lastBlameMessage.clear();
    DoLoadBlameInfo(false);
}

IProcess* GitPlugin::AsyncRunGit(wxEvtHandler* handler, const wxString& command_args, size_t create_flags,
                                 const wxString& working_directory, bool logMessage)
{
    if(m_isRemoteWorkspace) {
        wxString command;
        command << "git " << command_args;

        clEnvList_t env;
        if(logMessage) {
            m_console->PrintPrompt();
        }

        GIT_MESSAGE_IF(logMessage, command);
        return m_remoteProcess.CreateAsyncProcess(handler, command, working_directory, env);

    } else {
        wxString command = m_pathGITExecutable;

        // Wrap the executable with quotes if needed
        command.Trim().Trim(false);
        ::WrapWithQuotes(command);

        command << " " << command_args;
        if(logMessage) {
            m_console->PrintPrompt();
        }
        GIT_MESSAGE_IF(logMessage, command);

        auto process = ::CreateAsyncProcess(handler, command, create_flags | IProcessWrapInShell, working_directory);
        return process;
    }
}

void GitPlugin::AsyncRunGitWithCallback(const wxString& command_args, std::function<void(const wxString&)> callback,
                                        size_t create_flags, const wxString& working_directory, bool logMessage)
{
    if(m_isRemoteWorkspace) {
        wxString command;
        command << "git " << command_args;

        clEnvList_t env;
        GIT_MESSAGE_IF(logMessage, command);
        m_remoteProcess.CreateAsyncProcessCB(command, std::move(callback), working_directory, env);
    } else {
        wxString command = m_pathGITExecutable;

        // Wrap the executable with quotes if needed
        command.Trim().Trim(false);
        ::WrapWithQuotes(command);

        command << " " << command_args;
        GIT_MESSAGE_IF(logMessage, command);
        ::CreateAsyncProcessCB(command, callback, create_flags, working_directory, nullptr);
    }
}

IEditor* GitPlugin::OpenFile(const wxString& relativePathFile)
{
    wxFileName fn(GetRepositoryPath() + "/" + relativePathFile);
    if(IsRemoteWorkspace()) {
#if USE_SFTP
        return clSFTPManager::Get().OpenFile(fn.GetFullPath(wxPATH_UNIX), m_remoteWorkspaceAccount);
#else
        return nullptr;
#endif
    } else {
        return clGetManager()->OpenFile(fn.GetFullPath());
    }
}

wxString GitPlugin::GetCommitMessageFile() const
{
    if(IsRemoteWorkspace()) {
        return "/tmp/CL_GIT_COMMIT_MSG.TXT";
    } else {
        wxFileName fn(clStandardPaths::Get().GetTempDir() + "/CL_GIT_COMMIT_MSG.TXT");
        return fn.GetFullPath(IsRemoteWorkspace() ? wxPATH_UNIX : wxPATH_NATIVE);
    }
}

wxString GitPlugin::FindRepositoryRoot(const wxString& starting_dir) const
{
    if(IsRemoteWorkspace()) {
        return starting_dir;
    }

    wxFileName fp(starting_dir, "");
    while(fp.GetDirCount()) {
        wxFileName gitdir(fp.GetPath(), wxEmptyString);
        gitdir.AppendDir(".git");
        if(wxFileName::DirExists(gitdir.GetPath())) {
            wxString realfilepath = CLRealPath(gitdir.GetPath());
            if(realfilepath != gitdir.GetPath() && wxFileName::DirExists(realfilepath)) {
                return realfilepath.BeforeLast('.');
            }
            gitdir.RemoveLastDir(); // remove the .git folder
            return gitdir.GetPath();
        }
        fp.RemoveLastDir();
    }
    return starting_dir;
}

void GitPlugin::OnFindPath(clCommandEvent& event)
{
    wxUnusedVar(event);
    if(event.GetEventType() == wxEVT_CODELITE_REMOTE_FINDPATH) {
        if(!event.GetString().empty()) {
            clDEBUG() << ".git folder found at:" << event.GetString() << endl;
            wxString new_path = event.GetString();
            new_path = new_path.BeforeLast('.');
            clDEBUG() << "Setting repository path at:" << new_path << endl;
            DoSetRepoPath(new_path);
            RefreshFileListView();
        }
    }
}

void GitPlugin::OnSftpFileSaved(clCommandEvent& event)
{
    event.Skip();
    if(IsGitEnabled()) {
        // file saved remotely, refresh the view
        RefreshFileListView();
    }
}
