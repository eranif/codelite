#include "cl_defs.h"

#if !USE_AUI_NOTEBOOK
#include "Notebook.h"
#include "clTabRenderer.h"
#include "clTabRendererSquare.h"
#include "editor_config.h"
#include <wx/dcmemory.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>

#if CL_BUILD
#include "drawingutils.h"
#endif

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
    wxColour faceColour = DrawingUtils::GetPanelBgColour();
    wxColour textColour = DrawingUtils::GetPanelTextColour();
    activeTabTextColour = textColour;
    inactiveTabTextColour = textColour;
    if(DrawingUtils::IsDark(faceColour)) {
        // Make the active tab draker
        activeTabBgColour = faceColour.ChangeLightness(60);
        activeTabPenColour = activeTabBgColour;

    } else {
        // Make it lighter
        activeTabBgColour = faceColour.ChangeLightness(150);
        activeTabPenColour = faceColour.ChangeLightness(70);
    }

    activeTabInnerPenColour = activeTabBgColour;
    if(DrawingUtils::IsDark(activeTabBgColour)) { activeTabTextColour = *wxWHITE; }

    tabAreaColour = faceColour;
    markerColour = DrawingUtils::GetCaptionColour();

    inactiveTabBgColour = tabAreaColour.ChangeLightness(95);
    inactiveTabPenColour = tabAreaColour.ChangeLightness(85);
    inactiveTabInnerPenColour = tabAreaColour;
}

bool clTabColours::IsDarkColours() const { return DrawingUtils::IsDark(activeTabBgColour); }

clTabInfo::clTabInfo(clTabCtrl* tabCtrl, size_t style, wxWindow* page, const wxString& text, const wxBitmap& bmp)
    : m_tabCtrl(tabCtrl)
    , m_label(text)
    , m_bitmap(bmp)
    , m_window(page)
    , m_active(false)
{
    CalculateOffsets(style);
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
{

    CalculateOffsets(0);
}

void clTabInfo::CalculateOffsets(size_t style)
{
    wxBitmap b(1, 1);
    wxMemoryDC memoryDC(b);
    m_bmpCloseX = wxNOT_FOUND;
    m_bmpCloseY = wxNOT_FOUND;

    wxDC& dc = memoryDC;
    wxFont font = clTabRenderer::GetTabFont();
    dc.SetFont(font);

    wxSize sz = dc.GetTextExtent(m_label);
    wxSize fixedHeight = dc.GetTextExtent("Tp");
    if(IS_VERTICAL_TABS(style)) {
        m_height = fixedHeight.GetHeight() + (5 * m_tabCtrl->GetArt()->ySpacer);
        m_height += 2;
    } else {
        m_height = fixedHeight.GetHeight() + (4 * m_tabCtrl->GetArt()->ySpacer);
    }

    m_width = 0;
    if(!IS_VERTICAL_TABS(style) || true) {
        m_width += m_tabCtrl->GetArt()->majorCurveWidth;
        m_width += m_tabCtrl->GetArt()->smallCurveWidth;
    }
    m_width += m_tabCtrl->GetArt()->xSpacer;

    // bitmap
    m_bmpX = wxNOT_FOUND;
    m_bmpY = wxNOT_FOUND;

    if(m_bitmap.IsOk()) {
        m_bmpX = m_width;
        m_width += m_tabCtrl->GetArt()->xSpacer;
        m_width += m_bitmap.GetScaledWidth();
        m_bmpY = ((m_height - m_bitmap.GetScaledHeight()) / 2);
    }

    // Text
    m_textX = m_width;
    m_textY = ((m_height - sz.y) / 2);
    m_width += sz.x;

    // x button
    if((style & kNotebook_CloseButtonOnActiveTab)) {
        m_width += m_tabCtrl->GetArt()->xSpacer;
        m_width += m_tabCtrl->GetArt()->xSpacer;
        m_bmpCloseX = m_width;
        m_bmpCloseY = ((m_height - 12) / 2) + 2;
        m_width += 12; // X button is 10 pixels in size
    }
    m_width += m_tabCtrl->GetArt()->xSpacer;

    if(!IS_VERTICAL_TABS(style) || true) {
        m_width += m_tabCtrl->GetArt()->majorCurveWidth;
        m_width += m_tabCtrl->GetArt()->smallCurveWidth;
    }

    // Update the rect width
    m_rect.SetWidth(m_width);

    if((style & kNotebook_RightTabs) || (style & kNotebook_LeftTabs)) {

        // swap the x and y coordinates
        wxSwap(m_height, m_width);
        wxSwap(m_bmpCloseY, m_bmpCloseX);
        wxSwap(m_bmpY, m_bmpX);
        wxSwap(m_textX, m_textY);

        m_width = m_tabCtrl->GetSize().GetWidth();
        m_textX += 1;

        m_rect.SetWidth(m_width);
        m_rect.SetHeight(m_height);
        m_rect.SetX(0);
        m_rect.SetY(0);
    }
}

void clTabInfo::SetBitmap(const wxBitmap& bitmap, size_t style)
{
    this->m_bitmap = bitmap;
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

clTabRenderer::clTabRenderer()
    : bottomAreaHeight(0)
    , majorCurveWidth(0)
    , smallCurveWidth(0)
    , overlapWidth(0)
    , verticalOverlapWidth(0)
    , xSpacer(10)
{
    ySpacer = EditorConfigST::Get()->GetOptions()->GetNotebookTabHeight();
}

wxFont clTabRenderer::GetTabFont() { return DrawingUtils::GetDefaultGuiFont(); }

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
        pt1.x += 1;
        pt2.x -= 1;
        DRAW_LINE(pt1, pt2);

        pt1.y += 1;
        pt2.y += 1;
        DRAW_LINE(pt1, pt2);
    }
}

void clTabRenderer::DrawButton(wxDC& dc, const wxRect& rect, const clTabColours& colours, eButtonState state)
{
    DrawingUtils::DrawButtonX(dc, nullptr, rect, colours.markerColour, state);
}

void clTabRenderer::DrawChevron(wxWindow* win, wxDC& dc, const wxRect& rect, const clTabColours& colours)
{
    DrawingUtils::DrawDropDownArrow(win, dc, rect, colours.activeTabPenColour);
}
#endif
