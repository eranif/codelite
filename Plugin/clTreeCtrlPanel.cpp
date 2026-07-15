#include "clTreeCtrlPanel.h"

#include "clAuiToolBarArt.h"
#include "clFileOrFolderDropTarget.h"
#include "clFileSystemEvent.h"
#include "clTreeCtrlPanelDefaultPage.h"
#include "clWorkspaceView.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "macros.h"
#include "resources/clXmlResource.hpp"

#include <wx/app.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/richmsgdlg.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

namespace
{
void collapse_all_children(clFileViewerTreeCtrl* tree, const wxDataViewItem& parent)
{
    int count = tree->GetChildCount(parent);
    for (int i = 0; i < count; ++i) {
        wxDataViewItem child = tree->GetNthChild(parent, i);
        collapse_all_children(tree, child);
        tree->Collapse(child);
    }
}

bool should_colour_item_in_gray(clTreeCtrlData* entry)
{
    if (!entry)
        return false;

    if (FileUtils::IsHidden(entry->GetPath()))
        return true;

    if (entry->IsFolder()) {
        // folders with CMakeCache.txt are build folders and are treated
        // as non interesting folders
        wxFileName cmake_cache(entry->GetPath(), "CMakeCache.txt");
        if (cmake_cache.FileExists()) {
            return true;
        }
    }
    return false;
}
} // namespace

clTreeCtrlPanel::clTreeCtrlPanel(wxWindow* parent)
    : clTreeCtrlPanelBase(parent)
    , m_newfileTemplate("Untitled.txt")
    , m_newfileTemplateHighlightLen(wxStrlen("Untitled"))
{
    int style = wxAUI_TB_DEFAULT_STYLE;
    m_toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    GetSizer()->Insert(0, m_toolbar, 0, wxEXPAND);

#ifdef __WXMSW__
    // Adjust Tree Control Alternate Row Color On Windows
    GetTreeCtrl()->SetAlternateRowColour(GetTreeCtrl()->GetBackgroundColour().ChangeLightness(103));
#endif

    auto images = clGetManager()->GetStdIcons();
    clAuiToolBarArt::AddTool(
        m_toolbar, XRCID("link_editor"), _("Link Editor"), images->LoadBitmap("link_editor"), "", wxITEM_CHECK);
    clAuiToolBarArt::AddTool(
        m_toolbar, XRCID("collapse_folders"), _("Fold Tree"), images->LoadBitmap("fold"), "", wxITEM_NORMAL);

    Bind(
        wxEVT_TOOL,
        [this](wxCommandEvent& e) {
            e.Skip();
            collapse_all_children(m_treeCtrl, m_treeCtrl->GetRootItem());
        },
        XRCID("collapse_folders"));
    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_TOOL, &clTreeCtrlPanel::OnLinkEditor, this, XRCID("link_editor"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &clTreeCtrlPanel::OnLinkEditorUI, this, XRCID("link_editor"));

    // Allow DnD
    SetDropTarget(new clFileOrFolderDropTarget(this));
    GetTreeCtrl()->SetDropTarget(new clFileOrFolderDropTarget(this));
    Bind(wxEVT_DND_FOLDER_DROPPED, &clTreeCtrlPanel::OnFolderDropped, this);
    GetTreeCtrl()->SetBitmaps(clBitmaps::Get().GetLoader()->GetStandardMimeBitmapListPtr());

    // Bind and respond to the bitmaps-updated event
    EventNotifier::Get()->Bind(wxEVT_BITMAPS_UPDATED, [this](clCommandEvent& event) {
        event.Skip();
        GetTreeCtrl()->SetBitmaps(clBitmaps::Get().GetLoader()->GetStandardMimeBitmapListPtr());
        GetTreeCtrl()->Refresh();
    });

    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &clTreeCtrlPanel::OnActiveEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &clTreeCtrlPanel::OnInitDone, this);
    EventNotifier::Get()->Bind(wxEVT_FINDINFILES_DLG_SHOWING, &clTreeCtrlPanel::OnFindInFilesShowing, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_CREATED, &clTreeCtrlPanel::OnFilesCreated, this);
    m_defaultView = new clTreeCtrlPanelDefaultPage(this);
    GetSizer()->Add(m_defaultView, 1, wxEXPAND);
    GetTreeCtrl()->Hide();
}

clTreeCtrlPanel::~clTreeCtrlPanel()
{
    Unbind(wxEVT_DND_FOLDER_DROPPED, &clTreeCtrlPanel::OnFolderDropped, this);
    m_toolbar->Unbind(wxEVT_TOOL, &clTreeCtrlPanel::OnLinkEditor, this, XRCID("link_editor"));
    m_toolbar->Unbind(wxEVT_UPDATE_UI, &clTreeCtrlPanel::OnLinkEditorUI, this, XRCID("link_editor"));
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &clTreeCtrlPanel::OnActiveEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &clTreeCtrlPanel::OnInitDone, this);
    EventNotifier::Get()->Unbind(wxEVT_FINDINFILES_DLG_SHOWING, &clTreeCtrlPanel::OnFindInFilesShowing, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_CREATED, &clTreeCtrlPanel::OnFilesCreated, this);
}

void clTreeCtrlPanel::OnContextMenu(wxDataViewEvent& event)
{
    wxDataViewItem item = event.GetItem();
    CHECK_ITEM_RET(item);

    clTreeCtrlData* cd = GetItemData(item);
    if (cd && cd->IsFolder()) {
        // Prepare a folder context menu
        std::unique_ptr<wxMenu> menu{clXmlResource::Get().LoadMenu("file_explorer_folder_menu")};
        // Handle conditional menu items based on IsTopLevelFolder()
        if (IsTopLevelFolder(item)) {
            // Disable items for top-level folders
            menu->Enable(XRCID("tree_ctrl_delete_folder"), false);
            menu->Enable(XRCID("tree_ctrl_rename_folder"), false);
        } else {
            // Disable items for non top-level folders
            menu->Enable(XRCID("tree_ctrl_close_folder"), false);
        }

        menu->Bind(
            wxEVT_MENU,
            [cd](wxCommandEvent& event) {
                event.Skip();
                CHECK_PTR_RET(cd);
                ::CopyToClipboard(cd->GetPath());
                clGetManager()->SetStatusMessage(_("Path copied to clipboard"));
            },
            XRCID("copy-path"));

        // Now that we added the basic menu, let the plugin
        // adjust it
        wxArrayString files, folders;
        GetSelections(folders, files);

        clContextMenuEvent dirMenuEvent(wxEVT_CONTEXT_MENU_FOLDER);
        dirMenuEvent.SetEventObject(this);
        dirMenuEvent.SetMenu(menu.get());
        dirMenuEvent.SetPath(cd->GetPath());
        EventNotifier::Get()->ProcessEvent(dirMenuEvent);

        // Connect events
        menu->Bind(wxEVT_MENU, &clTreeCtrlPanel::OnCloseFolder, this, XRCID("tree_ctrl_close_folder"));
        menu->Bind(wxEVT_MENU, &clTreeCtrlPanel::OnNewFolder, this, XRCID("tree_ctrl_new_folder"));
        menu->Bind(wxEVT_MENU, &clTreeCtrlPanel::OnNewFile, this, XRCID("tree_ctrl_new_file"));
        menu->Bind(wxEVT_MENU, &clTreeCtrlPanel::OnDeleteSelections, this, XRCID("tree_ctrl_delete_folder"));
        menu->Bind(wxEVT_MENU, &clTreeCtrlPanel::OnRenameFolder, this, XRCID("tree_ctrl_rename_folder"));
        menu->Bind(wxEVT_MENU, &clTreeCtrlPanel::OnFindInFilesFolder, this, XRCID("tree_ctrl_find_in_files_folder"));
        menu->Bind(
            wxEVT_MENU, &clTreeCtrlPanel::OnOpenContainingFolder, this, XRCID("tree_ctrl_open_containig_folder"));
        menu->Bind(wxEVT_MENU, &clTreeCtrlPanel::OnOpenShellFolder, this, XRCID("tree_ctrl_open_shell_folder"));
        menu->Bind(wxEVT_MENU, &clTreeCtrlPanel::OnRefresh, this, wxID_REFRESH);
        PopupMenu(menu.get());

    } else if (cd && cd->IsFile()) {
        // File context menu
        // Prepare a folder context menu
        std::unique_ptr<wxMenu> menu{clXmlResource::Get().LoadMenu("file_explorer_file_menu")};

        menu->Bind(
            wxEVT_MENU,
            [cd](wxCommandEvent& event) {
                event.Skip();
                CHECK_PTR_RET(cd);
                ::CopyToClipboard(cd->GetPath());
                clGetManager()->SetStatusMessage(_("Path copied to clipboard"));
            },
            XRCID("copy-path"));

        // Now that we added the basic menu, let the plugin
        // adjust it
        wxArrayString files, folders;
        GetSelections(folders, files);

        clContextMenuEvent fileMenuEvent(wxEVT_CONTEXT_MENU_FILE);
        fileMenuEvent.SetEventObject(this);
        fileMenuEvent.SetMenu(menu.get());
        fileMenuEvent.SetStrings(files);
        EventNotifier::Get()->ProcessEvent(fileMenuEvent);

        // Connect events
        menu->Bind(wxEVT_MENU, &clTreeCtrlPanel::OnOpenFile, this, XRCID("tree_ctrl_open_file"));
        menu->Bind(wxEVT_MENU, &clTreeCtrlPanel::OnRenameFile, this, XRCID("tree_ctrl_rename_file"));
        menu->Bind(wxEVT_MENU, &clTreeCtrlPanel::OnDeleteSelections, this, XRCID("tree_ctrl_delete_file"));
        menu->Bind(
            wxEVT_MENU, &clTreeCtrlPanel::OnOpenWithDefaultApplication, this, XRCID("tree_ctrl_open_with_default_app"));
        menu->Bind(
            wxEVT_MENU, &clTreeCtrlPanel::OnOpenContainingFolder, this, XRCID("tree_ctrl_open_containig_folder"));
        menu->Bind(wxEVT_MENU, &clTreeCtrlPanel::OnOpenShellFolder, this, XRCID("tree_ctrl_open_shell_folder"));
        PopupMenu(menu.get());

    } else {
        // context menu elsewhere
        wxMenu menu;
        menu.Append(XRCID("open_folder_default_page"), _("Open Folder..."));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnOpenFolder, this, XRCID("open_folder_default_page"));
        PopupMenu(&menu);
    }
}

void clTreeCtrlPanel::OnItemActivated(wxDataViewEvent& event)
{
    event.Skip();
    wxCommandEvent dummy;
    OnOpenFile(dummy);
}

void clTreeCtrlPanel::OnItemExpanding(wxDataViewEvent& event)
{
    event.Skip();
    wxDataViewItem item = event.GetItem();
    CHECK_ITEM_RET(item);
    DoExpandItem(item, true, /*isNativeExpandEvent=*/true);
}

void clTreeCtrlPanel::OnFolderDropped(clCommandEvent& event)
{
    const wxArrayString& folders = event.GetStrings();
    for (size_t i = 0; i < folders.size(); ++i) {
        AddFolder(folders.Item(i));
    }
    ::clGetManager()->GetWorkspaceView()->SelectPage(GetViewName());
}

void clTreeCtrlPanel::DoExpandItem(const wxDataViewItem& parent, bool expand, bool isNativeExpandEvent)
{
    clTreeCtrlData* cd = GetItemData(parent);
    CHECK_PTR_RET(cd);

    // we only know how to expand folders...
    if (!cd->IsFolder())
        return;
    wxString folderPath = cd->GetPath();

    if (!m_treeCtrl->ItemHasChildren(parent))
        return;
    // Test the first item for dummy

    wxDataViewItem child = m_treeCtrl->GetNthChild(parent, 0);
    CHECK_ITEM_RET(child);

    cd = GetItemData(child);
    CHECK_PTR_RET(cd);

    // If not dummy - already expanded do nothing here
    if (!cd->IsDummy())
        return;

    m_treeCtrl->DeleteItem(child);
    cd = NULL;

    // Get the top level folders
    wxDir dir(folderPath);
    if (!dir.IsOpened())
        return;
    wxBusyCursor bc;
    wxString filename;
    bool cont = dir.GetFirst(&filename, wxEmptyString);
    while (cont) {
        wxFileName fullpath(folderPath, filename);
        // Make sure we don't add the same folder twice
        if (wxFileName::DirExists(fullpath.GetFullPath())) {
            // a folder
            if (!(m_options & kShowHiddenFolders) && FileUtils::IsHidden(fullpath)) {
                cont = dir.GetNext(&filename);
                continue;
            }
            DoAddFolder(parent, fullpath.GetFullPath());
        } else {
            // make sure we don't add the same file twice
            if (!(m_options & kShowHiddenFiles) && FileUtils::IsHidden(fullpath)) {
                cont = dir.GetNext(&filename);
                continue;
            } else if (!m_excludeFilePatterns.empty() && FileUtils::WildMatch(m_excludeFilePatterns, fullpath)) {
                // exclude this file?
                cont = dir.GetNext(&filename);
                continue;
            }
            DoAddFile(parent, fullpath.GetFullPath());
        }
        cont = dir.GetNext(&filename);
    }

    // Sort the parent
    if (GetTreeCtrl()->ItemHasChildren(parent)) {
        if (expand && !isNativeExpandEvent) {
            // When called from OnItemExpanding(), the native control is already in the process of
            // expanding "parent" for us; calling Expand() again here would re-enter its expand
            // logic for the same node (see the comment on the declaration for details).
            GetTreeCtrl()->Expand(parent);
        }
        SelectItem(parent);
    }
}

clTreeCtrlData* clTreeCtrlPanel::GetItemData(const wxDataViewItem& item) const { return m_treeCtrl->GetItemData(item); }

void clTreeCtrlPanel::AddFolder(const wxString& path)
{
    wxDataViewItem itemFolder = DoAddFolder(GetTreeCtrl()->GetRootItem(), path);
    DoExpandItem(itemFolder, false);
    ToggleView();
}

wxDataViewItem clTreeCtrlPanel::DoAddFile(const wxDataViewItem& parent, const wxString& path)
{
    wxFileName filename(path);
    clTreeCtrlData* parentData = GetItemData(parent);
    if (!parentData) {
        return wxDataViewItem();
    }
    if (parentData->GetIndex()) {
        wxDataViewItem cachedItem = parentData->GetIndex()->Find(filename.GetFullName());
        if (cachedItem.IsOk()) {
            // the item already exists, return it
            return cachedItem;
        }
    }

    clTreeCtrlData* cd = new clTreeCtrlData(clTreeCtrlData::kFile);
    cd->SetPath(filename.GetFullPath());

    bool isHidden = should_colour_item_in_gray(cd);
    int imgIdx = clBitmaps::Get().GetLoader()->GetMimeImageId(filename.GetFullName(), isHidden);
    if (imgIdx == wxNOT_FOUND) {
        imgIdx = clBitmaps::Get().GetLoader()->GetMimeImageId(FileExtManager::TypeText, isHidden);
    }

    wxString fullname = filename.GetFullName();
    wxDataViewItem fileItem = GetTreeCtrl()->InsertFileSorted(parent, fullname, imgIdx, cd);

    // Add this entry to the index
    if (parentData->GetIndex()) {
        parentData->GetIndex()->Add(filename.GetFullName(), fileItem);
    }

    return fileItem;
}

wxDataViewItem clTreeCtrlPanel::DoAddFolder(const wxDataViewItem& parent, const wxString& path)
{
    // If we already have this folder opened, don't re-add it
    wxArrayString topFolders;
    wxDataViewItemArray topFoldersItems;
    GetTopLevelFolders(topFolders, topFoldersItems);
    int where = topFolders.Index(path);
    if (where != wxNOT_FOUND) {
        return topFoldersItems.Item(where);
    }

    wxFileName filename(path, "");
    clTreeCtrlData* cd = new clTreeCtrlData(clTreeCtrlData::kFolder);
    cd->SetPath(path);
    wxString displayName = (m_options & kShowRootFullPath) ? cd->GetPath() : cd->GetName();

    clTreeCtrlData* parentData = GetItemData(parent);
    if (!parentData) {
        wxDELETE(cd);
        return wxDataViewItem();
    }

    // Check the index before adding new folder
    if (parentData->GetIndex()) {
        wxDataViewItem cachedItem = parentData->GetIndex()->Find(displayName);
        if (cachedItem.IsOk()) {
            // the item already exists, return it
            wxDELETE(cd);
            return cachedItem;
        }
    }

    bool isHiddenFolder = should_colour_item_in_gray(cd);
    int imgIdx = clBitmaps::Get().GetLoader()->GetMimeImageId(FileExtManager::TypeFolder, isHiddenFolder);
    int imgOpenedIDx = clBitmaps::Get().GetLoader()->GetMimeImageId(FileExtManager::TypeFolderExpanded, isHiddenFolder);

    wxDataViewItem itemFolder = GetTreeCtrl()->InsertFolderSorted(parent, displayName, imgIdx, imgOpenedIDx, cd);

    // Add this entry to the index
    if (parentData->GetIndex()) {
        parentData->GetIndex()->Add(displayName, itemFolder);
    }

    // Append the dummy item
    GetTreeCtrl()->AppendItem(itemFolder, "Dummy", -1, new clTreeCtrlData(clTreeCtrlData::kDummy));

    // Pin this folder
    if (GetConfig() && IsTopLevelFolder(itemFolder)) {
        wxArrayString pinnedFolders;
        pinnedFolders = GetConfig()->Read("ExplorerFolders", pinnedFolders);
        if (pinnedFolders.Index(cd->GetPath()) == wxNOT_FOUND) {
            pinnedFolders.Add(cd->GetPath());
        }
        GetConfig()->Write("ExplorerFolders", pinnedFolders);
    }
    return itemFolder;
}

void clTreeCtrlPanel::GetSelections(wxArrayString& folders, wxArrayString& files)
{
    wxDataViewItemArray d1, d2;
    GetSelections(folders, d1, files, d2);
}

TreeItemInfo clTreeCtrlPanel::GetSelectedItemInfo()
{
    TreeItemInfo info;
    wxArrayString folders, files;
    GetSelections(folders, files);

    folders.insert(folders.end(), files.begin(), files.end());
    if (folders.empty())
        return info;

    info.m_paths = folders;
    info.m_item = wxTreeItemId();
    return info;
}

void clTreeCtrlPanel::OnCloseFolder(wxCommandEvent& event)
{
    wxArrayString paths, files;
    wxDataViewItemArray items, fileItems;
    GetSelections(paths, items, files, fileItems);

    if (items.IsEmpty())
        return;
    for (size_t i = 0; i < items.GetCount(); ++i) {
        DoCloseFolder(items.Item(i));
    }
}

bool clTreeCtrlPanel::IsTopLevelFolder(const wxDataViewItem& item)
{
    clTreeCtrlData* cd = GetItemData(item);
    return (cd && cd->IsFolder() && GetTreeCtrl()->GetItemParent(item) == GetTreeCtrl()->GetRootItem());
}

void clTreeCtrlPanel::OnNewFile(wxCommandEvent& event)
{
    wxDataViewItem item = GetTreeCtrl()->GetCurrentItem();
    clTreeCtrlData* cd = GetItemData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsFolder());

    wxString filename = ::clGetTextFromUser(_("New File"), _("Set the file name:"), m_newfileTemplate);
    if (filename.IsEmpty())
        return; // user cancelled

    wxFileName file(cd->GetPath(), filename);

    // Write the file content
    if (!FileUtils::WriteFileContent(file, ""))
        return;
    wxDataViewItem newFile = DoAddFile(item, file.GetFullPath());
    // Open the file in the editor
    clGetManager()->OpenFile(file.GetFullPath());
    SelectItem(newFile);

    // Notify about file creation
    clFileSystemEvent fsEvent(wxEVT_FILE_CREATED);
    fsEvent.SetPath(file.GetFullPath());
    fsEvent.SetFileName(file.GetFullName());
    fsEvent.GetPaths().Add(file.GetFullPath());
    EventNotifier::Get()->AddPendingEvent(fsEvent);
}

void clTreeCtrlPanel::OnNewFolder(wxCommandEvent& event)
{
    wxDataViewItem item = GetTreeCtrl()->GetCurrentItem();
    clTreeCtrlData* cd = GetItemData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsFolder());

    wxString foldername = ::clGetTextFromUser(_("New Folder"), _("Set the folder name:"), "New Folder");
    if (foldername.IsEmpty())
        return; // user cancelled

    wxFileName file(cd->GetPath(), "");
    file.AppendDir(foldername);

    // Create the folder
    wxFileName::Mkdir(file.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    // Add it to the tree view
    wxDataViewItem newFile = DoAddFolder(item, file.GetPath());
    SelectItem(newFile);

    // Notify about folder creation
    clFileSystemEvent fsEvent(wxEVT_FOLDER_CREATED);
    fsEvent.SetPath(file.GetPath());
    EventNotifier::Get()->AddPendingEvent(fsEvent);
}

void clTreeCtrlPanel::GetSelections(wxArrayString& folders,
                                    wxDataViewItemArray& folderItems,
                                    wxArrayString& files,
                                    wxDataViewItemArray& fileItems)
{
    folders.clear();
    files.clear();
    folderItems.clear();
    fileItems.clear();

    wxDataViewItemArray items;
    if (GetTreeCtrl()->GetSelections(items) > 0) {
        for (size_t i = 0; i < items.size(); ++i) {
            clTreeCtrlData* cd = GetItemData(items.Item(i));
            if (cd) {
                if (cd->IsFile()) {
                    files.Add(cd->GetPath());
                    fileItems.Add(items.Item(i));
                } else if (cd->IsFolder()) {
                    folders.Add(cd->GetPath());
                    folderItems.Add(items.Item(i));
                }
            }
        }
    }
}

void clTreeCtrlPanel::SelectItem(const wxDataViewItem& item)
{
    CHECK_ITEM_RET(item);
    wxDataViewItemArray items;
    if (GetTreeCtrl()->GetSelections(items) > 0) {
        for (size_t i = 0; i < items.size(); ++i) {
            GetTreeCtrl()->Unselect(items.Item(i));
        }
    }

    GetTreeCtrl()->Select(item);
    GetTreeCtrl()->EnsureVisible(item);
}

struct FileOrFolder {
    wxDataViewItem item;
    bool folder;
    wxString path;
};

namespace
{
struct wxDataViewItemCompare {
    bool operator()(const wxDataViewItem& a, const wxDataViewItem& b) const { return a.GetID() < b.GetID(); }
};
} // namespace

void clTreeCtrlPanel::OnDeleteSelections(wxCommandEvent& event)
{
    wxArrayString folders, files;
    wxDataViewItemArray folderItems, fileItems;
    GetSelections(folders, folderItems, files, fileItems);
    if (files.empty() && folders.empty())
        return;

    std::set<wxDataViewItem, wxDataViewItemCompare> selectedItems;
    std::vector<FileOrFolder> v;

    selectedItems.insert(folderItems.begin(), folderItems.end());
    selectedItems.insert(fileItems.begin(), fileItems.end());

    // loop over the selections and remove all items that their parents
    // also exists in the selected items list
    for (size_t i = 0; i < folderItems.size(); ++i) {
        wxDataViewItem item = folderItems.Item(i);
        bool foundParent = false;
        wxDataViewItem itemParent = GetTreeCtrl()->GetItemParent(item);
        while (itemParent.IsOk()) {
            if (selectedItems.count(itemParent)) {
                // item's parent is in the list, don't delete it as it will get deleted
                // by its parent
                foundParent = true;
                break;
            }
            itemParent = GetTreeCtrl()->GetItemParent(itemParent);
        }

        if (!foundParent) {
            FileOrFolder fof;
            fof.folder = true;
            fof.item = item;
            fof.path = folders.Item(i);
            v.push_back(fof);
        }
    }

    for (size_t i = 0; i < fileItems.size(); ++i) {
        wxDataViewItem item = fileItems.Item(i);
        bool foundParent = false;
        wxDataViewItem itemParent = GetTreeCtrl()->GetItemParent(item);
        while (itemParent.IsOk()) {
            if (selectedItems.count(itemParent)) {
                // item's parent is in the list, don't delete it as it will get deleted
                // by its parent
                foundParent = true;
                break;
            }
            itemParent = GetTreeCtrl()->GetItemParent(itemParent);
        }

        if (!foundParent) {
            FileOrFolder fof;
            fof.folder = false;
            fof.item = item;
            fof.path = files.Item(i);
            v.push_back(fof);
        }
    }

    // At this point "v" contains a unique list of items to delete
    // the items in "v" have no common parent
    wxString message;
    message << _("Are you sure you want to delete the selected items?");

    bool delete_items =
        ::clMessageBox(message, _("Confirm"), wxYES_NO | wxCANCEL | wxNO_DEFAULT | wxCENTER | wxICON_WARNING) == wxYES;

    wxWindowUpdateLocker locker(GetTreeCtrl());
    std::vector<wxDataViewItem> deletedItems;
    wxArrayString deletedFiles, deletedFolders;
    if (delete_items) {
        wxLogNull nl;
        for (const auto& entry : v) {
            if (entry.folder) {
                if (wxFileName::Rmdir(entry.path, wxPATH_RMDIR_RECURSIVE)) {
                    deletedItems.push_back(entry.item);
                    deletedFolders.push_back(entry.path);
                }
            } else {
                if (FileUtils::RemoveFile(entry.path)) {
                    deletedItems.push_back(entry.item);
                    deletedFiles.push_back(entry.path);
                }
            }
        }

        // Notify about the folder/files deletion
        if (!deletedFiles.empty()) {
            clFileSystemEvent files_deleted(wxEVT_FILE_DELETED);
            files_deleted.SetPaths(deletedFiles);
            files_deleted.SetEventObject(this);
            EventNotifier::Get()->AddPendingEvent(files_deleted);
        }

        if (!deletedFolders.empty()) {
            clFileSystemEvent folders_deleted(wxEVT_FOLDER_DELETED);
            folders_deleted.SetPaths(deletedFolders);
            folders_deleted.SetEventObject(this);
            EventNotifier::Get()->AddPendingEvent(folders_deleted);
        }
    }

    // Update the UI
    for (const wxDataViewItem& item : deletedItems) {
        // Before we delete the item from the tree, update the parent cache
        UpdateItemDeleted(item);
        // And now remove the item from the tree
        GetTreeCtrl()->DeleteItem(item);
    }
}

void clTreeCtrlPanel::OnOpenFile(wxCommandEvent& event)
{
    wxArrayString folders, files;
    GetSelections(folders, files);

    for (size_t i = 0; i < files.size(); ++i) {
        // Fire an event before resolving to the default action
        clCommandEvent fileEvent(wxEVT_TREE_ITEM_FILE_ACTIVATED);
        fileEvent.SetEventObject(this);
        fileEvent.SetFileName(files.Item(i));
        if (!EventNotifier::Get()->ProcessEvent(fileEvent)) {
            clGetManager()->OpenFile(files.Item(i));
        }
    }
}

void clTreeCtrlPanel::OnRenameFile(wxCommandEvent& event)
{
    wxArrayString files, folders;
    wxDataViewItemArray fileItems, folderItems;
    GetSelections(folders, folderItems, files, fileItems);

    if (files.empty())
        return;

    // Prompt and rename each file
    for (size_t i = 0; i < files.size(); ++i) {
        wxFileName oldname(files.Item(i));

        wxString newname =
            ::clGetTextFromUser(_("Rename File"), _("New Name:"), oldname.GetFullName(), wxStrlen(oldname.GetName()));
        if (!newname.IsEmpty() && (newname != oldname.GetFullName())) {
            clTreeCtrlData* d = GetItemData(fileItems.Item(i));
            if (d) {
                wxFileName oldpath = d->GetPath();
                wxFileName newpath = oldpath;
                newpath.SetFullName(newname);
                if (::wxRenameFile(oldpath.GetFullPath(), newpath.GetFullPath(), false)) {
                    DoRenameItem(fileItems.Item(i), oldname.GetFullName(), newname);
                }
            }
        }
    }
}

void clTreeCtrlPanel::ExpandToFileVoid(const wxFileName& filename) { ExpandToFile(filename); }

bool clTreeCtrlPanel::ExpandToFile(const wxFileName& file_to_search)
{
#ifdef __WXMSW__
    wxFileName filename{file_to_search.GetFullPath().Lower()};
#else
    wxFileName filename{file_to_search};
#endif

    wxArrayString topFolders;
    wxDataViewItemArray topFoldersItems;
    GetTopLevelFolders(topFolders, topFoldersItems);

#ifdef __WXMSW__
    for (auto& top_folder : topFolders) {
        top_folder.MakeLower();
    }
#endif

    int where = wxNOT_FOUND;
    wxString fullpath = filename.GetFullPath();

    for (size_t i = 0; i < topFolders.size(); ++i) {
        auto& top_folder = topFolders[i];
        if (fullpath.StartsWith(top_folder)) {
            where = i;
            break;
        }
    }

    // Could not find a folder that matches the filename
    if (where == wxNOT_FOUND)
        return false;

    wxString topFolder = topFolders.Item(where);
    wxDataViewItem closestItem = topFoldersItems.Item(where);
    fullpath.Remove(0, topFolder.length());
    wxFileName left(fullpath);
    wxArrayString parts = left.GetDirs();
    parts.Add(filename.GetFullName());
    clTreeCtrlData* d = GetItemData(closestItem);
    while (!parts.IsEmpty()) {
        if (!d->GetIndex())
            return false; // ??
        wxDataViewItem child = d->GetIndex()->Find(parts.Item(0));
        if (!child.IsOk()) {
            // Probably the this folder was not expanded just yet...
            if (d->IsFolder()) {
                DoExpandItem(closestItem, true);
                // Try again
                child = d->GetIndex()->Find(parts.Item(0));
                if (!child.IsOk()) {
                    return false;
                }
            }
        }
        closestItem = child;
        d = GetItemData(closestItem);
        parts.RemoveAt(0);
    }

    if (parts.IsEmpty()) {
        // we found our file!
        if (GetTreeCtrl()->IsSelected(closestItem)) {
            GetTreeCtrl()->EnsureVisible(closestItem);
        } else {
            SelectItem(closestItem);
        }
        return true;
    }
    return false;
}

void clTreeCtrlPanel::GetTopLevelFolders(wxArrayString& paths, wxDataViewItemArray& items) const
{
    const clFileViewerTreeCtrl* tree = m_treeCtrl;
    wxDataViewItem root = tree->GetRootItem();
    int count = tree->GetChildCount(root);
    for (int i = 0; i < count; ++i) {
        wxDataViewItem child = tree->GetNthChild(root, i);
        clTreeCtrlData* clientData = GetItemData(child);
        paths.Add(clientData->GetPath());
        items.Add(child);
    }
}

void clTreeCtrlPanel::OnActiveEditorChanged(wxCommandEvent& event)
{
    event.Skip();
    auto editor = clGetManager()->GetActiveEditor();
    if (editor && (m_options & kLinkToEditor)) {
        CallAfter(&clTreeCtrlPanel::ExpandToFileVoid, editor->GetFileName());
    }
}

void clTreeCtrlPanel::UpdateItemDeleted(const wxDataViewItem& item)
{
    wxDataViewItem parent = GetTreeCtrl()->GetItemParent(item);
    CHECK_ITEM_RET(parent);

    clTreeCtrlData* parentData = GetItemData(parent);
    wxString text = GetTreeCtrl()->GetItemText(item);

    // Update the parent cache
    if (parentData->GetIndex()) {
        parentData->GetIndex()->Delete(text);
    }
}

void clTreeCtrlPanel::DoRenameItem(const wxDataViewItem& item, const wxString& oldname, const wxString& newname)
{
    // Update the UI + client data
    clTreeCtrlData* d = GetItemData(item);
    if (d->IsFile()) {
        wxFileName fn(d->GetPath());
        fn.SetFullName(newname);
        d->SetPath(fn.GetFullPath());
    } else if (d->IsFolder()) {
        // FIXME:
    }

    GetTreeCtrl()->SetItemText(item, newname);

    // Update the parent's cache
    wxDataViewItem parent = GetTreeCtrl()->GetItemParent(item);
    CHECK_ITEM_RET(parent);
    clTreeCtrlData* parentData = GetItemData(parent);

    // Update the parent cache
    if (parentData->GetIndex()) {
        parentData->GetIndex()->Delete(oldname);
        parentData->GetIndex()->Add(newname, item);
    }
}

void clTreeCtrlPanel::OnFindInFilesFolder(wxCommandEvent& event)
{
    wxArrayString folders, files;
    GetSelections(folders, files);

    if (folders.IsEmpty())
        return;
    clGetManager()->OpenFindInFileForPaths(folders);
}

void clTreeCtrlPanel::OnOpenContainingFolder(wxCommandEvent& event)
{
    wxDataViewItem item = GetTreeCtrl()->GetCurrentItem();
    clTreeCtrlData* cd = GetItemData(item);

    if (cd && cd->IsFolder()) {
        FileUtils::OpenFileExplorer(cd->GetPath());
    } else if (cd && cd->IsFile()) {
        wxFileName fn(cd->GetPath());
        FileUtils::OpenFileExplorerAndSelect(fn);
    }
}

void clTreeCtrlPanel::OnOpenShellFolder(wxCommandEvent& event)
{
    wxDataViewItem item = GetTreeCtrl()->GetCurrentItem();
    clTreeCtrlData* cd = GetItemData(item);

    if (cd && cd->IsFolder()) {
        FileUtils::OpenTerminal(cd->GetPath());
    } else if (cd && cd->IsFile()) {
        wxFileName fn(cd->GetPath());
        FileUtils::OpenTerminal(fn.GetPath());
    }
}

void clTreeCtrlPanel::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    if (GetConfig()) {
        wxArrayString pinnedFolders;
        m_options = GetConfig()->Read("FileExplorer/Options", m_options);
        m_options |= kShowHiddenFiles;
        m_options |= kShowHiddenFolders;

        pinnedFolders = GetConfig()->Read("ExplorerFolders", pinnedFolders);
        for (size_t i = 0; i < pinnedFolders.size(); ++i) {
            AddFolder(pinnedFolders.Item(i));
        }
    }
}

void clTreeCtrlPanel::Clear()
{
    wxDataViewItem root = GetTreeCtrl()->GetRootItem();
    // DoCloseFolder() removes the item from the tree, so always take the first remaining child
    while (GetTreeCtrl()->GetChildCount(root) > 0) {
        DoCloseFolder(GetTreeCtrl()->GetNthChild(root, 0));
    }
}

void clTreeCtrlPanel::DoCloseFolder(const wxDataViewItem& item)
{
    CHECK_ITEM_RET(item);
    if (!IsTopLevelFolder(item))
        return;

    if (GetConfig()) {
        // If this folder is a pinned one, remove it
        wxArrayString pinnedFolders;
        pinnedFolders = GetConfig()->Read("ExplorerFolders", pinnedFolders);
        clTreeCtrlData* d = GetItemData(item);
        int where = pinnedFolders.Index(d->GetPath());
        if (where != wxNOT_FOUND) {
            pinnedFolders.RemoveAt(where);
        }
        GetConfig()->Write("ExplorerFolders", pinnedFolders);
    }
    // Now, delete the item
    GetTreeCtrl()->DeleteItem(item);

    ToggleView();
}

void clTreeCtrlPanel::ToggleView()
{
    wxArrayString paths;
    wxDataViewItemArray items;
    GetTopLevelFolders(paths, items);

    if (paths.IsEmpty()) {
        GetTreeCtrl()->Hide();
        m_defaultView->Show();
    } else {
        GetTreeCtrl()->Show();
        m_defaultView->Hide();
    }
    GetSizer()->Layout();
}

void clTreeCtrlPanel::RefreshSelections()
{
    wxArrayString paths, files;
    wxDataViewItemArray items, fileItems;
    GetSelections(paths, items, files, fileItems);
    if (items.IsEmpty()) {
        return;
    }

    // If we have a top level folder, ignore any non top level folder
    bool hasTopLevelFolder = false;
    for (size_t i = 0; i < items.GetCount(); ++i) {
        if (IsTopLevelFolder(items.Item(i))) {
            hasTopLevelFolder = true;
            break;
        }
    }

    if (!hasTopLevelFolder) {
        // Non top level folders
        for (size_t i = 0; i < items.GetCount(); ++i) {
            RefreshNonTopLevelFolder(items.Item(i));
        }
    } else {
        // Close the selected folders
        std::vector<std::pair<wxString, bool>> topFolders;
        for (size_t i = 0; i < items.GetCount(); ++i) {
            topFolders.push_back(std::make_pair(paths.Item(i), GetTreeCtrl()->IsExpanded(items.Item(i))));
            DoCloseFolder(items.Item(i));
        }

        // Re-add them
        for (size_t i = 0; i < topFolders.size(); ++i) {
            wxDataViewItem itemFolder = DoAddFolder(GetTreeCtrl()->GetRootItem(), topFolders.at(i).first);
            DoExpandItem(itemFolder, topFolders.at(i).second);
        }
        ToggleView();
    }
}

void clTreeCtrlPanel::OnRefresh(wxCommandEvent& event)
{
    wxUnusedVar(event);
    RefreshSelections();

    if (clGetManager()->GetActiveEditor() && (m_options & kLinkToEditor)) {
        CallAfter(&clTreeCtrlPanel::ExpandToFileVoid, clGetManager()->GetActiveEditor()->GetFileName());
    }
}

void clTreeCtrlPanel::SetNewFileTemplate(const wxString& newfile, size_t charsToHighlight)
{
    m_newfileTemplate = newfile;
    m_newfileTemplateHighlightLen = charsToHighlight;
}

void clTreeCtrlPanel::OnOpenFolder(wxCommandEvent& event)
{
    wxCommandEvent eventOpenFolder(wxEVT_MENU, XRCID("open_folder"));
    wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(eventOpenFolder);
}

void clTreeCtrlPanel::OnOpenWithDefaultApplication(wxCommandEvent& event)
{
    wxArrayString folders, files;
    GetSelections(folders, files);

    for (size_t i = 0; i < files.size(); ++i) {
        ::wxLaunchDefaultApplication(files.Item(i));
    }
}

void clTreeCtrlPanel::OnFindInFilesShowing(clFindInFilesEvent& event)
{
    event.Skip();
    if (IsShownOnScreen() && GetTreeCtrl()->HasFocus()) {
        wxArrayString folders, files;
        GetSelections(folders, files);

        wxString paths = event.GetTransientPaths();
        paths.Trim().Trim(false);
        if (!paths.IsEmpty()) {
            paths << "\n";
        }
        for (size_t i = 0; i < folders.size(); ++i) {
            paths << folders.Item(i) << "\n";
        }
        paths.Trim();
        event.SetTransientPaths(paths);
    }
}

void clTreeCtrlPanel::OnLinkEditor(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        m_options |= kLinkToEditor;
    } else {
        m_options &= ~kLinkToEditor;
    }
    if (GetConfig()) {
        GetConfig()->Write("FileExplorer/Options", m_options);
    }

    auto editor = clGetManager()->GetActiveEditor();
    if ((m_options & kLinkToEditor) && editor) {
        // show the active editor's item
        CallAfter(&clTreeCtrlPanel::ExpandToFileVoid, clGetManager()->GetActiveEditor()->GetFileName());
    }
}

void clTreeCtrlPanel::OnLinkEditorUI(wxUpdateUIEvent& event)
{
    auto editor = clGetManager()->GetActiveEditor();
    if (editor && IsFolderOpened()) {
        event.Enable(true);
        event.Check(m_options & kLinkToEditor);
    } else {
        event.Enable(false);
    }
}

bool clTreeCtrlPanel::IsFolderOpened() const
{
    wxArrayString paths;
    wxDataViewItemArray items;
    GetTopLevelFolders(paths, items);
    return !paths.IsEmpty();
}

void clTreeCtrlPanel::RefreshNonTopLevelFolder(const wxDataViewItem& item)
{
    CHECK_ITEM_RET(item);
    clTreeCtrlData* cd = GetItemData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsFolder());

    // Collapse the item if needed
    bool expandItem = GetTreeCtrl()->IsExpanded(item);
    if (expandItem) {
        GetTreeCtrl()->Collapse(item);
    }

    // Clear the item children
    GetTreeCtrl()->DeleteChildren(item);

    // Append the dummy item
    GetTreeCtrl()->AppendItem(item, "Dummy", -1, new clTreeCtrlData(clTreeCtrlData::kDummy));

    // Clear the folder index
    if (cd->GetIndex()) {
        cd->GetIndex()->Clear();
    }

    // Re-expand the item
    if (expandItem) {
        CallAfter(&clTreeCtrlPanel::DoExpandItem, item, true);
    }
}

void clTreeCtrlPanel::OnRenameFolder(wxCommandEvent& event)
{
    wxArrayString files, folders;
    wxDataViewItemArray fileItems, folderItems;
    GetSelections(folders, folderItems, files, fileItems);

    if (folderItems.size() != 1) {
        return;
    }
    wxDataViewItem item = folderItems.Item(0);
    if (IsTopLevelFolder(item)) {
        clWARNING() << "Renaming of top level folder is not supported";
        return;
    }

    clTreeCtrlData* d = GetItemData(item);
    CHECK_PTR_RET(d);
    CHECK_COND_RET(d->IsFolder());
    wxString newName = ::clGetTextFromUser(_("Rename folder"), _("Type the new folder name:"), d->GetName());
    if ((newName == d->GetName()) || newName.IsEmpty()) {
        return;
    }
    wxFileName oldFullPath(d->GetPath(), "");
    wxFileName newFullPath(oldFullPath);
    newFullPath.RemoveLastDir();
    newFullPath.AppendDir(newName);
    LOG_IF_TRACE { clDEBUG1() << "Renaming:" << oldFullPath.GetPath() << "->" << newFullPath.GetPath(); }
    if (::wxRename(oldFullPath.GetPath(), newFullPath.GetPath()) == 0) {
        // Rename was successful
        d->SetPath(newFullPath.GetPath());
        m_treeCtrl->SetItemText(item, newName);
        CallAfter(&clTreeCtrlPanel::RefreshNonTopLevelFolder, item);

    } else {
        clWARNING() << "Failed to rename folder:" << oldFullPath << "->" << newFullPath;
    }
}

void clTreeCtrlPanel::RefreshTree()
{
    // Close the selected folders
    wxArrayString paths;
    wxDataViewItemArray items;
    GetTopLevelFolders(paths, items);

    for (size_t i = 0; i < items.size(); ++i) {
        bool isExpanded = GetTreeCtrl()->IsExpanded(items.Item(i));
        DoCloseFolder(items.Item(i));
        wxDataViewItem itemFolder = DoAddFolder(GetTreeCtrl()->GetRootItem(), paths.Item(i));
        DoExpandItem(itemFolder, isExpanded);
    }

    ToggleView();

    if (clGetManager()->GetActiveEditor() && (m_options & kLinkToEditor)) {
        CallAfter(&clTreeCtrlPanel::ExpandToFileVoid, clGetManager()->GetActiveEditor()->GetFileName());
    }
}

void clTreeCtrlPanel::OnFilesCreated(clFileSystemEvent& event)
{
    event.Skip();
    wxArrayString files, folders;
    wxDataViewItemArray fileItems, folderItems;
    GetSelections(folders, folderItems, files, fileItems);
    for (const auto& item : folderItems) {
        RefreshNonTopLevelFolder(item);
    }
}
