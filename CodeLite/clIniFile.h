//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clIniFile.h
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

#ifndef CLINIFILE_H
#define CLINIFILE_H

#include "codelite_exports.h"

#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/string.h>

class WXDLLIMPEXP_CL clIniFile : public wxFileConfig
{
public:
    /**
     * @brief create a INI file under iniFullPath.GetFullPath()
     */
    clIniFile(const wxFileName& iniFullPath);

    /**
     * @brief create a INI file under ~/.codelite/config/fullname
     * note that the fullname should include the file suffix
     */
    clIniFile(const wxString& fullname);

    virtual ~clIniFile();
};

#endif // CLINIFILE_H
