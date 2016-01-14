#include "clTabRendererSquare.h"
#include <wx/settings.h>
#include <wx/font.h>
#include "drawingutils.h"

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
    xSpacer = 10;
    ySpacer = 4;
}

clTabRendererSquare::~clTabRendererSquare() {}

void clTabRendererSquare::Draw(wxDC& dc, const clTabInfo& tabInfo, const clTabColours& colours, size_t style)
{
    wxColour inactiveTabPenColour = colours.inactiveTabPenColour;
    
    wxColour bgColour(tabInfo.IsActive() ? colours.activeTabBgColour : colours.inactiveTabBgColour);
    wxColour penColour(tabInfo.IsActive() ? colours.activeTabPenColour : inactiveTabPenColour);
    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    dc.SetTextForeground(tabInfo.IsActive() ? colours.activeTabTextColour : colours.inactiveTabTextColour);
    dc.SetFont(font);

    wxRect rr = tabInfo.m_rect;

    dc.SetBrush(bgColour);
    dc.SetPen(penColour);
    if(tabInfo.IsActive()) {
        dc.DrawRectangle(rr);
    }

    if(style & kNotebook_BottomTabs) {
        // Draw bitmap
        if(tabInfo.GetBitmap().IsOk()) {
            dc.DrawBitmap(tabInfo.GetBitmap(), tabInfo.m_bmpX + rr.GetX(), tabInfo.m_bmpY);
        }
        dc.DrawText(tabInfo.m_label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY);
        if(tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
            dc.DrawBitmap(colours.closeButton, tabInfo.m_bmpCloseX + rr.GetX(), tabInfo.m_bmpCloseY);
        }
        dc.DrawLine(rr.GetTopRight(), rr.GetBottomRight());

    } else if(style & kNotebook_LeftTabs) {
        dc.DrawRotatedText(tabInfo.m_label, tabInfo.m_textX, rr.GetY() + rr.GetHeight() - tabInfo.m_textY, 90.0);
        dc.DrawLine(rr.GetBottomLeft(), rr.GetBottomRight());
        dc.SetPen(bgColour);
        DRAW_LINE(rr.GetTopLeft(), rr.GetBottomLeft());
        
    } else if(style & kNotebook_RightTabs) {
        dc.DrawRotatedText(tabInfo.m_label, tabInfo.m_textX, rr.GetY() + rr.GetHeight() - tabInfo.m_textY, 90);
        dc.DrawLine(rr.GetBottomLeft(), rr.GetBottomRight());
        dc.SetPen(bgColour);
        DRAW_LINE(rr.GetTopRight(), rr.GetBottomRight());
        
    } else {
        // Draw bitmap
        if(tabInfo.GetBitmap().IsOk()) {
            dc.DrawBitmap(tabInfo.GetBitmap(), tabInfo.m_bmpX + rr.GetX(), tabInfo.m_bmpY);
        }
        dc.DrawText(tabInfo.m_label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY);
        if(tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
            dc.DrawBitmap(colours.closeButton, tabInfo.m_bmpCloseX + rr.GetX(), tabInfo.m_bmpCloseY);
        }
        dc.DrawLine(rr.GetTopRight(), rr.GetBottomRight());
    }
}

void clTabRendererSquare::DrawBottomRect(
    clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc, const clTabColours& colours, size_t style)
{
    const int penWidth = 1;
    wxPen pen(colours.activeTabPenColour, penWidth);
    wxPen markerPen(colours.markerColour, penWidth);

    wxPoint p1, p2;
    dc.SetPen(pen);

    if(style & kNotebook_LeftTabs) {
        dc.DrawLine(clientRect.GetTopLeft(), clientRect.GetBottomLeft());

        p1 = activeTab->GetRect().GetTopRight();
        p2 = activeTab->GetRect().GetBottomRight();

        p2.x -= penWidth;
        p1.x -= penWidth;
    } else if(style & kNotebook_RightTabs) {
        dc.DrawLine(clientRect.GetTopRight(), clientRect.GetBottomRight());

        p1 = activeTab->GetRect().GetTopLeft();
        p2 = activeTab->GetRect().GetBottomLeft();

        // p1.x += penWidth + 1;
        // p2.x += penWidth + 1;
    } else if(style & kNotebook_BottomTabs){
        // draw a single line at the top
        dc.DrawLine(clientRect.GetBottomLeft(), clientRect.GetBottomRight());
        
    } else {
        // draw a single line at the top
        dc.DrawLine(clientRect.GetTopLeft(), clientRect.GetTopRight());

        p1 = activeTab->GetRect().GetTopLeft();
        p2 = activeTab->GetRect().GetTopRight();
    }

    bool underlineTab = (style & kNotebook_UnderlineActiveTab);

    pen = wxPen(colours.activeTabBgColour, penWidth);

    // Draw marker line if needed
    if(underlineTab) {
        if(style & kNotebook_BottomTabs) {
            p1 = activeTab->GetRect().GetTopLeft();
            p2 = activeTab->GetRect().GetTopRight();
            dc.SetPen(markerPen);

            for(size_t i = 0; i < 3; ++i) {
                DRAW_LINE(p1, p2);
                p1.y += 1;
                p2.y += 1;
            }

        } else if(style & kNotebook_LeftTabs) {
            dc.SetPen(markerPen);
            for(size_t i = 0; i < 3; ++i) {
                DRAW_LINE(p1, p2);
                p1.x -= 1;
                p2.x -= 1;
            }
        } else if(style & kNotebook_RightTabs) {
            dc.SetPen(markerPen);
            for(size_t i = 0; i < 3; ++i) {
                DRAW_LINE(p1, p2);
                p1.x += 1;
                p2.x += 1;
            }
        } else {
            // top tabs
            dc.SetPen(pen);
            p1.x += 1;
            p2.x -= 1;
            DRAW_LINE(p1, p2);

            // draw the marker at the bottom of the tabs
            p1 = activeTab->GetRect().GetBottomLeft();
            p2 = activeTab->GetRect().GetBottomRight();

            dc.SetPen(markerPen);
            for(size_t i = 0; i < 3; ++i) {
                DRAW_LINE(p1, p2);
                p1.y -= 1;
                p2.y -= 1;
            }
        }
    }
}
