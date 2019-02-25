//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBPivot.cpp
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

#include "LLDBPivot.h"
#include <wx/filename.h>

LLDBPivot::LLDBPivot() {}

LLDBPivot::~LLDBPivot() {}

wxString LLDBPivot::ToLocal(const wxString& remotePath) const
{
    wxString pathInNativeSeparators = remotePath;
    if(pathInNativeSeparators.StartsWith(m_remoteFolder)) {
        pathInNativeSeparators.Replace(m_remoteFolder, m_localFolder, false);
        wxFileName fn(pathInNativeSeparators);
        return fn.GetFullPath();
    } else {
        return remotePath;
    }
}

wxString LLDBPivot::ToRemote(const wxString& localPath) const
{
    wxFileName fn(localPath);
    wxString pathInNativeSeparators = fn.GetFullPath();
    if(pathInNativeSeparators.StartsWith(m_localFolder)) {
        pathInNativeSeparators.Replace(m_localFolder, m_remoteFolder, false);
        pathInNativeSeparators.Replace("\\", "/");
        return pathInNativeSeparators;
    } else {
        return localPath;
    }
}

bool LLDBPivot::IsValid() const { return !m_localFolder.IsEmpty() && !m_remoteFolder.IsEmpty(); }
