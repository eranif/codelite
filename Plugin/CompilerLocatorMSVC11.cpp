//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CompilerLocatorMSVC11.cpp
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

#include "CompilerLocatorMSVC11.h"
#include <wx/filename.h>

CompilerLocatorMSVC11::CompilerLocatorMSVC11()
{
}

CompilerLocatorMSVC11::~CompilerLocatorMSVC11()
{
}

bool CompilerLocatorMSVC11::Locate()
{
    m_compilers.clear();
    
    // We locate it by searching for the environment variable
    // %VS120COMNTOOLS%
    wxString path = ::wxGetenv("VS110COMNTOOLS");
    if ( path.IsEmpty() ) {
        return false;
    }
    
    // We found the installation path
    // Go to 1 folder up
    wxFileName masterPath(path, "");
    wxFileName binPath( masterPath );
    binPath.RemoveLastDir();
    binPath.RemoveLastDir();
    binPath.AppendDir("VC");
    binPath.AppendDir("bin");
    
    wxFileName cl(binPath.GetPath(), "cl.exe");
    if ( !cl.FileExists() ) {
        return false;
    }
    
    // Add the tools
    AddTools(masterPath.GetPath(), "Visual C++ 11");
    return true;
}
