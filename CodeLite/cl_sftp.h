//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : cl_sftp.h
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

#ifndef CLSCP_H
#define CLSCP_H

#if USE_SFTP

#include "cl_ssh.h"
#include "cl_exception.h"
#include <wx/filename.h>
#include "codelite_exports.h"
#include "cl_sftp_attribute.h"
#include <wx/buffer.h>

// We do it this way to avoid exposing the include to <libssh/sftp.h> to files including this header
struct sftp_session_struct;
typedef struct sftp_session_struct* SFTPSession_t;

class WXDLLIMPEXP_CL clSFTP
{
    clSSH::Ptr_t m_ssh;
    SFTPSession_t m_sftp;
    bool m_connected;
    wxString m_currentFolder;
    wxString m_account;

public:
    typedef wxSharedPtr<clSFTP> Ptr_t;
    enum {
        SFTP_BROWSE_FILES = 0x00000001,
        SFTP_BROWSE_FOLDERS = 0x00000002,
    };

public:
    clSFTP(clSSH::Ptr_t ssh);
    virtual ~clSFTP();
    
    /**
     * @brief return the default download folder
     */
    static wxString GetDefaultDownloadFolder();
    
    /**
     * @brief return the underlying ssh session
     */
    clSSH::Ptr_t GetSsh() const { return m_ssh; }

    bool IsConnected() const { return m_connected; }

    void SetAccount(const wxString& account) { this->m_account = account; }
    const wxString& GetAccount() const { return m_account; }
    /**
     * @brief intialize the scp over ssh
     */
    void Initialize() ;

    /**
     * @brief close the scp channel
     */
    void Close();

    /**
     * @brief write the content of local file into a remote file
     * @param localFile the local file
     * @param remotePath the remote path (abs path)
     */
    void Write(const wxFileName& localFile,
               const wxString& remotePath,
               SFTPAttribute::Ptr_t attributes = SFTPAttribute::Ptr_t(NULL)) ;

    /**
     * @brief write the content of 'fileContent' into the remote file represented by remotePath
     */
    void Write(const wxMemoryBuffer& fileContent,
               const wxString& remotePath,
               SFTPAttribute::Ptr_t attributes = SFTPAttribute::Ptr_t(NULL)) ;

    /**
     * @brief read remote file and return its content
     * @return the file content + the file attributes
     */
    SFTPAttribute::Ptr_t Read(const wxString& remotePath, wxMemoryBuffer& buffer) ;

    /**
     * @brief list the content of a folder
     * @param folder
     * @param foldersOnly
     * @param filter filter out files that do not match the filter
     * @throw clException incase an error occurred
     */
    SFTPAttribute::List_t List(const wxString& folder, size_t flags, const wxString& filter = "") ;

    /**
     * @brief create a directory
     * @param dirname
     */
    void CreateDir(const wxString& dirname) ;

    /**
     * @brief create a file. This function also creates the path to the file (by calling internally to Mkpath)
     */
    void CreateRemoteFile(const wxString& remoteFullPath,
                          const wxString& content,
                          SFTPAttribute::Ptr_t attr) ;

    /**
     * @brief this version create a copy of the local file on the remote server. Similar to the previous
     * version, it also creates the path to the file if needed
     */
    void CreateRemoteFile(const wxString& remoteFullPath,
                          const wxFileName& localFile,
                          SFTPAttribute::Ptr_t attr) ;

    /**
     * @brief create path . If the directory does not exist, create it (all sub paths if needed)
     */
    void Mkpath(const wxString& remoteDirFullpath) ;

    /**
     * @brief Remove a directoy.
     * @param dirname
     */
    void RemoveDir(const wxString& dirname) ;

    /**
     * @brief Unlink (delete) a file.
     * @param dirname
     */
    void UnlinkFile(const wxString& path) ;

    /**
     * @brief Rename or move a file or directory
     * @param oldpath
     * @param newpath
     */
    void Rename(const wxString& oldpath, const wxString& newpath) ;
    /**
     * @brief cd up and list the content of the directory
     * @return
     */
    SFTPAttribute::List_t CdUp(size_t flags, const wxString& filter) ;

    /**
     * @brief stat the path
     */
    SFTPAttribute::Ptr_t Stat(const wxString& path) ;

    /**
     * @brief change remote file permissions
     */
    void Chmod(const wxString& remotePath, size_t permissions) ;

    /**
     * @brief return the current folder
     */
    const wxString& GetCurrentFolder() const { return m_currentFolder; }
};

#endif // USE_SFTP
#endif // CLSCP_H
