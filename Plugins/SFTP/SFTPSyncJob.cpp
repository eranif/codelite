#include "SFTPSyncJob.h"

#include "SFTPStatusPage.h"
#include "clSFTPManager.hpp"
#include "sftp.h"
#include "sftp_worker_thread.h"

#include <wx/filename.h>

SFTPSyncJob::SFTPSyncJob(SFTP* plugin,
                         const wxString& account,
                         const wxString& remoteFolder,
                         const wxArrayString& localFiles)
    : m_plugin(plugin)
    , m_account(account)
    , m_remoteFolder(remoteFolder)
    , m_files(localFiles)
{
    Bind(wxEVT_SFTP_NEW_FOLDER_COMPLETED, &SFTPSyncJob::OnFolderCreated, this);
    Bind(wxEVT_SFTP_NEW_FOLDER_ERROR, &SFTPSyncJob::OnFolderError, this);
    Bind(wxEVT_SFTP_ASYNC_SAVE_COMPLETED, &SFTPSyncJob::OnUploadCompleted, this);
    Bind(wxEVT_SFTP_ASYNC_SAVE_ERROR, &SFTPSyncJob::OnUploadError, this);
}

SFTPSyncJob::~SFTPSyncJob()
{
    Unbind(wxEVT_SFTP_NEW_FOLDER_COMPLETED, &SFTPSyncJob::OnFolderCreated, this);
    Unbind(wxEVT_SFTP_NEW_FOLDER_ERROR, &SFTPSyncJob::OnFolderError, this);
    Unbind(wxEVT_SFTP_ASYNC_SAVE_COMPLETED, &SFTPSyncJob::OnUploadCompleted, this);
    Unbind(wxEVT_SFTP_ASYNC_SAVE_ERROR, &SFTPSyncJob::OnUploadError, this);
}

void SFTPSyncJob::Start()
{
    auto* page = m_plugin->GetOutputPane();
    page->BeginSync(m_files.GetCount());
    Log(SFTPThreadMessage::STATUS_NONE,
        wxString::Format(_("Syncing %d file(s) to %s"), static_cast<int>(m_files.GetCount()), m_remoteFolder));

    // The queue is FIFO: the uploads start only after the folder is created
    clSFTPManager::Get().AsyncNewFolder(m_remoteFolder, m_account, this);
}

void SFTPSyncJob::Detach()
{
    m_plugin = nullptr;
    m_cancelled = true;
}

void SFTPSyncJob::Cancel()
{
    if (m_cancelled) {
        return;
    }
    m_cancelled = true;
    Log(SFTPThreadMessage::STATUS_NONE, _("Cancelling... the next files will not be uploaded"));
}

void SFTPSyncJob::OnFolderCreated(clSFTPEvent& event)
{
    wxUnusedVar(event);
    UploadNext();
}

void SFTPSyncJob::OnFolderError(clSFTPEvent& event)
{
    Log(SFTPThreadMessage::STATUS_ERROR,
        wxString::Format(_("Failed to create remote folder %s. %s"), m_remoteFolder, event.GetString()));
    m_failed = m_files.GetCount();
    Finish();
}

void SFTPSyncJob::UploadNext()
{
    if (m_cancelled || m_plugin == nullptr || m_next >= m_files.GetCount()) {
        Finish();
        return;
    }

    const wxString& local_file = m_files[m_next];
    m_plugin->GetOutputPane()->UpdateSync(m_next, m_files.GetCount(), wxFileName(local_file).GetFullName());
    clSFTPManager::Get().AsyncSaveFile(local_file, GetRemotePath(local_file), m_account, this);
}

void SFTPSyncJob::OnUploadCompleted(clCommandEvent& event)
{
    ++m_succeeded;
    ++m_next;
    Log(SFTPThreadMessage::STATUS_OK, wxString::Format(_("Uploaded: %s"), event.GetFileName()));
    UploadNext();
}

void SFTPSyncJob::OnUploadError(clCommandEvent& event)
{
    ++m_failed;
    ++m_next;
    Log(SFTPThreadMessage::STATUS_ERROR,
        wxString::Format(_("Failed to upload: %s. %s"), event.GetFileName(), event.GetString()));
    UploadNext();
}

void SFTPSyncJob::Finish()
{
    if (m_plugin == nullptr) {
        // detached: nobody is listening, just delete ourselves
        CallAfter([this]() { delete this; });
        return;
    }

    size_t skipped = m_files.GetCount() - (m_succeeded + m_failed);
    wxString summary = wxString::Format(
        _("Sync finished: %d uploaded, %d failed"), static_cast<int>(m_succeeded), static_cast<int>(m_failed));
    if (skipped > 0) {
        summary << wxString::Format(_(", %d skipped"), static_cast<int>(skipped));
    }
    Log(m_failed > 0 ? SFTPThreadMessage::STATUS_ERROR : SFTPThreadMessage::STATUS_OK, summary);

    m_plugin->GetOutputPane()->EndSync();

    // We can't delete ourselves from within our own event handler
    m_plugin->CallAfter(&SFTP::OnSyncJobFinished);
}

void SFTPSyncJob::Log(int status, const wxString& message)
{
    if (m_plugin == nullptr) {
        return;
    }

    auto* msg = new SFTPThreadMessage();
    msg->SetAccount(m_account);
    msg->SetMessage(message);
    msg->SetStatus(status);
    m_plugin->GetOutputPane()->AddLine(msg); // takes ownership
}

wxString SFTPSyncJob::GetRemotePath(const wxString& localFile) const
{
    wxString remote = m_remoteFolder;
    if (!remote.EndsWith("/")) {
        remote << "/";
    }
    return remote + wxFileName(localFile).GetFullName();
}
