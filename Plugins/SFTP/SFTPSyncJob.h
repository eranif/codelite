#pragma once

#include "clSFTPEvent.h"
#include "cl_command_event.h"

#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/string.h>

class SFTP;

/// Uploads a list of local files into a remote folder, one file at a time. The remote folder is created if needed.
/// After each upload (success or failure) the job checks whether it was cancelled before starting the next file,
/// so a running upload is never interrupted.
///
/// The job reports its progress to the "SFTP Log" view. Once it is done, it asks the plugin to delete it.
class SFTPSyncJob : public wxEvtHandler
{
public:
    SFTPSyncJob(SFTP* plugin, const wxString& account, const wxString& remoteFolder, const wxArrayString& localFiles);
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
    wxString GetRemotePath(const wxString& localFile) const;

    SFTP* m_plugin = nullptr;
    wxString m_account;
    wxString m_remoteFolder;
    wxArrayString m_files;
    size_t m_next = 0; // index of the next file to upload
    size_t m_succeeded = 0;
    size_t m_failed = 0;
    bool m_cancelled = false;
};
