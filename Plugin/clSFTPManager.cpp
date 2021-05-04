#if USE_SFTP
#include "SFTPClientData.hpp"
#include "clSFTPEvent.h"
#include "clSFTPManager.hpp"
#include "clTempFile.hpp"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "wx/debug.h"
#include "wx/thread.h"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/stc/stc.h>
#include <wx/utils.h>

wxDEFINE_EVENT(wxEVT_SFTP_ASYNC_SAVE_COMPLETED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SFTP_ASYNC_SAVE_ERROR, clCommandEvent);

clSFTPManager::clSFTPManager()
{
    assert(wxThread::IsMain());
    EventNotifier::Get()->Bind(wxEVT_GOING_DOWN, &clSFTPManager::OnGoingDown, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &clSFTPManager::OnFileSaved, this);
    m_eventsConnected = true;
    m_shutdown.store(false);

    m_timer = new wxTimer(this);
    m_timer->Start(10000); // 10 seconds should be good enough for a "keep-alive" interval
    Bind(wxEVT_TIMER, &clSFTPManager::OnTimer, this, m_timer->GetId());
    Bind(wxEVT_SFTP_ASYNC_SAVE_COMPLETED, &clSFTPManager::OnSaveCompleted, this);
    Bind(wxEVT_SFTP_ASYNC_SAVE_ERROR, &clSFTPManager::OnSaveError, this);
}

clSFTPManager::~clSFTPManager()
{
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
    assert(wxThread::IsMain());
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
    if(m_saveThread) {
        m_shutdown.store(true);
        m_saveThread->join();
        wxDELETE(m_saveThread);
    }
}

bool clSFTPManager::AddConnection(const SSHAccountInfo& account, bool replace)
{
    assert(wxThread::IsMain());
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
        CaptureError("AddConnection", e);
        return false;
    }
    return true;
}

IEditor* clSFTPManager::OpenFile(const wxString& path, const SSHAccountInfo& accountInfo)
{
    assert(wxThread::IsMain());
    wxBusyCursor bc;
    bool res = AddConnection(accountInfo);
    if(!res) {
        return nullptr;
    }
    return OpenFile(path, accountInfo.GetAccountName());
}

IEditor* clSFTPManager::OpenFile(const wxString& path, const wxString& accountName)
{
    assert(wxThread::IsMain());
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
    if(!DoDownload(path, localPath.GetFullPath(), accountName)) {
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
    editor->SetClientData("sftp", cd);
    return editor;
}

std::pair<SSHAccountInfo, clSFTP::Ptr_t> clSFTPManager::GetConnectionPair(const wxString& account) const
{
    assert(wxThread::IsMain());
    auto iter = m_connections.find(account);
    if(iter == m_connections.end()) {
        return { {}, clSFTP::Ptr_t(nullptr) };
    }
    return iter->second;
}

clSFTP::Ptr_t clSFTPManager::GetConnectionPtr(const wxString& account) const
{
    assert(wxThread::IsMain());
    auto iter = m_connections.find(account);
    if(iter == m_connections.end()) {
        return clSFTP::Ptr_t(nullptr);
    }
    return iter->second.second;
}

clSFTP::Ptr_t clSFTPManager::GetConnectionPtrAddIfMissing(const wxString& account)
{
    assert(wxThread::IsMain());
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
    assert(wxThread::IsMain());
    auto clientData = editor->GetClientData("sftp");
    if(!clientData) {
        return nullptr;
    }
    return dynamic_cast<SFTPClientData*>(clientData);
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
    AsyncSaveFile(cd->GetLocalPath(), cd->GetRemotePath(), conn_info.first.GetAccountName(), nullptr);
}

bool clSFTPManager::DoSaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName,
                               bool delete_local, wxEvtHandler* sink, std::function<void()> notify_cb)
{
    assert(wxThread::IsMain());
    wxBusyCursor bc;
    auto conn = GetConnectionPtrAddIfMissing(accountName);
    CHECK_PTR_RET_FALSE(conn);

    wxString message;
    message << _("Uploading file: ") << localPath << " -> " << accountName << "@" << remotePath;
    clDEBUG() << message << endl;

    // start the thread if needed
    StartSaveThread();

    // Post a save request
    auto request = MakeSaveRequest(conn, localPath, remotePath, sink, delete_local, notify_cb);
    m_q.Post(request);
    return true;
}

bool clSFTPManager::AwaitSaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName)
{
    std::condition_variable cv;
    std::mutex m;
    bool write_is_completed = false;
    auto notify_cb = [&cv, &m, &write_is_completed]() {
        {
            std::unique_lock<std::mutex> lk{ m };
            write_is_completed = true;
        }
        cv.notify_all();
    };

    if(!DoSaveFile(localPath, remotePath, accountName, false, this, std::move(notify_cb))) {
        return false;
    }

    std::unique_lock<std::mutex> lk{ m };
    cv.wait(lk, [&write_is_completed] { return write_is_completed == true; });
    return true;
}

bool clSFTPManager::AwaitWriteFile(const wxString& content, const wxString& remotePath, const wxString& accountName)
{
    clTempFile tmpfile;
    tmpfile.Persist(); // do not delete it on exit
    if(!tmpfile.Write(content, wxConvUTF8)) {
        return false;
    }
    return AwaitSaveFile(tmpfile.GetFullPath(), remotePath, accountName);
}

bool clSFTPManager::AsyncSaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName,
                                  wxEvtHandler* sink)
{
    return DoSaveFile(localPath, remotePath, accountName, false, sink, nullptr);
}

bool clSFTPManager::DeleteConnection(const wxString& accountName, bool promptUser)
{
    assert(wxThread::IsMain());
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

    // notify that a session was closed
    clSFTPEvent event(wxEVT_SFTP_SESSION_CLOSED);
    event.SetAccount(accountName);
    EventNotifier::Get()->AddPendingEvent(event);

    // and finally remove the connection
    m_connections.erase(iter);
    return true;
}

SFTPAttribute::List_t clSFTPManager::List(const wxString& path, const SSHAccountInfo& accountInfo)
{
    assert(wxThread::IsMain());
    wxBusyCursor bc;
    auto conn = GetConnectionPtr(accountInfo.GetAccountName());
    if(!conn) {
        return {};
    }

    // get list of files and populate the tree
    SFTPAttribute::List_t attributes;
    try {
        attributes = conn->List(path, clSFTP::SFTP_BROWSE_FILES | clSFTP::SFTP_BROWSE_FOLDERS);

    } catch(clException& e) {
        CaptureError("List", e);
        return {};
    }
    return attributes;
}

bool clSFTPManager::NewFile(const wxString& path, const SSHAccountInfo& accountInfo)
{
    assert(wxThread::IsMain());
    auto conn = GetConnectionPtr(accountInfo.GetAccountName());
    if(!conn) {
        return false;
    }
    try {
        conn->CreateEmptyFile(path);
    } catch(clException& e) {
        CaptureError("NewFile", e);
        return false;
    }
    return true;
}

void clSFTPManager::OnGoingDown(clCommandEvent& event)
{
    event.Skip();
    clDEBUG() << "SFTP manager is shutting down..." << endl;
    Release();
}

bool clSFTPManager::NewFolder(const wxString& path, const SSHAccountInfo& accountInfo)
{
    assert(wxThread::IsMain());
    auto conn = GetConnectionPtr(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    try {
        conn->CreateDir(path);
    } catch(clException& e) {
        CaptureError("NewFolder", e);
        return false;
    }
    return true;
}

bool clSFTPManager::Rename(const wxString& oldpath, const wxString& newpath, const SSHAccountInfo& accountInfo)
{
    assert(wxThread::IsMain());
    auto conn = GetConnectionPtr(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    try {
        conn->Rename(oldpath, newpath);
    } catch(clException& e) {
        CaptureError("Rename", e);
        return false;
    }
    return true;
}

bool clSFTPManager::DeleteDir(const wxString& fullpath, const SSHAccountInfo& accountInfo)
{
    assert(wxThread::IsMain());
    auto conn = GetConnectionPtr(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    try {
        conn->RemoveDir(fullpath);
    } catch(clException& e) {
        CaptureError("DeleteDir", e);
        return false;
    }
    return true;
}

bool clSFTPManager::UnlinkFile(const wxString& fullpath, const SSHAccountInfo& accountInfo)
{
    assert(wxThread::IsMain());
    auto conn = GetConnectionPtr(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    try {
        conn->UnlinkFile(fullpath);
    } catch(clException& e) {
        CaptureError("UnlinkFile", e);
        return false;
    }
    return true;
}

void clSFTPManager::OnTimer(wxTimerEvent& event)
{
    assert(wxThread::IsMain());
    event.Skip();
    if(m_connections.empty()) {
        return;
    }

    for(const auto& vt : m_connections) {
        try {
            vt.second.second->SendKeepAlive();
        } catch(clException& e) {
            clWARNING() << "failed to send keep-alive message for account:" << vt.first << "." << e.What() << endl;
        }
    }
}

bool clSFTPManager::IsFileExists(const wxString& fullpath, const SSHAccountInfo& accountInfo)
{
    assert(wxThread::IsMain());
    auto conn = GetConnectionPtr(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    try {
        auto attr = conn->Stat(fullpath);
        return attr->IsFile();
    } catch(clException& e) {
        wxUnusedVar(e);
        return false;
    }
}

bool clSFTPManager::IsDirExists(const wxString& fullpath, const SSHAccountInfo& accountInfo)
{
    assert(wxThread::IsMain());
    auto conn = GetConnectionPtr(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);
    try {
        auto attr = conn->Stat(fullpath);
        return attr->IsFolder();
    } catch(clException& e) {
        wxUnusedVar(e);
        return false;
    }
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
    if(DoDownload(path, local_file.GetFullPath(), accountName)) {
        return local_file;
    }
    return {};
}

bool clSFTPManager::DoDownload(const wxString& remotePath, const wxString& localPath, const wxString& accountName)
{
    // Open it
    clDEBUG() << "SFTP Manager: downloading file" << remotePath << "->" << localPath << "for account:" << accountName
              << endl;
    auto connection_info = GetConnectionPair(accountName);
    if(!connection_info.second) {
        // No such connection, attempt to load the connection details and open a session
        auto account = SSHAccountInfo::LoadAccount(accountName);
        if(accountName.empty()) {
            m_lastError.clear();
            m_lastError << "Could not locate account:" << accountName;
            return false;
        }

        if(!AddConnection(account)) {
            m_lastError.clear();
            m_lastError << "Failed to open connection:" << accountName;
            return false;
        }

        connection_info = GetConnectionPair(accountName);
        if(!connection_info.second) {
            return false;
        }
    }

    // build the local file path
    auto sftp = connection_info.second;
    auto info = connection_info.first;
    wxMemoryBuffer buffer;
    SFTPAttribute::Ptr_t fileAttr;
    try {
        fileAttr = sftp->Read(remotePath, buffer);
    } catch(clException& e) {
        m_lastError.clear();
        m_lastError << "Failed to open file:" << remotePath << "." << e.What();
        return false;
    }

    wxLogNull noLog;
    wxFFile fp(localPath, "w+b");
    if(fp.IsOpened()) {
        fp.Write(buffer.GetData(), buffer.GetDataLen());
        fp.Close();
        return true;
    } else {
        m_lastError.clear();
        m_lastError << "Failed to write local file content:" << localPath;
        return false;
    }
}

bool clSFTPManager::AsyncWriteFile(const wxString& content, const wxString& remotePath, const wxString& accountName,
                                   wxEvtHandler* sink)
{
    clTempFile tmpfile;
    tmpfile.Persist(); // do not delete it on exit
    if(!tmpfile.Write(content, wxConvUTF8)) {
        return false;
    }
    return DoSaveFile(tmpfile.GetFullPath(), remotePath, accountName, true, sink, nullptr);
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

void clSFTPManager::StartSaveThread()
{
    if(m_saveThread) {
        return;
    }

    m_saveThread = new std::thread(
        [](wxMessageQueue<save_request*>& Q, atomic_bool& shutdown) {
            while(!shutdown.load()) {
                save_request* req = nullptr;
                if(Q.ReceiveTimeout(10, req) == wxMSGQUEUE_NO_ERROR) {
                    try {
                        req->conn->Write(req->local_path, req->remote_path);

                        // notify about save success
                        clCommandEvent success_event(wxEVT_SFTP_ASYNC_SAVE_COMPLETED);
                        success_event.SetFileName(req->remote_path);
                        req->sink->AddPendingEvent(success_event);
                    } catch(clException& e) {
                        clERROR() << e.What() << endl;
                        // notify about save error
                        wxString errmsg;
                        errmsg << _("Failed to save file: ") << req->remote_path << "\n";
                        errmsg << e.What();
                        clCommandEvent err_event(wxEVT_SFTP_ASYNC_SAVE_ERROR);
                        err_event.SetString(errmsg);
                        err_event.SetFileName(req->remote_path);
                        req->sink->AddPendingEvent(err_event);
                    }
                    if(req->delete_local_file) {
                        clRemoveFile(req->local_path);
                    }
                    if(req->notify_cb) {
                        req->notify_cb();
                    }
                    wxDELETE(req);
                }
            }
        },
        std::ref(m_q), std::ref(m_shutdown));
}

clSFTPManager::save_request* clSFTPManager::MakeSaveRequest(clSFTP::Ptr_t conn, const wxString& local_path,
                                                            const wxString& remote_path, wxEvtHandler* sink,
                                                            bool delete_local, std::function<void()> notify_cb)
{
    save_request* req = new save_request();
    req->conn = conn;
    req->local_path = local_path;
    req->remote_path = remote_path;
    req->sink = sink == nullptr ? this : sink; // always assign a sink object
    if(notify_cb) {
        req->notify_cb = std::move(notify_cb);
    }
    req->delete_local_file = delete_local;
    return req;
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

void clSFTPManager::CaptureError(const wxString& context, const clException& e)
{
    m_lastError.clear();
    m_lastError << context << "." << e.What();
    // clERROR() << m_lastError << endl;
}

#endif
