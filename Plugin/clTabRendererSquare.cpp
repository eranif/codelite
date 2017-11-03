#include "clTabRendererSquare.h"
#include <wx/settings.h>
#include <wx/font.h>
#include "drawingutils.h"
#include <wx/dcmemory.h>

#define DRAW_LINE(__p1, __p2) \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);

clTabRendererSquare::clTabRendererSquare()
{
#ifdef __WXGTK__
    bottomAreaHeight = 0;
#else
    bottomAreaHeight = 3;
#endif
    majorCurveWidth = 0;
    smallCurveWidth = 0;
    overlapWidth = 2;
    verticalOverlapWidth = 2;
    xSpacer = 15;
    ySpacer = 5;
}

clTabRendererSquare::~clTabRendererSquare() {}

void clTabRendererSquare::Draw(wxDC& dc, const clTabInfo& tabInfo, const clTabColours& colours, size_t style)
{
    wxColour inactiveTabPenColour = colours.inactiveTabPenColour;

    wxColour bgColour(tabInfo.IsActive() ? colours.activeTabBgColour : colours.inactiveTabBgColour);
    wxColour penColour(tabInfo.IsActive() ? colours.activeTabPenColour : inactiveTabPenColour);
    wxFont font = GetTabFont();
    dc.SetTextForeground(tabInfo.IsActive() ? colours.activeTabTextColour : colours.inactiveTabTextColour);
    dc.SetFont(font);

    wxRect rr = tabInfo.m_rect;

    dc.SetBrush(bgColour);
    dc.SetPen(penColour);
    //    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rr);

    // Restore the pen
    dc.SetPen(penColour);
    if(style & kNotebook_BottomTabs) {
        // Draw bitmap
        if(tabInfo.GetBitmap().IsOk()) {
            dc.DrawBitmap(tabInfo.GetBitmap(), tabInfo.m_bmpX + rr.GetX(), tabInfo.m_bmpY);
        }
        dc.DrawText(tabInfo.m_label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY);
        if(tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
            DrawButton(
                dc, wxRect(tabInfo.m_bmpCloseX + rr.GetX(), tabInfo.m_bmpCloseY, CLOSE_BUTTON_SIZE, CLOSE_BUTTON_SIZE),
                colours, eButtonState::kNormal);
        }

    } else if(IS_VERTICAL_TABS(style)) {
        wxRect rotatedRect(0, 0, tabInfo.m_rect.GetHeight(), tabInfo.m_rect.GetWidth());
        wxBitmap bmp(rotatedRect.GetSize());
        wxMemoryDC memDC(bmp);
        memDC.SetPen(penColour);
        memDC.SetBrush(tabInfo.IsActive() ? colours.activeTabBgColour : colours.inactiveTabBgColour);
        memDC.DrawRectangle(rotatedRect);
        memDC.SetFont(font);
        memDC.SetTextForeground(tabInfo.IsActive() ? colours.activeTabTextColour : colours.inactiveTabTextColour);
        memDC.DrawText(tabInfo.m_label, tabInfo.m_textY, tabInfo.m_textX);
        if(tabInfo.GetBitmap().IsOk()) {
            memDC.DrawBitmap(tabInfo.GetBitmap(), tabInfo.m_bmpY, tabInfo.m_bmpX);
        }
        memDC.SelectObject(wxNullBitmap);
        wxImage img = bmp.ConvertToImage();
        img = img.Rotate90((style & kNotebook_RightTabs));
        bmp = wxBitmap(img);
        dc.DrawBitmap(bmp, tabInfo.m_rect.GetTopLeft());
    } else {
        // Draw bitmap
        if(tabInfo.GetBitmap().IsOk()) {
            dc.DrawBitmap(tabInfo.GetBitmap(), tabInfo.m_bmpX + rr.GetX(), tabInfo.m_bmpY);
        }
        dc.DrawText(tabInfo.m_label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY);
        if(tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
            DrawButton(
                dc, wxRect(tabInfo.m_bmpCloseX + rr.GetX(), tabInfo.m_bmpCloseY, CLOSE_BUTTON_SIZE, CLOSE_BUTTON_SIZE),
                colours, eButtonState::kNormal);
        }
    }
}

void clTabRendererSquare::DrawBottomRect(clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc,
                                         const clTabColours& colours, size_t style)
{
    const int penWidth = 1;
    wxPen markerPen(colours.markerColour, penWidth);
    bool underlineTab = (style & kNotebook_UnderlineActiveTab);

    // Draw marker line if needed
    if(underlineTab) {
        wxPoint pt1, pt2;
        if((style & kNotebook_LeftTabs) || (style & kNotebook_RightTabs)) {
            pt1 = activeTab->GetRect().GetTopLeft();
            pt2 = activeTab->GetRect().GetTopRight();
#ifdef __WXOSX__
            pt2.x += 1;
#endif
            dc.SetPen(markerPen);
            for(size_t i = 0; i < 3; ++i) {
                DRAW_LINE(pt1, pt2);
                pt1.y += 1;
                pt2.y += 1;
            }
        } else {
            pt1 = activeTab->GetRect().GetTopLeft();
            pt2 = activeTab->GetRect().GetBottomLeft();
#ifdef __WXOSX__
            pt2.y += 2;
#endif
            dc.SetPen(markerPen);

            for(size_t i = 0; i < 3; ++i) {
                DRAW_LINE(pt1, pt2);
                pt1.x += 1;
                pt2.x += 1;
            }
        }
    }
    wxPoint pt1, pt2;
    dc.SetPen(colours.activeTabPenColour);
    if(style & kNotebook_LeftTabs) {
        // Left tabs
        pt1 = clientRect.GetTopRight();
        pt2 = clientRect.GetBottomRight();
        DRAW_LINE(pt1, pt2);

    } else if(style & kNotebook_RightTabs) {
        // Right tabs
        pt1 = clientRect.GetTopLeft();
        pt2 = clientRect.GetBottomLeft();
        DRAW_LINE(pt1, pt2);
    } else if(style & kNotebook_BottomTabs) {
        // bottom tabs
        pt1 = clientRect.GetTopLeft();
        pt2 = clientRect.GetTopRight();
        DRAW_LINE(pt1, pt2);
    } else {
        // Top tabs
        pt1 = clientRect.GetBottomLeft();
        pt2 = clientRect.GetBottomRight();
        dc.SetPen(colours.activeTabPenColour);
        DRAW_LINE(pt1, pt2);
    }

    ClearActiveTabExtraLine(activeTab, dc, colours, style);
}
