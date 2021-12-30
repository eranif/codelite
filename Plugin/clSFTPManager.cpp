#if USE_SFTP
#include "clSFTPManager.hpp"

#include "SFTPClientData.hpp"
#include "clSFTPEvent.h"
#include "clTempFile.hpp"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "macros.h"
#include "wx/debug.h"
#include "wx/thread.h"

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <thread>
#include <vector>
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/stc/stc.h>
#include <wx/utils.h>

wxDEFINE_EVENT(wxEVT_SFTP_ASYNC_SAVE_COMPLETED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SFTP_ASYNC_SAVE_ERROR, clCommandEvent);

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
    if(m_eventsConnected) {
        EventNotifier::Get()->Unbind(wxEVT_GOING_DOWN, &clSFTPManager::OnGoingDown, this);
        EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &clSFTPManager::OnFileSaved, this);
        m_eventsConnected = false;
    }

    if(m_timer) {
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
    while(!m_connections.empty()) {
        const auto& conn_info = *(m_connections.begin());
        DeleteConnection(conn_info.first, false);
    }
    m_connections.clear();

    if(m_eventsConnected) {
        EventNotifier::Get()->Unbind(wxEVT_GOING_DOWN, &clSFTPManager::OnGoingDown, this);
        EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &clSFTPManager::OnFileSaved, this);
        m_eventsConnected = false;
    }
    if(m_timer) {
        Unbind(wxEVT_TIMER, &clSFTPManager::OnTimer, this, m_timer->GetId());
        m_timer->Stop();
        wxDELETE(m_timer);
    }
    if(m_worker_thread) {
        m_shutdown.store(true);
        m_worker_thread->join();
        wxDELETE(m_worker_thread);
    }
}

bool clSFTPManager::AddConnection(const SSHAccountInfo& account, bool replace)
{
    wxBusyCursor bc;
    {
        auto iter = m_connections.find(account.GetAccountName());
        if(iter != m_connections.end()) {
            if(!replace) {
                // do not replace the current connection
                return true;
            }
            m_connections.erase(iter);
        }
    }

    try {
        clSSH::Ptr_t ssh(new clSSH(account.GetHost(), account.GetUsername(), account.GetPassword(), account.GetPort()));
        ssh->Connect();
        wxString message;
        if(!ssh->AuthenticateServer(message)) {
            if(::wxMessageBox(message, "SSH", wxYES_NO | wxCENTER | wxICON_QUESTION) == wxYES) {
                ssh->AcceptServerAuthentication();
            } else {
                return false;
            }
        }
        ssh->Login();
        clSFTP::Ptr_t sftp(new clSFTP(ssh));
        sftp->Initialize();
        m_connections.insert({ account.GetAccountName(), { account, sftp } });

        // Notify that a session is established
        clSFTPEvent event(wxEVT_SFTP_SESSION_OPENED);
        event.SetAccount(account.GetAccountName());
        EventNotifier::Get()->AddPendingEvent(event);

    } catch(clException& e) {
        clERROR() << "AddConnection() error:" << e.What();
        return false;
    }
    return true;
}

std::pair<SSHAccountInfo, clSFTP::Ptr_t> clSFTPManager::GetConnectionPair(const wxString& account) const
{
    auto iter = m_connections.find(account);
    if(iter == m_connections.end()) {
        return { {}, clSFTP::Ptr_t(nullptr) };
    }
    return iter->second;
}

clSFTP::Ptr_t clSFTPManager::GetConnectionPtr(const wxString& account) const
{
    auto iter = m_connections.find(account);
    if(iter == m_connections.end()) {
        return clSFTP::Ptr_t(nullptr);
    }
    return iter->second.second;
}

clSFTP::Ptr_t clSFTPManager::GetConnectionPtrAddIfMissing(const wxString& account)
{
    auto iter = m_connections.find(account);
    if(iter != m_connections.end()) {
        return iter->second.second;
    }
    // no such connection, add it
    auto acc = SSHAccountInfo::LoadAccount(account);
    if(acc.GetAccountName().empty()) {
        return clSFTP::Ptr_t(nullptr);
    }
    if(!AddConnection(acc)) {
        return clSFTP::Ptr_t(nullptr);
    }
    return m_connections[account].second;
}

SFTPClientData* clSFTPManager::GetSFTPClientData(IEditor* editor)
{
    auto clientData = editor->GetClientData("sftp");
    if(!clientData) {
        return nullptr;
    }
    return dynamic_cast<SFTPClientData*>(clientData);
}

IEditor* clSFTPManager::OpenFile(const wxString& path, const SSHAccountInfo& accountInfo)
{
    wxBusyCursor bc;
    bool res = AddConnection(accountInfo);
    if(!res) {
        return nullptr;
    }
    return OpenFile(path, accountInfo.GetAccountName());
}

IEditor* clSFTPManager::OpenFile(const wxString& path, const wxString& accountName)
{
    // if the file is already opened in the editor, return it
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
    for(auto editor : editors) {
        auto clientData = GetSFTPClientData(editor);
        if(clientData && clientData->GetAccountName() == accountName && clientData->GetRemotePath() == path) {
            clGetManager()->SelectPage(editor->GetCtrl());
            return editor;
        }
    }

    auto account = SSHAccountInfo::LoadAccount(accountName);
    if(account.GetAccountName().empty()) {
        m_lastError.clear();
        m_lastError << "failed to locate account:" << accountName;
        return nullptr;
    }

    wxFileName localPath = clSFTP::GetLocalFileName(account, path, true);
    if(!DoSyncDownload(path, localPath.GetFullPath(), accountName)) {
        return nullptr;
    }

    // set the client data for this editor
    SFTPClientData* cd = new SFTPClientData;
    cd->SetLocalPath(localPath.GetFullPath());
    cd->SetRemotePath(path);
    cd->SetPermissions(0); // not used
    cd->SetLineNumber(wxNOT_FOUND);
    cd->SetAccountName(accountName);

    wxString tooltip;
    tooltip << "Local: " << cd->GetLocalPath() << "\n"
            << "Remote: " << cd->GetRemotePath();

    auto editor = clGetManager()->OpenFile(localPath.GetFullPath(), "download", tooltip);
    if(!editor) {
        // probably opened using the image viewer
        wxDELETE(cd);
        return nullptr;
    }
    editor->SetClientData("sftp", cd);
    return editor;
}

bool clSFTPManager::DoSyncDownload(const wxString& remotePath, const wxString& localPath, const wxString& accountName)
{
    // Open it
    clDEBUG() << "SFTP Manager: downloading file" << remotePath << "->" << localPath << "for account:" << accountName
              << endl;
    auto conn = GetConnectionPtrAddIfMissing(accountName);
    CHECK_PTR_RET_FALSE(conn);

    // if the local file already exists, compare checksums before downloading it
    if(wxFileName::FileExists(localPath)) {
        clDEBUG() << "Local file with the same path already exists, comapring checksums..." << endl;
        try {
            // read the file size
            size_t remote_file_checksum, local_file_checksum;
            if(conn->GetChecksum(remotePath, &remote_file_checksum) &&
               FileUtils::GetChecksum(localPath, &local_file_checksum) && local_file_checksum == remote_file_checksum) {
                clDEBUG() << "Using cached local file (checksum are the same)" << endl;
                return true;
            }
        } catch(clException& e) {
            wxUnusedVar(e);
        }
    }

    // prepare the download work
    std::promise<bool> download_promise;
    auto future = download_promise.get_future();
    auto download_func = [&download_promise, localPath, remotePath, conn]() {
        // build the local file path
        wxMemoryBuffer buffer;
        SFTPAttribute::Ptr_t fileAttr;
        try {
            fileAttr = conn->Read(remotePath, buffer);

            // write the content in the local file
            wxLogNull noLog;
            wxFFile fp(localPath, "w+b");
            if(fp.IsOpened()) {
                fp.Write(buffer.GetData(), buffer.GetDataLen());
                fp.Close();
                download_promise.set_value(true);
            } else {
                download_promise.set_value(false);
            }
        } catch(clException& e) {
            clERROR() << "Failed to open file:" << remotePath << "." << e.What();
            download_promise.set_value(false);
        }
    };
    m_q.push_back(std::move(download_func));

    // Wait for the worker thread to complete
    bool res = future.get();
    if(!res) {
        return false;
    }

    // remember this file as ours
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
            if(sink) {
                // notify about save success
                clCommandEvent success_event(wxEVT_SFTP_ASYNC_SAVE_COMPLETED);
                success_event.SetFileName(remotePath);
                sink->AddPendingEvent(success_event);
            }
        } catch(clException& e) {
            clERROR() << "Failed to write file:" << remotePath << "." << e.What();
            clCommandEvent fail_event(wxEVT_SFTP_ASYNC_SAVE_ERROR);
            fail_event.SetFileName(remotePath);
            fail_event.SetString(e.What());
            sink->AddPendingEvent(fail_event);
        }
        // always delete the file
        if(delete_local) {
            clRemoveFile(localPath);
        }
    };
    m_q.push_back(std::move(save_func));
}

bool clSFTPManager::DoSyncSaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName,
                                   bool delete_local)
{
    // save file async
    auto conn = GetConnectionPtrAddIfMissing(accountName);
    CHECK_PTR_RET_FALSE(conn);

    // prepare the download work
    std::promise<bool> save_promise;
    auto future = save_promise.get_future();
    auto save_func = [localPath, remotePath, conn, delete_local, &save_promise]() {
        try {
            conn->Write(localPath, remotePath);
        } catch(clException& e) {
            clERROR() << "Failed to write file:" << remotePath << "." << e.What();
            save_promise.set_value(false);
        }
        // always delete the file
        if(delete_local) {
            clRemoveFile(localPath);
        }
        save_promise.set_value(true);
    };
    m_q.push_back(std::move(save_func));
    return future.get();
}

void clSFTPManager::AsyncSaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName,
                                  wxEvtHandler* sink)
{
    DoAsyncSaveFile(localPath, remotePath, accountName, false, sink);
}

void clSFTPManager::OnFileSaved(clCommandEvent& event)
{
    event.Skip();
    if(clGetManager()->IsShutdownInProgress()) {
        return;
    }

    const wxString& filename = event.GetString();
    auto editor = clGetManager()->FindEditor(filename);
    CHECK_PTR_RET(editor);

    auto cd = GetSFTPClientData(editor);
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
    if(!tmpfile.Write(content, wxConvUTF8)) {
        return false;
    }
    return DoSyncSaveFile(tmpfile.GetFullPath(), remotePath, accountName, true);
}

bool clSFTPManager::DeleteConnection(const wxString& accountName, bool promptUser)
{
    auto iter = m_connections.find(accountName);
    if(iter == m_connections.end()) {
        // nothing to be done here
        return false;
    }

    // close all editors owned by this account
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
    for(auto editor : editors) {
        auto cd = GetSFTPClientData(editor);
        if(cd && (cd->GetAccountName() == accountName)) {
            if(!clGetManager()->CloseEditor(editor, promptUser)) {
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

SFTPAttribute::List_t clSFTPManager::List(const wxString& path, const SSHAccountInfo& accountInfo)
{
    wxBusyCursor bc;
    // save file async
    auto conn = GetConnectionPtrAddIfMissing(accountInfo.GetAccountName());
    if(!conn) {
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
        } catch(clException& e) {
            clERROR() << "List error." << e.What();
            promise.set_value(false);
        }
    };
    m_q.push_back(std::move(func));
    if(!future.get()) {
        return {};
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
        } catch(clException& e) {
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
        } catch(clException& e) {
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
        } catch(clException& e) {
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
        } catch(clException& e) {
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
        } catch(clException& e) {
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
    if(count == 0) {
        return;
    }

    // push SendKeepAlive() function for all the connections that we hold
    for(auto conn : all_connections) {
        auto func = [conn]() {
            try {
                conn->SendKeepAlive();
            } catch(clException& e) {
                clWARNING() << "failed to send keep-alive message for account:" << e.What() << endl;
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
        } catch(clException& e) {
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
        } catch(clException& e) {
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
    if(localFileName.empty()) {
        local_file = wxFileName(clStandardPaths::Get().GetTempDir(), path.AfterLast('/'));
        local_file.AppendDir("sftp-downloads");
    } else {
        local_file = wxFileName(localFileName);
    }
    local_file.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    if(DoSyncDownload(path, local_file.GetFullPath(), accountName)) {
        return local_file;
    }
    return {};
}

void clSFTPManager::AsyncWriteFile(const wxString& content, const wxString& remotePath, const wxString& accountName,
                                   wxEvtHandler* sink)
{
    clTempFile tmpfile;
    tmpfile.Persist(); // do not delete it on exit
    if(!tmpfile.Write(content, wxConvUTF8)) {
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
    if(local_path.StartsWith(download_folder, &rest)) {
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
    if(fn.IsOk()) {
        local_path = fn.GetFullPath();
        return true;
    }
    return false;
}

void clSFTPManager::StopWorkerThread()
{
    if(m_worker_thread) {
        m_shutdown.store(true);
        m_worker_thread->join();
        wxDELETE(m_worker_thread);
    }
    m_shutdown.store(false);
}

void clSFTPManager::StartWorkerThread()
{
    if(m_worker_thread) {
        return;
    }

    m_worker_thread = new std::thread(
        [](SyncQueue<std::function<void()>>& Q, atomic_bool& shutdown) {
            while(!shutdown.load()) {
                auto work_func = Q.pop_front();
                if(work_func == nullptr) {
                    continue;
                }
                work_func();
            }
        },
        std::ref(m_q), std::ref(m_shutdown));
}

void clSFTPManager::OnSaveCompleted(clCommandEvent& e)
{
    clGetManager()->SetStatusMessage(e.GetFileName() + _(" saved"), 3);
}

void clSFTPManager::OnSaveError(clCommandEvent& e)
{
    m_lastError.clear();
    m_lastError << "SaveError: " << e.GetString();
    clERROR() << m_lastError << endl;
}

bool clSFTPManager::IsRemoteFile(const wxString& filepath, wxString* account, wxString* remote_path) const
{
    if(m_downloadedFileToAccount.count(filepath) == 0) {
        return false;
    }

    *account = m_downloadedFileToAccount.find(filepath)->second.account_name;
    *remote_path = m_downloadedFileToAccount.find(filepath)->second.remote_path;
    return true;
}

size_t clSFTPManager::GetAllConnectionsPtr(std::vector<clSFTP::Ptr_t>& connections) const
{
    if(m_connections.empty()) {
        return 0;
    }
    connections.clear();
    connections.reserve(m_connections.size());
    for(const auto& vt : m_connections) {
        auto conn = GetConnectionPtr(vt.first);
        if(!conn) {
            continue;
        }
        connections.push_back(conn);
    }
    return connections.size();
}

#endif
