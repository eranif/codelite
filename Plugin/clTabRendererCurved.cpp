#include "clTabRendererCurved.h"
#if !USE_AUI_NOTEBOOK
#include <wx/dcmemory.h>
#include <wx/font.h>
#include <wx/settings.h>

#define DRAW_LINE(__p1, __p2) \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);

clTabRendererCurved::clTabRendererCurved()
{
    bottomAreaHeight = 5;
    majorCurveWidth = 15;
    smallCurveWidth = 4;
    overlapWidth = 20;
    verticalOverlapWidth = 3;
}

clTabRendererCurved::~clTabRendererCurved() {}

void clTabRendererCurved::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                               const clTabColours& colours, size_t style)
{
    const int TOP_SMALL_HEIGHT = 0;
    wxColour bgColour(tabInfo.IsActive() ? colours.activeTabBgColour : colours.inactiveTabBgColour);
    wxColour penColour(tabInfo.IsActive() ? colours.activeTabPenColour : colours.inactiveTabPenColour);
    wxFont font = GetTabFont();
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
    } else if(IS_VERTICAL_TABS(style)) {

        // Left side tabs
        wxRect rotatedRect(0, 0, tabInfo.m_rect.GetHeight(), tabInfo.m_rect.GetWidth());
        wxBitmap b(rotatedRect.GetSize());

        wxMemoryDC tmpDC(b);
        tmpDC.SetPen(colours.tabAreaColour);
        tmpDC.SetBrush(colours.tabAreaColour);
        tmpDC.DrawRectangle(rotatedRect);
        tmpDC.SetFont(font);
        tmpDC.SetTextForeground(tabInfo.IsActive() ? colours.activeTabTextColour : colours.inactiveTabTextColour);

        tmpDC.SetPen(penColour);
        tmpDC.SetBrush(bgColour);
        if(tabInfo.IsActive()) {
            {
                wxPoint points[6];
                points[0] = rotatedRect.GetBottomLeft();
                points[1].x = points[0].x + majorCurveWidth;
                points[1].y = rotatedRect.GetLeftTop().y + TOP_SMALL_HEIGHT;

                points[2].x = points[1].x + smallCurveWidth;
                points[2].y = points[1].y - TOP_SMALL_HEIGHT;

                points[3].x = points[0].x + (rotatedRect.GetWidth() - (majorCurveWidth + smallCurveWidth));
                points[3].y = points[2].y;

                points[4].x = points[3].x + smallCurveWidth;
                points[4].y = points[3].y + TOP_SMALL_HEIGHT;

                points[5] = rotatedRect.GetBottomRight();

                tmpDC.SetPen(penColour);
                tmpDC.SetBrush(bgColour);
                tmpDC.DrawPolygon(6, points);
            }

            {
                wxPoint points[6];
                points[0] = rotatedRect.GetBottomLeft();
                points[0].x += 1;

                points[1].x = points[0].x + majorCurveWidth;
                points[1].y = rotatedRect.GetLeftTop().y + TOP_SMALL_HEIGHT + 1;

                points[2].x = points[1].x + smallCurveWidth;
                points[2].y = points[1].y - TOP_SMALL_HEIGHT;

                points[3].x = points[0].x + (rotatedRect.GetWidth() - 2 - (majorCurveWidth + smallCurveWidth));
                points[3].y = points[2].y;

                points[4].x = points[3].x + smallCurveWidth;
                points[4].y = points[3].y + TOP_SMALL_HEIGHT;

                points[5] = rotatedRect.GetBottomRight();
                points[5].x -= 2;

                tmpDC.SetPen(tabInfo.IsActive() ? colours.activeTabInnerPenColour : colours.inactiveTabInnerPenColour);
                tmpDC.SetBrush(bgColour);
                tmpDC.DrawPolygon(6, points);
            }
        } else {
            // Inactive tabs
            wxDirection direction = wxNORTH;
            wxPoint basePoint = ((style & kNotebook_LeftTabs) ? rotatedRect.GetLeftTop() : rotatedRect.GetRightTop());
            {
                wxPoint pt = basePoint;
                pt.x -= 1;
                wxRect gr(pt, wxSize(1, rotatedRect.GetHeight()));
                tmpDC.SetPen(*wxTRANSPARENT_PEN);
                tmpDC.GradientFillLinear(gr, colours.inactiveTabPenColour, bgColour, direction);
            }
            {
                wxPoint pt = basePoint;
                wxRect gr(pt, wxSize(1, rotatedRect.GetHeight()));
                tmpDC.SetPen(*wxTRANSPARENT_PEN);
                tmpDC.GradientFillLinear(gr, colours.inactiveTabInnerPenColour, bgColour, direction);
            }
            {
                wxPoint pt = basePoint;
                pt.x += 1;
                wxRect gr(pt, wxSize(1, rotatedRect.GetHeight()));
                tmpDC.SetPen(*wxTRANSPARENT_PEN);
                tmpDC.GradientFillLinear(gr, colours.inactiveTabPenColour, bgColour, direction);
            }
        }

        // Vertical tabs
        // Draw bitmap
        if(tabInfo.GetBitmap().IsOk()) { tmpDC.DrawBitmap(tabInfo.GetBitmap(), tabInfo.m_bmpY, tabInfo.m_bmpX); }

        tmpDC.DrawText(tabInfo.m_label, tabInfo.m_textY, tabInfo.m_textX);
        if(tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
            DrawButton(tmpDC, wxRect(tabInfo.m_bmpCloseX, tabInfo.m_bmpCloseY, CLOSE_BUTTON_SIZE, CLOSE_BUTTON_SIZE),
                       colours, eButtonState::kNormal);
        }
        tmpDC.SelectObject(wxNullBitmap);
        wxImage img = b.ConvertToImage();
        img = img.Rotate90((style & kNotebook_RightTabs));
        b = wxBitmap(img);
        dc.DrawBitmap(b, tabInfo.m_rect.GetTopLeft());

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

    if(!IS_VERTICAL_TABS(style)) {
        // Draw bitmap
        if(tabInfo.GetBitmap().IsOk()) {
            dc.DrawBitmap(tabInfo.GetBitmap(), tabInfo.m_bmpX + tabInfo.m_rect.GetX(), tabInfo.m_bmpY);
        }
        fontDC.DrawText(tabInfo.m_label, tabInfo.m_textX + tabInfo.m_rect.GetX(), tabInfo.m_textY);
        if(tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
            DrawButton(dc,
                       wxRect(tabInfo.m_bmpCloseX + tabInfo.m_rect.GetX(), tabInfo.m_bmpCloseY, CLOSE_BUTTON_SIZE,
                              CLOSE_BUTTON_SIZE),
                       colours, eButtonState::kNormal);
        }
    }
}

void clTabRendererCurved::DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect,
                                         wxDC& dc, const clTabColours& colours, size_t style)
{
    if(!IS_VERTICAL_TABS(style)) {
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
}
#endif
