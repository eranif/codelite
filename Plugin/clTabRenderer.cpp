#include "cl_config.h"
#include "cl_defs.h"
#include "globals.h"

#include "ColoursAndFontsManager.h"
#include "Notebook.h"
#include "clTabRenderer.h"
#include "clTabRendererClassic.h"
#include "clTabRendererCurved.h"
#include "clTabRendererGTK3.h"
#include "clTabRendererSquare.h"
#include "cl_config.h"
#include "editor_config.h"
#include <wx/dcmemory.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include "clSystemSettings.h"

#if CL_BUILD
#include "drawingutils.h"
#endif

#define X_BUTTON_SIZE 14

clTabColours::clTabColours() { InitDarkColours(); }

void clTabColours::InitFromColours(const wxColour& baseColour, const wxColour& textColour)
{
#if CL_BUILD
    if(DrawingUtils::IsDark(baseColour)) {
        activeTabTextColour = "WHITE";
        activeTabBgColour = baseColour;
        activeTabPenColour = baseColour.ChangeLightness(80);
        activeTabInnerPenColour = baseColour.ChangeLightness(120);

        inactiveTabTextColour = "WHITE";
        inactiveTabBgColour = baseColour.ChangeLightness(110);
        inactiveTabPenColour = inactiveTabBgColour.ChangeLightness(80);
        inactiveTabInnerPenColour = inactiveTabPenColour; // inactiveTabBgColour.ChangeLightness(120);

        tabAreaColour = baseColour.ChangeLightness(130);
    } else {
        activeTabTextColour = "BLACK";
        activeTabBgColour = baseColour;
        activeTabPenColour = baseColour.ChangeLightness(80);
        activeTabInnerPenColour = "WHITE";

        inactiveTabTextColour = "BLACK";
        inactiveTabBgColour = baseColour.ChangeLightness(90);
        inactiveTabPenColour = inactiveTabBgColour.ChangeLightness(80);
        inactiveTabInnerPenColour = inactiveTabPenColour; // baseColour;

        tabAreaColour = baseColour.ChangeLightness(130);
    }
#else
    wxUnusedVar(baseColour);
    wxUnusedVar(textColour);
#endif
}

void clTabColours::InitDarkColours()
{
    InitLightColours();
    activeTabTextColour = "WHITE";
    activeTabBgColour = *wxBLACK;
}

void clTabColours::InitLightColours()
{
    wxColour faceColour;
    faceColour = DrawingUtils::GetPanelBgColour();
    wxColour textColour = DrawingUtils::GetPanelTextColour();
    activeTabTextColour = textColour;
    inactiveTabTextColour = clSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
    if(DrawingUtils::IsDark(faceColour)) {
        // Make the active tab draker
        activeTabBgColour = faceColour.ChangeLightness(60);
        activeTabPenColour = activeTabBgColour;
        inactiveTabTextColour = activeTabTextColour;

    } else {
        // Make it lighter
        activeTabBgColour = faceColour;
        activeTabPenColour = faceColour.ChangeLightness(70);
    }

    activeTabInnerPenColour = activeTabBgColour;
    if(DrawingUtils::IsDark(activeTabBgColour)) { activeTabTextColour = *wxWHITE; }

    tabAreaColour = faceColour;
    markerColour = clConfig::Get().Read("ActiveTabMarkerColour", wxColour("#dc7633"));

#ifdef __WXMSW__
    inactiveTabBgColour = faceColour.ChangeLightness(90);
#else
    inactiveTabBgColour = tabAreaColour.ChangeLightness(90);
#endif
    inactiveTabPenColour = tabAreaColour.ChangeLightness(70);
    inactiveTabInnerPenColour = tabAreaColour;
}

bool clTabColours::IsDarkColours() const { return DrawingUtils::IsDark(activeTabBgColour); }

clTabInfo::clTabInfo(clTabCtrl* tabCtrl, size_t style, wxWindow* page, const wxString& text, const wxBitmap& bmp)
    : m_bitmap(bmp)
    , m_tabCtrl(tabCtrl)
    , m_label(text)
    , m_window(page)
    , m_active(false)
    , m_textWidth(0)
{
    CalculateOffsets(style);
    if(m_bitmap.IsOk()) { m_disabledBitmp = DrawingUtils::CreateDisabledBitmap(m_bitmap); }
}

clTabInfo::clTabInfo(clTabCtrl* tabCtrl)
    : m_tabCtrl(tabCtrl)
    , m_window(NULL)
    , m_active(false)
    , m_textX(wxNOT_FOUND)
    , m_textY(wxNOT_FOUND)
    , m_bmpX(wxNOT_FOUND)
    , m_bmpY(wxNOT_FOUND)
    , m_bmpCloseX(wxNOT_FOUND)
    , m_bmpCloseY(wxNOT_FOUND)
    , m_textWidth(0)
{
    CalculateOffsets(0);
}

void clTabInfo::CalculateOffsets(size_t style, wxDC& dc)
{
    m_bmpCloseX = wxNOT_FOUND;
    m_bmpCloseY = wxNOT_FOUND;

    int Y_spacer = m_tabCtrl ? m_tabCtrl->GetArt()->ySpacer : 5;
    int X_spacer = m_tabCtrl ? m_tabCtrl->GetArt()->xSpacer : 5;
    int M_spacer = m_tabCtrl ? m_tabCtrl->GetArt()->majorCurveWidth : 5;
    int S_spacer = m_tabCtrl ? m_tabCtrl->GetArt()->smallCurveWidth : 2;

    wxFont font = clTabRenderer::GetTabFont(true);
    dc.SetFont(font);

    wxSize sz = dc.GetTextExtent(m_label);
    wxSize fixedHeight = dc.GetTextExtent("Tp");
    m_height = fixedHeight.GetHeight() + (4 * Y_spacer);

    // Make that the tab can contain at least the miimum bitmap height
    int bmpHeight = clTabRenderer::GetDefaultBitmapHeight(Y_spacer);
    m_height = wxMax(m_height, bmpHeight);

    m_width = 0;
    m_width += X_spacer;
    m_width += M_spacer;
    m_width += S_spacer;

    bool bVerticalTabs = IS_VERTICAL_TABS(style);
    // bitmap
    m_bmpX = wxNOT_FOUND;
    m_bmpY = wxNOT_FOUND;

    if(m_bitmap.IsOk() && !bVerticalTabs) {
        m_bmpX = m_width;
        m_width += X_spacer;
        m_width += m_bitmap.GetScaledWidth();
        m_bmpY = ((m_height - m_bitmap.GetScaledHeight()) / 2);
    }

    // Text
    m_textX = m_width;
    m_textY = ((m_height - sz.y) / 2);
    m_width += sz.x;
    m_textWidth = sz.x;

    // x button
    wxRect xrect;
    if((style & kNotebook_CloseButtonOnActiveTab)) {
        m_width += X_spacer;
        xrect = wxRect(m_width, 0, X_BUTTON_SIZE, X_BUTTON_SIZE);
        m_bmpCloseX = xrect.GetX();
        m_bmpCloseY = 0; // we will fix this later
        m_width += xrect.GetWidth();
    }

    m_width += X_spacer;
    m_width += M_spacer;
    m_width += S_spacer;
    if((style & kNotebook_UnderlineActiveTab) && bVerticalTabs) { m_width += 8; }
    // Update the rect width
    m_rect.SetWidth(m_width);
    m_rect.SetHeight(m_height);

    // If we got a X button, adjust its Y axis
    if(!xrect.IsEmpty()) {
        xrect = xrect.CenterIn(m_rect, wxVERTICAL);
        m_bmpCloseY = xrect.GetY();
    }
}

void clTabInfo::CalculateOffsets(size_t style)
{
    wxBitmap b(1, 1);
    wxMemoryDC memoryDC(b);
    wxGCDC gcdc(memoryDC);
    CalculateOffsets(style, gcdc);
}

void clTabInfo::SetBitmap(const wxBitmap& bitmap, size_t style)
{
    this->m_bitmap = bitmap;
    if(m_bitmap.IsOk()) { m_disabledBitmp = DrawingUtils::CreateDisabledBitmap(m_bitmap); }
    CalculateOffsets(style);
}

void clTabInfo::SetLabel(const wxString& label, size_t style)
{
    this->m_label = label;
    CalculateOffsets(style);
}

void clTabInfo::SetActive(bool active, size_t style)
{
    this->m_active = active;
    CalculateOffsets(style);
}

wxRect clTabInfo::GetCloseButtonRect() const
{
    wxRect xRect(GetRect().x + GetBmpCloseX(), GetRect().y + GetBmpCloseY(), clTabRenderer::GetXButtonSize(),
                 clTabRenderer::GetXButtonSize());
    return xRect;
}

clTabRenderer::clTabRenderer(const wxString& name)
    : bottomAreaHeight(0)
    , majorCurveWidth(0)
    , smallCurveWidth(0)
    , overlapWidth(0)
    , verticalOverlapWidth(0)
    , xSpacer(10)
    , m_name(name)
{
    ySpacer = EditorConfigST::Get()->GetOptions()->GetNotebookTabHeight();
}

wxFont clTabRenderer::GetTabFont(bool bold)
{
    wxFont f = DrawingUtils::GetDefaultGuiFont();
    if(bold) { f.SetWeight(wxFONTWEIGHT_BOLD); }
    return f;
}

#define DRAW_LINE(__p1, __p2) \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);

void clTabRenderer::ClearActiveTabExtraLine(clTabInfo::Ptr_t activeTab, wxDC& dc, const clTabColours& colours,
                                            size_t style)
{
    wxPoint pt1, pt2;
    dc.SetPen(colours.activeTabPenColour);
    if(style & kNotebook_LeftTabs) {
        dc.SetPen(colours.activeTabBgColour);
        pt1 = activeTab->GetRect().GetTopRight();
        pt2 = activeTab->GetRect().GetBottomRight();
        pt2.y -= 1;
        DRAW_LINE(pt1, pt2);

        pt1.x -= 1;
        pt2.x -= 1;
        DRAW_LINE(pt1, pt2);

    } else if(style & kNotebook_RightTabs) {
        // Right tabs
        dc.SetPen(colours.activeTabBgColour);
        pt1 = activeTab->GetRect().GetTopLeft();
        pt2 = activeTab->GetRect().GetBottomLeft();
        pt2.y -= 1;
        DRAW_LINE(pt1, pt2);

    } else if(style & kNotebook_BottomTabs) {
        // bottom tabs
        dc.SetPen(colours.activeTabBgColour);
        pt1 = activeTab->GetRect().GetTopLeft();
        pt2 = activeTab->GetRect().GetTopRight();
        pt1.x += 1;
        pt2.x -= 1;
        DRAW_LINE(pt1, pt2);

    } else {
        // Top tabs
        dc.SetPen(colours.activeTabBgColour);
        pt1 = activeTab->GetRect().GetBottomLeft();
        pt2 = activeTab->GetRect().GetBottomRight();
#ifndef __WXOSX__
        pt1.x += 1;
        pt2.x -= 1;
#else
        pt1.x += 2; // Skip the marker on the left side drawn in pen 3 pixel width
                    //        pt2.x -= 1;
#endif
        DRAW_LINE(pt1, pt2);
        pt1.y += 1;
        pt2.y += 1;
        DRAW_LINE(pt1, pt2);
    }
}

void clTabRenderer::DrawButton(wxWindow* win, wxDC& dc, const clTabInfo& tabInfo, const clTabColours& colours,
                               eButtonState state)
{
    // Draw the X button
    wxRect buttonRect = wxRect(tabInfo.m_bmpCloseX + tabInfo.GetRect().GetX(),
                               tabInfo.m_bmpCloseY + tabInfo.GetRect().GetY(), X_BUTTON_SIZE, X_BUTTON_SIZE);
    buttonRect = buttonRect.CenterIn(tabInfo.GetRect(), wxVERTICAL);
    DrawingUtils::DrawButtonX(dc, win, buttonRect, colours.activeTabTextColour, colours.activeTabBgColour, state);
}

void clTabRenderer::DrawChevron(wxWindow* win, wxDC& dc, const wxRect& rect, const clTabColours& colours)
{
    wxColour buttonColour;
    if(DrawingUtils::IsDark(colours.tabAreaColour)) {
        buttonColour = colours.tabAreaColour.ChangeLightness(150);
    } else {
        buttonColour = colours.tabAreaColour.ChangeLightness(50);
    }
    DrawingUtils::DrawDropDownArrow(win, dc, rect, buttonColour);
}

int clTabRenderer::GetDefaultBitmapHeight(int Y_spacer)
{
    int bmpHeight = 0;
    wxBitmap dummyBmp = clGetManager()->GetStdIcons()->LoadBitmap("cog");
    if(dummyBmp.IsOk()) { bmpHeight = dummyBmp.GetScaledHeight() + (2 * Y_spacer); }
    return bmpHeight;
}

clTabRenderer::Ptr_t clTabRenderer::CreateRenderer(size_t tabStyle)
{
    wxString tab = clConfig::Get().Read("TabStyle", wxString("GTK3"));
    wxString name = tab.Upper();
    if((tabStyle & kNotebook_LeftTabs) || (tabStyle & kNotebook_RightTabs)) {
        // Only these styles support vertical tabs
        if(name == "MINIMAL") {
            return clTabRenderer::Ptr_t(new clTabRendererSquare());
        } else {
            return clTabRenderer::Ptr_t(new clTabRendererGTK3());
        }
    }
    if(name == "MINIMAL") {
        return clTabRenderer::Ptr_t(new clTabRendererSquare());
    } else if(name == "TRAPEZOID") {
        return clTabRenderer::Ptr_t(new clTabRendererCurved());
    } else if(name == "GTK3") {
        return clTabRenderer::Ptr_t(new clTabRendererGTK3());
    } else {
        return clTabRenderer::Ptr_t(new clTabRendererClassic());
    }
}

wxArrayString clTabRenderer::GetRenderers()
{
    wxArrayString renderers;
    renderers.Add("GTK3");
    renderers.Add("MINIMAL");
    renderers.Add("TRAPEZOID");
    renderers.Add("DEFAULT");
    return renderers;
}

int clTabRenderer::GetMarkerWidth()
{
#ifdef __WXOSX__
    return 2;
#else
    return 3;
#endif
}

void clTabRenderer::DrawBackground(wxWindow* parent, wxDC& dc, const wxRect& clientRect, const clTabColours& colours,
                                   size_t style)
{
    wxUnusedVar(parent);
    wxUnusedVar(colours);
    wxUnusedVar(style);
    dc.SetPen(colours.tabAreaColour);
    dc.SetBrush(colours.tabAreaColour);
    dc.DrawRectangle(clientRect);
}

void clTabRenderer::FinaliseBackground(wxWindow* parent, wxDC& dc, const wxRect& clientRect,
                                       const clTabColours& colours, size_t style)
{
    wxUnusedVar(parent);
    wxUnusedVar(colours);
    wxUnusedVar(style);
    wxUnusedVar(dc);
    wxUnusedVar(clientRect);
}

void clTabRenderer::AdjustColours(clTabColours& colours, size_t style)
{
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
            } else if(lexer) {
                // Light theme
                colours.activeTabBgColour = lexer->GetProperty(0).GetBgColour();
                colours.activeTabInnerPenColour = colours.activeTabBgColour;
                colours.activeTabTextColour = *wxBLACK;
            }
        }
    }
}

void clTabRenderer::DrawMarker(wxDC& dc, const clTabInfo& tabInfo, const clTabColours& colours, size_t style)
{
    if((style & kNotebook_UnderlineActiveTab) == 0) { return; }
    wxPen markerPen(colours.markerColour);
    // Draw marker line if needed
    // wxRect confinedRect = parent->GetClientRect();
    wxPoint p1, p2;
    if((style & kNotebook_LeftTabs)) {
        p1 = tabInfo.GetRect().GetTopRight();
        p2 = tabInfo.GetRect().GetBottomRight();
        dc.SetPen(markerPen);
        DrawMarkerLine(dc, p1, p2, wxLEFT);
    } else if(style & kNotebook_RightTabs) {
        // Right tabs
        p1 = tabInfo.GetRect().GetTopLeft();
        p2 = tabInfo.GetRect().GetBottomLeft();
        dc.SetPen(markerPen);
        DrawMarkerLine(dc, p1, p2, wxRIGHT);
    } else if(style & kNotebook_BottomTabs) {
        // Bottom tabs
        p1 = tabInfo.GetRect().GetTopLeft();
        p2 = tabInfo.GetRect().GetTopRight();
        dc.SetPen(markerPen);
        DrawMarkerLine(dc, p1, p2, wxDOWN);
    } else {
        // Top tabs
        p1 = tabInfo.GetRect().GetBottomLeft();
        p2 = tabInfo.GetRect().GetBottomRight();
        dc.SetPen(markerPen);
        DrawMarkerLine(dc, p1, p2, wxUP);
    }
}

void clTabRenderer::DrawMarkerLine(wxDC& dc, const wxPoint& p1, const wxPoint& p2, wxDirection direction)
{
    const int width = GetMarkerWidth();
    wxPoint point1 = p1;
    wxPoint point2 = p2;
    for(int i = 0; i < width; ++i) {
        dc.DrawLine(point1, point2);
        if(direction == wxDOWN) {
            point1.y++;
            point2.y++;
        } else if(direction == wxUP) {
            point1.y--;
            point2.y--;
        } else if(direction == wxLEFT) {
            point1.x--;
            point2.x--;
        } else {
            // wxRIGHT
            point1.x++;
            point2.x++;
        }
    }
}

int clTabRenderer::GetXButtonSize() { return X_BUTTON_SIZE; }
