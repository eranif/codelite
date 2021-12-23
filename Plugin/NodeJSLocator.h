//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSLocator.h
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

#ifndef NODEJSLOCATOR_H
#define NODEJSLOCATOR_H

#include "codelite_exports.h"

#include <wx/arrstr.h>
#include <wx/filename.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK NodeJSLocator
{
    wxString m_nodejs;
    wxString m_npm;

protected:
    bool TryPaths(const wxArrayString& paths, const wxString& fullname, wxFileName& fullpath);

public:
    NodeJSLocator();
    ~NodeJSLocator();

    /**
     * @brief attempt to auto locate nodejs installation
     */
    void Locate(const wxArrayString& hints = {});

    const wxString& GetNodejs() const { return m_nodejs; }
    const wxString& GetNpm() const { return m_npm; }
};

#endif // NODEJSLOCATOR_H
