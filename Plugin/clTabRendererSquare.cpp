#include "clTabRendererSquare.h"
#if !USE_AUI_NOTEBOOK
#include "drawingutils.h"
#include "editor_config.h"
#include <wx/dcmemory.h>
#include <wx/font.h>
#include <wx/settings.h>

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
    ySpacer = EditorConfigST::Get()->GetOptions()->GetNotebookTabHeight();
}

clTabRendererSquare::~clTabRendererSquare() {}

void clTabRendererSquare::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                               const clTabColours& colours, size_t style)
{
    wxColour inactiveTabPenColour = colours.inactiveTabPenColour;

    wxColour bgColour(tabInfo.IsActive() ? colours.activeTabBgColour : colours.inactiveTabBgColour);
    wxColour penColour(tabInfo.IsActive() ? colours.activeTabPenColour : inactiveTabPenColour);
    wxColour separatorColour = penColour.ChangeLightness(110);

    wxFont font = GetTabFont();
    fontDC.SetTextForeground(tabInfo.IsActive() ? colours.activeTabTextColour : colours.inactiveTabTextColour);
    fontDC.SetFont(font);

    wxRect rr = tabInfo.m_rect;

    dc.SetBrush(bgColour);
    dc.SetPen(bgColour);
    dc.DrawRectangle(rr);

    // Restore the pen
    penColour = bgColour;
    dc.SetPen(penColour);
    if(style & kNotebook_BottomTabs) {
        // Draw bitmap
        if(tabInfo.GetBitmap().IsOk()) {
            dc.DrawBitmap(tabInfo.GetBitmap(), tabInfo.m_bmpX + rr.GetX(), tabInfo.m_bmpY);
        }
        fontDC.DrawText(tabInfo.m_label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY);
        if(tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
            DrawingUtils::DrawButtonX(
                dc, parent,
                wxRect(tabInfo.m_bmpCloseX + rr.GetX(), tabInfo.m_bmpCloseY, CLOSE_BUTTON_SIZE, CLOSE_BUTTON_SIZE),
                colours.markerColour, eButtonState::kNormal);
        }

        // Draw the separator line
        {
            wxPoint pt_1 = rr.GetRightTop();
            wxPoint pt_2 = rr.GetRightBottom();
            dc.SetPen(separatorColour);
            dc.DrawLine(pt_1, pt_2);
        }

        if(tabInfo.IsActive()) {
            wxPoint pt_1 = rr.GetLeftTop();
            wxPoint pt_2 = rr.GetLeftBottom();
            dc.SetPen(separatorColour);
            dc.DrawLine(pt_1, pt_2);
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
        if(tabInfo.GetBitmap().IsOk()) { memDC.DrawBitmap(tabInfo.GetBitmap(), tabInfo.m_bmpY, tabInfo.m_bmpX); }

        // Draw the separator line
        memDC.SetPen(separatorColour);
        {
            wxPoint pt_1 = rotatedRect.GetRightTop();
            wxPoint pt_2 = rotatedRect.GetRightBottom();
            memDC.DrawLine(pt_1, pt_2);
        }
        {
            wxPoint pt_1 = rotatedRect.GetLeftTop();
            wxPoint pt_2 = rotatedRect.GetLeftBottom();
            memDC.DrawLine(pt_1, pt_2);
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
        fontDC.DrawText(tabInfo.m_label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY);
        if(tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
            DrawButton(
                dc, wxRect(tabInfo.m_bmpCloseX + rr.GetX(), tabInfo.m_bmpCloseY, CLOSE_BUTTON_SIZE, CLOSE_BUTTON_SIZE),
                colours, eButtonState::kNormal);
        }
        // Draw the separator line
        {
            wxPoint pt_1 = rr.GetRightTop();
            wxPoint pt_2 = rr.GetRightBottom();
            dc.SetPen(separatorColour);
            dc.DrawLine(pt_1, pt_2);
        }
        if(tabInfo.IsActive()) {
            wxPoint pt_1 = rr.GetLeftTop();
            wxPoint pt_2 = rr.GetLeftBottom();
            dc.SetPen(separatorColour);
            dc.DrawLine(pt_1, pt_2);
        }
    }
}

void clTabRendererSquare::DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect,
                                         wxDC& dc, const clTabColours& colours, size_t style)
{
    const int penWidth = 3;
    wxPen markerPen(colours.markerColour, penWidth);
    bool underlineTab = (style & kNotebook_UnderlineActiveTab);

    // Draw marker line if needed
    wxRect confinedRect = parent->GetClientRect();
    if(underlineTab) {
        wxPoint p1, p2;
        if((style & kNotebook_LeftTabs) || (style & kNotebook_RightTabs)) {
            p1 = activeTab->GetRect().GetTopLeft();
            p2 = activeTab->GetRect().GetBottomLeft();
            p1.y += 1;
            p2.y -= 1;
            p1.x += 1;
            p2.x += 1;
            dc.SetPen(markerPen);
            dc.DrawLine(p1, p2);
        } else {

            int xx = activeTab->GetRect().GetTopRight().x;
            if((activeTab->GetRect().GetTopRight().x > confinedRect.GetTopRight().x) &&
               (style & kNotebook_ShowFileListButton)) {
                // The active tab is only partially drawn
                // and we have the style "kNotebook_ShowFileListButton":
                // Ensure that we don't draw our marker on top of the button
                xx = confinedRect.GetTopRight().x - CHEVRON_SIZE;
            }

            if((style & kNotebook_BottomTabs)) {
                // Bottom tabs
                p1 = activeTab->GetRect().GetBottomLeft();
                p2 = activeTab->GetRect().GetBottomRight();
                p1.x += 1;
                // Update the ending X coordinate (see above comment for why)
                if(xx > 0) { p2.x = xx; }
                p2.x -= 1;
                p1.y -= 1;
                p2.y -= 1;
                dc.SetPen(markerPen);
                dc.DrawLine(p1, p2);
            } else {
                p1 = activeTab->GetRect().GetTopLeft();
                p2 = activeTab->GetRect().GetTopRight();
                p1.x += 1;
                // Update the ending X coordinate (see above comment for why)
                if(xx > 0) { p2.x = xx; }
                p2.x -= 1;
                p1.y += 1;
                p2.y += 1;
                dc.SetPen(markerPen);
                dc.DrawLine(p1, p2);
            }
        }
    }
    ClearActiveTabExtraLine(activeTab, dc, colours, style);
}
#endif
