#pragma once

#include "SFTPSyncItem.h"
#include "UI.hpp"

#include <vector>
#include <wx/arrstr.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/dataview.h>
#include <wx/dialog.h>
#include <wx/srchctrl.h>
#include <wx/textctrl.h>

/// Prompts the user for the target account, the remote folder and the files to upload.
///
/// The files are displayed with their path relative to the local folder (`Local Path`), and with the path that they
/// will have relative to the remote folder (`Remote Path`). The remote folder is the remote counterpart of the local
/// folder, so a file is uploaded to <remote folder>/<remote path>.
///
/// The file list can be filtered (case insensitive, "contains"): only the files that are displayed are uploaded.
class SFTPSyncFolderDlg : public SFTPSyncFolderDlgBase
{
public:
    /// `localFolder` is the base folder of the files: all of `files` must be located under it (full paths).
    /// A file that is not under it is skipped.
    /// If there is no remote folder saved for `localFolder`, the last remote folder that was used for another local
    /// folder is suggested when `useLastRemoteFolder` is true. It is a guess that only fits a folder sync.
    SFTPSyncFolderDlg(wxWindow* parent,
                      const wxString& localFolder,
                      const wxArrayString& files,
                      bool useLastRemoteFolder = true);
    ~SFTPSyncFolderDlg() override = default;

    /// Return the full path of the files that are directly under `folder` (nested folders are not included), sorted
    static wxArrayString ListFolderFiles(const wxString& folder);

    /// Return true if there are files that can be uploaded
    bool HasFiles() const { return !m_entries.empty(); }

    /// Pre-fill the remote folder, and the account (unless `account` is empty, then the account is not changed).
    /// Returns false (and changes nothing) if the account is unknown
    bool SetTarget(const wxString& account, const wxString& remoteFolder);

    wxString GetAccount() const;
    wxString GetRemoteFolder() const;

    /// Return true if at least one file is checked (and matches the filter)
    bool HasSelectedItems() const;

    /// Return the files that the user chose to upload (checked files that match the filter), with the full remote
    /// path of each one
    std::vector<SFTPSyncItem> GetSelectedItems() const;

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
    void PopulateFiles(const wxArrayString& files);
    void ApplyFilter();
    void StoreChecked();
    void UpdateTitle();
    wxString ConfigKey(const wxString& name) const;

    wxString m_localFolder;
    bool m_useLastRemoteFolder = true;
    struct FileEntry {
        wxString path;       // the full local path
        wxString localPath;  // relative to the local folder, native separators
        wxString remotePath; // relative to the remote folder, always uses "/"
        wxString size;
        bool checked = true;
    };
    std::vector<FileEntry> m_entries; // all the files
    std::vector<size_t> m_visible;    // for each row in the list: the index of its entry in m_entries
};
