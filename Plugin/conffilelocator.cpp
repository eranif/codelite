//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : conffilelocator.cpp
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

#include "cl_standard_paths.h"
#include "conffilelocator.h"
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include "fileutils.h"

ConfFileLocator* ConfFileLocator::ms_instance = 0;

ConfFileLocator::ConfFileLocator() {}

ConfFileLocator::~ConfFileLocator() {}

ConfFileLocator* ConfFileLocator::Instance()
{
    if(ms_instance == 0) { ms_instance = new ConfFileLocator(); }
    return ms_instance;
}

void ConfFileLocator::Release()
{
    if(ms_instance) { delete ms_instance; }
    ms_instance = 0;
}

void ConfFileLocator::Initialize(const wxString& installpath, const wxString& startuppath)
{
    m_installPath = installpath;
    m_startupPath = startuppath;
}

wxString ConfFileLocator::Locate(const wxString& baseName)
{
    wxFileName privateFile(GetLocalCopy(baseName));
    wxFileName defaultFile(GetDefaultCopy(baseName));

    if(privateFile.FileExists()) {
        return privateFile.GetFullPath();
    } else {
        return defaultFile.GetFullPath();
    }
}

void ConfFileLocator::DeleteLocalCopy(const wxString& basename)
{
    wxFileName privateFile(GetLocalCopy(basename));
    if(privateFile.FileExists()) { clRemoveFile(privateFile.GetFullPath()); }
}

wxString ConfFileLocator::GetLocalCopy(const wxString& baseName)
{
    return clStandardPaths::Get().GetUserDataDir() + wxT("/") + baseName;
}

wxString ConfFileLocator::GetDefaultCopy(const wxString& baseName)
{
    return m_installPath + wxT("/") + baseName + wxT(".default");
}
