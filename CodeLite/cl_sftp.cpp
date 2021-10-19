//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_sftp.cpp
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

#if USE_SFTP
#include "cl_sftp.h"
#include "cl_standard_paths.h"
#include "file_logger.h"
#include "wx/tokenzr.h"
#include <atomic>
#include <libssh/sftp.h>
#include <string.h>
#include <sys/stat.h>
#include <wx/ffile.h>
#include <wx/filefn.h>

class SFTPDirCloser
{
    sftp_dir m_dir;

public:
    SFTPDirCloser(sftp_dir d)
        : m_dir(d)
    {
    }
    ~SFTPDirCloser() { sftp_closedir(m_dir); }
};

clSFTP::clSFTP(clSSH::Ptr_t ssh)
    : m_ssh(ssh)
    , m_sftp(NULL)
    , m_connected(false)
{
}

clSFTP::~clSFTP() { Close(); }

/**
 * @brief update 'attr' to include details about the symlink
 */
static void clSFTPReadLink(SFTPAttribute::Ptr_t attr, SFTPSession_t sftp, const wxString& curdir)
{
    if(!attr->IsSymlink()) {
        return;
    }
    // build the symlink full path and read the target path
    wxString symlinkPath = curdir + "/" + attr->GetName();
    const char* target_path = sftp_readlink(sftp, symlinkPath.mb_str(wxConvUTF8).data());
    if(target_path) {
        wxString targetPath = target_path;
        if(!targetPath.StartsWith("/")) {
            // relative path
            targetPath.Prepend(curdir + "/");
            targetPath.Replace("//", "/");
        }
        attr->SetSymlinkPath(targetPath);
        sftp_attributes linkattr = sftp_stat(sftp, targetPath.mb_str(wxConvUTF8).data());
        if(linkattr) {
            SFTPAttribute::Ptr_t targetAttr(new SFTPAttribute(linkattr));
            if(targetAttr->IsFile()) {
                attr->SetFile();
            }
            if(targetAttr->IsFolder()) {
                attr->SetFolder();
            }
        }
    }
}

void clSFTP::Initialize()
{
    if(m_sftp)
        return;

    m_sftp = sftp_new(m_ssh->GetSession());
    if(m_sftp == NULL) {
        throw clException(wxString() << "Error allocating SFTP session: " << ssh_get_error(m_ssh->GetSession()));
    }

    int rc = sftp_init(m_sftp);
    if(rc != SSH_OK) {
        throw clException(wxString() << "Error initializing SFTP session: " << ssh_get_error(m_ssh->GetSession()),
                          sftp_get_error(m_sftp));
    }
    m_connected = true;
}

void clSFTP::Close()
{
    if(m_sftp) {
        sftp_free(m_sftp);
    }

    m_connected = false;
    m_sftp = NULL;
}

void clSFTP::Write(const wxFileName& localFile, const wxString& remotePath)
{
    if(!m_connected) {
        throw clException("scp is not initialized!");
    }

    if(!localFile.Exists()) {
        throw clException(wxString() << "scp::Write file '" << localFile.GetFullPath() << "' does not exist!");
    }

    wxFFile fp(localFile.GetFullPath(), "rb");
    if(!fp.IsOpened()) {
        throw clException(wxString() << "scp::Write could not open file '" << localFile.GetFullPath() << "'. "
                                     << ::strerror(errno));
    }

    wxMemoryBuffer memBuffer;
    size_t len = fp.Length();
    void* buffer = memBuffer.GetWriteBuf(len);
    size_t nbytes(0);
    nbytes = fp.Read(buffer, fp.Length());
    if(nbytes != (size_t)fp.Length()) {
        throw clException(wxString() << "failed to read local file content. expected read size: " << len
                                     << ". bytes read: " << nbytes);
    }
    fp.Close();
    memBuffer.SetDataLen(len);
    Write(memBuffer, remotePath);
}

void clSFTP::Write(const wxMemoryBuffer& fileContent, const wxString& remotePath)
{
    if(!m_sftp) {
        throw clException("SFTP is not initialized");
    }

    static size_t counter = 0;
    int access_type = O_WRONLY | O_CREAT | O_TRUNC;
    sftp_file file;
    wxString tmpRemoteFile = remotePath;
    tmpRemoteFile << ".codelitesftp" << (++counter);

    auto cb = tmpRemoteFile.mb_str(wxConvUTF8);
    file = sftp_open(m_sftp, cb.data(), access_type, 0644);
    if(file == NULL) {
        throw clException(wxString() << _("Can't open file: ") << tmpRemoteFile << ". "
                                     << ssh_get_error(m_ssh->GetSession()),
                          sftp_get_error(m_sftp));
    }

    char* p = (char*)fileContent.GetData();
    const int maxChunkSize = 65536;
    wxInt64 bytesLeft = fileContent.GetDataLen();

    while(bytesLeft > 0) {
        wxInt64 chunkSize = bytesLeft > maxChunkSize ? maxChunkSize : bytesLeft;
        wxInt64 bytesWritten = sftp_write(file, p, chunkSize);
        if(bytesWritten < 0) {
            sftp_close(file);
            throw clException(wxString() << _("Can't write data to file: ") << tmpRemoteFile << ". "
                                         << ssh_get_error(m_ssh->GetSession()),
                              sftp_get_error(m_sftp));
        }
        bytesLeft -= bytesWritten;
        p += bytesWritten;
    }
    sftp_close(file);

    // Unlink the original file if it exists
    auto char_buffer_remote = remotePath.mb_str(wxConvUTF8);
    SFTPAttribute::Ptr_t pattr(new SFTPAttribute(sftp_stat(m_sftp, char_buffer_remote.data())));

    if(pattr->IsOk() && sftp_unlink(m_sftp, char_buffer_remote.data()) < 0) {
        throw clException(wxString() << _("Failed to unlink file: ") << remotePath << ". "
                                     << ssh_get_error(m_ssh->GetSession()),
                          sftp_get_error(m_sftp));
    }

    // Rename the file
    if(sftp_rename(m_sftp, cb.data(), char_buffer_remote.data()) < 0) {
        throw clException(wxString() << _("Failed to rename file: ") << tmpRemoteFile << " -> " << remotePath << ". "
                                     << ssh_get_error(m_ssh->GetSession()),
                          sftp_get_error(m_sftp));
    }

    if(pattr->IsOk()) {
        Chmod(remotePath, pattr->GetPermissions());
    }
}

SFTPAttribute::List_t clSFTP::List(const wxString& folder, size_t flags, const wxString& filter)
{
    sftp_dir dir;
    sftp_attributes attributes;

    if(!m_sftp) {
        throw clException("SFTP is not initialized");
    }

    dir = sftp_opendir(m_sftp, folder.mb_str(wxConvUTF8).data());
    if(!dir) {
        throw clException(wxString() << _("Failed to list directory: ") << folder << ". "
                                     << ssh_get_error(m_ssh->GetSession()),
                          sftp_get_error(m_sftp));
    }

    // Keep the current folder name
    m_currentFolder = dir->name;

    // Ensure the directory is closed
    SFTPDirCloser dc(dir);
    SFTPAttribute::List_t files;

    attributes = sftp_readdir(m_sftp, dir);
    while(attributes) {
        SFTPAttribute::Ptr_t attr(new SFTPAttribute(attributes));
        if(attr->IsSymlink()) {
            clSFTPReadLink(attr, m_sftp, m_currentFolder);
        }
        attributes = sftp_readdir(m_sftp, dir);

        // Don't show files ?
        if(!(flags & SFTP_BROWSE_FILES) && !attr->IsFolder()) {
            continue;

        } else if((flags & SFTP_BROWSE_FILES) && !attr->IsFolder() // show files
                  && filter.IsEmpty()) {                           // no filter is given
            files.push_back(attr);

        } else if((flags & SFTP_BROWSE_FILES) && !attr->IsFolder() // show files
                  && !::wxMatchWild(filter, attr->GetName())) {    // but the file does not match the filter
            continue;

        } else {
            files.push_back(attr);
        }
    }
    files.sort(SFTPAttribute::Compare);
    return files;
}

SFTPAttribute::List_t clSFTP::CdUp(size_t flags, const wxString& filter)
{
    wxString curfolder = m_currentFolder;
    curfolder << "/../"; // Force a cd up

    wxFileName fn(curfolder, "", wxPATH_UNIX);
    fn.Normalize();
    return List(fn.GetPath(false, wxPATH_UNIX), flags, filter);
}

SFTPAttribute::Ptr_t clSFTP::Read(const wxString& remotePath, wxMemoryBuffer& buffer)
{
    if(!m_sftp) {
        throw clException("SFTP is not initialized");
    }

    sftp_file file = sftp_open(m_sftp, remotePath.mb_str(wxConvUTF8).data(), O_RDONLY, 0);
    if(file == NULL) {
        throw clException(wxString() << _("Failed to open remote file: ") << remotePath << ". "
                                     << ssh_get_error(m_ssh->GetSession()),
                          sftp_get_error(m_sftp));
    }

    SFTPAttribute::Ptr_t fileAttr = Stat(remotePath);
    if(!fileAttr) {
        throw clException(wxString() << _("Could not stat file:") << remotePath << ". "
                                     << ssh_get_error(m_ssh->GetSession()),
                          sftp_get_error(m_sftp));
    }
    wxInt64 fileSize = fileAttr->GetSize();
    if(fileSize == 0)
        return fileAttr;

    // Allocate buffer for the file content
    char pBuffer[65536]; // buffer

    // Read the entire file content
    wxInt64 bytesLeft = fileSize;
    wxInt64 bytesRead = 0;
    while(bytesLeft > 0) {
        wxInt64 nbytes = sftp_read(file, pBuffer, sizeof(pBuffer));
        bytesRead += nbytes;
        bytesLeft -= nbytes;
        buffer.AppendData(pBuffer, nbytes);
    }

    if(bytesRead != fileSize) {
        sftp_close(file);
        buffer.Clear();
        throw clException(wxString() << _("Could not read file:") << remotePath << ". "
                                     << ssh_get_error(m_ssh->GetSession()),
                          sftp_get_error(m_sftp));
    }
    sftp_close(file);
    return fileAttr;
}

void clSFTP::CreateDir(const wxString& dirname)
{
    if(!m_sftp) {
        throw clException("SFTP is not initialized");
    }

    int rc;
    rc = sftp_mkdir(m_sftp, dirname.mb_str(wxConvUTF8).data(), S_IRWXU);

    if(rc != SSH_OK) {
        throw clException(wxString() << _("Failed to create directory: ") << dirname << ". "
                                     << ssh_get_error(m_ssh->GetSession()),
                          sftp_get_error(m_sftp));
    }
}

void clSFTP::Rename(const wxString& oldpath, const wxString& newpath)
{
    if(!m_sftp) {
        throw clException("SFTP is not initialized");
    }

    int rc;
    rc = sftp_rename(m_sftp, oldpath.mb_str(wxConvUTF8).data(), newpath.mb_str(wxConvUTF8).data());

    if(rc != SSH_OK) {
        throw clException(wxString() << _("Failed to rename path. ") << ssh_get_error(m_ssh->GetSession()),
                          sftp_get_error(m_sftp));
    }
}

void clSFTP::RemoveDir(const wxString& dirname)
{
    if(!m_sftp) {
        throw clException("SFTP is not initialized");
    }

    int rc;
    rc = sftp_rmdir(m_sftp, dirname.mb_str(wxConvUTF8).data());

    if(rc != SSH_OK) {
        throw clException(wxString() << _("Failed to remove directory: ") << dirname << "\n" << GetErrorString());
    }
}

void clSFTP::UnlinkFile(const wxString& path)
{
    if(!m_sftp) {
        throw clException("SFTP is not initialized");
    }

    int rc;
    rc = sftp_unlink(m_sftp, path.mb_str(wxConvUTF8).data());

    if(rc != SSH_OK) {
        throw clException(wxString() << _("Failed to unlink path: ") << path << ". "
                                     << ssh_get_error(m_ssh->GetSession()),
                          sftp_get_error(m_sftp));
    }
}

SFTPAttribute::Ptr_t clSFTP::Stat(const wxString& path)
{
    if(!m_sftp) {
        throw clException("SFTP is not initialized");
    }

    sftp_attributes attr = sftp_stat(m_sftp, path.mb_str(wxConvUTF8).data());
    if(!attr) {
        throw clException(wxString() << _("Could not stat: ") << path << ". " << ssh_get_error(m_ssh->GetSession()),
                          sftp_get_error(m_sftp));
    }
    wxString target;
    if(attr->type == SSH_FILEXFER_TYPE_SYMLINK) {
        // this is a symlink file, read the symlink info
        const char* ctarget = sftp_readlink(m_sftp, path.mb_str(wxConvUTF8).data());
        if(!ctarget) {
            throw clException(wxString() << _("Failed to read symlink target. ") << ssh_get_error(m_ssh->GetSession()));
        }
        target = ctarget;
    }
    SFTPAttribute::Ptr_t pattr(new SFTPAttribute(attr));
    if(attr->type == SSH_FILEXFER_TYPE_SYMLINK) {
        pattr->SetSymlinkPath(target);
    }
    return pattr;
}

void clSFTP::CreateRemoteFile(const wxString& remoteFullPath, const wxString& content)
{
    // Create the path to the file
    Mkpath(wxFileName(remoteFullPath).GetPath());
    Write(content, remoteFullPath);
}

void clSFTP::CreateEmptyFile(const wxString& remotePath)
{
    Mkpath(wxFileName(remotePath).GetPath());
    wxMemoryBuffer mb;
    Write(mb, remotePath);
}

void clSFTP::Mkpath(const wxString& remoteDirFullpath)
{
    if(!m_sftp) {
        throw clException("SFTP is not initialized");
    }

    wxString tmpPath = remoteDirFullpath;
    tmpPath.Replace("\\", "/");
    if(!tmpPath.StartsWith("/")) {
        throw clException("Mkpath: path must be absolute");
    }

    wxFileName fn(remoteDirFullpath, "");
    const wxArrayString& dirs = fn.GetDirs();
    wxString curdir;

    curdir << "/";
    for(size_t i = 0; i < dirs.GetCount(); ++i) {
        curdir << dirs.Item(i);
        sftp_attributes attr = sftp_stat(m_sftp, curdir.mb_str(wxConvUTF8).data());
        if(!attr) {
            // directory does not exists
            CreateDir(curdir);

        } else {
            // directory already exists
            sftp_attributes_free(attr);
        }
        curdir << "/";
    }
}

void clSFTP::CreateRemoteFile(const wxString& remoteFullPath, const wxFileName& localFile)
{
    Mkpath(wxFileName(remoteFullPath).GetPath());
    Write(localFile, remoteFullPath);
}

void clSFTP::Chmod(const wxString& remotePath, size_t permissions)
{
    if(!m_sftp) {
        throw clException("SFTP is not initialized");
    }

    if(permissions == 0)
        return;

    int rc = sftp_chmod(m_sftp, remotePath.mb_str(wxConvUTF8).data(), permissions);
    if(rc != SSH_OK) {
        throw clException(wxString() << _("Failed to chmod file: ") << remotePath << ". "
                                     << ssh_get_error(m_ssh->GetSession()),
                          sftp_get_error(m_sftp));
    }
}

wxString clSFTP::GetDefaultDownloadFolder(const SSHAccountInfo& accountInfo)
{
    wxFileName path(clStandardPaths::Get().GetUserDataDir(), "");
    path.AppendDir("sftp");
    path.AppendDir("download");
    if(!accountInfo.GetHost().empty()) {
        path.AppendDir(accountInfo.GetHost());
    }
    return path.GetPath();
}

wxFileName clSFTP::GetLocalFileName(const SSHAccountInfo& accountInfo, const wxString& remotePath, bool mkdirRecrusive)
{
    // Generate a temporary file location
    wxFileName fnRemoteFile(remotePath);
    wxFileName localFile(GetDefaultDownloadFolder(accountInfo), fnRemoteFile.GetFullName());
    const wxArrayString& dirs = fnRemoteFile.GetDirs();
    for(size_t i = 0; i < dirs.GetCount(); ++i) {
        localFile.AppendDir(dirs.Item(i));
    }

    if(mkdirRecrusive) {
        // Ensure that the path to the sftp temp folder exists
        localFile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }
    return localFile.GetFullPath();
}

wxString clSFTP::GetErrorString() const
{
    if(!m_sftp) {
        return wxEmptyString;
    }

    int errorCode = sftp_get_error(m_sftp);
    switch(errorCode) {
    case SSH_FX_OK:
        return "no error";
    case SSH_FX_EOF:
        return "end-of-file encountered";
    case SSH_FX_NO_SUCH_FILE:
        return "file does not exist";
    case SSH_FX_PERMISSION_DENIED:
        return "permission denied";
    case SSH_FX_FAILURE:
        return "generic failure";
    case SSH_FX_BAD_MESSAGE:
        return "garbage received from server";
    case SSH_FX_NO_CONNECTION:
        return "no connection has been set up";
    case SSH_FX_CONNECTION_LOST:
        return "there was a connection, but we lost it";
    case SSH_FX_OP_UNSUPPORTED:
        return "operation not supported by libssh yet";
    case SSH_FX_INVALID_HANDLE:
        return "invalid file handle";
    case SSH_FX_NO_SUCH_PATH:
        return "no such file or directory path exists";
    case SSH_FX_FILE_ALREADY_EXISTS:
        return "an attempt to create an already existing file or directory has been made";
    case SSH_FX_WRITE_PROTECT:
        return "write-protected filesystem";
    case SSH_FX_NO_MEDIA:
        return "no media was in remote drive";
    default:
        return wxEmptyString;
    }
}

void clSFTP::SendKeepAlive()
{
    if(!m_sftp || !m_ssh) {
        return;
    }
    m_ssh->SendIgnore();
}

SFTPAttribute::List_t clSFTP::Chdir(const wxString& remotePath)
{
    if(!m_sftp) {
        throw clException("SFTP is not initialized");
    }

    // Check that the directory exists
    auto attr = Stat(remotePath);
    if(!attr->IsFolder()) {
        throw clException("Chdir failed. " + remotePath + " is not a directory");
    }
    return List(remotePath, SFTP_BROWSE_FILES | SFTP_BROWSE_FOLDERS);
}

wxString clSFTP::ExecuteCommand(const wxString& command)
{
    if(!m_sftp) {
        throw clException("SFTP is not initialized");
    }

    ssh_channel channel = ssh_channel_new(m_ssh->GetSession());
    if(!channel) {
        throw clException("Failed to allocate ssh channel");
    }

    int rc = ssh_channel_open_session(channel);
    if(rc != SSH_OK) {
        ssh_channel_free(channel);
        throw clException("Failed to open ssh channel");
    }
    rc = ssh_channel_request_exec(channel, command.mb_str(wxConvUTF8).data());
    if(rc != SSH_OK) {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        throw clException(wxString() << "Failed to execute command: " << command);
    }

    // read the result
    char buffer[256];
    int nbytes;

    wxString result;
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer) - 1, 0);
    while(nbytes > 0) {
        buffer[nbytes] = 0;
        result << buffer;
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }

    if(nbytes < 0) {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        throw clException(wxString() << "Failed to read from channel. Command: " << command);
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return result;
}

bool clSFTP::GetChecksum(const wxString& remoteFile, size_t* checksum)
{
    wxString command;
    command << "cksum " << remoteFile;
    try {
        wxString output = ExecuteCommand(command);
        auto parts = ::wxStringTokenize(output, " \t", wxTOKEN_STRTOK);
        if(parts.empty()) {
            return false;
        }
        // the checksum is the first part
        unsigned long ck;
        if(!parts[0].ToCULong(&ck)) {
            return false;
        }
        *checksum = ck;
        return true;
    } catch(clException& e) {
        clWARNING() << e.What() << endl;
        return false;
    }
}

#endif // USE_SFTP
