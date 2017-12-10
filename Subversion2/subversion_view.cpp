//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : subversion_view.cpp
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

#include "DiffSideBySidePanel.h"
#include "SvnInfoDialog.h"
#include "bitmap_loader.h"
#include "cl_command_event.h"
#include "clcommandlineparser.h"
#include "diff_dialog.h"
#include "dirsaver.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "globals.h"
#include "imanager.h"
#include "plugin.h"
#include "procutils.h"
#include "subversion2.h"
#include "subversion2_ui.h"
#include "subversion_strings.h"
#include "subversion_view.h"
#include "svn_checkout_dialog.h"
#include "svn_command_handlers.h"
#include "svn_console.h"
#include "svn_copy_dialog.h"
#include "svn_default_command_handler.h"
#include "svn_local_properties.h"
#include "svn_login_dialog.h"
#include "svn_overlay_tool.h"
#include "svn_props_dialog.h"
#include "svn_select_local_repo_dlg.h"
#include "svnsettingsdata.h"
#include "svnstatushandler.h"
#include "svntreedata.h"
#include "workspace.h"
#include "workspacesvnsettings.h"
#include "wx_tree_traverser.h"
#include <map>
#include <wx/app.h>
#include <wx/aui/auibar.h>
#include <wx/cmdline.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/imaglist.h>
#include <wx/menu.h>
#include <wx/settings.h>
#include <wx/textdlg.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>
#include "clDiffFrame.h"

BEGIN_EVENT_TABLE(SubversionView, SubversionPageBase)
EVT_UPDATE_UI(XRCID("svn_stop"), SubversionView::OnStopUI)
EVT_UPDATE_UI(XRCID("clear_svn_output"), SubversionView::OnClearOuptutUI)

EVT_MENU(XRCID("svn_link_editor"), SubversionView::OnLinkEditor)
EVT_MENU(XRCID("svn_revert"), SubversionView::OnRevert)
EVT_MENU(XRCID("svn_tag"), SubversionView::OnTag)
EVT_MENU(XRCID("svn_branch"), SubversionView::OnBranch)
EVT_MENU(XRCID("svn_diff"), SubversionView::OnDiff)
EVT_MENU(XRCID("svn_patch"), SubversionView::OnPatch)
EVT_MENU(XRCID("svn_patch_dry_run"), SubversionView::OnPatchDryRun)
EVT_MENU(XRCID("svn_resolve"), SubversionView::OnResolve)
EVT_MENU(XRCID("svn_add"), SubversionView::OnAdd)
EVT_MENU(XRCID("svn_delete"), SubversionView::OnDelete)
EVT_MENU(XRCID("svn_ignore_file"), SubversionView::OnIgnoreFile)
EVT_MENU(XRCID("svn_ignore_file_pattern"), SubversionView::OnIgnoreFilePattern)
EVT_MENU(XRCID("svn_blame"), SubversionView::OnBlame)
EVT_MENU(XRCID("svn_checkout"), SubversionView::OnCheckout)
EVT_MENU(XRCID("svn_open_file"), SubversionView::OnOpenFile)
EVT_MENU(XRCID("svn_switch"), SubversionView::OnSwitch)
EVT_MENU(XRCID("svn_properties"), SubversionView::OnProperties)
EVT_MENU(XRCID("svn_log"), SubversionView::OnLog)
EVT_MENU(XRCID("svn_lock"), SubversionView::OnLock)
EVT_MENU(XRCID("svn_unlock"), SubversionView::OnUnLock)
EVT_MENU(XRCID("svn_rename"), SubversionView::OnRename)
EVT_MENU(XRCID("svn_open_local_repo_browser"), SubversionView::OnChangeRootDir)
EVT_MENU(XRCID("svn_close_view"), SubversionView::OnCloseView)

END_EVENT_TABLE()

static int FOLDER_IMG_ID = wxNOT_FOUND;
static int MODIFIED_IMG_ID = wxNOT_FOUND;
static int NEW_IMG_ID = wxNOT_FOUND;
static int DELETED_IMG_ID = wxNOT_FOUND;
static int CONFLICT_IMG_ID = wxNOT_FOUND;
static int UNVERSIONED_IMG_ID = wxNOT_FOUND;
static int PROJECT_IMG_ID = wxNOT_FOUND;
static int WORKSPACE_IMG_ID = wxNOT_FOUND;
static int LOCKED_IMG_ID = wxNOT_FOUND;

/**
 * @class DiffCmdHandler
 * Handle diff "codelite-echo" command output
 * Once we have our 3 lines output, we can "finalize" the diff
 */
class DiffCmdHandler : public IProcessCallback
{
    SubversionView* m_view;
    wxString m_output;
    wxFileName m_filename; // the file which we want to diff

public:
    DiffCmdHandler(SubversionView* view, const wxFileName& filename)
        : m_view(view)
        , m_filename(filename)
    {
    }
    ~DiffCmdHandler() {}

    virtual void OnProcessOutput(const wxString& str)
    {
        m_output << str;
        wxArrayString lines = ::wxStringTokenize(m_output, "\n", wxTOKEN_STRTOK);
        if(lines.GetCount() == 3) {
            // we got all the info we need
            m_view->FinishDiff(lines.Item(2).Trim(), m_filename);
        }
    }
    /**
     * @brief the process has terminated, delete the instance and
     * ourself
     */
    virtual void OnProcessTerminated() { delete this; }

    const wxFileName& GetFilename() const { return m_filename; }
};

SubversionView::SubversionView(wxWindow* parent, Subversion2* plugin)
    : SubversionPageBase(parent)
    , m_plugin(plugin)
    , m_simpleCommand(plugin)
    , m_diffCommand(plugin)
    , m_fileExplorerLastBaseImgIdx(-1)
    , m_codeliteEcho(NULL)
{
    CreatGUIControls();
    m_themeHelper = new ThemeHandlerHelper(this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SubversionView::OnWorkspaceLoaded),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(SubversionView::OnWorkspaceClosed),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVED, clCommandEventHandler(SubversionView::OnFileSaved), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PROJ_FILE_ADDED, clCommandEventHandler(SubversionView::OnFileAdded), NULL,
                                  this);
    EventNotifier::Get()->Bind(wxEVT_FILE_RENAMED, &SubversionView::OnFileRenamed, this);
    EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED,
                                  wxCommandEventHandler(SubversionView::OnActiveEditorChanged), NULL, this);

    ::clRecalculateSTCHScrollBar(m_sci);

    wxTheApp->Bind(wxEVT_MENU, &SubversionView::OnCommit, this, XRCID("svn_commit"));
    wxTheApp->Bind(wxEVT_MENU, &SubversionView::OnUpdate, this, XRCID("svn_update"));
    wxTheApp->Bind(wxEVT_MENU, &SubversionView::OnCommitGotoAnything, this, XRCID("gotoanything_svn_commit"));
    wxTheApp->Bind(wxEVT_MENU, &SubversionView::OnUpdateGotoAnything, this, XRCID("gotoanything_svn_update"));
}

SubversionView::~SubversionView()
{
    wxDELETE(m_themeHelper);
    DisconnectEvents();
}

void SubversionView::OnChangeRootDir(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString newPath = ::wxDirSelector(_("Choose directory"));
    if(!newPath.IsEmpty()) {
        DoRootDirChanged(newPath);
    }
}

void SubversionView::OnTreeMenu(wxTreeEvent& event)
{
    // Popup the menu
    wxArrayTreeItemIds items;
    size_t count = m_treeCtrl->GetSelections(items);
    if(count) {
        SvnTreeData::SvnNodeType type = DoGetSelectionType(items);
        if(type == SvnTreeData::SvnNodeTypeInvalid)
            // Mix or an invalid selection
            return;

        wxMenu menu;
        switch(type) {
        case SvnTreeData::SvnNodeTypeFile:
            CreateFileMenu(&menu);
            break;

        case SvnTreeData::SvnNodeTypeRoot:
            CreateRootMenu(&menu);
            break;

        case SvnTreeData::SvnNodeTypeAddedRoot:
        case SvnTreeData::SvnNodeTypeDeletedRoot:
        case SvnTreeData::SvnNodeTypeModifiedRoot:
        case SvnTreeData::SvnNodeTypeFolder:
            CreateSecondRootMenu(&menu);
            break;

        default:
            return;
        }

        PopupMenu(&menu);
    }
}

void SubversionView::CreatGUIControls()
{
    MSWSetNativeTheme(m_treeCtrl);
    m_treeCtrl->SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

    // Assign the image list
    BitmapLoader* bmpLoader = m_plugin->GetManager()->GetStdIcons();

    // Prepare a default image list which contains all the mimetypes knows to FileExtManager
    wxImageList* imageList = bmpLoader->MakeStandardMimeImageList();

    // Append the subversion unique icons
    FOLDER_IMG_ID = imageList->Add(bmpLoader->LoadBitmap(wxT("folder-yellow")));
    MODIFIED_IMG_ID = imageList->Add(bmpLoader->LoadBitmap(wxT("error")));
    NEW_IMG_ID = imageList->Add(bmpLoader->LoadBitmap(wxT("plus")));
    DELETED_IMG_ID = imageList->Add(bmpLoader->LoadBitmap(wxT("minus")));
    CONFLICT_IMG_ID = imageList->Add(bmpLoader->LoadBitmap(wxT("warning")));
    UNVERSIONED_IMG_ID = imageList->Add(bmpLoader->LoadBitmap(wxT("info")));
    PROJECT_IMG_ID = imageList->Add(bmpLoader->LoadBitmap(wxT("project")));
    WORKSPACE_IMG_ID = imageList->Add(bmpLoader->LoadBitmap(wxT("php-workspace")));
    LOCKED_IMG_ID = imageList->Add(bmpLoader->LoadBitmap(wxT("lock")));

    m_treeCtrl->AssignImageList(imageList);

    // Add toolbar
    // Create the toolbar
    BitmapLoader* bmpLdr = m_plugin->GetManager()->GetStdIcons();
    wxAuiToolBar* tb = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND);
    tb->AddTool(XRCID("svn_link_editor"), _("Link Editor"), bmpLdr->LoadBitmap(wxT("link_editor")), _("Link Editor"),
                wxITEM_CHECK);
    tb->ToggleTool(XRCID("svn_link_editor"), m_plugin->GetSettings().GetFlags() & SvnLinkEditor);
    tb->AddTool(XRCID("svn_open_local_repo_browser"), _("Select a Directory to View..."), bmpLdr->LoadBitmap("folder"),
                _("Select a Directory to View..."), wxITEM_NORMAL);
    tb->AddSeparator();

    tb->AddTool(XRCID("svn_stop"), _("Stop current svn process"), bmpLdr->LoadBitmap("stop"),
                _("Stop current svn process"));
    tb->AddTool(XRCID("svn_cleanup"), _("Svn Cleanup"), bmpLdr->LoadBitmap("clean"), _("Svn Cleanup"));
    tb->AddSeparator();
    tb->AddTool(XRCID("svn_checkout"), _("Svn Checkout"), bmpLdr->LoadBitmap("next"), _("Svn Checkout"));
    tb->AddSeparator();
    tb->AddTool(XRCID("svn_refresh"), _("Refresh View"), bmpLdr->LoadBitmap("debugger_restart"), _("Refresh View"));
    tb->AddSeparator();
    tb->AddTool(XRCID("clear_svn_output"), _("Clear Svn Output Tab"), bmpLdr->LoadBitmap("clear"),
                _("Clear Svn Output Tab"), wxITEM_NORMAL);
    tb->AddTool(XRCID("svn_settings"), _("Svn Settings..."), bmpLdr->LoadBitmap("cog"), _("Svn Settings..."));
    tb->AddTool(XRCID("svn_info"), _("Svn Info"), bmpLdr->LoadBitmap("info"), _("Svn Info"));

    tb->Connect(XRCID("clear_svn_output"), wxEVT_COMMAND_MENU_SELECTED,
                wxCommandEventHandler(SubversionView::OnClearOuptut), NULL, this);
    tb->Connect(XRCID("svn_stop"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionView::OnStop), NULL,
                this);
    tb->Connect(XRCID("svn_cleanup"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionView::OnCleanup),
                NULL, this);
    tb->Connect(XRCID("svn_info"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionView::OnShowSvnInfo),
                NULL, this);
    tb->Connect(XRCID("svn_refresh"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionView::OnRefreshView),
                NULL, this);
    tb->Connect(XRCID("svn_settings"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionView::OnSettings),
                NULL, this);

    wxSizer* sz = GetSizer();
    sz->Insert(0, tb, 0, wxEXPAND);
    tb->Realize();

    m_subversionConsole = new SvnConsole(m_sci, m_plugin);

    DoRootDirChanged(wxGetCwd());
    BuildTree();
}

void SubversionView::BuildTree() { BuildTree(DoGetCurRepoPath()); }

void SubversionView::BuildTree(const wxString& root)
{
    if(root.IsEmpty()) return;

    DoChangeRootPathUI(root);

    wxString command;
    command << m_plugin->GetSvnExeName() << wxT(" status");
    m_simpleCommand.Execute(command, root, new SvnStatusHandler(m_plugin, wxNOT_FOUND, NULL), m_plugin);
}

void SubversionView::BuildExplorerTree(const wxString& root)
{
    if(root.IsEmpty()) return;

    wxString command;
    command << m_plugin->GetSvnExeName() << wxT(" status");
    m_simpleCommand.Execute(command, root, new SvnStatusHandler(m_plugin, wxNOT_FOUND, NULL, true, root), m_plugin);
}

void SubversionView::OnWorkspaceLoaded(wxCommandEvent& event)
{
    event.Skip();

    // Workspace changes its directory to the workspace path, update the SVN path
    wxString path = ::wxGetCwd();
    m_workspaceFile = event.GetString();

    WorkspaceSvnSettings conf(m_workspaceFile);
    wxString customizedRepo = conf.Load().GetRepoPath();
    if(!customizedRepo.IsEmpty()) {
        path.swap(customizedRepo);
    }

    if(!m_plugin->IsPathUnderSvn(path)) {
        DoCloseView();

    } else {
        DoRootDirChanged(path);
        BuildTree();
    }
}

void SubversionView::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();

    // Save the local svn settings
    if(m_workspaceFile.IsOk() && m_workspaceFile.Exists()) {
        WorkspaceSvnSettings conf(m_workspaceFile);
        conf.SetRepoPath(m_curpath);
        conf.Save();
    }

    m_workspaceFile.Clear();
    DoChangeRootPathUI(wxEmptyString);
    m_plugin->GetConsole()->Clear();
}

void SubversionView::ClearAll() { m_treeCtrl->DeleteAllItems(); }

void SubversionView::UpdateTree(const wxArrayString& modifiedFiles, const wxArrayString& conflictedFiles,
                                const wxArrayString& unversionedFiles, const wxArrayString& newFiles,
                                const wxArrayString& deletedFiles, const wxArrayString& lockedFiles,
                                const wxArrayString& ignoreFiles, bool fileExplorerOnly, const wxString& sRootDir)
{
    wxString rootDir = sRootDir;
    if(rootDir.IsEmpty()) rootDir = DoGetCurRepoPath();

    if(!fileExplorerOnly) {
        wxWindowUpdateLocker locker(m_treeCtrl);
        ClearAll();

        // Add root node
        wxTreeItemId root = m_treeCtrl->AddRoot(rootDir, FOLDER_IMG_ID, FOLDER_IMG_ID,
                                                new SvnTreeData(SvnTreeData::SvnNodeTypeRoot, rootDir));

        if(root.IsOk() == false) return;

        DoAddNode(svnMODIFIED_FILES, MODIFIED_IMG_ID, SvnTreeData::SvnNodeTypeModifiedRoot, modifiedFiles);
        DoAddNode(svnADDED_FILES, NEW_IMG_ID, SvnTreeData::SvnNodeTypeAddedRoot, newFiles);
        DoAddNode(svnDELETED_FILES, DELETED_IMG_ID, SvnTreeData::SvnNodeTypeDeletedRoot, deletedFiles);
        DoAddNode(svnCONFLICTED_FILES, CONFLICT_IMG_ID, SvnTreeData::SvnNodeTypeConflictRoot, conflictedFiles);
        DoAddNode(svnLOCKED_FILES, LOCKED_IMG_ID, SvnTreeData::SvnNodeTypeLockedRoot, lockedFiles);
        DoAddNode(svnUNVERSIONED_FILES, UNVERSIONED_IMG_ID, SvnTreeData::SvnNodeTypeUnversionedRoot, unversionedFiles);

        if(m_treeCtrl->ItemHasChildren(root)) {
            m_treeCtrl->Expand(root);
        }

        DoLinkEditor();
    }
}

void SubversionView::DoAddNode(const wxString& title, int imgId, SvnTreeData::SvnNodeType nodeType,
                               const wxArrayString& files)
{
    wxTreeItemId root = m_treeCtrl->GetRootItem();
    wxString basePath = DoGetCurRepoPath();

    // Add the basic four root items
    if(files.IsEmpty() == false) {

        wxTreeItemId parent = m_treeCtrl->AppendItem(root, title, imgId, imgId, new SvnTreeData(nodeType, wxT("")));

        // Set the parent node with bold font
        wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        font.SetWeight(wxFONTWEIGHT_BOLD);
        m_treeCtrl->SetItemFont(parent, font);

        // Add all children items
        for(size_t i = 0; i < files.GetCount(); i++) {
            wxFileName filename(files.Item(i));
            wxTreeItemId folderParent = DoGetParentNode(files.Item(i), parent);
            m_treeCtrl->AppendItem(folderParent, filename.GetFullName(), DoGetIconIndex(filename.GetFullName()),
                                   DoGetIconIndex(filename.GetFullName()),
                                   new SvnTreeData(SvnTreeData::SvnNodeTypeFile, files.Item(i)));
        }

        if(nodeType != SvnTreeData::SvnNodeTypeUnversionedRoot) {
            m_treeCtrl->Expand(parent);

            // Expand the top level children as well
            wxTreeItemIdValue cookie;
            wxTreeItemId child = m_treeCtrl->GetFirstChild(parent, cookie);
            while(child.IsOk()) {
                if(m_treeCtrl->ItemHasChildren(child)) {
                    m_treeCtrl->Expand(child);
                }
                child = m_treeCtrl->GetNextChild(parent, cookie);
            }
        }
    }
}

int SubversionView::DoGetIconIndex(const wxString& filename)
{
    FileExtManager::Init();
    int iconIndex = m_plugin->GetManager()->GetStdIcons()->GetMimeImageId(filename);
    if(iconIndex == wxNOT_FOUND)
        iconIndex =
            m_plugin->GetManager()->GetStdIcons()->GetMimeImageId(wxT("file.txt")); // text file icon is the default

    return iconIndex;
}

SvnTreeData::SvnNodeType SubversionView::DoGetSelectionType(const wxArrayTreeItemIds& items)
{
    m_selectionInfo.Clear();
    SvnTreeData::SvnNodeType type(SvnTreeData::SvnNodeTypeInvalid);
    for(size_t i = 0; i < items.GetCount(); i++) {
        if(items.Item(i).IsOk() == false) {
            m_selectionInfo.Clear();
            return m_selectionInfo.m_selectionType; // Invalid
        }

        SvnTreeData* data = (SvnTreeData*)m_treeCtrl->GetItemData(items.Item(i));
        if(!data) {
            m_selectionInfo.Clear();
            return m_selectionInfo.m_selectionType; // Invalid
        }

        if(data->GetType() == SvnTreeData::SvnNodeTypeRoot && items.GetCount() == 1) {
            // populate the list of paths with all the added paths
            DoGetPaths(items.Item(i), m_selectionInfo.m_paths);
            m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeRoot;
            return m_selectionInfo.m_selectionType;
        }

        if(data->GetType() == SvnTreeData::SvnNodeTypeAddedRoot && items.GetCount() == 1) {
            // populate the list of paths with all the added paths
            DoGetPaths(items.Item(i), m_selectionInfo.m_paths);
            m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeAddedRoot;
            return m_selectionInfo.m_selectionType;
        }

        if(data->GetType() == SvnTreeData::SvnNodeTypeDeletedRoot && items.GetCount() == 1) {
            // populate the list of paths with all the deleted paths
            DoGetPaths(items.Item(i), m_selectionInfo.m_paths);
            m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeDeletedRoot;
            return m_selectionInfo.m_selectionType;
        }

        if(data->GetType() == SvnTreeData::SvnNodeTypeConflictRoot && items.GetCount() == 1) {
            // populate the list of paths with all the conflicted paths
            DoGetPaths(items.Item(i), m_selectionInfo.m_paths);
            m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeConflictRoot;
            return m_selectionInfo.m_selectionType;
        }

        if(data->GetType() == SvnTreeData::SvnNodeTypeModifiedRoot && items.GetCount() == 1) {
            // populate the list of paths with all the conflicted paths
            DoGetPaths(items.Item(i), m_selectionInfo.m_paths);
            m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeModifiedRoot;
            return m_selectionInfo.m_selectionType;
        }

        if(data->GetType() == SvnTreeData::SvnNodeTypeFolder && items.GetCount() == 1) {
            // populate the list of paths with all the conflicted paths
            DoGetPaths(items.Item(i), m_selectionInfo.m_paths);
            m_selectionInfo.m_selectionType = SvnTreeData::SvnNodeTypeFolder;
            return m_selectionInfo.m_selectionType;
        }

        if(type == SvnTreeData::SvnNodeTypeInvalid &&
           (data->GetType() == SvnTreeData::SvnNodeTypeFile || data->GetType() == SvnTreeData::SvnNodeTypeRoot)) {
            type = data->GetType();
            m_selectionInfo.m_selectionType = type;
            m_selectionInfo.m_paths.Add(data->GetFilepath());

        } else if(type == SvnTreeData::SvnNodeTypeInvalid) {
            type = data->GetType();

        } else if(data->GetType() != type) {
            m_selectionInfo.m_paths.Clear();
            return SvnTreeData::SvnNodeTypeInvalid;

        } else {
            // Same type, just add the path
            m_selectionInfo.m_paths.Add(data->GetFilepath());
        }
    }
    return type;
}

void SubversionView::CreateSecondRootMenu(wxMenu* menu)
{
    menu->Append(XRCID("svn_commit"), wxT("Commit"));
    menu->Append(XRCID("svn_update"), wxT("Update"));
    menu->AppendSeparator();
    menu->Append(XRCID("svn_revert"), wxT("Revert"));
    menu->Append(XRCID("svn_add"), wxT("Add"));
    menu->AppendSeparator();
    menu->Append(XRCID("svn_diff"), _("Create Diff..."));
}

void SubversionView::CreateFileMenu(wxMenu* menu)
{
    menu->Append(XRCID("svn_open_file"), _("Open File..."));
    menu->AppendSeparator();
    menu->Append(XRCID("svn_update"), wxT("Update"));
    menu->Append(XRCID("svn_commit"), wxT("Commit"));
    menu->AppendSeparator();
    menu->Append(XRCID("svn_revert"), wxT("Revert"));

    menu->AppendSeparator();
    menu->Append(XRCID("svn_lock"), wxT("Lock"));
    menu->Append(XRCID("svn_unlock"), wxT("Unlock"));

    menu->AppendSeparator();
    menu->Append(XRCID("svn_add"), wxT("Add"));
    menu->Append(XRCID("svn_delete"), wxT("Delete"));
    menu->Append(XRCID("svn_rename"), wxT("Rename"));
    menu->AppendSeparator();
    menu->Append(XRCID("svn_resolve"), wxT("Resolve"));
    menu->AppendSeparator();
    menu->Append(XRCID("svn_diff"), _("Create Diff..."));
    menu->AppendSeparator();

    menu->Append(XRCID("svn_blame"), _("Blame..."));
    menu->AppendSeparator();

    wxMenu* subMenu;
    subMenu = new wxMenu;
    subMenu->Append(XRCID("svn_ignore_file"), _("Ignore this file"));
    subMenu->Append(XRCID("svn_ignore_file_pattern"), _("Ignore this file pattern"));
    menu->Append(wxID_ANY, wxT("Ignore"), subMenu);
}

void SubversionView::CreateRootMenu(wxMenu* menu)
{
    menu->Append(XRCID("svn_update"), wxT("Update"));
    menu->Append(XRCID("svn_commit"), wxT("Commit"));
    menu->AppendSeparator();

    menu->Append(XRCID("svn_revert"), wxT("Revert"));
    menu->AppendSeparator();

    menu->Append(XRCID("svn_tag"), _("Create Tag"));
    menu->Append(XRCID("svn_branch"), _("Create Branch"));
    menu->AppendSeparator();

    menu->Append(XRCID("svn_switch"), _("Switch URL..."));
    menu->AppendSeparator();

    menu->Append(XRCID("svn_diff"), _("Create Diff..."));
    menu->Append(XRCID("svn_patch"), _("Apply Patch..."));
    menu->Append(XRCID("svn_patch_dry_run"), _("Apply Patch - Dry Run..."));

    menu->AppendSeparator();
    menu->Append(XRCID("svn_log"), _("Change Log..."));

    menu->AppendSeparator();
    menu->Append(XRCID("svn_properties"), _("Properties..."));

    menu->AppendSeparator();
    wxMenuItem* menuItem = new wxMenuItem(menu, XRCID("svn_close_view"), _("Close"));
    menuItem->SetBitmap(m_plugin->GetManager()->GetStdIcons()->LoadBitmap("file_close"));
    menu->Append(menuItem);
}

void SubversionView::DoGetPaths(const wxTreeItemId& parent, wxArrayString& paths)
{
    if(m_treeCtrl->ItemHasChildren(parent) == false) {
        return;
    }

    wxTreeItemIdValue cookie;
    wxTreeItemId item = m_treeCtrl->GetFirstChild(parent, cookie);
    while(item.IsOk()) {
        SvnTreeData* data = (SvnTreeData*)m_treeCtrl->GetItemData(item);
        if(data) {

            if(data->GetFilepath().IsEmpty() == false && data->GetType() == SvnTreeData::SvnNodeTypeFile) {
                paths.Add(data->GetFilepath());
            }

            if((data->GetType() == SvnTreeData::SvnNodeTypeAddedRoot ||
                data->GetType() == SvnTreeData::SvnNodeTypeModifiedRoot ||
                data->GetType() == SvnTreeData::SvnNodeTypeDeletedRoot ||
                data->GetType() == SvnTreeData::SvnNodeTypeFolder) &&
               m_treeCtrl->ItemHasChildren(item)) {
                DoGetPaths(item, paths);
            }
        }
        item = m_treeCtrl->GetNextChild(parent, cookie);
    }
}

////////////////////////////////////////////
// Source control command handlers
////////////////////////////////////////////

void SubversionView::OnUpdate(wxCommandEvent& event)
{
    wxString command;
    wxString loginString;
    if(m_plugin->LoginIfNeeded(event, DoGetCurRepoPath(), loginString) == false) {
        return;
    }
    command << m_plugin->GetSvnExeName() << loginString << wxT(" update ");
    m_plugin->AddCommandLineOption(command, Subversion2::kOpt_ForceInteractive);

    if(m_selectionInfo.m_selectionType != SvnTreeData::SvnNodeTypeRoot) {
        // Concatenate list of files to be updated
        for(size_t i = 0; i < m_selectionInfo.m_paths.GetCount(); i++) {
            command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
        }
    }

    m_plugin->GetConsole()->Execute(command, DoGetCurRepoPath(), new SvnUpdateHandler(m_plugin, event.GetId(), this),
                                    true, true);
}

void SubversionView::OnCommit(wxCommandEvent& event)
{
    m_plugin->DoCommit(m_selectionInfo.m_paths, DoGetCurRepoPath(), event);
}

void SubversionView::OnAdd(wxCommandEvent& event)
{
    wxString command;
    wxString loginString;
    if(m_plugin->LoginIfNeeded(event, DoGetCurRepoPath(), loginString) == false) {
        return;
    }

    command << m_plugin->GetSvnExeName() << loginString << wxT(" add ");

    // Concatenate list of files to be updated
    for(size_t i = 0; i < m_selectionInfo.m_paths.GetCount(); i++) {
        command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
    }

    m_plugin->GetConsole()->Execute(command, DoGetCurRepoPath(),
                                    new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));
}

void SubversionView::OnRevert(wxCommandEvent& event)
{
    wxString command;

    if(wxMessageBox(_("You are about to revert all your changes\nAre you sure?"), "CodeLite",
                    wxICON_WARNING | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxCENTER) != wxYES) {
        return;
    }

    // Svn revert does not require login string
    command << m_plugin->GetSvnExeName() << wxT(" revert --recursive ");

    if(m_selectionInfo.m_selectionType != SvnTreeData::SvnNodeTypeRoot) {
        // Concatenate list of files to be updated
        for(size_t i = 0; i < m_selectionInfo.m_paths.GetCount(); i++) {
            command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
        }
    } else {
        command << wxT(".");
    }
    m_plugin->GetConsole()->Execute(command, DoGetCurRepoPath(),
                                    new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));
}

void SubversionView::OnBranch(wxCommandEvent& event)
{
    wxString command;
    command << m_plugin->GetSvnExeName() << wxT("info --xml ");

    SvnInfo svnInfo;
    m_plugin->DoGetSvnInfoSync(svnInfo, DoGetCurRepoPath());

    command.Clear();
    wxString loginString;
    if(m_plugin->LoginIfNeeded(event, DoGetCurRepoPath(), loginString) == false) {
        return;
    }

    // Prompt user for URLs + comment
    SvnCopyDialog dlg(m_plugin->GetManager()->GetTheApp()->GetTopWindow());

    dlg.SetTitle(_("Create Branch"));
    dlg.SetSourceURL(svnInfo.m_sourceUrl);
    dlg.SetTargetURL(svnInfo.m_sourceUrl);

    if(dlg.ShowModal() == wxID_OK) {
        command.Clear();
        // Prepare the 'copy' command
        command << m_plugin->GetSvnExeName() << loginString << wxT(" copy ") << dlg.GetSourceURL() << wxT(" ")
                << dlg.GetTargetURL() << wxT(" -m \"") << dlg.GetMessage() << wxT("\"");

        m_plugin->GetConsole()->Execute(command, DoGetCurRepoPath(),
                                        new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));
    }
}

void SubversionView::OnTag(wxCommandEvent& event)
{
    wxString command;
    command << m_plugin->GetSvnExeName() << wxT("info --xml ");

    SvnInfo svnInfo;
    m_plugin->DoGetSvnInfoSync(svnInfo, DoGetCurRepoPath());

    // Prompt the login dialog now
    command.Clear();
    wxString loginString;
    if(m_plugin->LoginIfNeeded(event, DoGetCurRepoPath(), loginString) == false) {
        return;
    }

    // Prompt user for URLs + comment
    SvnCopyDialog dlg(m_plugin->GetManager()->GetTheApp()->GetTopWindow());

    dlg.SetTitle(_("Create Tag"));
    dlg.SetSourceURL(svnInfo.m_sourceUrl);
    dlg.SetTargetURL(svnInfo.m_sourceUrl);

    if(dlg.ShowModal() == wxID_OK) {
        // Prepare the 'copy' command
        command.Clear();
        command << m_plugin->GetSvnExeName() << loginString << wxT(" copy ") << dlg.GetSourceURL() << wxT(" ")
                << dlg.GetTargetURL() << wxT(" -m \"") << dlg.GetMessage() << wxT("\"");

        m_plugin->GetConsole()->Execute(command, DoGetCurRepoPath(),
                                        new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));
    }
}

void SubversionView::OnDelete(wxCommandEvent& event)
{
    wxString command;
    wxString loginString;
    if(m_plugin->LoginIfNeeded(event, DoGetCurRepoPath(), loginString) == false) {
        return;
    }
    command << m_plugin->GetSvnExeName() << loginString << wxT(" --force delete ");

    if(::wxMessageBox(_("Delete the selected files?"), _("Confirm"),
                      wxICON_WARNING | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT) != wxYES) {
        return;
    }

    // Concatenate list of files to be updated
    for(size_t i = 0; i < m_selectionInfo.m_paths.GetCount(); i++) {
        command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
    }

    m_plugin->GetConsole()->Execute(command, DoGetCurRepoPath(),
                                    new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));
}

void SubversionView::OnResolve(wxCommandEvent& event)
{
    wxString command;
    wxString loginString;
    if(m_plugin->LoginIfNeeded(event, DoGetCurRepoPath(), loginString) == false) {
        return;
    }

    command << m_plugin->GetSvnExeName() << loginString << wxT(" resolved ");

    // Concatenate list of files to be updated
    for(size_t i = 0; i < m_selectionInfo.m_paths.GetCount(); i++) {
        command << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
    }

    m_plugin->GetConsole()->Execute(command, DoGetCurRepoPath(),
                                    new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));
}

void SubversionView::OnDiff(wxCommandEvent& event)
{
    wxString loginString;
    if(m_plugin->LoginIfNeeded(event, DoGetCurRepoPath(), loginString) == false) {
        return;
    }

    DiffDialog dlg(this, m_plugin->GetManager());
    if(dlg.ShowModal() == wxID_OK) {
        wxString from = dlg.GetFromRevision();
        wxString to = dlg.GetToRevision();

        if(to.IsEmpty() == false) {
            to.Prepend(wxT(":"));
        }

        // Simple diff
        wxString diff_cmd;
        diff_cmd << m_plugin->GetSvnExeNameNoConfigDir() << loginString;

        SvnSettingsData ssd = m_plugin->GetSettings();
        if(ssd.GetFlags() & SvnUseExternalDiff) {
            diff_cmd << " --diff-cmd=\"" << ssd.GetExternalDiffViewer() << "\" ";
        }
        diff_cmd << " diff ";

        if(dlg.IgnoreWhitespaces() && !(ssd.GetFlags() & SvnUseExternalDiff)) {
            diff_cmd << " -x -w ";
        }

        diff_cmd << " -r " << from << to << " ";

        for(size_t i = 0; i < m_selectionInfo.m_paths.GetCount(); i++) {
            diff_cmd << wxT("\"") << m_selectionInfo.m_paths.Item(i) << wxT("\" ");
        }
        m_plugin->GetConsole()->Execute(diff_cmd, DoGetCurRepoPath(), new SvnDiffHandler(m_plugin, event.GetId(), this),
                                        false);
    }
}

void SubversionView::OnPatch(wxCommandEvent& event) { m_plugin->Patch(false, DoGetCurRepoPath(), this, event.GetId()); }

void SubversionView::OnPatchDryRun(wxCommandEvent& event)
{
    m_plugin->Patch(true, DoGetCurRepoPath(), this, event.GetId());
}

void SubversionView::OnCleanup(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString command;
    command << m_plugin->GetSvnExeName() << wxT(" cleanup ");
    m_plugin->GetConsole()->Execute(command, DoGetCurRepoPath(),
                                    new SvnDefaultCommandHandler(m_plugin, wxNOT_FOUND, NULL));
}

void SubversionView::OnStop(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_plugin->GetConsole()->Stop();
}

void SubversionView::OnClearOuptut(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_plugin->GetConsole()->Clear();
}

void SubversionView::OnRefreshView(wxCommandEvent& event)
{
    event.Skip();
    BuildTree();
}

void SubversionView::OnFileAdded(clCommandEvent& event)
{
    event.Skip();

    typedef std::map<wxString, bool> StringBoolMap_t;
    StringBoolMap_t path_in_svn;

    // svn is setup ?
    int flags = event.GetInt();
    if(flags & kEventImportingFolder) return;

    SvnSettingsData ssd = m_plugin->GetSettings();
    if(ssd.GetFlags() & SvnAddFileToSvn) {
        const wxArrayString& files = event.GetStrings();
        bool addToSvn(false);
        wxString command;
        command << m_plugin->GetSvnExeName() << wxT(" add ");
        for(size_t i = 0; i < files.GetCount(); i++) {

            wxString currentFilePath = files.Item(i).BeforeLast(wxFILE_SEP_PATH);
            bool curPathUnderSvn = false;
            if(path_in_svn.count(currentFilePath)) {
                // use the cached result
                curPathUnderSvn = path_in_svn.find(currentFilePath)->second;

            } else {
                // query svn and cache the result for future use
                curPathUnderSvn = m_plugin->IsPathUnderSvn(currentFilePath);
                path_in_svn.insert(std::make_pair(currentFilePath, curPathUnderSvn));
            }

            if(curPathUnderSvn) {
                command << wxT("\"") << files.Item(i) << wxT("\" ");
                addToSvn = true;
            }
        }

        if(addToSvn) {
            command.RemoveLast();
            m_plugin->GetConsole()->Execute(command, DoGetCurRepoPath(),
                                            new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));
        }
    }
}

void SubversionView::OnFileRenamed(clFileSystemEvent& event)
{
    // If the Svn Client Version is set to 0.0 it means that we dont have SVN client installed
    if((event.GetEventObject() != this) && m_plugin->GetSvnClientVersion() &&
       (m_plugin->GetSettings().GetFlags() & SvnRenameFileInRepo)) {
        wxString oldName = event.GetPath();
        wxString newName = event.GetNewpath();

        if(m_plugin->IsPathUnderSvn(wxFileName(oldName).GetPath()) == false) {
            event.Skip();
            return;
        }

        wxString command;
        command << m_plugin->GetSvnExeName() << wxT(" rename \"") << oldName << wxT("\" \"") << newName << wxT("\"");
        m_plugin->GetConsole()->Execute(command, DoGetCurRepoPath(),
                                        new SvnDefaultCommandHandler(m_plugin, event.GetId(), this));

        // We need to fire this event again. This time we set the event-object to 'this'
        // so the fired event won't get handled by this handler
        clFileSystemEvent e(wxEVT_FILE_RENAMED);
        e.SetEventObject(this); // to avoid stackoverflow...
        e.SetPath(oldName);
        e.SetNewpath(newName);
        EventNotifier::Get()->AddPendingEvent(e);
    } else {
        event.Skip();
    }
}

void SubversionView::OnShowSvnInfo(wxCommandEvent& event)
{
    wxUnusedVar(event);

    SvnInfo svnInfo;
    m_plugin->DoGetSvnInfoSync(svnInfo, DoGetCurRepoPath());

    SvnInfoDialog dlg(m_plugin->GetManager()->GetTheApp()->GetTopWindow());
    dlg.GetTextCtrlAuthor()->SetValue(svnInfo.m_author);
    dlg.GetTextCtrlDate()->SetValue(svnInfo.m_date);
    dlg.GetTextCtrlRevision()->SetValue(svnInfo.m_revision);
    dlg.GetTextCtrlRootURL()->SetValue(svnInfo.m_url);
    dlg.GetTextCtrlURL()->SetValue(svnInfo.m_sourceUrl);
    dlg.ShowModal();
}

void SubversionView::OnItemActivated(wxTreeEvent& event)
{
    wxTreeItemId item = m_treeCtrl->GetFocusedItem();
    CHECK_ITEM_RET(item);

    SvnTreeData* data = (SvnTreeData*)m_treeCtrl->GetItemData(item);
    if(!data || data->GetType() != SvnTreeData::SvnNodeTypeFile) {
        event.Skip();
        return;
    }

    wxString loginString;
    if(m_plugin->LoginIfNeeded(event, DoGetCurRepoPath(), loginString) == false) {
        return;
    }

    // Simple diff
    wxString command;

    // By default use ignore-whitespaces
    command << m_plugin->GetSvnExeNameNoConfigDir() << loginString;

    SvnSettingsData ssd = m_plugin->GetSettings();
    if(ssd.GetFlags() & SvnUseExternalDiff) {

        // Using external diff viewer
        command << " diff \"" << data->GetFilepath() << "\" --diff-cmd=\"" << ssd.GetExternalDiffViewer() << "\"";
        m_plugin->GetConsole()->Execute(command, DoGetCurRepoPath(), new SvnDiffHandler(m_plugin, event.GetId(), this),
                                        false);

    } else {

        // Use the internal diff viewer
        // --diff-cmd will execute external tool like this:
        // -u -L "php-plugin/XDebugManager.cpp	(revision 447)" -L "php-plugin/XDebugManager.cpp	(working copy)"
        // C:\src\codelite\codelitephp\.svn\pristine\ae\ae25b80b53f432c6124c455ef815679df6ed4ea4.svn-base
        // C:\src\codelite\codelitephp\php-plugin\XDebugManager.cpp
        command << " diff \"" << data->GetFilepath() << "\" --diff-cmd=";
        // We dont have proper echo on windows that can be used here, so
        // we provide our own batch script wrapper
        wxString echo = wxFileName(clStandardPaths::Get().GetBinaryFullPath("codelite-echo")).GetFullPath();
        command << ::WrapWithQuotes(echo);

        // make sure we kill previous codelite-echo executable
        wxDELETE(m_codeliteEcho);

        wxArrayString lines;
        {
            DirSaver ds;
            ::wxSetWorkingDirectory(DoGetCurRepoPath());
            DiffCmdHandler* cmdHandler = new DiffCmdHandler(this, data->GetFilepath());
            m_codeliteEcho = ::CreateAsyncProcessCB(this, cmdHandler, command);
        }
    }
}

void SubversionView::OnStopUI(wxUpdateUIEvent& event) { event.Enable(m_plugin->GetConsole()->IsRunning()); }

void SubversionView::OnClearOuptutUI(wxUpdateUIEvent& event)
{
    event.Enable(m_plugin->GetConsole()->IsEmpty() == false);
}

void SubversionView::OnCheckout(wxCommandEvent& event)
{
    wxString loginString;
    if(!m_plugin->LoginIfNeeded(event, DoGetCurRepoPath(), loginString)) return;

    wxString command;
    SvnCheckoutDialog dlg(m_plugin->GetManager()->GetTheApp()->GetTopWindow(), m_plugin);
    if(dlg.ShowModal() == wxID_OK) {
        command << m_plugin->GetSvnExeName() << loginString << wxT(" co ") << dlg.GetURL() << wxT(" \"")
                << dlg.GetTargetDir() << wxT("\"");
        m_plugin->GetConsole()->ExecuteURL(command, dlg.GetURL(), new SvnCheckoutHandler(m_plugin, event.GetId(), this),
                                           true);
    }
}

void SubversionView::OnIgnoreFile(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_plugin->IgnoreFiles(m_selectionInfo.m_paths, false);
}

void SubversionView::OnIgnoreFilePattern(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_plugin->IgnoreFiles(m_selectionInfo.m_paths, true);
}

void SubversionView::OnSettings(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_plugin->EditSettings();
}

void SubversionView::OnBlame(wxCommandEvent& event) { m_plugin->Blame(event, m_selectionInfo.m_paths); }

void SubversionView::OnLinkEditor(wxCommandEvent& event)
{
    SvnSettingsData ssd = m_plugin->GetSettings();
    if(event.IsChecked())
        ssd.SetFlags(ssd.GetFlags() | SvnLinkEditor);
    else
        ssd.SetFlags(ssd.GetFlags() & ~SvnLinkEditor);

    m_plugin->SetSettings(ssd);

    DoLinkEditor();
}

void SubversionView::DoLinkEditor()
{
    if(!(m_plugin->GetSettings().GetFlags() & SvnLinkEditor)) return;

    IEditor* editor = m_plugin->GetManager()->GetActiveEditor();
    if(!editor) return;

    wxString fullPath = editor->GetFileName().GetFullPath();
    wxTreeItemId root = m_treeCtrl->GetRootItem();
    if(root.IsOk() == false) return;

    wxString basePath = DoGetCurRepoPath();
    wxTreeItemIdValue cookie;
    wxTreeItemIdValue childCookie;
    wxTreeItemId parent = m_treeCtrl->GetFirstChild(root, cookie);
    while(parent.IsOk()) {
        // Loop over the main nodes 'modified', 'unversioned' etc
        if(m_treeCtrl->ItemHasChildren(parent)) {
            // Loop over the files under the main nodes
            wxTreeItemId child = m_treeCtrl->GetFirstChild(parent, childCookie);
            while(child.IsOk()) {
                wxTreeItemId match = DoFindFile(child, basePath, fullPath);
                if(match.IsOk()) {
                    m_treeCtrl->UnselectAll();
                    m_treeCtrl->SelectItem(match);
                    m_treeCtrl->EnsureVisible(match);
                    return;
                }
                child = m_treeCtrl->GetNextChild(parent, childCookie);
            }
        }
        parent = m_treeCtrl->GetNextChild(root, cookie);
    }
}

void SubversionView::OnActiveEditorChanged(wxCommandEvent& event)
{
    event.Skip();
    DoLinkEditor();
}

void SubversionView::DisconnectEvents()
{
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SubversionView::OnWorkspaceLoaded),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(SubversionView::OnWorkspaceClosed),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED, clCommandEventHandler(SubversionView::OnFileSaved), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PROJ_FILE_ADDED, clCommandEventHandler(SubversionView::OnFileAdded), NULL,
                                     this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_RENAMED, &SubversionView::OnFileRenamed, this);
    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED,
                                     wxCommandEventHandler(SubversionView::OnActiveEditorChanged), NULL, this);
    wxTheApp->Unbind(wxEVT_MENU, &SubversionView::OnCommit, this, XRCID("svn_commit"));
    wxTheApp->Unbind(wxEVT_MENU, &SubversionView::OnUpdate, this, XRCID("svn_update"));
    wxTheApp->Unbind(wxEVT_MENU, &SubversionView::OnCommitGotoAnything, this, XRCID("gotoanything_svn_commit"));
    wxTheApp->Unbind(wxEVT_MENU, &SubversionView::OnUpdateGotoAnything, this, XRCID("gotoanything_svn_update"));
}

void SubversionView::OnOpenFile(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxArrayTreeItemIds items;
    wxArrayString paths;
    size_t count = m_treeCtrl->GetSelections(items);
    for(size_t i = 0; i < count; i++) {
        wxTreeItemId item = items.Item(i);

        if(item.IsOk() == false) continue;

        SvnTreeData* data = (SvnTreeData*)m_treeCtrl->GetItemData(item);
        if(data && data->GetType() == SvnTreeData::SvnNodeTypeFile) {
            paths.Add(DoGetCurRepoPath() + wxFileName::GetPathSeparator() + data->GetFilepath());
        }
    }

    for(size_t i = 0; i < paths.GetCount(); i++) {

        if(wxFileName(paths.Item(i)).IsDir() == false) m_plugin->GetManager()->OpenFile(paths.Item(i));
    }
}

void SubversionView::OnSwitch(wxCommandEvent& event)
{
    SvnInfo svnInfo;
    m_plugin->DoGetSvnInfoSync(svnInfo, DoGetCurRepoPath());
    m_plugin->DoSwitchURL(DoGetCurRepoPath(), svnInfo.m_sourceUrl, event);
}

void SubversionView::OnProperties(wxCommandEvent& event)
{
    SvnInfo svnInfo;
    m_plugin->DoGetSvnInfoSync(svnInfo, DoGetCurRepoPath());

    SvnPropsDlg dlg(m_plugin->GetManager()->GetTheApp()->GetTopWindow(), svnInfo.m_sourceUrl, m_plugin);
    if(dlg.ShowModal() == wxID_OK) {
        SubversionLocalProperties props(svnInfo.m_sourceUrl);
        props.WriteProperty(SubversionLocalProperties::BUG_TRACKER_MESSAGE, dlg.GetBugMsg());
        props.WriteProperty(SubversionLocalProperties::BUG_TRACKER_URL, dlg.GetBugTrackerURL());
        props.WriteProperty(SubversionLocalProperties::FR_TRACKER_MESSAGE, dlg.GetFRMsg());
        props.WriteProperty(SubversionLocalProperties::FR_TRACKER_URL, dlg.GetFRTrackerURL());
    }
}

void SubversionView::OnLog(wxCommandEvent& event)
{
    m_plugin->ChangeLog(DoGetCurRepoPath(), DoGetCurRepoPath(), event);
}

void SubversionView::OnLock(wxCommandEvent& event)
{
    wxArrayString files;
    for(size_t i = 0; i < m_selectionInfo.m_paths.size(); i++) {
        wxFileName fn(DoGetCurRepoPath() + wxFileName::GetPathSeparator() + m_selectionInfo.m_paths.Item(i));
        files.Add(fn.GetFullPath());
    }
    m_plugin->DoLockFile(DoGetCurRepoPath(), files, event, true);
}

void SubversionView::OnUnLock(wxCommandEvent& event)
{
    wxArrayString files;
    for(size_t i = 0; i < m_selectionInfo.m_paths.size(); i++) {
        wxFileName fn(DoGetCurRepoPath() + wxFileName::GetPathSeparator() + m_selectionInfo.m_paths.Item(i));
        files.Add(fn.GetFullPath());
    }
    m_plugin->DoLockFile(DoGetCurRepoPath(), files, event, false);
}

void SubversionView::DoChangeRootPathUI(const wxString& path)
{
    if(path == wxEmptyString) {
        ClearAll();
    }
    m_curpath = path;
}

void SubversionView::DoRootDirChanged(const wxString& path)
{
    if(path == wxEmptyString) {
        DoChangeRootPathUI(path);

    } else {

        // If a workspace is opened, set this new path to the workspace
        SvnSettingsData ssd = m_plugin->GetSettings();

        const wxArrayString& repos = ssd.GetRepos();
        wxArrayString modDirs = repos;
        if(modDirs.Index(path) == wxNOT_FOUND) {
            modDirs.Add(path);
        }

        ssd.SetRepos(modDirs);
        m_plugin->SetSettings(ssd);

        if(m_plugin->GetManager()->IsWorkspaceOpen()) {
            LocalWorkspaceST::Get()->SetCustomData(wxT("SubversionPath"), path);
            LocalWorkspaceST::Get()->Flush();
        }
        DoChangeRootPathUI(path);
        BuildTree();

        // Clear the source control image
        clStatusBar* sb = clGetManager()->GetStatusBar();
        if(sb) {
            wxBitmap bmp = clGetManager()->GetStdIcons()->LoadBitmap("subversion");
            sb->SetSourceControlBitmap(bmp, "Svn", _("Using Subversion\nClick to open the Subversion view"));
        }
    }
}

wxTreeItemId SubversionView::DoGetParentNode(const wxString& filename, const wxTreeItemId& parent)
{
    wxFileName fn(filename);
    wxTreeItemId actualParentNode = parent;
    wxArrayString dirs = fn.GetDirs();
    wxString curpath;
    for(size_t i = 0; i < dirs.GetCount(); i++) {
        // add or get the parent for this node
        curpath << dirs.Item(i) << wxT("/");
        actualParentNode = DoFindChild(actualParentNode, dirs.Item(i), curpath);
    }
    return actualParentNode;
}

wxTreeItemId SubversionView::DoFindChild(const wxTreeItemId& parent, const wxString& name, const wxString& curpath)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeCtrl->GetFirstChild(parent, cookie);
    while(child.IsOk()) {
        if(m_treeCtrl->GetItemText(child) == name) {
            return child;
        }
        child = m_treeCtrl->GetNextChild(parent, cookie);
    }
    // if we reached here, we did not find a tree node for this name
    return m_treeCtrl->AppendItem(parent,        // parent node
                                  name,          // text
                                  FOLDER_IMG_ID, // folder icon
                                  FOLDER_IMG_ID, new SvnTreeData(SvnTreeData::SvnNodeTypeFolder, curpath));
}

void SubversionView::OnRename(wxCommandEvent& event)
{
    wxArrayString files;
    for(size_t i = 0; i < m_selectionInfo.m_paths.size(); i++) {
        wxFileName oldname(DoGetCurRepoPath() + wxFileName::GetPathSeparator() + m_selectionInfo.m_paths.Item(i));
        wxString newname = wxGetTextFromUser(_("New name:"), _("Svn rename..."), oldname.GetFullName());

        if(newname.IsEmpty() || newname == oldname.GetFullName()) continue;

        m_plugin->DoRename(DoGetCurRepoPath(), oldname.GetFullName(), newname, event);
    }
}

wxString SubversionView::DoGetCurRepoPath() const { return m_curpath; }

wxTreeItemId SubversionView::DoFindFile(const wxTreeItemId& parent, const wxString& basepath, const wxString& fullpath)
{
    if(parent.IsOk() == false) {
        return wxTreeItemId();
    }

    SvnTreeData* data = static_cast<SvnTreeData*>(m_treeCtrl->GetItemData(parent));
    if(data && data->GetType() == SvnTreeData::SvnNodeTypeFile) {
        wxFileName fn(data->GetFilepath());
        fn.MakeAbsolute(basepath);
        if(fn.GetFullPath() == fullpath) {
            return parent;
        }
    }

    if(m_treeCtrl->ItemHasChildren(parent)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_treeCtrl->GetFirstChild(parent, cookie);
        while(child.IsOk()) {
            wxTreeItemId fileId = DoFindFile(child, basepath, fullpath);
            if(fileId.IsOk()) {
                return fileId;
            }
            child = m_treeCtrl->GetNextChild(parent, cookie);
        }
    }
    return wxTreeItemId();
}

void SubversionView::OnFileSaved(clCommandEvent& event)
{
    event.Skip();
    OnRefreshView(event);
}
void SubversionView::OnCharAdded(wxStyledTextEvent& event) { m_subversionConsole->OnCharAdded(event); }
void SubversionView::OnKeyDown(wxKeyEvent& event) { m_subversionConsole->OnKeyDown(event); }
void SubversionView::OnUpdateUI(wxStyledTextEvent& event) { m_subversionConsole->OnUpdateUI(event); }

void SubversionView::FinishDiff(wxString output, wxFileName fileBeingDiffed)
{
    clCommandLineParser parser(output);
    wxArrayString tokens = parser.ToArray();
    if(tokens.GetCount() < 2) {
        wxDELETE(m_codeliteEcho);
        return;
    }
    wxString rightFile = tokens.Last();
    tokens.RemoveAt(tokens.GetCount() - 1);
    wxString leftFile = tokens.Last();

    // get the left file title
    wxString title_left, title_right;
    title_right = _("Working copy");
    title_left = _("HEAD version");

    DiffSideBySidePanel::FileInfo l(leftFile, title_left, true);
    DiffSideBySidePanel::FileInfo r(rightFile, title_right, false);
    clDiffFrame* diffView = new clDiffFrame(EventNotifier::Get()->TopFrame(), l, r, true);
    diffView->Show();
    wxDELETE(m_codeliteEcho);
}
void SubversionView::OnSciStcChange(wxStyledTextEvent& event)
{
    event.Skip();
    ::clRecalculateSTCHScrollBar(m_sci);
}

void SubversionView::OnCloseView(wxCommandEvent& event)
{
    if(::wxMessageBox(_("Close SVN view?"), _("Confirm"), wxICON_QUESTION | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT) !=
       wxYES) {
        return;
    }
    DoCloseView();
}

void SubversionView::DoCloseView()
{
    DoChangeRootPathUI("");
    wxCommandEvent dummy;
    OnClearOuptut(dummy);
}

void SubversionView::OnCommitGotoAnything(wxCommandEvent& event)
{
    if(m_treeCtrl->IsEmpty()) {
        return;
    }
    wxArrayString paths;
    DoGetPaths(m_treeCtrl->GetRootItem(), paths);
    m_plugin->DoCommit(paths, DoGetCurRepoPath(), event);
}

void SubversionView::OnUpdateGotoAnything(wxCommandEvent& event)
{
    wxString command;
    wxString loginString;
    if(m_plugin->LoginIfNeeded(event, DoGetCurRepoPath(), loginString) == false) {
        return;
    }
    command << m_plugin->GetSvnExeName() << loginString << wxT(" update ");
    m_plugin->AddCommandLineOption(command, Subversion2::kOpt_ForceInteractive);
    m_plugin->GetConsole()->Execute(command, DoGetCurRepoPath(), new SvnUpdateHandler(m_plugin, event.GetId(), this),
                                    true, true);
}
