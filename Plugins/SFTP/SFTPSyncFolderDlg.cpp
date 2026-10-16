#include "SFTPSyncFolderDlg.h"

#include "SFTPBrowserDlg.h"
#include "SFTPSyncPaths.h"
#include "cl_config.h"
#include "file_logger.h"
#include "globals.h"
#include "sftp_settings.h"
#include "ssh/ssh_account_info.h"

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/srchctrl.h>
#include <wx/stattext.h>
#include <wx/vector.h>

namespace
{
const wxString kLastAccount = "sftp/sync-folder/last-account";
const wxString kLastRemoteFolder = "sftp/sync-folder/last-remote-folder";
} // namespace

SFTPSyncFolderDlg::SFTPSyncFolderDlg(wxWindow* parent,
                                     const wxString& localFolder,
                                     const wxArrayString& files,
                                     bool useLastRemoteFolder)
    : SFTPSyncFolderDlgBase(parent, wxID_ANY, _("Sync with Remote"))
    , m_localFolder(localFolder)
    , m_useLastRemoteFolder(useLastRemoteFolder)
{
    // The local folder is for display only
    m_textCtrlLocalFolder->SetEditable(false);
    m_textCtrlLocalFolder->ChangeValue(m_localFolder);

    m_dvListCtrlFiles->AppendToggleColumn(wxEmptyString, wxDATAVIEW_CELL_ACTIVATABLE, FromDIP(40));
    m_dvListCtrlFiles->AppendTextColumn(_("Local Path"), wxDATAVIEW_CELL_INERT, FromDIP(260));
    m_dvListCtrlFiles->AppendTextColumn(_("Remote Path"), wxDATAVIEW_CELL_INERT, FromDIP(260));
    m_dvListCtrlFiles->AppendTextColumn(_("Size"), wxDATAVIEW_CELL_INERT, FromDIP(100), wxALIGN_RIGHT);

    // Fill the account list and restore the last values (the folder specific values come first)
    SFTPSettings settings;
    settings.Load();
    for (const auto& account : settings.GetAccounts()) {
        m_choiceAccount->Append(account.GetAccountName());
    }

    auto& conf = clConfig::Get();
    wxString lastAccount = conf.Read(ConfigKey("account"), conf.Read(kLastAccount, wxString{}));
    // The last remote folder of another local folder is a guess, use it only if the caller wants it
    const wxString fallbackRemoteFolder = m_useLastRemoteFolder ? conf.Read(kLastRemoteFolder, wxString{}) : wxString{};
    wxString lastRemoteFolder = conf.Read(ConfigKey("remote-folder"), fallbackRemoteFolder);
    int where = m_choiceAccount->FindString(lastAccount);
    if (where != wxNOT_FOUND) {
        m_choiceAccount->SetSelection(where);
    } else if (m_choiceAccount->GetCount() > 0) {
        m_choiceAccount->SetSelection(0);
    }
    m_textCtrlRemoteFolder->ChangeValue(lastRemoteFolder);

    PopulateFiles(files);
    UpdateTitle();

    SetMinSize(FromDIP(wxSize(500, 350)));
    SetSize(FromDIP(wxSize(650, 500)));
    CentreOnParent();
    ::AdjustDataViewAlternateColour(m_dvListCtrlFiles);
    ::clSetDialogBestSizeAndPosition(*this);
    m_textCtrlRemoteFolder->SetFocus();
}

wxArrayString SFTPSyncFolderDlg::ListFolderFiles(const wxString& folder)
{
    // Only the files that are directly under the folder, nested folders are not included
    wxArrayString names;
    wxDir dir(folder);
    if (dir.IsOpened()) {
        wxString name;
        bool cont = dir.GetFirst(&name, wxEmptyString, wxDIR_FILES | wxDIR_HIDDEN);
        while (cont) {
            names.Add(name);
            cont = dir.GetNext(&name);
        }
    }
    names.Sort();

    wxArrayString files;
    for (const auto& name : names) {
        files.Add(wxFileName(folder, name).GetFullPath());
    }
    return files;
}

void SFTPSyncFolderDlg::PopulateFiles(const wxArrayString& files)
{
    for (const auto& file : files) {
        // A file outside of the local folder has no place in the remote folder. Do not guess one (files with the same
        // name would overwrite each other)
        auto relative = SFTPRelativePath(file, m_localFolder);
        if (!relative) {
            clWARNING() << "SFTP: skipping" << file << ", it is not under" << m_localFolder << endl;
            continue;
        }

        wxFileName fn(file);
        FileEntry entry;
        entry.path = fn.GetFullPath();
        entry.localPath = relative->GetFullPath();
        entry.remotePath = relative->GetFullPath(wxPATH_UNIX);
        entry.size = wxFileName::GetHumanReadableSize(fn.GetSize());
        m_entries.push_back(std::move(entry));
    }
    ApplyFilter();
}

void SFTPSyncFolderDlg::StoreChecked()
{
    for (size_t row = 0; row < m_visible.size(); ++row) {
        m_entries[m_visible[row]].checked = m_dvListCtrlFiles->GetToggleValue(static_cast<unsigned>(row), 0);
    }
}

void SFTPSyncFolderDlg::ApplyFilter()
{
    // Remember what the user checked, before the list is rebuilt
    StoreChecked();

    wxString filter = m_searchCtrlFilter->GetValue();
    filter.Trim().Trim(false).MakeLower();

    m_dvListCtrlFiles->DeleteAllItems();
    m_visible.clear();
    for (size_t i = 0; i < m_entries.size(); ++i) {
        const FileEntry& entry = m_entries[i];
        if (!filter.empty() && !entry.localPath.Lower().Contains(filter)) {
            continue;
        }

        wxVector<wxVariant> cols;
        cols.push_back(entry.checked);
        cols.push_back(entry.localPath);
        cols.push_back(entry.remotePath);
        cols.push_back(entry.size);
        m_dvListCtrlFiles->AppendItem(cols);
        m_visible.push_back(i);
    }

    // Select the first row, so the Space key works right away
    if (!m_visible.empty()) {
        m_dvListCtrlFiles->SelectRow(0);
    }
}

void SFTPSyncFolderDlg::UpdateTitle()
{
    wxString remote = GetRemoteFolder();
    if (remote.empty()) {
        SetTitle(wxString::Format(_("Syncing %s"), m_localFolder));
    } else {
        SetTitle(wxString::Format(_("Syncing %s -> %s"), m_localFolder, remote));
    }
}

void SFTPSyncFolderDlg::OnFilterChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ApplyFilter();
}

void SFTPSyncFolderDlg::OnRemoteFolderChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    UpdateTitle();
}

wxString SFTPSyncFolderDlg::ConfigKey(const wxString& name) const
{
    return "sftp/sync-folder/" + m_localFolder + "/" + name;
}

bool SFTPSyncFolderDlg::SetTarget(const wxString& account, const wxString& remoteFolder)
{
    if (!account.empty()) {
        int where = m_choiceAccount->FindString(account);
        if (where == wxNOT_FOUND) {
            return false;
        }
        m_choiceAccount->SetSelection(where);
    }
    m_textCtrlRemoteFolder->ChangeValue(remoteFolder);
    UpdateTitle();
    return true;
}

wxString SFTPSyncFolderDlg::GetAccount() const { return m_choiceAccount->GetStringSelection(); }

wxString SFTPSyncFolderDlg::GetRemoteFolder() const
{
    wxString folder = m_textCtrlRemoteFolder->GetValue();
    folder.Trim().Trim(false);
    return folder;
}

bool SFTPSyncFolderDlg::HasSelectedItems() const
{
    for (size_t row = 0; row < m_visible.size(); ++row) {
        if (m_dvListCtrlFiles->GetToggleValue(static_cast<unsigned>(row), 0)) {
            return true;
        }
    }
    return false;
}

std::vector<SFTPSyncItem> SFTPSyncFolderDlg::GetSelectedItems() const
{
    wxString remoteFolder = GetRemoteFolder();
    // Remote paths always use "/": fix the separators and collapse the repeated ones
    remoteFolder.Replace("\\", "/");
    while (remoteFolder.Contains("//")) {
        remoteFolder.Replace("//", "/");
    }
    if (!remoteFolder.EndsWith("/")) {
        remoteFolder << "/";
    }

    std::vector<SFTPSyncItem> items;
    for (size_t row = 0; row < m_visible.size(); ++row) {
        if (m_dvListCtrlFiles->GetToggleValue(static_cast<unsigned>(row), 0)) {
            const FileEntry& entry = m_entries[m_visible[row]];
            items.push_back({entry.path, remoteFolder + entry.remotePath});
        }
    }
    return items;
}

void SFTPSyncFolderDlg::SaveSelection() const
{
    auto& conf = clConfig::Get();
    conf.Write(kLastAccount, GetAccount());
    if (m_useLastRemoteFolder) {
        conf.Write(kLastRemoteFolder, GetRemoteFolder());
    }
    conf.Write(ConfigKey("account"), GetAccount());
    conf.Write(ConfigKey("remote-folder"), GetRemoteFolder());
}

void SFTPSyncFolderDlg::OnBrowse(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString account = GetAccount();
    SFTPBrowserDlg dlg(this, _("Select the remote folder"), wxEmptyString, clSFTP::SFTP_BROWSE_FOLDERS, account);
    dlg.Initialize(account, GetRemoteFolder());
    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    // The user may have switched the account in the browser dialog
    int where = m_choiceAccount->FindString(dlg.GetAccount());
    if (where != wxNOT_FOUND) {
        m_choiceAccount->SetSelection(where);
    }

    wxString path = dlg.GetPath();
    if (!path.empty()) {
        m_textCtrlRemoteFolder->ChangeValue(path);
        UpdateTitle();
    }
}

void SFTPSyncFolderDlg::SetAllChecked(bool checked)
{
    // Only the displayed rows (the ones that match the filter)
    for (size_t row = 0; row < m_visible.size(); ++row) {
        m_dvListCtrlFiles->SetToggleValue(checked, static_cast<unsigned>(row), 0);
    }
}

void SFTPSyncFolderDlg::OnSelectAll(wxCommandEvent& event)
{
    wxUnusedVar(event);
    SetAllChecked(true);
}

void SFTPSyncFolderDlg::OnUnselectAll(wxCommandEvent& event)
{
    wxUnusedVar(event);
    SetAllChecked(false);
}

void SFTPSyncFolderDlg::OnListKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() != WXK_SPACE) {
        event.Skip();
        return;
    }

    // Space: check / uncheck the selected row
    int row = m_dvListCtrlFiles->GetSelectedRow();
    if (row != wxNOT_FOUND) {
        m_dvListCtrlFiles->SetToggleValue(!m_dvListCtrlFiles->GetToggleValue(row, 0), row, 0);
    }
}

void SFTPSyncFolderDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(m_choiceAccount->GetSelection() != wxNOT_FOUND && !GetRemoteFolder().empty() && HasSelectedItems());
}

void SFTPSyncFolderDlg::OnOK(wxCommandEvent& event)
{
    SaveSelection();
    event.Skip();
}
