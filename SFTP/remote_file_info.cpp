//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : remote_file_info.cpp
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

#include "remote_file_info.h"
#include <wx/stdpaths.h>
#include "cl_standard_paths.h"
#include "cl_sftp.h"

RemoteFileInfo::RemoteFileInfo()
    : m_premissions(0)
{
}

RemoteFileInfo::~RemoteFileInfo() {}

void RemoteFileInfo::SetRemoteFile(const wxString& remoteFile)
{
    this->m_remoteFile = remoteFile;

    // Generate a temporary file location
    wxFileName fnRemoteFile(m_remoteFile);
    wxFileName localFile(clSFTP::GetDefaultDownloadFolder(), fnRemoteFile.GetFullName());
    const wxArrayString& dirs = fnRemoteFile.GetDirs();
    for(size_t i = 0; i < dirs.GetCount(); ++i) {
        localFile.AppendDir(dirs.Item(i));
    }

    // Ensure that the path to the sftp temp folder exists
    localFile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    m_localFile = localFile.GetFullPath();
}

wxString RemoteFileInfo::GetTempFolder() { return clSFTP::GetDefaultDownloadFolder(); }
