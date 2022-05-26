//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : sftp_item_comparator.h
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

#ifndef SFTPITEMCOMPARATOR_H
#define SFTPITEMCOMPARATOR_H

#include "codelite_exports.h"

#include <vector>
#include <wx/treectrl.h>

class WXDLLIMPEXP_SDK clRemoteDirCtrlItemData : public wxTreeItemData
{
public:
    enum eFileType {
        kFile = (1 << 0),
        kFolder = (1 << 1),
        kSymlink = (1 << 2),
    };

protected:
    wxString m_path;
    wxString m_symlinkTarget;
    bool m_initialized = false;
    size_t m_flags = kFile;

public:
    typedef std::vector<clRemoteDirCtrlItemData*> Vec_t;

public:
    clRemoteDirCtrlItemData(const wxString& path)
        : m_path(path)
    {
        m_path.Replace("\\", "/");
        while(m_path.Replace("//", "/")) {}
    }

    virtual ~clRemoteDirCtrlItemData() {}

    wxString GetBasename() const { return GetFullPath().BeforeLast('/'); }
    wxString GetFullName() const { return GetFullPath().AfterLast('/'); }
    void SetFullName(const wxString& fullname)
    {
        wxString base = GetBasename();
        base << "/" << fullname;
        m_path.swap(base);
    }

    void SetInitialized(bool initialized) { this->m_initialized = initialized; }
    bool IsInitialized() const { return m_initialized; }
    void SetPath(const wxString& path) { this->m_path = path; }
    const wxString& GetFullPath() const { return m_path; }
    void SetFolder()
    {
        this->m_flags &= ~kFile;
        this->m_flags |= kFolder;
    }
    void SetFile()
    {
        this->m_flags &= ~kFolder;
        this->m_flags |= kFile;
    }
    void SetSymlink() { this->m_flags |= kSymlink; }
    bool IsSymlink() const { return this->m_flags & kSymlink; }
    bool IsFile() const { return this->m_flags & kFile; }
    bool IsFolder() const { return this->m_flags & kFolder; }
    void SetSymlinkTarget(const wxString& path) { this->m_symlinkTarget = path; }
    const wxString& GetSymlinkTarget() const { return m_symlinkTarget; }
};

#endif // SFTPITEMCOMPARATOR_H
