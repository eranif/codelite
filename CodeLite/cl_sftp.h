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

#include "cl_ssh.h"
#include "cl_exception.h"
#include <libssh/sftp.h>
#include <wx/filename.h>
#include "codelite_exports.h"
#include "cl_sftp_attribute.h"

class WXDLLIMPEXP_CL clSFTP
{
    clSSH::Ptr_t m_ssh;
    sftp_session m_sftp;
    bool         m_connected;
    
public:
    typedef wxSharedPtr<clSFTP> Ptr_t;
    
public:
    clSFTP(clSSH::Ptr_t ssh);
    virtual ~clSFTP();

    bool IsConnected() const {
        return m_connected;
    }
    
    /**
     * @brief intialize the scp over ssh
     */
    void Initialize() throw (clException);

    /**
     * @brief close the scp channel
     */
    void Close();
    
    /**
     * @brief write the content of local file into a remote file
     * @param localFile the local file
     * @param remotePath the remote path (abs path)
     */
    void Write(const wxFileName& localFile, const wxString &remotePath) throw (clException);
    
    /**
     * @brief write the content of 'fileContent' into the remote file represented by remotePath
     */
    void Write(const wxString &fileContent, const wxString &remotePath) throw (clException);
    
    /**
     * @brief list the content of a folder
     * @param folder
     * @param foldersOnly
     * @param filter filter out files that do not match the filter
     * @throw clException incase an error occured
     */
    SFTPAttribute::List_t List(const wxString &folder, bool foldersOnly, const wxString &filter = "") throw (clException);
};

#endif // CLSCP_H
