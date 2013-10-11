//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : clsplashscreen.h
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

#ifndef CLSPLASHSCREEN_H
#define CLSPLASHSCREEN_H

#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/bitmap.h>

class clSplashScreen : public wxFrame
{
    wxBitmap m_bmp;
    wxTimer  m_timer;
    
public:
    clSplashScreen(wxWindow* parent, const wxBitmap& bmp);
    virtual ~clSplashScreen();
    
    DECLARE_EVENT_TABLE()
    void OnPaint(wxPaintEvent& e);
    void OnEraseBg(wxEraseEvent& e);
    void OnTimer(wxTimerEvent &e);
};

#endif // CLSPLASHSCREEN_H
