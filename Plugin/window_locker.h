//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : window_locker.h
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

#ifndef __WINDOW_LOCKER__H__
#define __WINDOW_LOCKER__H__

#include "codelite_exports.h"

#include <wx/window.h>

class WXDLLIMPEXP_SDK clWindowUpdateLocker
{
public:
    explicit clWindowUpdateLocker(wxWindow* win)
        : m_win(win)
    {
        if (m_win)
            m_win->Freeze();
    }
    ~clWindowUpdateLocker()
    {
        if (m_win)
            m_win->Thaw();
    }

private:
    wxWindow* m_win = nullptr;
};

#endif // __WINDOW_LOCKER__H__
