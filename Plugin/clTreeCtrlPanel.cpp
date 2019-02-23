#include "clFileOrFolderDropTarget.h"
#include "clFileSystemEvent.h"
#include "clTreeCtrlPanel.h"
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
#include <imanager.h>
#include <wx/app.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/richmsgdlg.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>
#include "clToolBar.h"

clTreeCtrlPanel::clTreeCtrlPanel(wxWindow* parent)
    : clTreeCtrlPanelBase(parent)
    , m_config(NULL)
    , m_newfileTemplate("Untitled.txt")
    , m_newfileTemplateHighlightLen(wxStrlen("Untitled"))
    , m_options(kShowHiddenFiles | kShowHiddenFolders | kLinkToEditor)
{
    ::MSWSetNativeTheme(GetTreeCtrl());
    m_bmpLoader = clGetManager()->GetStdIcons();
    GetTreeCtrl()->SetFont(DrawingUtils::GetDefaultGuiFont());

    m_toolbar = new clToolBar(this);
    GetSizer()->Insert(0, m_toolbar, 0, wxEXPAND);
    m_toolbar->AddTool(XRCID("link_editor"), _("Link Editor"), m_bmpLoader->LoadBitmap("link_editor"), "",
                       wxITEM_CHECK);
    m_toolbar->Realize();

    m_toolbar->Bind(wxEVT_TOOL, &clTreeCtrlPanel::OnLinkEditor, this, XRCID("link_editor"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &clTreeCtrlPanel::OnLinkEditorUI, this, XRCID("link_editor"));

    // Allow DnD
    SetDropTarget(new clFileOrFolderDropTarget(this));
    GetTreeCtrl()->SetDropTarget(new clFileOrFolderDropTarget(this));
    Bind(wxEVT_DND_FOLDER_DROPPED, &clTreeCtrlPanel::OnFolderDropped, this);
    GetTreeCtrl()->SetBitmaps(m_bmpLoader->GetStandardMimeBitmapListPtr());
    GetTreeCtrl()->AddRoot(_("Folders"), wxNOT_FOUND, wxNOT_FOUND, new clTreeCtrlData(clTreeCtrlData::kRoot));

    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &clTreeCtrlPanel::OnActiveEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &clTreeCtrlPanel::OnInitDone, this);
    EventNotifier::Get()->Bind(wxEVT_FINDINFILES_DLG_SHOWING, &clTreeCtrlPanel::OnFindInFilesShowing, this);
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
}

void clTreeCtrlPanel::OnContextMenu(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    CHECK_ITEM_RET(item);

    clTreeCtrlData* cd = GetItemData(item);
    if(cd && cd->IsFolder()) {
        // Prepare a folder context menu
        wxMenu menu;
        menu.Append(wxID_REFRESH, _("Refresh"));
        menu.AppendSeparator();

        menu.Append(XRCID("tree_ctrl_new_folder"), _("New Folder"));
        menu.Append(XRCID("tree_ctrl_new_file"), _("New File"));
        if(!IsTopLevelFolder(item)) {
            menu.AppendSeparator();
            menu.Append(XRCID("tree_ctrl_delete_folder"), _("Delete"));
        }

        if(!IsTopLevelFolder(item)) {
            menu.AppendSeparator();
            menu.Append(XRCID("tree_ctrl_rename_folder"), _("Rename..."));
        }

        menu.AppendSeparator();
        menu.Append(XRCID("tree_ctrl_find_in_files_folder"), _("Find in Files"));
        menu.AppendSeparator();
        menu.Append(XRCID("tree_ctrl_open_containig_folder"), _("Open Containing Folder"));
        menu.Append(XRCID("tree_ctrl_open_shell_folder"), _("Open Shell"));

        if(IsTopLevelFolder(item)) {
            menu.AppendSeparator();
            menu.Append(XRCID("tree_ctrl_close_folder"), _("Close"));
        }

        // Now that we added the basic menu, let the plugin
        // adjust it
        wxArrayString files, folders;
        GetSelections(folders, files);

        clContextMenuEvent dirMenuEvent(wxEVT_CONTEXT_MENU_FOLDER);
        dirMenuEvent.SetEventObject(this);
        dirMenuEvent.SetMenu(&menu);
        dirMenuEvent.SetPath(cd->GetPath());
        EventNotifier::Get()->ProcessEvent(dirMenuEvent);

        // Connect events
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnCloseFolder, this, XRCID("tree_ctrl_close_folder"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnNewFolder, this, XRCID("tree_ctrl_new_folder"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnNewFile, this, XRCID("tree_ctrl_new_file"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnDeleteSelections, this, XRCID("tree_ctrl_delete_folder"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnRenameFolder, this, XRCID("tree_ctrl_rename_folder"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnFindInFilesFolder, this, XRCID("tree_ctrl_find_in_files_folder"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnOpenContainingFolder, this, XRCID("tree_ctrl_open_containig_folder"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnOpenShellFolder, this, XRCID("tree_ctrl_open_shell_folder"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnRefresh, this, wxID_REFRESH);
        PopupMenu(&menu);

    } else if(cd && cd->IsFile()) {
        // File context menu
        // Prepare a folder context menu
        wxMenu menu;

        menu.Append(XRCID("tree_ctrl_open_file"), _("Open"));
        menu.Append(XRCID("tree_ctrl_rename_file"), _("Rename"));
        menu.AppendSeparator();
        menu.Append(XRCID("tree_ctrl_open_with_default_app"), _("Open with default application"));
        menu.AppendSeparator();
        menu.Append(XRCID("tree_ctrl_delete_file"), _("Delete"));

        menu.AppendSeparator();
        menu.Append(XRCID("tree_ctrl_open_containig_folder"), _("Open Containing Folder"));
        menu.Append(XRCID("tree_ctrl_open_shell_folder"), _("Open Shell"));

        // Now that we added the basic menu, let the plugin
        // adjust it
        wxArrayString files, folders;
        GetSelections(folders, files);

        clContextMenuEvent fileMenuEvent(wxEVT_CONTEXT_MENU_FILE);
        fileMenuEvent.SetEventObject(this);
        fileMenuEvent.SetMenu(&menu);
        fileMenuEvent.SetStrings(files);
        EventNotifier::Get()->ProcessEvent(fileMenuEvent);

        // Connect events
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnOpenFile, this, XRCID("tree_ctrl_open_file"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnRenameFile, this, XRCID("tree_ctrl_rename_file"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnDeleteSelections, this, XRCID("tree_ctrl_delete_file"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnOpenWithDefaultApplication, this,
                  XRCID("tree_ctrl_open_with_default_app"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnOpenContainingFolder, this, XRCID("tree_ctrl_open_containig_folder"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnOpenShellFolder, this, XRCID("tree_ctrl_open_shell_folder"));

        PopupMenu(&menu);
    } else {
        // context menu elsewhere
        wxMenu menu;
        menu.Append(XRCID("open_folder_default_page"), _("Open Folder..."));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnOpenFolder, this, XRCID("open_folder_default_page"));
        PopupMenu(&menu);
    }
}

void clTreeCtrlPanel::OnItemActivated(wxTreeEvent& event)
{
    event.Skip();
    wxCommandEvent dummy;
    OnOpenFile(dummy);
}

void clTreeCtrlPanel::OnItemExpanding(wxTreeEvent& event)
{
    event.Skip();
    wxTreeItemId item = event.GetItem();
    CHECK_ITEM_RET(item);
    DoExpandItem(item, true);
}

void clTreeCtrlPanel::OnFolderDropped(clCommandEvent& event)
{
    const wxArrayString& folders = event.GetStrings();
    for(size_t i = 0; i < folders.size(); ++i) {
        AddFolder(folders.Item(i));
    }
    ::clGetManager()->GetWorkspaceView()->SelectPage(GetViewName());
}

void clTreeCtrlPanel::DoExpandItem(const wxTreeItemId& parent, bool expand)
{
    clTreeCtrlData* cd = GetItemData(parent);
    CHECK_PTR_RET(cd);

    // we only know how to expand folders...
    if(!cd->IsFolder()) return;
    wxString folderPath = cd->GetPath();

    if(!m_treeCtrl->ItemHasChildren(parent)) return;
    // Test the first item for dummy

    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeCtrl->GetFirstChild(parent, cookie);
    CHECK_ITEM_RET(child);

    cd = GetItemData(child);
    CHECK_PTR_RET(cd);

    // If not dummy - already expanded do nothing here
    if(!cd->IsDummy()) return;

    m_treeCtrl->Delete(child);
    cd = NULL;

    // Get the top level folders
    wxDir dir(folderPath);
    if(!dir.IsOpened()) return;
    wxBusyCursor bc;
    wxString filename;
    bool cont = dir.GetFirst(&filename, wxEmptyString);
    while(cont) {
        wxFileName fullpath(folderPath, filename);
        // Make sure we don't add the same folder twice
        if(wxFileName::DirExists(fullpath.GetFullPath())) {
            // a folder
            if(!(m_options & kShowHiddenFolders) && FileUtils::IsHidden(fullpath)) {
                cont = dir.GetNext(&filename);
                continue;
            }
            DoAddFolder(parent, fullpath.GetFullPath());
        } else {
            // make sure we don't add the same file twice
            if(!(m_options & kShowHiddenFiles) && FileUtils::IsHidden(fullpath)) {
                cont = dir.GetNext(&filename);
                continue;
            }
            DoAddFile(parent, fullpath.GetFullPath());
        }
        cont = dir.GetNext(&filename);
    }

    // Sort the parent
    if(GetTreeCtrl()->ItemHasChildren(parent)) {
        if(expand) { GetTreeCtrl()->Expand(parent); }
        SelectItem(parent);
    }
}

clTreeCtrlData* clTreeCtrlPanel::GetItemData(const wxTreeItemId& item) const
{
    CHECK_ITEM_RET_NULL(item);
    clTreeCtrlData* cd = dynamic_cast<clTreeCtrlData*>(m_treeCtrl->GetItemData(item));
    return cd;
}

void clTreeCtrlPanel::AddFolder(const wxString& path)
{
    wxTreeItemId itemFolder = DoAddFolder(GetTreeCtrl()->GetRootItem(), path);
    DoExpandItem(itemFolder, false);
    ToggleView();
}

wxTreeItemId clTreeCtrlPanel::DoAddFile(const wxTreeItemId& parent, const wxString& path)
{
    wxFileName filename(path);
    clTreeCtrlData* parentData = GetItemData(parent);
    if(!parentData) { return wxTreeItemId(); }
    if(parentData->GetIndex()) {
        wxTreeItemId cachedItem = parentData->GetIndex()->Find(filename.GetFullName());
        if(cachedItem.IsOk()) {
            // the item already exists, return it
            return cachedItem;
        }
    }

    clTreeCtrlData* cd = new clTreeCtrlData(clTreeCtrlData::kFile);
    cd->SetPath(filename.GetFullPath());

    int imgIdx = m_bmpLoader->GetMimeImageId(filename.GetFullName());
    if(imgIdx == wxNOT_FOUND) { imgIdx = m_bmpLoader->GetMimeImageId(FileExtManager::TypeText); }
    wxTreeItemId fileItem = GetTreeCtrl()->AppendItem(parent, filename.GetFullName(), imgIdx, imgIdx, cd);
    // Add this entry to the index
    if(parentData->GetIndex()) { parentData->GetIndex()->Add(filename.GetFullName(), fileItem); }
    return fileItem;
}

wxTreeItemId clTreeCtrlPanel::DoAddFolder(const wxTreeItemId& parent, const wxString& path)
{
    // If we already have this folder opened, dont re-add it
    wxArrayString topFolders;
    wxArrayTreeItemIds topFoldersItems;
    GetTopLevelFolders(topFolders, topFoldersItems);
    int where = topFolders.Index(path);
    if(where != wxNOT_FOUND) { return topFoldersItems.Item(where); }

    wxFileName filename(path, "");
    clTreeCtrlData* cd = new clTreeCtrlData(clTreeCtrlData::kFolder);
    cd->SetPath(path);
    wxString displayName = (m_options & kShowRootFullPath) ? cd->GetPath() : cd->GetName();

    clTreeCtrlData* parentData = GetItemData(parent);
    if(!parentData) {
        wxDELETE(cd);
        return wxTreeItemId();
    }

    // Check the index before adding new folder
    if(parentData->GetIndex()) {
        wxTreeItemId cachedItem = parentData->GetIndex()->Find(displayName);
        if(cachedItem.IsOk()) {
            // the item already exists, return it
            wxDELETE(cd);
            return cachedItem;
        }
    }

    int imgIdx = m_bmpLoader->GetMimeImageId(FileExtManager::TypeFolder);
    int imgOpenedIDx = m_bmpLoader->GetMimeImageId(FileExtManager::TypeFolderExpanded);
    wxTreeItemId itemFolder = GetTreeCtrl()->AppendItem(parent, displayName, imgIdx, imgOpenedIDx, cd);

    // Add this entry to the index
    if(parentData->GetIndex()) { parentData->GetIndex()->Add(displayName, itemFolder); }

    // Append the dummy item
    GetTreeCtrl()->AppendItem(itemFolder, "Dummy", -1, -1, new clTreeCtrlData(clTreeCtrlData::kDummy));

    // Pin this folder
    if(GetConfig() && IsTopLevelFolder(itemFolder)) {
        wxArrayString pinnedFolders;
        pinnedFolders = GetConfig()->Read("ExplorerFolders", pinnedFolders);
        int where = pinnedFolders.Index(cd->GetPath());
        if(where == wxNOT_FOUND) { pinnedFolders.Add(cd->GetPath()); }
        GetConfig()->Write("ExplorerFolders", pinnedFolders);
    }
    return itemFolder;
}

void clTreeCtrlPanel::GetSelections(wxArrayString& folders, wxArrayString& files)
{
    wxArrayTreeItemIds d1, d2;
    GetSelections(folders, d1, files, d2);
}

TreeItemInfo clTreeCtrlPanel::GetSelectedItemInfo()
{
    TreeItemInfo info;
    wxArrayString folders, files;
    GetSelections(folders, files);

    folders.insert(folders.end(), files.begin(), files.end());
    if(folders.empty()) return info;

    info.m_paths = folders;
    info.m_item = wxTreeItemId();
    return info;
}

void clTreeCtrlPanel::OnCloseFolder(wxCommandEvent& event)
{
    wxArrayString paths, files;
    wxArrayTreeItemIds items, fileItems;
    GetSelections(paths, items, files, fileItems);

    if(items.IsEmpty()) return;
    for(size_t i = 0; i < items.GetCount(); ++i) {
        DoCloseFolder(items.Item(i));
    }
}

bool clTreeCtrlPanel::IsTopLevelFolder(const wxTreeItemId& item)
{
    clTreeCtrlData* cd = GetItemData(item);
    return (cd && cd->IsFolder() && GetTreeCtrl()->GetItemParent(item) == GetTreeCtrl()->GetRootItem());
}

void clTreeCtrlPanel::OnNewFile(wxCommandEvent& event)
{
    wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
    clTreeCtrlData* cd = GetItemData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsFolder());

    wxString filename = ::clGetTextFromUser(_("New File"), _("Set the file name:"), m_newfileTemplate);
    if(filename.IsEmpty()) return; // user cancelled

    wxFileName file(cd->GetPath(), filename);

    // Write the file content
    if(!FileUtils::WriteFileContent(file, "")) return;
    wxTreeItemId newFile = DoAddFile(item, file.GetFullPath());
    GetTreeCtrl()->SortChildren(item);
    // Open the file in the editor
    clGetManager()->OpenFile(file.GetFullPath());
    CallAfter(&clTreeCtrlPanel::SelectItem, newFile);

    // Notify about file creation
    clFileSystemEvent fsEvent(wxEVT_FILE_CREATED);
    fsEvent.SetPath(file.GetFullPath());
    fsEvent.SetFileName(file.GetFullName());
    EventNotifier::Get()->AddPendingEvent(fsEvent);
}

void clTreeCtrlPanel::OnNewFolder(wxCommandEvent& event)
{
    wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
    clTreeCtrlData* cd = GetItemData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsFolder());

    wxString foldername = ::clGetTextFromUser(_("New Folder"), _("Set the folder name:"), "New Folder");
    if(foldername.IsEmpty()) return; // user cancelled

    wxFileName file(cd->GetPath(), "");
    file.AppendDir(foldername);

    // Create the folder
    wxFileName::Mkdir(file.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    // Add it to the tree view
    wxTreeItemId newFile = DoAddFolder(item, file.GetPath());
    GetTreeCtrl()->SortChildren(item);
    CallAfter(&clTreeCtrlPanel::SelectItem, newFile);

    // Notify about folder creation
    clFileSystemEvent fsEvent(wxEVT_FOLDER_CREATED);
    fsEvent.SetPath(file.GetPath());
    EventNotifier::Get()->AddPendingEvent(fsEvent);
}

void clTreeCtrlPanel::GetSelections(wxArrayString& folders, wxArrayTreeItemIds& folderItems, wxArrayString& files,
                                    wxArrayTreeItemIds& fileItems)
{
    folders.clear();
    files.clear();
    folderItems.clear();
    fileItems.clear();

    wxArrayTreeItemIds items;
    if(GetTreeCtrl()->GetSelections(items)) {
        for(size_t i = 0; i < items.size(); ++i) {
            clTreeCtrlData* cd = GetItemData(items.Item(i));
            if(cd) {
                if(cd->IsFile()) {
                    files.Add(cd->GetPath());
                    fileItems.Add(items.Item(i));
                } else if(cd->IsFolder()) {
                    folders.Add(cd->GetPath());
                    folderItems.Add(items.Item(i));
                }
            }
        }
    }
}

void clTreeCtrlPanel::SelectItem(const wxTreeItemId& item)
{
    CHECK_ITEM_RET(item);
    wxArrayTreeItemIds items;
    if(GetTreeCtrl()->GetSelections(items)) {
        for(size_t i = 0; i < items.size(); ++i) {
            GetTreeCtrl()->SelectItem(items.Item(i), false);
        }
    }

    GetTreeCtrl()->SelectItem(item);
    // GetTreeCtrl()->SetFocusedItem(item);
    GetTreeCtrl()->EnsureVisible(item);
}

struct FileOrFolder {
    wxTreeItemId item;
    bool folder;
    wxString path;
};

void clTreeCtrlPanel::OnDeleteSelections(wxCommandEvent& event)
{
    wxArrayString folders, files;
    wxArrayTreeItemIds folderItems, fileItems;
    GetSelections(folders, folderItems, files, fileItems);
    if(files.empty() && folders.empty()) return;

    std::set<wxTreeItemId> selectedItems;
    std::vector<FileOrFolder> v;

    selectedItems.insert(folderItems.begin(), folderItems.end());
    selectedItems.insert(fileItems.begin(), fileItems.end());

    // loop over the selections and remove all items that their parents
    // also exists in the selected items list
    for(size_t i = 0; i < folderItems.size(); ++i) {
        wxTreeItemId item = folderItems.Item(i);
        bool foundParent = false;
        wxTreeItemId itemParent = GetTreeCtrl()->GetItemParent(item);
        while(itemParent.IsOk()) {
            if(selectedItems.count(itemParent)) {
                // item's parent is in the list, don't delete it as it will get deleted
                // by its parent
                foundParent = true;
                break;
            }
            itemParent = GetTreeCtrl()->GetItemParent(itemParent);
        }

        if(!foundParent) {
            FileOrFolder fof;
            fof.folder = true;
            fof.item = item;
            fof.path = folders.Item(i);
            v.push_back(fof);
        }
    }

    for(size_t i = 0; i < fileItems.size(); ++i) {
        wxTreeItemId item = fileItems.Item(i);
        bool foundParent = false;
        wxTreeItemId itemParent = GetTreeCtrl()->GetItemParent(item);
        while(itemParent.IsOk()) {
            if(selectedItems.count(itemParent)) {
                // item's parent is in the list, don't delete it as it will get deleted
                // by its parent
                foundParent = true;
                break;
            }
            itemParent = GetTreeCtrl()->GetItemParent(itemParent);
        }

        if(!foundParent) {
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

    wxRichMessageDialog dialog(EventNotifier::Get()->TopFrame(), message, _("Confirm"),
                               wxYES_NO | wxCANCEL | wxNO_DEFAULT | wxCENTER | wxICON_WARNING);

    wxWindowUpdateLocker locker(GetTreeCtrl());
    wxArrayTreeItemIds deletedItems;
    wxArrayString deletedFiles, deletedFolders;
    if(dialog.ShowModal() == wxID_YES) {
        wxLogNull nl;
        for(size_t i = 0; i < v.size(); ++i) {
            if(v.at(i).folder) {
                if(wxFileName::Rmdir(v.at(i).path, wxPATH_RMDIR_RECURSIVE)) {
                    deletedItems.Add(v.at(i).item);
                    deletedFolders.Add(v.at(i).path);
                }
            } else {
                if(clRemoveFile(v.at(i).path)) {
                    deletedItems.Add(v.at(i).item);
                    deletedFiles.Add(v.at(i).path);
                }
            }
        }
    }

    // Notify about the folder/files deletion
    {
        clFileSystemEvent evt(wxEVT_FILE_DELETED);
        evt.SetPaths(deletedFiles);
        evt.SetEventObject(this);
        EventNotifier::Get()->AddPendingEvent(evt);
    }
    {
        clFileSystemEvent evt(wxEVT_FOLDER_DELETED);
        evt.SetPaths(deletedFolders);
        evt.SetEventObject(this);
        EventNotifier::Get()->AddPendingEvent(evt);
    }

    // Update the UI
    for(size_t i = 0; i < deletedItems.size(); ++i) {
        // Before we delete the item from the tree, update the parent cache
        UpdateItemDeleted(deletedItems.Item(i));
        // And now remove the item from the tree
        GetTreeCtrl()->Delete(deletedItems.Item(i));
    }
}

void clTreeCtrlPanel::OnOpenFile(wxCommandEvent& event)
{
    wxArrayString folders, files;
    GetSelections(folders, files);

    for(size_t i = 0; i < files.size(); ++i) {
        // Fire an event before resolving to the default action
        clCommandEvent fileEvent(wxEVT_TREE_ITEM_FILE_ACTIVATED);
        fileEvent.SetEventObject(this);
        fileEvent.SetFileName(files.Item(i));
        if(!EventNotifier::Get()->ProcessEvent(fileEvent)) { clGetManager()->OpenFile(files.Item(i)); }
    }
}

void clTreeCtrlPanel::OnRenameFile(wxCommandEvent& event)
{
    wxArrayString files, folders;
    wxArrayTreeItemIds fileItems, folderItems;
    GetSelections(folders, folderItems, files, fileItems);

    if(files.empty()) return;

    // Prompt and rename each file
    for(size_t i = 0; i < files.size(); ++i) {
        wxFileName oldname(files.Item(i));

        wxString newname =
            ::clGetTextFromUser(_("Rename File"), _("New Name:"), oldname.GetFullName(), wxStrlen(oldname.GetName()));
        if(!newname.IsEmpty() && (newname != oldname.GetFullName())) {
            clTreeCtrlData* d = GetItemData(fileItems.Item(i));
            if(d) {
                wxFileName oldpath = d->GetPath();
                wxFileName newpath = oldpath;
                newpath.SetFullName(newname);
                if(::wxRenameFile(oldpath.GetFullPath(), newpath.GetFullPath(), false)) {
                    DoRenameItem(fileItems.Item(i), oldname.GetFullName(), newname);
                }
            }
        }
    }
}

bool clTreeCtrlPanel::ExpandToFile(const wxFileName& filename)
{
    wxArrayString topFolders;
    wxArrayTreeItemIds topFoldersItems;
    GetTopLevelFolders(topFolders, topFoldersItems);

    int where = wxNOT_FOUND;
    wxString fullpath = filename.GetFullPath();
    for(size_t i = 0; i < topFolders.size(); ++i) {
        if(fullpath.StartsWith(topFolders.Item(i))) {
            where = i;
            break;
        }
    }

    // Could not find a folder that matches the filename
    if(where == wxNOT_FOUND) return false;
    wxString topFolder = topFolders.Item(where);
    wxTreeItemId closestItem = topFoldersItems.Item(where);
    fullpath.Remove(0, topFolder.length());
    wxFileName left(fullpath);

    wxArrayString parts = left.GetDirs();
    parts.Add(filename.GetFullName());
    clTreeCtrlData* d = GetItemData(closestItem);
    while(!parts.IsEmpty()) {
        if(!d->GetIndex()) return false; // ??
        wxTreeItemId child = d->GetIndex()->Find(parts.Item(0));
        if(!child.IsOk()) {
            // Probably the this folder was not expanded just yet...
            if(d->IsFolder()) {
                DoExpandItem(closestItem, true);
                // Try again
                child = d->GetIndex()->Find(parts.Item(0));
                if(!child.IsOk()) { return false; }
            }
        }
        closestItem = child;
        d = GetItemData(closestItem);
        parts.RemoveAt(0);
    }

    if(parts.IsEmpty()) {
        // we found our file!
        SelectItem(closestItem);
        return true;
    }
    return false;
}

void clTreeCtrlPanel::GetTopLevelFolders(wxArrayString& paths, wxArrayTreeItemIds& items) const
{
    wxTreeItemIdValue cookie;
    const clTreeCtrl* tree = m_treeCtrl;
    wxTreeItemId child = tree->GetFirstChild(tree->GetRootItem(), cookie);
    while(child.IsOk()) {
        clTreeCtrlData* clientData = GetItemData(child);
        paths.Add(clientData->GetPath());
        items.Add(child);
        child = tree->GetNextChild(tree->GetRootItem(), cookie);
    }
}

void clTreeCtrlPanel::OnActiveEditorChanged(wxCommandEvent& event)
{
    event.Skip();
    if(clGetManager()->GetActiveEditor() && (m_options & kLinkToEditor)) {
        ExpandToFile(clGetManager()->GetActiveEditor()->GetFileName());
    }
}

void clTreeCtrlPanel::UpdateItemDeleted(const wxTreeItemId& item)
{
    wxTreeItemId parent = GetTreeCtrl()->GetItemParent(item);
    CHECK_ITEM_RET(parent);

    clTreeCtrlData* parentData = GetItemData(parent);
    wxString text = GetTreeCtrl()->GetItemText(item);

    // Update the parent cache
    if(parentData->GetIndex()) { parentData->GetIndex()->Delete(text); }
}

void clTreeCtrlPanel::DoRenameItem(const wxTreeItemId& item, const wxString& oldname, const wxString& newname)
{
    // Update the UI + client data
    clTreeCtrlData* d = GetItemData(item);
    if(d->IsFile()) {
        wxFileName fn(d->GetPath());
        fn.SetFullName(newname);
        d->SetPath(fn.GetFullPath());
    } else if(d->IsFolder()) {
        // FIXME:
    }

    GetTreeCtrl()->SetItemText(item, newname);

    // Update the parent's cache
    wxTreeItemId parent = GetTreeCtrl()->GetItemParent(item);
    CHECK_ITEM_RET(parent);
    clTreeCtrlData* parentData = GetItemData(parent);

    // Update the parent cache
    if(parentData->GetIndex()) {
        parentData->GetIndex()->Delete(oldname);
        parentData->GetIndex()->Add(newname, item);
    }
}

void clTreeCtrlPanel::OnFindInFilesFolder(wxCommandEvent& event)
{
    wxArrayString folders, files;
    GetSelections(folders, files);

    if(folders.IsEmpty()) return;
    clGetManager()->OpenFindInFileForPaths(folders);
}

void clTreeCtrlPanel::OnOpenContainingFolder(wxCommandEvent& event)
{
    wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
    clTreeCtrlData* cd = GetItemData(item);

    if(cd && cd->IsFolder()) {
        FileUtils::OpenFileExplorer(cd->GetPath());
    } else if(cd && cd->IsFile()) {
        wxFileName fn(cd->GetPath());
        FileUtils::OpenFileExplorerAndSelect(fn);
    }
}

void clTreeCtrlPanel::OnOpenShellFolder(wxCommandEvent& event)
{
    wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
    clTreeCtrlData* cd = GetItemData(item);

    if(cd && cd->IsFolder()) {
        FileUtils::OpenTerminal(cd->GetPath());
    } else if(cd && cd->IsFile()) {
        wxFileName fn(cd->GetPath());
        FileUtils::OpenTerminal(fn.GetPath());
    }
}

void clTreeCtrlPanel::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    if(GetConfig()) {
        wxArrayString pinnedFolders;
        m_options = GetConfig()->Read("FileExplorer/Options", m_options);
        pinnedFolders = GetConfig()->Read("ExplorerFolders", pinnedFolders);
        for(size_t i = 0; i < pinnedFolders.size(); ++i) {
            AddFolder(pinnedFolders.Item(i));
        }
    }
}

void clTreeCtrlPanel::Clear()
{
    wxTreeItemIdValue cookie;
    wxTreeItemId item = GetTreeCtrl()->GetFirstChild(GetTreeCtrl()->GetRootItem(), cookie);
    while(item.IsOk()) {
        DoCloseFolder(item);
        item = GetTreeCtrl()->GetNextChild(GetTreeCtrl()->GetRootItem(), cookie);
    }
}

void clTreeCtrlPanel::DoCloseFolder(const wxTreeItemId& item)
{
    CHECK_ITEM_RET(item);
    if(!IsTopLevelFolder(item)) return;

    if(GetConfig()) {
        // If this folder is a pinned one, remove it
        wxArrayString pinnedFolders;
        pinnedFolders = GetConfig()->Read("ExplorerFolders", pinnedFolders);
        clTreeCtrlData* d = GetItemData(item);
        int where = pinnedFolders.Index(d->GetPath());
        if(where != wxNOT_FOUND) { pinnedFolders.RemoveAt(where); }
        GetConfig()->Write("ExplorerFolders", pinnedFolders);
    }
    // Now, delete the item
    GetTreeCtrl()->Delete(item);

    ToggleView();
}

void clTreeCtrlPanel::ToggleView()
{
    wxArrayString paths;
    wxArrayTreeItemIds items;
    GetTopLevelFolders(paths, items);

    if(paths.IsEmpty()) {
        GetTreeCtrl()->Hide();
        m_defaultView->Show();
    } else {
        GetTreeCtrl()->Show();
        m_defaultView->Hide();
    }
    GetSizer()->Layout();
}

void clTreeCtrlPanel::OnRefresh(wxCommandEvent& event)
{
    wxArrayString paths, files;
    wxArrayTreeItemIds items, fileItems;
    GetSelections(paths, items, files, fileItems);
    if(items.IsEmpty()) { return; }

    // If we have a top level folder, ignore any non top level folder
    bool hasTopLevelFolder = false;
    for(size_t i = 0; i < items.GetCount(); ++i) {
        if(IsTopLevelFolder(items.Item(i))) {
            hasTopLevelFolder = true;
            break;
        }
    }

    if(!hasTopLevelFolder) {
        // Non top level folders
        for(size_t i = 0; i < items.GetCount(); ++i) {
            RefreshNonTopLevelFolder(items.Item(i));
        }
    } else {
        // Close the selected folders
        std::vector<std::pair<wxString, bool> > topFolders;
        for(size_t i = 0; i < items.GetCount(); ++i) {
            topFolders.push_back(std::make_pair(paths.Item(i), GetTreeCtrl()->IsExpanded(items.Item(i))));
            DoCloseFolder(items.Item(i));
        }

        // Re-add them
        for(size_t i = 0; i < topFolders.size(); ++i) {
            wxTreeItemId itemFolder = DoAddFolder(GetTreeCtrl()->GetRootItem(), topFolders.at(i).first);
            DoExpandItem(itemFolder, topFolders.at(i).second);
        }
        GetTreeCtrl()->SortChildren(GetTreeCtrl()->GetRootItem());
        ToggleView();
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

    for(size_t i = 0; i < files.size(); ++i) {
        ::wxLaunchDefaultApplication(files.Item(i));
    }
}

void clTreeCtrlPanel::OnFindInFilesShowing(clFindInFilesEvent& event)
{
    event.Skip();
    if(IsShownOnScreen() && GetTreeCtrl()->HasFocus()) {
        wxArrayString folders, files;
        GetSelections(folders, files);

        wxString paths = event.GetTransientPaths();
        paths.Trim().Trim(false);
        if(!paths.IsEmpty()) { paths << "\n"; }
        for(size_t i = 0; i < folders.size(); ++i) {
            paths << folders.Item(i) << "\n";
        }
        paths.Trim();
        event.SetTransientPaths(paths);
    }
}

void clTreeCtrlPanel::OnLinkEditor(wxCommandEvent& event)
{
    if(event.IsChecked()) {
        m_options |= kLinkToEditor;
    } else {
        m_options &= ~kLinkToEditor;
    }
    if(GetConfig()) { GetConfig()->Write("FileExplorer/Options", m_options); }
}

void clTreeCtrlPanel::OnLinkEditorUI(wxUpdateUIEvent& event)
{
    if(IsFolderOpened()) {
        event.Enable(true);
        event.Check(m_options & kLinkToEditor);
    } else {
        event.Enable(false);
    }
}

bool clTreeCtrlPanel::IsFolderOpened() const
{
    wxArrayString paths;
    wxArrayTreeItemIds items;
    GetTopLevelFolders(paths, items);
    return !paths.IsEmpty();
}

void clTreeCtrlPanel::RefreshNonTopLevelFolder(const wxTreeItemId& item)
{
    CHECK_ITEM_RET(item);
    clTreeCtrlData* cd = GetItemData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsFolder());

    // Collpase the item if needed
    bool expandItem = GetTreeCtrl()->IsExpanded(item);
    if(expandItem) { GetTreeCtrl()->Collapse(item); }

    // Clear the item children
    GetTreeCtrl()->DeleteChildren(item);

    // Append the dummy item
    GetTreeCtrl()->AppendItem(item, "Dummy", -1, -1, new clTreeCtrlData(clTreeCtrlData::kDummy));

    // Clear the folder index
    if(cd->GetIndex()) { cd->GetIndex()->Clear(); }

    // Re-expand the item
    if(expandItem) { CallAfter(&clTreeCtrlPanel::DoExpandItem, item, true); }
}

void clTreeCtrlPanel::OnRenameFolder(wxCommandEvent& event)
{
    wxArrayString files, folders;
    wxArrayTreeItemIds fileItems, folderItems;
    GetSelections(folders, folderItems, files, fileItems);

    if(folderItems.size() != 1) { return; }
    wxTreeItemId item = folderItems.Item(0);
    if(IsTopLevelFolder(item)) {
        clWARNING() << "Renaming of top level folder is not supported";
        return;
    }

    clTreeCtrlData* d = GetItemData(item);
    CHECK_PTR_RET(d);
    CHECK_COND_RET(d->IsFolder());
    wxString newName = ::clGetTextFromUser(_("Rename folder"), _("Type the new folder name:"), d->GetName());
    if((newName == d->GetName()) || newName.IsEmpty()) { return; }
    wxFileName oldFullPath(d->GetPath(), "");
    wxFileName newFullPath(oldFullPath);
    newFullPath.RemoveLastDir();
    newFullPath.AppendDir(newName);
    clDEBUG1() << "Renaming:" << oldFullPath.GetPath() << "->" << newFullPath.GetPath();
    if(::wxRename(oldFullPath.GetPath(), newFullPath.GetPath()) == 0) {
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
    wxArrayTreeItemIds items;
    GetTopLevelFolders(paths, items);

    for(size_t i = 0; i < items.size(); ++i) {
        bool isExpanded = GetTreeCtrl()->IsExpanded(items.Item(i));
        DoCloseFolder(items.Item(i));
        wxTreeItemId itemFolder = DoAddFolder(GetTreeCtrl()->GetRootItem(), paths.Item(i));
        DoExpandItem(itemFolder, isExpanded);
    }

    GetTreeCtrl()->SortChildren(GetTreeCtrl()->GetRootItem());
    ToggleView();
}
