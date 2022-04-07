#include "clTabRendererFirefox.hpp"

#include "clSystemSettings.h"
#include "drawingutils.h"
#include "globals.h"
#include "imanager.h"

namespace
{
#if defined(__WXOSX__) || defined(__WXMSW__)
constexpr double TAB_RADIUS = 0.0;
#else
constexpr double TAB_RADIUS = 0.0;
#endif
} // namespace

clTabRendererFirefox::clTabRendererFirefox(const wxWindow* parent)
    : clTabRenderer("FIREFOX", parent)
{
}

clTabRendererFirefox::~clTabRendererFirefox() {}

clTabRenderer* clTabRendererFirefox::New(const wxWindow* parent) const { return new clTabRendererFirefox(parent); }

void clTabRendererFirefox::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                                const clTabColours& colours, size_t style, eButtonState buttonState)
{
    wxRect tabRect = tabInfo.GetRect();
    if(tabInfo.IsActive()) {
        DrawShadow(colours, tabRect, dc);
    }
    tabRect.Deflate(3);
    wxColour pen_colour = tabInfo.IsActive() ? colours.activeTabPenColour : colours.tabAreaColour;
    wxColour bg_colour = tabInfo.IsActive() ? colours.activeTabBgColour : colours.tabAreaColour;

    bool is_dark_colours = DrawingUtils::IsDark(colours.activeTabBgColour);
    bool is_light_colours = !is_dark_colours;

    if(is_dark_colours && tabInfo.IsActive()) {
        pen_colour = pen_colour.ChangeLightness(110);
    } else if(tabInfo.IsActive()) {
        pen_colour = *wxWHITE;
    }

    dc.SetPen(pen_colour);
    dc.SetBrush(bg_colour);
    dc.DrawRoundedRectangle(tabRect, TAB_RADIUS);

    // Draw bitmap
    if(tabInfo.HasBitmap()) {
        int bmpIndex =
            (!tabInfo.IsActive() && tabInfo.HasDisableBitmap()) ? tabInfo.GetDisabledBitmp() : tabInfo.GetBitmap();
        bool disabledBmp = (!tabInfo.IsActive() && tabInfo.HasDisableBitmap());
        const wxBitmap& bmp = tabInfo.GetBitmap(bmpIndex, disabledBmp);
        if(bmp.IsOk()) {
            dc.DrawBitmap(bmp, tabInfo.m_bmpX + tabInfo.m_rect.GetX(), tabInfo.m_bmpY);
        }
    }

    // text and close button
    wxFont font = GetTabFont(tabInfo.IsActive() && IsUseBoldFont());
    fontDC.SetFont(font);
    wxColour text_colour;
    if(is_light_colours) {
        text_colour = wxColour(*wxBLACK).ChangeLightness(tabInfo.IsActive() ? 130 : 140);
    } else {
        text_colour = wxColour(*wxWHITE).ChangeLightness(tabInfo.IsActive() ? 70 : 60);
    }
    fontDC.SetTextForeground(text_colour);

    wxRect rr = tabInfo.m_rect;
    wxString label = tabInfo.GetBestLabel(style);
    wxRect text_rect = fontDC.GetTextExtent(label);
    text_rect = text_rect.CenterIn(rr, wxVERTICAL);
    fontDC.DrawText(label, tabInfo.m_textX + rr.GetX(), text_rect.GetY());
    if(style & kNotebook_CloseButtonOnActiveTab) {
        DrawButton(parent, dc, tabInfo, colours, buttonState);
    }

    wxUnusedVar(parent);
}

void clTabRendererFirefox::DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect,
                                          wxDC& dc, const clTabColours& colours, size_t style)
{
    wxUnusedVar(parent);
    wxUnusedVar(activeTab);
    wxUnusedVar(clientRect);
    wxUnusedVar(dc);
    wxUnusedVar(colours);
    wxUnusedVar(style);
}

void clTabRendererFirefox::DrawShadow(const clTabColours& colours, const wxRect& rect, wxDC& dc)
{
    wxRect shadow_rect = rect;
    wxColour b1, b2;
    if(false && colours.IsDarkColours()) {
        b1 = colours.tabAreaColour.ChangeLightness(105);
        b2 = b1.ChangeLightness(105);
    } else {
        b1 = colours.tabAreaColour.ChangeLightness(95);
        b2 = b1.ChangeLightness(95);
    }
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    shadow_rect.Deflate(1);
    dc.SetPen(b1);
    dc.DrawRoundedRectangle(shadow_rect, TAB_RADIUS);

    shadow_rect.Deflate(1);
    dc.SetPen(b2);
    dc.DrawRoundedRectangle(shadow_rect, TAB_RADIUS);
}
