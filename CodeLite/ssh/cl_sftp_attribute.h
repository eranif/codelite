//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : cl_sftp_attribute.h
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
#ifndef SFTPATTRIBUTE_H
#define SFTPATTRIBUTE_H

#if USE_SFTP

#include "codelite_exports.h"
#include "smart_ptr.h"
#include <list>
#include <wx/clntdata.h>
#include <wx/filename.h>

// We do it this way to avoid exposing the include to <libssh/sftp.h> to files including this header
struct sftp_attributes_struct;
typedef struct sftp_attributes_struct* SFTPAttribute_t;

class WXDLLIMPEXP_CL SFTPAttribute : public wxClientData
{
    wxString m_name;
    size_t m_flags;
    size_t m_size;
    SFTPAttribute_t m_attributes = nullptr;
    size_t m_permissions = 0;
    wxString m_symlinkPath; // incase this file represents a symlink, this member will hold the target path

public:
    typedef SmartPtr<SFTPAttribute> Ptr_t;
    typedef std::list<SFTPAttribute::Ptr_t> List_t;

    enum {
        TYPE_FOLDER = 0x00000001,
        TYPE_SYMBLINK = 0x00000002,
        TYPE_REGULAR_FILE = 0x00000004,
        TYPE_SEPCIAL = 0x00000008,
        TYPE_UNKNOWN = 0x00000010,
    };

protected:
    void DoClear();
    void DoConstruct();

public:
    SFTPAttribute(SFTPAttribute_t attr);
    virtual ~SFTPAttribute();

    bool IsOk() const { return m_attributes != nullptr; }
    static bool Compare(SFTPAttribute::Ptr_t one, SFTPAttribute::Ptr_t two);
    /**
     * @brief assign this object with attributes.
     * This object takes ownership of the attributes and it will free it when done
     * @param attr
     */
    void Assign(SFTPAttribute_t attr);

    size_t GetSize() const { return m_size; }
    wxString GetTypeAsString() const;
    const wxString& GetName() const { return m_name; }

    bool IsFolder() const { return m_flags & TYPE_FOLDER; }

    bool IsFile() const { return m_flags & TYPE_REGULAR_FILE; }

    bool IsSymlink() const { return m_flags & TYPE_SYMBLINK; }

    bool IsSpecial() const { return m_flags & TYPE_SEPCIAL; }
    void SetPermissions(size_t permissions) { this->m_permissions = permissions; }
    size_t GetPermissions() const { return m_permissions; }

    void SetSymlinkPath(const wxString& symlinkPath) { this->m_symlinkPath = symlinkPath; }
    const wxString& GetSymlinkPath() const { return m_symlinkPath; }

    void SetFile()
    {
        m_flags &= ~TYPE_FOLDER;
        m_flags |= TYPE_REGULAR_FILE;
    }
    void SetFolder()
    {
        m_flags &= ~TYPE_REGULAR_FILE;
        m_flags |= TYPE_FOLDER;
    }
};
#endif
#endif // SFTPATTRIBUTE_H
