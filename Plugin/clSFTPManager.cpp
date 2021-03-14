#if USE_SFTP
#include "SFTPClientData.hpp"
#include "clSFTPEvent.h"
#include "clSFTPManager.hpp"
#include "clTempFile.hpp"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "wx/debug.h"
#include "wx/thread.h"
#include <wx/msgdlg.h>
#include <wx/stc/stc.h>
#include <wx/utils.h>

clSFTPManager::clSFTPManager()
{
    assert(wxThread::IsMain());
    EventNotifier::Get()->Bind(wxEVT_GOING_DOWN, &clSFTPManager::OnGoingDown, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &clSFTPManager::OnFileSaved, this);

    m_timer = new wxTimer(this);
    m_timer->Start(10000); // 10 seconds should be good enough for a "keep-alive" interval
    Bind(wxEVT_TIMER, &clSFTPManager::OnTimer, this, m_timer->GetId());
}

clSFTPManager::~clSFTPManager()
{
    EventNotifier::Get()->Unbind(wxEVT_GOING_DOWN, &clSFTPManager::OnGoingDown, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &clSFTPManager::OnFileSaved, this);
    Unbind(wxEVT_TIMER, &clSFTPManager::OnTimer, this, m_timer->GetId());

    m_timer->Stop();
    wxDELETE(m_timer);
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
        wxMessageBox(wxString() << _("Failed to open SSH connection:\n") << e.What(), "CodeLite", wxOK | wxICON_ERROR);
        clERROR() << "SFTP Manager error:" << e.What() << clEndl;
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
        clWARNING() << "failed to locate account:" << accountName << endl;
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
    const wxString& filename = event.GetString();
    auto editor = clGetManager()->FindEditor(filename);
    CHECK_PTR_RET(editor);

    auto cd = GetSFTPClientData(editor);
    CHECK_PTR_RET(cd);

    auto conn_info = GetConnectionPair(cd->GetAccountName());
    CHECK_PTR_RET(conn_info.second);
    SaveFile(cd->GetLocalPath(), cd->GetRemotePath(), conn_info.first.GetAccountName());
}

bool clSFTPManager::SaveFile(const wxString& localPath, const wxString& remotePath, const wxString& accountName)
{
    assert(wxThread::IsMain());
    wxBusyCursor bc;
    auto conn = GetConnectionPtrAddIfMissing(accountName);
    CHECK_PTR_RET_FALSE(conn);

    wxString message;
    message << _("Uploading file: ") << localPath << " -> " << accountName << "@" << remotePath;
    clDEBUG() << message << endl;
    clGetManager()->SetStatusMessage(message, 3);
    try {
        conn->Write(localPath, remotePath);

    } catch(clException& e) {
        wxMessageBox(_("Failed to write file: ") + remotePath + _("\n") + e.What(), "CodeLite",
                     wxICON_ERROR | wxCENTER);
        return false;
    }
    return true;
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

SFTPAttribute::List_t clSFTPManager::List(const wxString& path, const SSHAccountInfo& accountInfo) const
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
        ::wxMessageBox(e.What(), "SFTP", wxOK | wxICON_ERROR | wxCENTER, EventNotifier::Get()->TopFrame());
        return {};
    }
    return attributes;
}

bool clSFTPManager::NewFile(const wxString& path, const SSHAccountInfo& accountInfo) const
{
    assert(wxThread::IsMain());
    auto conn = GetConnectionPtr(accountInfo.GetAccountName());
    if(!conn) {
        return false;
    }
    try {
        conn->CreateEmptyFile(path);
    } catch(clException& e) {
        ::wxMessageBox(wxString() << _("Failed to create file: ") << path << "\n"
                                  << e.What(),
                       "SFTP", wxOK | wxICON_ERROR | wxCENTER, EventNotifier::Get()->TopFrame());
        return false;
    }
    return true;
}

void clSFTPManager::OnGoingDown(clCommandEvent& event)
{
    event.Skip();
    Release();
}

bool clSFTPManager::NewFolder(const wxString& path, const SSHAccountInfo& accountInfo) const
{
    assert(wxThread::IsMain());
    auto conn = GetConnectionPtr(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    try {
        conn->CreateDir(path);
    } catch(clException& e) {
        ::wxMessageBox(e.What(), "SFTP", wxOK | wxICON_ERROR | wxCENTER, EventNotifier::Get()->TopFrame());
        return false;
    }
    return true;
}

bool clSFTPManager::Rename(const wxString& oldpath, const wxString& newpath, const SSHAccountInfo& accountInfo) const
{
    assert(wxThread::IsMain());
    auto conn = GetConnectionPtr(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    try {
        conn->Rename(oldpath, newpath);
    } catch(clException& e) {
        ::wxMessageBox(e.What(), "SFTP", wxOK | wxICON_ERROR | wxCENTER, EventNotifier::Get()->TopFrame());
        return false;
    }
    return true;
}

bool clSFTPManager::DeleteDir(const wxString& fullpath, const SSHAccountInfo& accountInfo) const
{
    assert(wxThread::IsMain());
    auto conn = GetConnectionPtr(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    try {
        conn->RemoveDir(fullpath);
    } catch(clException& e) {
        ::wxMessageBox(e.What(), "SFTP", wxOK | wxICON_ERROR | wxCENTER, EventNotifier::Get()->TopFrame());
        return false;
    }
    return true;
}

bool clSFTPManager::UnlinkFile(const wxString& fullpath, const SSHAccountInfo& accountInfo) const
{
    assert(wxThread::IsMain());
    auto conn = GetConnectionPtr(accountInfo.GetAccountName());
    CHECK_PTR_RET_FALSE(conn);

    try {
        conn->UnlinkFile(fullpath);
    } catch(clException& e) {
        ::wxMessageBox(e.What(), "SFTP", wxOK | wxICON_ERROR | wxCENTER, EventNotifier::Get()->TopFrame());
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

bool clSFTPManager::IsFileExists(const wxString& fullpath, const SSHAccountInfo& accountInfo) const
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

bool clSFTPManager::IsDirExists(const wxString& fullpath, const SSHAccountInfo& accountInfo) const
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

wxFileName clSFTPManager::Download(const wxString& path, const wxString& accountName)
{
    clDEBUG() << "SFTP Manager: Download() called for" << path << endl;
    wxFileName local_file(clStandardPaths::Get().GetTempDir(), path.AfterLast('/'));
    local_file.AppendDir("sftp-downloads");
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
            clWARNING() << "Could not locate account:" << accountName << endl;
            return false;
        }

        if(!AddConnection(account)) {
            clWARNING() << "Failed to open connection:" << accountName << endl;
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
        clWARNING() << "Failed to open file:" << remotePath << "." << e.What() << clEndl;
        return false;
    }

    wxLogNull noLog;
    wxFFile fp(localPath, "w+b");
    if(fp.IsOpened()) {
        fp.Write(buffer.GetData(), buffer.GetDataLen());
        fp.Close();
        return true;
    } else {
        clWARNING() << "Failed to write local file content:" << localPath << endl;
        return false;
    }
}

bool clSFTPManager::WriteFile(const wxString& content, const wxString& remotePath, const wxString& accountName)
{
    clTempFile tmpfile;
    if(!tmpfile.Write(content, wxConvUTF8)) {
        return false;
    }
    return SaveFile(tmpfile.GetFullPath(), remotePath, accountName);
}

#endif
