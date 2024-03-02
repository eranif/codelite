#include "clTabRendererMinimal.hpp"

#include "clColours.h"
#include "clGenericNotebook.hpp"
#include "clSystemSettings.h"
#include "drawingutils.h"
#include "globals.h"
#include "imanager.h"

#include <wx/dcmemory.h>
#include <wx/font.h>
#include <wx/settings.h>
#include <wx/stc/stc.h>

#define DRAW_LINE(__p1, __p2) \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);

namespace
{
#ifdef __WXMAC__
constexpr int BOTTOM_PEN_LIGHNTESS = 80;
constexpr int SIDE_PEN_LIGHNTESS_WHEN_DARK = 115;
constexpr int SIDE_PEN_LIGHNTESS_WHEN_LIGHT = 70;
#else
constexpr int BOTTOM_PEN_LIGHNTESS = 80;
constexpr int SIDE_PEN_LIGHNTESS_WHEN_DARK = 115;
constexpr int SIDE_PEN_LIGHNTESS_WHEN_LIGHT = 80;
#endif

void GetTabColours(const clTabColours& colours, size_t style, wxColour* activeTabBgColour, wxColour* bgColour)
{
    *bgColour = colours.tabAreaColour;
    *activeTabBgColour = colours.activeTabBgColour;
}
} // namespace

clTabRendererMinimal::clTabRendererMinimal(const wxWindow* parent)
    : clTabRenderer("MINIMAL", parent)
{
    bottomAreaHeight = 0;
    smallCurveWidth = 0;
    majorCurveWidth = 0;
    overlapWidth = 0;
    verticalOverlapWidth = 0;
}

clTabRendererMinimal::~clTabRendererMinimal() {}

void clTabRendererMinimal::InitDarkColours(clTabColours& colours, const wxColour& activeTabBGColour)
{
    wxUnusedVar(colours);
    wxUnusedVar(activeTabBGColour);
}

void clTabRendererMinimal::InitLightColours(clTabColours& colours, const wxColour& activeTabBGColour)
{
    // Active tab
    wxUnusedVar(colours);
    wxUnusedVar(activeTabBGColour);
}

void clTabRendererMinimal::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo, size_t tabIndex,
                                size_t activeTabIndex, const clTabColours& colors, size_t style, eButtonState tabState,
                                eButtonState xButtonState)
{
    DoDraw(parent, dc, fontDC, tabInfo, tabIndex, activeTabIndex, colors, style, tabState, xButtonState);
}

void clTabRendererMinimal::FinaliseBackground(wxWindow* parent, wxDC& dc, const wxRect& clientRect,
                                              const wxRect& activeTabRect, const clTabColours& colours, size_t style)
{
    clTabRenderer::FinaliseBackground(parent, dc, clientRect, activeTabRect, colours, style);
}

void clTabRendererMinimal::DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t tabInfo, const wxRect& clientRect,
                                          wxDC& dc, const clTabColours& colors, size_t style)
{
    wxUnusedVar(parent);
    wxUnusedVar(tabInfo);
    wxUnusedVar(clientRect);
    wxUnusedVar(dc);
    wxUnusedVar(colors);
    wxUnusedVar(style);
}

void clTabRendererMinimal::DrawBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const clTabColours& colours,
                                          size_t style)
{
    wxUnusedVar(style);
    wxUnusedVar(colours);
    DrawingUtils::DrawTabBackgroundArea(dc, parent, rect);
}

wxRect clTabRendererMinimal::DoDraw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo, size_t tabIndex,
                                    size_t activeTabIndex, const clTabColours& colors, size_t style,
                                    eButtonState tabState, eButtonState xButtonState)
{
    wxRect tabRect = tabInfo.GetRect();
    if (tabIndex == 0) {
        // the first tab: move it 1 pixel to the left
        // so the left border is not drawn
        tabRect.x -= 1;
        tabRect.width += 1;
    }

    clTabColours colours = colors;

    wxDCTextColourChanger text_colour_changer(dc);

    wxColour penColour(tabInfo.IsActive() ? colours.activeTabPenColour : colours.inactiveTabPenColour);
    wxColour bgColour, activeTabBgColour;
    GetTabColours(colours, style, &activeTabBgColour, &bgColour);

    // update the is_dark flag now that we set a bgColour
    bool is_dark = DrawingUtils::IsDark(bgColour);

    // the visible tab is the part of the rect that is actually seen
    // on the tab area. the tabRect might have negative coordinates for
    // hiding unwanted curves parts of the rect
    // we use the visible tab for centering text
    wxRect visibleTab = tabRect;

#ifdef __WXMAC__
    visibleTab.y -= 1;
    visibleTab.height += 1;
#endif

    wxColour brush_colour = tabInfo.IsActive() ? activeTabBgColour : bgColour;
    switch (tabState) {
    case eButtonState::kHover:
        brush_colour = brush_colour.ChangeLightness(is_dark ? 105 : 95);
        break;
    default:
        break;
    }

    dc.SetPen(tabInfo.IsActive() ? activeTabBgColour : bgColour);
    dc.SetBrush(brush_colour);
    dc.DrawRoundedRectangle(tabRect, TAB_RADIUS);

    if (!tabInfo.IsActive() && (tabIndex + 1) != activeTabIndex /* not to the LEFT of the active tab */) {
        // draw a line at the bottom of the tab
        wxColour marker_colour = brush_colour.ChangeLightness(is_dark ? 105 : 95);
        wxDCPenChanger pc(dc, marker_colour);

        auto from = tabRect.GetRightTop();
        auto to = tabRect.GetRightBottom();

        dc.DrawLine(from, to);
    }

    // Draw bitmap
    if (tabInfo.HasBitmap()) {
        int bmpIndex =
            (!tabInfo.IsActive() && tabInfo.HasDisableBitmap()) ? tabInfo.GetDisabledBitmp() : tabInfo.GetBitmap();
        bool disabledBmp = (!tabInfo.IsActive() && tabInfo.HasDisableBitmap());
        const wxBitmap& bmp = tabInfo.GetBitmap(bmpIndex, disabledBmp);
        if (bmp.IsOk()) {
            dc.DrawBitmap(bmp, tabInfo.m_bmpX + tabInfo.m_rect.GetX(), tabInfo.m_bmpY);
        }
    }

    wxRect rr = tabInfo.m_rect;
    wxString label = tabInfo.GetBestLabel(style);
    // Check that the text can fit into the tab label
    int textEndCoord = tabInfo.m_textX + tabInfo.m_textWidth;
    int tabEndCoord = tabInfo.GetRect().GetRightTop().x;
    if ((textEndCoord + clTabRenderer::GetMarkerWidth()) > tabEndCoord) {
        int newSize = tabEndCoord - tabInfo.m_textX;
        wxString fixedLabel;
        DrawingUtils::TruncateText(label, newSize, fontDC, fixedLabel);
        label.swap(fixedLabel);
    }

    if (tabInfo.IsActive()) {
        wxPen bottom_pen = activeTabBgColour.ChangeLightness(BOTTOM_PEN_LIGHNTESS);
        wxPen side_pen =
            activeTabBgColour.ChangeLightness(is_dark ? SIDE_PEN_LIGHNTESS_WHEN_DARK : SIDE_PEN_LIGHNTESS_WHEN_LIGHT);
        {
            wxDCPenChanger pen_changer(dc, side_pen);
            dc.DrawLine(tabRect.GetTopLeft(), tabRect.GetBottomLeft());
            dc.DrawLine(tabRect.GetRightTop(), tabRect.GetRightBottom());
        }
        {
            wxDCPenChanger pen_changer(dc, bottom_pen);
            dc.DrawLine(tabRect.GetBottomLeft(), tabRect.GetBottomRight());
        }
    }

    wxFont font = GetTabFont(tabInfo.IsActive() && IsUseBoldFont());
    wxColour text_colour =
        is_dark ? (tabInfo.IsActive() ? *wxWHITE : bgColour.ChangeLightness(170)) : bgColour.ChangeLightness(30);

    // use distinct colour to mark modified tabs
    bool has_close_button = (style & kNotebook_CloseButtonOnActiveTab);
    if (!has_close_button && tabInfo.IsModified()) {
        // no close button, and a modified tab: use different colour for drawing
        // the tab label
        text_colour = is_dark ? "PINK" : "RED";
    }

    fontDC.SetTextForeground(text_colour);
    wxDCFontChanger font_changer(fontDC, font);

    wxRect textRect = fontDC.GetTextExtent(label);
    textRect = textRect.CenterIn(visibleTab, wxVERTICAL);
    textRect.SetX(tabInfo.m_textX + rr.GetX());
    fontDC.DrawText(label, textRect.GetTopLeft());

    if (has_close_button) {
        // use the adjusted tab rect and not the original one passed to us
        clTabInfo tab_info = tabInfo;
        tab_info.SetRect(tabRect);
        clTabColours c = colours;
        c.activeTabBgColour = activeTabBgColour;
        c.inactiveTabBgColour = bgColour;
        c.activeTabTextColour = text_colour;
        DrawButton(parent, dc, tab_info, c, xButtonState);
    }
    return tabRect;
}
