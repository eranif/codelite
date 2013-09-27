#include "cl_sftp.h"
#include <wx/ffile.h>
#include <string.h>
#include <sys/stat.h>
#include <wx/filefn.h>
#include <libssh/sftp.h>

class SFTPDirCloser {
    sftp_dir m_dir;
public:
    SFTPDirCloser(sftp_dir d) : m_dir(d) {}
    ~SFTPDirCloser() {
        sftp_closedir(m_dir);
    }
};

clSFTP::clSFTP(clSSH::Ptr_t ssh)
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

    m_sftp = sftp_new(m_ssh->GetSession());
    if (m_sftp == NULL) {
        throw clException(wxString() << "Error allocating SFTP session: " << ssh_get_error(m_ssh->GetSession()));
    }

    int rc = sftp_init(m_sftp);
    if (rc != SSH_OK) {
        throw clException(wxString() << "Error initializing SFTP session: " << ssh_get_error(m_ssh->GetSession()), sftp_get_error(m_sftp));
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
        throw clException(wxString() << _("Can't open file: ") << remotePath << ". " << ssh_get_error(m_ssh->GetSession()), sftp_get_error(m_sftp));
    }

    size_t nbytes = sftp_write(file, str.c_str(), str.length());
    if (nbytes != str.length()) {
        sftp_close(file);
        throw clException(wxString() << _("Can't write data to file: ") << remotePath << ". " << ssh_get_error(m_ssh->GetSession()), sftp_get_error(m_sftp));
    }

    sftp_close(file);
}

SFTPAttribute::List_t clSFTP::List(const wxString &folder, size_t flags, const wxString &filter) throw (clException)
{
    sftp_dir dir;
    sftp_attributes attributes;
    
    if ( !m_sftp ) {
        throw clException("SFTP is not initialized");
    }
    
    dir = sftp_opendir(m_sftp, folder.mb_str(wxConvUTF8).data());
    if ( !dir ) {
        throw clException(wxString() << _("Failed to list directory: ") << folder << ". " << ssh_get_error(m_ssh->GetSession()), sftp_get_error(m_sftp));
    }
    
    // Keep the current folder name
    m_currentFolder = dir->name;
    
    // Ensure the directory is closed
    SFTPDirCloser dc(dir);
    SFTPAttribute::List_t files;
    
    attributes = sftp_readdir(m_sftp, dir);
    while ( attributes ) {
        
        SFTPAttribute::Ptr_t attr( new SFTPAttribute(attributes) );
        attributes = sftp_readdir(m_sftp, dir);
        
        // Don't show files ?
        if ( !(flags & SFTP_BROWSE_FILES) && !attr->IsFolder()) {
            continue;
        
        } else if ( (flags & SFTP_BROWSE_FILES) && !attr->IsFolder()  // show files
                    && filter.IsEmpty() ) {                           // no filter is given
            files.push_back( attr );
            
        } else if ( (flags & SFTP_BROWSE_FILES) && !attr->IsFolder()  // show files
                    && !::wxMatchWild(filter, attr->GetName()) ) {    // but the file does not match the filter
            continue;
            
        } else {
            files.push_back( attr );
            
        }
    }
    files.sort( SFTPAttribute::Compare );
    return files;
}

wxString clSFTP::GetAccountName() const
{
    if ( !m_ssh ) {
        return "";
        
    } else {
        return wxString() << m_ssh->GetUsername() << "@" << m_ssh->GetHost();
        
    }
}

SFTPAttribute::List_t clSFTP::CdUp(size_t flags, const wxString &filter) throw (clException)
{
    wxString curfolder = m_currentFolder;
    curfolder << "/../"; // Force a cd up
    
    wxFileName fn(curfolder, "", wxPATH_UNIX);
    fn.Normalize();
    return List(fn.GetPath(false, wxPATH_UNIX), flags, filter);
}
