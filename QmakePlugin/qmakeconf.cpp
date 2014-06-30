//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : qmakeconf.cpp
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

#include "qmakeconf.h"

QmakeConf::QmakeConf(const wxString &confPath)
    : wxFileConfig(wxEmptyString, wxEmptyString, confPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE)
{
}

QmakeConf::~QmakeConf()
{
    Flush();
}

wxArrayString QmakeConf::GetAllConfigurations() const
{
    wxArrayString configs;
    wxString      group;
    long          idx;

    bool cont = GetFirstGroup(group, idx);
    while (cont) {
        configs.Add( group );
        cont = GetNextGroup(group, idx);
    }

    return configs;
}
