#include "clTabRendererMinimal.hpp"

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

namespace
{
void GetTabColours(const clTabColours& colours, size_t style, wxColour* activeTabBgColour, wxColour* bgColour)
{
    *bgColour = colours.tabAreaColour;
    *activeTabBgColour = colours.activeTabBgColour;

    bool is_dark = DrawingUtils::IsDark(colours.activeTabBgColour);
    // If we are painting the active tab, check to see if the page is of type wxStyledTextCtrl
    if(style & kNotebook_DynamicColours) {
        auto editor = clGetManager()->GetActiveEditor();
        if(editor) {
            *activeTabBgColour = editor->GetCtrl()->StyleGetBackground(0);
            is_dark = DrawingUtils::IsDark(*activeTabBgColour);
        }
        *bgColour = activeTabBgColour->ChangeLightness(is_dark ? 120 : 80);
    }
}
} // namespace

clTabRendererMinimal::clTabRendererMinimal(const wxWindow* parent)
    : clTabRenderer("MINIMAL", parent)
{
    bottomAreaHeight = 0;
    smallCurveWidth = 0;
    majorCurveWidth = 0;
    overlapWidth = 0;
    verticalOverlapWidth = 0;
}

clTabRendererMinimal::~clTabRendererMinimal() {}

void clTabRendererMinimal::InitDarkColours(clTabColours& colours, const wxColour& activeTabBGColour)
{
    wxUnusedVar(colours);
    wxUnusedVar(activeTabBGColour);
}

void clTabRendererMinimal::InitLightColours(clTabColours& colours, const wxColour& activeTabBGColour)
{
    // Active tab
    wxUnusedVar(colours);
    wxUnusedVar(activeTabBGColour);
}

void clTabRendererMinimal::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                                const clTabColours& colors, size_t style, eButtonState buttonState)
{
    DoDraw(parent, dc, fontDC, tabInfo, colors, style, buttonState);
}

void clTabRendererMinimal::DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t tabInfo, const wxRect& clientRect,
                                          wxDC& dc, const clTabColours& colors, size_t style)
{
    wxUnusedVar(parent);
    wxUnusedVar(tabInfo);
    wxUnusedVar(clientRect);
    wxUnusedVar(dc);
    wxUnusedVar(colors);
    wxUnusedVar(style);
}

wxColour clTabRendererMinimal::DrawBackground(wxWindow* parent, wxDC& dc, const wxRect& rect,
                                              const clTabColours& colours, size_t style)
{
    wxColour bg_colour;
    wxColour active_tab_colour;
    GetTabColours(colours, style, &active_tab_colour, &bg_colour);
    wxUnusedVar(active_tab_colour);

    dc.SetBrush(bg_colour);
    dc.SetPen(bg_colour);
    dc.DrawRectangle(rect);
    return bg_colour;
}

wxRect clTabRendererMinimal::DoDraw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                                    const clTabColours& colors, size_t style, eButtonState buttonState)
{
    wxRect tabRect = tabInfo.GetRect();
    tabRect.Deflate(1, 0);

    clTabColours colours = colors;

    wxColour penColour(tabInfo.IsActive() ? colours.activeTabPenColour : colours.inactiveTabPenColour);
    wxColour bgColour, activeTabBgColour;
    GetTabColours(colours, style, &activeTabBgColour, &bgColour);

    // update the is_dark flag now that we set a bgColour
    bool is_dark = DrawingUtils::IsDark(bgColour);

    // the visible tab is the part of the rect that is actually seen
    // on the tab area. the tabRect might have negative coordinates for
    // hiding unwanted curves parts of the rect
    // we use the visible tab for centering text
    wxRect visibleTab = tabRect;
    if(style & kNotebook_BottomTabs) {
        // bottom tabs
        tabRect.y -= TAB_RADIUS;
        visibleTab.height -= TAB_RADIUS;

    } else if(style & kNotebook_LeftTabs) {
        tabRect.x += TAB_RADIUS;
        tabRect.width += TAB_RADIUS;

    } else if(style & kNotebook_RightTabs) {
        tabRect.x -= (TAB_RADIUS + 1);

    } else {
        // top tabs
        tabRect.y += TAB_RADIUS;
        visibleTab.y += TAB_RADIUS;
        visibleTab.height -= TAB_RADIUS;
    }

    dc.SetPen(tabInfo.IsActive() ? activeTabBgColour : bgColour);
    dc.SetBrush(tabInfo.IsActive() ? activeTabBgColour : bgColour);
    dc.DrawRoundedRectangle(tabRect, TAB_RADIUS);

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

    wxFont font = GetTabFont(tabInfo.IsActive() && IsUseBoldFont());
    wxColour text_colour = is_dark ? bgColour.ChangeLightness(170) : bgColour.ChangeLightness(30);
    fontDC.SetTextForeground(text_colour);
    fontDC.SetFont(font);

    wxRect textRect = dc.GetTextExtent(label);
    textRect = textRect.CenterIn(visibleTab);
    fontDC.DrawText(label, tabInfo.m_textX + rr.GetX(), textRect.GetY());
    if(style & kNotebook_CloseButtonOnActiveTab) {
        // use the adjusted tab rect and not the original one passed to us
        clTabInfo tab_info = tabInfo;
        tab_info.SetRect(tabRect);
        clTabColours c = colours;
        c.activeTabBgColour = activeTabBgColour;
        c.inactiveTabBgColour = bgColour;
        c.activeTabTextColour = text_colour;
        DrawButton(parent, dc, tab_info, c, buttonState);
    }
    return tabRect;
}
