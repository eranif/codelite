#include "clTabRendererGTK3.h"

#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
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

clTabRendererGTK3::clTabRendererGTK3(const wxWindow* parent)
    : clTabRenderer("GTK3", parent)
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
    SetUseBoldFont(true);
}

clTabRendererGTK3::~clTabRendererGTK3() {}

void clTabRendererGTK3::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                             const clTabColours& colours, size_t style, eButtonState buttonState)
{
    wxColour inactiveTabPenColour = colours.inactiveTabPenColour;

    wxColour tabBgColour;
    wxColour tabPenColour;
    if(tabInfo.IsActive()) {
        tabBgColour = DrawingUtils::IsDark(colours.tabAreaColour) ? colours.tabAreaColour.ChangeLightness(105)
                                                                  : colours.tabAreaColour.ChangeLightness(95);
        tabPenColour = DrawingUtils::IsDark(colours.tabAreaColour) ? colours.tabAreaColour.ChangeLightness(50)
                                                                   : colours.tabAreaColour.ChangeLightness(85);
    } else {
        tabBgColour = colours.tabAreaColour;
        tabPenColour = colours.tabAreaColour;
    }

    wxColour bgColour(tabBgColour);
    wxColour penColour(tabPenColour);

    wxFont font = GetTabFont(tabInfo.IsActive() && IsUseBoldFont());
    fontDC.SetTextForeground(tabInfo.IsActive() ? colours.activeTabTextColour : colours.inactiveTabTextColour);
    if(tabInfo.IsActive()) {
        font.SetWeight(wxFONTWEIGHT_BOLD);
    }
    fontDC.SetFont(font);

    wxRect rr = tabInfo.m_rect;

    dc.SetBrush(bgColour);
    dc.SetPen(bgColour);
    dc.DrawRectangle(rr);

    bool bVerticalTabs = IS_VERTICAL_TABS(style);
    // Draw bitmap
    if(tabInfo.HasBitmap() && !bVerticalTabs) {
        int bmpIndex =
            (!tabInfo.IsActive() && tabInfo.HasDisableBitmap()) ? tabInfo.GetDisabledBitmp() : tabInfo.GetBitmap();
        bool disabledBmp = !tabInfo.IsActive();
        dc.DrawBitmap(tabInfo.GetBitmap(bmpIndex, disabledBmp), tabInfo.m_bmpX + rr.GetX(), tabInfo.m_bmpY + rr.GetY());
    }
    wxString label = tabInfo.GetBestLabel(style);
    if(bVerticalTabs) {
        // Check that the text can fit into the tab label
        int textEndCoord = tabInfo.m_textX + tabInfo.m_textWidth;
        int tabEndCoord = tabInfo.GetRect().GetRightTop().x;
        if((textEndCoord + clTabRenderer::GetMarkerWidth()) > tabEndCoord) {
            int newSize = tabEndCoord - tabInfo.m_textX;
            DrawingUtils::TruncateText(tabInfo.m_label, newSize, dc, label);
        }
    }

    fontDC.DrawText(label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY + rr.GetY());
    if(style & kNotebook_CloseButtonOnActiveTab) {
        DrawButton(parent, dc, tabInfo, colours, buttonState);
    }

    if(tabInfo.IsActive()) {
        DrawMarker(dc, tabInfo, colours, style | kNotebook_UnderlineActiveTab);
    }

    dc.SetPen(penColour);
    dc.DrawLine(rr.GetTopLeft(), rr.GetBottomLeft());
    dc.DrawLine(rr.GetTopRight(), rr.GetBottomRight());
}

void clTabRendererGTK3::DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc,
                                       const clTabColours& colours, size_t style)
{
    wxUnusedVar(parent);
    wxUnusedVar(activeTab);
    wxUnusedVar(style);
    wxUnusedVar(colours);
    wxUnusedVar(dc);
    wxUnusedVar(clientRect);
}

wxColour clTabRendererGTK3::DrawBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const clTabColours& colours,
                                           size_t style)
{
    wxColour bgColour(colours.tabAreaColour);
    dc.SetPen(bgColour);
    dc.SetBrush(bgColour);
    dc.DrawRectangle(rect);
    return bgColour;
}

void clTabRendererGTK3::FinaliseBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const wxRect& activeTabRect,
                                           const clTabColours& colours, size_t style)
{
    clTabRenderer::FinaliseBackground(parent, dc, rect, activeTabRect, colours, style);
}

void clTabRendererGTK3::AdjustColours(clTabColours& colours, size_t style)
{
    wxUnusedVar(style);
    wxUnusedVar(colours);
}
