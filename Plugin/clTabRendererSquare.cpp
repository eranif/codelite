#include "clTabRendererSquare.h"

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
    : clTabRenderer("MINIMAL")
{
#ifdef __WXGTK__
    bottomAreaHeight = 0;
#else
    bottomAreaHeight = 3;
#endif
    majorCurveWidth = 0;
    smallCurveWidth = 0;
    overlapWidth = 0;
    verticalOverlapWidth = 0;
    xSpacer = 15;
    ySpacer = EditorConfigST::Get()->GetOptions()->GetNotebookTabHeight();
}

clTabRendererSquare::~clTabRendererSquare() {}

void clTabRendererSquare::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                               const clTabColours& colours, size_t style, eButtonState buttonState)
{
    wxColour inactiveTabPenColour = colours.inactiveTabPenColour;
    bool isDark = DrawingUtils::IsDark(colours.activeTabBgColour);

    wxColour bgColour;
    wxColour textColour;
    if(tabInfo.IsActive()) {
        bgColour = colours.activeTabBgColour;
        textColour = colours.activeTabTextColour;
        if(isDark) { bgColour = bgColour.ChangeLightness(80); }
    } else {
        bgColour = colours.inactiveTabBgColour;
        textColour = colours.inactiveTabTextColour;
        if(isDark) { textColour = textColour.ChangeLightness(90); }
    }

    wxColour penColour(tabInfo.IsActive() ? colours.activeTabPenColour : inactiveTabPenColour);
    wxColour separatorColour = penColour.ChangeLightness(110);

    wxFont font = GetTabFont(false);
    fontDC.SetTextForeground(textColour);
    fontDC.SetFont(font);

    wxRect rr = tabInfo.m_rect;

    dc.SetBrush(bgColour);
    dc.SetPen(bgColour);
    dc.DrawRectangle(rr);

    // Restore the pen
    penColour = bgColour;
    dc.SetPen(penColour);

    bool bVerticalTabs = IS_VERTICAL_TABS(style);
    // Draw bitmap
    if(tabInfo.GetBitmap().IsOk() && !bVerticalTabs) {
        const wxBitmap& bmp = (!tabInfo.IsActive() && tabInfo.GetDisabledBitmp().IsOk()) ? tabInfo.GetDisabledBitmp()
                                                                                         : tabInfo.GetBitmap();
        dc.DrawBitmap(bmp, tabInfo.m_bmpX + rr.GetX(), tabInfo.m_bmpY + rr.GetY());
    }

    wxString label = tabInfo.m_label;
    if(bVerticalTabs) {
        // Check that the text can fit into the tab label
        int textEndCoord = tabInfo.m_textX + tabInfo.m_textWidth;
        int tabEndCoord = tabInfo.GetRect().GetRightTop().x;
        if(textEndCoord > tabEndCoord) {
            int newSize = tabEndCoord - tabInfo.m_textX;
            DrawingUtils::TruncateText(tabInfo.m_label, newSize, dc, label);
        }
    }

    fontDC.DrawText(label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY + rr.GetY());
    if(tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
        DrawButton(parent, dc, tabInfo, colours, buttonState);
    }
    if(tabInfo.IsActive()) { DrawMarker(dc, tabInfo, colours, style); }
    // Draw the separator line
    if(bVerticalTabs) {
        wxPoint pt_1 = rr.GetRightBottom();
        wxPoint pt_2 = rr.GetLeftBottom();
        dc.SetPen(separatorColour);
        dc.DrawLine(pt_1, pt_2);

    } else {
        wxPoint pt_1 = rr.GetRightTop();
        wxPoint pt_2 = rr.GetRightBottom();
        dc.SetPen(separatorColour);
        dc.DrawLine(pt_1, pt_2);
    }
}

void clTabRendererSquare::DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect,
                                         wxDC& dc, const clTabColours& colours, size_t style)
{
    // ClearActiveTabExtraLine(activeTab, dc, colours, style);
}
