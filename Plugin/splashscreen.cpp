//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : splashscreen.cpp              
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
 /////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/splash.cpp
// Purpose:     SplashScreen class
// Author:      Julian Smart
// Modified by: Eran Ifrah
// Created:     28/6/2000
// RCS-ID:      $Id: splash.cpp,v 1.31 2006/10/30 19:41:23 VZ Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/dcbuffer.h>
#include "wx/wxprec.h"
#include "wx/splash.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SPLASH

#ifdef __WXGTK20__
    #include <gtk/gtk.h>
#endif

#include "splashscreen.h"
#include "wx/settings.h"

#ifndef WX_PRECOMP
    #include "wx/dcmemory.h"
    #include "wx/dcclient.h"
#endif

static int SplashID = wxNewId();

BEGIN_EVENT_TABLE(SplashScreen, wxFrame)
    //EVT_TIMER(SplashID, SplashScreen::OnNotify)
    EVT_CLOSE(SplashScreen::OnCloseWindow)
END_EVENT_TABLE()

/* Note that unless we pass a non-default size to the frame, SetClientSize
 * won't work properly under Windows, and the splash screen frame is sized
 * slightly too small.
 */

SplashScreen::SplashScreen(	const wxBitmap& bitmap, 
							const wxString &mainTitle,
							const wxString &subTitle,
							long splashStyle, 
							int milliseconds, 
							wxWindow* parent, 
							wxWindowID id, 
							const wxPoint& pos, 
							const wxSize& size, 
							long style):
    wxFrame(parent, id, wxEmptyString, wxPoint(0,0), wxSize(100, 100), style)
{
    // At least for GTK+ 2.0, this hint is not available.
#if defined(__WXGTK20__)
#if GTK_CHECK_VERSION(2,2,0)
    gtk_window_set_type_hint(GTK_WINDOW(m_widget),
                             GDK_WINDOW_TYPE_HINT_SPLASHSCREEN);
#endif
#endif

    m_window = NULL;
    m_splashStyle = splashStyle;
    m_milliseconds = milliseconds;

    m_window = new SplashScreenWindow(bitmap, mainTitle, subTitle, this, wxID_ANY, pos, size, wxNO_BORDER);

    SetClientSize(bitmap.GetWidth(), bitmap.GetHeight());

    if (m_splashStyle & wxSPLASH_CENTRE_ON_PARENT)
        CentreOnParent();
    else if (m_splashStyle & wxSPLASH_CENTRE_ON_SCREEN)
        CentreOnScreen();

    if (m_splashStyle & wxSPLASH_TIMEOUT)
    {
        m_timer.SetOwner(this, SplashID);
        m_timer.Start(milliseconds, true);
    }

    Show(true);
    m_window->SetFocus();
#if defined( __WXMSW__ ) || defined(__WXMAC__)
    Update(); // Without this, you see a blank screen for an instant
#else
    wxYieldIfNeeded(); // Should eliminate this
#endif
}

SplashScreen::~SplashScreen()
{
    m_timer.Stop();
}

void SplashScreen::OnNotify(wxTimerEvent& WXUNUSED(event))
{
    Close(true);
}

void SplashScreen::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    m_timer.Stop();
//  this->Destroy();
}

/*
 * SplashScreenWindow
 */

BEGIN_EVENT_TABLE(SplashScreenWindow, wxWindow)
#ifdef __WXGTK__
    EVT_PAINT(SplashScreenWindow::OnPaint)
#endif
    EVT_ERASE_BACKGROUND(SplashScreenWindow::OnEraseBackground)
    EVT_CHAR(SplashScreenWindow::OnChar)
    EVT_MOUSE_EVENTS(SplashScreenWindow::OnMouseEvent)
END_EVENT_TABLE()

SplashScreenWindow::SplashScreenWindow(const wxBitmap& bitmap, const wxString &mainTitle,
							const wxString &subTitle, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style):
    wxWindow(parent, id, pos, size, style)
{
    m_bitmap = bitmap; 
	m_mainTitle = mainTitle;
	m_subTitle = subTitle;
	
#if !defined(__WXGTK__) && wxUSE_PALETTE
    bool hiColour = (wxDisplayDepth() >= 16) ;

    if (bitmap.GetPalette() && !hiColour)
    {
        SetPalette(* bitmap.GetPalette());
    }
#endif

}

// VZ: why don't we do it under wxGTK?
#if !defined(__WXGTK__) && wxUSE_PALETTE
    #define USE_PALETTE_IN_SPLASH
#endif

static void wxDrawSplashBitmap(	wxDC& dc, 
								const wxBitmap& bitmap,
								const wxString &mainTitle,
								const wxString &subTitle)
{
    wxMemoryDC dcMem;

#ifdef USE_PALETTE_IN_SPLASH
    bool hiColour = (wxDisplayDepth() >= 16) ;

    if (bitmap.GetPalette() && !hiColour)
    {
        dcMem.SetPalette(* bitmap.GetPalette());
    }
#endif // USE_PALETTE_IN_SPLASH

    dcMem.SelectObjectAsSource(bitmap);
	//write the main title & subtitle
	wxCoord w, h, w1, h1;
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont smallfont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	font.SetPointSize(14);
	smallfont.SetPointSize(10);
	dcMem.SetFont(font);
	dcMem.GetMultiLineTextExtent(mainTitle, &w, &h);
	wxCoord bmpW = bitmap.GetWidth();
	wxCoord bmpH = bitmap.GetHeight();
	
	//draw shadow 
	dcMem.SetTextForeground(wxT("BLACK"));
	
	dcMem.DrawText(mainTitle, bmpW - w - 9, 11);
	//draw the text	
	dcMem.SetTextForeground(wxT("BLACK"));
	dcMem.SetFont(font);
	
	//draw the main title
	wxCoord textX = bmpW - w - 10;
	wxCoord textY = 10;
	dcMem.DrawText(mainTitle, textX, textY);
	
	//draw the subtitle
	dcMem.SetFont(smallfont);
	dcMem.SetTextForeground(wxT("WHITE"));
	dcMem.GetMultiLineTextExtent(subTitle, &w1, &h1);
	
	wxCoord stextX = textX + (w - w1)/2;
	wxCoord stextY = bmpH - h1 - 10;
	
	dcMem.DrawText(subTitle, stextX, stextY);
	
	dc.Blit(0, 0, bitmap.GetWidth(), bitmap.GetHeight(), & dcMem, 0, 0);
	dcMem.SelectObject(wxNullBitmap);

	//draw the subtitle
	
#ifdef USE_PALETTE_IN_SPLASH
    if (bitmap.GetPalette() && !hiColour)
    {
        dcMem.SetPalette(wxNullPalette);
    }
#endif // USE_PALETTE_IN_SPLASH
}

void SplashScreenWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxBufferedPaintDC dc(this);
	
//	wxRect rr = GetClientSize();
//	dc.SetPen( wxPen(wxT("WHITE")) );
//	dc.SetBrush( wxBrush(wxT("WHITE")) );
//	dc.DrawRectangle(rr);
//	
    if (m_bitmap.Ok()) {
        wxDrawSplashBitmap(dc, m_bitmap, m_mainTitle, m_subTitle);
	}
	
	
}

void SplashScreenWindow::OnEraseBackground(wxEraseEvent& event)
{
    if (event.GetDC())
    {
        if (m_bitmap.Ok())
        {
            wxDrawSplashBitmap(* event.GetDC(), m_bitmap, m_mainTitle, m_subTitle);
        }
    }
    else
    {
        wxClientDC dc(this);
        if (m_bitmap.Ok())
        {
            wxDrawSplashBitmap(dc, m_bitmap, m_mainTitle, m_subTitle);
        }
    }
	wxUnusedVar(event);
}

void SplashScreenWindow::OnMouseEvent(wxMouseEvent& event)
{
    if (event.LeftDown() || event.RightDown())
        GetParent()->Close(true);
}

void SplashScreenWindow::OnChar(wxKeyEvent& WXUNUSED(event))
{
    GetParent()->Close(true);
}

#endif // wxUSE_SPLASH
