//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : importfilessettings.h
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
#ifndef __importfilessettings__
#define __importfilessettings__
#include "serialized_object.h"

enum { IFS_INCLUDE_FILES_WO_EXT = 0x00000001 };

class ImportFilesSettings : public SerializedObject
{
    wxString m_fileMask;
    size_t m_flags;
    wxString m_baseDir;

public:
    ImportFilesSettings();
    virtual ~ImportFilesSettings() = default;

    virtual void DeSerialize(Archive& arch);
    virtual void Serialize(Archive& arch);

    // Setters
    void SetFileMask(const wxString& fileMask)
    {
        this->m_fileMask = fileMask;
    }
    void SetFlags(size_t flags) { this->m_flags = flags; }
    // Getters
    const wxString& GetFileMask() const
    {
        return m_fileMask;
    }
    size_t GetFlags() const { return m_flags; }

    void SetBaseDir(const wxString& baseDir)
    {
        this->m_baseDir = baseDir;
    }
    const wxString& GetBaseDir() const
    {
        return m_baseDir;
    }
};
#endif // __importfilessettings__
