#include "clTabRendererGTK3.h"

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
    xSpacer = 10;
    ySpacer = EditorConfigST::Get()->GetOptions()->GetNotebookTabHeight();
}

clTabRendererGTK3::~clTabRendererGTK3() {}

void clTabRendererGTK3::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                             const clTabColours& colours, size_t style)
{
    wxColour inactiveTabPenColour = colours.inactiveTabPenColour;

    wxColour bgColour(colours.inactiveTabBgColour);
    wxColour penColour(colours.inactiveTabBgColour);

    wxFont font = GetTabFont();
    fontDC.SetTextForeground(tabInfo.IsActive() ? colours.inactiveTabTextColour
                                                : colours.inactiveTabBgColour.ChangeLightness(50));
    font.SetWeight(wxFONTWEIGHT_BOLD);
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
        dc.DrawBitmap(tabInfo.GetBitmap(), tabInfo.m_bmpX + rr.GetX(), tabInfo.m_bmpY + rr.GetY());
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
        DrawButton(dc,
                   wxRect(tabInfo.m_bmpCloseX + rr.GetX(), tabInfo.m_bmpCloseY + rr.GetY(), CLOSE_BUTTON_SIZE,
                          CLOSE_BUTTON_SIZE),
                   colours, eButtonState::kNormal);
    }
}

void clTabRendererGTK3::DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc,
                                       const clTabColours& colours, size_t style)
{
    wxPen markerPen(colours.markerColour, clTabRenderer::GetMarkerWidth());
    bool underlineTab = (style & kNotebook_UnderlineActiveTab);

    // Draw marker line if needed
    // wxRect confinedRect = parent->GetClientRect();
    if(underlineTab) {
        wxPoint p1, p2;
        if((style & kNotebook_LeftTabs)) {
            p1 = activeTab->GetRect().GetTopRight();
            p2 = activeTab->GetRect().GetBottomRight();
            dc.SetPen(markerPen);
            dc.DrawLine(p1, p2);
            dc.SetPen(colours.activeTabPenColour);
            dc.DrawLine(clientRect.GetRightTop(), clientRect.GetRightBottom());
        } else if(style & kNotebook_RightTabs) {
            // Right tabs
            p1 = activeTab->GetRect().GetTopLeft();
            p2 = activeTab->GetRect().GetBottomLeft();
            dc.SetPen(markerPen);
            dc.DrawLine(p1, p2);
            dc.SetPen(colours.activeTabPenColour);
            dc.DrawLine(clientRect.GetLeftTop(), clientRect.GetLeftBottom());
        } else if(style & kNotebook_BottomTabs) {
            // Bottom tabs
            p1 = activeTab->GetRect().GetTopLeft();
            p2 = activeTab->GetRect().GetTopRight();
            p1.x += 5;
            p2.x -= 5;
            dc.SetPen(markerPen);
            dc.DrawLine(p1, p2);
            dc.SetPen(colours.activeTabPenColour);
            dc.DrawLine(clientRect.GetTopRight(), clientRect.GetTopLeft());
        } else {
            // Top tabs
            p1 = activeTab->GetRect().GetBottomLeft();
            p2 = activeTab->GetRect().GetBottomRight();
            p1.x += 5;
            p2.x -= 5;
            dc.SetPen(markerPen);
            dc.DrawLine(p1, p2);
            dc.SetPen(colours.activeTabPenColour);
            dc.DrawLine(clientRect.GetBottomRight(), clientRect.GetBottomLeft());
        }
    }
}
