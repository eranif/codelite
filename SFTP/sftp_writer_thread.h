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

#include "worker_thread.h" // Base class: WorkerThread
#include "cl_sftp.h"
#include "ssh_account_info.h"

class SFTPWriterThreadRequet : public ThreadRequest
{
    SSHAccountInfo m_account;
    wxString       m_remoteFile;
    wxString       m_localFile;
    size_t         m_retryCounter;

public:
    SFTPWriterThreadRequet(const SSHAccountInfo& accountInfo, const wxString &remoteFile, const wxString &localFile);
    SFTPWriterThreadRequet(const SFTPWriterThreadRequet& other);
    virtual ~SFTPWriterThreadRequet();

    void SetAccount(const SSHAccountInfo& account) {
        this->m_account = account;
    }
    void SetLocalFile(const wxString& localFile) {
        this->m_localFile = localFile;
    }
    void SetRemoteFile(const wxString& remoteFile) {
        this->m_remoteFile = remoteFile;
    }
    void SetRetryCounter(size_t retryCounter) {
        this->m_retryCounter = retryCounter;
    }
    size_t GetRetryCounter() const {
        return m_retryCounter;
    }
    const SSHAccountInfo& GetAccount() const {
        return m_account;
    }
    const wxString& GetRemoteFile() const {
        return m_remoteFile;
    }
    const wxString& GetLocalFile() const {
        return m_localFile;
    }

    ThreadRequest* Clone() const;
};

class SFTPWriterThread : public WorkerThread
{
    static SFTPWriterThread* ms_instance;
    clSFTP::Ptr_t m_sftp;

public:
    static SFTPWriterThread* Instance();
    static void Release();

private:
    SFTPWriterThread();
    virtual ~SFTPWriterThread();
    void DoConnect(SFTPWriterThreadRequet *req);

public:
    virtual void ProcessRequest(ThreadRequest* request);
};

#endif // SFTPWRITERTHREAD_H
