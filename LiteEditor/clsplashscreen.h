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

#include <wx/splash.h>
#ifdef __WXMSW__
#define SPLASH_STYLE wxSIMPLE_BORDER | wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP
#else
#define SPLASH_STYLE wxSIMPLE_BORDER | wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP
#endif

class clSplashScreen : public wxSplashScreen
{
public:
    clSplashScreen(const wxBitmap& bitmap,
                   long splashStyle,
                   int milliseconds,
                   wxWindow* parent,
                   wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = SPLASH_STYLE);
    virtual ~clSplashScreen();
    void OnCloseWindow(wxCloseEvent& event);
    static wxBitmap CreateSplashScreenBitmap(const wxBitmap& origBmp);
    static clSplashScreen* g_splashScreen;
    static bool g_destroyed;
};

#endif // CLSPLASHSCREEN_H
