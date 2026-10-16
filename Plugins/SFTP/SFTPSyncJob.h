#pragma once

#include "SFTPSyncItem.h"
#include "clSFTPEvent.h"
#include "cl_command_event.h"

#include <set>
#include <vector>
#include <wx/event.h>
#include <wx/string.h>

class SFTP;

/// Uploads a list of local files to their remote paths, one file at a time. The remote folders are created if needed.
/// After each upload (success or failure) the job checks whether it was cancelled before starting the next file,
/// so a running upload is never interrupted.
///
/// The job reports its progress to the "SFTP Log" view. Once it is done, it asks the plugin to delete it.
class SFTPSyncJob : public wxEvtHandler
{
public:
    SFTPSyncJob(SFTP* plugin, const wxString& account, std::vector<SFTPSyncItem> items);
    ~SFTPSyncJob() override;

    void Start();

    /// Cancel the job. The file that is currently being uploaded is not interrupted, the next ones are skipped
    void Cancel();

    /// The plugin is going away: cancel, stop reporting, and delete the job once its pending work is done
    void Detach();

private:
    void OnFolderCreated(clSFTPEvent& event);
    void OnFolderError(clSFTPEvent& event);
    void OnUploadCompleted(clCommandEvent& event);
    void OnUploadError(clCommandEvent& event);
    void UploadNext();
    void Finish();
    void Log(int status, const wxString& message);

    SFTP* m_plugin = nullptr;
    wxString m_account;
    std::vector<SFTPSyncItem> m_items;
    size_t m_pendingFolders = 0;        // number of remote folders that are still being created
    std::set<wxString> m_failedFolders; // remote folders that could not be created
    size_t m_next = 0;                  // index of the next file to upload
    size_t m_succeeded = 0;
    size_t m_failed = 0;
    bool m_cancelled = false;
};
