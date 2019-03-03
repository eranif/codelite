#include "clTabRendererGTK3.h"

#include "ColoursAndFontsManager.h"
#include "drawingutils.h"
#include "editor_config.h"
#include <wx/dcmemory.h>
#include <wx/font.h>
#include <wx/settings.h>
#include "clSystemSettings.h"

#define DRAW_LINE(__p1, __p2) \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);

clTabRendererGTK3::clTabRendererGTK3()
    : clTabRenderer("GTK3")
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
    ySpacer = EditorConfigST::Get()->GetOptions()->GetNotebookTabHeight() + 2;
}

clTabRendererGTK3::~clTabRendererGTK3() {}

void clTabRendererGTK3::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                             const clTabColours& colours, size_t style, eButtonState buttonState)
{
    wxColour inactiveTabPenColour = colours.inactiveTabPenColour;

    wxColour activeTabBgColour = DrawingUtils::IsDark(colours.tabAreaColour)
                                     ? colours.tabAreaColour.ChangeLightness(105)
                                     : colours.tabAreaColour.ChangeLightness(125);
    wxColour bgColour(colours.tabAreaColour);
    wxColour penColour(colours.tabAreaColour);

    wxFont font = GetTabFont(false);
    fontDC.SetTextForeground(tabInfo.IsActive() ? colours.activeTabTextColour : colours.inactiveTabTextColour);
    // font.SetWeight(wxFONTWEIGHT_BOLD);
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
        if((textEndCoord + clTabRenderer::GetMarkerWidth()) > tabEndCoord) {
            int newSize = tabEndCoord - tabInfo.m_textX;
            DrawingUtils::TruncateText(tabInfo.m_label, newSize, dc, label);
        }
    }

    fontDC.DrawText(label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY + rr.GetY());
    if(tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
        // Draw the X button
        DrawButton(parent, dc, tabInfo, colours, buttonState);
    }
    if(tabInfo.IsActive()) { DrawMarker(dc, tabInfo, colours, style | kNotebook_UnderlineActiveTab); }
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

void clTabRendererGTK3::DrawBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const clTabColours& colours,
                                       size_t style)
{
    wxColour bgColour(colours.tabAreaColour);
    dc.SetPen(bgColour);
    dc.SetBrush(bgColour);
    dc.DrawRectangle(rect);
}

void clTabRendererGTK3::FinaliseBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const clTabColours& colours,
                                           size_t style)
{
    wxUnusedVar(parent);
    wxUnusedVar(dc);
    wxUnusedVar(rect);
    wxUnusedVar(colours);
    wxUnusedVar(style);
}

void clTabRendererGTK3::AdjustColours(clTabColours& colours, size_t style)
{
    bool useDefaults = true;
    if(style & kNotebook_DynamicColours) {
        wxString globalTheme = ColoursAndFontsManager::Get().GetGlobalTheme();
        if(!globalTheme.IsEmpty()) {
            LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++", globalTheme);
            if(lexer && lexer->IsDark()) {
                // Dark theme, update all the colours
                colours.activeTabBgColour = lexer->GetProperty(0).GetBgColour();
                colours.activeTabInnerPenColour = colours.activeTabBgColour;
                colours.activeTabPenColour = colours.activeTabBgColour.ChangeLightness(110);
                colours.activeTabTextColour = *wxWHITE;
                colours.inactiveTabTextColour = wxColour("#909497");
                colours.tabAreaColour = colours.activeTabBgColour.ChangeLightness(110);
                useDefaults = false;
            }
        }
    }

    if(useDefaults) {
        colours.activeTabBgColour = clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        colours.activeTabInnerPenColour = colours.activeTabBgColour;
        colours.tabAreaColour = colours.inactiveTabBgColour;
        colours.activeTabPenColour = colours.activeTabBgColour;
        colours.activeTabTextColour = DrawingUtils::GetPanelTextColour();
        colours.inactiveTabTextColour = DrawingUtils::IsDark(colours.activeTabTextColour)
                                            ? colours.activeTabTextColour.ChangeLightness(140)
                                            : colours.activeTabTextColour.ChangeLightness(60);
    }
}
