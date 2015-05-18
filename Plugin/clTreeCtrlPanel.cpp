#include "clTreeCtrlPanel.h"
#include "clFileOrFolderDropTarget.h"
#include "codelite_events.h"
#include "macros.h"
#include <wx/dir.h>
#include <wx/filename.h>
#include "clWorkspaceView.h"
#include <imanager.h>
#include "globals.h"
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include <wx/richmsgdlg.h>
#include "event_notifier.h"
#include "fileutils.h"

clTreeCtrlPanel::clTreeCtrlPanel(wxWindow* parent)
    : clTreeCtrlPanelBase(parent)
{
    ::MSWSetNativeTheme(GetTreeCtrl());
    // Allow DnD
    SetDropTarget(new clFileOrFolderDropTarget(this));
    GetTreeCtrl()->SetDropTarget(new clFileOrFolderDropTarget(this));
    Bind(wxEVT_DND_FOLDER_DROPPED, &clTreeCtrlPanel::OnFolderDropped, this);
    GetTreeCtrl()->AddRoot(_("Folders"), wxNOT_FOUND, wxNOT_FOUND, new clTreeCtrlData(clTreeCtrlData::kRoot));
    GetTreeCtrl()->AssignImageList(m_bmpLoader.MakeStandardMimeImageList());
}

clTreeCtrlPanel::~clTreeCtrlPanel() { Unbind(wxEVT_DND_FOLDER_DROPPED, &clTreeCtrlPanel::OnFolderDropped, this); }

void clTreeCtrlPanel::OnContextMenu(wxTreeEvent& event)
{
    wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
    clTreeCtrlData* cd = GetItemData(item);

    if(cd && cd->IsFolder()) {

        // Prepare a folder context menu
        wxMenu menu;
        if(IsTopLevelFolder(item)) {
            menu.Append(XRCID("tree_ctrl_close_folder"), _("Close Folder..."));
            menu.AppendSeparator();
        }

        menu.Append(XRCID("tree_ctrl_new_folder"), _("New Folder..."));
        menu.Append(XRCID("tree_ctrl_new_file"), _("New File..."));
        menu.AppendSeparator();
        menu.Append(XRCID("tree_ctrl_delete_folder"), _("Delete"));
        
        // Now that we added the basic menu, let the plugin 
        // adjust it
        wxArrayString files, folders;
        GetSelections(folders, files);
        
        clContextMenuEvent dirMenuEvent(wxEVT_CONTEXT_MENU_FOLDER);
        dirMenuEvent.SetMenu(&menu);
        dirMenuEvent.SetPath(cd->GetPath());
        EventNotifier::Get()->ProcessEvent(dirMenuEvent);

        // Connect events
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnCloseFolder, this, XRCID("tree_ctrl_close_folder"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnNewFolder, this, XRCID("tree_ctrl_new_folder"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnNewFile, this, XRCID("tree_ctrl_new_file"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnDeleteFolder, this, XRCID("tree_ctrl_delete_folder"));
        PopupMenu(&menu);

    } else if(cd && cd->IsFile()) {
        // File context menu
        // Prepare a folder context menu
        wxMenu menu;

        menu.Append(XRCID("tree_ctrl_open_file"), _("Open"));
        menu.Append(XRCID("tree_ctrl_rename_file"), _("Rename"));
        menu.AppendSeparator();
        menu.Append(XRCID("tree_ctrl_delete_file"), _("Delete"));
        
        // Now that we added the basic menu, let the plugin 
        // adjust it
        wxArrayString files, folders;
        GetSelections(folders, files);
        
        clContextMenuEvent fileMenuEvent(wxEVT_CONTEXT_MENU_FILE);
        fileMenuEvent.SetMenu(&menu);
        fileMenuEvent.SetStrings(files);
        EventNotifier::Get()->ProcessEvent(fileMenuEvent);
        
        // Connect events
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnOpenFile, this, XRCID("tree_ctrl_open_file"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnRenameFile, this, XRCID("tree_ctrl_rename_file"));
        menu.Bind(wxEVT_MENU, &clTreeCtrlPanel::OnDeleteFile, this, XRCID("tree_ctrl_delete_file"));
        PopupMenu(&menu);
    }
}

void clTreeCtrlPanel::OnItemActivated(wxTreeEvent& event) {}

void clTreeCtrlPanel::OnItemExpanding(wxTreeEvent& event)
{
    event.Skip();
    wxTreeItemId item = event.GetItem();
    CHECK_ITEM_RET(item);
    DoExpandItem(item);
}

void clTreeCtrlPanel::OnFolderDropped(clCommandEvent& event)
{
    const wxArrayString& folders = event.GetStrings();
    for(size_t i = 0; i < folders.size(); ++i) {
        AddFolder(folders.Item(i));
    }
    ::clGetManager()->GetWorkspaceView()->SelectPage(_("File Viewer"));
}

void clTreeCtrlPanel::DoExpandItem(const wxTreeItemId& parent)
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
        if(wxFileName::DirExists(fullpath.GetFullPath())) {
            // a folder
            DoAddFolder(parent, fullpath.GetFullPath());
        } else {
            DoAddFile(parent, fullpath.GetFullPath());
        }
        cont = dir.GetNext(&filename);
    }

    // Sort the parent
    if(GetTreeCtrl()->ItemHasChildren(parent)) {
        GetTreeCtrl()->SortChildren(parent);
        GetTreeCtrl()->Expand(parent);
        SelectItem(parent);
    }
}

clTreeCtrlData* clTreeCtrlPanel::GetItemData(const wxTreeItemId& item)
{
    CHECK_ITEM_RET_NULL(item);
    clTreeCtrlData* cd = dynamic_cast<clTreeCtrlData*>(m_treeCtrl->GetItemData(item));
    return cd;
}

void clTreeCtrlPanel::AddFolder(const wxString& path)
{
    wxTreeItemId itemFolder = DoAddFolder(GetTreeCtrl()->GetRootItem(), path);
    DoExpandItem(itemFolder);
}

wxTreeItemId clTreeCtrlPanel::DoAddFile(const wxTreeItemId& parent, const wxString& path)
{
    wxFileName filename(path);
    clTreeCtrlData* cd = new clTreeCtrlData(clTreeCtrlData::kFile);
    cd->SetPath(filename.GetFullPath());

    int imgIdx = m_bmpLoader.GetMimeImageId(filename.GetFullName());
    if(imgIdx == wxNOT_FOUND) {
        imgIdx = m_bmpLoader.GetMimeImageId(FileExtManager::TypeText);
    }
    return GetTreeCtrl()->AppendItem(parent, filename.GetFullName(), imgIdx, imgIdx, cd);
}

wxTreeItemId clTreeCtrlPanel::DoAddFolder(const wxTreeItemId& parent, const wxString& path)
{
    // if we already have this folder opened, dont re-add it
    wxTreeItemIdValue cookie;
    wxTreeItemId child = GetTreeCtrl()->GetFirstChild(GetTreeCtrl()->GetRootItem(), cookie);
    while(child.IsOk()) {
        clTreeCtrlData* clientData = GetItemData(child);
        if(path == clientData->GetPath()) {
            return child;
        }
        child = GetTreeCtrl()->GetNextChild(GetTreeCtrl()->GetRootItem(), cookie);
    }
    
    // Add the folder
    clTreeCtrlData* cd = new clTreeCtrlData(clTreeCtrlData::kFolder);
    cd->SetPath(path);

    wxFileName filename(path, "");

    wxString displayName;
    if(filename.GetDirCount() && GetTreeCtrl()->GetRootItem() != parent) {
        displayName << filename.GetDirs().Last();
    } else {
        displayName << path;
    }
    int imgIdx = m_bmpLoader.GetMimeImageId(FileExtManager::TypeFolder);
    wxTreeItemId itemFolder = GetTreeCtrl()->AppendItem(parent, displayName, imgIdx, imgIdx, cd);

    // Append the dummy item
    GetTreeCtrl()->AppendItem(itemFolder, "Dummy", -1, -1, new clTreeCtrlData(clTreeCtrlData::kDummy));
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
    wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
    CHECK_ITEM_RET(item);
    if(!IsTopLevelFolder(item)) return;
    GetTreeCtrl()->Delete(item);
}

bool clTreeCtrlPanel::IsTopLevelFolder(const wxTreeItemId& item)
{
    clTreeCtrlData* cd = GetItemData(item);
    return (cd && cd->IsFolder() && GetTreeCtrl()->GetItemParent(item) == GetTreeCtrl()->GetRootItem());
}

void clTreeCtrlPanel::OnDeleteFolder(wxCommandEvent& event)
{
    wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
    clTreeCtrlData* cd = GetItemData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsFolder());

    wxString message;
    message << _("Are you sure you want to delete folder:\n'") << cd->GetPath() << _("'");

    wxRichMessageDialog dialog(EventNotifier::Get()->TopFrame(),
                               message,
                               _("Confirm"),
                               wxYES_NO | wxCANCEL | wxNO_DEFAULT | wxCENTER | wxICON_WARNING);
    dialog.SetYesNoLabels(_("Delete Folder"), _("Don't Delete"));
    if(dialog.ShowModal() == wxID_YES) {
        if(wxFileName::Rmdir(cd->GetPath(), wxPATH_RMDIR_RECURSIVE)) {
            GetTreeCtrl()->Delete(item);
        }
    }
}

void clTreeCtrlPanel::OnNewFile(wxCommandEvent& event)
{
    wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
    clTreeCtrlData* cd = GetItemData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsFolder());
    
    wxString filename =
        ::clGetTextFromUser(_("New File"), _("Set the file name:"), "Untitled.txt", wxStrlen("Untitled"));
    if(filename.IsEmpty()) return; // user cancelled
    
    wxFileName file(cd->GetPath(), filename);
    
    // Write the file content
    if(!FileUtils::WriteFileContent(file, "")) return;
    wxTreeItemId newFile = DoAddFile(item, file.GetFullPath());
    GetTreeCtrl()->SortChildren(item);
    CallAfter(&clTreeCtrlPanel::SelectItem, newFile);
}

void clTreeCtrlPanel::OnNewFolder(wxCommandEvent& event) 
{
    wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
    clTreeCtrlData* cd = GetItemData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsFolder());
    
    wxString foldername =
        ::clGetTextFromUser(_("New Folder"), _("Set the folder name:"), "New Folder");
    if(foldername.IsEmpty()) return; // user cancelled
    
    wxFileName file(cd->GetPath(), "");
    file.AppendDir(foldername);
    
    // Create the folder
    wxFileName::Mkdir(file.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    
    // Add it to the tree view
    wxTreeItemId newFile = DoAddFolder(item, file.GetPath());
    GetTreeCtrl()->SortChildren(item);
    CallAfter(&clTreeCtrlPanel::SelectItem, newFile);
}

void clTreeCtrlPanel::GetSelections(wxArrayString& folders,
                                    wxArrayTreeItemIds& folderItems,
                                    wxArrayString& files,
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
        for(size_t i=0; i<items.size(); ++i) {
            GetTreeCtrl()->SelectItem(items.Item(i), false);
        }
    }
    
    GetTreeCtrl()->SelectItem(item);
}

void clTreeCtrlPanel::OnDeleteFile(wxCommandEvent& event)
{
}

void clTreeCtrlPanel::OnOpenFile(wxCommandEvent& event)
{
    wxArrayString folders, files;
    GetSelections(folders, files);
    
    for(size_t i=0; i<files.size(); ++i) {
        clGetManager()->OpenFile(files.Item(i));
    }
}

void clTreeCtrlPanel::OnRenameFile(wxCommandEvent& event)
{
}
