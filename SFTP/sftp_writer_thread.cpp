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
    
    for(size_t i=0; i<2; ++i) {
        if ( m_sftp && m_sftp->IsConnected() ) {
            
            // try to write
            try {
                wxString msg;
                msg << "SFTP: Successfully uploaded file: " << req->GetAccount().GetAccountName() << " | " << req->GetRemoteFile();
                
                m_sftp->Write(wxFileName(req->GetLocalFile()), req->GetRemoteFile());
                GetNotifiedWindow()->CallAfter( &SFTP::OnFileWriteOK, msg);
                
            } catch (clException &e) {
                // Check to see if this is a connection error
                if ( e.ErrorCode() == SSH_FX_CONNECTION_LOST ) {
                    // Lost the connection, reconnect and try again
                    m_sftp.reset(NULL);
                    DoConnect(req);
                    continue;
                }
                wxString msg;
                msg << "SFTP: failed to write file: " << req->GetAccount().GetAccountName() << ":" << req->GetRemoteFile() << ". " << e.What();
                GetNotifiedWindow()->CallAfter( &SFTP::OnFileWriteError, msg);
                m_sftp.reset(NULL);
            }
            break; // the for loop
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
        msg << "SFTP: Connect error. " << req->GetAccount().GetAccountName() << ":" << req->GetRemoteFile() << ": " << e.What();
        GetNotifiedWindow()->CallAfter( &SFTP::OnFileWriteError, msg);
        m_sftp.reset(NULL);
    }
}

SFTPWriterThreadRequet::SFTPWriterThreadRequet(const SSHAccountInfo& accountInfo, const wxString& remoteFile, const wxString& localFile)
    : m_account(accountInfo)
    , m_remoteFile(remoteFile)
    , m_localFile(localFile)
{
}

SFTPWriterThreadRequet::~SFTPWriterThreadRequet()
{
}
