//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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
#include "autoversion.h"
#include <wx/dcmemory.h>
#include <wx/settings.h>

clSplashScreen* clSplashScreen::g_splashScreen = NULL;
bool clSplashScreen::g_destroyed = false;

clSplashScreen::clSplashScreen(const wxBitmap& bitmap,
                               long splashStyle,
                               int milliseconds,
                               wxWindow* parent,
                               wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style)
    : wxSplashScreen(bitmap, splashStyle, milliseconds, parent, id, pos, size, style)
{
    Bind(wxEVT_CLOSE_WINDOW, &clSplashScreen::OnCloseWindow, this);
}

void clSplashScreen::OnCloseWindow(wxCloseEvent& event)
{
    // mark the splash as "destroyed" so we won't destroy it again later
    g_destroyed = true;
    event.Skip();
}

clSplashScreen::~clSplashScreen() {}

wxBitmap clSplashScreen::CreateSplashScreenBitmap(const wxBitmap& origBmp)
{
    wxBitmap bmp;
    wxMemoryDC memDC;
    
    bmp = wxBitmap(origBmp.GetWidth(), origBmp.GetHeight());
    memDC.SelectObject(bmp);
    memDC.SetBrush(wxColour(63, 80, 24));
    memDC.SetPen(wxColour(63, 80, 24));
    memDC.DrawRectangle(0, 0, origBmp.GetWidth(), origBmp.GetHeight());
    memDC.DrawBitmap(origBmp, 0, 0, true);
    memDC.SetPen(*wxWHITE);
    memDC.SetBrush(*wxTRANSPARENT_BRUSH);
    memDC.DrawRectangle(0, 0, origBmp.GetWidth(), origBmp.GetHeight());

    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    font.SetPointSize(14);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    
    memDC.SetFont(font);
    wxString versionString;
    versionString << "v" << CODELITE_VERSION_STRING;
    wxSize textSize = memDC.GetTextExtent(versionString);
    wxCoord textx, texty;
    textx = (bmp.GetWidth() - textSize.GetWidth()) - 5;
    texty = 5;
    memDC.DrawText(versionString, textx, texty);
    memDC.SelectObject(wxNullBitmap);
    return bmp;
}
