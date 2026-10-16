#pragma once

#include "UI.hpp"

#include <vector>
#include <wx/arrstr.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/dataview.h>
#include <wx/dialog.h>
#include <wx/srchctrl.h>
#include <wx/textctrl.h>

/// Prompts the user for the target account, the remote folder and the files (of a local folder) to upload.
/// Nested folders are not listed. The file list can be filtered (case insensitive, "contains"): only the files that are
/// displayed are uploaded.
class SFTPSyncFolderDlg : public SFTPSyncFolderDlgBase
{
public:
    SFTPSyncFolderDlg(wxWindow* parent, const wxString& localFolder);
    ~SFTPSyncFolderDlg() override = default;

    /// Return true if the folder has files that can be uploaded
    bool HasFiles() const { return !m_entries.empty(); }

    /// Pre-fill the remote folder, and the account (unless `account` is empty, then the account is not changed).
    /// Returns false (and changes nothing) if the account is unknown
    bool SetTarget(const wxString& account, const wxString& remoteFolder);

    wxString GetAccount() const;
    wxString GetRemoteFolder() const;

    /// Return the full path of the files that the user chose to upload (checked files that match the filter)
    wxArrayString GetSelectedFiles() const;

    /// Remember the account & the remote folder, so the dialog is pre-filled next time
    void SaveSelection() const;

private:
    void OnBrowse(wxCommandEvent& event) override;
    void OnOKUI(wxUpdateUIEvent& event) override;
    void OnOK(wxCommandEvent& event) override;
    void OnSelectAll(wxCommandEvent& event) override;
    void OnUnselectAll(wxCommandEvent& event) override;
    void OnListKeyDown(wxKeyEvent& event) override;
    void OnFilterChanged(wxCommandEvent& event) override;
    void OnRemoteFolderChanged(wxCommandEvent& event) override;
    void SetAllChecked(bool checked);
    void PopulateFiles();
    void ApplyFilter();
    void StoreChecked();
    void UpdateTitle();
    wxString ConfigKey(const wxString& name) const;

    wxString m_localFolder;
    struct FileEntry {
        wxString path;
        wxString name;
        wxString size;
        bool checked = true;
    };
    std::vector<FileEntry> m_entries; // all the files in the folder
    std::vector<size_t> m_visible;    // for each row in the list: the index of its entry in m_entries
};
