#include "clTabRendererFirefox.hpp"

namespace
{
constexpr double TAB_RADIUS = 0.0;
}

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
    wxFont font = GetTabFont(false);
    fontDC.SetTextForeground(tabInfo.IsActive() ? colours.activeTabTextColour : colours.inactiveTabTextColour);
    fontDC.SetFont(font);

    wxRect rr = tabInfo.m_rect;
    wxString label = tabInfo.GetBestLabel(style);
    fontDC.DrawText(label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY + rr.GetY());
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
