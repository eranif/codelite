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
    dc.SetPen(*wxTRANSPARENT_PEN);
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
            dc.DrawBitmap(colours.closeButton, tabInfo.m_bmpCloseX + rr.GetX(), tabInfo.m_bmpCloseY);
        }
        // dc.DrawLine(rr.GetTopRight(), rr.GetBottomRight());

    } else if(IS_VERTICAL_TABS(style)) {
        wxRect rotatedRect(0, 0, tabInfo.m_rect.GetHeight(), tabInfo.m_rect.GetWidth());
        wxBitmap bmp(rotatedRect.GetSize());
        wxMemoryDC memDC(bmp);
        memDC.SetPen(colours.tabAreaColour);
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
            dc.DrawBitmap(colours.closeButton, tabInfo.m_bmpCloseX + rr.GetX(), tabInfo.m_bmpCloseY);
        }
        // dc.DrawLine(rr.GetTopRight(), rr.GetBottomRight());
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
    
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(activeTab->GetRect());
    
    if(style & kNotebook_BottomTabs) {
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
    
    if(!IS_VERTICAL_TABS(style)) {
        wxPoint delPt1, delPt2;
        if(style & kNotebook_BottomTabs) {
            delPt1 = activeTab->GetRect().GetBottomLeft();
            delPt2 = activeTab->GetRect().GetBottomRight();
            
        } else {
            delPt1 = activeTab->GetRect().GetTopLeft();
            delPt2 = activeTab->GetRect().GetTopRight();
        }
        delPt2.x -= penWidth;
        dc.SetPen(colours.activeTabBgColour);
        DRAW_LINE(delPt1, delPt2);
    }
    
    // Draw marker line if needed
    if(underlineTab) {

        if((style & kNotebook_LeftTabs) || (style & kNotebook_RightTabs)) {
            p1 = activeTab->GetRect().GetTopLeft();
            p2 = activeTab->GetRect().GetTopRight();
            dc.SetPen(markerPen);
            for(size_t i = 0; i < 2; ++i) {
                DRAW_LINE(p1, p2);
                p1.y += 1;
                p2.y += 1;
            }
        } else {
            p1 = activeTab->GetRect().GetTopLeft();
            p2 = activeTab->GetRect().GetBottomLeft();
            dc.SetPen(markerPen);

            for(size_t i = 0; i < 2; ++i) {
                DRAW_LINE(p1, p2);
                p1.x += 1;
                p2.x += 1;
            }
        }
    }
}
