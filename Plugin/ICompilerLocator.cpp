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
