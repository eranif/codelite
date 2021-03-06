#include "clRemoteDirCtrl.hpp"
#if USE_SFTP
#include "SFTPClientData.hpp"
#include "bitmap_loader.h"
#include "clSFTPManager.hpp"
#include "event_notifier.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "sftp_item_comparator.hpp"
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/xrc/xmlres.h>

clRemoteDirCtrl::clRemoteDirCtrl(wxWindow* parent)
    : wxPanel(parent)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_treeCtrl = new clThemedTreeCtrl(this);
    m_treeCtrl->Bind(wxEVT_CONTEXT_MENU, &clRemoteDirCtrl::OnContextMenu, this);
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_EXPANDING, &clRemoteDirCtrl::OnItemExpanding, this);
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_ACTIVATED, &clRemoteDirCtrl::OnItemActivated, this);

    GetSizer()->Add(m_treeCtrl, 1, wxEXPAND);
    GetSizer()->Fit(this);

    // events
    EventNotifier::Get()->Bind(wxEVT_BITMAPS_UPDATED, [this](clCommandEvent& e) {
        e.Skip();
        m_treeCtrl->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
    });

    auto SortFunc = [&](const wxTreeItemId& itemA, const wxTreeItemId& itemB) {
        clRemoteDirCtrlItemData* a = static_cast<clRemoteDirCtrlItemData*>(GetItemData(itemA));
        clRemoteDirCtrlItemData* b = static_cast<clRemoteDirCtrlItemData*>(GetItemData(itemB));
        if(a->IsFolder() && !b->IsFolder())
            return true;
        else if(b->IsFolder() && !a->IsFolder())
            return false;
        // same kind
        return (a->GetFullName().CmpNoCase(b->GetFullName()) < 0);
    };
    m_treeCtrl->SetSortFunction(SortFunc);
}

clRemoteDirCtrl::~clRemoteDirCtrl()
{
    m_treeCtrl->Unbind(wxEVT_CONTEXT_MENU, &clRemoteDirCtrl::OnContextMenu, this);
    m_treeCtrl->Unbind(wxEVT_TREE_ITEM_EXPANDING, &clRemoteDirCtrl::OnItemExpanding, this);
    m_treeCtrl->Unbind(wxEVT_TREE_ITEM_ACTIVATED, &clRemoteDirCtrl::OnItemActivated, this);
}

clRemoteDirCtrlItemData* clRemoteDirCtrl::GetItemData(const wxTreeItemId& item)
{
    CHECK_ITEM_RET_NULL(item);
    clRemoteDirCtrlItemData* cd = dynamic_cast<clRemoteDirCtrlItemData*>(m_treeCtrl->GetItemData(item));
    return cd;
}

void clRemoteDirCtrl::OnItemActivated(wxTreeEvent& event) {}

void clRemoteDirCtrl::OnItemExpanding(wxTreeEvent& event) { DoExpandItem(event.GetItem()); }

void clRemoteDirCtrl::Open(const wxString& path, const SSHAccountInfo& account)
{
    if(!clSFTPManager::Get().AddConnection(account)) {
        return;
    }
    m_account = account;
    m_treeCtrl->DeleteAllItems();

    // add the root item
    clRemoteDirCtrlItemData* cd = new clRemoteDirCtrlItemData(path);
    cd->SetFolder();

    wxTreeItemId root = m_treeCtrl->AddRoot(
        path, clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolder), wxNOT_FOUND, cd);
    m_treeCtrl->AppendItem(root, "<dummy>");
    DoExpandItem(root);
}

void clRemoteDirCtrl::Close()
{
    clSFTPManager::Get().DeleteConnection(m_account.GetAccountName());
    m_account = {};
    m_treeCtrl->DeleteAllItems();
}

void clRemoteDirCtrl::DoExpandItem(const wxTreeItemId& item)
{
    wxBusyCursor bc;
    clRemoteDirCtrlItemData* cd = GetItemData(item);
    CHECK_PTR_RET(cd);

    // already initialized this folder before?
    if(cd->IsInitialized()) {
        return;
    }

    auto entries = clSFTPManager::Get().List(cd->IsSymlink() ? cd->GetSymlinkTarget() : cd->GetFullPath(), m_account);

    // remove the fake item "dummy"
    wxTreeItemIdValue cookie;
    wxTreeItemId dummyItem = m_treeCtrl->GetFirstChild(item, cookie);
    m_treeCtrl->Delete(dummyItem);

    // mark the entry as "initialized"
    cd->SetInitialized(true);

    for(auto entry : entries) {
        if(entry->GetName() == "." || entry->GetName() == "..")
            continue;
        // determine the icon index
        int imgIdx = wxNOT_FOUND;
        int expandImgIDx = wxNOT_FOUND;
        if(entry->IsFolder()) {
            imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolder);
            expandImgIDx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolderExpanded);
        } else if(entry->IsFile()) {
            imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(entry->GetName());
        }

        if(entry->IsSymlink()) {
            if(entry->IsFile()) {
                imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFileSymlink);

            } else {
                imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolderSymlink);
                expandImgIDx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolderSymlinkExpanded);
            }
        }

        // default bitmap
        if(imgIdx == wxNOT_FOUND) {
            imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeText);
        }

        wxString path;
        path << cd->GetFullPath() << "/" << entry->GetName();
        while(path.Replace("//", "/")) {}

        // prepare the client data
        auto childClientData = new clRemoteDirCtrlItemData(path);
        if(entry->IsFolder()) {
            childClientData->SetFolder();
        } else if(entry->IsFile()) {
            childClientData->SetFile();
        }

        if(entry->IsSymlink()) {
            childClientData->SetSymlink();
            childClientData->SetSymlinkTarget(entry->GetSymlinkPath());
        }

        wxTreeItemId child = m_treeCtrl->AppendItem(item, entry->GetName(), imgIdx, expandImgIDx, childClientData);
        // if its type folder, add a fake child item
        if(entry->IsFolder()) {
            m_treeCtrl->AppendItem(child, "<dummy>");
        }
    }
}

void clRemoteDirCtrl::OnContextMenu(wxContextMenuEvent& event)
{
    wxArrayTreeItemIds items;
    m_treeCtrl->GetSelections(items);
    if(items.size() == 0)
        return;

    wxTreeItemId item = items.Item(0);
    CHECK_ITEM_RET(item);

    clRemoteDirCtrlItemData* cd = GetItemData(item);
    CHECK_PTR_RET(cd);

    wxMenu menu;

    // Just incase, make sure the item is selected
    m_treeCtrl->SelectItem(item);
    wxMenuItem* menuItem = nullptr;
    if(!cd->IsFolder()) {
        item = menu.Append(wxID_ANY, _("Open"));
        menu.Bind(
            wxEVT_MENU,
            [this, item, items](wxCommandEvent& event) {
                event.Skip();
                // open the items
                for(const auto& i : items) {
                    CallAfter(&clRemoteDirCtrl::DoOpenItem, i, kOpenInCodeLite);
                }
            },
            menuItem->GetId());
        menu.AppendSeparator();
        item = menu.Append(wxID_ANY, _("Download and Open Containing Folder..."));
        menu.Bind(
            wxEVT_MENU,
            [this, item](wxCommandEvent& event) {
                event.Skip();
                auto items = GetSelections();
                if(items.empty()) {
                    return;
                }
                // open the items
                for(const auto& i : items) {
                    CallAfter(&clRemoteDirCtrl::DoOpenItem, i, kOpenInExplorer);
                }
            },
            menuItem->GetId());
        menu.AppendSeparator();

    } else {
        menuItem = menu.Append(wxID_ANY, _("Create a new directory..."));
        menu.Bind(
            wxEVT_MENU,
            [this, item](wxCommandEvent& event) {
                event.Skip();
                wxString folderName = ::clGetTextFromUser(_("Create a new folder"), _("New folder name"));
                if(folderName.empty()) {
                    return;
                }
                CallAfter(&clRemoteDirCtrl::DoCreateFolder, item, folderName);
            },
            menuItem->GetId());
        menuItem = menu.Append(wxID_ANY, _("Create a new file..."));
        menu.Bind(
            wxEVT_MENU,
            [this, item](wxCommandEvent& event) {
                event.Skip();
                wxString fileName = ::clGetTextFromUser(_("Create a new file"), _("New file name"));
                if(fileName.empty()) {
                    return;
                }
                CallAfter(&clRemoteDirCtrl::DoCreateFile, item, fileName);
            },
            menuItem->GetId());
        menuItem = menu.Append(wxID_ANY, _("Refresh"));
        menu.Bind(
            wxEVT_MENU,
            [this, item](wxCommandEvent& event) {
                event.Skip();
                auto cd = GetItemData(item);
                CHECK_PTR_RET(cd);
                m_treeCtrl->DeleteChildren(item);
                cd->SetInitialized(false);
                m_treeCtrl->AppendItem(item, "<dummy>");
                m_treeCtrl->Collapse(item);
            },
            menuItem->GetId());
        menu.AppendSeparator();
        menuItem = menu.Append(wxID_ANY, _("Run 'grep' in this folder"));
        menu.Bind(
            wxEVT_MENU,
            [this, item](wxCommandEvent& event) {
                event.Skip();
                CallAfter(&clRemoteDirCtrl::DoGrepFolder, item);
            },
            menuItem->GetId());
        menu.AppendSeparator();
    }
    menuItem = menu.Append(wxID_ANY, _("Rename"));
    menu.Bind(
        wxEVT_MENU,
        [this, item](wxCommandEvent& event) {
            event.Skip();
            CallAfter(&clRemoteDirCtrl::DoRename, item);
        },
        menuItem->GetId());
    menu.AppendSeparator();
    menuItem = menu.Append(wxID_ANY, _("Delete"));
    menu.Bind(
        wxEVT_MENU,
        [this, item](wxCommandEvent& event) {
            event.Skip();
            CallAfter(&clRemoteDirCtrl::DoDelete, item);
        },
        menuItem->GetId());
    m_treeCtrl->PopupMenu(&menu);
}

wxArrayTreeItemIds clRemoteDirCtrl::GetSelections() const
{
    wxArrayTreeItemIds selections;
    m_treeCtrl->GetSelections(selections);
    return selections;
}

void clRemoteDirCtrl::DoOpenItem(const wxTreeItemId& item, eDownloadAction action)
{
    CHECK_ITEM_RET(item);
    auto cd = GetItemData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsFile());

    switch(action) {
    case kOpenInCodeLite:
        clSFTPManager::Get().OpenFile(cd->GetFullPath(), m_account);
        break;
    case kOpenInExplorer: {
        auto editor = clSFTPManager::Get().OpenFile(cd->GetFullPath(), m_account);
        if(editor) {
            auto cd = reinterpret_cast<SFTPClientData*>(editor->GetClientData("sftp"));
            if(cd) {
                FileUtils::OpenFileExplorerAndSelect(cd->GetLocalPath());
            }
        }
        break;
    }
    }
}

void clRemoteDirCtrl::DoCreateFolder(const wxTreeItemId& item, const wxString& name)
{
    CHECK_ITEM_RET(item);
    auto cd = GetItemData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsFolder());

    wxString fullpath;
    fullpath << cd->GetFullPath() << "/" << name;
    if(!clSFTPManager::Get().NewFolder(fullpath, m_account)) {
        return;
    }

    clRemoteDirCtrlItemData* itemData = new clRemoteDirCtrlItemData(fullpath);
    itemData->SetFolder();
    int imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolder);
    int expandImgIDx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolderExpanded);

    auto child = m_treeCtrl->AppendItem(item, name, imgIdx, expandImgIDx, itemData);
    // append dummy item, to get the 'expand' icon
    m_treeCtrl->AppendItem(child, "<dummy>");
    m_treeCtrl->SelectItem(child); // select the newly added folder
}

void clRemoteDirCtrl::DoCreateFile(const wxTreeItemId& item, const wxString& name)
{
    wxBusyCursor bc;
    CHECK_ITEM_RET(item);
    auto cd = GetItemData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsFolder());

    wxString fullpath;
    fullpath << cd->GetFullPath() << "/" << name;
    if(!clSFTPManager::Get().NewFile(fullpath, m_account)) {
        return;
    }

    // update the tree view and open the file in the editor
    clRemoteDirCtrlItemData* itemData = new clRemoteDirCtrlItemData(fullpath);
    itemData->SetFile();
    int imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(name);
    int expandImgIDx = wxNOT_FOUND;
    auto childItem = m_treeCtrl->AppendItem(item, name, imgIdx, expandImgIDx, itemData);
    m_treeCtrl->SelectItem(childItem);
    CallAfter(&clRemoteDirCtrl::DoOpenItem, childItem, kOpenInCodeLite);
}

void clRemoteDirCtrl::DoGrepFolder(const wxTreeItemId& item)
{
    // TODO: implement this
}

void clRemoteDirCtrl::DoRename(const wxTreeItemId& item)
{
    // TODO: implement this
}

void clRemoteDirCtrl::DoDelete(const wxTreeItemId& item)
{
    // TODO: implement this
}

#endif // USE_SFTP
