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

#include <vector>
#include <wx/treectrl.h>

class MyClientData : public wxTreeItemData
{
    wxString m_path;
    bool     m_initialized;
    bool     m_isFolder;

public:
    typedef std::vector<MyClientData*> Vector_t;

public:
    MyClientData(const wxString &path)
        : m_path(path)
        , m_initialized(false)
        , m_isFolder(false) {
        while (m_path.Replace("//", "/")) {}
        while (m_path.Replace("\\\\", "\\")) {}
    }

    virtual ~MyClientData() {}
    
    wxString GetBasename() const {
        return GetFullPath().BeforeLast('/');
    }
    wxString GetFullName() const {
        return GetFullPath().AfterLast('/');
    }
    void SetFullName( const wxString &fullname ) {
        wxString base = GetBasename();
        base << "/" << fullname;
        m_path.swap( base );
    }
    
    void SetInitialized(bool initialized) {
        this->m_initialized = initialized;
    }
    bool IsInitialized() const {
        return m_initialized;
    }
    void SetPath(const wxString& path) {
        this->m_path = path;
    }
    const wxString& GetFullPath() const {
        return m_path;
    }
    void SetIsFolder(bool isFolder) {
        this->m_isFolder = isFolder;
    }
    bool IsFolder() const {
        return m_isFolder;
    }
};

#endif // SFTPITEMCOMPARATOR_H
