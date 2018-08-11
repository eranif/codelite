#include "clTabRendererClassic.h"

#include "drawingutils.h"
#include <wx/dcmemory.h>
#include <wx/font.h>
#include <wx/settings.h>

#define DRAW_LINE(__p1, __p2) \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);

clTabRendererClassic::clTabRendererClassic()
    : clTabRenderer("DEFAULT")
{
    bottomAreaHeight = 3;
    majorCurveWidth = 15;
    smallCurveWidth = 4;
    overlapWidth = 20;
    verticalOverlapWidth = 3;
}

clTabRendererClassic::~clTabRendererClassic() {}

void clTabRendererClassic::InitDarkColours(clTabColours& colours, const wxColour& activeTabBGColour)
{
    // Active tab
    colours.activeTabTextColour = "WHITE";
    colours.activeTabBgColour = activeTabBGColour;
    colours.activeTabPenColour = activeTabBGColour.ChangeLightness(90);
    colours.activeTabInnerPenColour = activeTabBGColour;

    // Inactive tab
    colours.inactiveTabBgColour = activeTabBGColour.ChangeLightness(120);
    colours.inactiveTabTextColour = colours.activeTabTextColour.ChangeLightness(80);
    colours.inactiveTabPenColour = colours.inactiveTabBgColour.ChangeLightness(85);
    colours.inactiveTabInnerPenColour = colours.inactiveTabBgColour.ChangeLightness(110);
    colours.tabAreaColour = DrawingUtils::GetPanelBgColour();
}

void clTabRendererClassic::InitLightColours(clTabColours& colours, const wxColour& activeTabBGColour)
{
    // Active tab
    colours.activeTabTextColour = DrawingUtils::GetButtonTextColour();
    colours.activeTabBgColour = activeTabBGColour;
    colours.activeTabPenColour = activeTabBGColour.ChangeLightness(80);
    colours.activeTabInnerPenColour = activeTabBGColour.ChangeLightness(110);

    // Inactive tab
    colours.inactiveTabBgColour = colours.activeTabBgColour.ChangeLightness(85); // darker
    colours.inactiveTabTextColour = DrawingUtils::GetButtonTextColour();
    colours.inactiveTabPenColour = colours.inactiveTabBgColour.ChangeLightness(80);
    colours.inactiveTabInnerPenColour = colours.inactiveTabBgColour.ChangeLightness(130);
    colours.tabAreaColour = DrawingUtils::GetPanelBgColour();
}

void clTabRendererClassic::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                                const clTabColours& colors, size_t style)
{
    const int TOP_SMALL_HEIGHT = 2;

    clTabColours colours = colors;
    if(DrawingUtils::IsDark(colours.activeTabBgColour)) {
        InitDarkColours(colours, colours.activeTabBgColour);
    } else {
        InitLightColours(colours, colours.activeTabBgColour);
    }

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
        DrawButton(dc,
                   wxRect(tabInfo.m_bmpCloseX + tabInfo.m_rect.GetX(), tabInfo.m_bmpCloseY, CLOSE_BUTTON_SIZE,
                          CLOSE_BUTTON_SIZE),
                   colours, eButtonState::kNormal);
    }
}

void clTabRendererClassic::DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t tabInfo, const wxRect& clientRect,
                                          wxDC& dc, const clTabColours& colors, size_t style)
{
    clTabColours colours = colors;
    if(DrawingUtils::IsDark(colours.activeTabBgColour)) {
        InitDarkColours(colours, colours.activeTabBgColour);
    } else {
        InitLightColours(colours, colours.activeTabBgColour);
    }

    if(style & kNotebook_LeftTabs) {
        // Draw 3 lines on the right
        dc.SetPen(colours.activeTabPenColour);
        dc.SetBrush(colours.activeTabBgColour);
        wxPoint topLeft = clientRect.GetTopRight();
        wxSize rectSize(bottomAreaHeight + 2, clientRect.height);
        topLeft.x -= bottomAreaHeight;
        wxRect bottomRect(topLeft, rectSize);

        // We intentionally move the rect out of the client rect
        // so the top and bottom lines will be drawn out of screen
        bottomRect.y -= 1;
        bottomRect.height += 2;
        dc.DrawRectangle(bottomRect);

        // Draw a line under the active tab
        // that will erase the line drawn by the above rect
        wxPoint from, to;
        from = tabInfo->GetRect().GetTopRight();
        to = tabInfo->GetRect().GetBottomRight();
        from.x = bottomRect.GetTopLeft().x;
        to.x = bottomRect.GetTopLeft().x;
        from.y += 2;
        to.y -= 2;

        dc.SetPen(colours.activeTabBgColour);
        DRAW_LINE(from, to);
    } else if(style & kNotebook_RightTabs) {
        // Draw 3 lines on the right
        dc.SetPen(colours.activeTabPenColour);
        dc.SetBrush(colours.activeTabBgColour);
        wxPoint topLeft = clientRect.GetTopLeft();
        wxSize rectSize(bottomAreaHeight + 2, clientRect.height);
        wxRect bottomRect(topLeft, rectSize);

        // We intentionally move the rect out of the client rect
        // so the top and bottom lines will be drawn out of screen
        bottomRect.y -= 1;
        bottomRect.height += 2;
        dc.DrawRectangle(bottomRect);

        // Draw a line under the active tab
        // that will erase the line drawn by the above rect
        wxPoint from, to;
        from = tabInfo->GetRect().GetTopLeft();
        to = tabInfo->GetRect().GetBottomLeft();
        from.x = bottomRect.GetTopRight().x;
        to.x = bottomRect.GetTopRight().x;
        from.y += 2;
        to.y -= 2;

        dc.SetPen(colours.activeTabBgColour);
        DRAW_LINE(from, to);

    } else if(style & kNotebook_BottomTabs) {
        // Draw 3 lines at the top
        dc.SetPen(colours.activeTabBgColour);
        dc.SetBrush(colours.activeTabBgColour);
        wxPoint topLeft = clientRect.GetTopLeft();
        wxSize rectSize(clientRect.width, bottomAreaHeight);
        topLeft.y = 0;
        wxRect bottomRect(topLeft, rectSize);
        // We intentionally move the rect out of the client rect
        // so the left and right lines will be drawn out of screen
        bottomRect.x -= 1;
        bottomRect.width += 2;
        dc.DrawRectangle(bottomRect);

        // Draw a line under the active tab
        // that will erase the line drawn by the above rect
        wxPoint from, to;
        from = tabInfo->GetRect().GetTopLeft();
        to = tabInfo->GetRect().GetTopRight();
        from.y += bottomAreaHeight - 1;
        from.x += 2;
        to.y += bottomAreaHeight - 1;
        to.x -= 2;

        dc.SetPen(colours.activeTabBgColour);
        dc.DrawLine(from, to);
        DRAW_LINE(from, to);

    } else {
        // Draw 3 lines at the bottom
        dc.SetPen(colours.activeTabBgColour);
        dc.SetBrush(colours.activeTabBgColour);
        wxPoint topLeft = clientRect.GetBottomLeft();
        wxSize rectSize(clientRect.width, bottomAreaHeight);
        topLeft.y -= rectSize.GetHeight() - 1;
        wxRect bottomRect(topLeft, rectSize);
        // We intentionally move the rect out of the client rect
        // so the left and right lines will be drawn out of screen
        bottomRect.x -= 1;
        bottomRect.width += 2;
        dc.DrawRectangle(bottomRect);

        // Draw a line under the active tab
        // that will erase the line drawn by the above rect
        wxPoint from, to;
        from = tabInfo->GetRect().GetBottomLeft();
        to = tabInfo->GetRect().GetBottomRight();
        from.y -= bottomAreaHeight - 1;
        from.x += 2;
        to.y -= bottomAreaHeight - 1;
        to.x -= 2;

        dc.SetPen(colours.activeTabBgColour);
        dc.DrawLine(from, to);
        DRAW_LINE(from, to);
    }
}
