#include "sftp_writer_thread.h"
#include <libssh/sftp.h>
#include "cl_ssh.h"
#include "sftp.h"

SFTPWriterThread* SFTPWriterThread::ms_instance = 0;

SFTPWriterThread::SFTPWriterThread()
    : m_sftp(NULL)
{
}

SFTPWriterThread::~SFTPWriterThread()
{
}

SFTPWriterThread* SFTPWriterThread::Instance()
{
    if (ms_instance == 0) {
        ms_instance = new SFTPWriterThread();
    }
    return ms_instance;
}

void SFTPWriterThread::Release()
{
    if (ms_instance) {
        ms_instance->Stop();
        delete ms_instance;
    }
    ms_instance = 0;
}

void SFTPWriterThread::ProcessRequest(ThreadRequest* request)
{
    SFTPWriterThreadRequet* req = dynamic_cast<SFTPWriterThreadRequet*>(request);

    // Check if we need to open an ssh connection
    if ( !m_sftp || m_sftp->GetAccountName() != req->GetAccount().GetAccountName() ) {
        m_sftp.reset(NULL);
        DoConnect( req );
    }

    if ( m_sftp && m_sftp->IsConnected() ) {
        // try to write
        try {
            wxString msg;
            msg << "SFTP: Successfully uploaded file: [" << req->GetAccount().GetAccountName() << "] " << req->GetLocalFile() << " -> " << req->GetRemoteFile();
            m_sftp->Write(wxFileName(req->GetLocalFile()), req->GetRemoteFile());
            GetNotifiedWindow()->CallAfter( &SFTP::OnFileWriteOK, msg);
            
        } catch (clException &e) {
            wxString msg;
            msg << "SFTP: failed to upload file: [" << req->GetAccount().GetAccountName() << "] " << req->GetRemoteFile() << ". " << e.What();
            GetNotifiedWindow()->CallAfter( &SFTP::OnFileWriteError, msg);
            m_sftp.reset(NULL);
            
            // Requeue our request
            if ( req->GetRetryCounter() == 0 ) {
                // first time trying this request, requeue it
                SFTPWriterThreadRequet* retryReq = static_cast<SFTPWriterThreadRequet*>(req->Clone());
                retryReq->SetRetryCounter(1);
                Add( retryReq );
            }
        }
    }
}

void SFTPWriterThread::DoConnect(SFTPWriterThreadRequet* req)
{
    clSSH::Ptr_t ssh( new clSSH(req->GetAccount().GetHost(), req->GetAccount().GetUsername(), req->GetAccount().GetPassword(), req->GetAccount().GetPort()) );
    try {
        wxString message;
        ssh->Connect();
        if ( !ssh->AuthenticateServer( message ) ) {
            ssh->AcceptServerAuthentication();
        }

        ssh->Login();
        m_sftp.reset( new clSFTP(ssh) );
        m_sftp->Initialize();

        wxString msg;
        msg << "SFTP: Successfully connected to " << req->GetAccount().GetAccountName();
        GetNotifiedWindow()->CallAfter( &SFTP::OnFileWriteOK, msg);

    } catch (clException &e) {
        wxString msg;
        msg << "SFTP: Connect error. [" << req->GetAccount().GetAccountName() << "] : " << e.What();
        GetNotifiedWindow()->CallAfter( &SFTP::OnFileWriteError, msg);
        m_sftp.reset(NULL);
    }
}

SFTPWriterThreadRequet::SFTPWriterThreadRequet(const SSHAccountInfo& accountInfo, const wxString& remoteFile, const wxString& localFile)
    : m_account(accountInfo)
    , m_remoteFile(remoteFile)
    , m_localFile(localFile)
    , m_retryCounter(0)
{
}

SFTPWriterThreadRequet::SFTPWriterThreadRequet(const SFTPWriterThreadRequet& other)
{
    m_account      = other.m_account;
    m_remoteFile   = other.m_remoteFile;
    m_localFile    = other.m_localFile;
    m_retryCounter = other.m_retryCounter;
}

SFTPWriterThreadRequet::~SFTPWriterThreadRequet()
{
}

ThreadRequest* SFTPWriterThreadRequet::Clone() const
{
    return new SFTPWriterThreadRequet(*this);
}
