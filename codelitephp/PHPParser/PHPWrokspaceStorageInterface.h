//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPWrokspaceStorageInterface.h
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

#ifndef PHPWROKSPACE_STORAGE_INTERFACE_H
#define PHPWROKSPACE_STORAGE_INTERFACE_H

#include <wx/filename.h>
#include <wx/string.h>
#include <vector>
#include <set>

struct FileInfo {
    wxFileName filename;
    wxString   folder;
    size_t     flags;
};
using FileArray_t = std::vector<FileInfo>;

#endif // PHPWROKSPACE_STORAGE_INTERFACE_H
