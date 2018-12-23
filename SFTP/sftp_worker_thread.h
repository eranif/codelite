//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : sftp_writer_thread.h
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

#ifndef SFTPWRITERTHREAD_H
#define SFTPWRITERTHREAD_H

#include "cl_sftp.h"
#include "remote_file_info.h"
#include "ssh_account_info.h"
#include "worker_thread.h" // Base class: WorkerThread

class SFTP;

enum class eSFTPActions {
    kUpload,
    kDownload,
    kConnect,
    kDownloadAndOpenWithDefaultApp,
    kDownloadAndOpenContainingFolder,
    kRename,
    kDelete,
};

class SFTPThreadRequet : public ThreadRequest
{
    SSHAccountInfo m_account;
    wxString m_remoteFile;
    wxString m_localFile;
    size_t m_retryCounter = 0;
    bool m_uploadSuccess = false;
    eSFTPActions m_action;
    size_t m_permissions = 0;
    wxString m_newRemoteFile;
    int m_lineNumber = wxNOT_FOUND;

public:
    SFTPThreadRequet(const SSHAccountInfo& accountInfo, const wxString& remoteFile, const wxString& localFile,
                     size_t persmissions);
    SFTPThreadRequet(const SSHAccountInfo& accountInfo, const wxString& oldName, const wxString& newName);
    SFTPThreadRequet(const SSHAccountInfo& accountInfo, const wxString& fileToDelete);
    SFTPThreadRequet(const RemoteFileInfo& remoteFile);
    SFTPThreadRequet(const SSHAccountInfo& accountInfo);
    SFTPThreadRequet(const SFTPThreadRequet& other);
    SFTPThreadRequet& operator=(const SFTPThreadRequet& other);
    virtual ~SFTPThreadRequet();

    void SetUploadSuccess(bool uploadSuccess) { this->m_uploadSuccess = uploadSuccess; }
    bool IsUploadSuccess() const { return m_uploadSuccess; }
    void SetAccount(const SSHAccountInfo& account) { this->m_account = account; }
    void SetLocalFile(const wxString& localFile) { this->m_localFile = localFile; }
    void SetRemoteFile(const wxString& remoteFile) { this->m_remoteFile = remoteFile; }
    void SetRetryCounter(size_t retryCounter) { this->m_retryCounter = retryCounter; }
    size_t GetRetryCounter() const { return m_retryCounter; }
    const SSHAccountInfo& GetAccount() const { return m_account; }
    const wxString& GetRemoteFile() const { return m_remoteFile; }
    const wxString& GetLocalFile() const { return m_localFile; }
    eSFTPActions GetAction() const { return m_action; }
    void SetAction(eSFTPActions d) { m_action = d; }
    void SetPermissions(size_t permissions) { this->m_permissions = permissions; }
    size_t GetPermissions() const { return m_permissions; }
    ThreadRequest* Clone() const;
    void SetNewRemoteFile(const wxString& newRemoteFile) { this->m_newRemoteFile = newRemoteFile; }
    const wxString& GetNewRemoteFile() const { return m_newRemoteFile; }
    void SetLineNumber(int lineNumber) { this->m_lineNumber = lineNumber; }
    int GetLineNumber() const { return m_lineNumber; }
};

class SFTPThreadMessage
{

    int m_status;
    wxString m_message;
    wxString m_account;

public:
    enum {
        STATUS_NONE = -1,
        STATUS_OK,
        STATUS_ERROR,
    };

public:
    SFTPThreadMessage();
    virtual ~SFTPThreadMessage();

    void SetAccount(const wxString& account) { this->m_account = account; }
    void SetMessage(const wxString& message) { this->m_message = message; }
    const wxString& GetAccount() const { return m_account; }
    const wxString& GetMessage() const { return m_message; }
    void SetStatus(int status) { this->m_status = status; }
    int GetStatus() const { return m_status; }
};

class SFTPWorkerThread : public WorkerThread
{
    static SFTPWorkerThread* ms_instance;
    clSFTP::Ptr_t m_sftp;
    SFTP* m_plugin;

public:
    static SFTPWorkerThread* Instance();
    static void Release();

private:
    SFTPWorkerThread();
    virtual ~SFTPWorkerThread();
    void DoConnect(SFTPThreadRequet* req);
    void DoReportMessage(const wxString& account, const wxString& message, int status);
    void DoReportStatusBarMessage(const wxString& message);

public:
    virtual void ProcessRequest(ThreadRequest* request);
    void SetSftpPlugin(SFTP* sftp);
};

#endif // SFTPWRITERTHREAD_H
