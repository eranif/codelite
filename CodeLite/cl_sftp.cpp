#include "cl_sftp.h"
#include <wx/ffile.h>
#include <string.h>
#include <sys/stat.h>

clSFTP::clSFTP(clSSH& ssh)
    : m_ssh(ssh)
    , m_sftp(NULL)
    , m_connected(false)
{
}

clSFTP::~clSFTP()
{
    Close();
}

void clSFTP::Initialize() throw (clException)
{
    if ( m_sftp )
        return;

    m_sftp = sftp_new(m_ssh.GetSession());
    if (m_sftp == NULL) {
        throw clException(wxString() << "Error allocating SFTP session: " << ssh_get_error(m_ssh.GetSession()));
    }

    int rc = sftp_init(m_sftp);
    if (rc != SSH_OK) {
        throw clException(wxString() << "Error initializing SFTP session: " << ssh_get_error(m_ssh.GetSession()));
    }
    m_connected = true;
}

void clSFTP::Close()
{
    if ( m_sftp ) {
        sftp_free(m_sftp);
    }

    m_connected = false;
    m_sftp = NULL;
}

void clSFTP::Write(const wxFileName& localFile, const wxString& remotePath) throw (clException)
{
    if ( !m_connected ) {
        throw clException("scp is not initialized!");
    }

    if ( !localFile.Exists() ) {
        throw clException(wxString() << "scp::Write file '" << localFile.GetFullPath() << "' does not exist!");
    }

    wxFFile fp(localFile.GetFullPath(), "rb");
    if ( !fp.IsOpened() ) {
        throw clException(wxString() << "scp::Write could not open file '" << localFile.GetFullPath() << "'. " << ::strerror(errno) );
    }
    
    wxString fileContent;
    fp.ReadAll(&fileContent);
    
    Write(fileContent, remotePath);
}

void clSFTP::Write(const wxString& fileContent, const wxString& remotePath) throw (clException)
{
    if ( !m_sftp ) {
        throw clException("SFTP is not initialized");
    }

    int access_type = O_WRONLY | O_CREAT | O_TRUNC;
    sftp_file file;
    std::string str = fileContent.mb_str(wxConvUTF8).data();

    file = sftp_open(m_sftp, remotePath.mb_str(wxConvUTF8).data(), access_type, 0644);
    if (file == NULL) {
        throw clException(wxString() << _("Can't open file: ") << remotePath << ". " << ssh_get_error(m_ssh.GetSession()));
    }

    size_t nbytes = sftp_write(file, str.c_str(), str.length());
    if (nbytes != str.length()) {
        sftp_close(file);
        throw clException(wxString() << _("Can't write data to file: ") << remotePath << ". " << ssh_get_error(m_ssh.GetSession()));
    }
    
    sftp_close(file);
}
