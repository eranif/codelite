//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_sftp_attribute.cpp
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

#include "cl_sftp_attribute.h"
#include <libssh/sftp.h>

SFTPAttribute::SFTPAttribute(SFTPAttribute_t attr)
    : m_attributes(NULL)
    , m_permissions(0)
{
    Assign(attr);
}

SFTPAttribute::~SFTPAttribute() { DoClear(); }

void SFTPAttribute::Assign(SFTPAttribute_t attr)
{
    DoClear();
    m_attributes = attr;
    DoConstruct();
}

void SFTPAttribute::DoClear()
{
    if(m_attributes) { sftp_attributes_free(m_attributes); }
    m_attributes = NULL;
    m_name.Clear();
    m_flags = 0;
    m_size = 0;
    m_permissions = 0;
}

void SFTPAttribute::DoConstruct()
{
    if(!m_attributes) return;

    m_name = m_attributes->name;
    m_size = m_attributes->size;
    m_permissions = m_attributes->permissions;
    m_flags = 0;

    switch(m_attributes->type) {
    case SSH_FILEXFER_TYPE_DIRECTORY:
        m_flags |= TYPE_FOLDER;
        break;
    default:
    case SSH_FILEXFER_TYPE_REGULAR:
        m_flags |= TYPE_REGULAR_FILE;
        break;
    case SSH_FILEXFER_TYPE_SPECIAL:
        m_flags |= TYPE_SEPCIAL;
        break;

    case SSH_FILEXFER_TYPE_UNKNOWN:
        m_flags |= TYPE_UNKNOWN;
        break;

    case SSH_FILEXFER_TYPE_SYMLINK:
        m_flags |= TYPE_SYMBLINK;
        break;
    }
}

wxString SFTPAttribute::GetTypeAsString() const
{
    if(IsSymlink()) {
        if(IsFolder()) {
            return " -> " + GetSymlinkPath();
        } else if(IsFile()) {
            return " -> " + GetSymlinkPath();
        } else {
            return "Symlink";
        }
    } else {
        if(IsSpecial()) {
            return "Special";
        } else if(IsFolder()) {
            return "Folder";
        } else if(IsFile()) {
            return "File";
        } else {
            return "Unknown";
        }
    }
}

bool SFTPAttribute::Compare(SFTPAttribute::Ptr_t one, SFTPAttribute::Ptr_t two)
{
    if(one->IsFolder() && !two->IsFolder()) {
        return true;

    } else if(!one->IsFolder() && two->IsFolder()) {
        return false;

    } else {
        return one->GetName() < two->GetName();
    }
}

#endif // USE_SFTP
