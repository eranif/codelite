#include "SFTPSyncJob.h"

#include "SFTPStatusPage.h"
#include "clSFTPManager.hpp"
#include "sftp.h"
#include "sftp_worker_thread.h"

#include <wx/filename.h>

namespace
{
/// Return the folder part of a remote path (`/a/b/c.txt` -> `/a/b`)
wxString GetParentFolder(const wxString& remotePath)
{
    const int pos = remotePath.Find('/', true);
    if (pos == wxNOT_FOUND) {
        return wxEmptyString;
    }
    return pos == 0 ? wxString('/') : remotePath.Left(pos);
}
} // namespace

SFTPSyncJob::SFTPSyncJob(SFTP* plugin, const wxString& account, std::vector<SFTPSyncItem> items)
    : m_plugin(plugin)
    , m_account(account)
    , m_items(std::move(items))
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
    page->BeginSync(m_items.size());
    Log(SFTPThreadMessage::STATUS_NONE,
        wxString::Format(_("Syncing %d file(s) using account %s"), static_cast<int>(m_items.size()), m_account));

    // Create each remote folder once. The uploads start after all the folders are created
    std::set<wxString> folders;
    for (const auto& item : m_items) {
        folders.insert(GetParentFolder(item.remote));
    }

    m_pendingFolders = folders.size();
    if (m_pendingFolders == 0) {
        UploadNext();
        return;
    }
    for (const auto& folder : folders) {
        clSFTPManager::Get().AsyncNewFolder(folder, m_account, this);
    }
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
    if (--m_pendingFolders == 0) {
        UploadNext();
    }
}

void SFTPSyncJob::OnFolderError(clSFTPEvent& event)
{
    Log(SFTPThreadMessage::STATUS_ERROR,
        wxString::Format(_("Failed to create remote folder %s. %s"), event.GetRemoteFile(), event.GetString()));
    m_failedFolders.insert(event.GetRemoteFile());
    if (--m_pendingFolders == 0) {
        UploadNext();
    }
}

void SFTPSyncJob::UploadNext()
{
    // Files whose remote folder could not be created are failed without trying
    while (m_plugin != nullptr && !m_cancelled && m_next < m_items.size() &&
           m_failedFolders.count(GetParentFolder(m_items[m_next].remote)) > 0) {
        Log(SFTPThreadMessage::STATUS_ERROR,
            wxString::Format(_("Failed to upload: %s. The remote folder was not created"), m_items[m_next].local));
        ++m_failed;
        ++m_next;
    }

    if (m_cancelled || m_plugin == nullptr || m_next >= m_items.size()) {
        Finish();
        return;
    }

    const SFTPSyncItem& item = m_items[m_next];
    m_plugin->GetOutputPane()->UpdateSync(m_next, m_items.size(), wxFileName(item.local).GetFullName());
    clSFTPManager::Get().AsyncSaveFile(item.local, item.remote, m_account, this);
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

    size_t skipped = m_items.size() - (m_succeeded + m_failed);
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
