//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_aui_dock_art.cpp
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

#include "cl_aui_dock_art.h"

#include "ColoursAndFontsManager.h"
#include "clStatusBar.h"
#include "clSystemSettings.h"
#include "clTabRenderer.h"
#include "cl_command_event.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"

#include <editor_config.h>
#include <wx/dcmemory.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>

// --------------------------------------------
namespace
{
bool IsRectOK(wxDC& dc, const wxRect& rect)
{
    const wxSize dc_size = dc.GetSize();

    if(0 > rect.x || 0 > rect.y || 0 >= rect.width || 0 >= rect.height || dc_size.GetWidth() < (rect.x + rect.width) ||
       dc_size.GetHeight() < (rect.y + rect.height))
        return (false);
    return (true);
}

wxString wxAuiChopText(wxDC& dc, const wxString& text, int max_size)
{
    wxCoord x, y;

    // first check if the text fits with no problems
    dc.GetTextExtent(text, &x, &y);
    if(x <= max_size)
        return text;

    size_t i, len = text.Length();
    size_t last_good_length = 0;
    for(i = 0; i < len; ++i) {
        wxString s = text.Left(i);
        s += wxT("...");

        dc.GetTextExtent(s, &x, &y);
        if(x > max_size)
            break;

        last_good_length = i;
    }

    wxString ret = text.Left(last_good_length);
    ret += wxT("...");
    return ret;
}
} // namespace

// ------------------------------------------------------------

clAuiDockArt::clAuiDockArt(IManager* manager)
    : m_manager(manager)
{
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clAuiDockArt::OnSettingsChanged, this);

    // initialise the colours to avoid assertion
    clCommandEvent dummy;
    OnSettingsChanged(dummy);
}

clAuiDockArt::~clAuiDockArt()
{
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clAuiDockArt::OnSettingsChanged, this);
}

void clAuiDockArt::DrawPaneButton(wxDC& dc, wxWindow* window, int button, int button_state, const wxRect& _rect,
                                  wxAuiPaneInfo& pane)
{
    wxRect buttonRect = _rect;

    if(!IsRectOK(dc, _rect))
        return;
    // Make sure that the height and width of the button are equals
    if(buttonRect.GetWidth() != buttonRect.GetHeight()) {
        buttonRect.SetHeight(wxMin(buttonRect.GetHeight(), buttonRect.GetWidth()));
        buttonRect.SetWidth(wxMin(buttonRect.GetHeight(), buttonRect.GetWidth()));
    } else {
        buttonRect.Deflate(1);
    }
    if(buttonRect.GetHeight() < 2) {
        // A wx3.0.x build may arrive here with a 1*1 rect -> a sigabort in libcairo
        return;
    }
    buttonRect = buttonRect.CenterIn(_rect);
    eButtonState buttonState = eButtonState::kNormal;
    switch(button_state) {
    case wxAUI_BUTTON_STATE_HOVER:
        buttonState = eButtonState::kHover;
        break;
    case wxAUI_BUTTON_STATE_PRESSED:
        buttonState = eButtonState::kPressed;
        break;
    case wxAUI_BUTTON_STATE_NORMAL:
    default:
        buttonState = eButtonState::kNormal;
        break;
    }

#ifdef __WXMSW__
    wxColour pen_colour = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    wxColour bg_colour = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
#else
    wxColour pen_colour = m_penColour;
    wxColour bg_colour = m_bgColour;
#endif

    // Prepare the colours
    switch(button) {
    case wxAUI_BUTTON_CLOSE:
        DrawingUtils::DrawButtonX(dc, window, buttonRect, pen_colour, bg_colour, buttonState);
        break;
    case wxAUI_BUTTON_MAXIMIZE_RESTORE:
        DrawingUtils::DrawButtonMaximizeRestore(dc, window, buttonRect, pen_colour, bg_colour, buttonState);
        break;
    default:
        // Make sure that the pane buttons are drawn with proper colours
        pane.state |= wxAuiPaneInfo::optionActive;
        wxAuiDefaultDockArt::DrawPaneButton(dc, window, button, button_state, _rect, pane);
        break;
    }
}

void clAuiDockArt::DrawCaption(wxDC& dc, wxWindow* window, const wxString& text, const wxRect& rect,
                               wxAuiPaneInfo& pane)
{
    if(!IsRectOK(dc, rect))
        return;

    wxRect tmpRect;
#if defined(__WXMAC__) || defined(__WXMSW__)
    window->PrepareDC(dc);

    tmpRect = rect;
    tmpRect.Inflate(2);

    // Prepare the colours
    wxFont f = DrawingUtils::GetDefaultGuiFont();

#if defined(__WXMSW__)
    dc.SetPen(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    dc.SetBrush(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    dc.SetTextForeground(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
#elif defined(__WXMAC__)
    f.SetFractionalPointSize(1.1 * f.GetFractionalPointSize());
    f.SetWeight(wxFONTWEIGHT_SEMIBOLD);
    dc.SetPen(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    dc.SetBrush(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    dc.SetTextForeground(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
#else
    f.SetFractionalPointSize(1.1 * f.GetFractionalPointSize());
    f.SetWeight(wxFONTWEIGHT_SEMIBOLD);
    dc.SetPen(clSystemSettings::GetDefaultPanelColour());
    dc.SetBrush(clSystemSettings::GetDefaultPanelColour());
    dc.SetTextForeground(m_captionTextColour);
#endif
    dc.SetFont(f);
    dc.DrawRectangle(tmpRect);

    wxCoord w, h;
    dc.GetTextExtent(wxT("ABCDEFHXfgkj"), &w, &h);

    wxRect clip_rect = tmpRect;
    clip_rect.width -= 3; // text offset
    clip_rect.width -= 2; // button padding
    if(pane.HasCloseButton()) {
        clip_rect.width -= m_buttonSize;
    }
    if(pane.HasPinButton()) {
        clip_rect.width -= m_buttonSize;
    }
    if(pane.HasMaximizeButton()) {
        clip_rect.width -= m_buttonSize;
    }

    wxString draw_text = wxAuiChopText(dc, text, clip_rect.width);
    wxRect text_rect = dc.GetTextExtent(draw_text);

    // on macOS, we center the caption text
    text_rect = text_rect.CenterIn(rect);
    dc.DrawText(draw_text, text_rect.GetTopLeft());

#else
    tmpRect = rect;
    tmpRect.SetPosition(wxPoint(0, 0));
    wxBitmap bmp(tmpRect.GetSize());
    {
        wxMemoryDC memDc;
        memDc.SelectObject(bmp);

        wxFont f = DrawingUtils::GetDefaultGuiFont();
        memDc.SetFont(f);

        // we inflate the rect by 1 to fix a one pixel glitch
        memDc.SetPen(*wxTRANSPARENT_PEN);
        memDc.SetBrush(m_captionColour);
        tmpRect.Inflate(2);
        memDc.DrawRectangle(tmpRect);

        int caption_offset = 5;
        wxRect clip_rect = tmpRect;
        clip_rect.width -= caption_offset; // text offset
        clip_rect.width -= 2;              // button padding
        if(pane.HasCloseButton())
            clip_rect.width -= m_buttonSize;
        if(pane.HasPinButton())
            clip_rect.width -= m_buttonSize;
        if(pane.HasMaximizeButton())
            clip_rect.width -= m_buttonSize;

        // Truncate the text if needed
        wxString draw_text = wxAuiChopText(memDc, text, clip_rect.width);
        wxSize textSize = memDc.GetTextExtent(draw_text);
        wxRect textRect(textSize);
        textRect = textRect.CenterIn(clip_rect, wxVERTICAL);
        textRect.SetX(caption_offset);

        memDc.SetTextForeground(m_captionTextColour);
        memDc.DrawText(draw_text, textRect.GetTopLeft());
        memDc.SelectObject(wxNullBitmap);
    }
    dc.DrawBitmap(bmp, rect.x, rect.y, true);
#endif
}

void clAuiDockArt::DrawBackground(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect)
{
#ifdef __WXMAC__
    dc.SetBrush(m_bgColour);
    dc.SetPen(m_bgColour);
    dc.DrawRectangle(rect);
#else
    wxRect tmpRect = rect;
    tmpRect.SetPosition(wxPoint(0, 0));
    wxBitmap bmp(tmpRect.GetSize());

    wxMemoryDC memDc;
    memDc.SelectObject(bmp);

    wxGCDC gdc(memDc);

    gdc.SetPen(m_bgColour);
    gdc.SetBrush(m_bgColour);
    gdc.DrawRectangle(tmpRect);
    memDc.SelectObject(wxNullBitmap);
    dc.DrawBitmap(bmp, rect.GetTopLeft(), true);
#endif
}

void clAuiDockArt::DrawBorder(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane)
{
    wxUnusedVar(dc);
    wxUnusedVar(window);
    wxUnusedVar(rect);
    wxUnusedVar(pane);
}

void clAuiDockArt::DrawSash(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect)
{
    wxUnusedVar(orientation);
    wxUnusedVar(window);
#ifdef __WXMSW__
    DrawingUtils::DrawStippleBackground(rect, dc);
#else
    dc.SetPen(m_bgColour);
    dc.SetBrush(m_bgColour);
    dc.DrawRectangle(rect);
#endif
}

void clAuiDockArt::OnSettingsChanged(clCommandEvent& event)
{
    event.Skip();
    m_bgColour = clSystemSettings::GetDefaultPanelColour();
    if(DrawingUtils::IsDark(m_bgColour)) {
        m_captionTextColour = wxColour(*wxWHITE).ChangeLightness(80);
    } else {
        m_captionTextColour = wxColour(*wxBLACK).ChangeLightness(120);
    }

    m_captionColour = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    m_captionTextColour = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    m_penColour = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);

    // trigger a refresh
    clGetManager()->GetDockingManager()->Update();
}
