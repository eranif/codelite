#include "clTabRendererSquare.h"
#include <wx/settings.h>
#include <wx/font.h>

clTabRendererSquare::clTabRendererSquare()
{
    bottomAreaHeight = 3;
    majorCurveWidth = 0;
    smallCurveWidth = 0;
    overlapWidth = 2;
    verticalOverlapWidth = 0;
    xSpacer = 5;
    ySpacer = 3;
}

clTabRendererSquare::~clTabRendererSquare() {}

void clTabRendererSquare::Draw(wxDC& dc, const clTabInfo& tabInfo, const clTabColours& colours, size_t style)
{
    wxColour bgColour(tabInfo.IsActive() ? colours.activeTabBgColour : colours.inactiveTabBgColour);
    wxColour penColour(tabInfo.IsActive() ? colours.activeTabPenColour : colours.inactiveTabPenColour);
    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    dc.SetTextForeground(tabInfo.IsActive() ? colours.activeTabTextColour : colours.inactiveTabTextColour);
    dc.SetFont(font);

    if(style & kNotebook_BottomTabs) {
        wxRect rr = tabInfo.m_rect;

        // Default top tabs
        dc.SetBrush(bgColour);
        dc.SetPen(penColour);
        dc.DrawRectangle(rr);

        // Draw bitmap
        if(tabInfo.GetBitmap().IsOk()) {
            dc.DrawBitmap(tabInfo.GetBitmap(), tabInfo.m_bmpX + rr.GetX(), tabInfo.m_bmpY);
        }
        dc.DrawText(tabInfo.m_label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY);
        if(tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
            dc.DrawBitmap(colours.closeButton, tabInfo.m_bmpCloseX + rr.GetX(), tabInfo.m_bmpCloseY);
        }

    } else if(style & kNotebook_LeftTabs) {

    } else if(style & kNotebook_RightTabs) {
    } else {
        wxRect rr = tabInfo.m_rect;

        // Default top tabs
        dc.SetBrush(bgColour);
        dc.SetPen(penColour);
        dc.DrawRectangle(rr);

        // Draw bitmap
        if(tabInfo.GetBitmap().IsOk()) {
            dc.DrawBitmap(tabInfo.GetBitmap(), tabInfo.m_bmpX + rr.GetX(), tabInfo.m_bmpY);
        }
        dc.DrawText(tabInfo.m_label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY);
        if(tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
            dc.DrawBitmap(colours.closeButton, tabInfo.m_bmpCloseX + rr.GetX(), tabInfo.m_bmpCloseY);
        }
    }
}

void clTabRendererSquare::DrawBottomRect(
    clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc, const clTabColours& colours, size_t style)
{
}
