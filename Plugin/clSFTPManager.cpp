#if USE_SFTP
#include "clSFTPManager.hpp"

#include "SFTPClientData.hpp"
#include "StringUtils.h"
#include "clSFTPEvent.h"
#include "clSSHChannelCommon.hpp"
#include "clTempFile.hpp"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "libssh/libssh.h"
#include "macros.h"

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <thread>
#include <vector>
#include <wx/debug.h>
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/stc/stc.h>
#include <wx/thread.h>
#include <wx/utils.h>

wxDEFINE_EVENT(wxEVT_SFTP_ASYNC_SAVE_COMPLETED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SFTP_ASYNC_SAVE_ERROR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SFTP_ASYNC_EXEC_ERROR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SFTP_ASYNC_EXEC_STDOUT, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SFTP_ASYNC_EXEC_STDERR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SFTP_ASYNC_EXEC_DONE, clCommandEvent);

clSFTPManager::clSFTPManager()
{
    EventNotifier::Get()->Bind(wxEVT_GOING_DOWN, &clSFTPManager::OnGoingDown, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &clSFTPManager::OnFileSaved, this);
    m_eventsConnected = true;
    m_shutdown.store(false);

    m_timer = new wxTimer(this);
    m_timer->Start(10000); // 10 seconds should be good enough for a "keep-alive" interval
    Bind(wxEVT_TIMER, &clSFTPManager::OnTimer, this, m_timer->GetId());
    Bind(wxEVT_SFTP_ASYNC_SAVE_COMPLETED, &clSFTPManager::OnSaveCompleted, this);
    Bind(wxEVT_SFTP_ASYNC_SAVE_ERROR, &clSFTPManager::OnSaveError, this);
    StartWorkerThread();
}

clSFTPManager::~clSFTPManager()
{
    StopWorkerThread();
    if (m_eventsConnected) {
        EventNotifier::Get()->Unbind(wxEVT_GOING_DOWN, &clSFTPManager::OnGoingDown, this);
        EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &clSFTPManager::OnFileSaved, this);
        m_eventsConnected = false;
    }

    if (m_timer) {
        Unbind(wxEVT_TIMER, &clSFTPManager::OnTimer, this, m_timer->GetId());
        m_timer->Stop();
        wxDELETE(m_timer);
    }
    Unbind(wxEVT_SFTP_ASYNC_SAVE_COMPLETED, &clSFTPManager::OnSaveCompleted, this);
    Unbind(wxEVT_SFTP_ASYNC_SAVE_ERROR, &clSFTPManager::OnSaveError, this);
}

clSFTPManager& clSFTPManager::Get()
{
    static clSFTPManager manager;
    return manager;
}

void clSFTPManager::Release()
{
    StopWorkerThread();
    while (!m_connections.empty()) {
        const auto& conn_info = *(m_connections.begin());
        DeleteConnection(conn_info.first, false);
    }
    m_connections.clear();

    if (m_eventsConnected) {
        EventNotifier::Get()->Unbind(wxEVT_GOING_DOWN, &clSFTPManager::OnGoingDown, this);
        EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &clSFTPManager::OnFileSaved, this);
        m_eventsConnected = false;
    }
    if (m_timer) {
        Unbind(wxEVT_TIMER, &clSFTPManager::OnTimer, this, m_timer->GetId());
        m_timer->Stop();
        wxDELETE(m_timer);
    }
    if (m_worker_thread) {
        m_shutdown.store(true);
        m_worker_thread->join();
        wxDELETE(m_worker_thread);
    }
}

bool clSFTPManager::AddConnection(const wxString& account_name, bool replace)
{
    auto acc = SSHAccountInfo::LoadAccount(account_name);
    if (!acc.IsOk()) {
        return false;
    }
    return AddConnection(acc, replace);
}

bool clSFTPManager::AddConnection(const SSHAccountInfo& account, bool replace)
{
    wxBusyCursor bc;
    {
        auto iter = m_connections.find(account.GetAccountName());
        if (iter != m_connections.end()) {
            if (!replace) {
                // do not replace the current connection
                return true;
            }
            m_connections.erase(iter);
        }
    }

    try {
        clSSH::Ptr_t ssh(new clSSH(account.GetHost(), account.GetUsername(), account.GetPassword(),
                                   account.GetKeyFiles(), account.GetPort()));
        EnvSetter env;
        ssh->Open();
        wxString message;
        if (!ssh->AuthenticateServer(message)) {
            if (::wxMessageBox(message, "SSH", wxYES_NO | wxCENTER | wxICON_QUESTION) == wxYES) {
                ssh->AcceptServerAuthentication();
            } else {
                return false;
            }
        }
        ssh->Login();
        clSFTP::Ptr_t sftp(new clSFTP(ssh));
        sftp->Initialize();
        sftp->SetAccount(account.GetAccountName());
        m_connections.insert({ account.GetAccountName(), { account, sftp } });

        // Notify that a session is established
        clSFTPEvent event(wxEVT_SFTP_SESSION_OPENED);
        event.SetAccount(account.GetAccountName());
        EventNotifier::Get()->AddPendingEvent(event);

    } catch (const clException& e) {
        clERROR() << "AddConnection() error:" << e.What();
        return false;
    }
    return true;
}

std::pair<SSHAccountInfo, clSFTP::Ptr_t> clSFTPManager::GetConnectionPair(const wxString& account) const
{
    auto iter = m_connections.find(account);
    if (iter == m_connections.end()) {
        return { {}, clSFTP::Ptr_t(nullptr) };
    }
    return iter->second;
}

clSFTP::Ptr_t clSFTPManager::GetConnectionPtr(const wxString& account) const
{
    auto iter = m_connections.find(account);
    if (iter == m_connections.end()) {
        return clSFTP::Ptr_t(nullptr);
    }
    return iter->second.second;
}

clSFTP::Ptr_t clSFTPManager::GetConnectionPtrAddIfMissing(const wxString& account)
{
    auto iter = m_connections.find(account);
    if (iter != m_connections.end()) {
        return iter->second.second;
    }
    // no such connection, add it
    auto acc = SSHAccountInfo::LoadAccount(account);
    if (acc.GetAccountName().empty()) {
        return clSFTP::Ptr_t(nullptr);
    }
    if (!AddConnection(acc)) {
        return clSFTP::Ptr_t(nullptr);
    }
    return m_connections[account].second;
}

SFTPClientData* clSFTPManager::GetSFTPClientData(IEditor* editor)
{
    auto clientData = editor->GetClientData("sftp");
    if (!clientData) {
        return nullptr;
    }
    return dynamic_cast<SFTPClientData*>(clientData);
}

IEditor* clSFTPManager::OpenFile(const wxString& path, const SSHAccountInfo& accountInfo)
{
    wxBusyCursor bc;
    bool res = AddConnection(accountInfo);
    if (!res) {
        return nullptr;
    }
    return OpenFile(path, accountInfo.GetAccountName());
}

IEditor* clSFTPManager::OpenFile(const wxString& path, const wxString& accountName)
{
    // if the file is already opened in the editor, return it
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
    for (auto editor : editors) {
        auto clientData = GetSFTPClientData(editor);
        if (clientData && clientData->GetAccountName() == accountName && clientData->GetRemotePath() == path) {
            clGetManager()->SelectPage(editor->GetCtrl());
            return editor;
        }
    }

    auto account = SSHAccountInfo::LoadAccount(accountName);
    if (account.GetAccountName().empty()) {
        m_lastError.clear();
        m_lastError << "failed to locate account:" << accountName;
        return nullptr;
    }

    wxFileName localPath = clSFTP::GetLocalFileName(account, path, true);
    if (!DoSyncDownload(path, localPath.GetFullPath(), accountName)) {
        return nullptr;
    }
    return clGetManager()->OpenRemoteFile(localPath.GetFullPath(), path, accountName, wxEmptyString);
}

bool clSFTPManager::DoSyncReadFile(const wxString& remotePath, const wxString& accountName, wxMemoryBuffer* content)
{
    clDEBUG() << "SFTP Manager: reading file (sync):" << remotePath << "for account:" << accountName << endl;
    auto conn = GetConnectionPtrAddIfMissing(accountName);
    CHECK_PTR_RET_FALSE(conn);

    std::promise<wxMemoryBuffer*> read_promise;
    auto future = read_promise.get_future();

    auto read_func = [&read_promise, remotePath, conn, accountName]() {
        // read the file content
        SFTPAttribute::Ptr_t fileAttr;
        wxMemoryBuffer* buffer = new wxMemoryBuffer;
        try {
            // read the file content
            fileAttr = conn->Read(remotePath, *buffer);
            wxUnusedVar(fileAttr);
            read_promise.set_value(buffer);

        } catch (const clException& e) {
            clERROR() << "Failed to read remote file:" << remotePath << "." << e.What();
            wxDELETE(buffer);
            read_promise.set_value(nullptr);
        }
    };

    // queue the task and wait for the response
    m_q.push_back(std::move(read_func));
    auto buffer = future.get();
    if (!buffer) {
        return false;
    }

    // convert the memory buffer to string and return true
    size_t len = buffer->GetDataLen();
    content->AppendData(buffer->release(), len);
    wxDELETE(buffer);
    return true;
}

void clSFTPManager::DoAsyncReadFile(const wxString& remotePath, const wxString& accountName, wxEvtHandler* sink)
{
    clDEBUG() << "SFTP Manager: reading file (async):" << remotePath << "for account:" << accountName << endl;
    auto conn = GetConnectionPtrAddIfMissing(accountName);
    CHECK_PTR_RET(conn);

    auto read_func = [remotePath, conn, accountName, sink]() {
        // build the local file path
        SFTPAttribute::Ptr_t fileAttr;
        try {
            // read the file content
            wxMemoryBuffer buffer;
            fileAttr = conn->Read(remotePath, buffer);
            wxUnusedVar(fileAttr);

            // convert to string and fire an event
            wxString content((const char*)buffer.GetData(), buffer.GetDataLen());
            clSFTPEvent event_read{ wxEVT_SFTP_FILE_READ };
            event_read.SetAccount(accountName);
            event_read.SetRemoteFile(remotePath);
            event_read.SetContent(content);
            sink->QueueEvent(event_read.Clone());

        } catch (const clException& e) {
            clERROR() << "Failed to read remote file:" << remotePath << "." << e.What();
        }
    };
    m_q.push_back(std::move(read_func));
}

bool clSFTPManager::DoSyncDownload(const wxString& remotePath, const wxString& localPath, const wxString& accountName)
{
    // Open it
    clDEBUG() << "SFTP Manager: downloading file" << remotePath << "->" << localPath << "for account:" << accountName
              << endl;
    auto conn = GetConnectionPtrAddIfMissing(accountName);
    CHECK_PTR_RET_FALSE(conn);

    // if the local file already exists, compare checksums before downloading it
    if (wxFileName::FileExists(localPath)) {
        clDEBUG() << "Local file with the same path already exists, comapring checksums..." << endl;
        try {
            // read the file size
            size_t remote_file_checksum, local_file_checksum;
            if (conn->GetChecksum(remotePath, &remote_file_checksum) &&
                FileUtils::GetChecksum(localPath, &local_file_checksum) &&
                local_file_checksum == remote_file_checksum) {
                clDEBUG() << "Using cached local file (checksum are the same)" << endl;
                return true;
            }
        } catch (const clException& e) {
            wxUnusedVar(e);
        }
    }

    wxMemoryBuffer file_content;
    if (!DoSyncReadFile(remotePath, accountName, &file_content)) {
        return false;
    }

    {
        wxLogNull no_log;
        wxFFile fp(localPath, "w+b");
        if (!fp.IsOpened()) {
            clERROR() << "failed to open local file:" << localPath << "for write" << endl;
            return false;
        }
        fp.Write(file_content.GetData(), file_content.GetDataLen());
    }

    // mark this file as ours
    saved_file info;
    info.account_name = accountName;
    info.local_path = localPath;
    info.remote_path = remotePath;
    m_downloadedFileToAccount.insert({ localPath, info });
    return true;
}

void clSFTPManager::DoAsyncSaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName,
                                    bool delete_local, wxEvtHandler* sink)
{
    // save file async
    auto conn = GetConnectionPtrAddIfMissing(accountName);
    CHECK_PTR_RET(conn);

    // prepare the download work
    auto save_func = [localPath, remotePath, conn, sink, delete_local]() {
        try {
            conn->Write(localPath, remotePath);
            if (sink) {
                // notify about save success
                clCommandEvent success_event(wxEVT_SFTP_ASYNC_SAVE_COMPLETED);
                success_event.SetFileName(remotePath);
                success_event.SetSshAccount(conn->GetAccount());
                sink->AddPendingEvent(success_event);
            }
        } catch (const clException& e) {
            clERROR() << "(AsyncSaveFile): Failed to write file:" << remotePath << "." << e.What();
            clCommandEvent fail_event(wxEVT_SFTP_ASYNC_SAVE_ERROR);
            fail_event.SetFileName(remotePath);
            fail_event.SetString(e.What());
            fail_event.SetSshAccount(conn->GetAccount());
            sink->AddPendingEvent(fail_event);
        }
        // always delete the file
        if (delete_local) {
            clRemoveFile(localPath);
        }
    };
    m_q.push_back(std::move(save_func));
}

bool clSFTPManager::DoSyncSaveFileWithConn(clSFTP::Ptr_t conn, const wxString& localPath, const wxString& remotePath,
                                           bool delete_local)
{
    // prepare the download work
    std::promise<bool> save_promise;
    auto future = save_promise.get_future();
    auto save_func = [localPath, remotePath, conn, delete_local, &save_promise]() {
        try {
            conn->Write(localPath, remotePath);
            save_promise.set_value(true);
        } catch (const clException& e) {
            clERROR() << "Failed to write file:" << remotePath << "." << e.What();
            save_promise.set_value(false);
        }

        // always delete the file
        if (delete_local) {
            clRemoveFile(localPath);
        }
    };
    m_q.push_back(std::move(save_func));
    return future.get();
}

bool clSFTPManager::DoSyncSaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName,
                                   bool delete_local)
{
    // save file async
    auto conn = GetConnectionPtrAddIfMissing(accountName);
    CHECK_PTR_RET_FALSE(conn);
    return DoSyncSaveFileWithConn(conn, localPath, remotePath, delete_local);
}

void clSFTPManager::AsyncSaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName,
                                  wxEvtHandler* sink)
{
    clDEBUG() << "(AsyncSaveFile):" << remotePath << "for account" << accountName << endl;
    DoAsyncSaveFile(localPath, remotePath, accountName, false, sink == nullptr ? this : sink);
}

void clSFTPManager::OnFileSaved(clCommandEvent& event)
{
    event.Skip();
    if (clGetManager()->IsShutdownInProgress()) {
        return;
    }

    const wxString& filename = event.GetString();
    auto editor = clGetManager()->FindEditor(filename);
    CHECK_PTR_RET(editor);
    CHECK_PTR_RET(editor->IsRemoteFile());

    auto cd = editor->GetRemoteData();
    CHECK_PTR_RET(cd);

    auto conn_info = GetConnectionPair(cd->GetAccountName());
    CHECK_PTR_RET(conn_info.second);
    AsyncSaveFile(cd->GetLocalPath(), cd->GetRemotePath(), conn_info.first.GetAccountName(), EventNotifier::Get());
}

bool clSFTPManager::AwaitSaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName)
{
    return DoSyncSaveFile(localPath, remotePath, accountName, false);
}

bool clSFTPManager::AwaitWriteFile(const wxString& content, const wxString& remotePath, const wxString& accountName)
{
    clTempFile tmpfile;
    tmpfile.Persist(); // do not delete it on exit
    if (!tmpfile.Write(content, wxConvUTF8)) {
        return false;
    }
    return DoSyncSaveFile(tmpfile.GetFullPath(), remotePath, accountName, true);
}

bool clSFTPManager::AwaitWriteFile(clSFTP::Ptr_t sftp, const wxString& content, const wxString& remotePath)
{
    clTempFile tmpfile;
    tmpfile.Persist(); // do not delete it on exit
    if (!tmpfile.Write(content, wxConvUTF8)) {
        return false;
    }
    return DoSyncSaveFileWithConn(sftp, tmpfile.GetFullPath(), remotePath, true);
}

bool clSFTPManager::DeleteConnection(const wxString& accountName, bool promptUser)
{
    auto iter = m_connections.find(accountName);
    if (iter == m_connections.end()) {
        // nothing to be done here
        return false;
    }

    // close all editors owned by this account
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
    for (auto editor : editors) {
        auto cd = GetSFTPClientData(editor);
        if (cd && (cd->GetAccountName() == accountName)) {
            if (!clGetManager()->CloseEditor(editor, promptUser)) {
                // user cancelled
                return false;
            }
        }
    }

    // before we can delete a connection, we must stop the worker thread
    // delete the connection and then restart it
    StopWorkerThread();

    // notify that a session was closed
    clSFTPEvent event(wxEVT_SFTP_SESSION_CLOSED);
    event.SetAccount(accountName);
    EventNotifier::Get()->AddPendingEvent(event);

    // and finally remove the connection
    m_connections.erase(iter);

    // start the worker thread again
    StartWorkerThread();
    return true;
}

clResult<SFTPAttribute::List_t, bool> clSFTPManager::List(const wxString& path, const SSHAccountInfo& accountInfo)
{
    wxBusyCursor bc;
    // save file async
    auto conn = GetConnectionPtrAddIfMissing(accountInfo.GetAccountName());
    if (!conn) {
        return {};
    }

    // prepare the download work
    SFTPAttribute::List_t result;
    std::promise<bool> promise;
    auto future = promise.get_future();
    auto func = [conn, path, &result, &promise]() {
        try {
            auto attr = conn->List(path, clSFTP::SFTP_BROWSE_FILES | clSFTP::SFTP_BROWSE_FOLDERS);
            result.swap(attr);
            promise.set_value(true);
        } catch (const clException& e) {
            clERROR() << "List error." << e.What();
            promise.set_value(false);
        }
    };
    m_q.push_back(std::move(func));
    if (!future.get()) {
        return clResult<SFTPAttribute::List_t, bool>::make_error(false);
    }
    return result;
}

bool clSFTPManager::NewFile(const wxString& path, const SSHAccountInfo& accountInfo)
{
    auto conn = GetConnectionPtrAddIfMissing(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    // prepare the download work
    std::promise<bool> promise;
    auto future = promise.get_future();
    auto func = [conn, path, &promise]() {
        try {
            conn->CreateEmptyFile(path);
            promise.set_value(true);
        } catch (const clException& e) {
            clDEBUG() << "NewFile() error." << e.What();
            promise.set_value(false);
        }
    };
    m_q.push_back(std::move(func));
    return future.get();
}

void clSFTPManager::OnGoingDown(clCommandEvent& event)
{
    event.Skip();
    clDEBUG() << "SFTP manager is shutting down..." << endl;
    Release();
}

bool clSFTPManager::NewFolder(const wxString& path, const SSHAccountInfo& accountInfo)
{
    auto conn = GetConnectionPtrAddIfMissing(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    // prepare the download work
    std::promise<bool> promise;
    auto future = promise.get_future();
    auto func = [conn, path, &promise]() {
        try {
            conn->CreateDir(path);
            promise.set_value(true);
        } catch (const clException& e) {
            clDEBUG() << "NewFolder() error." << e.What();
            promise.set_value(false);
        }
    };
    m_q.push_back(std::move(func));
    return future.get();
}

bool clSFTPManager::Rename(const wxString& oldpath, const wxString& newpath, const SSHAccountInfo& accountInfo)
{
    auto conn = GetConnectionPtrAddIfMissing(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    // prepare the download work
    std::promise<bool> promise;
    auto future = promise.get_future();
    auto func = [conn, oldpath, newpath, &promise]() {
        try {
            conn->Rename(oldpath, newpath);
            promise.set_value(true);
        } catch (const clException& e) {
            clERROR() << "Rename() error." << e.What();
            promise.set_value(false);
        }
    };
    m_q.push_back(std::move(func));
    return future.get();
}

bool clSFTPManager::DeleteDir(const wxString& fullpath, const SSHAccountInfo& accountInfo)
{
    auto conn = GetConnectionPtrAddIfMissing(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    // prepare the download work
    std::promise<bool> promise;
    auto future = promise.get_future();
    auto func = [conn, fullpath, &promise]() {
        try {
            conn->RemoveDir(fullpath);
            promise.set_value(true);
        } catch (const clException& e) {
            clERROR() << "Rename() error." << e.What();
            promise.set_value(false);
        }
    };
    m_q.push_back(std::move(func));
    return future.get();
}

bool clSFTPManager::UnlinkFile(const wxString& fullpath, const SSHAccountInfo& accountInfo)
{
    // save file async
    auto conn = GetConnectionPtrAddIfMissing(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    // prepare the download work
    std::promise<bool> promise;
    auto future = promise.get_future();
    auto func = [conn, fullpath, &promise]() {
        try {
            conn->UnlinkFile(fullpath);
            promise.set_value(true);
        } catch (const clException& e) {
            clERROR() << "Rename() error." << e.What();
            promise.set_value(false);
        }
    };
    m_q.push_back(std::move(func));
    return future.get();
}

void clSFTPManager::OnTimer(wxTimerEvent& event)
{
    event.Skip();

    std::vector<clSFTP::Ptr_t> all_connections;
    size_t count = GetAllConnectionsPtr(all_connections);
    if (count == 0) {
        return;
    }

    // push SendKeepAlive() function for all the connections that we hold
    for (auto conn : all_connections) {
        auto func = [conn]() {
            try {
                conn->SendKeepAlive();
            } catch (const clException& e) {
                clERROR() << "failed to send keep-alive message for account:" << e.What() << endl;
            }
        };
        m_q.push_back(std::move(func));
    }
}

bool clSFTPManager::IsFileExists(const wxString& fullpath, const SSHAccountInfo& accountInfo)
{
    auto conn = GetConnectionPtrAddIfMissing(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    // prepare the download work
    std::promise<bool> promise;
    auto future = promise.get_future();
    auto func = [conn, fullpath, &promise]() {
        try {
            auto d = conn->Stat(fullpath);
            promise.set_value(d->IsFile());
        } catch (const clException& e) {
            clDEBUG() << "IsFileExists() error." << e.What();
            promise.set_value(false);
        }
    };
    m_q.push_back(std::move(func));
    return future.get();
}

bool clSFTPManager::IsDirExists(const wxString& fullpath, const SSHAccountInfo& accountInfo)
{
    auto conn = GetConnectionPtrAddIfMissing(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    // prepare the download work
    std::promise<bool> promise;
    auto future = promise.get_future();
    auto func = [conn, fullpath, &promise]() {
        try {
            auto d = conn->Stat(fullpath);
            promise.set_value(d->IsFolder());
        } catch (const clException& e) {
            clERROR() << "IsDirExists() error." << e.What();
            promise.set_value(false);
        }
    };
    m_q.push_back(std::move(func));
    return future.get();
}

wxFileName clSFTPManager::Download(const wxString& path, const wxString& accountName, const wxString& localFileName)
{
    clDEBUG() << "SFTP Manager: Download() called for" << path << endl;
    wxFileName local_file;
    if (localFileName.empty()) {
        local_file = wxFileName(clStandardPaths::Get().GetTempDir(), path.AfterLast('/'));
        local_file.AppendDir("sftp-downloads");
    } else {
        local_file = wxFileName(localFileName);
    }
    local_file.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    if (DoSyncDownload(path, local_file.GetFullPath(), accountName)) {
        return local_file;
    }
    return {};
}

void clSFTPManager::AsyncWriteFile(const wxString& content, const wxString& remotePath, const wxString& accountName,
                                   wxEvtHandler* sink)
{
    clTempFile tmpfile;
    tmpfile.Persist(); // do not delete it on exit
    if (!tmpfile.Write(content, wxConvUTF8)) {
        return;
    }
    AsyncSaveFile(tmpfile.GetFullPath(), remotePath, accountName, sink);
}

bool clSFTPManager::GetRemotePath(const wxString& local_path, const wxString& accountName, wxString& remote_path)
{
    auto p = GetConnectionPair(accountName);
    CHECK_PTR_RET_FALSE(p.second);

    const auto& account_info = p.first;
    wxString download_folder = clSFTP::GetDefaultDownloadFolder(account_info);
    wxString rest;
    if (local_path.StartsWith(download_folder, &rest)) {
        rest.Replace("\\", "/");
        remote_path.swap(rest);
        return true;
    }
    return false;
}

bool clSFTPManager::GetLocalPath(const wxString& remote_path, const wxString& accountName, wxString& local_path)
{
    auto p = GetConnectionPair(accountName);
    CHECK_PTR_RET_FALSE(p.second);

    const auto& account_info = p.first;
    wxFileName fn = clSFTP::GetLocalFileName(account_info, remote_path);
    if (fn.IsOk()) {
        local_path = fn.GetFullPath();
        return true;
    }
    return false;
}

void clSFTPManager::StopWorkerThread()
{
    if (m_worker_thread) {
        m_shutdown.store(true);
        m_worker_thread->join();
        wxDELETE(m_worker_thread);
    }
    m_shutdown.store(false);
}

void clSFTPManager::StartWorkerThread()
{
    if (m_worker_thread) {
        return;
    }

    m_worker_thread = new std::thread(
        [](SyncQueue<std::function<void()>>& Q, std::atomic_bool& shutdown) {
            while (!shutdown.load()) {
                auto work_func = Q.pop_front();
                if (work_func == nullptr) {
                    continue;
                }
                work_func();
            }
        },
        std::ref(m_q), std::ref(m_shutdown));
}

void clSFTPManager::OnSaveCompleted(clCommandEvent& e)
{
    clGetManager()->SetStatusMessage("SFTP: " + e.GetFileName() + _(" saved"), 3);
}

void clSFTPManager::OnSaveError(clCommandEvent& e)
{
    m_lastError.clear();
    m_lastError << "SaveError: " << e.GetString();
    clWARNING() << m_lastError << endl;
    wxString errormsg = _("SFTP error: failed to save file. ") + e.GetString();
    errormsg.Trim();
    clGetManager()->GetStatusBar()->SetMessage(errormsg);
}

bool clSFTPManager::IsRemoteFile(const wxString& filepath, wxString* account, wxString* remote_path) const
{
    if (m_downloadedFileToAccount.count(filepath) == 0) {
        return false;
    }

    *account = m_downloadedFileToAccount.find(filepath)->second.account_name;
    *remote_path = m_downloadedFileToAccount.find(filepath)->second.remote_path;
    return true;
}

size_t clSFTPManager::GetAllConnectionsPtr(std::vector<clSFTP::Ptr_t>& connections) const
{
    if (m_connections.empty()) {
        return 0;
    }
    connections.clear();
    connections.reserve(m_connections.size());
    for (const auto& vt : m_connections) {
        auto conn = GetConnectionPtr(vt.first);
        if (!conn) {
            continue;
        }
        connections.push_back(conn);
    }
    return connections.size();
}

void clSFTPManager::AsyncReadFile(const wxString& remotePath, const wxString& accountName, wxEvtHandler* sink)
{
    DoAsyncReadFile(remotePath, accountName, sink);
}

bool clSFTPManager::AwaitReadFile(const wxString& remotePath, const wxString& accountName, wxMemoryBuffer* content)
{
    return DoSyncReadFile(remotePath, accountName, content);
}

#define QUEUE_ERROR_EVENT(msg)                                  \
    {                                                           \
        clSFTPEvent event_error{ wxEVT_SFTP_ASYNC_EXEC_ERROR }; \
        event_error.SetString(msg);                             \
        sink->AddPendingEvent(event_error);                     \
    }

ReadOutput_t clSFTPManager::AwaitExecute(const wxString& accountName, const wxString& command, const wxString& wd,
                                         clEnvList_t* env)
{
    clDEBUG() << "SFTP Manager: AwaitExecute:" << command << "for account:" << accountName << endl;
    auto conn = GetConnectionPtrAddIfMissing(accountName);
    if (!conn) {
        clWARNING() << "Failed to obtain connection for command:" << command << ". Account:" << accountName << endl;
        return {};
    }

    std::promise<ReadOutput_t> exec_promise;
    auto future = exec_promise.get_future();

    auto exec_func = [command, wd, conn, env, accountName, &exec_promise]() {
        // read the file content
        auto session = conn->GetSsh()->GetSession();
        auto channel = ssh_channel_new(conn->GetSsh()->GetSession());
        if (!channel) {
            clWARNING() << "Execution of command:" << command << "failed." << ssh_get_error(session) << endl;
            exec_promise.set_value({});
            return;
        }

        int rc = SSH_OK;
        rc = ssh_channel_open_session(channel);
        if (rc != SSH_OK) {
            ssh_channel_free(channel);
            clWARNING() << "Failed to open channel for command:" << command << "." << ssh_get_error(session) << endl;
            exec_promise.set_value({});
            return;
        }

        wxString cmd_w_dir;
        if (!wd.empty()) {
            cmd_w_dir << "cd " << StringUtils::WrapWithDoubleQuotes(wd) << " && " << command;
        } else {
            cmd_w_dir << command;
        }
        rc = ssh_channel_request_exec(channel, cmd_w_dir.mb_str(wxConvUTF8).data());
        if (rc != SSH_OK) {
            // mark the channel + ssh session as "broken"
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            clWARNING() << "Execution of command :" << cmd_w_dir << " failed." << ssh_get_error(session) << endl;
            exec_promise.set_value({});
            return;
        }

        std::tuple<std::string, std::string, int> result;
        std::string std_out;
        std::string std_err;
        ssh::channel_read_all(channel, &std_err, true);
        int exit_code = ssh::channel_read_all(channel, &std_out, false);

        result = { std_out, std_err, exit_code };
        exec_promise.set_value(std::move(result));

        // release the channel
        ssh_channel_close(channel);
        ssh_channel_free(channel);
    };
    m_q.push_back(std::move(exec_func));
    return future.get();
}

#define QUEUE_ERROR_EVENT(msg)                                  \
    {                                                           \
        clSFTPEvent event_error{ wxEVT_SFTP_ASYNC_EXEC_ERROR }; \
        event_error.SetString(msg);                             \
        sink->AddPendingEvent(event_error);                     \
    }

#define QUEUE_OUTPUT_EVENT(output, is_stdout)                                                            \
    {                                                                                                    \
        clSFTPEvent event_ok{ is_stdout ? wxEVT_SFTP_ASYNC_EXEC_STDOUT : wxEVT_SFTP_ASYNC_EXEC_STDERR }; \
        event_ok.SetStringRaw(output);                                                                   \
        sink->AddPendingEvent(event_ok);                                                                 \
    }

#define QUEUE_DONE_EVENT(exit_code)                           \
    {                                                         \
        clSFTPEvent event_done{ wxEVT_SFTP_ASYNC_EXEC_DONE }; \
        event_done.SetInt(exit_code);                         \
        sink->AddPendingEvent(event_done);                    \
    }

void clSFTPManager::AsyncExecute(wxEvtHandler* sink, const wxString& accountName, const wxString& command,
                                 const wxString& wd, clEnvList_t* env)
{
    clDEBUG() << "SFTP Manager: AsyncExecute:" << command << "for account:" << accountName << endl;
    auto conn = GetConnectionPtrAddIfMissing(accountName);
    if (!conn) {
        QUEUE_ERROR_EVENT(wxString() << "Failed to obtain connection for command:" << command
                                     << ". Account:" << accountName);
        return;
    }

    auto exec_func = [command, wd, conn, env, accountName, sink]() {
        // read the file content
        auto session = conn->GetSsh()->GetSession();
        auto channel = ssh_channel_new(conn->GetSsh()->GetSession());
        if (!channel) {
            QUEUE_ERROR_EVENT(wxString() << "Execution of command:" << command << "failed." << ssh_get_error(session));
            return;
        }

        int rc = SSH_OK;
        rc = ssh_channel_open_session(channel);
        if (rc != SSH_OK) {
            ssh_channel_free(channel);
            QUEUE_ERROR_EVENT(wxString() << "Failed to open channel for command:" << command << "."
                                         << ssh_get_error(session));
            return;
        }

        wxString cmd_w_dir;
        if (!wd.empty()) {
            cmd_w_dir << "cd " << StringUtils::WrapWithDoubleQuotes(wd) << " && " << command;
        } else {
            cmd_w_dir << command;
        }

        rc = ssh_channel_request_exec(channel, cmd_w_dir.mb_str(wxConvUTF8).data());
        if (rc != SSH_OK) {
            // mark the channel + ssh session as "broken"
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            QUEUE_ERROR_EVENT(wxString() << "Execution of command :" << cmd_w_dir << " failed."
                                         << ssh_get_error(session));
            return;
        }

        std::string std_out;
        std::string std_err;
        ssh::channel_read_all(channel, &std_err, true);
        if (!std_err.empty()) {
            QUEUE_OUTPUT_EVENT(std_err, false);
        }
        int exit_code = ssh::channel_read_all(channel, &std_out, false);
        if (!std_out.empty()) {
            QUEUE_OUTPUT_EVENT(std_out, true);
        }

        QUEUE_DONE_EVENT(exit_code);

        // release the channel
        ssh_channel_close(channel);
        ssh_channel_free(channel);
    };
    m_q.push_back(std::move(exec_func));
}
#endif
