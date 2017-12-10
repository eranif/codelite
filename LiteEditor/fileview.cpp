//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : fileview.cpp
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
#include "ICompilerLocator.h"
#include "ImportFilesDialogNew.h"
#include "NewVirtualFolderDlg.h"
#include "bitmap_loader.h"
#include "build_custom_targets_menu_manager.h"
#include "build_settings_config.h"
#include "buildmanager.h"
#include "clFileOrFolderDropTarget.h"
#include "cl_command_event.h"
#include "compiler.h"
#include "ctags_manager.h"
#include "depends_dlg.h"
#include "dirtraverser.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "editorsettingslocal.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "evnvarlist.h"
#include "file_logger.h"
#include "fileutils.h"
#include "fileview.h"
#include "frame.h"
#include "globals.h"
#include "importfilessettings.h"
#include "localworkspace.h"
#include "macros.h"
#include "manager.h"
#include "nameanddescdlg.h"
#include "new_item_dlg.h"
#include "plugin.h"
#include "pluginmanager.h"
#include "progress_dialog.h"
#include "project.h"
#include "project_settings_dlg.h"
#include "tree.h"
#include "workspacesettingsdlg.h"
#include "workspacetab.h"
#include "wx/imaglist.h"
#include "wxStringHash.h"
#include <algorithm>
#include <deque>
#include <project.h>
#include <queue>
#include <wx/colordlg.h>
#include <wx/filedlg.h>
#include <wx/mimetype.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/richmsgdlg.h>
#include <wx/textdlg.h>
#include <wx/tokenzr.h>
#include <wx/treectrl.h>
#include <wx/xrc/xmlres.h>

IMPLEMENT_DYNAMIC_CLASS(FileViewTree, wxTreeCtrl)

static const wxString gsCustomTargetsMenu(wxT("Custom Build Targets"));

BEGIN_EVENT_TABLE(FileViewTree, wxTreeCtrl)
EVT_TREE_BEGIN_DRAG(wxID_ANY, FileViewTree::OnItemBeginDrag)
EVT_TREE_END_DRAG(wxID_ANY, FileViewTree::OnItemEndDrag)
EVT_TREE_ITEM_MENU(wxID_ANY, FileViewTree::OnPopupMenu)
EVT_TREE_SEL_CHANGED(wxID_ANY, FileViewTree::OnSelectionChanged)

EVT_MENU(XRCID("local_workspace_prefs"), FileViewTree::OnLocalPrefs)
EVT_MENU(XRCID("local_workspace_settings"), FileViewTree::OnLocalWorkspaceSettings)
EVT_MENU(XRCID("remove_project"), FileViewTree::OnRemoveProject)
EVT_MENU(XRCID("rename_project"), FileViewTree::OnRenameProject)
EVT_MENU(XRCID("set_as_active"), FileViewTree::OnSetActive)
EVT_MENU(XRCID("new_item"), FileViewTree::OnNewItem)
EVT_MENU(XRCID("add_existing_item"), FileViewTree::OnAddExistingItem)
EVT_MENU(XRCID("new_virtual_folder"), FileViewTree::OnNewVirtualFolder)
EVT_MENU(XRCID("remove_virtual_folder"), FileViewTree::OnRemoveVirtualFolder)
EVT_MENU(XRCID("local_project_prefs"), FileViewTree::OnLocalPrefs)
EVT_MENU(XRCID("project_properties"), FileViewTree::OnProjectProperties)
EVT_MENU(XRCID("sort_item"), FileViewTree::OnSortItem)
EVT_MENU(XRCID("remove_item"), FileViewTree::OnRemoveItem)
EVT_MENU(XRCID("save_as_template"), FileViewTree::OnSaveAsTemplate)
EVT_MENU(XRCID("build_order"), FileViewTree::OnBuildOrder)
EVT_MENU(XRCID("clean_project"), FileViewTree::OnClean)
EVT_MENU(XRCID("build_project"), FileViewTree::OnBuild)
EVT_MENU(XRCID("rebuild_project"), FileViewTree::OnReBuild)
EVT_MENU(XRCID("generate_makefile"), FileViewTree::OnRunPremakeStep)
EVT_MENU(XRCID("stop_build"), FileViewTree::OnStopBuild)
EVT_MENU(XRCID("retag_project"), FileViewTree::OnRetagProject)
EVT_MENU(XRCID("build_project_only"), FileViewTree::OnBuildProjectOnly)
EVT_MENU(XRCID("clean_project_only"), FileViewTree::OnCleanProjectOnly)
EVT_MENU(XRCID("rebuild_project_only"), FileViewTree::OnRebuildProjectOnly)
EVT_MENU(XRCID("import_directory"), FileViewTree::OnImportDirectory)
EVT_MENU(XRCID("reconcile_project"), FileViewTree::OnReconcileProject)
EVT_MENU(XRCID("open_in_editor"), FileViewTree::OnOpenInEditor)
EVT_MENU(XRCID("compile_item"), FileViewTree::OnCompileItem)
EVT_MENU(XRCID("cxx_fileview_open_shell_from_filepath"), FileViewTree::OnOpenShellFromFilePath)
EVT_MENU(XRCID("cxx_fileview_open_file_explorer"), FileViewTree::OnOpenFileExplorerFromFilePath)
EVT_MENU(XRCID("exclude_from_build"), FileViewTree::OnExcludeFromBuild)
EVT_MENU(XRCID("preprocess_item"), FileViewTree::OnPreprocessItem)
EVT_MENU(XRCID("rename_item"), FileViewTree::OnRenameItem)
EVT_MENU(XRCID("rename_virtual_folder"), FileViewTree::OnRenameVirtualFolder)
EVT_MENU(XRCID("colour_virtual_folder"), FileViewTree::OnSetBgColourVirtualFolder)
EVT_MENU(XRCID("clear_virtual_folder_colour"), FileViewTree::OnClearBgColourVirtualFolder)
EVT_MENU(XRCID("open_with_default_application"), FileViewTree::OnOpenWithDefaultApplication)

EVT_UPDATE_UI(XRCID("remove_project"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("rename_project"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("set_as_active"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("new_item"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("add_existing_item"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("new_virtual_folder"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("remove_virtual_folder"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("project_properties"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("sort_item"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("remove_item"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("export_makefile"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("save_as_template"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("build_order"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("clean_project"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("build_project"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("rebuild_project"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("retag_project"), FileViewTree::OnRetagInProgressUI)
EVT_UPDATE_UI(XRCID("retag_workspace"), FileViewTree::OnRetagInProgressUI)
EVT_UPDATE_UI(XRCID("build_project_only"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("clean_project_only"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("rebuild_project_only"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("import_directory"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("reconcile_project"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("compile_item"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("exclude_from_build"), FileViewTree::OnExcludeFromBuildUI)
EVT_UPDATE_UI(XRCID("preprocess_item"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("rename_item"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("generate_makefile"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("local_workspace_settings"), FileViewTree::OnBuildInProgress)
END_EVENT_TABLE()

static int PROJECT_IMG_IDX = wxNOT_FOUND;
static int FOLDER_IMG_IDX = wxNOT_FOUND;
static int WORKSPACE_IMG_IDX = wxNOT_FOUND;
static int ACTIVE_PROJECT_IMG_IDX = wxNOT_FOUND;
static int FOLDER_EXPAND_IMG_IDX = wxNOT_FOUND;
static int WORKSPACE_FOLDER_IMG_IDX = wxNOT_FOUND;

FileViewTree::FileViewTree()
    : m_eventsBound(false)
{
    m_colourHelper.Reset(new clTreeCtrlColourHelper(this));
}

void FileViewTree::OnBuildInProgress(wxUpdateUIEvent& event) { event.Enable(!ManagerST::Get()->IsBuildInProgress()); }

FileViewTree::FileViewTree(wxWindow* parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : m_eventsBound(false)
{
    Create(parent, id, pos, size, style);
    m_colourHelper.Reset(new clTreeCtrlColourHelper(this));
    
#ifndef __WXGTK3__
    SetBackgroundColour(wxBG_STYLE_CUSTOM);
#endif

    MSWSetNativeTheme(this);
    m_keyboardHelper.reset(new clTreeKeyboardInput(this));

    // Initialise images map
    BitmapLoader* bmpLoader = PluginManager::Get()->GetStdIcons();

    // Prepare the standard mime-type image list
    wxImageList* images = bmpLoader->MakeStandardMimeImageList();

    FOLDER_EXPAND_IMG_IDX = bmpLoader->GetMimeImageId(FileExtManager::TypeFolderExpanded);
    FOLDER_IMG_IDX = images->Add(bmpLoader->LoadBitmap(wxT("folder-yellow")));
    ACTIVE_PROJECT_IMG_IDX = images->Add(bmpLoader->LoadBitmap(wxT("project")));
    WORKSPACE_IMG_IDX = bmpLoader->GetMimeImageId(FileExtManager::TypeWorkspace);
    PROJECT_IMG_IDX = bmpLoader->GetMimeImageId(FileExtManager::TypeProject);
    WORKSPACE_FOLDER_IMG_IDX = images->Add(bmpLoader->LoadBitmap("workspace-folder-yellow"));

    AssignImageList(images);
    Connect(GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler(FileViewTree::OnMouseDblClick));
    Connect(GetId(), wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(FileViewTree::OnItemActivated));
    EventNotifier::Get()->Connect(wxEVT_REBUILD_WORKSPACE_TREE, wxCommandEventHandler(FileViewTree::OnBuildTree), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_CMD_BUILD_PROJECT_ONLY,
                                  wxCommandEventHandler(FileViewTree::OnBuildProjectOnlyInternal), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CMD_CLEAN_PROJECT_ONLY,
                                  wxCommandEventHandler(FileViewTree::OnCleanProjectOnlyInternal), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CONFIG_CHANGED, &FileViewTree::OnBuildConfigChanged, this);
    Bind(wxEVT_DND_FOLDER_DROPPED, &FileViewTree::OnFolderDropped, this);
    Bind(wxEVT_TREE_ITEM_EXPANDING, &FileViewTree::OnItemExpanding, this);
}

FileViewTree::~FileViewTree()
{
    EventNotifier::Get()->Disconnect(wxEVT_REBUILD_WORKSPACE_TREE, wxCommandEventHandler(FileViewTree::OnBuildTree),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_BUILD_PROJECT_ONLY,
                                     wxCommandEventHandler(FileViewTree::OnBuildProjectOnlyInternal), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_CLEAN_PROJECT_ONLY,
                                     wxCommandEventHandler(FileViewTree::OnCleanProjectOnlyInternal), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CONFIG_CHANGED, &FileViewTree::OnBuildConfigChanged, this);
    Unbind(wxEVT_DND_FOLDER_DROPPED, &FileViewTree::OnFolderDropped, this);
    Unbind(wxEVT_TREE_ITEM_EXPANDING, &FileViewTree::OnItemExpanding, this);
    m_keyboardHelper.reset(NULL);
}

void FileViewTree::Create(wxWindow* parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    bool multi(true);
    style |= (wxTR_HAS_BUTTONS | wxTR_FULL_ROW_HIGHLIGHT | wxTR_NO_LINES | wxBORDER_NONE);
    if(multi) style |= wxTR_MULTIPLE;

    wxTreeCtrl::Create(parent, id, pos, size, style);
    SetDropTarget(new clFileOrFolderDropTarget(this));
    BuildTree();
}

void FileViewTree::BuildTree()
{
    wxFont defaultGuiFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

    wxWindowUpdateLocker locker(this);
    clCommandEvent event(wxEVT_WORKSPACE_VIEW_BUILD_STARTING);
    if(EventNotifier::Get()->ProcessEvent(event)) {
        // User wishes to replace the icons
        wxImageList* imgList = reinterpret_cast<wxImageList*>(event.GetClientData());
        if(imgList) { AssignImageList(imgList); }
    }

    SetFont(defaultGuiFont);

    DoClear();
    DoBindEvents(); // This only works once
    long flags = GetWindowStyle();
    SetWindowStyle(flags | wxTR_MULTIPLE);

    if(ManagerST::Get()->IsWorkspaceOpen()) {
        // Add an invisible tree root
        ProjectItem data;
        data.m_displayName = clCxxWorkspaceST::Get()->GetName();
        data.m_kind = ProjectItem::TypeWorkspace;

        wxTreeItemId root = AddRoot(data.m_displayName, WORKSPACE_IMG_IDX, -1, new FilewViewTreeItemData(data));
        SetItemFont(root, defaultGuiFont);
        m_itemsToSort[root.m_pItem] = true;

        wxArrayString list;
        ManagerST::Get()->GetProjectList(list);

        wxArrayString folders = clCxxWorkspaceST::Get()->GetWorkspaceFolders();
        for(size_t i = 0; i < folders.size(); ++i) {
            AddWorkspaceFolder(folders.Item(i));
        }

        for(size_t n = 0; n < list.GetCount(); n++) {
            BuildProjectNode(list.Item(n));
        }
        SortTree();

        // set selection to first item
        SelectItem(root, HasFlag(wxTR_MULTIPLE) ? false : true);
    }

    // Always expand to the active project
    ExpandToPath(clCxxWorkspaceST::Get()->GetActiveProjectName(), wxFileName());
}

void FileViewTree::SortItem(wxTreeItemId& item)
{
    if(item.IsOk() && ItemHasChildren(item)) { SortChildren(item); }
}

void FileViewTree::SortTree()
{
    // sort the tree
    std::unordered_map<void*, bool>::const_iterator iter = m_itemsToSort.begin();
    for(; iter != m_itemsToSort.end(); ++iter) {
        wxTreeItemId item = iter->first;
        SortItem(item);
    }
    m_itemsToSort.clear();
}

wxTreeItemId FileViewTree::GetSingleSelection()
{
    std::queue<wxTreeItemId> Q;
    Q.push(GetRootItem());
    while(!Q.empty()) {
        wxTreeItemId item = Q.front();
        Q.pop();
        if(IsSelected(item)) { return item; }

        wxTreeItemIdValue k;
        wxTreeItemId child = GetFirstChild(item, k);
        while(child.IsOk()) {
            Q.push(child);
            child = GetNextChild(item, k);
        }
    }
    // Return an invalid tree-item-id
    return wxTreeItemId();
}

int FileViewTree::GetIconIndex(const ProjectItem& item)
{
    BitmapLoader* bmpLoader = PluginManager::Get()->GetStdIcons();
    int icondIndex(bmpLoader->GetMimeImageId(FileExtManager::TypeText));
    switch(item.GetKind()) {
    case ProjectItem::TypeProject:
        icondIndex = PROJECT_IMG_IDX;
        break;
    case ProjectItem::TypeVirtualDirectory:
        icondIndex = FOLDER_IMG_IDX;
        break;
    case ProjectItem::TypeFile: {
        wxFileName filename(item.GetFile());
        icondIndex = bmpLoader->GetMimeImageId(filename.GetFullName());
        if(icondIndex == wxNOT_FOUND) icondIndex = bmpLoader->GetMimeImageId(FileExtManager::TypeText);
    } break;
    case ProjectItem::TypeWorkspaceFolder:
        icondIndex = WORKSPACE_FOLDER_IMG_IDX;
        break;
    default:
        break;
    }
    return icondIndex;
}

void FileViewTree::BuildProjectNode(const wxString& projectName)
{
    wxString err_msg;
    ProjectPtr prj = clCxxWorkspaceST::Get()->FindProjectByName(projectName, err_msg);

    // Add the folder containing this project
    wxTreeItemId rootItem = AddWorkspaceFolder(prj->GetWorkspaceFolder());

    FolderColour::Map_t coloursMap;
    FolderColour::List_t coloursList;
    LocalWorkspaceST::Get()->GetFolderColours(coloursMap);

    // Sort the list
    FolderColour::SortToList(coloursMap, coloursList);

    // We only the project node
    bool iconFromPlugin = false;
    int projectIconIndex = wxNOT_FOUND;
    DoGetProjectIconIndex(projectName, projectIconIndex, iconFromPlugin);

    ProjectItem item(projectName, projectName, prj->GetFileName().GetFullPath(), ProjectItem::TypeProject);
    wxTreeItemId hti = AppendItem(rootItem,         // parent
                                  projectName,      // display name
                                  projectIconIndex, // item image index
                                  projectIconIndex, // selected item image
                                  new FilewViewTreeItemData(item));
    DoSetItemBackgroundColour(hti, coloursList, item);
    m_projectsMap.insert({ projectName, hti });

    // If the project has children (either VD or files) add a dummy item
    // So the user can "Expand" it
    if(!prj->IsEmpty()) {
        AppendItem(hti, "<dummy>", -1, -1,
                   new FilewViewTreeItemData(ProjectItem("", "", "", ProjectItem::TypeInvalid)));
    }

    if(projectName == ManagerST::Get()->GetActiveProjectName()) {
        wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        f.SetWeight(wxFONTWEIGHT_BOLD);
        f.SetStyle(wxFONTSTYLE_ITALIC);
        SetItemFont(hti, f);
        if(!iconFromPlugin) {
            SetItemImage(hti, ACTIVE_PROJECT_IMG_IDX);
            SetItemImage(hti, ACTIVE_PROJECT_IMG_IDX, wxTreeItemIcon_Selected);
            SetItemImage(hti, ACTIVE_PROJECT_IMG_IDX, wxTreeItemIcon_SelectedExpanded);
        }
    }
    m_itemsToSort[rootItem.m_pItem] = true;
}

//-----------------------------------------------
// Event handlers
//-----------------------------------------------

void FileViewTree::ShowFileContextMenu()
{
    wxArrayTreeItemIds items;
    GetSelections(items);
    if(items.IsEmpty()) return;

    wxMenu* menu = wxXmlResource::Get()->LoadMenu(wxT("file_tree_file"));
    if(!ManagerST::Get()->IsBuildInProgress()) {
        // Let the plugins alter it
        clContextMenuEvent event(wxEVT_CONTEXT_MENU_FILE);
        event.SetMenu(menu);

        wxArrayString files;
        for(size_t i = 0; i < items.GetCount(); ++i) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(items.Item(i)));
            if(data->GetData().GetKind() == ProjectItem::TypeFile) { files.Add(data->GetData().GetFile()); }
        }
        event.SetStrings(files);
        EventNotifier::Get()->ProcessEvent(event);

        // Let the plugin hook their content (using the deprecated way)
        PluginManager::Get()->HookPopupMenu(menu, MenuTypeFileView_File);
    }

    PopupMenu(menu);
    wxDELETE(menu);
}

void FileViewTree::ShowVirtualFolderContextMenu(FilewViewTreeItemData* itemData)
{
    wxMenu* menu = wxXmlResource::Get()->LoadMenu("file_tree_folder");
    int where = clFindMenuItemPosition(menu, XRCID("clear_virtual_folder_colour"));
    if(where != wxNOT_FOUND) {
        // Insert the "Set Custom Background Colour" menu entry. We do it like this
        // instead of using the menu.xrc file cause there is a bug that a bitmap can
        // not be set to an already existing menu item
        wxMenuItem* menuItem =
            new wxMenuItem(menu, XRCID("colour_virtual_folder"), _("Set Custom Background Colour..."));
        menuItem->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("colour-pallette"));
        menu->Insert(where + 1, menuItem);
    }

    if(!ManagerST::Get()->IsBuildInProgress()) {
        // Let the plugins alter it
        clContextMenuEvent event(wxEVT_CONTEXT_MENU_VIRTUAL_FOLDER);
        event.SetMenu(menu);
        EventNotifier::Get()->ProcessEvent(event);

        // Let the plugin hook their content (using the deprecated way)
        PluginManager::Get()->HookPopupMenu(menu, MenuTypeFileView_Folder);
    }

    PopupMenu(menu);
    wxDELETE(menu);
}

void FileViewTree::ShowProjectContextMenu(const wxString& projectName)
{
    wxMenu menu;
    DoCreateProjectContextMenu(menu, projectName);

    if(!ManagerST::Get()->IsBuildInProgress()) {
        // Let the plugins alter it
        wxMenu* pluginsMenu = new wxMenu;
        clContextMenuEvent event(wxEVT_CONTEXT_MENU_PROJECT);
        event.SetMenu(pluginsMenu);
        pluginsMenu->SetParent(&menu);
        EventNotifier::Get()->ProcessEvent(event);

        // Use the old system
        PluginManager::Get()->HookPopupMenu(pluginsMenu, MenuTypeFileView_Project);
        if(pluginsMenu->GetMenuItemCount()) {
            // we got something from the plugins
            menu.PrependSeparator();
            menu.Prepend(wxID_ANY, _("Plugins..."), pluginsMenu);
        } else {
            wxDELETE(pluginsMenu);
        }
    }
    PopupMenu(&menu);
}

void FileViewTree::ShowWorkspaceContextMenu()
{
    // Load the basic menu
    wxMenu* menu = wxXmlResource::Get()->LoadMenu(wxT("workspace_popup_menu"));
    if(!ManagerST::Get()->IsBuildInProgress()) {
        // Let the plugins alter it
        clContextMenuEvent event(wxEVT_CONTEXT_MENU_WORKSPACE);
        event.SetMenu(menu);
        EventNotifier::Get()->ProcessEvent(event);

        // Use the old system
        PluginManager::Get()->HookPopupMenu(menu, MenuTypeFileView_Workspace);
    }

    // Show it
    PopupMenu(menu);
    wxDELETE(menu);
}

void FileViewTree::OnPopupMenu(wxTreeEvent& event)
{
    if(event.GetItem().IsOk()) {
        if(IsSelected(event.GetItem()) == false) {
            // Don't call SelectItem() if it's already selected: in <wx2.9 it toggles!
            SelectItem(event.GetItem());
        }
        wxTreeItemId item = event.GetItem();

        if(item.IsOk()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            switch(data->GetData().GetKind()) {
            case ProjectItem::TypeProject:
                ShowProjectContextMenu(data->GetData().GetDisplayName());
                break;
            case ProjectItem::TypeVirtualDirectory:
                ShowVirtualFolderContextMenu(data);
                break;
            case ProjectItem::TypeFile:
                ShowFileContextMenu();
                break;
            case ProjectItem::TypeWorkspace:
                ShowWorkspaceContextMenu();
                break;
            case ProjectItem::TypeWorkspaceFolder:
                ShowWorkspaceFolderContextMenu();
                break;
            default:
                break;
            }
        }
    } else {
        PopupMenu(wxXmlResource::Get()->LoadMenu(wxT("file_view_empty")));
    }
}

TreeItemInfo FileViewTree::GetSelectedItemInfo()
{
    wxTreeItemId item = GetSingleSelection();
    TreeItemInfo info;
    info.m_item = item;
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));

        // set the text of the item
        info.m_text = GetItemText(item);
        info.m_itemType = data->GetData().GetKind();
        info.m_fileName = data->GetData().GetFile();
        if(info.m_itemType == ProjectItem::TypeVirtualDirectory) {
            // in-case of virtual directories, set the file name to be the directory of
            // the project
            wxString path = GetItemPath(item);
            wxString project = path.BeforeFirst(wxT(':'));
            info.m_fileName = wxFileName(ManagerST::Get()->GetProjectCwd(project), wxEmptyString);
        }
    }
    return info;
}

void FileViewTree::OnMouseDblClick(wxMouseEvent& event)
{
    wxArrayTreeItemIds items;
    size_t num = GetMultiSelection(items);
    if(num <= 0) {
        event.Skip();
        return;
    }

    // Make sure the double click was done on an actual item
    int flags = wxTREE_HITTEST_ONITEMLABEL;
    for(size_t i = 0; i < num; i++) {
        if(HitTest(event.GetPosition(), flags) == items.Item(i)) {
            wxTreeItemId item = items.Item(i);
            DoItemActivated(item, event);
            return;
        }
    }
    event.Skip();
}

void FileViewTree::DoItemActivated(wxTreeItemId& item, wxEvent& event)
{
    //-----------------------------------------------------
    // Each tree items, keeps a private user data that
    // holds the key for searching the its corresponding
    // node in the m_tree data structure
    //-----------------------------------------------------
    if(item.IsOk() == false) return;
    FilewViewTreeItemData* itemData = static_cast<FilewViewTreeItemData*>(GetItemData(item));
    if(!itemData) {
        event.Skip();
        return;
    }

    // if file item was hit, open it
    if(itemData->GetData().GetKind() == ProjectItem::TypeFile) {

        wxString filename = itemData->GetData().GetFile();
        wxString key = itemData->GetData().Key();
        wxString project;
        if(key.GetChar(0) == ':') {
            // All the entries I've tested have started with a : so exclude this one, otherwise the project is always ""
            project = key.AfterFirst(':').BeforeFirst(wxT(':'));
        } else {
            project = key.BeforeFirst(wxT(':'));
        }

        // Convert the file name to be in absolute path
        wxFileName fn(filename);
        fn.MakeAbsolute(ManagerST::Get()->GetProjectCwd(project));

        // send event to the plugins to see if they want the file opening in another way
        wxString file_path = fn.GetFullPath();
        clCommandEvent activateEvent(wxEVT_TREE_ITEM_FILE_ACTIVATED);
        activateEvent.SetFileName(file_path);
        if(EventNotifier::Get()->ProcessEvent(activateEvent)) return;

        clMainFrame::Get()->GetMainBook()->OpenFile(fn.GetFullPath(), project, -1);

    } else if(itemData->GetData().GetKind() == ProjectItem::TypeProject) {
        // make it active
        DoSetProjectActive(item);
    } else {
        event.Skip();
    }
}

void FileViewTree::OnOpenInEditor(wxCommandEvent& event)
{
    wxArrayTreeItemIds items;
    size_t num = GetMultiSelection(items);
    for(size_t i = 0; i < num; i++) {
        wxTreeItemId item = items.Item(i);
        FilewViewTreeItemData* itemData = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(itemData && itemData->GetData().GetKind() == ProjectItem::TypeFile) {
            wxString filename = itemData->GetData().GetFile();
            wxString project = itemData->GetData().Key().BeforeFirst(wxT(':'));

            // Convert the file name to an absolute path
            wxFileName fn(filename);
            fn.MakeAbsolute(ManagerST::Get()->GetProjectCwd(project));

            // DON'T ask the plugins if they want the file opening in another way, as happens from a double-click
            // Here we _know_ the user wants to open in CL
            clMainFrame::Get()->GetMainBook()->OpenFile(fn.GetFullPath(), project, -1);
        }
    }
}

void FileViewTree::OnRemoveProject(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(data->GetData().GetKind() == ProjectItem::TypeProject) { DoRemoveProject(data->GetData().GetDisplayName()); }
    }
}

void FileViewTree::OnSortItem(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = GetSingleSelection();
    SortItem(item);
}

bool FileViewTree::AddFilesToVirtualFolder(const wxString& vdFullPath, wxArrayString& paths)
{
    wxArrayString actualAdded;
    ManagerST::Get()->AddFilesToProject(paths, vdFullPath, actualAdded);

    // locate the item
    wxTreeItemId item = ItemByFullPath(vdFullPath);
    if(item.IsOk()) {
        for(size_t i = 0; i < actualAdded.Count(); i++) {

            // Add the tree node
            wxFileName fnFileName(actualAdded.Item(i));
            wxString path(vdFullPath);
            path += wxT(":");
            path += fnFileName.GetFullName();
            ProjectItem projItem(path, fnFileName.GetFullName(), fnFileName.GetFullPath(), ProjectItem::TypeFile);

            wxTreeItemId hti = AppendItem(item,                      // parent
                                          projItem.GetDisplayName(), // display name
                                          GetIconIndex(projItem),    // item image index
                                          GetIconIndex(projItem),    // selected item image
                                          new FilewViewTreeItemData(projItem));
            wxUnusedVar(hti);
        }

        SortItem(item);
        Expand(item);
        SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
        return true;
    }
    return false;
}

bool FileViewTree::AddFilesToVirtualFolderIntelligently(const wxString& vdFullPath, wxArrayString& paths)
{
    // Try to put .cpp files in a :src and .h files in a :include dir
    // This should only happen if :src and :include are terminal subdirs of vdFullPath itself, not distant cousins
    // Note: This function is only used atm to place a pair of cpp/h files, so I'm not checking 'paths'.
    // If you use it for anything else in the future...

    // Check first for vdFullPath being one of the subdirs:
    wxString basename, srcname, includename;
    if(!vdFullPath.EndsWith(wxT(":src"), &basename)) { vdFullPath.EndsWith(wxT(":include"), &basename); }
    if(basename.empty()) {
        basename = vdFullPath; // If not, assume that we were passed the parent folder
    }

    // Either way, basename should now contain the putative parent of src and include
    // Check by getting treeitemids for all 3
    wxTreeItemId parentitem = ItemByFullPath(basename);
    if(!parentitem.IsOk()) { return false; }
    wxTreeItemId srcitem = DoGetItemByText(parentitem, wxT("src"));
    wxTreeItemId includeitem = DoGetItemByText(parentitem, wxT("include"));
    if(!(srcitem.IsOk() && includeitem.IsOk())) {
        return false; // The alleged parent folder doesn't have a relevant matching pair of children
    }

    // We're winning. Now it's just a matter of putting the cpp file into :src, etc
    wxArrayString cppfiles, hfiles;
    for(int c = (int)paths.GetCount() - 1; c >= 0; --c) {
        wxString file = paths.Item(c);
        if(file.Right(4) == wxT(".cpp")) {
            cppfiles.Add(file);
            paths.RemoveAt(c);
        } else if((file.Right(2) == wxT(".h")) || (file.Right(4) == wxT(".hpp"))) {
            hfiles.Add(file);
            paths.RemoveAt(c);
        }
    }
    // Finally do the Adds
    AddFilesToVirtualFolder(basename + wxT(":src"), cppfiles);
    AddFilesToVirtualFolder(basename + wxT(":include"), hfiles);
    // There shouldn't have been any other files passed; but if there were, add them to the selected folder
    if(paths.GetCount()) { AddFilesToVirtualFolder(vdFullPath, paths); }

    return true;
}

bool FileViewTree::AddFilesToVirtualFolder(wxTreeItemId& item, wxArrayString& paths)
{
    if(item.IsOk() == false) return false;

    FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
    switch(data->GetData().GetKind()) {
    case ProjectItem::TypeVirtualDirectory:
        // OK
        break;
    default:
        return false;
    }

    wxArrayString actualAdded;
    wxString vdPath = GetItemPath(item);
    wxString project;
    project = vdPath.BeforeFirst(wxT(':'));
    ManagerST::Get()->AddFilesToProject(paths, vdPath, actualAdded);
    for(size_t i = 0; i < actualAdded.Count(); i++) {

        // Add the tree node
        wxFileName fnFileName(actualAdded.Item(i));
        wxString path(vdPath);
        path += wxT(":");
        path += fnFileName.GetFullName();
        ProjectItem projItem(path, fnFileName.GetFullName(), fnFileName.GetFullPath(), ProjectItem::TypeFile);

        wxTreeItemId hti = AppendItem(item,                      // parent
                                      projItem.GetDisplayName(), // display name
                                      GetIconIndex(projItem),    // item image index
                                      GetIconIndex(projItem),    // selected item image
                                      new FilewViewTreeItemData(projItem));
        wxUnusedVar(hti);
    }

    SortItem(item);
    Expand(item);
    SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
    return true;
}

void FileViewTree::OnAddExistingItem(wxCommandEvent& WXUNUSED(event))
{
    wxString start_path(wxEmptyString);
    wxTreeItemId item = GetSingleSelection();
    if(!item.IsOk()) { return; }

    const wxString ALL(
        wxT("All Files (*)|*|") wxT("C/C++ Source Files (*.c;*.cpp;*.cxx;*.cc)|*.c;*.cpp;*.cxx;*.cc|")
            wxT("C/C++ Header Files (*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc)|*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc"));

    wxString vdPath = GetItemPath(item);
    wxString project, vd;
    project = vdPath.BeforeFirst(wxT(':'));
    vd = vdPath.AfterFirst(wxT(':'));

    ProjectPtr proj = ManagerST::Get()->GetProject(project);
    start_path = proj->GetBestPathForVD(vd);

    wxArrayString paths;
    wxFileDialog dlg(this, _("Add Existing Item"), start_path, wxEmptyString, ALL,
                     wxFD_MULTIPLE | wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if(dlg.ShowModal() == wxID_OK) {
        dlg.GetPaths(paths);

        if(paths.IsEmpty() == false) {
            // keep the last used path
            wxFileName fn(paths.Item(0));
            start_path = fn.GetPath();
        }
        AddFilesToVirtualFolder(item, paths);
    }
}

void FileViewTree::OnNewItem(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = GetSingleSelection();
    if(!item.IsOk()) { return; }

    wxString path = GetItemPath(item);

    // Project name
    wxString projName = path.BeforeFirst(wxT(':'));
    wxString vd = path.AfterFirst(wxT(':'));
    wxString projCwd;
    ProjectPtr project = ManagerST::Get()->GetProject(projName);
    if(project) { projCwd = project->GetBestPathForVD(vd); }

    NewItemDlg dlg(clMainFrame::Get(), projCwd);
    dlg.SetTitle(_("New Item"));
    if(dlg.ShowModal() == wxID_OK) { DoAddNewItem(item, dlg.GetFileName().GetFullPath(), path); }
}

void FileViewTree::OnSetActive(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = GetSingleSelection();
    DoSetProjectActive(item);
}

void FileViewTree::DoSetProjectActive(wxTreeItemId& item)
{
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(data->GetData().GetKind() == ProjectItem::TypeProject) {
            UnselectAllProject(); // Clear any previously marked item
            ManagerST::Get()->SetActiveProject(data->GetData().GetDisplayName());
            wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
            f.SetStyle(wxFONTSTYLE_ITALIC);
            f.SetWeight(wxFONTWEIGHT_BOLD);
            SetItemFont(item, f);
        }
    }
}

void FileViewTree::OnRemoveVirtualFolder(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = GetSingleSelection();
    DoRemoveVirtualFolder(item);
}

void FileViewTree::OnRemoveItem(wxCommandEvent& WXUNUSED(event)) { DoRemoveItems(); }

void FileViewTree::DoRemoveItems()
{
    wxArrayTreeItemIds items;
    size_t num = GetMultiSelection(items);
    if(!num) { return; }

    bool ApplyToEachFileRemoval = false;
    bool ApplyToEachFileDeletion = false;
    bool AlsoDeleteFromDisc = false;

    // We need to get the item names and data first. Why? Because if they're projects and multiple,
    // removing the second one segs in GetItemText(item) or data->GetData() as 'item' is no longer valid.
    // Why doesn't this happen for files and VDs too? Good question :/
    wxArrayString namearray;
    wxArrayInt kindarray;
    std::vector<FilewViewTreeItemData*> itemdata;
    for(size_t i = 0; i < num; i++) {
        wxTreeItemId item = items.Item(i);
        if(item.IsOk()) {
            namearray.Add(GetItemText(item));
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            itemdata.push_back(data);
            // Store the kind too, as this would become an invalid value
            kindarray.Add(data->GetData().GetKind());
        }
    }

    wxArrayString filesRemoved;
    wxString affectedProject;
    for(size_t i = 0; i < num; i++) {
        wxTreeItemId item = items.Item(i);
        if(!item.IsOk()) { continue; }

        wxString name = namearray.Item(i);
        FilewViewTreeItemData* data = itemdata.at(i);

        if(data) {
            switch(kindarray.Item(i)) {
            case ProjectItem::TypeFile: {
                int result = wxID_YES;
                if(ApplyToEachFileRemoval == false) {
                    wxString message;
                    message << _("Are you sure you want remove '") << name << wxT("' ?");
                    if((num > 1) && ((i + 1) < num)) {

                        // For multiple selections, use a YesToAll dialog
                        wxRichMessageDialog dlg(wxTheApp->GetTopWindow(), message, _("Confirm"),
                                                wxYES_NO | wxYES_DEFAULT | wxCANCEL | wxCENTER | wxICON_QUESTION);
                        dlg.ShowCheckBox(_("Remember my answer and apply it all files"), false);
                        result = dlg.ShowModal();
                        ApplyToEachFileRemoval = dlg.IsCheckBoxChecked();

                    } else {
                        result = wxMessageBox(message, _("Are you sure?"), wxYES_NO | wxICON_QUESTION, this);
                    }
                }
                if(result == wxID_CANCEL || (result == wxID_NO && ApplyToEachFileRemoval == true)) {
                    return; // Assume Cancel or No+ApplyToEachFileRemoval means for folders etc too, not just files
                }
                if(result == wxID_YES || result == wxYES) {
                    wxTreeItemId parent = GetItemParent(item);
                    if(parent.IsOk()) {
                        wxString path = GetItemPath(parent);

                        // Remove the file. Do not fire an event here, we will send a "bulk" event
                        // with a list of all files removed
                        wxString fullpathOfFileRemoved;
                        CL_DEBUG("File removed %s", path);
                        if(affectedProject.IsEmpty()) { affectedProject = path.BeforeFirst(wxT(':')); }

                        if(ManagerST::Get()->RemoveFile(data->GetData().GetFile(), path, fullpathOfFileRemoved,
                                                        false)) {
                            filesRemoved.Add(fullpathOfFileRemoved);
                        }

                        wxString file_name(data->GetData().GetFile());
                        Delete(item);
                        SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);

                        int DeleteThisItemFromDisc = false;
                        if(ApplyToEachFileDeletion == false) {
                            wxString message;
                            message << _("Do you also want to delete the file '") << name << _("' from disc?");
                            if((num > 1) && ((i + 1) < num)) {
                                // For multiple selections, use a YesToAll dialog
                                wxRichMessageDialog dlg(EventNotifier::Get()->TopFrame(), message, _("Confirm"),
                                                        wxYES_NO | wxNO_DEFAULT | wxCANCEL | wxCENTER |
                                                            wxICON_QUESTION);
                                dlg.ShowCheckBox(_("Remember my answer and apply it all files"), false);
                                DeleteThisItemFromDisc = dlg.ShowModal();
                                ApplyToEachFileDeletion = dlg.IsCheckBoxChecked();
                            } else {
                                DeleteThisItemFromDisc = ::PromptForYesNoCancelDialogWithCheckbox(
                                    message, "fileview_del_file_from_disc", _("Yes"), _("No"), _("Cancel"),
                                    _("Remember my answer and don't ask me again"),
                                    wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
                            }
                        }

                        if((DeleteThisItemFromDisc == wxID_YES || DeleteThisItemFromDisc == wxYES) ||
                           AlsoDeleteFromDisc) {
                            AlsoDeleteFromDisc = ApplyToEachFileDeletion; // If we're here, ApplyToAll means delete all

                            wxString message(_("An error occurred during file removal. Maybe it has been already "
                                               "deleted or you don't have the necessary permissions"));
                            if(wxDirExists(name)) {
                                if(!wxFileName::Rmdir(name, wxPATH_RMDIR_RECURSIVE)) {
                                    wxMessageBox(message, _("Error"), wxOK | wxICON_ERROR, this);
                                } else {
                                    // Folder was removed from the disc, notify about it
                                    clFileSystemEvent rmEvent(wxEVT_FOLDER_DELETED);
                                    rmEvent.GetPaths().Add(name);
                                    rmEvent.SetEventObject(this);
                                    EventNotifier::Get()->AddPendingEvent(rmEvent);
                                }
                            } else {
                                if(wxFileName::FileExists(file_name)) {
                                    if(!wxRemoveFile(file_name)) {
                                        wxMessageBox(message, _("Error"), wxOK | wxICON_ERROR, this);
                                    } else {
                                        // File was removed from the disc, notify about it
                                        clFileSystemEvent rmEvent(wxEVT_FILE_DELETED);
                                        rmEvent.GetPaths().Add(file_name);
                                        rmEvent.SetEventObject(this);
                                        EventNotifier::Get()->AddPendingEvent(rmEvent);
                                    }
                                }
                            }
                        }
                    }
                }
            } break;
            case ProjectItem::TypeVirtualDirectory:
                DoRemoveVirtualFolder(item);
                break;
            case ProjectItem::TypeProject:
                DoRemoveProject(name);
                break;
            default:
                break;
            }
        }
    }

    // Notify plugins if we actually removed files
    if(!filesRemoved.IsEmpty()) {
        clCommandEvent evtFileRemoved(wxEVT_PROJ_FILE_REMOVED);
        evtFileRemoved.SetStrings(filesRemoved);   // list of files removed
        evtFileRemoved.SetString(affectedProject); // the affected project
        evtFileRemoved.SetEventObject(this);
        EventNotifier::Get()->ProcessEvent(evtFileRemoved);
    }
}

void FileViewTree::DoRemoveVirtualFolder(wxTreeItemId& item)
{
    wxString name = GetItemText(item);
    wxString message(wxT("'") + name + wxT("'"));
    message << _(" and all its contents will be removed from the project.");

    if(wxMessageBox(message, _("CodeLite"), wxYES_NO | wxICON_WARNING) == wxYES) {
        wxString path = GetItemPath(item);
        ManagerST::Get()->RemoveVirtualDirectory(path);

        DeleteChildren(item);
        Delete(item);
        SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
    }
}

void FileViewTree::OnNewVirtualFolder(wxCommandEvent& WXUNUSED(event))
{
    static int count = 0;
    wxString defaultName(wxT("NewDirectory"));
    defaultName << count++;

    wxTreeItemId item = GetSingleSelection();
    NewVirtualFolderDlg dlg(clMainFrame::Get(), GetItemPath(item));
    if(dlg.ShowModal() == wxID_OK) {
        DoAddVirtualFolder(item, dlg.GetName());
        if(dlg.GetCreateOnDisk()) {
            // Create the path on the file system, but don't complain if it is already there
            wxFileName::Mkdir(dlg.GetDiskPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }
    }
}

wxTreeItemId FileViewTree::DoAddVirtualFolder(wxTreeItemId& parent, const wxString& text)
{
    wxString path = GetItemPath(parent) + wxT(":") + text;

    // Virtual directory already exists?
    if(ManagerST::Get()->AddVirtualDirectory(path, true) == Manager::VD_EXISTS) return wxTreeItemId();

    wxTreeItemId item;
    ProjectItem itemData(path, text, wxEmptyString, ProjectItem::TypeVirtualDirectory);
    item = AppendItem(parent,                    // parent
                      itemData.GetDisplayName(), // display name
                      GetIconIndex(itemData),    // item image index
                      GetIconIndex(itemData),    // selected item image
                      new FilewViewTreeItemData(itemData));

    SortItem(parent);
    Expand(parent);
    SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
    return item;
}

wxString FileViewTree::GetItemPath(const wxTreeItemId& item) const
{
    std::deque<wxString> queue;
    wxString text = GetItemText(item);
    queue.push_front(text);

    wxTreeItemId p = GetItemParent(item);
    while(true) {

        if(!p.IsOk() || p == GetRootItem()) break;

        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(p));
        if(!data) break;

        if(data->GetData().GetKind() == ProjectItem::TypeWorkspaceFolder) {
            // We reached the top level
            break;
        }

        text = GetItemText(p);
        queue.push_front(text);
        p = GetItemParent(p);
    }

    wxString path;
    size_t count = queue.size();
    for(size_t i = 0; i < count; i++) {
        path += queue.front();
        path += wxT(":");
        queue.pop_front();
    }

    if(!queue.empty()) {
        path += queue.front();
    } else {
        path = path.BeforeLast(wxT(':'));
    }

    return path;
}

void FileViewTree::OnLocalPrefs(wxCommandEvent& event)
{
    if(!ManagerST::Get()->IsWorkspaceOpen()) {
        return; // Probably not possible, but...
    }

    wxXmlNode* lwsnode = LocalWorkspaceST::Get()->GetLocalWorkspaceOptionsNode();
    // Don't check lwsnode: it'll be NULL if there are currently no local workspace options

    // Start by getting the global settings
    OptionsConfigPtr higherOptions = EditorConfigST::Get()->GetOptions();

    // If we're setting workspace options, run the dialog and return
    if(event.GetId() == XRCID("local_workspace_prefs")) {
        EditorSettingsLocal dlg(higherOptions, lwsnode, pLevel_workspace, this);
        if(dlg.ShowModal() == wxID_OK && LocalWorkspaceST::Get()->SetWorkspaceOptions(dlg.GetLocalOpts())) {
            clMainFrame::Get()->GetMainBook()->ApplySettingsChanges();
            // Notify plugins that some settings have changed
            PostCmdEvent(wxEVT_EDITOR_SETTINGS_CHANGED);
        }
        return;
    }

    // Otherwise we're getting project prefs
    wxTreeItemId item = GetSingleSelection();
    if(!item.IsOk()) { return; }

    wxXmlNode* lpnode = LocalWorkspaceST::Get()->GetLocalProjectOptionsNode(GetItemText(item));
    // Don't check lpnode: it'll be NULL if there are currently no local project options
    // Merge any local workspace options with the global ones inside 'higherOptions'
    LocalOptionsConfig wsOC(higherOptions, lwsnode);

    EditorSettingsLocal dlg(higherOptions, lpnode, pLevel_project, this);
    if(dlg.ShowModal() == wxID_OK &&
       LocalWorkspaceST::Get()->SetProjectOptions(dlg.GetLocalOpts(), GetItemText(item))) {
        clMainFrame::Get()->GetMainBook()->ApplySettingsChanges();
        // Notify plugins that some settings have changed
        PostCmdEvent(wxEVT_EDITOR_SETTINGS_CHANGED);
    }
}

void FileViewTree::OnProjectProperties(wxCommandEvent& WXUNUSED(event))
{
    ProjectPtr p = GetSelectedProject();
    if(p) { clMainFrame::Get()->GetWorkspaceTab()->OpenProjectSettings(p->GetName()); }
}

void FileViewTree::DoRemoveProject(const wxString& name)
{
    wxString message(_("You are about to remove project '"));
    message << name << wxT("' ");
    message << _(" from the workspace, click 'Yes' to proceed or 'No' to abort.");
    if(wxMessageBox(message, _("Confirm"), wxYES_NO) == wxYES) {
        ManagerST::Get()->RemoveProject(name, true);

        // Remove the project from the cache
        if(m_projectsMap.count(name)) { m_projectsMap.erase(name); }
    }
}

int FileViewTree::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
    // used for SortChildren, reroute to our sort routine
    FilewViewTreeItemData *a = (FilewViewTreeItemData*)GetItemData(item1),
                          *b = (FilewViewTreeItemData*)GetItemData(item2);
    if(a && b) return OnCompareItems(a, b);

    return 0;
}

int FileViewTree::OnCompareItems(const FilewViewTreeItemData* a, const FilewViewTreeItemData* b)
{
    // if dir and other is not, dir has preference
    if(a->GetData().GetKind() == ProjectItem::TypeVirtualDirectory && b->GetData().GetKind() == ProjectItem::TypeFile)
        return -1;
    else if(b->GetData().GetKind() == ProjectItem::TypeVirtualDirectory &&
            a->GetData().GetKind() == ProjectItem::TypeFile)
        return 1;

    // else let ascii fight it out
    return a->GetData().GetDisplayName().CmpNoCase(b->GetData().GetDisplayName());
}

void FileViewTree::OnSaveAsTemplate(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        wxString name = GetItemText(item);
        ProjectPtr proj = ManagerST::Get()->GetProject(name);
        if(proj) {
            NameAndDescDlg dlg(clMainFrame::Get(), PluginManager::Get(), name);
            if(dlg.ShowModal() == wxID_OK) {
                wxString newName = dlg.GetName();
                wxString desc = dlg.GetDescription();
                wxString type = dlg.GetType();

                newName = newName.Trim().Trim(false);
                desc = desc.Trim().Trim(false);

                if(!newName.IsEmpty()) {

                    // Copy the user template to the user folder
                    wxFileName userTemplates(clStandardPaths::Get().GetUserDataDir(), "");
                    userTemplates.AppendDir("templates");
                    userTemplates.AppendDir("projects");
                    userTemplates.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

                    Project newProj(*proj);
                    newProj.SetProjectInternalType(type);
                    newProj.CopyTo(userTemplates.GetPath(), newName, desc);
                }
            }
        }
    }
}

void FileViewTree::OnBuildOrder(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        DependenciesDlg dlg(clMainFrame::Get(), GetItemText(item));
        dlg.ShowModal();
    }
}

void FileViewTree::OnClean(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        wxString projectName = GetItemText(item);
        wxString conf;

        // get the selected configuration to be built
        BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) { conf = bldConf->GetName(); }
        QueueCommand buildInfo(projectName, conf, false, QueueCommand::kClean);

        if(bldConf && bldConf->IsCustomBuild()) {
            buildInfo.SetKind(QueueCommand::kCustomBuild);
            buildInfo.SetCustomBuildTarget(wxT("Clean"));
        }
        ManagerST::Get()->PushQueueCommand(buildInfo);
        ManagerST::Get()->ProcessCommandQueue();
    }
}

void FileViewTree::OnBuild(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        wxString projectName = GetItemText(item);

        wxString conf;
        // get the selected configuration to be built
        BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) { conf = bldConf->GetName(); }

        QueueCommand buildInfo(projectName, conf, false, QueueCommand::kBuild);
        if(bldConf && bldConf->IsCustomBuild()) {
            buildInfo.SetKind(QueueCommand::kCustomBuild);
            buildInfo.SetCustomBuildTarget(wxT("Build"));
        }
        ManagerST::Get()->PushQueueCommand(buildInfo);
        ManagerST::Get()->ProcessCommandQueue();
    }
}

void FileViewTree::OnCompileItem(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(data->GetData().GetKind() == ProjectItem::TypeFile) {
            Manager* mgr = ManagerST::Get();
            wxTreeItemId parent = GetItemParent(item);
            if(parent.IsOk()) {
                wxString logmsg;
                wxString path = GetItemPath(parent);
                wxString proj = path.BeforeFirst(wxT(':'));
                logmsg << _("Compiling file: ") << data->GetData().GetFile() << _(" of project ") << proj << wxT("\n");
                mgr->CompileFile(proj, data->GetData().GetFile());
            }
        }
    }
}

void FileViewTree::OnPreprocessItem(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(data->GetData().GetKind() == ProjectItem::TypeFile) {
            Manager* mgr = ManagerST::Get();
            wxTreeItemId parent = GetItemParent(item);
            if(parent.IsOk()) {
                wxString logmsg;
                wxString path = GetItemPath(parent);
                wxString proj = path.BeforeFirst(wxT(':'));
                logmsg << _("Preprocessing file: ") << data->GetData().GetFile() << _(" of project ") << proj
                       << wxT("\n");
                mgr->CompileFile(proj, data->GetData().GetFile(), true);
            }
        }
    }
}

void FileViewTree::OnStopBuild(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ManagerST::Get()->StopBuild();
}

void FileViewTree::OnItemActivated(wxTreeEvent& event)
{
    if(event.GetKeyCode() == WXK_RETURN) {
        wxArrayTreeItemIds items;
        size_t num = GetMultiSelection(items);
        if(num > 0) {
            for(size_t i = 0; i < num; i++) {
                wxTreeItemId item = items.Item(i);
                DoItemActivated(item, event);
            }
        }
    } else if(event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_NUMPAD_DELETE) {
        DoRemoveItems();
    } else {
        event.Skip();
    }
}

size_t FileViewTree::GetMultiSelection(wxArrayTreeItemIds& arr)
{
    if(HasFlag(wxTR_MULTIPLE)) {
        // we are using multiple selection tree
        return GetSelections(arr);
    } else {
        wxTreeItemId sel = GetSelection();
        if(sel.IsOk()) {
            arr.Add(sel);
            return 1;
        }
        arr.Clear();
        return 0;
    }
}

void FileViewTree::OnRetagProject(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        wxString projectName = GetItemText(item);
        ManagerST::Get()->RetagProject(projectName, true);
    }
}

void FileViewTree::OnRetagWorkspace(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ManagerST::Get()->RetagWorkspace(TagsManager::Retag_Quick);
}

void FileViewTree::OnItemBeginDrag(wxTreeEvent& event)
{
    wxArrayTreeItemIds selections;
    size_t num = GetMultiSelection(selections);

    // We allow dragging of homogenous items (all files or all projects)
    m_draggedFiles.Clear();
    m_draggedProjects.Clear();
    for(size_t n = 0; n < num; ++n) {
        wxTreeItemId item = selections[n];
        if(item.IsOk() && item != GetRootItem()) {
            // If it's a file, add it to the array
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            if(data && data->GetData().GetKind() == ProjectItem::TypeFile) {
                m_draggedFiles.Add(item);
            } else if(data && data->GetData().GetKind() == ProjectItem::TypeProject) {
                m_draggedProjects.Add(item);
            }
        }
    }

    // Allow the event only if there were any valid selections
    if(m_draggedFiles.IsEmpty() && !m_draggedProjects.IsEmpty()) {
        // only projects are being dragged
        event.Allow();
    } else if(!m_draggedFiles.IsEmpty() && m_draggedProjects.IsEmpty()) {
        // only files are being dragged
        event.Allow();
    }
}

void FileViewTree::OnItemEndDrag(wxTreeEvent& event)
{
    wxTreeItemId itemDst = event.GetItem();
    CHECK_ITEM_RET(itemDst);

    if(!m_draggedFiles.IsEmpty()) {
        // Files were being dragged
        DoFilesEndDrag(itemDst);

    } else if(!m_draggedProjects.IsEmpty()) {
        // Projects were being dragged
        DoProjectsEndDrag(itemDst);
    }
}

void FileViewTree::OnBuildProjectOnly(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();

    if(item.IsOk()) {
        wxString projectName = GetItemText(item);
        wxCommandEvent e(wxEVT_CMD_BUILD_PROJECT_ONLY);
        e.SetString(projectName);
        EventNotifier::Get()->AddPendingEvent(e);
    }
}

void FileViewTree::OnCleanProjectOnly(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();

    if(item.IsOk()) {
        wxString projectName = GetItemText(item);
        wxCommandEvent e(wxEVT_CMD_CLEAN_PROJECT_ONLY);
        e.SetString(projectName);
        EventNotifier::Get()->AddPendingEvent(e);
    }
}

void FileViewTree::ExpandToPath(const wxString& project, const wxFileName& fileName)
{
    if(m_projectsMap.count(project)) {
        wxTreeItemId child = m_projectsMap.find(project)->second;
        FilewViewTreeItemData* childData = static_cast<FilewViewTreeItemData*>(GetItemData(child));
        if(childData->GetData().GetDisplayName() == project) {
            wxTreeItemId fileItem =
                fileName.GetName().IsEmpty()
                    ? child
                    : FindItemByPath(child, ManagerST::Get()->GetProjectCwd(project), fileName.GetFullPath());
            if(fileItem.IsOk()) {
                // Now we're using a wxTR_MULTIPLE tree, we need to unselect here, otherwise all project files get
                // selected
                // And,no, SelectItem(fileItem, false) isn't the answer: in 2.8 it toggles (a wx bug) and the 'selected'
                // tab ends up unselected
                if(HasFlag(wxTR_MULTIPLE)) { UnselectAll(); }

                SelectItem(fileItem);

                if(IsVisible(fileItem) == false) { EnsureVisible(fileItem); }
            } else {
                wxString message;
                message << _("Failed to find file: ") << fileName.GetFullPath() << _(" in FileView.");
                wxLogMessage(message);
            }
        }
    }
}

wxTreeItemId FileViewTree::FindItemByPath(wxTreeItemId& projectHTI, const wxString& projectPath,
                                          const wxString& fileName)
{
    if(!projectHTI.IsOk()) { return wxTreeItemId(); }

    if(!ItemHasChildren(projectHTI)) return wxTreeItemId();
    wxString projectName = clCxxWorkspaceST::Get()->GetProjectFromFile(fileName);
    ProjectPtr proj = clCxxWorkspaceST::Get()->GetProject(projectName);
    if(!proj) return wxTreeItemId();

    wxString vdFullPath = proj->GetVDByFileName(fileName);

#if defined(__WXGTK__)
    wxString realpathItem = CLRealPath(fileName);
#endif

    wxTreeItemId curItem = projectHTI;
    wxArrayString dirs = ::wxStringTokenize(vdFullPath, ":", wxTOKEN_STRTOK);
    for(size_t i = 0; i < dirs.size(); ++i) {

        // Ensure that the 'curItem' is expanded before we loop
        DoBuildSubTreeIfNeeded(curItem);

        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(curItem, cookie);
        bool matchFound = false;
        while(child.IsOk()) {
            wxString name = GetItemText(child);
            if(name == dirs.Item(i)) {
                matchFound = true;
                curItem = child;
                break;
            }
            child = GetNextChild(curItem, cookie);
        }
        if(!matchFound) {
            curItem = wxTreeItemId();
            break;
        }
    }

    if(curItem.IsOk()) {
        DoBuildSubTreeIfNeeded(curItem);

        // We found the virtual folder that should contain the filename
        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(curItem, cookie);
        while(child.IsOk()) {
            FilewViewTreeItemData* childData = static_cast<FilewViewTreeItemData*>(GetItemData(child));
            wxFileName fn(childData->GetData().GetFile());
            fn.MakeAbsolute(projectPath);
            if(fn.GetFullPath().CmpNoCase(fileName) == 0) {
                return child;
            }
#if defined(__WXGTK__)
            else { // Try again, dereferencing fn
                wxString fdest = CLRealPath(fn.GetFullPath());
                if(fdest.CmpNoCase(realpathItem) == 0) { return child; }
            }
#endif
            child = GetNextChild(curItem, cookie);
        }
    }
    return wxTreeItemId();
}

wxTreeItemId FileViewTree::ItemByFullPath(const wxString& fullPath)
{
    if(!ItemHasChildren(GetRootItem())) return wxTreeItemId();

    // Locate the project item
    wxString projname = fullPath.BeforeFirst(':');
    if(m_projectsMap.count(projname) == 0) return wxTreeItemId();

    wxTreeItemId parent = GetItemParent(m_projectsMap[projname]);
    wxArrayString texts = wxStringTokenize(fullPath, wxT(":"), wxTOKEN_STRTOK);
    for(size_t i = 0; i < texts.GetCount(); i++) {
        parent = DoGetItemByText(parent, texts.Item(i));
        if(parent.IsOk() == false) { return wxTreeItemId(); }
    }
    return parent;
}

void FileViewTree::OnImportDirectory(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxTreeItemId item = GetSingleSelection();
    if(!item.IsOk()) { return; }

    wxString vdPath = GetItemPath(item);
    wxString project;
    project = vdPath.BeforeFirst(wxT(':'));
    ProjectPtr proj = ManagerST::Get()->GetProject(project);

    bool extlessFiles(false);
    wxStringBoolMap_t dirs;
    wxArrayString files;
    wxArrayString all_files;
    wxString filespec;

    ImportFilesDialogNew dlg(clMainFrame::Get());
    if(dlg.ShowModal() != wxID_OK) return;

    extlessFiles = dlg.ExtlessFiles();
    dlg.GetDirectories(dirs);
    filespec = dlg.GetFileMask();

    // get list of all files based on the checked directories
    wxStringBoolMap_t::const_iterator iter = dirs.begin();
    for(; iter != dirs.end(); ++iter) {
        int flags = iter->second ? (wxDIR_FILES | wxDIR_DIRS) : (wxDIR_FILES);
        wxDir::GetAllFiles(iter->first, &all_files, "", flags);
    }

    DoImportFolder(proj, dlg.GetBaseDir(), all_files, filespec, extlessFiles);
}

void FileViewTree::DoImportFolder(ProjectPtr proj, const wxString& baseDir, const wxArrayString& all_files,
                                  const wxString& filespec, bool extlessFiles)
{
    wxStringTokenizer tok(filespec, wxT(";"));
    wxStringSet_t specMap;
    while(tok.HasMoreTokens()) {
        wxString v = tok.GetNextToken();
        // Cater for *.*, and also for idiots asking for *.foo;*.*
        if(v == wxT("*.*")) {
            // Remove any previous entries, and stop looking for more: an empty specMap signals *.*
            specMap.clear();
            break;
        }
        v = v.AfterLast(wxT('*'));
        v = v.AfterLast(wxT('.')).MakeLower();
        specMap.insert(v);
    }

    // filter non interesting files
    wxArrayString files;
    for(size_t i = 0; i < all_files.GetCount(); i++) {
        wxFileName fn(all_files.Item(i));

        /* always excluded by default */
        const wxArrayString& dirs = fn.GetDirs();
        bool cont = true;
        for(size_t j = 0; j < dirs.GetCount() && cont; j++) {
            wxString filepath = fn.GetPath();
            if(dirs.Item(j) == wxT(".svn") || dirs.Item(j) == wxT(".cvs") || dirs.Item(j) == wxT(".arch-ids") ||
               dirs.Item(j) == wxT("arch-inventory") || dirs.Item(j) == wxT("autom4te.cache") ||
               dirs.Item(j) == wxT("BitKeeper") || dirs.Item(j) == wxT(".bzr") || dirs.Item(j) == wxT(".bzrignore") ||
               dirs.Item(j) == wxT("CVS") || dirs.Item(j) == wxT(".cvsignore") || dirs.Item(j) == wxT("_darcs") ||
               dirs.Item(j) == wxT(".deps") || dirs.Item(j) == wxT("EIFGEN") || dirs.Item(j) == wxT(".git") ||
               dirs.Item(j) == wxT(".hg") || dirs.Item(j) == wxT("PENDING") || dirs.Item(j) == wxT("RCS") ||
               dirs.Item(j) == wxT("RESYNC") || dirs.Item(j) == wxT("SCCS") || dirs.Item(j) == wxT("{arch}")) {
                cont = false;
                break;
            }
        }

        // skip the directory?
        if(!cont) continue;

        if(specMap.empty()) {
            files.Add(all_files.Item(i));

        } else if(fn.GetExt().IsEmpty() & extlessFiles) {
            files.Add(all_files.Item(i));

        } else if(specMap.find(fn.GetExt().MakeLower()) != specMap.end()) {
            files.Add(all_files.Item(i));
        }
    }

    wxString path = baseDir;
    //{ Fixe bug 2847625
    if(path.EndsWith(wxT("/")) || path.EndsWith(wxT("\\"))) { path.RemoveLast(); } //} Fixe bug 2847625

    wxFileName rootPath(path);

    // loop over the files and construct for each file a record with
    // the following information:
    // -virtual directory (full path, starting from project level)
    // -display name
    // -full path of the file
    proj->BeginTranscation();
    {
        // Create a progress dialog
        clProgressDlg* prgDlg = new clProgressDlg(NULL, _("Importing files ..."), wxT(""), (int)files.GetCount());

        // get list of files
        for(size_t i = 0; i < files.GetCount(); i++) {
            wxFileName fn(files.Item(i));

            // if the file already exist, skip it
            if(proj->IsFileExist(fn.GetFullPath())) { continue; }

            FileViewItem fvitem;
            fvitem.fullpath = fn.GetFullPath();
            fvitem.displayName = fn.GetFullName();

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
            // relativePath.Append(wxT(":"));

            fvitem.virtualDir = relativePath;
            DoAddItem(proj, fvitem);

            wxString msg;
            msg << _("Adding file: ") << fn.GetFullPath();
            prgDlg->Update((int)i, msg);
        }
        m_itemsToSort.clear();
        prgDlg->Destroy();
    }

    // save the project file to disk
    proj->CommitTranscation();

    // Reload the view
    CallAfter(&FileViewTree::BuildTree);
}

void FileViewTree::OnReconcileProject(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxString projectName;

    // Allow the selected project to be reconciled, even if it's inactive
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        FilewViewTreeItemData* fvid = dynamic_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(fvid && fvid->GetData().GetKind() == ProjectItem::TypeProject) {
            ManagerST::Get()->ReconcileProject(fvid->GetData().GetDisplayName());
        }
    }
}

void FileViewTree::RedefineProjFiles(ProjectPtr proj, const wxString& path, std::vector<wxString>& files)
{
    wxFileName rootPath(path);

    // loop over the files and construct for each file a record with
    // the following information:
    // -virtual directory (full path, starting from project level)
    // -display name
    // -full path of the file
    proj->BeginTranscation();
    {
        // Create a progress dialog
        clProgressDlg* prgDlg = new clProgressDlg(NULL, _("Importing files ..."), wxT(""), (int)files.size());

        proj->ClearAllVirtDirs();

        wxString relativePath;
        for(size_t i = 0; i < files.size(); i++) {
            wxFileName fn(files[i]);

            FileViewItem fvitem;
            fvitem.fullpath = fn.GetFullPath();
            fvitem.displayName = fn.GetFullName();

            fn.MakeRelativeTo(path);

            // anchor all files to a base folder
            relativePath = rootPath.GetName() + wxT(":") + fn.GetPath() + wxT(":");
            relativePath.Replace(wxT("/"), wxT(":"));
            relativePath.Replace(wxT("\\"), wxT(":"));

            fvitem.virtualDir = relativePath;
            DoAddItem(proj, fvitem);

            wxString msg;
            msg << _("Adding file: ") << fn.GetFullPath();
            prgDlg->Update((int)i, msg);
        }
        m_itemsToSort.clear();
        prgDlg->Destroy();
    }

    // save the project file to disk
    proj->CommitTranscation();

    // reload the project
    wxString curr_proj_name(proj->GetName());
    bool was_active(ManagerST::Get()->GetActiveProjectName() == curr_proj_name);
    ManagerST::Get()->RemoveProject(proj->GetName(), false);
    ManagerST::Get()->AddProject(proj->GetFileName().GetFullPath());

    // restore the active project
    if(was_active) { MarkActive(curr_proj_name); }
}

void FileViewTree::DoAddItem(ProjectPtr proj, const FileViewItem& item)
{
    if(!proj) { return; }

    // first add the virtual directory, if it already exist,
    // this function does nothing
    proj->CreateVirtualDir(item.virtualDir, true);

    // add the file.
    // For performance reasons, we dont go through the Workspace API
    // but directly through the project API
    proj->FastAddFile(item.fullpath, item.virtualDir);
}

void FileViewTree::OnRunPremakeStep(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        wxString projectName = GetItemText(item);
        ManagerST::Get()->RunCustomPreMakeCommand(projectName);
    }
}

void FileViewTree::OnRenameItem(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(data->GetData().GetKind() == ProjectItem::TypeFile) {
            wxTreeItemId parent = GetItemParent(item);
            if(parent.IsOk()) {

                wxString path = GetItemPath(parent);
                wxString proj = path.BeforeFirst(wxT(':'));

                ProjectPtr p = ManagerST::Get()->GetProject(proj);
                if(p) {
                    // prompt user for new name
                    wxString newName = wxGetTextFromUser(_("New file name:"), _("Rename file:"), GetItemText(item));
                    if(newName.IsEmpty() == false) {

                        wxFileName tmp(data->GetData().GetFile());
                        tmp.SetFullName(newName);

                        if(tmp.FileExists()) {
                            wxMessageBox(_("A File with that name already exists!"), _("CodeLite"),
                                         wxICON_WARNING | wxOK);
                            return;
                        }

                        // rename the file (this will erase it from the symbol database and will
                        // also close the editor that it is currently opened in (if any))
                        if(ManagerST::Get()->RenameFile(data->GetData().GetFile(), tmp.GetFullPath(), path)) {
                            // update the item's info
                            data->SetDisplayName(tmp.GetFullName());
                            data->SetFile(tmp.GetFullPath());

                            // rename the tree item
                            SetItemText(item, tmp.GetFullName());

                            SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
                        }
                    }
                } // p
            }     // parent.IsOk()
        }         // TypeFile
    }             // item.IsOk()
}

void FileViewTree::OnRenameVirtualFolder(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        // got the item, prompt user for new name
        wxString newName =
            wxGetTextFromUser(_("New virtual folder name:"), _("Rename virtual folder:"), GetItemText(item));
        if(newName.IsEmpty() || newName == GetItemText(item)) {
            // user clicked cancel
            return;
        }

        // locate the project
        wxString path = GetItemPath(item);
        wxString proj = path.BeforeFirst(wxT(':'));

        path = path.AfterFirst(wxT(':'));
        ProjectPtr p = ManagerST::Get()->GetProject(proj);
        if(!p) {
            wxLogMessage(_("failed to rename virtual folder: ") + path + _(", reason: could not locate project ") +
                         proj);
            return;
        }

        if(!p->RenameVirtualDirectory(path, newName)) {
            wxLogMessage(_("failed to rename virtual folder: ") + path);
            return;
        }
        SetItemText(item, newName);
        SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
    }
}

void FileViewTree::OnReBuild(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        wxString projectName = GetItemText(item);

        wxString conf;
        // get the selected configuration to be built
        BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) { conf = bldConf->GetName(); }

        // Custom build supports the 'Rebuild' target
        if(bldConf && bldConf->IsCustomBuild()) {
            QueueCommand buildInfo(projectName, conf, false, QueueCommand::kRebuild);
            if(bldConf && bldConf->IsCustomBuild()) {
                buildInfo.SetKind(QueueCommand::kCustomBuild);
                buildInfo.SetCustomBuildTarget(wxT("Rebuild"));
            }

            ManagerST::Get()->PushQueueCommand(buildInfo);
            ManagerST::Get()->ProcessCommandQueue();

        } else {
            clMainFrame::Get()->RebuildProject(projectName);
        }
    }
}

wxTreeItemId FileViewTree::DoGetItemByText(const wxTreeItemId& parent, const wxString& text)
{
    if(!parent.IsOk()) { return wxTreeItemId(); }

    if(!ItemHasChildren(parent)) { return wxTreeItemId(); }

    wxTreeItemIdValue cookie;
    wxTreeItemId child = GetFirstChild(parent, cookie);
    while(child.IsOk()) {
        if(GetItemText(child) == text) { return child; }
        child = GetNextChild(parent, cookie);
    }
    return wxTreeItemId();
}

bool FileViewTree::CreateVirtualDirectory(const wxString& parentPath, const wxString& vdName)
{
    // try to locate that VD first, if it exists, do nothing
    wxTreeItemId item = ItemByFullPath(wxString::Format(wxT("%s:%s"), parentPath.c_str(), vdName.c_str()));
    if(item.IsOk()) { return true; }

    wxString project = parentPath.BeforeFirst(wxT(':'));
    wxString parentVDs = parentPath.AfterFirst(wxT(':'));
    wxArrayString vds = ::wxStringTokenize(parentVDs, wxT(":"), wxTOKEN_STRTOK);

    wxTreeItemId curItem = ItemByFullPath(project);
    if(!curItem.IsOk())
        // Could not locate the project item...
        return false;

    wxString path = project;
    for(size_t i = 0; i < vds.GetCount(); i++) {
        path << wxT(":") << vds.Item(i);
        wxTreeItemId tmpItem = ItemByFullPath(path);
        if(!tmpItem.IsOk()) {
            curItem = DoAddVirtualFolder(curItem, vds.Item(i));
            if(curItem.IsOk() == false) {
                // failed to add virtual directory
                break;
            }
        } else {
            curItem = tmpItem;
        }
    }

    if(!curItem.IsOk()) return false;

    DoAddVirtualFolder(curItem, vdName);
    return true;
}

void FileViewTree::MarkActive(const wxString& projectName)
{
    // Find the project item ID by name and select it
    if(m_projectsMap.count(projectName)) { DoSetProjectActive(m_projectsMap.find(projectName)->second); }
}

bool FileViewTree::CreateAndAddFile(const wxString& filename, const wxString& vdFullPath)
{
    wxTreeItemId item = ItemByFullPath(vdFullPath);
    return DoAddNewItem(item, filename, vdFullPath);
}

bool FileViewTree::DoAddNewItem(wxTreeItemId& item, const wxString& filename, const wxString& vdFullpath)
{
    if(item.IsOk() == false) { return false; }

    ManagerST::Get()->AddNewFileToProject(filename, vdFullpath);

    // Add the tree node
    wxFileName fnFileName(filename);
    wxString path(vdFullpath);

    path += wxT(":");
    path += fnFileName.GetFullName();
    ProjectItem projItem(path, fnFileName.GetFullName(), fnFileName.GetFullPath(), ProjectItem::TypeFile);

    wxTreeItemId hti = AppendItem(item,                      // parent
                                  projItem.GetDisplayName(), // display name
                                  GetIconIndex(projItem),    // item image index
                                  GetIconIndex(projItem),    // selected item image
                                  new FilewViewTreeItemData(projItem));
    wxUnusedVar(hti);
    SortItem(item);
    Expand(item);
    SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
    return true;
}

void FileViewTree::OnRebuildProjectOnly(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        wxString projectName = GetItemText(item);

        wxString conf;
        // get the selected configuration to be built
        BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) { conf = bldConf->GetName(); }

        QueueCommand info(projectName, conf, true, QueueCommand::kRebuild);
        if(bldConf && bldConf->IsCustomBuild()) {
            info.SetKind(QueueCommand::kCustomBuild);
            info.SetCustomBuildTarget(wxT("Rebuild"));
        }

        ManagerST::Get()->PushQueueCommand(info);
        ManagerST::Get()->ProcessCommandQueue();
    }
}

void FileViewTree::OnLocalWorkspaceSettings(wxCommandEvent& e)
{
    if(ManagerST::Get()->IsWorkspaceOpen()) {
        WorkspaceSettingsDlg dlg(clMainFrame::Get(), LocalWorkspaceST::Get());
        if(dlg.ShowModal() == wxID_OK) {
            clMainFrame::Get()->SelectBestEnvSet();
            // Update the new paths
            ManagerST::Get()->UpdateParserPaths(true);
        }
    }
}

void FileViewTree::OnRetagInProgressUI(wxUpdateUIEvent& event)
{
    event.Enable(!ManagerST::Get()->GetRetagInProgress());
}

void FileViewTree::OnOpenWithDefaultApplication(wxCommandEvent& event)
{
    wxArrayTreeItemIds items;
    GetMultiSelection(items);

    wxMimeTypesManager* mgr = wxTheMimeTypesManager;
    for(size_t i = 0; i < items.GetCount(); i++) {
        wxTreeItemId item = items.Item(i);
        FilewViewTreeItemData* itemData = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(itemData && itemData->GetData().GetKind() == ProjectItem::TypeFile) {

            wxFileName fn(itemData->GetData().GetFile());
            wxFileType* type = mgr->GetFileTypeFromExtension(fn.GetExt());
            bool bFoundCommand = false;
            wxUnusedVar(bFoundCommand);

            if(type) {

                wxString cmd = type->GetOpenCommand(fn.GetFullPath());
                delete type;

                if(!cmd.IsEmpty()) {
                    bFoundCommand = true;
                    wxExecute(cmd);
                }
            }

#ifdef __WXGTK__
            if(!bFoundCommand && itemData && itemData->GetData().GetKind() == ProjectItem::TypeFile) {
                // All hell break loose, try xdg-open
                wxString cmd = wxString::Format(wxT("xdg-open \"%s\""), fn.GetFullPath().c_str());
                wxExecute(cmd);
            }
#endif
        }
    }
}

ProjectPtr FileViewTree::GetSelectedProject() const
{
    wxArrayTreeItemIds selections;
    size_t count = GetSelections(selections);
    if(count == 0) { return NULL; }

    // Return the first item which is of type "TypeProject"
    for(size_t i = 0; i < count; ++i) {
        FilewViewTreeItemData* itemData = dynamic_cast<FilewViewTreeItemData*>(GetItemData(selections.Item(i)));
        if(itemData && (itemData->GetData().GetKind() == ProjectItem::TypeProject)) {
            return clCxxWorkspaceST::Get()->GetProject(GetItemText(selections.Item(i)));
        }
    }

    // None of the selected items is a project.
    // Return the project parent of the first item
    wxTreeItemId item = selections.Item(0);
    while(item.IsOk() && (item != GetRootItem())) {
        FilewViewTreeItemData* itemData = dynamic_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(!itemData) {
            return NULL;
        } else if(itemData->GetData().GetKind() == ProjectItem::TypeProject) {
            // Found a project
            return clCxxWorkspaceST::Get()->GetProject(GetItemText(item));
        }
        item = GetItemParent(item);
    }

    // No project was found
    return NULL;
}

void FileViewTree::OnBuildTree(wxCommandEvent& e)
{
    e.Skip();
    BuildTree();
}

void FileViewTree::OnBuildProjectOnlyInternal(wxCommandEvent& e)
{
    e.Skip();
    wxString projectName = e.GetString();
    if(projectName.IsEmpty()) { projectName = ManagerST::Get()->GetActiveProjectName(); }

    wxString conf;
    // get the selected configuration to be built
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
    if(bldConf) { conf = bldConf->GetName(); }

    QueueCommand info(projectName, conf, true, QueueCommand::kBuild);
    if(bldConf && bldConf->IsCustomBuild()) {
        info.SetKind(QueueCommand::kCustomBuild);
        info.SetCustomBuildTarget(wxT("Build"));
    }
    ManagerST::Get()->PushQueueCommand(info);
    ManagerST::Get()->ProcessCommandQueue();
}

void FileViewTree::OnCleanProjectOnlyInternal(wxCommandEvent& e)
{
    e.Skip();
    wxString projectName = e.GetString();
    if(projectName.IsEmpty()) { projectName = ManagerST::Get()->GetActiveProjectName(); }

    wxString conf;
    // get the selected configuration to be built
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
    if(bldConf) { conf = bldConf->GetName(); }

    QueueCommand info(projectName, conf, true, QueueCommand::kClean);
    if(bldConf && bldConf->IsCustomBuild()) {
        info.SetKind(QueueCommand::kCustomBuild);
        info.SetCustomBuildTarget(wxT("Clean"));
    }

    ManagerST::Get()->PushQueueCommand(info);
    ManagerST::Get()->ProcessCommandQueue();
}

void FileViewTree::OnExcludeFromBuild(wxCommandEvent& e)
{
    wxUnusedVar(e);

    wxArrayTreeItemIds selections;
    size_t count = GetSelections(selections);
    for(size_t selectionIndex = 0; selectionIndex < count; selectionIndex++) {
        wxTreeItemId item = selections[selectionIndex];
        if(item.IsOk()) {
            FilewViewTreeItemData* data = ItemData(item);
            if(data && data->GetData().IsFile()) {
                const ProjectItem& pi = data->GetData();
                ProjectPtr proj = ManagerST::Get()->GetProject(pi.Key().BeforeFirst(':'));
                if(proj) {
                    if(e.IsChecked()) {
                        proj->AddExcludeConfigForFile(pi.GetFile());
                        ExcludeFileFromBuildUI(item, true);

                    } else {
                        proj->RemoveExcludeConfigForFile(pi.GetFile());
                        ExcludeFileFromBuildUI(item, false);
                    }
                }
            }
        }
    }
}

void FileViewTree::OnExcludeFromBuildUI(wxUpdateUIEvent& event)
{
    // by default enable it
    event.Check(IsFileExcludedFromBuild(GetSingleSelection()));
}

bool FileViewTree::IsFileExcludedFromBuild(const wxTreeItemId& item) const
{
    if(item.IsOk() && clCxxWorkspaceST::Get()->IsOpen()) {
        FilewViewTreeItemData* data = ItemData(item);
        const ProjectItem& pi = data->GetData();
        if(pi.IsFile()) {
            wxString projectName = pi.Key().BeforeFirst(':');
            if(!projectName.IsEmpty()) {
                ProjectPtr proj = clCxxWorkspaceST::Get()->GetProject(projectName);
                if(proj) { return proj->IsFileExcludedFromConfig(pi.GetFile()); }
            }
        }
    }
    return false;
}

void FileViewTree::OnSelectionChanged(wxTreeEvent& e)
{
    e.Skip();
    if(!e.GetItem().IsOk()) return;

    FilewViewTreeItemData* data = dynamic_cast<FilewViewTreeItemData*>(GetItemData(e.GetItem()));
    if(data && data->GetData().GetKind() == ProjectItem::TypeProject) {
        wxCommandEvent evtProjectSelected(wxEVT_PROJECT_TREEITEM_CLICKED);
        evtProjectSelected.SetString(data->GetData().GetDisplayName());
        EventNotifier::Get()->AddPendingEvent(evtProjectSelected);
    }
}

void FileViewTree::DoGetProjectIconIndex(const wxString& projectName, int& iconIndex, bool& fromPlugin)
{
    fromPlugin = false;
    clColourEvent event(wxEVT_WORKSPACE_VIEW_CUSTOMIZE_PROJECT);
    event.SetEventObject(this);
    // set the project name
    event.SetString(projectName);
    if(EventNotifier::Get()->ProcessEvent(event)) {
        iconIndex = event.GetInt();
        fromPlugin = true;

    } else {
        iconIndex = PROJECT_IMG_IDX;
    }
}

void FileViewTree::OnRenameProject(wxCommandEvent& event)
{
    CHECK_COND_RET(clCxxWorkspaceST::Get()->IsOpen());
    wxTreeItemId item = GetSingleSelection();
    CHECK_ITEM_RET(item);

    FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
    if(data->GetData().GetKind() == ProjectItem::TypeProject) {
        wxString newname = ::wxGetTextFromUser(_("Project new name:"), _("Rename project"));
        newname.Trim().Trim(false);
        CHECK_COND_RET(!newname.IsEmpty());

        wxString oldname = data->GetData().GetDisplayName();
        // If the new name and the old name are the same, do nothing
        if(oldname == newname) return;

        // If a project with this name already exists, abort
        if(m_projectsMap.count(newname)) {
            ::wxMessageBox(_("A project with this name already exists in the workspace"), "CodeLite",
                           wxOK | wxICON_WARNING | wxCENTRE, this);
            return;
        }

        if(!::clIsVaidProjectName(newname)) {
            wxMessageBox(_("Project names may contain only the following characters [a-z0-9_-]"), "CodeLite",
                         wxOK | wxICON_WARNING | wxCENTER, this);
            return;
        }

        // Calling 'RenameProject' will trigger a wxEVT_PROJ_RENAMED event
        clCxxWorkspaceST::Get()->RenameProject(data->GetData().GetDisplayName(), newname);

        // Update the display name
        SetItemText(item, newname);

        // Update the user data
        data->GetData().SetDisplayName(newname);

        // Update the cache
        m_projectsMap.erase(oldname);
        m_projectsMap.insert(std::make_pair(newname, item));
    }
}

void FileViewTree::OnFolderDropped(clCommandEvent& event)
{
    // User dragged a folder into our workspace
    const wxArrayString& folders = event.GetStrings();
    if(folders.size() != 1) {
        ::wxMessageBox(_("You can only drag one folder at a time"), "CodeLite", wxOK | wxCENTER | wxICON_ERROR);
        return;
    }

    bool reloadWorkspaceIsNeeded(false);
    const wxString& folder = folders.Item(0);
    wxFileName workspaceFileName(folder, "");
    wxString errMsg;
    if(!clCxxWorkspaceST::Get()->IsOpen()) {

        wxFileName fnWorkspace(folder, "");

        // If a workspace is already exist at the selected path - load it
        wxArrayString workspaceFiles;
        wxString workspaceFile;
        wxDir::GetAllFiles(folder, &workspaceFiles, "*.workspace", wxDIR_FILES);
        // Check the workspace type
        for(size_t i = 0; i < workspaceFiles.size(); ++i) {
            if(FileExtManager::GetType(workspaceFiles.Item(i)) == FileExtManager::TypeWorkspace) {
                // Found a workspace in the folder, just open it
                wxCommandEvent evtOpenworkspace(wxEVT_MENU, XRCID("switch_to_workspace"));
                evtOpenworkspace.SetString(workspaceFiles.Item(i));
                evtOpenworkspace.SetEventObject(clMainFrame::Get());
                clMainFrame::Get()->GetEventHandler()->AddPendingEvent(evtOpenworkspace);
                return;
            }
        }

        wxString project_name = workspaceFileName.GetDirs().Last();
        project_name = FileUtils::NormaliseName(project_name);

        workspaceFileName.SetName(project_name);
        workspaceFileName.SetExt("workspace");

        // Create an empty workspace
        if(!clCxxWorkspaceST::Get()->CreateWorkspace(project_name, folder, errMsg)) {
            ::wxMessageBox(_("Failed to create workspace:\n") + errMsg, "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
            return;
        }

        // Create an empty project with sensible defaults
        ProjectData pd;
        CompilerPtr cmp = BuildSettingsConfigST::Get()->GetDefaultCompiler(COMPILER_DEFAULT_FAMILY);
        if(cmp) {
            pd.m_cmpType = cmp->GetName();
        } else {
            pd.m_cmpType = "gnu g++"; // Default :/
        }

        pd.m_name = project_name;
        pd.m_path = folder;

        // Set a default empty project
        pd.m_srcProject.Reset(new Project());

// Use sensible debugger defaults
#ifdef __WXMAC__
        pd.m_debuggerType = "LLDB Debugger";
#else
        pd.m_debuggerType = "GNU gdb debugger";
#endif
        ManagerST::Get()->CreateProject(pd, "");
        reloadWorkspaceIsNeeded = true;
    }

    // to which project should we import the folder?
    wxArrayString projects;
    clCxxWorkspaceST::Get()->GetProjectList(projects);
    if(projects.IsEmpty()) {
        ::wxMessageBox(_("Can't import files to workspace without projects"), "CodeLite",
                       wxICON_ERROR | wxOK | wxCENTER);
        return;
    }

    wxString projectName;
    if(projects.GetCount() > 1) {
        int selection = projects.Index(clCxxWorkspaceST::Get()->GetActiveProjectName());
        projectName = ::wxGetSingleChoice(_("Select project:"), _("Import files to project"), projects, selection);
    } else {
        // single project, just add it
        projectName = projects.Item(0);
    }

    // user cancelled?
    if(projectName.IsEmpty()) return;
    ProjectPtr pProj = clCxxWorkspaceST::Get()->GetProject(projectName);
    CHECK_PTR_RET(pProj);

    wxArrayString all_files;
    wxDir::GetAllFiles(folder, &all_files, wxEmptyString, wxDIR_DIRS | wxDIR_FILES);

    ImportFilesSettings ifs;
    DoImportFolder(pProj, folder, all_files, ifs.GetFileMask(), ifs.GetFlags() & IFS_INCLUDE_FILES_WO_EXT);

    if(reloadWorkspaceIsNeeded) {
        // Now that we have created a workspace + one project reload the workspace
        wxCommandEvent evtOpenworkspace(wxEVT_MENU, XRCID("switch_to_workspace"));
        evtOpenworkspace.SetString(workspaceFileName.GetFullPath());
        evtOpenworkspace.SetEventObject(clMainFrame::Get());
        clMainFrame::Get()->GetEventHandler()->AddPendingEvent(evtOpenworkspace);
    }

    // And trigger a full reparse of the workspace
    wxCommandEvent evtOpenworkspace(wxEVT_MENU, XRCID("full_retag_workspace"));
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(evtOpenworkspace);
}

void FileViewTree::FolderDropped(const wxArrayString& folders)
{
    clCommandEvent dummy;
    dummy.SetStrings(folders);
    OnFolderDropped(dummy);
}

void FileViewTree::OnOpenShellFromFilePath(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(data->GetData().GetKind() == ProjectItem::TypeFile) {
            wxFileName fn(data->GetData().GetFile());
            FileUtils::OpenTerminal(fn.GetPath());
        } else if(data->GetData().GetKind() == ProjectItem::TypeProject) {
            ProjectPtr p = clCxxWorkspaceST::Get()->GetProject(data->GetData().GetDisplayName());
            if(p) {
                // Apply the environment before launching the terminal
                EnvSetter env(p);
                FileUtils::OpenTerminal(p->GetFileName().GetPath());
            }
        }
    }
}

void FileViewTree::OnOpenFileExplorerFromFilePath(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(data->GetData().GetKind() == ProjectItem::TypeFile) {
            wxFileName fn(data->GetData().GetFile());
            FileUtils::OpenFileExplorerAndSelect(fn.GetFullPath());
        } else if(data->GetData().GetKind() == ProjectItem::TypeProject) {
            ProjectPtr p = clCxxWorkspaceST::Get()->GetProject(data->GetData().GetDisplayName());
            if(p) { FileUtils::OpenFileExplorerAndSelect(p->GetFileName()); }
        }
    }
}

void FileViewTree::DoCreateProjectContextMenu(wxMenu& menu, const wxString& projectName)
{
    wxMenuItem* item(NULL);
    BitmapLoader* bmpLoader = clGetManager()->GetStdIcons();
    wxBitmap bmpBuild = bmpLoader->LoadBitmap("toolbars/16/build/build");
    wxBitmap bmpClean = bmpLoader->LoadBitmap("toolbars/16/build/clean");
    wxBitmap bmpSettings = bmpLoader->LoadBitmap("cog");
    wxBitmap bmpSort = bmpLoader->LoadBitmap("sort");
    wxBitmap bmpFolder = bmpLoader->LoadBitmap("folder-yellow");
    wxBitmap bmpConsole = bmpLoader->LoadBitmap("console");
    wxBitmap bmpColourPallette = bmpLoader->LoadBitmap("colour-pallette");

    item = new wxMenuItem(&menu, XRCID("build_project"), _("Build"), _("Build project"));
    item->SetBitmap(bmpBuild);
    menu.Append(item);

    item = new wxMenuItem(&menu, XRCID("rebuild_project"), _("Rebuild"), _("Rebuild project"));
    menu.Append(item);

    item = new wxMenuItem(&menu, XRCID("clean_project"), _("Clean"), _("Clean project"));
    item->SetBitmap(bmpClean);
    menu.Append(item);

    item = new wxMenuItem(&menu, XRCID("stop_build"), _("Stop Build"), _("Stop Build"));
    menu.Append(item);
    menu.AppendSeparator();

    wxMenu* projectOnly = new wxMenu();
    projectOnly->Append(XRCID("build_project_only"), _("Build"));
    projectOnly->Append(XRCID("clean_project_only"), _("Clean"));
    projectOnly->Append(XRCID("rebuild_project_only"), _("Rebuild"));
    menu.Append(wxID_ANY, _("Project Only"), projectOnly);
    menu.AppendSeparator();

    // Add custom project targets
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
    if(bldConf && bldConf->IsCustomBuild()) {
        wxMenuItem* item = NULL;
        // append the custom build targets
        const BuildConfig::StringMap_t& targets = bldConf->GetCustomTargets();
        if(targets.empty() == false) {
            wxMenu* customTargetsMenu = new wxMenu();
            CustomTargetsMgr::Get().SetTargets(projectName, targets);
            const CustomTargetsMgr::Map_t& targetsMap = CustomTargetsMgr::Get().GetTargets();
            // get list of custom targets, and create menu entry for each target
            CustomTargetsMgr::Map_t::const_iterator iter = targetsMap.begin();
            for(; iter != targetsMap.end(); ++iter) {
                item = new wxMenuItem(customTargetsMenu,
                                      iter->first,        // Menu ID
                                      iter->second.first, // Menu Name
                                      wxEmptyString, wxITEM_NORMAL);
                customTargetsMenu->Append(item);
            }
            menu.Append(wxID_ANY, _("Custom Targets..."), customTargetsMenu);
            menu.AppendSeparator();
        }
    }

    item = new wxMenuItem(&menu, XRCID("build_order"), _("Build Order..."), _("Build Order..."));
    item->SetBitmap(bmpSort);
    menu.Append(item);
    menu.AppendSeparator();

    item = new wxMenuItem(&menu, XRCID("cxx_fileview_open_file_explorer"), _("Open Containing Folder"),
                          _("Open Containing Folder"));
    item->SetBitmap(bmpFolder);
    menu.Append(item);

    item = new wxMenuItem(&menu, XRCID("cxx_fileview_open_shell_from_filepath"), _("Open Shell"), _("Open Shell"));
    item->SetBitmap(bmpConsole);
    menu.Append(item);

    menu.AppendSeparator();
    menu.Append(XRCID("set_as_active"), _("Make Active (double click)"));
    menu.AppendSeparator();

    menu.Append(XRCID("import_directory"), _("Import Files From Directory..."));
    menu.Append(XRCID("reconcile_project"), _("Reconcile Project..."));
    menu.AppendSeparator();

    menu.Append(XRCID("new_virtual_folder"), _("New Virtual Folder"));
    menu.AppendSeparator();

    menu.Append(XRCID("remove_project"), _("Remove Project"));
    menu.AppendSeparator();
    menu.Append(XRCID("rename_project"), _("Rename Project"));
    menu.AppendSeparator();

    menu.Append(XRCID("save_as_template"), _("Save As Template..."));
    menu.AppendSeparator();
    menu.Append(XRCID("local_project_prefs"), _("Project Editor Preferences..."));

    menu.AppendSeparator();

    item = new wxMenuItem(&menu, XRCID("colour_virtual_folder"), _("Set Custom Background Colour..."));
    item->SetBitmap(bmpColourPallette);
    menu.Append(item);

    menu.Append(XRCID("clear_virtual_folder_colour"), _("Clear Background Colour"));
    menu.AppendSeparator();
    item = new wxMenuItem(&menu, XRCID("project_properties"), _("Settings..."), _("Settings..."));
    item->SetBitmap(bmpSettings);
    menu.Append(item);
}

void FileViewTree::UnselectAllProject()
{
    std::for_each(m_projectsMap.begin(), m_projectsMap.end(), [&](std::pair<wxString, wxTreeItemId> p) {
        wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        f.SetWeight(wxFONTWEIGHT_NORMAL);
        f.SetStyle(wxFONTSTYLE_NORMAL);
        SetItemFont(p.second, f);
    });
}

wxTreeItemId FileViewTree::AddWorkspaceFolder(const wxString& folderPath)
{
    if(folderPath.IsEmpty()) return GetRootItem();
    wxTreeItemId folderItemId;
    wxArrayString folders = ::wxStringTokenize(folderPath, "/", wxTOKEN_STRTOK);
    wxString current;
    wxTreeItemId parentItem = GetRootItem();
    for(size_t i = 0; i < folders.size(); ++i) {
        if(!current.IsEmpty()) { current << "/"; }
        current << folders.Item(i);
        if(m_workspaceFolders.count(current) == 0) {
            // New folder
            ProjectItem itemData;
            itemData.SetKind(ProjectItem::TypeWorkspaceFolder);
            itemData.SetDisplayName(folders.Item(i));
            itemData.m_key = current;
            FilewViewTreeItemData* cd = new FilewViewTreeItemData(itemData);
            parentItem =
                AppendItem(parentItem, folders.Item(i), WORKSPACE_FOLDER_IMG_IDX, WORKSPACE_FOLDER_IMG_IDX, cd);
            m_workspaceFolders.insert(std::make_pair(current, parentItem));
        } else {
            parentItem = m_workspaceFolders.find(current)->second;
        }
        folderItemId = parentItem;
    }
    return folderItemId;
}

void FileViewTree::DoClear()
{
    DeleteAllItems();
    m_itemsToSort.clear();
    m_workspaceFolders.clear();
    m_projectsMap.clear();
    m_excludeBuildFiles.clear();
}

void FileViewTree::ShowWorkspaceFolderContextMenu()
{
    wxMenu menu;
    menu.Append(XRCID("workspace_folder_new"), _("New Workspace Folder"));
    menu.Append(XRCID("workspace_folder_delete"), _("Delete"));
    menu.AppendSeparator();
    menu.Append(XRCID("new_cxx_project"), _("New Project..."));
    menu.AppendSeparator();
    menu.Append(XRCID("add_existing_cxx_project"), _("Add an Existing Project..."));

    menu.Bind(wxEVT_MENU, &FileViewTree::OnWorkspaceFolderNewFolder, this, XRCID("workspace_folder_new"));
    menu.Bind(wxEVT_MENU, &FileViewTree::OnWorkspaceFolderDelete, this, XRCID("workspace_folder_delete"));

    PopupMenu(&menu);
}

void FileViewTree::OnWorkspaceFolderDelete(wxCommandEvent& evt)
{
    wxArrayTreeItemIds items, folderItems;
    GetMultiSelection(items);

    for(size_t i = 0; i < items.size(); ++i) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(items.Item(i)));
        if(data && data->GetData().GetKind() == ProjectItem::TypeWorkspaceFolder) { folderItems.Add(items.Item(i)); }
    }

    CHECK_COND_RET(!folderItems.IsEmpty());

    wxString message;
    if(folderItems.size() > 1) {
        message << _("Are you sure you want to delete the following workspace folders:\n");
        for(size_t i = 0; i < folderItems.size(); ++i) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(folderItems.Item(i)));
            message << data->GetData().GetDisplayName() << "\n";
        }
        message << "?";
    } else {
        message << _("Are you sure you want to delete the workspace folder:\n");
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(folderItems.Item(0)));
        message << data->GetData().GetDisplayName() << "\n?";
    }

    if(::wxMessageBox(message, "CodeLite", wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxCENTER,
                      EventNotifier::Get()->TopFrame()) != wxYES) {
        return;
    }

    // Delete each folder
    for(size_t i = 0; i < folderItems.size(); ++i) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(folderItems.Item(i)));
        clCxxWorkspaceST::Get()->DeleteWorkspaceFolder(data->GetData().Key());
    }
    CallAfter(&FileViewTree::BuildTree);
}

void FileViewTree::OnWorkspaceNewWorkspaceFolder(wxCommandEvent& evt)
{
    wxString name = ::wxGetTextFromUser(_("Enter folder name:"), _("New Folder Name"));
    if(name.IsEmpty()) return;
    clCxxWorkspaceST::Get()->CreateWorkspaceFolder(name);
    CallAfter(&FileViewTree::BuildTree);
}

void FileViewTree::OnWorkspaceFolderNewFolder(wxCommandEvent& evt)
{
    wxTreeItemId item = GetSingleSelection();
    CHECK_ITEM_RET(item);

    wxString name = ::wxGetTextFromUser(_("Enter folder name:"), _("New Folder Name"));
    if(name.IsEmpty()) return;

    // Append the path
    FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
    name.Prepend(data->GetData().Key() + "/");

    clCxxWorkspaceST::Get()->CreateWorkspaceFolder(name);
    CallAfter(&FileViewTree::BuildTree);
}

void FileViewTree::DoBindEvents()
{
    if(m_eventsBound) return;
    wxFrame* frame = EventNotifier::Get()->TopFrame();
    frame->Bind(wxEVT_MENU, &FileViewTree::OnWorkspaceNewWorkspaceFolder, this, XRCID("add_workspace_folder"));
    frame->Bind(wxEVT_MENU, &FileViewTree::OnNewProject, this, XRCID("new_cxx_project"));
    frame->Bind(wxEVT_MENU, &FileViewTree::OnAddProjectToWorkspaceFolder, this, XRCID("add_existing_cxx_project"));
}

void FileViewTree::DoUnbindEvents()
{
    if(m_eventsBound) {
        wxFrame* frame = EventNotifier::Get()->TopFrame();
        frame->Unbind(wxEVT_MENU, &FileViewTree::OnWorkspaceNewWorkspaceFolder, this, XRCID("add_workspace_folder"));
        frame->Unbind(wxEVT_MENU, &FileViewTree::OnNewProject, this, XRCID("new_cxx_project"));
        frame->Unbind(wxEVT_MENU, &FileViewTree::OnAddProjectToWorkspaceFolder, this,
                      XRCID("add_existing_cxx_project"));
        m_eventsBound = false;
    }
}

void FileViewTree::OnNewProject(wxCommandEvent& evt)
{
    wxString workspaceFolder;
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(data && (data->GetData().GetKind() == ProjectItem::TypeWorkspaceFolder)) {
            workspaceFolder = data->GetData().Key();
        }
    }
    ManagerST::Get()->ShowNewProjectWizard(workspaceFolder);
}

void FileViewTree::DoFilesEndDrag(wxTreeItemId& itemDst)
{
    // Files were being dragged
    wxString targetVD, fromVD;
    while(true) {
        if(!itemDst.IsOk()) { return; }
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(itemDst));
        if(data && data->GetData().GetKind() == ProjectItem::TypeVirtualDirectory) {
            break; // Found a vd, so break out of the while loop
        }
        // We're only allowed to drag items between virtual folders, so find the parent folder
        itemDst = GetItemParent(itemDst);
    }

    wxTreeItemId target = itemDst;
    if(target.IsOk()) {
        targetVD = GetItemPath(target);
    } else {
        return;
    }

    for(size_t n = 0; n < m_draggedFiles.GetCount(); ++n) {
        wxTreeItemId itemSrc = m_draggedFiles.Item(n);
        wxTreeItemId fromItem = GetItemParent(itemSrc);
        if(fromItem.IsOk()) {
            fromVD = GetItemPath(fromItem);
        } else {
            continue;
        }

        if(fromVD == targetVD) {
            // Not much point dropping onto the same virtual dir
            continue;
        }

        // the file name to remove
        FilewViewTreeItemData* srcData = static_cast<FilewViewTreeItemData*>(GetItemData(itemSrc));

        // no tree-item-data? skip this one
        if(!srcData) continue;

        wxString filename = srcData->GetData().GetFile();

        ProjectItem itemData = srcData->GetData();

        // call the manager to remove them in the underlying project
        if(ManagerST::Get()->MoveFileToVD(filename, fromVD, targetVD)) {
            // remove the item from its current node, and place it under the
            // new parent node
            AppendItem(target,                    // parent
                       itemData.GetDisplayName(), // display name
                       GetIconIndex(itemData),    // item image index
                       GetIconIndex(itemData),    // selected item image
                       new FilewViewTreeItemData(itemData));
            Delete(itemSrc);
            Expand(target);
            SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
        }
    }
}

void FileViewTree::DoProjectsEndDrag(wxTreeItemId& itemDst)
{
    // We allow dropping on workspace or an a "workspace folder"
    FilewViewTreeItemData* cd = static_cast<FilewViewTreeItemData*>(GetItemData(itemDst));
    CHECK_PTR_RET(cd);

    // Sanity
    if(m_draggedProjects.IsEmpty()) return;

    wxString targetPath;
    if(cd->GetData().GetKind() == ProjectItem::TypeWorkspaceFolder) {
        targetPath = cd->GetData().Key(); // The full path
    } else if(cd->GetData().GetKind() == ProjectItem::TypeWorkspace) {
        targetPath = "";
    } else {
        return; // We do not allow dropping on anything else than the workspace or a workspace folder
    }

    // Move the projects to the target folder and rebuild the tree view
    for(size_t i = 0; i < m_draggedProjects.size(); ++i) {
        FilewViewTreeItemData* d = static_cast<FilewViewTreeItemData*>(GetItemData(m_draggedProjects.Item(i)));
        clCxxWorkspaceST::Get()->MoveProjectToFolder(d->GetData().GetDisplayName(), targetPath,
                                                     (i == (m_draggedProjects.size() - 1)));
    }
    CallAfter(&FileViewTree::BuildTree);
}

void FileViewTree::OnSetBgColourVirtualFolder(wxCommandEvent& e)
{
    wxTreeItemId item = GetSingleSelection();
    CHECK_ITEM_RET(item);

    // Get colour from the user
    wxColour col = ::wxGetColourFromUser(EventNotifier::Get()->TopFrame());
    if(!col.IsOk()) return;

    // Read the current colours map
    FolderColour::Map_t coloursMap;
    if(!LocalWorkspaceST::Get()->GetFolderColours(coloursMap)) return;
    // Colour the tree (it will also update the 'coloursMap' table)
    m_colourHelper->SetBgColour(item, col, coloursMap);
    // Store the settings
    LocalWorkspaceST::Get()->SetFolderColours(coloursMap);
}

void FileViewTree::OnClearBgColourVirtualFolder(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxTreeItemId item = GetSingleSelection();
    CHECK_ITEM_RET(item);

    // Fetch the current colours map
    FolderColour::Map_t coloursMap;
    if(!LocalWorkspaceST::Get()->GetFolderColours(coloursMap)) return;

    // Colour the tree (it will also update the 'coloursMap' table)
    m_colourHelper->ResetBgColour(item, coloursMap);

    // Update the local settings
    LocalWorkspaceST::Get()->SetFolderColours(coloursMap);
}

void FileViewTree::OnAddProjectToWorkspaceFolder(wxCommandEvent& evt)
{
    wxUnusedVar(evt);
    wxTreeItemId item = GetSingleSelection();
    CHECK_ITEM_RET(item);

    FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
    CHECK_PTR_RET(data);

    if(data->GetData().GetKind() != ProjectItem::TypeWorkspaceFolder) return;
    wxString workspaceFolder = data->GetData().Key();
    const wxString ALL(wxT("CodeLite Projects (*.project)|*.project|") wxT("All Files (*)|*"));
    wxFileDialog dlg(this, _("Open Project"), wxEmptyString, wxEmptyString, ALL, wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                     wxDefaultPosition);
    if(dlg.ShowModal() == wxID_OK) {
        wxString errmsg;
        if(!clCxxWorkspaceST::Get()->AddProject(dlg.GetPath(), workspaceFolder, errmsg)) {
            ::wxMessageBox(errmsg, "CodeLite", wxICON_ERROR | wxOK | wxCENTER, EventNotifier::Get()->TopFrame());
        }
    }

    // Fire "Project-Added" event
    clCommandEvent evtProjectAdded(wxEVT_PROJ_ADDED);
    EventNotifier::Get()->AddPendingEvent(evtProjectAdded);
}

size_t FileViewTree::GetSelections(wxArrayTreeItemIds& selections) const
{
#if defined(__WXMSW__) || defined(__WXGTK__)
    selections.Clear();
    std::queue<wxTreeItemId> Q;
    if(!HasFlag(wxTR_HIDE_ROOT)) {
        Q.push(GetRootItem());
    } else {

        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(GetRootItem(), cookie);
        while(child.IsOk()) {
            Q.push(child);
            child = GetNextChild(GetRootItem(), cookie);
        }
    }

    while(!Q.empty()) {
        wxTreeItemId item = Q.front();
        Q.pop();
        if(IsSelected(item)) { selections.Add(item); }

        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(item, cookie);
        while(child.IsOk()) {
            Q.push(child);
            child = GetNextChild(item, cookie);
        }
    }

    return selections.GetCount();
#else
    return wxTreeCtrl::GetSelections(selections);
#endif
}

void FileViewTree::DoSetItemBackgroundColour(const wxTreeItemId& item, const FolderColour::List_t& colours,
                                             const ProjectItem& projectItem)
{
    // Set the background colour for the item if it is a virtual folder or a file
    if(colours.empty()) { return; }
    wxColour bgColour;
    if(projectItem.GetKind() == ProjectItem::TypeVirtualDirectory || projectItem.GetKind() == ProjectItem::TypeFile ||
       projectItem.GetKind() == ProjectItem::TypeProject) {
        // A virtual folder, try to find a custom colour for it
        wxString itemPath = m_colourHelper->GetItemPath(this, item);
        const FolderColour& match = FolderColour::FindForPath(colours, itemPath);
        if(match.IsOk()) {
            bgColour = match.GetColour();
        } else {
            bgColour = wxNullColour;
        }
        if(bgColour.IsOk()) { SetItemBackgroundColour(item, bgColour); }
    }
}

void FileViewTree::OnItemExpanding(wxTreeEvent& e)
{
    e.Skip();
    wxTreeItemId item = e.GetItem();
    DoBuildSubTreeIfNeeded(item);
}

void FileViewTree::DoAddChildren(const wxTreeItemId& parentItem)
{
    ProjectPtr proj = GetItemProject(parentItem);
    if(!proj) { return; }

    FolderColour::Map_t coloursMap;
    FolderColour::List_t coloursList;
    LocalWorkspaceST::Get()->GetFolderColours(coloursMap);

    // Sort the list
    FolderColour::SortToList(coloursMap, coloursList);

    // int iconIndex = GetIconIndex(node->GetData());

    FilewViewTreeItemData* cd = ItemData(parentItem);
    if(!cd) return;

    const ProjectItem& pi = cd->GetData();
    if(!pi.IsVirtualFolder() && !pi.IsProject()) return;

    wxArrayString folders, files;
    wxString vdFullPath;
    if(pi.IsVirtualFolder()) {
        vdFullPath = GetItemPath(parentItem);
        wxString vdFullPathNoProject = vdFullPath.AfterFirst(':');

        proj->GetFolders(vdFullPathNoProject, folders);
        proj->GetFiles(vdFullPathNoProject, files);
    } else {
        proj->GetFolders("", folders);
    }

    // First, we add the virtual folders
    for(size_t i = 0; i < folders.size(); ++i) {
        const wxString& childVdFullPath = folders.Item(i);
        wxString displayName = childVdFullPath.AfterLast(':');
        ProjectItem folderItem(proj->GetName() + ":" + childVdFullPath, displayName, "",
                               ProjectItem::TypeVirtualDirectory);
        wxTreeItemId hti = AppendItem(parentItem,     // parent
                                      displayName,    // display name
                                      FOLDER_IMG_IDX, // item image index
                                      FOLDER_IMG_IDX, // selected item image
                                      new FilewViewTreeItemData(folderItem));
        DoSetItemBackgroundColour(hti, coloursList, folderItem);
        if(!proj->IsVirtualDirectoryEmpty(childVdFullPath)) {
            // Add a dummy item So the user can "Expand" it
            AppendItem(hti, "<dummy>", -1, -1,
                       new FilewViewTreeItemData(ProjectItem("", "", "", ProjectItem::TypeInvalid)));
        }
    }

    BuildConfigPtr buildConf = proj->GetBuildConfiguration();
    wxString buildConfName = buildConf ? buildConf->GetName() : "";

    for(size_t i = 0; i < files.size(); ++i) {
        const wxString& filepath = files.Item(i);
        wxFileName fn(filepath);
        ProjectItem fileItem(vdFullPath + ":" + fn.GetFullName(), fn.GetFullName(), filepath, ProjectItem::TypeFile);

        int iconIndex = GetIconIndex(fileItem);
        wxTreeItemId hti = AppendItem(parentItem,                // parent
                                      fileItem.GetDisplayName(), // display name
                                      iconIndex,                 // item image index
                                      iconIndex,                 // selected item image
                                      new FilewViewTreeItemData(fileItem));
        DoSetItemBackgroundColour(hti, coloursList, fileItem);

        // If the file is disabled for the current build configuration, mark it as such
        clProjectFile::Ptr_t fileInfo = proj->GetFile(fn.GetFullPath());
        if(fileInfo && !buildConfName.IsEmpty() && fileInfo->IsExcludeFromConfiguration(buildConfName)) {
            // Set the item text with disabled colour
            ExcludeFileFromBuildUI(hti, true);
        }
    }

    // Now, add the files
    m_itemsToSort[parentItem] = true;
    SortTree();
}

ProjectPtr FileViewTree::GetItemProject(const wxTreeItemId& item) const
{
    wxTreeItemId cur = item;
    while(cur.IsOk() && (cur != GetRootItem())) {
        FilewViewTreeItemData* cd = ItemData(cur);
        if(cd && cd->GetData().IsProject()) {
            return clCxxWorkspaceST::Get()->GetProject(cd->GetData().GetDisplayName());
        }
        cur = GetItemParent(cur);
    }
    return nullptr;
}

FilewViewTreeItemData* FileViewTree::ItemData(const wxTreeItemId& item) const
{
    CHECK_ITEM_RET_NULL(item);
    wxTreeItemData* cd = GetItemData(item);
    CHECK_PTR_RET_NULL(cd);
    return static_cast<FilewViewTreeItemData*>(cd);
}

void FileViewTree::DoBuildSubTreeIfNeeded(const wxTreeItemId& parent)
{
    if(parent.IsOk() && ItemHasChildren(parent)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId childItem = GetFirstChild(parent, cookie);

        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(childItem));
        if(data && data->GetData().IsInvalid()) {
            // Delete the dummy children
            DeleteChildren(parent);

            // Append the real items
            DoAddChildren(parent);
        }
    }
}

void FileViewTree::ExcludeFileFromBuildUI(const wxTreeItemId& item, bool exclude)
{
    FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
    CHECK_PTR_RET(data);
    CHECK_COND_RET(data->GetData().IsFile());
    const wxString& filename = data->GetData().GetFile();
    if(exclude && m_excludeBuildFiles.count(filename)) { return; }
    if(!exclude && m_excludeBuildFiles.count(filename) == 0) { return; }

    if(exclude) {
        m_excludeBuildFiles.insert({ filename, item });
        SetItemTextColour(item, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
    } else {
        m_excludeBuildFiles.erase(filename);
        SetItemTextColour(item, DrawingUtils::GetOutputPaneFgColour());
    }
}

bool FileViewTree::IsItemExcludedFromBuild(const wxTreeItemId& item, const wxString& configName) const { return false;}

void FileViewTree::OnBuildConfigChanged(wxCommandEvent& e)
{
    e.Skip();
    // Remove the Gray text from the all the "Exclude files"
    std::for_each(m_excludeBuildFiles.begin(), m_excludeBuildFiles.end(),
                  [&](const std::unordered_map<wxString, wxTreeItemId>::value_type& vt) {
                      SetItemTextColour(vt.second, DrawingUtils::GetOutputPaneFgColour());
                  });
    m_excludeBuildFiles.clear();

    std::unordered_map<wxString, wxTreeItemId> allFiles;
    // We need to collect list of all file items from the tree
    std::queue<wxTreeItemId> Q;
    Q.push(GetRootItem());
    while(!Q.empty()) {
        wxTreeItemId item = Q.front();
        Q.pop();

        if(!ItemHasChildren(item)) {
            FilewViewTreeItemData* d = ItemData(item);
            if(d && d->GetData().IsFile()) { allFiles.insert({ d->GetData().GetFile(), item }); }
        } else {
            wxTreeItemIdValue k;
            wxTreeItemId child = GetFirstChild(item, k);
            while(child.IsOk()) {
                Q.push(child);
                child = GetNextChild(item, k);
            }
        }
    }

    // Get list of all excluded files for this configuration
    std::vector<wxString> excludeFiles;
    if(clCxxWorkspaceST::Get()->GetExcludeFilesForConfig(excludeFiles)) {
        for(size_t i = 0; i < excludeFiles.size(); ++i) {
            if(allFiles.count(excludeFiles[i])) {
                // This file was expanded and we have a valid item id for it
                ExcludeFileFromBuildUI(allFiles[excludeFiles[i]], true);
            }
        }
    }
}
