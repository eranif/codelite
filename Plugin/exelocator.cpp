//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : exelocator.cpp
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
#include "exelocator.h"

#include <wx/filename.h>
#include <wx/tokenzr.h>

bool ExeLocator::Locate(const wxString& name, wxString& where)
{
    wxString filename = name;
    filename.Trim().Trim(false);
    if(filename.StartsWith("\"")) {
        filename = filename.Mid(1);
    }

    if(filename.EndsWith("\"")) {
        filename = filename.RemoveLast();
    }

    // Incase the name is a full path, just test for the file existance
    wxFileName fn(filename);
    if(fn.IsAbsolute() && fn.FileExists()) {
        where = name;
        return true;
    }

    // Check the path
    wxString path = wxGetenv("PATH");
    wxArrayString paths = ::wxStringTokenize(path, wxPATH_SEP, wxTOKEN_STRTOK);
    for(size_t i = 0; i < paths.size(); ++i) {
        wxFileName fnExe(paths.Item(i), fn.GetFullName());
        if(fnExe.FileExists()) {
            where = fnExe.GetFullPath();
            return true;
        }
    }
    return false;
}
