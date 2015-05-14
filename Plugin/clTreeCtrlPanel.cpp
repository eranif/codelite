#include "clTreeCtrlPanel.h"
#include "clFileOrFolderDropTarget.h"
#include "codelite_events.h"
#include "macros.h"
#include <wx/dir.h>
#include <wx/filename.h>
#include "clWorkspaceView.h"
#include <imanager.h>
#include "globals.h"

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

void clTreeCtrlPanel::OnContextMenu(wxTreeEvent& event) {}

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
    GetTreeCtrl()->SortChildren(parent);
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
    wxArrayTreeItemIds items;
    if(GetTreeCtrl()->GetSelections(items)) {
        for(size_t i=0; i<items.size(); ++i) {
            clTreeCtrlData* cd = GetItemData(items.Item(i));
            if(cd) {
                if(cd->IsFile()) {
                    files.Add(cd->GetPath());
                } else if(cd->IsFolder()) {
                    folders.Add(cd->GetPath());
                }
            }
        }
    } else {
        folders.clear();
        files.clear();
    }
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
