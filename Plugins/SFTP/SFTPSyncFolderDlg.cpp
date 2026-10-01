#include "SFTPSyncFolderDlg.h"

#include "SFTPBrowserDlg.h"
#include "cl_config.h"
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

SFTPSyncFolderDlg::SFTPSyncFolderDlg(wxWindow* parent, const wxString& localFolder)
    : wxDialog(parent,
               wxID_ANY,
               _("Sync Folder with Remote"),
               wxDefaultPosition,
               wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    , m_localFolder(localFolder)
{
    const int border = FromDIP(10);
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Account + remote folder
    auto* grid = new wxFlexGridSizer(2, border, border);
    grid->AddGrowableCol(1);

    m_choiceAccount = new wxChoice(this, wxID_ANY);
    m_textCtrlRemoteFolder = new wxTextCtrl(this, wxID_ANY);
    m_buttonBrowse = new wxButton(this, wxID_ANY, _("Browse..."));

    grid->Add(new wxStaticText(this, wxID_ANY, _("Account:")), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(m_choiceAccount, 1, wxEXPAND);

    auto* folderSizer = new wxBoxSizer(wxHORIZONTAL);
    folderSizer->Add(m_textCtrlRemoteFolder, 1, wxEXPAND);
    folderSizer->Add(m_buttonBrowse, 0, wxLEFT, border);
    grid->Add(new wxStaticText(this, wxID_ANY, _("Remote folder:")), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(folderSizer, 1, wxEXPAND);
    mainSizer->Add(grid, 0, wxEXPAND | wxALL, border);

    // The filter
    m_searchCtrlFilter = new wxSearchCtrl(this, wxID_ANY);
    m_searchCtrlFilter->SetDescriptiveText(_("Filter files..."));
    m_searchCtrlFilter->ShowCancelButton(true);
    mainSizer->Add(m_searchCtrlFilter, 0, wxEXPAND | wxLEFT | wxRIGHT, border);

    // The files, with the Select All | Unselect All buttons on the right
    m_dvListCtrlFiles = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES);
    m_dvListCtrlFiles->AppendToggleColumn(wxEmptyString, wxDATAVIEW_CELL_ACTIVATABLE, FromDIP(40));
    m_dvListCtrlFiles->AppendTextColumn(_("File Name"), wxDATAVIEW_CELL_INERT, FromDIP(380));
    m_dvListCtrlFiles->AppendTextColumn(_("Size"), wxDATAVIEW_CELL_INERT, FromDIP(100), wxALIGN_RIGHT);

    m_buttonSelectAll = new wxButton(this, wxID_ANY, _("Select All"));
    m_buttonUnselectAll = new wxButton(this, wxID_ANY, _("Unselect All"));
    auto* selectionSizer = new wxBoxSizer(wxVERTICAL);
    selectionSizer->Add(m_buttonSelectAll, 0, wxEXPAND);
    selectionSizer->Add(m_buttonUnselectAll, 0, wxEXPAND | wxTOP, border);

    auto* filesSizer = new wxBoxSizer(wxHORIZONTAL);
    filesSizer->Add(m_dvListCtrlFiles, 1, wxEXPAND);
    filesSizer->Add(selectionSizer, 0, wxLEFT, border);
    mainSizer->Add(filesSizer, 1, wxEXPAND | wxALL, border);

    // OK | Cancel
    mainSizer->Add(CreateStdDialogButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, border);
    SetSizer(mainSizer);

    // Fill the account list and restore the last values (the folder specific values come first)
    SFTPSettings settings;
    settings.Load();
    for (const auto& account : settings.GetAccounts()) {
        m_choiceAccount->Append(account.GetAccountName());
    }

    auto& conf = clConfig::Get();
    wxString lastAccount = conf.Read(ConfigKey("account"), conf.Read(kLastAccount, wxString{}));
    wxString lastRemoteFolder = conf.Read(ConfigKey("remote-folder"), conf.Read(kLastRemoteFolder, wxString{}));
    int where = m_choiceAccount->FindString(lastAccount);
    if (where != wxNOT_FOUND) {
        m_choiceAccount->SetSelection(where);
    } else if (m_choiceAccount->GetCount() > 0) {
        m_choiceAccount->SetSelection(0);
    }
    m_textCtrlRemoteFolder->ChangeValue(lastRemoteFolder);

    PopulateFiles();
    UpdateTitle();

    m_buttonBrowse->Bind(wxEVT_BUTTON, &SFTPSyncFolderDlg::OnBrowse, this);
    m_buttonSelectAll->Bind(wxEVT_BUTTON, &SFTPSyncFolderDlg::OnSelectAll, this);
    m_buttonUnselectAll->Bind(wxEVT_BUTTON, &SFTPSyncFolderDlg::OnUnselectAll, this);
    m_dvListCtrlFiles->Bind(wxEVT_KEY_DOWN, &SFTPSyncFolderDlg::OnListKeyDown, this);
    m_searchCtrlFilter->Bind(wxEVT_TEXT, &SFTPSyncFolderDlg::OnFilterChanged, this);
    m_textCtrlRemoteFolder->Bind(wxEVT_TEXT, &SFTPSyncFolderDlg::OnRemoteFolderChanged, this);
    Bind(wxEVT_UPDATE_UI, &SFTPSyncFolderDlg::OnOKUI, this, wxID_OK);
    Bind(wxEVT_BUTTON, &SFTPSyncFolderDlg::OnOK, this, wxID_OK);

    SetMinSize(FromDIP(wxSize(500, 350)));
    SetSize(FromDIP(wxSize(650, 500)));
    CentreOnParent();
    ::AdjustDataViewAlternateColour(m_dvListCtrlFiles);
    ::clSetDialogBestSizeAndPosition(*this);
    m_textCtrlRemoteFolder->SetFocus();
}

void SFTPSyncFolderDlg::PopulateFiles()
{
    // Only the files that are directly under the folder, nested folders are not included
    wxArrayString names;
    wxDir dir(m_localFolder);
    if (dir.IsOpened()) {
        wxString name;
        bool cont = dir.GetFirst(&name, wxEmptyString, wxDIR_FILES | wxDIR_HIDDEN);
        while (cont) {
            names.Add(name);
            cont = dir.GetNext(&name);
        }
    }
    names.Sort();

    for (const auto& name : names) {
        wxFileName fn(m_localFolder, name);
        FileEntry entry;
        entry.path = fn.GetFullPath();
        entry.name = name;
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
        if (!filter.empty() && !entry.name.Lower().Contains(filter)) {
            continue;
        }

        wxVector<wxVariant> cols;
        cols.push_back(entry.checked);
        cols.push_back(entry.name);
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

wxArrayString SFTPSyncFolderDlg::GetSelectedFiles() const
{
    wxArrayString files;
    for (size_t row = 0; row < m_visible.size(); ++row) {
        if (m_dvListCtrlFiles->GetToggleValue(static_cast<unsigned>(row), 0)) {
            files.Add(m_entries[m_visible[row]].path);
        }
    }
    return files;
}

void SFTPSyncFolderDlg::SaveSelection() const
{
    auto& conf = clConfig::Get();
    conf.Write(kLastAccount, GetAccount());
    conf.Write(kLastRemoteFolder, GetRemoteFolder());
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
    event.Enable(m_choiceAccount->GetSelection() != wxNOT_FOUND && !GetRemoteFolder().empty() &&
                 !GetSelectedFiles().IsEmpty());
}

void SFTPSyncFolderDlg::OnOK(wxCommandEvent& event)
{
    SaveSelection();
    event.Skip();
}
