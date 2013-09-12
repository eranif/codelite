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

#include <libssh/sftp.h>
#include <wx/filename.h>
#include "codelite_exports.h"
#include <list>
#include <wx/sharedptr.h>

class WXDLLIMPEXP_CL SFTPAttribute
{
    wxString m_name;
    size_t   m_flags;
    size_t   m_size;
    sftp_attributes m_attributes;

public:
    typedef wxSharedPtr<SFTPAttribute>      Ptr_t;
    typedef std::list<SFTPAttribute::Ptr_t> List_t;

    enum {
        TYPE_FOLDER       = 0x00000001,
        TYPE_SYMBLINK     = 0x00000002,
        TYPE_REGULAR_FILE = 0x00000004,
        TYPE_UNKNOWN      = 0x00000008,
    };

protected:
    void DoClear();
    void DoConstruct();
    
public:
    SFTPAttribute(sftp_attributes attr);
    virtual ~SFTPAttribute();
    
    /**
     * @brief assign this object with attributes.
     * This object takes ownership of the attributes and it will free it when done
     * @param attr
     */
    void Assign(sftp_attributes attr);
    
    const wxString& GetName() const {
        return m_name;
    }
    
    bool IsFolder() const {
        return m_flags & TYPE_FOLDER;
    }
    
    bool IsFile() const {
        return m_flags & TYPE_REGULAR_FILE;
    }
    
    bool IsSymlink() const {
        return m_flags & TYPE_SYMBLINK;
    }
};
#endif // SFTPATTRIBUTE_H
