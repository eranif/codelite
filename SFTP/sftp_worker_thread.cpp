//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : sftp_worker_thread.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "sftp_worker_thread.h"

#include "SFTPStatusPage.h"
#include "environmentconfig.h"
#include "sftp.h"
#include "ssh/cl_ssh.h"

#include <libssh/sftp.h>
#include <wx/ffile.h>

SFTPWorkerThread* SFTPWorkerThread::ms_instance = 0;

SFTPWorkerThread::SFTPWorkerThread()
    : m_sftp(NULL)
    , m_plugin(NULL)
{
}

SFTPWorkerThread* SFTPWorkerThread::Instance()
{
    if (ms_instance == 0) {
        ms_instance = new SFTPWorkerThread();
    }
    return ms_instance;
}

void SFTPWorkerThread::Release()
{
    if (ms_instance) {
        ms_instance->Stop();
        delete ms_instance;
    }
    ms_instance = 0;
}

void SFTPWorkerThread::ProcessRequest(ThreadRequest* request)
{
    SFTPThreadRequest* req = dynamic_cast<SFTPThreadRequest*>(request);
    // Check if we need to open an ssh connection
    wxString currentAccount = m_sftp ? m_sftp->GetAccount() : "";
    wxString requestAccount = req->GetAccount().GetAccountName();

    if (currentAccount.IsEmpty() || currentAccount != requestAccount) {
        m_sftp.reset();
        DoConnect(req);
    }

    if (req->GetAction() == eSFTPActions::kConnect) {
        // Nothing more to be done here
        // Disconnect
        m_sftp.reset();
        return;
    }

    wxString msg;
    wxString accountName = req->GetAccount().GetAccountName();
    if (m_sftp && m_sftp->IsConnected()) {
        msg.Clear();
        try {
            switch (req->GetAction()) {
            case eSFTPActions::kConnect:
                // We don't really need this case. Just make the compiler silence
                return;
            case eSFTPActions::kUpload: {
                DoReportStatusBarMessage(wxString() << _("Uploading file: ") << req->GetRemoteFile());
                m_sftp->CreateRemoteFile(req->GetRemoteFile(), wxFileName(req->GetLocalFile()));
                msg << "Successfully uploaded file: " << req->GetLocalFile() << " -> " << req->GetRemoteFile();
                DoReportMessage(accountName, msg, SFTPThreadMessage::STATUS_OK);
                DoReportStatusBarMessage("");
                break;
            }
            case eSFTPActions::kDownload:
            case eSFTPActions::kDownloadAndOpenContainingFolder:
            case eSFTPActions::kDownloadAndOpenWithDefaultApp: {
                DoReportStatusBarMessage(wxString() << _("Downloading file: ") << req->GetRemoteFile());
                wxMemoryBuffer buffer;
                SFTPAttribute::Ptr_t fileAttr = m_sftp->Read(req->GetRemoteFile(), buffer);
                wxFFile fp(req->GetLocalFile(), "w+b");
                if (fp.IsOpened()) {
                    fp.Write(buffer.GetData(), buffer.GetDataLen());
                    fp.Close();
                }

                msg << "Successfully downloaded file: " << req->GetLocalFile() << " <- " << req->GetRemoteFile();
                DoReportMessage(accountName, msg, SFTPThreadMessage::STATUS_OK);
                DoReportStatusBarMessage("");

                // We should also notify the parent window about download completed
                if (req->GetAction() == eSFTPActions::kDownload) {
                    SFTPClientData cd;
                    cd.SetLocalPath(req->GetLocalFile());
                    cd.SetRemotePath(req->GetRemoteFile());
                    cd.SetPermissions(fileAttr ? fileAttr->GetPermissions() : 0);
                    cd.SetLineNumber(req->GetLineNumber());
                    cd.SetAccountName(req->GetAccount().GetAccountName());
                    m_plugin->CallAfter(&SFTP::FileDownloadedSuccessfully, cd);

                } else if (req->GetAction() == eSFTPActions::kDownloadAndOpenContainingFolder) {
                    m_plugin->CallAfter(&SFTP::OpenContainingFolder, req->GetLocalFile());

                } else {
                    m_plugin->CallAfter(&SFTP::OpenWithDefaultApp, req->GetLocalFile());
                }
                break;
            }
            case eSFTPActions::kRename: {
                DoReportStatusBarMessage(wxString() << _("Renaming: ") << req->GetRemoteFile() << " -> "
                                                    << req->GetNewRemoteFile());
                m_sftp->Rename(req->GetRemoteFile(), req->GetNewRemoteFile());
                wxString msg;
                msg << _("Renamed ") << req->GetRemoteFile() << " -> " << req->GetNewRemoteFile();
                DoReportMessage(accountName, msg, SFTPThreadMessage::STATUS_OK);
                break;
            }
            case eSFTPActions::kDelete: {
                DoReportStatusBarMessage(wxString() << _("Deleting: ") << req->GetRemoteFile());
                m_sftp->UnlinkFile(req->GetRemoteFile());
                wxString msg;
                msg << _("Deleted ") << req->GetRemoteFile();
                DoReportMessage(accountName, msg, SFTPThreadMessage::STATUS_OK);
                break;
            }
            }
        } catch (const clException& e) {

            msg.Clear();
            msg << "SFTP error: " << e.What();
            DoReportMessage(accountName, msg, SFTPThreadMessage::STATUS_ERROR);
            DoReportStatusBarMessage(msg);
            m_sftp.reset();

            // Requeue our request
            if (req->GetRetryCounter() == 0) {
                msg.Clear();
                msg << "Retrying to upload file: " << req->GetRemoteFile();
                DoReportMessage(req->GetAccount().GetAccountName(), msg, SFTPThreadMessage::STATUS_NONE);

                // first time trying this request, requeue it
                SFTPThreadRequest* retryReq = static_cast<SFTPThreadRequest*>(req->Clone());
                retryReq->SetRetryCounter(1);
                Add(retryReq);
            }
        }
    }
}

void SFTPWorkerThread::DoConnect(SFTPThreadRequest* req)
{
    wxString accountName = req->GetAccount().GetAccountName();
    clSSH::Ptr_t ssh(new clSSH(req->GetAccount().GetHost(), req->GetAccount().GetUsername(),
                               req->GetAccount().GetPassword(), req->GetAccount().GetKeyFile(),
                               req->GetAccount().GetPort()));
    try {
        wxString message;
        DoReportStatusBarMessage(wxString() << _("Connecting to ") << accountName);
        DoReportMessage(accountName, "Connecting...", SFTPThreadMessage::STATUS_NONE);
        EnvSetter env;
        ssh->Open();
        if (!ssh->AuthenticateServer(message)) {
            ssh->AcceptServerAuthentication();
        }

        ssh->Login();
        m_sftp.reset(new clSFTP(ssh));

        // associate the account with the connection
        m_sftp->SetAccount(req->GetAccount().GetAccountName());
        m_sftp->Initialize();

        wxString msg;
        msg << "Successfully connected to " << accountName;
        DoReportMessage(accountName, msg, SFTPThreadMessage::STATUS_OK);

    } catch (const clException& e) {
        wxString msg;
        msg << "Connect error. " << e.What();
        DoReportMessage(accountName, msg, SFTPThreadMessage::STATUS_ERROR);
        m_sftp.reset();
    }
}

void SFTPWorkerThread::DoReportMessage(const wxString& account, const wxString& message, int status)
{
    SFTPThreadMessage* pMessage = new SFTPThreadMessage();
    pMessage->SetStatus(status);
    pMessage->SetMessage(message);
    pMessage->SetAccount(account);
    GetNotifiedWindow()->CallAfter(&SFTPStatusPage::AddLine, pMessage);
}

void SFTPWorkerThread::SetSftpPlugin(SFTP* sftp) { m_plugin = sftp; }

void SFTPWorkerThread::DoReportStatusBarMessage(const wxString& message)
{
    GetNotifiedWindow()->CallAfter(&SFTPStatusPage::SetStatusBarMessage, message);
}

// -----------------------------------------
// SFTPWriterThreadRequest
// -----------------------------------------

SFTPThreadRequest::SFTPThreadRequest(const SSHAccountInfo& accountInfo,
                                     const wxString& remoteFile,
                                     const wxString& localFile,
                                     size_t permissions)
    : m_account(accountInfo)
    , m_remoteFile(remoteFile)
    , m_localFile(localFile)
    , m_action(eSFTPActions::kUpload)
    , m_permissions(permissions)
{
}

SFTPThreadRequest::SFTPThreadRequest(const RemoteFileInfo& remoteFile)
    : m_account(remoteFile.GetAccount())
    , m_remoteFile(remoteFile.GetRemoteFile())
    , m_localFile(remoteFile.GetLocalFile())
    , m_action(eSFTPActions::kDownload)
    , m_lineNumber(remoteFile.GetLineNumber())
{
}

SFTPThreadRequest::SFTPThreadRequest(const SSHAccountInfo& accountInfo)
    : m_account(accountInfo)
    , m_action(eSFTPActions::kConnect)
{
}

SFTPThreadRequest::SFTPThreadRequest(const SSHAccountInfo& accountInfo,
                                     const wxString& oldName,
                                     const wxString& newName)
    : m_account(accountInfo)
    , m_remoteFile(oldName)
    , m_action(eSFTPActions::kRename)
    , m_newRemoteFile(newName)
{
}

SFTPThreadRequest::SFTPThreadRequest(const SSHAccountInfo& accountInfo, const wxString& fileToDelete)
    : m_account(accountInfo)
    , m_remoteFile(fileToDelete)
    , m_action(eSFTPActions::kDelete)
{
}

ThreadRequest* SFTPThreadRequest::Clone() const { return new SFTPThreadRequest(*this); }

// -----------------------------------------
// SFTPThreadMessage
// -----------------------------------------

SFTPThreadMessage::SFTPThreadMessage()
    : m_status(STATUS_NONE)
{
}
