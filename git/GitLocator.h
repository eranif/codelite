//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : GitLocator.h
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

#ifndef GITLOCATOR_H
#define GITLOCATOR_H

#include <wx/filename.h>

class GitLocator
{
    bool DoCheckGitInFolder(const wxString &folder, wxString& git) const;
public:
    GitLocator() = default;
    virtual ~GitLocator() = default;
    
    bool GetExecutable(wxFileName& gitpath) const;
    /**
     * @brief return the command required to open a git shell
     */
    bool MSWGetGitShellCommand(wxString& bashCommand) const;
};

#endif // GITLOCATOR_H
