//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : splashscreen.h              
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
// Name:        splash.h
// Purpose:     Splash screen class
// Author:      Julian Smart
// Modified by: Eran Ifrah
// Created:     28/6/2000
// RCS-ID:      $Id: splash.h,v 1.14 2006/09/05 20:45:25 VZ Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _SPLASHSCREEN_H_
#define _SPLASHSCREEN_H_

#include "wx/bitmap.h"
#include "wx/timer.h"
#include "wx/frame.h"
#include "wx/splash.h"

#define HIDE_SPLASH_EVENT_ID 48675

class  SplashScreenWindow;

/*
 * SplashScreen
 */

class  SplashScreen: public wxFrame
{
public:
    // for RTTI macros only
    SplashScreen() {}
    SplashScreen(  const wxBitmap& bitmap, const wxString &mainTitle,
				   const wxString &subTitle, long splashStyle, int milliseconds,
                   wxWindow* parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxSIMPLE_BORDER|wxFRAME_NO_TASKBAR|wxSTAY_ON_TOP);
				   
    virtual ~SplashScreen();

    void OnCloseWindow(wxCloseEvent& event);
    void OnNotify(wxTimerEvent& event);

    long GetSplashStyle() const { return m_splashStyle; }
    SplashScreenWindow* GetSplashWindow() const { return m_window; }
    int GetTimeout() const { return m_milliseconds; }

protected:
    SplashScreenWindow*   m_window;
    long                    m_splashStyle;
    int                     m_milliseconds;
    wxTimer                 m_timer;

    DECLARE_EVENT_TABLE()
};

/*
 * SplashScreenWindow
 */

class  SplashScreenWindow: public wxWindow
{
public:
    SplashScreenWindow(const wxBitmap& bitmap, const wxString &mainTitle,
							const wxString &subTitle, wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER);

    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnChar(wxKeyEvent& event);

    void SetBitmap(const wxBitmap& bitmap) { m_bitmap = bitmap; }
    wxBitmap& GetBitmap() { return m_bitmap; }

protected:
    wxBitmap    m_bitmap;
	wxString 	m_mainTitle;
	wxString 	m_subTitle;
	
    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(SplashScreenWindow)
};


#endif
    // _SPLASHSCREEN_H_
