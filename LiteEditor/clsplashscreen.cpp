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
#if 0
#include <wx/dcscreen.h>
#include <wx/dcbuffer.h>
#include <wx/settings.h>

extern wxString CODELITE_VERSION_STR;

clSplashScreen::clSplashScreen(wxWindow* parent, const wxBitmap& bmp)
{
    wxMemoryDC label_dc;
    m_bmp = wxBitmap(bmp.GetWidth(), bmp.GetHeight());
    label_dc.SelectObject(m_bmp);
    label_dc.DrawBitmap(bmp, 0, 0, true);
    
    wxCoord ww, hh;
    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    font.SetPointSize(12);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    
    label_dc.SetFont(font);
    
    wxString versionString = CODELITE_VERSION_STR;
    //versionString = versionString.BeforeLast('-');
    
    label_dc.GetMultiLineTextExtent(versionString, &ww, &hh);
    wxCoord bmpW = bmp.GetWidth();
    label_dc.SetTextForeground( *wxWHITE );
    wxCoord textX = (bmpW - ww)/2;
    label_dc.DrawText(versionString, textX, 31);
    label_dc.SetTextForeground( wxColour("#003D00") );
    label_dc.DrawText(versionString, textX, 30);
    label_dc.SelectObject(wxNullBitmap);
    
    // m_bmp contains an updated version of the bitmap
    wxSplashScreen::Create()
    Show(true);
    SetThemeEnabled(false);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

clSplashScreen::~clSplashScreen()
{
}

void clSplashScreen::OnPaint(wxPaintEvent& e)
{
    wxUnusedVar(e);
    wxBufferedPaintDC dc(this);
    dc.DrawBitmap( m_bmp , 0, 0 , true);
}

void clSplashScreen::OnEraseBg(wxEraseEvent& e)
{
    wxUnusedVar(e);
}

void clSplashScreen::OnTimer(wxTimerEvent& e)
{
    wxUnusedVar(e);
    Hide();
}
#endif
