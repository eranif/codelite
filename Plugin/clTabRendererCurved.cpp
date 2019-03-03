#include "clTabRendererCurved.h"
#include <wx/dcmemory.h>
#include <wx/font.h>
#include <wx/settings.h>

#define DRAW_LINE(__p1, __p2) \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);

clTabRendererCurved::clTabRendererCurved()
    : clTabRenderer("TRAPEZOID")
{
    bottomAreaHeight = 5;
    majorCurveWidth = 15;
    smallCurveWidth = 4;
    overlapWidth = 20;
    verticalOverlapWidth = 3;
}

clTabRendererCurved::~clTabRendererCurved() {}

void clTabRendererCurved::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                               const clTabColours& colours, size_t style, eButtonState buttonState)
{
    const int TOP_SMALL_HEIGHT = 0;
    wxColour bgColour(tabInfo.IsActive() ? colours.activeTabBgColour : colours.inactiveTabBgColour);
    wxColour penColour(tabInfo.IsActive() ? colours.activeTabPenColour : colours.inactiveTabPenColour);
    wxFont font = GetTabFont(false);
    fontDC.SetTextForeground(tabInfo.IsActive() ? colours.activeTabTextColour : colours.inactiveTabTextColour);
    fontDC.SetFont(font);

    if(style & kNotebook_BottomTabs) {
        // Bottom tabs
        {
            wxPoint points[6];
            points[0] = tabInfo.m_rect.GetTopLeft();

            points[1].x = points[0].x + majorCurveWidth;
            points[1].y = tabInfo.m_rect.GetBottomLeft().y - TOP_SMALL_HEIGHT;

            points[2].x = points[1].x + smallCurveWidth;
            points[2].y = points[1].y + TOP_SMALL_HEIGHT;

            points[3].x = points[0].x + (tabInfo.m_rect.GetWidth() - (majorCurveWidth + smallCurveWidth));
            points[3].y = points[2].y;

            points[4].x = points[3].x + smallCurveWidth;
            points[4].y = points[1].y;

            points[5] = tabInfo.m_rect.GetTopRight();

            dc.SetPen(penColour);
            dc.SetBrush(bgColour);
            dc.DrawPolygon(6, points);
        }
        {
            wxPoint points[6];
            points[0] = tabInfo.m_rect.GetTopLeft();
            points[0].x += 1;

            points[1].x = points[0].x + majorCurveWidth;
            points[1].y = tabInfo.m_rect.GetBottomLeft().y - TOP_SMALL_HEIGHT - 1;

            points[2].x = points[1].x + smallCurveWidth;
            points[2].y = points[1].y + TOP_SMALL_HEIGHT;

            points[3].x = points[0].x + (tabInfo.m_rect.GetWidth() - 2 - (majorCurveWidth + smallCurveWidth));
            points[3].y = points[2].y;

            points[4].x = points[3].x + smallCurveWidth;
            points[4].y = points[1].y;

            points[5] = tabInfo.m_rect.GetTopRight();
            points[5].x -= 2;

            dc.SetPen(tabInfo.IsActive() ? colours.activeTabInnerPenColour : colours.inactiveTabInnerPenColour);
            dc.SetBrush(bgColour);
            dc.DrawPolygon(6, points);
        }
    } else {
        // Default tabs (placed at the top)
        {
            wxPoint points[6];
            points[0] = tabInfo.m_rect.GetBottomLeft();

            points[1].x = points[0].x + majorCurveWidth;
            points[1].y = tabInfo.m_rect.GetLeftTop().y + TOP_SMALL_HEIGHT;

            points[2].x = points[1].x + smallCurveWidth;
            points[2].y = points[1].y - TOP_SMALL_HEIGHT;

            points[3].x = points[0].x + (tabInfo.m_rect.GetWidth() - (majorCurveWidth + smallCurveWidth));
            points[3].y = points[2].y;

            points[4].x = points[3].x + smallCurveWidth;
            points[4].y = points[3].y + TOP_SMALL_HEIGHT;

            points[5] = tabInfo.m_rect.GetBottomRight();

            dc.SetPen(penColour);
            dc.SetBrush(bgColour);
            dc.DrawPolygon(6, points);
        }

        {
            wxPoint points[6];
            points[0] = tabInfo.m_rect.GetBottomLeft();
            points[0].x += 1;

            points[1].x = points[0].x + majorCurveWidth;
            points[1].y = tabInfo.m_rect.GetLeftTop().y + TOP_SMALL_HEIGHT + 1;

            points[2].x = points[1].x + smallCurveWidth;
            points[2].y = points[1].y - TOP_SMALL_HEIGHT;

            points[3].x = points[0].x + (tabInfo.m_rect.GetWidth() - 2 - (majorCurveWidth + smallCurveWidth));
            points[3].y = points[2].y;

            points[4].x = points[3].x + smallCurveWidth;
            points[4].y = points[3].y + TOP_SMALL_HEIGHT;

            points[5] = tabInfo.m_rect.GetBottomRight();
            points[5].x -= 2;

            dc.SetPen(tabInfo.IsActive() ? colours.activeTabInnerPenColour : colours.inactiveTabInnerPenColour);
            dc.SetBrush(bgColour);
            dc.DrawPolygon(6, points);
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

void clTabRendererCurved::DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect,
                                         wxDC& dc, const clTabColours& colours, size_t style)
{

    wxPoint pt1, pt2;
    dc.SetPen(colours.activeTabPenColour);
    if(style & kNotebook_BottomTabs) {
        // bottom tabs
        pt1 = clientRect.GetTopLeft();
        pt2 = clientRect.GetTopRight();
        DRAW_LINE(pt1, pt2);

    } else {
        // Top tabs
        pt1 = clientRect.GetBottomLeft();
        pt2 = clientRect.GetBottomRight();
        DRAW_LINE(pt1, pt2);
    }

    ClearActiveTabExtraLine(activeTab, dc, colours, style);
}
