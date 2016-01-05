#include "clTabRendererSquare.h"
#include <wx/settings.h>
#include <wx/font.h>

clTabRendererSquare::clTabRendererSquare()
{
    clTabInfo::SMALL_CURVE_WIDTH = 0;
    clTabInfo::MAJOR_CURVE_WIDTH = 0;
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

    } else if(style & kNotebook_LeftTabs) {

    } else if(style & kNotebook_RightTabs) {
    } else {
        // Default top tabs
        dc.SetBrush(bgColour);
        dc.SetPen(penColour);
        dc.DrawRectangle(tabInfo.m_rect);

        // Draw bitmap
        if(tabInfo.GetBitmap().IsOk()) {
            dc.DrawBitmap(tabInfo.GetBitmap(), tabInfo.m_bmpX + tabInfo.m_rect.GetX(), tabInfo.m_bmpY);
        }
        dc.DrawText(tabInfo.m_label, tabInfo.m_textX + tabInfo.m_rect.GetX(), tabInfo.m_textY);
        if(tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
            dc.DrawBitmap(colours.closeButton, tabInfo.m_bmpCloseX + tabInfo.m_rect.GetX(), tabInfo.m_bmpCloseY);
        }
    }
}
