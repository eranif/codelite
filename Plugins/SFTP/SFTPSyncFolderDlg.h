#pragma once

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
class SFTPSyncFolderDlg : public wxDialog
{
public:
    SFTPSyncFolderDlg(wxWindow* parent, const wxString& localFolder);
    ~SFTPSyncFolderDlg() override = default;

    /// Return true if the folder has files that can be uploaded
    bool HasFiles() const { return !m_entries.empty(); }

    wxString GetAccount() const;
    wxString GetRemoteFolder() const;

    /// Return the full path of the files that the user chose to upload (checked files that match the filter)
    wxArrayString GetSelectedFiles() const;

    /// Remember the account & the remote folder, so the dialog is pre-filled next time
    void SaveSelection() const;

private:
    void OnBrowse(wxCommandEvent& event);
    void OnOKUI(wxUpdateUIEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);
    void OnUnselectAll(wxCommandEvent& event);
    void OnListKeyDown(wxKeyEvent& event);
    void OnFilterChanged(wxCommandEvent& event);
    void OnRemoteFolderChanged(wxCommandEvent& event);
    void SetAllChecked(bool checked);
    void PopulateFiles();
    void ApplyFilter();
    void StoreChecked();
    void UpdateTitle();
    wxString ConfigKey(const wxString& name) const;

    wxString m_localFolder;
    wxChoice* m_choiceAccount = nullptr;
    wxTextCtrl* m_textCtrlRemoteFolder = nullptr;
    wxButton* m_buttonBrowse = nullptr;
    wxButton* m_buttonSelectAll = nullptr;
    wxButton* m_buttonUnselectAll = nullptr;
    wxSearchCtrl* m_searchCtrlFilter = nullptr;
    wxDataViewListCtrl* m_dvListCtrlFiles = nullptr;

    struct FileEntry {
        wxString path;
        wxString name;
        wxString size;
        bool checked = true;
    };
    std::vector<FileEntry> m_entries; // all the files in the folder
    std::vector<size_t> m_visible;    // for each row in the list: the index of its entry in m_entries
};
