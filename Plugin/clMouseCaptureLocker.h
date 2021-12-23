//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clMouseCaptureLocker.h
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

#ifndef CLMOUSECAPTURELOCKER_H
#define CLMOUSECAPTURELOCKER_H

#include "codelite_exports.h"

#include <wx/window.h>

class WXDLLIMPEXP_SDK clMouseCaptureLocker
{
    wxWindow* m_win;

public:
    clMouseCaptureLocker();
    virtual ~clMouseCaptureLocker();

    /**
     * @brief capture the mouse (if not captured)
     * If this class already captured some other window mouse, release it before
     * capturing the mouse for the new window. If passing NULL, release any window captured by
     * this class and do nothing
     */
    void CaptureMouse(wxWindow* win);
};

#endif // CLMOUSECAPTURELOCKER_H
