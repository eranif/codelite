//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPUserWorkspace.h
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

#ifndef PHPUSERWORKSPACE_H
#define PHPUSERWORKSPACE_H

#include "XDebugBreakpoint.h"
#include <wx/filename.h>

class PHPUserWorkspace
{
    XDebugBreakpoint::List_t m_breakpoints;
    wxString                 m_workspacePath;
    
protected:
    wxFileName GetFileName() const;

public:
    PHPUserWorkspace(const wxString &workspacePath);
    virtual ~PHPUserWorkspace() = default;

    PHPUserWorkspace& Load();
    PHPUserWorkspace& Save();
    PHPUserWorkspace& SetBreakpoints(const XDebugBreakpoint::List_t& breakpoints) {
        this->m_breakpoints = breakpoints;
        return *this;
    }
    const XDebugBreakpoint::List_t& GetBreakpoints() const {
        return m_breakpoints;
    }
};

#endif // PHPUSERWORKSPACE_H
