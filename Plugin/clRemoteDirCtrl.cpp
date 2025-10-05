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
#include <wx/window.h>
#include <wx/xrc/xmlres.h>

wxDEFINE_EVENT(wxEVT_REMOTEDIR_DIR_CONTEXT_MENU_SHOWING, clContextMenuEvent);
wxDEFINE_EVENT(wxEVT_REMOTEDIR_FILE_CONTEXT_MENU_SHOWING, clContextMenuEvent);

namespace
{
// we don't use FileUtis::IsHidden since it checks for local file existance on windows
bool is_hidden(const wxString& name) { return !name.empty() && name[0] == '.'; }
} // namespace

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

    // set the default bitmaps
    m_treeCtrl->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());

    // events
    EventNotifier::Get()->Bind(wxEVT_BITMAPS_UPDATED, [this](clCommandEvent& e) {
        e.Skip();
        // update the bitmaps
        m_treeCtrl->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
    });

    auto SortFunc = [&](const wxTreeItemId& itemA, const wxTreeItemId& itemB) {
        clRemoteDirCtrlItemData* a = static_cast<clRemoteDirCtrlItemData*>(GetItemData(itemA));
        clRemoteDirCtrlItemData* b = static_cast<clRemoteDirCtrlItemData*>(GetItemData(itemB));
        if (a->IsFolder() && !b->IsFolder())
            return true;
        else if (b->IsFolder() && !a->IsFolder())
            return false;
        // same kind
        return (a->GetFullName().CmpNoCase(b->GetFullName()) < 0);
    };
    m_treeCtrl->SetSortFunction(std::move(SortFunc));
}

clRemoteDirCtrl::~clRemoteDirCtrl()
{
    m_treeCtrl->Unbind(wxEVT_CONTEXT_MENU, &clRemoteDirCtrl::OnContextMenu, this);
    m_treeCtrl->Unbind(wxEVT_TREE_ITEM_EXPANDING, &clRemoteDirCtrl::OnItemExpanding, this);
    m_treeCtrl->Unbind(wxEVT_TREE_ITEM_ACTIVATED, &clRemoteDirCtrl::OnItemActivated, this);
}

clRemoteDirCtrlItemData* clRemoteDirCtrl::GetItemData(const wxTreeItemId& item) const
{
    CHECK_ITEM_RET_NULL(item);
    clRemoteDirCtrlItemData* cd = dynamic_cast<clRemoteDirCtrlItemData*>(m_treeCtrl->GetItemData(item));
    return cd;
}

void clRemoteDirCtrl::OnItemActivated(wxTreeEvent& event) { DoOpenItem(event.GetItem(), kOpenInCodeLite); }

void clRemoteDirCtrl::OnItemExpanding(wxTreeEvent& event) { DoExpandItem(event.GetItem()); }

bool clRemoteDirCtrl::Open(const wxString& path, const SSHAccountInfo& account)
{
    Close(false); // close any view previously opened, do not prompt the user
    if (!clSFTPManager::Get().AddConnection(account)) {
        return false;
    }
    m_account = account;
    m_treeCtrl->DeleteAllItems();

    // add the root item
    clRemoteDirCtrlItemData* cd = new clRemoteDirCtrlItemData(path);
    cd->SetFolder();

    // only display the dir name not its full path
    wxString displayString = path.AfterLast('/');
    displayString.Trim();

    if (displayString.empty()) {
        displayString = path;
    }

    wxTreeItemId root = m_treeCtrl->AddRoot(
        displayString, clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolder), wxNOT_FOUND, cd);
    m_treeCtrl->AppendItem(root, "<dummy>");
    DoExpandItem(root);
    return true;
}

bool clRemoteDirCtrl::Close(bool promptUser)
{
    if (!clSFTPManager::Get().DeleteConnection(m_account.GetAccountName(), promptUser)) {
        return false;
    }
    m_account = {};
    m_treeCtrl->DeleteAllItems();
    return true;
}

#define RECONNECT_RET(msg)                                                                                \
    clGetManager()->SetStatusMessage(wxString() << _("Reconnecting to: ") << m_account.GetAccountName()); \
    wxYield();                                                                                            \
    if (!clSFTPManager::Get().AddConnection(m_account, true)) {                                           \
        ::wxMessageBox(msg, "CodeLite", wxICON_ERROR | wxOK);                                             \
        return;                                                                                           \
    }                                                                                                     \
    clGetManager()->SetStatusMessage(wxString() << _("Connected to: ") << m_account.GetAccountName());    \
    wxYield();

void clRemoteDirCtrl::DoExpandItem(const wxTreeItemId& item)
{
    wxBusyCursor bc;
    clRemoteDirCtrlItemData* cd = GetItemData(item);
    CHECK_PTR_RET(cd);

    // already initialized this folder before?
    if (cd->IsInitialized()) {
        return;
    }

    auto res = clSFTPManager::Get().List(cd->IsSymlink() ? cd->GetSymlinkTarget() : cd->GetFullPath(), m_account);
    if (!res) {
        wxString msg;
        msg << "Failed to list remote files. " << res.error_message();
        RECONNECT_RET(msg);
        res = clSFTPManager::Get().List(cd->IsSymlink() ? cd->GetSymlinkTarget() : cd->GetFullPath(), m_account);
    }

    if (!res) {
        ::wxMessageBox(
            wxString() << "Failed to list remote files. " << res.error_message(), "CodeLite", wxICON_ERROR | wxOK);
        return;
    }

    // remove the fake item "dummy"
    wxTreeItemIdValue cookie;
    wxTreeItemId dummyItem = m_treeCtrl->GetFirstChild(item, cookie);
    m_treeCtrl->Delete(dummyItem);

    // mark the entry as "initialized"
    cd->SetInitialized(true);
    SFTPAttribute::List_t list = res.value();
    for (auto entry : list) {
        if (entry->GetName() == "." || entry->GetName() == "..")
            continue;

        // determine the icon index
        bool isHidden = is_hidden(entry->GetName());
        int imgIdx = wxNOT_FOUND;
        int expandImgIDx = wxNOT_FOUND;
        if (entry->IsFolder()) {
            imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolder, isHidden);
            expandImgIDx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolderExpanded, isHidden);
        } else if (entry->IsFile()) {
            imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(entry->GetName(), isHidden);
        }

        if (entry->IsSymlink()) {
            if (entry->IsFile()) {
                imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFileSymlink, isHidden);

            } else {
                imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolderSymlink, isHidden);
                expandImgIDx =
                    clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolderSymlinkExpanded, isHidden);
            }
        }

        // default bitmap
        if (imgIdx == wxNOT_FOUND) {
            imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeText, isHidden);
        }

        wxString path;
        path << cd->GetFullPath() << "/" << entry->GetName();
        while (path.Replace("//", "/")) {}

        // prepare the client data
        auto childClientData = new clRemoteDirCtrlItemData(path);
        if (entry->IsFolder()) {
            childClientData->SetFolder();
        } else if (entry->IsFile()) {
            childClientData->SetFile();
        }

        if (entry->IsSymlink()) {
            childClientData->SetSymlink();
            childClientData->SetSymlinkTarget(entry->GetSymlinkPath());
        }

        wxTreeItemId child = m_treeCtrl->AppendItem(item, entry->GetName(), imgIdx, expandImgIDx, childClientData);

        // if its type folder, add a fake child item
        if (entry->IsFolder()) {
            m_treeCtrl->AppendItem(child, "<dummy>");
        }

        if (isHidden) {
            // a hidden item, use a disabled colour
            m_treeCtrl->SetItemTextColour(child, m_treeCtrl->GetColours().GetGrayText());
        }
    }
}

void clRemoteDirCtrl::OnContextMenu(wxContextMenuEvent& event)
{
    wxArrayTreeItemIds items;
    m_treeCtrl->GetSelections(items);
    if (items.size() == 0)
        return;

    wxTreeItemId item = items.Item(0);
    CHECK_ITEM_RET(item);

    clRemoteDirCtrlItemData* cd = GetItemData(item);
    CHECK_PTR_RET(cd);

    bool is_root_item = (m_treeCtrl->GetRootItem() == item);
    wxMenu menu;

    // Just incase, make sure the item is selected
    m_treeCtrl->SelectItem(item);
    if (!cd->IsFolder()) {
        menu.Append(wxID_OPEN, _("Open"));
        menu.Bind(
            wxEVT_MENU,
            [this, items](wxCommandEvent& event) {
                event.Skip();
                // open the items
                for (const auto& i : items) {
                    CallAfter(&clRemoteDirCtrl::DoOpenItem, i, kOpenInCodeLite);
                }
            },
            wxID_OPEN);
        menu.AppendSeparator();
        menu.Append(wxID_DOWN, _("Download and Open Containing Folder..."));
        menu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent& event) {
                event.Skip();
                auto items = GetSelections();
                if (items.empty()) {
                    return;
                }
                // open the items
                for (const auto& i : items) {
                    CallAfter(&clRemoteDirCtrl::DoOpenItem, i, kOpenInExplorer);
                }
            },
            wxID_DOWN);

    } else {
        menu.Append(XRCID("new-dir"), _("Create a new directory..."));
        menu.Bind(
            wxEVT_MENU,
            [this, item](wxCommandEvent& event) {
                event.Skip();
                wxString folderName = ::clGetTextFromUser(_("Create a new folder"), _("New folder name"));
                if (folderName.empty()) {
                    return;
                }
                CallAfter(&clRemoteDirCtrl::DoCreateFolder, item, folderName);
            },
            XRCID("new-dir"));
        menu.Append(XRCID("new-new-file"), _("Create a new file..."));
        menu.Bind(
            wxEVT_MENU,
            [this, item](wxCommandEvent& event) {
                event.Skip();
                wxString fileName = ::clGetTextFromUser(_("Create a new file"), _("New file name"));
                if (fileName.empty()) {
                    return;
                }
                CallAfter(&clRemoteDirCtrl::DoCreateFile, item, fileName);
            },
            XRCID("new-new-file"));
        menu.Append(wxID_REFRESH, _("Refresh"));
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
            wxID_REFRESH);
    }
    if (!is_root_item) {
        menu.AppendSeparator();
        menu.Append(XRCID("rename_item"), _("Rename"));
        menu.Bind(
            wxEVT_MENU,
            [this, item](wxCommandEvent& event) {
                event.Skip();
                CallAfter(&clRemoteDirCtrl::DoRename, item);
            },
            XRCID("rename_item"));
        menu.AppendSeparator();
        menu.Append(XRCID("delete-file"), _("Delete"));
        menu.Bind(
            wxEVT_MENU,
            [this, item](wxCommandEvent& event) {
                event.Skip();
                CallAfter(&clRemoteDirCtrl::DoDelete, item);
            },
            XRCID("delete-file"));
    }
    // common
    menu.AppendSeparator();
    menu.Append(XRCID("copy-path"), _("Copy path"));
    menu.Bind(
        wxEVT_MENU,
        [this, item](wxCommandEvent& event) {
            event.Skip();
            auto cd = GetItemData(item);
            CHECK_PTR_RET(cd);
            ::CopyToClipboard(cd->GetFullPath());
            clGetManager()->SetStatusMessage(_("Path copied to clipboard"));
        },
        XRCID("copy-path"));
    // let others know that we are about to show the context menu for this control
    clContextMenuEvent menuEvent(cd->IsFolder() ? wxEVT_REMOTEDIR_DIR_CONTEXT_MENU_SHOWING
                                                : wxEVT_REMOTEDIR_FILE_CONTEXT_MENU_SHOWING);
    menuEvent.SetMenu(&menu);
    menuEvent.SetEventObject(this);
    menuEvent.SetClientData(item);
    m_treeCtrl->GetEventHandler()->ProcessEvent(menuEvent);

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

    switch (action) {
    case kOpenInCodeLite:
        if (!clSFTPManager::Get().OpenFile(cd->GetFullPath(), m_account)) {
            RECONNECT_RET(_("Failed to open file: connection lost"));
            clSFTPManager::Get().OpenFile(cd->GetFullPath(), m_account);
        }
        break;
    case kOpenInExplorer: {
        auto editor = clSFTPManager::Get().OpenFile(cd->GetFullPath(), m_account);
        if (!editor) {
            RECONNECT_RET(_("Failed to open file: connection lost"));
            editor = clSFTPManager::Get().OpenFile(cd->GetFullPath(), m_account);
        }
        CHECK_PTR_RET(editor);
        auto cd = reinterpret_cast<SFTPClientData*>(editor->GetClientData("sftp"));
        if (cd) {
            FileUtils::OpenFileExplorerAndSelect(cd->GetLocalPath());
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
    if (!clSFTPManager::Get().NewFolder(fullpath, m_account)) {
        return;
    }

    if (!cd->IsInitialized()) {
        // make sure that the folder is expanded
        DoExpandItem(item);
    }

    clRemoteDirCtrlItemData* itemData = new clRemoteDirCtrlItemData(fullpath);
    itemData->SetFolder();
    int imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolder);
    int expandImgIDx = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolderExpanded);

    auto child = m_treeCtrl->AppendItem(item, name, imgIdx, expandImgIDx, itemData);
    // append dummy item, to get the 'expand' icon
    m_treeCtrl->AppendItem(child, "<dummy>");
    if (!m_treeCtrl->IsExpanded(item)) {
        m_treeCtrl->Expand(item);
    }
    m_treeCtrl->SelectItem(child); // select the newly added folder
}

void clRemoteDirCtrl::DoCreateFile(const wxTreeItemId& item, const wxString& name)
{
    wxBusyCursor bc;
    CHECK_ITEM_RET(item);
    auto cd = GetItemData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsFolder());

    if (!cd->IsInitialized()) {
        // make sure that the folder is expanded
        DoExpandItem(item);
    }
    wxString fullpath;
    fullpath << cd->GetFullPath() << "/" << name;
    if (!clSFTPManager::Get().NewFile(fullpath, m_account)) {
        RECONNECT_RET(_("Failed to create file: connection lost"));
        CHECK_COND_RET(clSFTPManager::Get().NewFile(fullpath, m_account));
    }

    // update the tree view and open the file in the editor
    clRemoteDirCtrlItemData* itemData = new clRemoteDirCtrlItemData(fullpath);
    itemData->SetFile();
    int imgIdx = clGetManager()->GetStdIcons()->GetMimeImageId(name);
    int expandImgIDx = wxNOT_FOUND;
    auto childItem = m_treeCtrl->AppendItem(item, name, imgIdx, expandImgIDx, itemData);
    if (!m_treeCtrl->IsExpanded(item)) {
        m_treeCtrl->Expand(item);
    }
    m_treeCtrl->SelectItem(childItem);
    CallAfter(&clRemoteDirCtrl::DoOpenItem, childItem, kOpenInCodeLite);
}

void clRemoteDirCtrl::DoRename(const wxTreeItemId& item)
{
    clRemoteDirCtrlItemData* cd = GetItemData(item);
    if (!cd) {
        return;
    }

    wxString new_name = ::clGetTextFromUser(_("Renaming ") + cd->GetFullName(), _("New name:"), cd->GetFullName());
    if (new_name.IsEmpty())
        return;

    wxString old_path = cd->GetFullPath();
    wxString oldName = cd->GetFullName(); // in case the rename will fail
    cd->SetFullName(new_name);
    if (!clSFTPManager::Get().Rename(old_path, cd->GetFullPath(), m_account)) {
        cd->SetFullName(oldName); // restore the old name
        RECONNECT_RET(_("Failed to rename file: connection lost"));
        // reconnect ok
        cd->SetFullName(new_name);
        if (!clSFTPManager::Get().Rename(old_path, cd->GetFullPath(), m_account)) {
            cd->SetFullName(oldName); // restore the old name
            return;
        }
    }

    // update the text
    m_treeCtrl->SetItemText(item, new_name);
    if (cd->IsFolder()) {
        // if it's a folder, remove all its children and mark it as non-initialised
        m_treeCtrl->DeleteChildren(item);
        cd->SetInitialized(false);
        m_treeCtrl->AppendItem(item, "<dummy>");
        m_treeCtrl->Collapse(item);
    }
}

void clRemoteDirCtrl::DoDelete(const wxTreeItemId& item)
{
    wxArrayTreeItemIds items;
    m_treeCtrl->GetSelections(items);
    if (items.empty())
        return;

    wxString message;
    message << _("Are you sure you want to delete the selected items?");
    if (::wxMessageBox(message, "Confirm", wxYES_NO | wxCANCEL | wxICON_WARNING) != wxYES) {
        return;
    }

    for (const auto& item : items) {
        clRemoteDirCtrlItemData* cd = GetItemData(item);
        bool success = false;
        if (cd->IsFolder()) {
            success = clSFTPManager::Get().DeleteDir(cd->GetFullPath(), m_account);

        } else {
            success = clSFTPManager::Get().UnlinkFile(cd->GetFullPath(), m_account);
        }
        // Remove the selection
        if (success) {
            m_treeCtrl->Delete(item);
        }
    }
}

bool clRemoteDirCtrl::IsConnected() const { return !m_treeCtrl->IsEmpty() && !m_account.GetAccountName().IsEmpty(); }

wxString clRemoteDirCtrl::GetSelectedFolder() const
{
    auto selections = GetSelections();
    if (selections.empty()) {
        return wxEmptyString;
    }

    auto cd = GetItemData(selections[0]);
    CHECK_COND_RET_EMPTY_STRING(cd);
    CHECK_COND_RET_EMPTY_STRING(cd->IsFolder());
    return cd->GetFullPath();
}

size_t clRemoteDirCtrl::GetSelectedFolders(wxArrayString& paths) const
{
    auto selections = GetSelections();
    if (selections.empty()) {
        paths.clear();
        return 0;
    }

    paths.reserve(selections.size());
    for (const auto& item : selections) {
        auto cd = GetItemData(item);
        if (!cd || !cd->IsFolder()) {
            continue;
        }
        paths.Add(cd->GetFullPath());
    }
    return paths.size();
}

bool clRemoteDirCtrl::SetNewRoot(const wxString& remotePath)
{
    wxBusyCursor bc;
    // Check that the new folder exists
    if (!clSFTPManager::Get().IsDirExists(remotePath, m_account)) {
        ::wxMessageBox(_("Can not set new root directory: ") + remotePath + _("\nNo such directory"),
                       "CodeLite",
                       wxICON_WARNING | wxCENTRE);
        return false;
    }
    m_treeCtrl->DeleteAllItems();

    // add new root item
    clRemoteDirCtrlItemData* cd = new clRemoteDirCtrlItemData(remotePath);
    cd->SetFolder();

    wxTreeItemId root = m_treeCtrl->AddRoot(
        remotePath, clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolder), wxNOT_FOUND, cd);
    m_treeCtrl->AppendItem(root, "<dummy>");
    DoExpandItem(root);
    return true;
}

#endif // USE_SFTP
