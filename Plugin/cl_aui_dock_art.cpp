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
#include "imanager.h"
#include <wx/dcmemory.h>
#include <wx/settings.h>
#include <editor_config.h>
#include "globals.h"
#include "cl_command_event.h"
#include "event_notifier.h"
#include "drawingutils.h"
#include "imanager.h"
#include "clStatusBar.h"
#include <wx/xrc/xmlres.h>
#include "codelite_events.h"
#include "clTabRenderer.h"

// --------------------------------------------

static wxString wxAuiChopText(wxDC& dc, const wxString& text, int max_size)
{
    wxCoord x, y;

    // first check if the text fits with no problems
    dc.GetTextExtent(text, &x, &y);
    if(x <= max_size) return text;

    size_t i, len = text.Length();
    size_t last_good_length = 0;
    for(i = 0; i < len; ++i) {
        wxString s = text.Left(i);
        s += wxT("...");

        dc.GetTextExtent(s, &x, &y);
        if(x > max_size) break;

        last_good_length = i;
    }

    wxString ret = text.Left(last_good_length);
    ret += wxT("...");
    return ret;
}

// ------------------------------------------------------------

clAuiDockArt::clAuiDockArt(IManager* manager)
    : m_manager(manager)
{
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &clAuiDockArt::OnSettingsChanged, this);
    m_useDarkColours = EditorConfigST::Get()->GetOptions()->IsTabColourDark();
    m_darkBgColour = wxColour("rgb(80,80,80)");
#ifdef __WXOSX__
    m_notebookTabAreaDarkBgColour = *wxBLACK;
#else
    m_notebookTabAreaDarkBgColour = wxColour("rgb(37,22,22)"); //.ChangeLightness(115);
#endif
    m_notebookTabAreaDarkBgColour = m_darkBgColour;

    if(m_useDarkColours) {
        m_dockCloseBmp = wxXmlResource::Get()->LoadBitmap("aui-close-white");
        m_dockMoreBmp = wxXmlResource::Get()->LoadBitmap("aui-more-white");
        m_dockExpandeBmp = wxXmlResource::Get()->LoadBitmap("aui-expand-white");
        m_dockMinimizeBmp = wxXmlResource::Get()->LoadBitmap("aui-minimize-white");
    } else {
        m_dockCloseBmp = wxXmlResource::Get()->LoadBitmap("aui-close");
        m_dockMoreBmp = wxXmlResource::Get()->LoadBitmap("aui-more");
        m_dockExpandeBmp = wxXmlResource::Get()->LoadBitmap("aui-expand");
        m_dockMinimizeBmp = wxXmlResource::Get()->LoadBitmap("aui-minimize");
    }
}

clAuiDockArt::~clAuiDockArt()
{
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &clAuiDockArt::OnSettingsChanged, this);
}

#define AUI_BUTTON_SIZE 12
void clAuiDockArt::DrawPaneButton(
    wxDC& dc, wxWindow* window, int button, int button_state, const wxRect& _rect, wxAuiPaneInfo& pane)
{
    int xx = _rect.GetTopLeft().x + ((_rect.GetWidth() - AUI_BUTTON_SIZE) / 2);
    int yy = _rect.GetTopLeft().y + ((_rect.GetHeight() - AUI_BUTTON_SIZE) / 2);
    switch(button) {
    case wxAUI_BUTTON_CLOSE:
        dc.DrawBitmap(m_dockCloseBmp, xx, yy);
        break;
    case wxAUI_BUTTON_MAXIMIZE_RESTORE:
        if(pane.IsMaximized()) {
            dc.DrawBitmap(m_dockMinimizeBmp, xx, yy);
        } else {
            dc.DrawBitmap(m_dockExpandeBmp, xx, yy);
        }
        break;
    case wxAUI_BUTTON_PIN:
        dc.DrawBitmap(m_dockMoreBmp, xx, yy);
        break;
    default:
        wxAuiDefaultDockArt::DrawPaneButton(dc, window, button, button_state, _rect, pane);
        break;
    }
}

void
clAuiDockArt::DrawCaption(wxDC& dc, wxWindow* window, const wxString& text, const wxRect& rect, wxAuiPaneInfo& pane)
{
    wxRect tmpRect(wxPoint(0, 0), rect.GetSize());

    // Hackishly prevent assertions on linux
    if(tmpRect.GetHeight() == 0) tmpRect.SetHeight(1);
    if(tmpRect.GetWidth() == 0) tmpRect.SetWidth(1);
#ifdef __WXOSX__
    tmpRect = rect;
    window->PrepareDC(dc);

    // Prepare the colours
    wxColour bgColour, penColour, textColour;
    textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE).ChangeLightness(90);

    penColour = bgColour;
    // Same as the notebook background colour?
    penColour = m_useDarkColours ? m_darkBgColour : penColour;
    bgColour = m_useDarkColours ? m_darkBgColour : bgColour;
    textColour = m_useDarkColours ? "WHITE" : textColour;

    wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    dc.SetFont(f);
    dc.SetPen(penColour);
    dc.SetBrush(bgColour);
    dc.DrawRectangle(tmpRect);

    // Fill the caption to look like OSX caption
    if(!m_useDarkColours) {
        dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
        dc.SetBrush(bgColour);
        dc.DrawRectangle(tmpRect);
    }
    int caption_offset = 0;
    if(pane.icon.IsOk()) {
        DrawIcon(dc, tmpRect, pane);
        caption_offset += pane.icon.GetWidth() + 3;
    } else {
        caption_offset = 3;
    }
    dc.SetTextForeground(textColour);
    wxCoord w, h;
    dc.GetTextExtent(wxT("ABCDEFHXfgkj"), &w, &h);

    wxRect clip_rect = tmpRect;
    clip_rect.width -= 3; // text offset
    clip_rect.width -= 2; // button padding
    if(pane.HasCloseButton()) clip_rect.width -= m_buttonSize;
    if(pane.HasPinButton()) clip_rect.width -= m_buttonSize;
    if(pane.HasMaximizeButton()) clip_rect.width -= m_buttonSize;

    wxString draw_text = wxAuiChopText(dc, text, clip_rect.width);
    wxSize textSize = dc.GetTextExtent(draw_text);

    dc.SetTextForeground(textColour);
    dc.DrawText(draw_text, tmpRect.x + 3 + caption_offset, tmpRect.y + ((tmpRect.height - textSize.y) / 2));
#else
    wxBitmap bmp(tmpRect.GetSize());
    {
        wxMemoryDC memDc;
        memDc.SelectObject(bmp);

        wxGCDC gdc;
        wxDC* pDC = NULL;
#ifdef __WXGTK__
        pDC = &memDc;
#else
        if(!DrawingUtils::GetGCDC(memDc, gdc)) {
            pDC = &memDc;
        } else {
            pDC = &gdc;
        }
#endif

        // Prepare the colours
        wxColour bgColour, penColour, textColour;
        if(!DrawingUtils::IsDark(DrawingUtils::GetPanelBgColour())) {
            bgColour = DrawingUtils::GetPanelBgColour().ChangeLightness(90);
            textColour = DrawingUtils::GetPanelTextColour();
        } else {
            textColour = DrawingUtils::GetPanelTextColour();
            bgColour = DrawingUtils::GetPanelBgColour().ChangeLightness(50);
        }
        
        // Same as the notebook background colour
        penColour = bgColour;
        penColour = m_useDarkColours ? m_darkBgColour : penColour;
        bgColour = m_useDarkColours ? m_darkBgColour : bgColour;
        textColour = m_useDarkColours ? "WHITE" : textColour;

        wxFont f = DrawingUtils::GetDefaultGuiFont();
        pDC->SetFont(f);
        pDC->SetPen(penColour);
        pDC->SetBrush(bgColour);
        pDC->DrawRectangle(tmpRect);

        pDC->SetPen(penColour);
        pDC->SetBrush(*wxTRANSPARENT_BRUSH);
        pDC->DrawRectangle(tmpRect);

        int caption_offset = 0;
        if(pane.icon.IsOk()) {
            DrawIcon(gdc, tmpRect, pane);
            caption_offset += pane.icon.GetWidth() + 3;
        } else {
            caption_offset = 3;
        }
        pDC->SetTextForeground(textColour);
        wxCoord w, h;
        pDC->GetTextExtent(wxT("ABCDEFHXfgkj"), &w, &h);

        wxRect clip_rect = tmpRect;
        clip_rect.width -= 3; // text offset
        clip_rect.width -= 2; // button padding
        if(pane.HasCloseButton()) clip_rect.width -= m_buttonSize;
        if(pane.HasPinButton()) clip_rect.width -= m_buttonSize;
        if(pane.HasMaximizeButton()) clip_rect.width -= m_buttonSize;

        wxString draw_text = wxAuiChopText(gdc, text, clip_rect.width);

        wxSize textSize = pDC->GetTextExtent(draw_text);
        pDC->SetTextForeground(textColour);
        pDC->DrawText(draw_text, tmpRect.x + 3 + caption_offset, tmpRect.y + ((tmpRect.height - textSize.y) / 2));
        memDc.SelectObject(wxNullBitmap);
    }
    dc.DrawBitmap(bmp, rect.x, rect.y, true);
#endif
}

void clAuiDockArt::DrawBackground(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect)
{
    wxUnusedVar(window);
    wxUnusedVar(orientation);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_useDarkColours ? m_notebookTabAreaDarkBgColour : DrawingUtils::GetPanelBgColour());
    dc.DrawRectangle(rect);
}

void clAuiDockArt::DrawBorder(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane)
{
    wxColour penColour = DrawingUtils::GetPanelBgColour();
    penColour = m_useDarkColours ? m_notebookTabAreaDarkBgColour : penColour;
    dc.SetPen(penColour);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(rect);
}

void clAuiDockArt::DrawSash(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect)
{
    if(m_useDarkColours) {
        wxUnusedVar(window);
        wxUnusedVar(orientation);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_notebookTabAreaDarkBgColour);
        dc.DrawRectangle(rect);
    } else {
        wxUnusedVar(window);
        wxUnusedVar(orientation);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(DrawingUtils::GetPanelBgColour());
        dc.DrawRectangle(rect);
    }
}

void clAuiDockArt::OnSettingsChanged(wxCommandEvent& event)
{
    event.Skip();
    m_useDarkColours = EditorConfigST::Get()->GetOptions()->IsTabColourDark();

    // update the bitmaps
    if(m_useDarkColours) {
        m_dockCloseBmp = wxXmlResource::Get()->LoadBitmap("aui-close-white");
        m_dockMoreBmp = wxXmlResource::Get()->LoadBitmap("aui-more-white");
        m_dockExpandeBmp = wxXmlResource::Get()->LoadBitmap("aui-expand-white");
        m_dockMinimizeBmp = wxXmlResource::Get()->LoadBitmap("aui-minimize-white");
    } else {
        m_dockCloseBmp = wxXmlResource::Get()->LoadBitmap("aui-close");
        m_dockMoreBmp = wxXmlResource::Get()->LoadBitmap("aui-more");
        m_dockExpandeBmp = wxXmlResource::Get()->LoadBitmap("aui-expand");
        m_dockMinimizeBmp = wxXmlResource::Get()->LoadBitmap("aui-minimize");
    }
}
