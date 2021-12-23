//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ICompilerLocator.cpp
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

#include "ICompilerLocator.h"

#include "file_logger.h"
#include "macros.h"

#include <wx/tokenzr.h>
#include <wx/utils.h>

#ifdef __WXMSW__
#include <array>
#include <wx/msw/registry.h>
#endif

ICompilerLocator::ICompilerLocator() {}

ICompilerLocator::~ICompilerLocator() {}

wxArrayString ICompilerLocator::GetPaths() const
{
    wxString path;
    if(!::wxGetEnv("PATH", &path)) {
        clWARNING() << "Could not read environment variable PATH";
        return {};
    }

    wxArrayString mergedPaths;
    wxArrayString paths = ::wxStringTokenize(path, clPATH_SEPARATOR, wxTOKEN_STRTOK);
    return paths;
}

void ICompilerLocator::ScanUninstRegKeys()
{
#ifdef __WXMSW__
    static const std::array<wxString, 2> unInstKey = {
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
        "SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
    };
    static const std::array<wxRegKey::StdKey, 2> regBase = { wxRegKey::HKCU, wxRegKey::HKLM };

    for(size_t i = 0; i < regBase.size(); ++i) {
        for(size_t j = 0; j < unInstKey.size(); ++j) {
            wxRegKey regKey(regBase[i], unInstKey[j]);
            if(!regKey.Exists() || !regKey.Open(wxRegKey::Read))
                continue;

            size_t subkeys = 0;
            regKey.GetKeyInfo(&subkeys, NULL, NULL, NULL);
            wxString keyName;
            long keyIndex = 0;
            regKey.GetFirstKey(keyName, keyIndex);

            for(size_t k = 0; k < subkeys; ++k) {
                wxRegKey subKey(regKey, keyName);
                if(!subKey.Exists() || !subKey.Open(wxRegKey::Read))
                    continue;

                wxString displayName, installFolder, displayVersion;
                if(subKey.HasValue("DisplayName") && subKey.HasValue("InstallLocation") &&
                   subKey.HasValue("DisplayVersion") && subKey.QueryValue("DisplayName", displayName) &&
                   subKey.QueryValue("InstallLocation", installFolder) &&
                   subKey.QueryValue("DisplayVersion", displayVersion)) {
                    CheckUninstRegKey(displayName, installFolder, displayVersion);
                }

                subKey.Close();
                regKey.GetNextKey(keyName, keyIndex);
            }
            regKey.Close();
        }
    }
#endif
}
