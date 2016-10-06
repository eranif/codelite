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
#include <libssh/sftp.h>
#include "cl_ssh.h"
#include "sftp.h"
#include "SFTPStatusPage.h"
#include <wx/ffile.h>

SFTPWorkerThread* SFTPWorkerThread::ms_instance = 0;

SFTPWorkerThread::SFTPWorkerThread()
    : m_sftp(NULL)
    , m_plugin(NULL)
{
}

SFTPWorkerThread::~SFTPWorkerThread() {}

SFTPWorkerThread* SFTPWorkerThread::Instance()
{
    if(ms_instance == 0) {
        ms_instance = new SFTPWorkerThread();
    }
    return ms_instance;
}

void SFTPWorkerThread::Release()
{
    if(ms_instance) {
        ms_instance->Stop();
        delete ms_instance;
    }
    ms_instance = 0;
}

void SFTPWorkerThread::ProcessRequest(ThreadRequest* request)
{
    SFTPThreadRequet* req = dynamic_cast<SFTPThreadRequet*>(request);
    // Check if we need to open an ssh connection
    wxString currentAccout = m_sftp ? m_sftp->GetAccount() : "";
    wxString requestAccount = req->GetAccount().GetAccountName();

    if(currentAccout.IsEmpty() || currentAccout != requestAccount) {
        m_sftp.reset(NULL);
        DoConnect(req);
    }

    if(req->GetDirection() == SFTPThreadRequet::kConnect) {
        // Nothing more to be done here
        // Disconnect
        m_sftp.reset(NULL);
        return;
    }

    wxString msg;
    wxString accountName = req->GetAccount().GetAccountName();
    if(m_sftp && m_sftp->IsConnected()) {

        try {

            msg.Clear();
            if(req->GetDirection() == SFTPThreadRequet::kUpload) {
                DoReportStatusBarMessage(wxString() << _("Uploading file: ") << req->GetRemoteFile());
                m_sftp->CreateRemoteFile(req->GetRemoteFile(), wxFileName(req->GetLocalFile()));
                msg << "Successfully uploaded file: " << req->GetLocalFile() << " -> " << req->GetRemoteFile();
                DoReportMessage(accountName, msg, SFTPThreadMessage::STATUS_OK);
                DoReportStatusBarMessage("");

            } else if(req->GetDirection() == SFTPThreadRequet::kDownload ||
                      req->GetDirection() == SFTPThreadRequet::kDownloadAndOpenContainingFolder ||
                      req->GetDirection() == SFTPThreadRequet::kDownloadAndOpenWithDefaultApp) {
                DoReportStatusBarMessage(wxString() << _("Downloading file: ") << req->GetRemoteFile());
                wxMemoryBuffer buffer;
                m_sftp->Read(req->GetRemoteFile(), buffer);
                wxFFile fp(req->GetLocalFile(), "w+b");
                if(fp.IsOpened()) {
                    fp.Write(buffer.GetData(), buffer.GetDataLen());
                    fp.Close();
                }

                msg << "Successfully downloaded file: " << req->GetLocalFile() << " <- " << req->GetRemoteFile();
                DoReportMessage(accountName, msg, SFTPThreadMessage::STATUS_OK);
                DoReportStatusBarMessage("");

                // We should also notify the parent window about download completed
                if(req->GetDirection() == SFTPThreadRequet::kDownload) {
                    m_plugin->CallAfter(&SFTP::FileDownloadedSuccessfully, req->GetLocalFile(), req->GetRemoteFile());

                } else if(req->GetDirection() == SFTPThreadRequet::kDownloadAndOpenContainingFolder) {
                    m_plugin->CallAfter(&SFTP::OpenContainingFolder, req->GetLocalFile());

                } else {
                    m_plugin->CallAfter(&SFTP::OpenWithDefaultApp, req->GetLocalFile());
                }
            }

        } catch(clException& e) {

            msg.Clear();
            msg << "SFTP error: " << e.What();
            DoReportMessage(accountName, msg, SFTPThreadMessage::STATUS_ERROR);
            DoReportStatusBarMessage(msg);
            m_sftp.reset(NULL);

            // Requeue our request
            if(req->GetRetryCounter() == 0) {
                msg.Clear();
                msg << "Retrying to upload file: " << req->GetRemoteFile();
                DoReportMessage(req->GetAccount().GetAccountName(), msg, SFTPThreadMessage::STATUS_NONE);

                // first time trying this request, requeue it
                SFTPThreadRequet* retryReq = static_cast<SFTPThreadRequet*>(req->Clone());
                retryReq->SetRetryCounter(1);
                Add(retryReq);
            }
        }
    }
}

void SFTPWorkerThread::DoConnect(SFTPThreadRequet* req)
{
    wxString accountName = req->GetAccount().GetAccountName();
    clSSH::Ptr_t ssh(new clSSH(req->GetAccount().GetHost(),
                               req->GetAccount().GetUsername(),
                               req->GetAccount().GetPassword(),
                               req->GetAccount().GetPort()));
    try {
        wxString message;
        DoReportStatusBarMessage(wxString() << _("Connecting to ") << accountName);
        DoReportMessage(accountName, "Connecting...", SFTPThreadMessage::STATUS_NONE);
        ssh->Connect();
        if(!ssh->AuthenticateServer(message)) {
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

    } catch(clException& e) {
        wxString msg;
        msg << "Connect error. " << e.What();
        DoReportMessage(accountName, msg, SFTPThreadMessage::STATUS_ERROR);
        m_sftp.reset(NULL);
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
// SFTPWriterThreadRequet
// -----------------------------------------

SFTPThreadRequet::SFTPThreadRequet(const SSHAccountInfo& accountInfo,
                                   const wxString& remoteFile,
                                   const wxString& localFile)
    : m_account(accountInfo)
    , m_remoteFile(remoteFile)
    , m_localFile(localFile)
    , m_retryCounter(0)
    , m_uploadSuccess(false)
    , m_direction(kUpload)
{
}

SFTPThreadRequet::SFTPThreadRequet(const RemoteFileInfo& remoteFile)
    : m_account(remoteFile.GetAccount())
    , m_remoteFile(remoteFile.GetRemoteFile())
    , m_localFile(remoteFile.GetLocalFile())
    , m_retryCounter(0)
    , m_uploadSuccess(false)
    , m_direction(kDownload)
{
}

SFTPThreadRequet::SFTPThreadRequet(const SSHAccountInfo& accountInfo)
    : m_account(accountInfo)
    , m_retryCounter(0)
    , m_uploadSuccess(false)
    , m_direction(kConnect)
{
}

SFTPThreadRequet::SFTPThreadRequet(const SFTPThreadRequet& other)
{
    if(this == &other) return;
    *this = other;
}

SFTPThreadRequet& SFTPThreadRequet::operator=(const SFTPThreadRequet& other)
{
    m_account = other.m_account;
    m_remoteFile = other.m_remoteFile;
    m_localFile = other.m_localFile;
    m_retryCounter = other.m_retryCounter;
    m_uploadSuccess = other.m_uploadSuccess;
    m_direction = other.m_direction;
    return *this;
}

SFTPThreadRequet::~SFTPThreadRequet() {}

ThreadRequest* SFTPThreadRequet::Clone() const { return new SFTPThreadRequet(*this); }

// -----------------------------------------
// SFTPThreadMessage
// -----------------------------------------

SFTPThreadMessage::SFTPThreadMessage()
    : m_status(STATUS_NONE)
{
}

SFTPThreadMessage::~SFTPThreadMessage() {}
