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
#include <wx/dcscreen.h>
#include <wx/dcbuffer.h>
#include <wx/settings.h>

BEGIN_EVENT_TABLE(clSplashScreen, wxFrame)
    EVT_PAINT(clSplashScreen::OnPaint)
    EVT_ERASE_BACKGROUND(clSplashScreen::OnEraseBg)
END_EVENT_TABLE()

extern wxString CODELITE_VERSION_STR;

clSplashScreen::clSplashScreen(wxWindow* parent, const wxBitmap& bmp)
    : wxFrame(parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxFRAME_NO_TASKBAR)
{
    int w = bmp.GetWidth();
    int h = bmp.GetHeight();

    SetClientSize(w, h);
    CentreOnScreen();
    
    wxScreenDC screen_dc;
    wxMemoryDC label_dc;
    
    int x;
    int y;

#ifndef __WXMAC__    
    x = GetPosition().x;
    y = GetPosition().y;
#endif
    
    m_bmp = wxBitmap(w, h);
    
    label_dc.SelectObject(m_bmp);
#ifndef __WXMAC__     
    label_dc.Blit(0, 0, w, h, &screen_dc, x, y);
#else
    label_dc.SetBackground(*wxWHITE_BRUSH);
    label_dc.Clear();
#endif
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
    Show(true);
    SetThemeEnabled(false);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

#ifdef __WXMAC__
    Centre(wxBOTH | wxCENTRE_ON_SCREEN); // centre only works when the window is showing
#endif

#if defined(__WXMSW__) || defined(__WXMAC__)
    Update();
#else
    wxYieldIfNeeded();
#endif
    
    this->Connect(wxEVT_TIMER, wxTimerEventHandler(clSplashScreen::OnTimer), NULL, this);
    m_timer.SetOwner(this);
    m_timer.Start(4000, true);
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
