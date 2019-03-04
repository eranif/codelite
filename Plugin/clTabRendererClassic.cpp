#include "clTabRendererClassic.h"

#include "drawingutils.h"
#include <wx/dcmemory.h>
#include <wx/font.h>
#include <wx/settings.h>
#include "clSystemSettings.h"
#include "clColours.h"

#define DRAW_LINE(__p1, __p2) \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);

clTabRendererClassic::clTabRendererClassic()
    : clTabRenderer("DEFAULT")
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
    // Active tab
    clColours c;
    c.InitFromColour(activeTabBGColour.ChangeLightness(110));

    colours.activeTabTextColour = c.GetItemTextColour();
    colours.activeTabBgColour = activeTabBGColour;
    colours.activeTabPenColour = c.GetBorderColour();
    colours.activeTabInnerPenColour = c.GetBgColour();

    // Inactive tab
    colours.inactiveTabBgColour = c.GetBgColour();
    colours.inactiveTabTextColour = c.GetItemTextColour().ChangeLightness(90);
    colours.inactiveTabPenColour = c.GetBgColour();
    colours.inactiveTabInnerPenColour = c.GetBgColour();
    colours.tabAreaColour = c.GetBgColour();
}

void clTabRendererClassic::InitLightColours(clTabColours& colours, const wxColour& activeTabBGColour)
{
    // Active tab
    clColours c;
    c.InitFromColour(activeTabBGColour.ChangeLightness(90));

    colours.activeTabTextColour = c.GetItemTextColour();
    colours.activeTabBgColour = activeTabBGColour;
    colours.activeTabPenColour = c.GetBorderColour();
    colours.activeTabInnerPenColour = c.GetBgColour();

    // Inactive tab
    colours.inactiveTabBgColour = c.GetBgColour();
    colours.inactiveTabTextColour = c.GetGrayText();
    colours.inactiveTabPenColour = c.GetBgColour();
    colours.inactiveTabInnerPenColour = c.GetBgColour();
    colours.tabAreaColour = c.GetBgColour();
}

void clTabRendererClassic::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                                const clTabColours& colors, size_t style, eButtonState buttonState)
{
    const int tabRaius = 1.5;

    clTabColours colours = colors;
    if(DrawingUtils::IsDark(colours.activeTabBgColour)) {
        InitDarkColours(colours, colours.activeTabBgColour);
    } else {
        InitLightColours(colours, colours.activeTabBgColour);
    }

    wxColour bgColour(tabInfo.IsActive() ? colours.activeTabBgColour : colours.inactiveTabBgColour);
    wxColour penColour(tabInfo.IsActive() ? colours.activeTabPenColour : colours.inactiveTabPenColour);
    wxFont font = GetTabFont(false);
    fontDC.SetTextForeground(tabInfo.IsActive() ? colours.activeTabTextColour : colours.inactiveTabTextColour);
    fontDC.SetFont(font);

    if(style & kNotebook_BottomTabs) {
        // Bottom tabs
        wxRect tabRect = tabInfo.GetRect();
        tabRect.SetHeight(tabRect.GetHeight() + tabRaius);
        tabRect.SetY(tabRect.GetY() - tabRaius);
        {
            dc.SetPen(penColour);
            dc.SetBrush(bgColour);
            dc.DrawRoundedRectangle(tabRect, tabRaius);
        }
        {
            tabRect.Deflate(1); // The inner border
            dc.SetPen(tabInfo.IsActive() ? colours.activeTabInnerPenColour : colours.inactiveTabInnerPenColour);
            dc.SetBrush(bgColour);
            dc.DrawRoundedRectangle(tabRect, tabRaius);
        }
    } else {
        // Default tabs (placed at the top)
        wxRect tabRect = tabInfo.GetRect();
        tabRect.SetHeight(tabRect.GetHeight() + tabRaius);
        {
            dc.SetPen(penColour);
            dc.SetBrush(bgColour);
            dc.DrawRoundedRectangle(tabRect, tabRaius);
        }
        {
            tabRect.Deflate(1); // The inner border
            dc.SetPen(tabInfo.IsActive() ? colours.activeTabInnerPenColour : colours.inactiveTabInnerPenColour);
            dc.SetBrush(bgColour);
            dc.DrawRoundedRectangle(tabRect, tabRaius);
        }
    }

    // Draw bitmap
    if(tabInfo.GetBitmap().IsOk()) {
        const wxBitmap& bmp = (!tabInfo.IsActive() && tabInfo.GetDisabledBitmp().IsOk()) ? tabInfo.GetDisabledBitmp()
                                                                                         : tabInfo.GetBitmap();
        dc.DrawBitmap(bmp, tabInfo.m_bmpX + tabInfo.m_rect.GetX(), tabInfo.m_bmpY);
    }
    fontDC.DrawText(tabInfo.m_label, tabInfo.m_textX + tabInfo.m_rect.GetX(), tabInfo.m_textY);
    if(tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
        DrawButton(parent, dc, tabInfo, colours, buttonState);
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
    wxUnusedVar(parent);
    clTabColours c = colours;
    if(DrawingUtils::IsDark(c.activeTabBgColour)) {
        InitDarkColours(c, c.activeTabBgColour);
    } else {
        InitLightColours(c, c.activeTabBgColour);
    }

    dc.SetPen(c.activeTabPenColour);
    if(style & kNotebook_BottomTabs) {
        dc.DrawLine(clientRect.GetTopLeft(), clientRect.GetTopRight());
    } else {
        dc.DrawLine(clientRect.GetBottomLeft(), clientRect.GetBottomRight());
    }
}