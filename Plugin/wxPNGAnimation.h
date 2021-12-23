//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : wxPNGAnimation.h
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

#ifndef WXPNGANIMATION_H
#define WXPNGANIMATION_H

#include "codelite_exports.h"

#include <wx/bitmap.h>
#include <wx/colour.h>
#include <wx/panel.h>
#include <wx/timer.h>
#include <wx/vector.h>

class WXDLLIMPEXP_SDK wxPNGAnimation : public wxPanel
{
    wxVector<wxBitmap> m_bitmaps;
    size_t m_index;
    wxTimer* m_timer;
    wxColour m_bgColour;

protected:
    void OnPaint(wxPaintEvent& event);
    void OnEraseBG(wxEraseEvent& event);
    void OnTimer(wxTimerEvent& event);

public:
    /**
     * @brief construct a bitmap spinner from a bitmap map
     * The bitmap map must contains all bitmaps that consists the animation
     * Each bitmap in the map must have the same size (height and width)
     * @param parent parent window
     * @param pngSprite the bitmap map
     * @param spriteOrientation the sprite orientation (wxVERTICAL || wxHORIZONTAL)
     * @param singleImageSize the size of a single bitmap within the map
     * @param id the control ID
     */
    wxPNGAnimation(wxWindow* parent, const wxBitmap& pngSprite, wxOrientation spriteOrientation,
                   const wxSize& singleImageSize, wxWindowID id = wxID_ANY);
    virtual ~wxPNGAnimation();

    /**
     * @brief start the animation
     * @param refreshRate refresh the animation every milliseconds
     */
    void Start(int refereshRate);
    /**
     * @brief stop the animation
     */
    void Stop();

    /**
     * @brief set the animation background colour
     * @param colour
     */
    void SetPanelColour(const wxColour& colour);

    /**
     * @brief is the animation running?
     */
    bool IsRunning() const { return m_timer && m_timer->IsRunning(); }
};

#endif // WXPNGANIMATION_H
