#include "sftp_writer_thread.h"
#include <libssh/sftp.h>
#include "cl_ssh.h"
#include "sftp.h"
#include "SFTPStatusPage.h"

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
    
    wxString msg;
    wxString accountName = req->GetAccount().GetAccountName();
    if ( m_sftp && m_sftp->IsConnected() ) {
        
        // try to write
        try {
            msg.Clear();
            m_sftp->Write(wxFileName(req->GetLocalFile()), req->GetRemoteFile());
            msg << "Successfully uploaded file: " << req->GetLocalFile() << " -> " << req->GetRemoteFile();
            DoReportMessage(accountName, msg, SFTPWriterThreadMessage::STATUS_OK);
            
        } catch (clException &e) {
            msg.Clear();
            msg << "Upload error: " << e.What();
            DoReportMessage(accountName, msg, SFTPWriterThreadMessage::STATUS_ERROR);
            m_sftp.reset(NULL);
            
            // Requeue our request
            if ( req->GetRetryCounter() == 0 ) {
                msg.Clear();
                msg << "Retrying to upload file: " << req->GetRemoteFile();
                DoReportMessage(req->GetAccount().GetAccountName(), msg, SFTPWriterThreadMessage::STATUS_NONE);
            
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
    wxString accountName = req->GetAccount().GetAccountName();
    clSSH::Ptr_t ssh( new clSSH(req->GetAccount().GetHost(), req->GetAccount().GetUsername(), req->GetAccount().GetPassword(), req->GetAccount().GetPort()) );
    try {
        wxString message;
        DoReportMessage(accountName, "Connecting...", SFTPWriterThreadMessage::STATUS_NONE);
        ssh->Connect();
        if ( !ssh->AuthenticateServer( message ) ) {
            ssh->AcceptServerAuthentication();
        }

        ssh->Login();
        m_sftp.reset( new clSFTP(ssh) );
        m_sftp->Initialize();

        wxString msg;
        msg << "Successfully connected to " << accountName;
        DoReportMessage(accountName, msg, SFTPWriterThreadMessage::STATUS_OK);

    } catch (clException &e) {
        wxString msg;
        msg << "Connect error. " << e.What();
        DoReportMessage(accountName, msg, SFTPWriterThreadMessage::STATUS_ERROR);
        m_sftp.reset(NULL);
    }
}

void SFTPWriterThread::DoReportMessage(const wxString& account, const wxString& message, int status)
{
    SFTPWriterThreadMessage *pMessage = new SFTPWriterThreadMessage();
    pMessage->SetStatus( status );
    pMessage->SetMessage( message );
    pMessage->SetAccount( account );
    GetNotifiedWindow()->CallAfter( &SFTPStatusPage::AddLine, pMessage );
}

// -----------------------------------------
// SFTPWriterThreadRequet
// -----------------------------------------

SFTPWriterThreadRequet::SFTPWriterThreadRequet(const SSHAccountInfo& accountInfo, const wxString& remoteFile, const wxString& localFile)
    : m_account(accountInfo)
    , m_remoteFile(remoteFile)
    , m_localFile(localFile)
    , m_retryCounter(0)
    , m_uploadSuccess(false)
{
}

SFTPWriterThreadRequet::SFTPWriterThreadRequet(const SFTPWriterThreadRequet& other)
{
    m_account       = other.m_account;
    m_remoteFile    = other.m_remoteFile;
    m_localFile     = other.m_localFile;
    m_retryCounter  = other.m_retryCounter;
    m_uploadSuccess = other.m_uploadSuccess;
}

SFTPWriterThreadRequet::~SFTPWriterThreadRequet()
{
}

ThreadRequest* SFTPWriterThreadRequet::Clone() const
{
    return new SFTPWriterThreadRequet(*this);
}

// -----------------------------------------
// SFTPWriterThreadMessage
// -----------------------------------------

SFTPWriterThreadMessage::SFTPWriterThreadMessage()
    : m_status(STATUS_NONE)
{
}

SFTPWriterThreadMessage::~SFTPWriterThreadMessage()
{
}
