//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : clsplashscreen.cpp
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

#include "clsplashscreen.h"

clSplashScreen::clSplashScreen(bool& flag,
                               const wxBitmap& bitmap,
                               long splashStyle,
                               int milliseconds,
                               wxWindow* parent,
                               wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style)
    : wxSplashScreen(bitmap, splashStyle, milliseconds, parent, id, pos, size, style)
    , m_flag(flag)
{
    Bind(wxEVT_CLOSE_WINDOW, &clSplashScreen::OnCloseWindow, this);
}

void clSplashScreen::OnCloseWindow(wxCloseEvent& event)
{
    // mark the splash as "destroyed" so we won't destroy it again later
    m_flag = true;
    event.Skip();
}

clSplashScreen::~clSplashScreen() {}
