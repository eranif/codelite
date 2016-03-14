//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_aui_tb_are.cpp
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

#include "cl_aui_tb_are.h"
#include <wx/settings.h>
#include "editor_config.h"
#include "drawingutils.h"
#include "codelite_events.h"
#include "event_notifier.h"

// these constants were copied from src/aui/auibar.cpp
const int BUTTON_DROPDOWN_WIDTH = 10;
static const unsigned char DISABLED_TEXT_GREY_HUE = wxColour::AlphaBlend(0, 255, 0.4);
const wxColour DISABLED_TEXT_COLOR(DISABLED_TEXT_GREY_HUE, DISABLED_TEXT_GREY_HUE, DISABLED_TEXT_GREY_HUE);

#if 0
static wxBitmap CreateDisabledBitmap(const wxBitmap& bmp)
{
    wxImage img = bmp.ConvertToImage();
    img = img.ConvertToGreyscale();
    wxBitmap greyBmp(img);
    if(DrawingUtils::IsThemeDark()) {
        return greyBmp.ConvertToDisabled(70);

    } else {
        return greyBmp.ConvertToDisabled(150);
    }
}
#endif

CLMainAuiTBArt::CLMainAuiTBArt()
{
    EventNotifier::Get()->Connect(
        wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(CLMainAuiTBArt::OnThemeChanged), NULL, this);
}

CLMainAuiTBArt::~CLMainAuiTBArt()
{
    EventNotifier::Get()->Disconnect(
        wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(CLMainAuiTBArt::OnThemeChanged), NULL, this);
}

void CLMainAuiTBArt::DrawPlainBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
#ifdef __WXMSW__
    wxAuiDefaultToolBarArt::DrawPlainBackground(dc, wnd, rect);
#else
    wxUnusedVar(wnd);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(DrawingUtils::GetAUIPaneBGColour());
    dc.DrawRectangle(rect);
#endif
}

void CLMainAuiTBArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
#ifdef __WXMSW__
    wxAuiDefaultToolBarArt::DrawBackground(dc, wnd, rect);
#else
    DrawPlainBackground(dc, wnd, rect);
#endif
}

void CLMainAuiTBArt::DrawButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect)
{
    wxAuiDefaultToolBarArt::DrawButton(dc, wnd, item, rect);
#if 0
    // if(!DrawingUtils::IsThemeDark()) {
    //     wxAuiDefaultToolBarArt::DrawButton(dc, wnd, item, rect);
    //     return;
    // }

    int textWidth = 0, textHeight = 0;

    if(m_flags & wxAUI_TB_TEXT) {
        dc.SetFont(m_font);

        int tx, ty;

        dc.GetTextExtent(wxT("ABCDHgj"), &tx, &textHeight);
        textWidth = 0;
        dc.GetTextExtent(item.GetLabel(), &textWidth, &ty);
    }

    int bmpX = 0, bmpY = 0;
    int textX = 0, textY = 0;

    if(m_textOrientation == wxAUI_TBTOOL_TEXT_BOTTOM) {
        bmpX = rect.x + (rect.width / 2) - (item.GetBitmap().GetWidth() / 2);

        bmpY = rect.y + ((rect.height - textHeight) / 2) - (item.GetBitmap().GetHeight() / 2);

        textX = rect.x + (rect.width / 2) - (textWidth / 2) + 1;
        textY = rect.y + rect.height - textHeight - 1;
    } else if(m_textOrientation == wxAUI_TBTOOL_TEXT_RIGHT) {
        bmpX = rect.x + 3;

        bmpY = rect.y + (rect.height / 2) - (item.GetBitmap().GetHeight() / 2);

        textX = bmpX + 3 + item.GetBitmap().GetWidth();
        textY = rect.y + (rect.height / 2) - (textHeight / 2);
    }

    if(!(item.GetState() & wxAUI_BUTTON_STATE_DISABLED)) {
        if(item.GetState() & wxAUI_BUTTON_STATE_PRESSED) {
            dc.SetPen(wxPen(m_highlightColour));
            dc.SetBrush(wxBrush(m_highlightColour.ChangeLightness(150)));
            dc.DrawRectangle(rect);
        } else if((item.GetState() & wxAUI_BUTTON_STATE_HOVER) || item.IsSticky()) {
            dc.SetPen(wxPen(m_highlightColour));
            dc.SetBrush(wxBrush(m_highlightColour.ChangeLightness(170)));

            // draw an even lighter background for checked item hovers (since
            // the hover background is the same color as the check background)
            if(item.GetState() & wxAUI_BUTTON_STATE_CHECKED)
                dc.SetBrush(wxBrush(m_highlightColour.ChangeLightness(180)));

            dc.DrawRectangle(rect);
        } else if(item.GetState() & wxAUI_BUTTON_STATE_CHECKED) {
            // it's important to put this code in an else statement after the
            // hover, otherwise hovers won't draw properly for checked items
            dc.SetPen(wxPen(m_highlightColour));
            dc.SetBrush(wxBrush(m_highlightColour.ChangeLightness(170)));
            dc.DrawRectangle(rect);
        }
    }

    wxBitmap bmp;
    if(item.GetState() & wxAUI_BUTTON_STATE_DISABLED) {
        bmp = CreateDisabledBitmap(item.GetBitmap());
    } else
        bmp = item.GetBitmap();

    if(bmp.IsOk()) dc.DrawBitmap(bmp, bmpX, bmpY, true);

    // set the item's text color based on if it is disabled
    dc.SetTextForeground(*wxBLACK);
    if(item.GetState() & wxAUI_BUTTON_STATE_DISABLED) dc.SetTextForeground(DISABLED_TEXT_COLOR);

    if((m_flags & wxAUI_TB_TEXT) && !item.GetLabel().empty()) {
        dc.DrawText(item.GetLabel(), textX, textY);
    }
#endif
}

void CLMainAuiTBArt::DrawDropDownButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect)
{
    wxAuiDefaultToolBarArt::DrawDropDownButton(dc, wnd, item, rect);
#if 0
    // Dark theme
    int textWidth = 0, textHeight = 0, textX = 0, textY = 0;
    int bmpX = 0, bmpY = 0, dropBmpX = 0, dropBmpY = 0;

    wxRect buttonRect = wxRect(rect.x, rect.y, rect.width - BUTTON_DROPDOWN_WIDTH, rect.height);
    wxRect dropDownRect =
        wxRect(rect.x + rect.width - BUTTON_DROPDOWN_WIDTH - 1, rect.y, BUTTON_DROPDOWN_WIDTH + 1, rect.height);

    if(m_flags & wxAUI_TB_TEXT) {
        dc.SetFont(m_font);

        int tx, ty;
        if(m_flags & wxAUI_TB_TEXT) {
            dc.GetTextExtent(wxT("ABCDHgj"), &tx, &textHeight);
            textWidth = 0;
        }

        dc.GetTextExtent(item.GetLabel(), &textWidth, &ty);
    }

    dropBmpX = dropDownRect.x + (dropDownRect.width / 2) - (m_buttonDropDownBmp.GetWidth() / 2);
    dropBmpY = dropDownRect.y + (dropDownRect.height / 2) - (m_buttonDropDownBmp.GetHeight() / 2);

    if(m_textOrientation == wxAUI_TBTOOL_TEXT_BOTTOM) {
        bmpX = buttonRect.x + (buttonRect.width / 2) - (item.GetBitmap().GetWidth() / 2);
        bmpY = buttonRect.y + ((buttonRect.height - textHeight) / 2) - (item.GetBitmap().GetHeight() / 2);

        textX = rect.x + (rect.width / 2) - (textWidth / 2) + 1;
        textY = rect.y + rect.height - textHeight - 1;
    } else if(m_textOrientation == wxAUI_TBTOOL_TEXT_RIGHT) {
        bmpX = rect.x + 3;

        bmpY = rect.y + (rect.height / 2) - (item.GetBitmap().GetHeight() / 2);

        textX = bmpX + 3 + item.GetBitmap().GetWidth();
        textY = rect.y + (rect.height / 2) - (textHeight / 2);
    }

    if(item.GetState() & wxAUI_BUTTON_STATE_PRESSED) {
        dc.SetPen(wxPen(m_highlightColour));
        dc.SetBrush(wxBrush(m_highlightColour.ChangeLightness(140)));
        dc.DrawRectangle(buttonRect);

        dc.SetBrush(wxBrush(m_highlightColour.ChangeLightness(170)));
        dc.DrawRectangle(dropDownRect);
    } else if(item.GetState() & wxAUI_BUTTON_STATE_HOVER || item.IsSticky()) {
        dc.SetPen(wxPen(m_highlightColour));
        dc.SetBrush(wxBrush(m_highlightColour.ChangeLightness(170)));
        dc.DrawRectangle(buttonRect);
        dc.DrawRectangle(dropDownRect);
    } else if(item.GetState() & wxAUI_BUTTON_STATE_CHECKED) {
        // Notice that this branch must come after the hover one to ensure the
        // correct appearance when the mouse hovers over a checked item.m_
        dc.SetPen(wxPen(m_highlightColour));
        dc.SetBrush(wxBrush(m_highlightColour.ChangeLightness(170)));
        dc.DrawRectangle(buttonRect);
        dc.DrawRectangle(dropDownRect);
    }

    wxBitmap bmp;
    wxBitmap dropbmp;
    if(item.GetState() & wxAUI_BUTTON_STATE_DISABLED) {
        bmp = CreateDisabledBitmap(item.GetBitmap());
        dropbmp = m_disabledButtonDropDownBmp;
    } else {
        bmp = item.GetBitmap();
        dropbmp = m_buttonDropDownBmp;
    }

    if(!bmp.IsOk()) return;

    dc.DrawBitmap(bmp, bmpX, bmpY, true);
    dc.DrawBitmap(dropbmp, dropBmpX, dropBmpY, true);

    // set the item's text color based on if it is disabled
    dc.SetTextForeground(*wxBLACK);
    if(item.GetState() & wxAUI_BUTTON_STATE_DISABLED) dc.SetTextForeground(DISABLED_TEXT_COLOR);

    if((m_flags & wxAUI_TB_TEXT) && !item.GetLabel().empty()) {
        dc.DrawText(item.GetLabel(), textX, textY);
    }
#endif
}

void CLMainAuiTBArt::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
#if 0
    if(DrawingUtils::IsThemeDark()) {
        m_gripperPen3 = *wxBLACK;
    } else {
        m_gripperPen3 = *wxWHITE;
    }
    m_baseColour = DrawingUtils::GetAUIPaneBGColour();
#endif
}
