//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : subversion2.cpp
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

#include "SvnCommitDialog.h"
#include "SvnLogDialog.h"
#include "SvnShowFileChangesHandler.h"
#include "SvnShowRecentChangesDlg.h"
#include "clGotoAnythingManager.h"
#include "cl_standard_paths.h"
#include "detachedpanesinfo.h"
#include "dockablepane.h"
#include "event_notifier.h"
#include "fileutils.h"
#include "globals.h"
#include "procutils.h"
#include "subversion2.h"
#include "subversion_password_db.h"
#include "subversion_strings.h"
#include "subversion_view.h"
#include "svn_command_handlers.h"
#include "svn_console.h"
#include "svn_login_dialog.h"
#include "svn_patch_dlg.h"
#include "svn_preferences_dialog.h"
#include "svn_sync_dialog.h"
#include "svnstatushandler.h"
#include "svnxml.h"
#include <algorithm>
#include <wx/app.h>
#include <wx/dir.h>
#include <wx/ffile.h>
#include <wx/fileconf.h>
#include <wx/filedlg.h>
#include <wx/filefn.h>
#include <wx/imaglist.h>
#include <wx/menu.h>
#include <wx/menuitem.h>
#include <wx/msgdlg.h>
#include <wx/numdlg.h>
#include <wx/regex.h>
#include <wx/stdpaths.h>
#include <wx/textdlg.h>
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>

static Subversion2* thePlugin = NULL;

// Convert to Windows EOL
static void ConvertToWindowsEOL(wxString& str)
{
    wxString newBuffer;
    newBuffer.Alloc(str.Len());

    for(size_t i = 0; i < str.Len(); i++) {
        wxChar nextChar = wxT('\0');
        wxChar ch = str.GetChar(i);
        if((i + 1) < str.Len()) { nextChar = str.GetChar(i + 1); }

        if(ch == wxT('\r') && nextChar == wxT('\n')) {
            newBuffer << wxT("\r\n");
            i++;

        } else if(ch == wxT('\n')) {
            newBuffer << wxT("\r\n");

        } else if(ch == wxT('\r') && nextChar != wxT('\n')) {
            newBuffer << wxT("\r\n");

        } else {
            newBuffer.Append(ch);
        }
    }
    str.swap(newBuffer);
}

// Convert to Unix style
static void ConvertToUnixEOL(wxString& str)
{
    wxString newBuffer;
    newBuffer.Alloc(str.Len());

    for(size_t i = 0; i < str.Len(); i++) {
        wxChar nextChar = wxT('\0');
        wxChar ch = str.GetChar(i);
        if((i + 1) < str.Len()) { nextChar = str.GetChar(i + 1); }

        if(ch == wxT('\r') && nextChar == wxT('\n')) {
            newBuffer << wxT("\n");
            i++;

        } else if(ch == wxT('\r') && nextChar != wxT('\n')) {
            newBuffer << wxT("\n");

        } else {
            newBuffer.Append(ch);
        }
    }
    str.swap(newBuffer);
}

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) { thePlugin = new Subversion2(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("Subversion"));
    info.SetDescription(_("Subversion plugin for codelite2.0 based on the svn command line tool"));
    info.SetVersion(wxT("v2.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

Subversion2::Subversion2(IManager* manager)
    : IPlugin(manager)
    , m_explorerSepItem(NULL)
    , m_projectSepItem(NULL)
    , m_simpleCommand(this)
    , m_diffCommand(this)
    , m_blameCommand(this)
    , m_svnClientVersion(0.0)
    , m_skipRemoveFilesDlg(false)
    , m_clientVersion(1700) // 1.7.0 (1*1000 + 7*100 + 0)
{
    m_longName = _("Subversion plugin for codelite2.0 based on the svn command line tool");
    m_shortName = wxT("Subversion2");

    DoInitialize();

    GetManager()->GetTheApp()->Connect(XRCID("subversion2_settings"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnSettings), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_commit"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnCommit), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_update"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnUpdate), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_add"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnFolderAdd), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_delete"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnDeleteFolder), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_rename"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnFileExplorerRenameItem), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_revert"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnFileExplorerRevertItem), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_revert_to_revision"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnRevertToRevision), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_diff"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnFileExplorerDiff), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_log"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnLog), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_blame"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnBlame), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_ignore_file"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnIgnoreFile), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_ignore_file_pattern"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnIgnoreFilePattern), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_set_as_view"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnSelectAsView), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_unlock"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnUnLockFile), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_lock"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnLockFile), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_workspace_sync"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnSync), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_show_changes"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnShowFileChanges), NULL, this);

    EventNotifier::Get()->Connect(wxEVT_GET_ADDITIONAL_COMPILEFLAGS, clBuildEventHandler(Subversion2::OnGetCompileLine),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CONFIG_CHANGED,
                                  wxCommandEventHandler(Subversion2::OnWorkspaceConfigChanged), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PROJ_FILE_REMOVED, clCommandEventHandler(Subversion2::OnProjectFileRemoved),
                                  NULL, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &Subversion2::OnFolderContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FILE, &Subversion2::OnFileContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_DELETED, &Subversion2::OnFileDeleted, this);
    EventNotifier::Get()->Bind(wxEVT_FOLDER_DELETED, &Subversion2::OnFolderDeleted, this);
    EventNotifier::Get()->Bind(wxEVT_GOTO_ANYTHING_SHOWING, &Subversion2::OnGotoAnythingShowing, this);
    // Register common SVN actins into the "Goto Anything" manager
    // clGotoAnythingManager::Get().Add(clGotoEntry("Svn > Commit", "", XRCID("svn_commit")));
    // clGotoAnythingManager::Get().Add(clGotoEntry("Svn > Update", "", XRCID("svn_update")));
}

Subversion2::~Subversion2() {}

clToolBar* Subversion2::CreateToolBar(wxWindow* parent)
{
    wxUnusedVar(parent);
    return NULL;
}

void Subversion2::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxUnusedVar(pluginsMenu);

    // You can use the below code a snippet:
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item = new wxMenuItem(menu, XRCID("subversion2_settings"), _("Subversion Options"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, wxT("Subversion2"), menu);
}

wxMenu* Subversion2::CreateProjectPopMenu()
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("svn_workspace_sync"), _("Sync Project Files..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    return menu;
}

void Subversion2::HookPopupMenu(wxMenu* menu, MenuType type)
{
    if(type == MenuTypeFileView_Project) {
        if(!menu->FindItem(XRCID("SUBVERSION_PROJECT_POPUP"))) {
            m_projectSepItem = menu->PrependSeparator();
            menu->Prepend(XRCID("SUBVERSION_PROJECT_POPUP"), wxT("Subversion"), CreateProjectPopMenu());
        }
    }
}

wxMenu* Subversion2::CreateFileExplorerPopMenu(bool isFile)
{
    // Create the popup menu for the file explorer
    // The only menu that we are interested is the file explorer menu
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    if(!isFile) {
        item = new wxMenuItem(menu, XRCID("svn_explorer_set_as_view"), _("Watch this folder"), wxEmptyString,
                              wxITEM_NORMAL);
        menu->Append(item);
        menu->AppendSeparator();
    }

    item = new wxMenuItem(menu, XRCID("svn_explorer_update"), _("Update"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("svn_explorer_commit"), _("Commit"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("svn_explorer_delete"), _("Delete"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    menu->AppendSeparator();
    item = new wxMenuItem(menu, XRCID("svn_explorer_revert"), _("Revert changes"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    menu->AppendSeparator();

    if(isFile) {
        item = new wxMenuItem(menu, XRCID("svn_explorer_lock"), _("Lock file"), wxEmptyString, wxITEM_NORMAL);
        menu->Append(item);

        item = new wxMenuItem(menu, XRCID("svn_explorer_unlock"), _("UnLock file"), wxEmptyString, wxITEM_NORMAL);
        menu->Append(item);

        menu->AppendSeparator();

        item = new wxMenuItem(menu, XRCID("svn_explorer_show_changes"), _("Show Recent Changes"), wxEmptyString,
                              wxITEM_NORMAL);
        menu->Append(item);
        menu->AppendSeparator();
    }

    item = new wxMenuItem(menu, XRCID("svn_explorer_add"), _("Add"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("svn_explorer_rename"), _("Rename"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    menu->AppendSeparator();

    item = new wxMenuItem(menu, XRCID("svn_explorer_revert_to_revision"), _("Revert to revision"), wxEmptyString,
                          wxITEM_NORMAL);
    menu->Append(item);
    menu->AppendSeparator();

    item = new wxMenuItem(menu, XRCID("svn_explorer_diff"), _("Create Diff"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    item = new wxMenuItem(menu, XRCID("svn_explorer_log"), _("Change Log..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    return menu;
}

void Subversion2::UnPlug()
{
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FOLDER, &Subversion2::OnFolderContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FILE, &Subversion2::OnFileContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_DELETED, &Subversion2::OnFileDeleted, this);
    EventNotifier::Get()->Unbind(wxEVT_FOLDER_DELETED, &Subversion2::OnFolderDeleted, this);
    EventNotifier::Get()->Unbind(wxEVT_GOTO_ANYTHING_SHOWING, &Subversion2::OnGotoAnythingShowing, this);

    m_tabToggler.reset(NULL);
    GetManager()->GetTheApp()->Disconnect(XRCID("subversion2_settings"), wxEVT_COMMAND_MENU_SELECTED,
                                          wxCommandEventHandler(Subversion2::OnSettings), NULL, this);
    GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_commit"), wxEVT_COMMAND_MENU_SELECTED,
                                          wxCommandEventHandler(Subversion2::OnCommit), NULL, this);
    GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_update"), wxEVT_COMMAND_MENU_SELECTED,
                                          wxCommandEventHandler(Subversion2::OnUpdate), NULL, this);
    GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_add"), wxEVT_COMMAND_MENU_SELECTED,
                                          wxCommandEventHandler(Subversion2::OnFolderAdd), NULL, this);
    GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_delete"), wxEVT_COMMAND_MENU_SELECTED,
                                          wxCommandEventHandler(Subversion2::OnDeleteFolder), NULL, this);
    GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_rename"), wxEVT_COMMAND_MENU_SELECTED,
                                          wxCommandEventHandler(Subversion2::OnFileExplorerRenameItem), NULL, this);
    GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_revert"), wxEVT_COMMAND_MENU_SELECTED,
                                          wxCommandEventHandler(Subversion2::OnFileExplorerRevertItem), NULL, this);
    GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_diff"), wxEVT_COMMAND_MENU_SELECTED,
                                          wxCommandEventHandler(Subversion2::OnFileExplorerDiff), NULL, this);
    GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_log"), wxEVT_COMMAND_MENU_SELECTED,
                                          wxCommandEventHandler(Subversion2::OnLog), NULL, this);
    GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_blame"), wxEVT_COMMAND_MENU_SELECTED,
                                          wxCommandEventHandler(Subversion2::OnBlame), NULL, this);
    GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_ignore_file"), wxEVT_COMMAND_MENU_SELECTED,
                                          wxCommandEventHandler(Subversion2::OnIgnoreFile), NULL, this);
    GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_ignore_file_pattern"), wxEVT_COMMAND_MENU_SELECTED,
                                          wxCommandEventHandler(Subversion2::OnIgnoreFilePattern), NULL, this);
    GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_set_as_view"), wxEVT_COMMAND_MENU_SELECTED,
                                          wxCommandEventHandler(Subversion2::OnSelectAsView), NULL, this);
    GetManager()->GetTheApp()->Disconnect(XRCID("svn_workspace_sync"), wxEVT_COMMAND_MENU_SELECTED,
                                          wxCommandEventHandler(Subversion2::OnSync), NULL, this);
    GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_show_changes"), wxEVT_COMMAND_MENU_SELECTED,
                                       wxCommandEventHandler(Subversion2::OnShowFileChanges), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_GET_ADDITIONAL_COMPILEFLAGS,
                                     clBuildEventHandler(Subversion2::OnGetCompileLine), NULL, this);

    m_subversionView->DisconnectEvents();

    // Remove the tab if it's actually docked in the workspace pane
    int index(wxNOT_FOUND);
    index = m_mgr->GetOutputPaneNotebook()->GetPageIndex(m_subversionView);
    if(index != wxNOT_FOUND) { m_mgr->GetOutputPaneNotebook()->RemovePage(index); }

    m_subversionView->Destroy();
}

void Subversion2::EnsureVisible()
{
    // Ensure that the Output View is displayed
    wxAuiPaneInfo& pi = GetManager()->GetDockingManager()->GetPane("Output View");
    if(pi.IsOk() && !pi.IsShown()) {
        pi.Show(true);
        GetManager()->GetDockingManager()->Update();
    }

    Notebook* book = GetManager()->GetOutputPaneNotebook();
    for(size_t i = 0; i < book->GetPageCount(); i++) {
        if(m_subversionView == book->GetPage(i)) {
            book->SetSelection(i);
            break;
        }
    }
}

void Subversion2::DoInitialize()
{
    m_svnBitmap = GetManager()->GetStdIcons()->LoadBitmap(wxT("subversion"));

    // create tab (possibly detached)
    Notebook* book = m_mgr->GetOutputPaneNotebook();
    if(IsSubversionViewDetached()) {
        // Make the window child of the main panel (which is the grand parent of the notebook)
        DockablePane* cp = new DockablePane(book->GetParent()->GetParent(), book, svnCONSOLE_TEXT, false, wxNullBitmap,
                                            wxSize(200, 200));
        m_subversionView = new SubversionView(cp, this);
        cp->SetChildNoReparent(m_subversionView);
    } else {
        m_subversionView = new SubversionView(book, this);
        book->AddPage(m_subversionView, svnCONSOLE_TEXT, false, m_svnBitmap);
    }
    m_tabToggler.reset(new clTabTogglerHelper(svnCONSOLE_TEXT, m_subversionView, "", NULL));
    m_tabToggler->SetOutputTabBmp(m_svnBitmap);

    DoSetSSH();
    // We need to perform a dummy call to svn so it will create all the default
    // setup directory layout
    wxString command;
    wxArrayString output;

    command << GetSvnExeName() << wxT(" --help ");

#ifndef __WXMSW__
    command << wxT("> /dev/null 2>&1");
#endif

    ProcUtils::ExecuteCommand(command, output);

    DoGetSvnVersion();
    DoGetSvnClientVersion();
    RecreateLocalSvnConfigFile();
}

SvnSettingsData Subversion2::GetSettings()
{
    SvnSettingsData ssd;
    GetManager()->GetConfigTool()->ReadObject(wxT("SvnSettingsData"), &ssd);
    return ssd;
}

void Subversion2::SetSettings(SvnSettingsData& ssd)
{
    GetManager()->GetConfigTool()->WriteObject(wxT("SvnSettingsData"), &ssd);
}

void Subversion2::OnSettings(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EditSettings();
}

void Subversion2::DoSetSSH()
{
    wxString sshClient = GetSettings().GetSshClient();
    wxString sshClientArgs = GetSettings().GetSshClientArgs();

    sshClient.Trim().Trim(false);
    sshClientArgs.Trim().Trim(false);

    // on Windows, SVN demands that the ssh client will not contain any
    // backward slashes
    sshClient.Replace(wxT("\\"), wxT("/"));

    if(sshClient.IsEmpty() == false) {
        wxString env_value(sshClient + wxT(" ") + sshClientArgs);
        wxSetEnv(wxT("SVN_SSH"), env_value.c_str());
    }
}

////////////////////////////////////////////////
// File Explorer SVN command handlers
////////////////////////////////////////////////

void Subversion2::OnFolderAdd(wxCommandEvent& event)
{
    wxString command;
    wxString loginString;
    if(LoginIfNeeded(event, DoGetFileExplorerItemPath(), loginString) == false) { return; }

    wxFileName workingDirectory(m_selectedFolder, "");
    if(m_selectedFile.IsOk()) {
        command << GetSvnExeName() << loginString << wxT(" add ") << m_selectedFile.GetFullName();
    } else {
        wxString folderName = workingDirectory.GetDirs().Last();
        ::WrapWithQuotes(folderName);

        workingDirectory.RemoveLastDir();
        command << GetSvnExeName() << loginString << wxT(" add ") << folderName;
    }
    GetConsole()->Execute(command, workingDirectory.GetPath(), new SvnStatusHandler(this, event.GetId(), this));
}

void Subversion2::OnCommit(wxCommandEvent& event)
{
    // Coming from file explorer
    wxArrayString paths;
    if(!m_selectedFile.IsOk()) {
        paths.Add(".");
    } else {
        paths.Add(m_selectedFile.GetFullName());
    }
    DoCommit(paths, m_selectedFolder, event);
}

void Subversion2::OnDeleteFolder(wxCommandEvent& event)
{
    // Coming from file explorer
    wxString command;
    wxString loginString;
    if(LoginIfNeeded(event, m_selectedFolder, loginString) == false) { return; }

    // svn delete --force <folder-name>
    wxFileName workingDirectory(m_selectedFolder, "");
    if(!m_selectedFile.IsOk()) {
        wxString folderName = workingDirectory.GetDirs().Last();
        ::WrapWithQuotes(folderName);

        workingDirectory.RemoveLastDir();
        command << GetSvnExeName() << loginString << wxT(" delete --force ") << folderName;

    } else {
        command << GetSvnExeName() << loginString << wxT(" delete --force ") << m_selectedFile.GetFullName();
    }
    GetConsole()->Execute(command, workingDirectory.GetPath(), new SvnDefaultCommandHandler(this, event.GetId(), this));
}

void Subversion2::OnFileExplorerRevertItem(wxCommandEvent& event)
{
    // Coming from the file explorer
    if(wxMessageBox(_("You are about to revert all your changes\nAre you sure?"), "CodeLite",
                    wxICON_WARNING | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxCENTER) != wxYES) {
        return;
    }

    wxString command;
    if(m_selectedFile.FileExists()) {
        // Revert was called on a file, revert only the file
        command << GetSvnExeName() << " revert --recursive " << m_selectedFile.GetFullName();
    } else {
        // Revert the folder
        command << GetSvnExeName() << " revert --recursive .";
    }
    GetConsole()->Execute(command, m_selectedFolder, new SvnDefaultCommandHandler(this, event.GetId(), this));
}

void Subversion2::OnUpdate(wxCommandEvent& event)
{
    // Coming from explorer view
    wxString command;
    wxString loginString;
    if(LoginIfNeeded(event, m_selectedFolder, loginString) == false) { return; }
    // svn update .
    command << GetSvnExeName() << loginString << wxT(" update ") << m_selectedFile.GetFullName() << " ";
    AddCommandLineOption(command, kOpt_ForceInteractive);
    command << ".";

    // Execute the command, but with console visible
    GetConsole()->Execute(command, m_selectedFolder, new SvnUpdateHandler(this, event.GetId(), this), true, true);
}

void Subversion2::OnFileExplorerDiff(wxCommandEvent& event)
{
    wxString diffAgainst(wxT("BASE"));
    diffAgainst = clGetTextFromUser(_("Svn Diff"), _("Insert base revision to diff against:"), "BASE", wxNOT_FOUND,
                                    GetManager()->GetTheApp()->GetTopWindow());
    if(diffAgainst.empty()) return;

    wxString command;
    wxString loginString;
    if(LoginIfNeeded(event, m_selectedFolder, loginString) == false) { return; }

    command << GetSvnExeNameNoConfigDir() << loginString;

    SvnSettingsData ssd = GetSettings();
    if(ssd.GetFlags() & SvnUseExternalDiff) { command << " --diff-cmd=\"" << ssd.GetExternalDiffViewer() << "\" "; }

    wxFileName workingDirectory(m_selectedFolder, "");
    command << wxT("diff -r") << diffAgainst;
    if(m_selectedFile.IsOk()) {
        command << " " << m_selectedFile.GetFullName();
    } else {
        command << " .";
    }
    GetConsole()->Execute(command, workingDirectory.GetPath(), new SvnDiffHandler(this, event.GetId(), this), false);
}

wxString Subversion2::GetSvnExeName()
{
    SvnSettingsData ssd = GetSettings();

    wxString exeName = ssd.GetExecutable();
    exeName.Trim().Trim(false);
    ::WrapWithQuotes(exeName);

    exeName << " --config-dir";

    wxString configDir = GetUserConfigDir();
    ::WrapWithQuotes(configDir);

    exeName << " " << configDir;
    return exeName;
}

wxString Subversion2::DoGetFileExplorerFilesAsString()
{
    wxString s;
    wxArrayString files = DoGetFileExplorerFiles();
    for(size_t i = 0; i < files.GetCount(); i++) {
        s << wxT(" \"") << files.Item(i) << wxT("\" ");
    }
    return s;
}

wxArrayString Subversion2::DoGetFileExplorerFiles()
{
    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileExplorer);
    return item.m_paths;
}

wxString Subversion2::DoGetFileExplorerItemFullPath()
{
    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileExplorer);
    wxString filename(item.m_fileName.GetFullPath());
    filename.Trim().Trim(false);

    if(filename.EndsWith(wxT("\\"))) {
        filename.RemoveLast();

    } else if(filename.EndsWith(wxT("/"))) {
        filename.RemoveLast();
    }
    return filename;
}

wxString Subversion2::DoGetFileExplorerItemPath()
{
    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileExplorer);
    if(!item.m_paths.IsEmpty()) {
        return item.m_paths.Item(0);
    } else {
        return wxEmptyString;
    }
}

wxString Subversion2::GetUserConfigDir()
{
    wxString configDir(clStandardPaths::Get().GetUserDataDir());
    if(wxFileName::DirExists(configDir) == false) { wxMkdir(configDir); }

    configDir << wxFileName::GetPathSeparator() << wxT("subversion");
    return configDir;
}

void Subversion2::RecreateLocalSvnConfigFile()
{
    wxString configFile;
    wxString configDir = GetUserConfigDir();
    configFile << configDir << wxFileName::GetPathSeparator() << wxT("config");

    // Convert any whitespace to space
    wxString ignorePatterns(GetSettings().GetIgnoreFilePattern());
    ignorePatterns.Replace(wxT("\r\n"), wxT(" "));
    ignorePatterns.Replace(wxT("\n"), wxT(" "));
    ignorePatterns.Replace(wxT("\t"), wxT(" "));
    ignorePatterns.Replace(wxT("\v"), wxT(" "));

    wxString diffTool = GetSettings().GetExternalDiffViewer();
    if(!(GetSettings().GetFlags() & SvnUseExternalDiff)) { diffTool.Empty(); }

    wxFileConfig iniConfig("", "", configFile, "", wxCONFIG_USE_LOCAL_FILE);
    iniConfig.Write("miscellany/global-ignores", ignorePatterns);
    iniConfig.Write("helpers/diff-cmd", diffTool);
    iniConfig.Flush();
}

void Subversion2::DoGetSvnVersion()
{
    wxString command;
    command << GetSvnExeName() << wxT(" --version ");
    m_simpleCommand.Execute(command, wxT(""), new SvnVersionHandler(this, wxNOT_FOUND, NULL), this);
}

void Subversion2::Patch(bool dryRun, const wxString& workingDirectory, wxEvtHandler* owner, int id)
{
    PatchDlg dlg(GetManager()->GetTheApp()->GetTopWindow());
    if(dlg.ShowModal() == wxID_OK) {
        wxBusyCursor cursor;

        wxString patchFile;
        patchFile = dlg.GetFilePicker()->GetPath();
        int eolPolicy = dlg.GetRadioBoxPolicy()->GetSelection();
        bool removeFileWhenDone = false;

        if(eolPolicy != 0) {
            // Read the file
            wxString fileContent;
            if(ReadFileWithConversion(patchFile, fileContent)) {
                switch(eolPolicy) {
                case 1: // Windows EOL
                    ConvertToWindowsEOL(fileContent);
                    break;
                case 2: // Convert to UNIX style
                    ConvertToUnixEOL(fileContent);
                    break;
                }

                // Write the content to a new file
                wxFFile fileTemp;
                wxString tmpFile = wxFileName::CreateTempFileName(wxT("clsvn"), &fileTemp);
                if(fileTemp.IsOpened()) {
                    if(fileTemp.Write(fileContent)) {
                        fileTemp.Close();
                        removeFileWhenDone = true;
                        patchFile = tmpFile;
                    }
                }
            }
        }

        if(patchFile.IsEmpty() == false) {

            // execute the command
            wxString command;
            command << wxT("patch -l -p0 ");
            if(dryRun) command << wxT(" --dry-run  ");
            command << wxT(" -i \"") << patchFile << wxT("\"");

            SvnCommandHandler* handler(NULL);
            if(dryRun) {
                handler = new SvnPatchDryRunHandler(this, id, owner, removeFileWhenDone, patchFile);
            } else {
                handler = new SvnPatchHandler(this, id, owner, removeFileWhenDone, patchFile);
            }
            m_simpleCommand.Execute(command, workingDirectory, handler, this);
        }
    }
}

void Subversion2::OnLog(wxCommandEvent& event) { ChangeLog(m_selectedFolder, ".", event); }

bool Subversion2::GetNonInteractiveMode(wxCommandEvent& event) { return event.GetInt() != INTERACTIVE_MODE; }

bool Subversion2::LoginIfNeeded(wxCommandEvent& event, const wxString& workingDirectory, wxString& loginString)
{
    RecreateLocalSvnConfigFile();

    SvnInfo svnInfo;
    wxString repoUrl;

    if(event.GetInt() == LOGIN_REQUIRES_URL) {
        repoUrl = event.GetString();

    } else {
        DoGetSvnInfoSync(svnInfo, workingDirectory);
        repoUrl = svnInfo.m_url;
    }

    bool loginFailed = (event.GetInt() == LOGIN_REQUIRES) || (event.GetInt() == LOGIN_REQUIRES_URL);

    SubversionPasswordDb db;
    wxString user, password;

    if(loginFailed) {
        // if we got here, it means that we already tried to login with either user prompt / using the stored password
        // to prevent an endless loop, remove the old entry from the password db
        db.DeleteLogin(repoUrl);
    }

    if(db.GetLogin(repoUrl, user, password)) {
        loginString << wxT(" --username ") << user << wxT(" --password \"") << password << wxT("\" ");
        return true;
    }

    // Use the root URL as the key for the login here
    loginString.Empty();
    if(loginFailed) {
        SvnLoginDialog dlg(GetManager()->GetTheApp()->GetTopWindow());
        if(dlg.ShowModal() == wxID_OK) {
            loginString << wxT(" --username ") << dlg.GetUsername() << wxT(" --password \"") << dlg.GetPassword()
                        << wxT("\" ");
            // Store the user name and password
            db.SetLogin(repoUrl, dlg.GetUsername(), dlg.GetPassword());
            return true;
        } else {
            return false;
        }
    }
    return true;
}

void Subversion2::IgnoreFiles(const wxArrayString& files, bool pattern)
{
    SvnSettingsData ssd = GetSettings();
    wxArrayString ignorePatternArr = wxStringTokenize(ssd.GetIgnoreFilePattern(), wxT(" \r\n\t\v"), wxTOKEN_STRTOK);

    for(size_t i = 0; i < files.GetCount(); i++) {
        wxString entry;
        wxFileName fn(files.Item(i));
        if(pattern) {
            entry << wxT("*.") << fn.GetExt();
        } else {
            entry << fn.GetFullName();
        }

        if(ignorePatternArr.Index(entry) == wxNOT_FOUND) { ignorePatternArr.Add(entry); }
    }

    wxString ignorePatternStr;
    for(size_t i = 0; i < ignorePatternArr.GetCount(); i++) {
        ignorePatternStr << ignorePatternArr.Item(i) << wxT(" ");
    }
    ignorePatternStr.RemoveLast();
    ssd.SetIgnoreFilePattern(ignorePatternStr);

    // write down the changes
    SetSettings(ssd);

    // update the config file
    RecreateLocalSvnConfigFile();

    // refresh the view
    GetSvnView()->BuildTree();
}

void Subversion2::OnIgnoreFile(wxCommandEvent& event) { IgnoreFiles(DoGetFileExplorerFiles(), false); }

void Subversion2::OnIgnoreFilePattern(wxCommandEvent& event) { IgnoreFiles(DoGetFileExplorerFiles(), true); }

void Subversion2::EditSettings()
{
    SvnPreferencesDialog dlg(GetManager()->GetTheApp()->GetTopWindow(), this);
    if(dlg.ShowModal() == wxID_OK) {
        // Update the Subversion view
        GetSvnView()->BuildTree();
        DoSetSSH();
        RecreateLocalSvnConfigFile();
    }
}

bool Subversion2::IsSubversionViewDetached()
{
    DetachedPanesInfo dpi;
    m_mgr->GetConfigTool()->ReadObject(wxT("DetachedPanesList"), &dpi);
    wxArrayString detachedPanes = dpi.GetPanes();
    return detachedPanes.Index(svnCONSOLE_TEXT) != wxNOT_FOUND;
}

void Subversion2::OnSelectAsView(wxCommandEvent& event)
{
    wxUnusedVar(event);
    GetSvnView()->BuildTree(m_selectedFolder);
}

void Subversion2::OnBlame(wxCommandEvent& event) { Blame(event, DoGetFileExplorerFiles()); }

void Subversion2::Blame(wxCommandEvent& event, const wxArrayString& files)
{
    wxString command;
    wxString loginString;

    if(files.GetCount() == 0) return;

    /*bool nonInteractive = unused var commented out*/
    GetNonInteractiveMode(event);
    if(LoginIfNeeded(event, files.Item(0), loginString) == false) { return; }

    if(files.GetCount() != 1) return;

    GetConsole()->EnsureVisible();
    command << GetSvnExeName() << wxT(" blame ") << loginString;
    for(size_t i = 0; i < files.GetCount(); i++) {
        command << wxT("\"") << files.Item(i) << wxT("\" ");
    }

    GetConsole()->AppendText(command + wxT("\n"));
    m_blameCommand.Execute(command, GetSvnView()->GetRootDir(),
                           new SvnBlameHandler(this, event.GetId(), this, files.Item(0)), this);
}

void Subversion2::OnGetCompileLine(clBuildEvent& event)
{
    if(!(GetSettings().GetFlags() & SvnExposeRevisionMacro)) return;

    wxString macroName(GetSettings().GetRevisionMacroName());
    macroName.Trim().Trim(false);

    if(macroName.IsEmpty()) return;

    wxString workingDirectory = m_subversionView->GetRootDir();
    workingDirectory.Trim().Trim(false);

    SvnInfo svnInfo;
    DoGetSvnInfoSync(svnInfo, workingDirectory);

    wxString content = event.GetCommand();
    content << wxT(" -D");
    content << macroName << wxT("=\\\"");
    content << svnInfo.m_revision << wxT("\\\" ");
    event.SetCommand(content);
    event.Skip();
}

void Subversion2::DoGetSvnInfoSync(SvnInfo& svnInfo, const wxString& workingDirectory)
{
    wxString svnInfoCommand;
    wxString xmlStr;

    svnInfoCommand << GetSvnExeName() << wxT(" info --xml ");
    if(workingDirectory.Find(wxT(" ")))
        svnInfoCommand << wxT("\"") << workingDirectory << wxT("\"");
    else
        svnInfoCommand << workingDirectory;

#ifndef __WXMSW__
    // Hide stderr
    svnInfoCommand << " 2> /dev/null";
#endif

    wxArrayString xmlArr;
    ::WrapInShell(svnInfoCommand);

    wxLog::EnableLogging(false);
    IProcess::Ptr_t proc(::CreateSyncProcess(svnInfoCommand, IProcessCreateDefault | IProcessCreateWithHiddenConsole));
    if(proc) {
        proc->WaitForTerminate(xmlStr);
        SvnXML::GetSvnInfo(xmlStr, svnInfo);
    }

    wxLog::EnableLogging(true);
}

bool Subversion2::IsPathUnderSvn(const wxString& path)
{
    wxFileName fn(path, ".svn");

    // search until we find .svn folder
    while(fn.GetDirCount()) {
        if(wxFileName::DirExists(fn.GetFullPath())) { return true; }
        fn.RemoveLastDir();
    }
    return false;
}

void Subversion2::OnSwitchURL(wxCommandEvent& event)
{
    SvnInfo svnInfo;
    wxString path = DoGetFileExplorerItemPath();
    DoGetSvnInfoSync(svnInfo, path);
    DoSwitchURL(DoGetFileExplorerItemPath(), svnInfo.m_sourceUrl, event);
}

void Subversion2::DoSwitchURL(const wxString& workingDirectory, const wxString& sourceUrl, wxCommandEvent& event)
{
    SvnInfo svnInfo;
    DoGetSvnInfoSync(svnInfo, workingDirectory);

    wxString loginString;
    if(LoginIfNeeded(event, workingDirectory, loginString) == false) { return; }
    wxString targetUrl = wxGetTextFromUser(_("Enter new URL:"), wxT("Svn Switch..."), sourceUrl);
    if(targetUrl.IsEmpty()) { return; }

    wxString command;
    command << GetSvnExeName() << wxT(" switch ") << targetUrl << loginString;
    GetConsole()->Execute(command, workingDirectory, new SvnDefaultCommandHandler(this, wxNOT_FOUND, NULL));
}

void Subversion2::ChangeLog(const wxString& path, const wxString& fullpath, wxCommandEvent& event)
{
    SvnInfo info;
    DoGetSvnInfoSync(info, path);

    SvnLogDialog dlg(GetManager()->GetTheApp()->GetTopWindow());
    dlg.GetTo()->SetValue(wxT("BASE"));
    dlg.GetCompact()->SetValue(true);
    dlg.GetFrom()->SetFocus();
    if(dlg.ShowModal() == wxID_OK) {
        wxString command;
        wxString loginString;
        if(LoginIfNeeded(event, path, loginString) == false) { return; }

        command << GetSvnExeName() << loginString << wxT(" log -r") << dlg.GetFrom()->GetValue() << wxT(":")
                << dlg.GetTo()->GetValue() << wxT(" \"") << fullpath << wxT("\"");
        GetConsole()->Execute(
            command, path,
            new SvnLogHandler(this, info.m_sourceUrl, dlg.GetCompact()->IsChecked(), event.GetId(), this), false);
    }
}

void Subversion2::OnLockFile(wxCommandEvent& event)
{
    DoLockFile(m_selectedFile.GetPath(), DoGetFileExplorerFiles(), event, true);
}

void Subversion2::OnUnLockFile(wxCommandEvent& event)
{
    DoLockFile(m_selectedFile.GetPath(), DoGetFileExplorerFiles(), event, false);
}

void Subversion2::DoLockFile(const wxString& workingDirectory, const wxArrayString& fullpaths, wxCommandEvent& event,
                             bool lock)
{
    wxString command;
    wxString loginString;

    if(fullpaths.empty()) return;

    if(LoginIfNeeded(event, workingDirectory, loginString) == false) { return; }

    command << GetSvnExeName() << loginString;
    if(lock) {
        command << wxT(" lock ");
    } else {
        command << wxT(" unlock ");
    }

    for(size_t i = 0; i < fullpaths.size(); i++)
        command << wxT("\"") << fullpaths.Item(i) << wxT("\" ");

    GetConsole()->Execute(command, workingDirectory, new SvnDefaultCommandHandler(this, event.GetId(), this));
}

void Subversion2::OnWorkspaceConfigChanged(wxCommandEvent& event)
{
    event.Skip();
    m_subversionView->BuildTree();
}

void Subversion2::OnProjectFileRemoved(clCommandEvent& event)
{
    event.Skip();
    if(m_skipRemoveFilesDlg) {
        m_skipRemoveFilesDlg = false;
        return;
    }
    DoFilesDeleted(event.GetStrings());
}

void Subversion2::OnFileExplorerRenameItem(wxCommandEvent& event)
{
    wxFileName workingDirectory(m_selectedFolder, "");
    if(!m_selectedFile.IsOk()) {
        wxString folderName = workingDirectory.GetDirs().Last();
        workingDirectory.RemoveLastDir();
        wxString newname = ::clGetTextFromUser(_("Svn Rename"), _("New name:"), folderName, folderName.length());
        if(newname.IsEmpty() || newname == folderName) return;
        ::WrapWithQuotes(newname);
        DoRename(workingDirectory.GetPath(), folderName, newname, event);
    } else {
        wxString newname = ::clGetTextFromUser(_("Svn Rename"), _("New name:"), m_selectedFile.GetFullName(),
                                               m_selectedFile.GetName().length());
        if(newname.IsEmpty() || newname == m_selectedFile.GetFullName()) return;
        ::WrapWithQuotes(newname);
        DoRename(workingDirectory.GetPath(), m_selectedFile.GetFullName(), newname, event);
    }
}

void Subversion2::DoRename(const wxString& workingDirectory, const wxString& oldname, const wxString& newname,
                           wxCommandEvent& event)
{
    wxString command;
    wxString loginString;

    if(LoginIfNeeded(event, workingDirectory, loginString) == false) { return; }

    if(oldname.IsEmpty() || newname.IsEmpty() || workingDirectory.IsEmpty()) return;

    command << GetSvnExeName() << loginString << wxT(" rename --force ") << oldname << wxT(" ") << newname;
    GetConsole()->Execute(command, workingDirectory, new SvnDefaultCommandHandler(this, event.GetId(), this));
}

SvnConsole* Subversion2::GetConsole() { return GetSvnView()->GetSubversionConsole(); }

void Subversion2::DoCommit(const wxArrayString& files, const wxString& workingDirectory, wxCommandEvent& event)
{
    wxString command;
    wxString loginString;
    if(LoginIfNeeded(event, workingDirectory, loginString) == false) { return; }

    SvnInfo svnInfo;
    if(!workingDirectory.IsEmpty()) { DoGetSvnInfoSync(svnInfo, workingDirectory); }

    command << GetSvnExeName() << loginString << wxT(" commit ");

    SvnCommitDialog dlg(EventNotifier::Get()->TopFrame(), files, svnInfo.m_sourceUrl, this, workingDirectory);
    if(dlg.ShowModal() == wxID_OK) {
        wxArrayString actualFiles = dlg.GetPaths();
        if(actualFiles.IsEmpty()) return;

        // Store the commit message into a temporary file
        wxFileName tmpFile(clStandardPaths::Get().GetTempDir(), ".svn-commit");
        if(!FileUtils::WriteFileContent(tmpFile, dlg.GetMesasge())) {
            ::wxMessageBox(_("Fail to write commit message to a temporary file!"), "CodeLite",
                           wxOK | wxCENTER | wxICON_ERROR);
            return;
        }

        wxString filepath = tmpFile.GetFullPath();
        ::WrapWithQuotes(filepath);
        command << wxT(" --file ") << filepath << " ";

        // Add the changed files
        for(size_t i = 0; i < actualFiles.GetCount(); ++i) {
            ::WrapWithQuotes(actualFiles.Item(i));
            command << actualFiles.Item(i) << " ";
        }
        GetConsole()->Execute(command, workingDirectory, new SvnCommitHandler(this, event.GetId(), this));
    }
}

wxArrayString Subversion2::DoGetFileExplorerFilesToCommitRelativeTo(const wxString& wd)
{
    wxArrayString files;
    TreeItemInfo itemInfo = m_mgr->GetSelectedTreeItemInfo(TreeFileExplorer);
    files.swap(itemInfo.m_paths);

    for(size_t i = 0; i < files.GetCount(); i++) {
        if(wxDir::Exists(files.Item(i))) {
            // Get the list of modified files from the directory
            wxArrayString modFiles = DoGetSvnStatusQuiet(files.Item(i));

            for(size_t j = 0; j < modFiles.GetCount(); j++) {
                wxFileName fn(modFiles.Item(j));
                fn.MakeAbsolute(files.Item(i));
                fn.MakeRelativeTo(wd);

                if(files.Index(fn.GetFullPath()) == wxNOT_FOUND) { files.Add(fn.GetFullPath()); }
            }

        } else {
            wxFileName fn(files.Item(i));
            fn.MakeRelativeTo(wd);

            if(files.Index(fn.GetFullPath()) == wxNOT_FOUND) { files.Add(fn.GetFullPath()); }
        }
    }
    return files;
}

wxArrayString Subversion2::DoGetSvnStatusQuiet(const wxString& wd)
{
    wxString command;
    wxString output;

    command << GetSvnExeName() << wxT(" status -q ");
    command << wxT("\"") << wd << wxT("\"");

    wxArrayString lines;

    wxLog::EnableLogging(false);
    ProcUtils::ExecuteCommand(command, lines);

    for(size_t i = 0; i < lines.GetCount(); i++) {
        output << wxT("\r\n") << lines.Item(i);
    }

    wxArrayString modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles, lockedFiles, ignoredFiles;
    SvnXML::GetFiles(output, modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles, lockedFiles,
                     ignoredFiles);

    modFiles.insert(modFiles.end(), newFiles.begin(), newFiles.end());
    modFiles.insert(modFiles.end(), deletedFiles.begin(), deletedFiles.end());
    wxLog::EnableLogging(true);
    return modFiles;
}

bool Subversion2::NormalizeDir(wxString& wd)
{
    if(!wxFileName::DirExists(wd)) { return false; }

    // gets rid of possible trailing slash and fixes mixed-case issues
    wxFileName fn(wd);
    fn.Normalize(); // wxPATH_NORM_CASE seems broken
    wd = fn.GetFullPath();
    if(wxPATH_DOS == wxFileName::GetFormat()) {
        wd.LowerCase();

        // Subversion *always* capitalizes Windows/Dos volume letters
        wxChar volume = wd.GetChar(0);
        volume = toupper(volume);
        wd.SetChar(0, volume);
    }
    // get rid of possible trailing slash/backslash
    if(wd.Last() == wxFileName::GetPathSeparator()) { wd.RemoveLast(); }
    return true;
}

std::vector<wxString> Subversion2::GetLocalAddsDels(const wxString& wd)
{
    wxString command;

    command << GetSvnExeName() << wxT(" status -q ");
    command << wxT("\"") << wd << wxT("\"");

    wxLog::EnableLogging(false);

    std::vector<wxString> aryFiles;
    wxArrayString lines;

    ProcUtils::ExecuteCommand(command, lines);

    wxString fileName;
    for(size_t i1 = 0; i1 < lines.GetCount(); i1++) {
        wxChar stat = lines.Item(i1).GetChar(0);

        if('A' == stat || 'D' == stat) {
            fileName = lines.Item(i1).Mid(8);

            if(!wxFileName::DirExists(fileName)) { aryFiles.push_back(fileName); }
        }
    }

    wxLog::EnableLogging(true);
    return aryFiles;
}

std::vector<wxString> Subversion2::GetFilesMarkedBinary(const wxString& wd)
{
    wxString command;

    command << GetSvnExeName() << wxT(" propget svn:mime-type -R ");
    command << wxT("\"") << wd << wxT("\"");

    wxLog::EnableLogging(false);

    std::vector<wxString> aryFiles;
    wxArrayString lines;

    ProcUtils::ExecuteCommand(command, lines);

    wxString fileName;
    for(size_t i1 = 0; i1 < lines.GetCount(); i1++) {
        lines.Item(i1).Trim(); // gets rid of \r\n, \n, etc.
        if(lines.Item(i1).EndsWith(_(" - application/octet-stream"), &fileName)) { aryFiles.push_back(fileName); }
    }

    wxLog::EnableLogging(true);
    return aryFiles;
}

std::vector<wxString> Subversion2::RemoveExcludeExts(const std::vector<wxString>& aryInFiles,
                                                     const wxString& excludeExtensions)
{
    std::vector<wxString> aryOutFiles;

    wxStringTokenizer tok(excludeExtensions, wxT(" ;"));
    std::set<wxString> specMap;
    while(tok.HasMoreTokens()) {
        wxString v = tok.GetNextToken();

        if(v == wxT("*.*")) {
            // Just ignore the request to not add any files
            continue;
        }
        v = v.AfterLast(wxT('*'));
        v = v.AfterLast(wxT('.')).MakeLower();
        specMap.insert(v);
    }

    for(size_t i1 = 0; i1 < aryInFiles.size(); i1++) {
        if(specMap.empty()) {
            aryOutFiles.push_back(aryInFiles[i1]);
            continue;
        }

        wxFileName fn(aryInFiles[i1]);
        if(specMap.find(fn.GetExt().MakeLower()) == specMap.end()) { aryOutFiles.push_back(aryInFiles[i1]); }
    }

    return aryOutFiles;
}

void Subversion2::OnSync(wxCommandEvent& event)
{
    if(!m_mgr->GetWorkspace() || !m_mgr->IsWorkspaceOpen()) { return; }

    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
    if(item.m_itemType != ProjectItem::TypeProject) {
        return; // a project must be selected
    }

    // retrieve complete list of source files of the workspace
    wxString project_name(item.m_text);
    wxString err_msg;

    ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(project_name, err_msg);
    if(!proj) { return; }

    wxString rawData = proj->GetPluginData("subversion2");

    wxArrayString options = wxStringTokenize(rawData, "\n");
    bool excludeBinary = true;
    wxString rootDir;
    wxString excludeExtensions;
    if(options.GetCount() >= 1) {
        if(options.Item(0) == _("false")) { excludeBinary = false; }
    }
    if(options.GetCount() >= 2) { rootDir = options.Item(1); }
    if(options.GetCount() >= 3) {
        excludeExtensions = options.Item(2);
    } else {
        excludeExtensions << "*.dll *.so *.o *.obj *.workspace *.project *.exe *.dylib";
    }

    SvnSyncDialog dlg(GetManager()->GetTheApp()->GetTopWindow(), this, rootDir, excludeBinary, excludeExtensions);
    if(dlg.ShowModal() != wxID_OK) { return; }
    excludeExtensions = dlg.GetExcludeExtensions();
    excludeBinary = dlg.GetExcludeBin();

    wxLogMessage("excludeBinary=%d\n", excludeBinary);

    // attempt to update the project files
    wxString workDir(dlg.GetRootDir());
    NormalizeDir(workDir);

    wxString command;
    command << GetSvnExeName() << wxT(" list -R ");
    command << wxT("\"") << workDir << wxT("\"");

    // Calls FinishSyncProcess()
    // Get password/authentication, if required
    GetConsole()->Execute(
        command, workDir,
        new SvnRepoListHandler(this, proj, workDir, excludeBinary, excludeExtensions, wxNOT_FOUND, NULL));
}

void Subversion2::FinishSyncProcess(ProjectPtr& proj, const wxString& workDir, bool excludeBin,
                                    const wxString& excludeExtensions, const wxString& output)
{
    // Convert output of "svn list" into a list of files
    // Note that svn list always uses '/' as path delimiter
    std::vector<wxString> aryRepoList;
    {
        wxArrayString repoListOutput = wxStringTokenize(output, wxT("\r\n"));
        wxFileName fn;
        for(size_t i1 = 0; i1 < repoListOutput.GetCount(); i1++) {
            if(repoListOutput.Item(i1).Last() != '/') {
                fn.Assign(workDir + wxFileName::GetPathSeparator() + repoListOutput.Item(i1));
                aryRepoList.push_back(fn.GetFullPath());
            }
        }
    }
    std::sort(aryRepoList.begin(), aryRepoList.end());

    std::vector<wxString> aryNoBins;
    if(excludeBin) {
        std::vector<wxString> aryBinaries = GetFilesMarkedBinary(workDir);
        std::sort(aryBinaries.begin(), aryBinaries.end());

        std::set_symmetric_difference(aryRepoList.begin(), aryRepoList.end(), aryBinaries.begin(), aryBinaries.end(),
                                      std::back_inserter(aryNoBins));
    }
    std::vector<wxString>& aryMaybeNoBins = excludeBin ? aryNoBins : aryRepoList;

    // get local added or deleted files; then add or del from list
    std::vector<wxString> aryUnfiltered;
    {
        std::vector<wxString> aryAddsDels = GetLocalAddsDels(workDir);
        std::sort(aryAddsDels.begin(), aryAddsDels.end());

        std::set_symmetric_difference(aryMaybeNoBins.begin(), aryMaybeNoBins.end(), aryAddsDels.begin(),
                                      aryAddsDels.end(), std::back_inserter(aryUnfiltered));
    }
    std::vector<wxString> aryFinal = RemoveExcludeExts(aryUnfiltered, excludeExtensions);

    m_skipRemoveFilesDlg = true;
    m_mgr->RedefineProjFiles(proj, workDir, aryFinal);

    // refresh project info
    wxString err_msg;
    ProjectPtr projRefreshed = m_mgr->GetWorkspace()->FindProjectByName(proj->GetName(), err_msg);

    if(projRefreshed) {
        wxChar delim = '\n';
        wxString excludeBinTF;
        if(excludeBin) {
            excludeBinTF = _("true");
        } else {
            excludeBinTF = _("false");
        }
        wxString rawData = excludeBinTF + delim + workDir + delim + excludeExtensions;
        wxLogMessage("rawData=%s\n", rawData.c_str());
        projRefreshed->SetPluginData("subversion2", rawData);
    }
}

wxString Subversion2::GetSvnExeNameNoConfigDir()
{
    SvnSettingsData ssd = GetSettings();
    wxString executeable;
    wxString exeName = ssd.GetExecutable();
    exeName.Trim().Trim(false);
    bool encloseQuotations = (exeName.Find(wxT(" ")) != wxNOT_FOUND);
    if(encloseQuotations) {
        executeable << wxT("\"") << ssd.GetExecutable() << wxT("\" ");
    } else {
        executeable << ssd.GetExecutable() << wxT(" ");
    }
    return executeable;
}

void Subversion2::OnRevertToRevision(wxCommandEvent& event)
{
    wxString command;
    wxString loginString;

    wxString revision = wxGetTextFromUser(_("Set the revision number:"), _("Revert to revision"));
    if(revision.IsEmpty()) {
        // user canceled
        return;
    }

    long nRevision;
    if(!revision.ToCLong(&nRevision)) {
        ::wxMessageBox(_("Invalid revision number"), "codelite", wxOK | wxICON_ERROR | wxCENTER);
        return;
    }

    wxFileName workingDirectory(m_selectedFolder, "");
    if(m_selectedFile.IsOk()) {
        command << GetSvnExeName() << loginString << " merge -r HEAD:" << nRevision << " "
                << m_selectedFile.GetFullName();
        GetConsole()->Execute(command, workingDirectory.GetPath(),
                              new SvnDefaultCommandHandler(this, event.GetId(), this));
    } else {
        wxString folderName = workingDirectory.GetDirs().Last();
        workingDirectory.RemoveLastDir();
        ::WrapWithQuotes(folderName);

        command << GetSvnExeName() << loginString << " merge -r HEAD:" << nRevision << " " << folderName;
        GetConsole()->Execute(command, workingDirectory.GetPath(),
                              new SvnDefaultCommandHandler(this, event.GetId(), this));
    }
}

void Subversion2::DoGetSvnClientVersion()
{
    static wxRegEx reSvnClient("svn, version ([0-9]+)\\.([0-9]+)\\.([0-9]+)");
    wxString svnVersionCommand;
    svnVersionCommand << GetSvnExeName() << " --version";

#ifndef __WXMSW__
    // Hide stderr
    svnVersionCommand << " 2> /dev/null";
#endif

    wxString versionOutput = ProcUtils::SafeExecuteCommand(svnVersionCommand);
    if(versionOutput.IsEmpty()) return;

    versionOutput = versionOutput.BeforeFirst('\n');
    if(reSvnClient.IsValid() && reSvnClient.Matches(versionOutput)) {
        long major, minor, patch;
        wxString sMajor = reSvnClient.GetMatch(versionOutput, 1);
        wxString sMinor = reSvnClient.GetMatch(versionOutput, 2);
        wxString sPatch = reSvnClient.GetMatch(versionOutput, 3);

        sMajor.ToCLong(&major);
        sMinor.ToCLong(&minor);
        sPatch.ToCLong(&patch);

        m_clientVersion = major * 1000 + minor * 100 + patch;

        GetConsole()->AppendText(wxString() << "-- Svn client version: " << m_clientVersion << "\n");
        GetConsole()->AppendText(wxString() << "-- " << versionOutput << "\n");
    }
}

void Subversion2::AddCommandLineOption(wxString& command, Subversion2::eCommandLineOption opt)
{
    switch(opt) {
    case kOpt_ForceInteractive:
        if(m_clientVersion >= 1800) { command << " --force-interactive "; }
        break;
    }
}

void Subversion2::OnFolderContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    m_selectedFolder = event.GetPath();
    m_selectedFile.Clear();
    wxMenuItem* item =
        new wxMenuItem(event.GetMenu(), wxID_ANY, "Svn", "", wxITEM_NORMAL, CreateFileExplorerPopMenu(false));
    item->SetBitmap(m_svnBitmap);
    event.GetMenu()->Append(item);
}

void Subversion2::OnFileContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    if(event.GetStrings().size() == 1) {
        m_selectedFile = event.GetStrings().Item(0);
        m_selectedFolder = wxFileName(m_selectedFile).GetPath();

        wxMenuItem* item =
            new wxMenuItem(event.GetMenu(), wxID_ANY, "Svn", "", wxITEM_NORMAL, CreateFileExplorerPopMenu(true));
        item->SetBitmap(m_svnBitmap);
        event.GetMenu()->Append(item);
    }
}

void Subversion2::DoFilesDeleted(const wxArrayString& files, bool isFolder)
{
    if(!files.IsEmpty()) {

        // test the first file, see if it is under SVN
        wxFileName fn = isFolder ? wxFileName(files.Item(0), "") : wxFileName(files.Item(0));
        if(IsPathUnderSvn(fn.GetPath())) {
            // Build the message:

            // Limit the message to maximum of 10 files
            wxString filesString;
            wxString msg;
            if(isFolder) {
                msg << _("Would you like to remove the following folders from SVN?\n\n");
            } else {
                msg << _("Would you like to remove the following files from SVN?\n\n");
            }

            size_t fileCount = files.GetCount();
            for(size_t i = 0; i < files.GetCount(); i++) {
                if(i < 10) {
                    msg << files.Item(i) << wxT("\n");
                    filesString << wxT("\"") << files.Item(i) << wxT("\" ");
                    --fileCount;
                } else {
                    break;
                }
            }

            if(fileCount) {
                if(isFolder) {
                    msg << ".. and " << fileCount << " more folders";
                } else {
                    msg << ".. and " << fileCount << " more files";
                }
            }

            if(wxMessageBox(msg, wxT("Subversion"), wxYES_NO | wxCANCEL | wxCENTER | wxNO_DEFAULT,
                            GetManager()->GetTheApp()->GetTopWindow()) == wxYES) {

                wxString command;
                RecreateLocalSvnConfigFile();
                command << GetSvnExeName() << wxT(" delete --force ") << filesString;
                GetConsole()->Execute(command, m_subversionView->GetRootDir(),
                                      new SvnDefaultCommandHandler(this, wxNOT_FOUND, this));
            }
        }
    }
}

void Subversion2::OnFileDeleted(clFileSystemEvent& event)
{
    event.Skip();
    DoFilesDeleted(event.GetPaths());
}

void Subversion2::OnFolderDeleted(clFileSystemEvent& event)
{
    event.Skip();
    DoFilesDeleted(event.GetPaths(), true);
}

void Subversion2::OnShowFileChanges(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ShowRecentChanges(m_selectedFile.GetFullPath());
}

void Subversion2::ShowRecentChanges(const wxString& file)
{
    if(!wxFileName::FileExists(file)) return;
    wxString filename(file);
    ::WrapWithQuotes(filename);
    long numberOfChanges = wxGetNumberFromUser(_("How many recent changes you want to view?"), "",
                                               _("Svn show recent changes"), 1, 1, 100);
    if(numberOfChanges == wxNOT_FOUND) return; // cancel
    // Build the command

    wxString command;
    command << GetSvnExeNameNoConfigDir() << " log --diff -l " << numberOfChanges << " " << filename;
    GetConsole()->Execute(command, m_subversionView->GetRootDir(),
                          new SvnShowFileChangesHandler(this, wxNOT_FOUND, this));
}

void Subversion2::ShowRecentChangesDialog(const SvnShowDiffChunk::List_t& changes)
{
    if(changes.empty()) return;
    SvnShowRecentChangesDlg dlg(EventNotifier::Get()->TopFrame(), changes);
    dlg.ShowModal();
}

void Subversion2::OnGotoAnythingShowing(clGotoEvent& e)
{
    e.Skip();
    // Add our entries 
    e.GetEntries().push_back(clGotoEntry("Svn > Commit", "", XRCID("gotoanything_svn_commit")));
    e.GetEntries().push_back(clGotoEntry("Svn > Update", "", XRCID("gotoanything_svn_update")));
}
