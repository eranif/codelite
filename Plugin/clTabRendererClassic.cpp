#include "clTabRendererClassic.h"

#include "clColours.h"
#include "clSystemSettings.h"
#include "drawingutils.h"
#include <wx/dcmemory.h>
#include <wx/font.h>
#include <wx/settings.h>

#define DRAW_LINE(__p1, __p2) \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);

clTabRendererClassic::clTabRendererClassic(const wxWindow* parent)
    : clTabRenderer("MINIMAL", parent)
{
    bottomAreaHeight = 0;
    smallCurveWidth = 0;
    majorCurveWidth = 0;
    overlapWidth = 0;
    verticalOverlapWidth = 0;
}

clTabRendererClassic::~clTabRendererClassic() {}

void clTabRendererClassic::InitDarkColours(clTabColours& colours, const wxColour& activeTabBGColour)
{
    wxUnusedVar(colours);
    wxUnusedVar(activeTabBGColour);
}

void clTabRendererClassic::InitLightColours(clTabColours& colours, const wxColour& activeTabBGColour)
{
    // Active tab
    wxUnusedVar(colours);
    wxUnusedVar(activeTabBGColour);
}

void clTabRendererClassic::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                                const clTabColours& colors, size_t style, eButtonState buttonState)
{
    wxRect tabRect = tabInfo.GetRect();

    bool isBottom = style & kNotebook_BottomTabs;
    bool isLeft = style & kNotebook_LeftTabs;
    bool isRight = style & kNotebook_RightTabs;

    wxPoint p1, p2;
    if(isBottom) {
        p1 = tabRect.GetTopLeft();
        p2 = tabRect.GetTopRight();
    } else if(isLeft) {
        p1 = tabRect.GetTopRight();
        p2 = tabRect.GetBottomRight();
    } else if(isRight) {
        p1 = tabRect.GetTopLeft();
        p2 = tabRect.GetBottomLeft();
    } else {
        p1 = tabRect.GetBottomLeft();
        p2 = tabRect.GetBottomRight();
    }

    clTabColours colours = colors;
    bool isDark = DrawingUtils::IsDark(colours.activeTabBgColour);
    // if(isDark) {
    //     InitDarkColours(colours, colours.activeTabBgColour);
    // } else {
    //     InitLightColours(colours, colours.activeTabBgColour);
    // }

    wxColour bgColour(tabInfo.IsActive() ? colours.activeTabBgColour : colours.inactiveTabBgColour);
    wxColour penColour(tabInfo.IsActive() ? colours.activeTabPenColour : colours.inactiveTabPenColour);

    if(isDark && !tabInfo.IsActive()) {
        bgColour = bgColour.ChangeLightness(105);
    }

    wxFont font = GetTabFont(tabInfo.IsActive());
    wxColour activeTabTextColour = isDark ? colours.markerColour : colours.activeTabTextColour;
    fontDC.SetTextForeground(tabInfo.IsActive() ? activeTabTextColour : colours.inactiveTabTextColour);
    fontDC.SetFont(font);

    // Draw the tab rectangle
    if(IS_VERTICAL_TABS(style)) {
        tabRect.height += 1;
    } else {
        tabRect.width += 1;
        if(tabInfo.IsActive()) {
            tabRect.height += 1;
            if(isBottom) {
                tabRect.y -= 1;
            }
        }
    }

    dc.SetPen(penColour);
    dc.SetBrush(bgColour);
    dc.DrawRectangle(tabRect);

    // Draw bitmap
    if(tabInfo.HasBitmap()) {
        int bmpIndex =
            (!tabInfo.IsActive() && tabInfo.HasDisableBitmap()) ? tabInfo.GetDisabledBitmp() : tabInfo.GetBitmap();
        bool disabledBmp = !tabInfo.IsActive();
        dc.DrawBitmap(tabInfo.GetBitmap(bmpIndex, disabledBmp), tabInfo.m_bmpX + tabInfo.m_rect.GetX(), tabInfo.m_bmpY);
    }

    wxRect rr = tabInfo.m_rect;
    wxString label = tabInfo.GetBestLabel(style);
    bool bVerticalTabs = IS_VERTICAL_TABS(style);
    if(bVerticalTabs) {
        // Check that the text can fit into the tab label
        int textEndCoord = tabInfo.m_textX + tabInfo.m_textWidth;
        int tabEndCoord = tabInfo.GetRect().GetRightTop().x;
        if((textEndCoord + clTabRenderer::GetMarkerWidth()) > tabEndCoord) {
            int newSize = tabEndCoord - tabInfo.m_textX;
            wxString fixedLabel;
            DrawingUtils::TruncateText(label, newSize, fontDC, fixedLabel);
            label.swap(fixedLabel);
        }
    }

    fontDC.DrawText(label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY + rr.GetY());
    if(style & kNotebook_CloseButtonOnActiveTab) {
        DrawButton(parent, dc, tabInfo, colours, buttonState);
    }

    if(!tabInfo.IsActive()) {
        dc.SetPen(colours.activeTabPenColour);
        dc.DrawLine(p1, p2);
    }
}

void clTabRendererClassic::DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t tabInfo, const wxRect& clientRect,
                                          wxDC& dc, const clTabColours& colors, size_t style)
{
    wxUnusedVar(parent);
    wxUnusedVar(tabInfo);
    wxUnusedVar(clientRect);
    wxUnusedVar(dc);
    wxUnusedVar(colors);
    wxUnusedVar(style);
}

void clTabRendererClassic::DrawBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const clTabColours& colours,
                                          size_t style)
{
    clTabColours c = colours;
    if(DrawingUtils::IsDark(c.activeTabBgColour)) {
        InitDarkColours(c, c.activeTabBgColour);
    } else {
        InitLightColours(c, c.activeTabBgColour);
    }
    clTabRenderer::DrawBackground(parent, dc, rect, c, style);
}

void clTabRendererClassic::FinaliseBackground(wxWindow* parent, wxDC& dc, const wxRect& clientRect,
                                              const clTabColours& colours, size_t style)
{
    // wxUnusedVar(parent);
    // wxUnusedVar(dc);
    // wxUnusedVar(clientRect);
    // wxUnusedVar(style);
    // wxColour penColour(colours.activeTabPenColour);
    //
    // dc.SetPen(penColour);
    // dc.SetBrush(*wxTRANSPARENT_BRUSH);
    // dc.DrawRectangle(clientRect);
    //
    // dc.SetPen(colours.activeTabBgColour);
    // if(!IS_VERTICAL_TABS(style)) {
    //     if(style & kNotebook_BottomTabs) {
    //         DRAW_LINE(clientRect.GetTopLeft(), clientRect.GetTopRight());
    //     } else {
    //         DRAW_LINE(clientRect.GetBottomLeft(), clientRect.GetBottomRight());
    //     }
    // }
}
