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
    const int tabRaius = 0;

    clTabColours colours = colors;
    bool isDark = DrawingUtils::IsDark(colours.activeTabBgColour);
    if(isDark) {
        InitDarkColours(colours, colours.activeTabBgColour);
    } else {
        InitLightColours(colours, colours.activeTabBgColour);
    }

    wxColour bgColour(tabInfo.IsActive() ? colours.activeTabBgColour : colours.inactiveTabBgColour);
    wxColour penColour(bgColour);

    if(isDark && !tabInfo.IsActive()) { bgColour = bgColour.ChangeLightness(105); }

    wxFont font = GetTabFont(false);
    wxColour activeTabTextColour = isDark ? colours.markerColour : colours.activeTabTextColour;
    fontDC.SetTextForeground(tabInfo.IsActive() ? activeTabTextColour : colours.inactiveTabTextColour);
    fontDC.SetFont(font);

    // Draw the tab rectangle
    wxRect tabRect = tabInfo.GetRect();
    tabRect.SetHeight(tabRect.GetHeight() + tabRaius);
    tabRect.SetY(tabRect.GetY() - tabRaius);
    tabRect.Inflate(1);

    {
        dc.SetPen(penColour);
        dc.SetBrush(bgColour);
        dc.DrawRoundedRectangle(tabRect, tabRaius);
    }

    // Draw bitmap
    if(tabInfo.GetBitmap().IsOk()) {
        const wxBitmap& bmp = (!tabInfo.IsActive() && tabInfo.GetDisabledBitmp().IsOk()) ? tabInfo.GetDisabledBitmp()
                                                                                         : tabInfo.GetBitmap();
        dc.DrawBitmap(bmp, tabInfo.m_bmpX + tabInfo.m_rect.GetX(), tabInfo.m_bmpY);
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
            DrawingUtils::TruncateText(label, newSize, dc, label);
        }
    }

    fontDC.DrawText(label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY + rr.GetY());
    if(style & kNotebook_CloseButtonOnActiveTab) { DrawButton(parent, dc, tabInfo, colours, buttonState); }
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
    wxUnusedVar(dc);
    wxUnusedVar(clientRect);
    wxUnusedVar(colours);
    wxUnusedVar(style);
}