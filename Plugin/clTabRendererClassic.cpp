#include "clTabRendererClassic.h"

#include "clColours.h"
#include "clGenericNotebook.hpp"
#include "clSystemSettings.h"
#include "drawingutils.h"
#include "globals.h"
#include "imanager.h"
#include <wx/dcmemory.h>
#include <wx/font.h>
#include <wx/settings.h>
#include <wx/stc/stc.h>

#define DRAW_LINE(__p1, __p2) \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);

clTabRendererClassic::clTabRendererClassic(const wxWindow* parent)
    : clTabRenderer("MINIMAL", parent)
{
    bottomAreaHeight = 0;
    smallCurveWidth = 0;
    majorCurveWidth = 0;
    overlapWidth = 0;
    verticalOverlapWidth = 0;
}

clTabRendererClassic::~clTabRendererClassic() {}

void clTabRendererClassic::InitDarkColours(clTabColours& colours, const wxColour& activeTabBGColour)
{
    wxUnusedVar(colours);
    wxUnusedVar(activeTabBGColour);
}

void clTabRendererClassic::InitLightColours(clTabColours& colours, const wxColour& activeTabBGColour)
{
    // Active tab
    wxUnusedVar(colours);
    wxUnusedVar(activeTabBGColour);
}

void clTabRendererClassic::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                                const clTabColours& colors, size_t style, eButtonState buttonState)
{
    wxRect tabRect = tabInfo.GetRect();
    tabRect.Deflate(1, 0);

    clTabColours colours = colors;

    wxColour penColour(tabInfo.IsActive() ? colours.activeTabPenColour : colours.inactiveTabPenColour);
    wxColour bgColour(colours.tabAreaColour);
    wxColour activeTabBgColour = colours.activeTabBgColour;

    bool is_dark = DrawingUtils::IsDark(colours.activeTabBgColour);
    // If we are painting the active tab, check to see if the page is of type wxStyledTextCtrl
    if(style & kNotebook_DynamicColours) {
        auto editor = clGetManager()->GetActiveEditor();
        if(editor) {
            activeTabBgColour = editor->GetCtrl()->StyleGetBackground(0);
        }

        bgColour = activeTabBgColour.ChangeLightness(is_dark ? 120 : 80);
    }

    wxFont font = GetTabFont(false);
    fontDC.SetTextForeground(tabInfo.IsActive() ? colours.activeTabTextColour : colours.inactiveTabTextColour);
    fontDC.SetFont(font);

    if(style & kNotebook_BottomTabs) {
        // bottom tabs
        tabRect.height -= 2;
    } else if(!IS_VERTICAL_TABS(style)) {
        // top tabs
        tabRect.y += 2;
        tabRect.height -= 2;
    }

    dc.SetPen(tabInfo.IsActive() ? activeTabBgColour : bgColour);
    dc.SetBrush(tabInfo.IsActive() ? activeTabBgColour : bgColour);
    dc.DrawRectangle(tabRect);

    // Draw bitmap
    if(tabInfo.HasBitmap()) {
        int bmpIndex =
            (!tabInfo.IsActive() && tabInfo.HasDisableBitmap()) ? tabInfo.GetDisabledBitmp() : tabInfo.GetBitmap();
        bool disabledBmp = (!tabInfo.IsActive() && tabInfo.HasDisableBitmap());
        const wxBitmap& bmp = tabInfo.GetBitmap(bmpIndex, disabledBmp);
        if(bmp.IsOk()) {
            dc.DrawBitmap(bmp, tabInfo.m_bmpX + tabInfo.m_rect.GetX(), tabInfo.m_bmpY);
        }
    }

    wxRect rr = tabInfo.m_rect;
    wxString label = tabInfo.GetBestLabel(style);
    bool bVerticalTabs = IS_VERTICAL_TABS(style);
    if(bVerticalTabs) {
        // Check that the text can fit into the tab label
        int textEndCoord = tabInfo.m_textX + tabInfo.m_textWidth;
        int tabEndCoord = tabInfo.GetRect().GetRightTop().x;
        if((textEndCoord + clTabRenderer::GetMarkerWidth()) > tabEndCoord) {
            int newSize = tabEndCoord - tabInfo.m_textX;
            wxString fixedLabel;
            DrawingUtils::TruncateText(label, newSize, fontDC, fixedLabel);
            label.swap(fixedLabel);
        }
    }

    wxRect textRect = dc.GetTextExtent(label);
    textRect = textRect.CenterIn(tabRect);
    fontDC.DrawText(label, tabInfo.m_textX + rr.GetX(), textRect.GetY());
    if(style & kNotebook_CloseButtonOnActiveTab) {
        // use the adjusted tab rect and not the original one passed to us
        clTabInfo tab_info = tabInfo;
        tab_info.SetRect(tabRect);
        DrawButton(parent, dc, tab_info, colours, buttonState);
    }
}

void clTabRendererClassic::DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t tabInfo, const wxRect& clientRect,
                                          wxDC& dc, const clTabColours& colors, size_t style)
{
    wxUnusedVar(parent);
    wxUnusedVar(tabInfo);
    wxUnusedVar(clientRect);
    wxUnusedVar(dc);
    wxUnusedVar(colors);
    wxUnusedVar(style);
}

void clTabRendererClassic::DrawBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const clTabColours& colours,
                                          size_t style)
{
    wxColour bg_colour = colours.tabAreaColour;
    if(style & kNotebook_DynamicColours) {
        bg_colour = clSystemSettings::GetDefaultPanelColour();
    }

    dc.SetBrush(bg_colour);
    dc.SetPen(bg_colour);
    dc.DrawRectangle(rect);
}

void clTabRendererClassic::FinaliseBackground(wxWindow* parent, wxDC& dc, const wxRect& clientRect,
                                              const wxRect& activeTabRect, const clTabColours& colours, size_t style)
{
    wxUnusedVar(parent);
    wxUnusedVar(dc);
    wxUnusedVar(clientRect);
    wxUnusedVar(activeTabRect);
    wxUnusedVar(colours);
    wxUnusedVar(style);
}
