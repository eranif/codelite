#if USE_SFTP
#include "SFTPClientData.hpp"
#include "clSFTPManager.hpp"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include <wx/msgdlg.h>
#include <wx/stc/stc.h>
#include <wx/utils.h>

clSFTPManager::clSFTPManager()
{
    EventNotifier::Get()->Bind(wxEVT_DEBUG_ENDED, &clSFTPManager::OnDebugEnded, this);
    EventNotifier::Get()->Bind(wxEVT_DEBUG_STARTED, &clSFTPManager::OnDebugStarted, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &clSFTPManager::OnFileSaved, this);
}

clSFTPManager::~clSFTPManager()
{
    EventNotifier::Get()->Unbind(wxEVT_DEBUG_ENDED, &clSFTPManager::OnDebugEnded, this);
    EventNotifier::Get()->Unbind(wxEVT_DEBUG_STARTED, &clSFTPManager::OnDebugStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &clSFTPManager::OnFileSaved, this);
}

clSFTPManager& clSFTPManager::Get()
{
    static clSFTPManager manager;
    return manager;
}

void clSFTPManager::Release()
{
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
    for(auto editor : editors) {
        auto cd = GetSFTPClientData(editor);
        if(cd && m_connections.count(cd->GetAccountName())) {
            clGetManager()->CloseEditor(editor, false);
        }
    }
    m_connections.clear();
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
    } catch(clException& e) {
        wxMessageBox(wxString() << _("Failed to open SSH connection:\n") << e.What(), "CodeLite", wxOK | wxICON_ERROR);
        clERROR() << "SFTP Manager error:" << e.What() << clEndl;
        return false;
    }
    return true;
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

    // Open it
    auto connection_info = GetConnection(accountName);
    if(!connection_info.second) {
        return nullptr;
    }

    // build the local file path
    auto sftp = connection_info.second;
    auto info = connection_info.first;
    wxFileName localPath = clSFTP::GetLocalFileName(info, path, true);
    wxMemoryBuffer buffer;
    SFTPAttribute::Ptr_t fileAttr;
    try {
        fileAttr = sftp->Read(path, buffer);
    } catch(clException& e) {
        clWARNING() << "Failed to open file:" << path << "." << e.What() << clEndl;
        return nullptr;
    }

    wxFFile fp(localPath.GetFullPath(), "w+b");
    if(fp.IsOpened()) {
        fp.Write(buffer.GetData(), buffer.GetDataLen());
        fp.Close();
    }

    // set the client data for this editor
    SFTPClientData* cd = new SFTPClientData;
    cd->SetLocalPath(localPath.GetFullPath());
    cd->SetRemotePath(path);
    cd->SetPermissions(fileAttr ? fileAttr->GetPermissions() : 0);
    cd->SetLineNumber(wxNOT_FOUND);
    cd->SetAccountName(accountName);

    wxString tooltip;
    tooltip << "Local: " << cd->GetLocalPath() << "\n"
            << "Remote: " << cd->GetRemotePath();

    wxBitmap bmp = clGetManager()->GetStdIcons()->LoadBitmap("download");
    auto editor = clGetManager()->OpenFile(localPath.GetFullPath(), bmp, tooltip);
    editor->SetClientData("sftp", cd);
    return editor;
}

std::pair<SSHAccountInfo, clSFTP::Ptr_t> clSFTPManager::GetConnection(const wxString& account) const
{
    auto iter = m_connections.find(account);
    if(iter == m_connections.end()) {
        return { {}, clSFTP::Ptr_t(nullptr) };
    }
    return iter->second;
}

void clSFTPManager::OnDebugEnded(clDebugEvent& event)
{
    // free all the connections
    event.Skip();
    Release();
}

void clSFTPManager::OnDebugStarted(clDebugEvent& event)
{
    // free all the connections
    event.Skip();
    Release();
}

SFTPClientData* clSFTPManager::GetSFTPClientData(IEditor* editor)
{
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

    auto conn_info = GetConnection(cd->GetAccountName());
    CHECK_PTR_RET(conn_info.second);

    auto conn = conn_info.second;
    auto account = conn_info.first;

    wxString message;
    message << _("Uploading file: ") << cd->GetLocalPath() << " -> " << cd->GetAccountName() << "@"
            << cd->GetRemotePath();
    clGetManager()->SetStatusMessage(message, 3);
    try {
        conn->Write(cd->GetLocalPath(), cd->GetRemotePath());

    } catch(clException& e) {
        wxMessageBox(_("Failed to write file: ") + cd->GetRemotePath() + _("\n") + e.What(), "CodeLite",
                     wxICON_ERROR | wxCENTER);
    }
}
#endif
