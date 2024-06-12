#include "clTabRenderer.h"

#include "clGenericNotebook.hpp"
#include "clSystemSettings.h"
#include "clTabRendererMinimal.hpp"
#include "cl_config.h"
#include "editor_config.h"
#include "globals.h"

#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>

#if CL_BUILD
#include "drawingutils.h"
#endif

#define TAB_FIXED_WIDTH() (m_tabCtrl ? (m_tabCtrl->IsFixedWidth() ? m_tabCtrl->GetLabelFixedWidth() : 0) : 0)

namespace
{
void GetTabColours(const clTabColours& colours, size_t style, wxColour* activeTabBgColour, wxColour* bgColour)
{
    *bgColour = colours.tabAreaColour;
    *activeTabBgColour = colours.activeTabBgColour;
    wxUnusedVar(style);
}
int X_BUTTON_SIZE = 20;
void SetBestXButtonSize(wxWindow* win) { wxUnusedVar(win); }

} // namespace

clTabColours::clTabColours() { UpdateColours(0); }

void clTabColours::UpdateColours(size_t notebookStyle)
{
    wxUnusedVar(notebookStyle);
    wxColour base_colour = clSystemSettings::GetDefaultPanelColour();
    bool is_dark = DrawingUtils::IsDark(base_colour);

    tabAreaColour = base_colour.ChangeLightness(is_dark ? 60 : 90);

#ifdef __WXMAC__
    activeTabBgColour = clSystemSettings::GetDefaultPanelColour();
#else
    activeTabBgColour = base_colour.ChangeLightness(is_dark ? 107 : 100);
#endif

    activeTabTextColour = clSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);
    activeTabPenColour = base_colour;
    activeTabInnerPenColour = activeTabPenColour;

    // inactive tab colours
    inactiveTabTextColour = activeTabTextColour.ChangeLightness(is_dark ? 85 : 115);
    inactiveTabBgColour = tabAreaColour;
    inactiveTabPenColour = tabAreaColour.ChangeLightness(90);
    inactiveTabInnerPenColour = inactiveTabBgColour;
    markerColour = clConfig::Get().Read("ActiveTabMarkerColour", wxColour("#dc7633"));
}

bool clTabColours::IsDarkColours() const { return DrawingUtils::IsDark(activeTabBgColour); }

clTabInfo::clTabInfo(clTabCtrl* tabCtrl, size_t style, wxWindow* page, const wxString& text, int bmp)
    : m_bitmap(bmp)
    , m_tabCtrl(tabCtrl)
    , m_label(text)
    , m_window(page)
    , m_active(false)
    , m_textWidth(0)
    , m_xButtonState(eButtonState::kDisabled)
{
    SetBestXButtonSize(tabCtrl);
    CalculateOffsets(style, TAB_FIXED_WIDTH());
    CreateDisabledBitmap();
}

void clTabInfo::CreateDisabledBitmap()
{
    // the disabled image as the same index as the normal one
    m_disabledBitmp = m_bitmap;
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
    , m_xButtonState(eButtonState::kDisabled)
{
    SetBestXButtonSize(tabCtrl);
    CalculateOffsets(0, 0);
}

void clTabInfo::CalculateOffsets(size_t style, size_t max_width, wxDC& dc)
{
    m_bmpCloseX = wxNOT_FOUND;
    m_bmpCloseY = wxNOT_FOUND;

    int Y_spacer = m_tabCtrl ? m_tabCtrl->GetArt()->ySpacer : DEFAULT_YSPACER;
    int X_spacer = m_tabCtrl ? m_tabCtrl->GetArt()->xSpacer : DEFAULT_XSPACER;
    bool using_bold_font = m_tabCtrl ? m_tabCtrl->GetArt()->IsUseBoldFont() : false;

    wxDCFontChanger font_changer(dc);
    wxFont font = clTabRenderer::GetTabFont(using_bold_font);
    dc.SetFont(font);

    // On vertical tabs, use the short label
    wxSize sz = dc.GetTextExtent(GetBestLabel(style));
    wxSize fixedHeight = dc.GetTextExtent("Tp");
    m_height = fixedHeight.GetHeight() + (4 * Y_spacer);

    // Make that the tab can contain at least the minimum bitmap height
    int bmpHeight = clTabRenderer::GetDefaultBitmapHeight(Y_spacer);
    m_height = wxMax(m_height, bmpHeight);

    m_width = 0;
    m_width += X_spacer;

    // bitmap
    m_bmpX = wxNOT_FOUND;
    m_bmpY = wxNOT_FOUND;

    // x button
    wxRect xrect;
    if ((style & kNotebook_CloseButtonOnActiveTab)) {
        xrect = wxRect(m_width, 0, X_BUTTON_SIZE, X_BUTTON_SIZE);
        m_bmpCloseX = xrect.GetX();
        m_bmpCloseY = 0; // we will fix this later
        m_width += xrect.GetWidth() + m_tabCtrl->FromDIP(5);
    }

    // Text
    m_textX = m_width;
    m_textY = ((m_height - sz.y) / 2);

    bool isFixedWidth = style & kNotebook_FixedWidth;
    m_width += (isFixedWidth ? max_width : sz.x);
    m_textWidth = (isFixedWidth ? max_width : sz.x);
    m_width += X_spacer;

    if (HasBitmap()) {
        const wxBitmap& bmp = m_tabCtrl->GetBitmaps()->Get(m_bitmap, false);
        m_bmpX = m_width;
        m_width += bmp.GetScaledWidth();
        m_bmpY = ((m_height - bmp.GetScaledHeight()) / 2);
        m_width += X_spacer;
    }

    // Update the rect width
    m_rect.SetWidth(m_width);
    m_rect.SetHeight(m_height);

    // If we got a X button, adjust its Y axis
    if (!xrect.IsEmpty()) {
        xrect = xrect.CenterIn(m_rect, wxVERTICAL);
        m_bmpCloseY = xrect.GetY();
    }
}

void clTabInfo::CalculateOffsets(size_t style, size_t max_width)
{
    if (m_tabCtrl) {
        wxBitmap bmp{ 1, 1 };
        wxMemoryDC memDC(bmp);
        wxGCDC gcdc;
        wxDC& dc = DrawingUtils::GetGCDC(memDC, gcdc);

        dc.SetFont(DrawingUtils::GetDefaultGuiFont());
        CalculateOffsets(style, max_width, dc);
    }
}

void clTabInfo::SetBitmap(int bitmap, size_t style)
{
    this->m_bitmap = bitmap;
    CreateDisabledBitmap();
    CalculateOffsets(style, TAB_FIXED_WIDTH());
}

void clTabInfo::SetLabel(const wxString& label, size_t style)
{
    this->m_label = label;
    CalculateOffsets(style, TAB_FIXED_WIDTH());
}

void clTabInfo::SetActive(bool active, size_t style)
{
    this->m_active = active;
    this->m_xButtonState = active ? eButtonState::kNormal : eButtonState::kDisabled;
    CalculateOffsets(style, TAB_FIXED_WIDTH());
}

wxRect clTabInfo::GetCloseButtonRect() const
{
    wxRect xRect(GetBmpCloseX() + GetRect().x, 0, clTabRenderer::GetXButtonSize(), clTabRenderer::GetXButtonSize());
    xRect.Inflate(2);
    return xRect.CenterIn(GetRect(), wxVERTICAL);
}

const wxString& clTabInfo::GetBestLabel(size_t style) const { return m_label; }

const wxBitmap& clTabInfo::GetBitmap(int index, bool disabled) const
{
    if (!m_tabCtrl) {
        return wxNullBitmap;
    }
    return m_tabCtrl->GetBitmaps()->Get(index, disabled);
}

bool clTabInfo::HasDisableBitmap() const
{
    return m_tabCtrl && m_tabCtrl->GetBitmaps()->Get(m_disabledBitmp, true).IsOk();
}
bool clTabInfo::HasBitmap() const { return m_tabCtrl && m_tabCtrl->GetBitmaps()->Get(m_bitmap, false).IsOk(); }

std::unordered_map<wxString, clTabRenderer*> clTabRenderer::ms_Renderes;

clTabRenderer::clTabRenderer(const wxString& name, const wxWindow* parent)
    : bottomAreaHeight(0)
    , m_name(name)
{
    ySpacer = EditorConfigST::Get()->GetOptions()->GetNotebookTabHeight() + 2;
}

wxFont clTabRenderer::GetTabFont(bool bold)
{
    wxFont f = DrawingUtils::GetDefaultGuiFont();
    if (bold) {
        f.SetWeight(wxFONTWEIGHT_BOLD);
    }
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
    if (style & kNotebook_BottomTabs) {
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

    wxColour text_colour = colours.activeTabTextColour;
    wxString unicode_symbol = wxT("\u2715");
    if (tabInfo.IsModified()) {
        // instead of drawing the standard X we draw
        // a round circele marked in RED indicating that this
        // tab is modified
        text_colour = *wxRED;
        unicode_symbol = wxT("\u25CF");
    }

#if defined(__WXMAC__)
    buttonRect.y -= 3;
    buttonRect.height += 3;

#elif defined(__WXMSW__)
    buttonRect.y -= 1;
#endif

    DrawingUtils::DrawButtonX(dc, win, buttonRect, text_colour,
                              tabInfo.IsActive() ? colours.activeTabBgColour : colours.inactiveTabBgColour, state,
                              unicode_symbol);
}

void clTabRenderer::DrawChevron(wxWindow* win, wxDC& dc, const wxRect& rect, const clTabColours& colours)
{
    wxColour buttonColour;
    if (DrawingUtils::IsDark(colours.tabAreaColour)) {
        buttonColour = colours.tabAreaColour.ChangeLightness(150);
    } else {
        buttonColour = colours.tabAreaColour.ChangeLightness(50);
    }

    int flags = wxCONTROL_NONE;
    DrawingUtils::DrawDropDownArrow(win, dc, rect, flags, buttonColour);
}

int clTabRenderer::GetDefaultBitmapHeight(int Y_spacer)
{
    int bmpHeight = 0;
    wxBitmap dummyBmp = clGetManager()->GetStdIcons()->LoadBitmap("cog");
    if (dummyBmp.IsOk()) {
        bmpHeight = dummyBmp.GetScaledHeight() + (2 * Y_spacer);
    }
    return bmpHeight;
}

clTabRenderer::Ptr_t clTabRenderer::CreateRenderer(const wxWindow* win, size_t tabStyle)
{
    if (ms_Renderes.empty()) {
        RegisterRenderer(new clTabRendererMinimal(win));
    }
    clTabRenderer::Ptr_t renderer;
    renderer = Create(win, "MINIMAL");
    return renderer;
}

wxArrayString clTabRenderer::GetRenderers()
{
    if (ms_Renderes.empty()) {
        RegisterRenderer(new clTabRendererMinimal(nullptr));
    }

    wxArrayString renderers;
    renderers.reserve(ms_Renderes.size());

    for (auto vt : ms_Renderes) {
        renderers.Add(vt.first);
    }
    renderers.Sort();
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
#ifdef __WXOSX__
    dc.SetPen(colours.tabAreaColour);
#else
    dc.SetPen(colours.inactiveTabPenColour);
#endif
    dc.SetBrush(colours.tabAreaColour);
    dc.DrawRectangle(clientRect);

#ifndef __WXOSX__
    bool isBottom = style & kNotebook_BottomTabs;

    wxPoint p1, p2;
    if (isBottom) {
        p1 = clientRect.GetTopLeft();
        p2 = clientRect.GetTopRight();
    } else {
        p1 = clientRect.GetBottomLeft();
        p2 = clientRect.GetBottomRight();
    }
    dc.SetPen(colours.inactiveTabPenColour);
    dc.DrawLine(p1, p2);
#endif
}

void clTabRenderer::FinaliseBackground(wxWindow* parent, wxDC& dc, const wxRect& clientRect,
                                       const wxRect& activeTabRect, const clTabColours& colours, size_t style)
{
    wxUnusedVar(parent);
    wxUnusedVar(activeTabRect);

#if defined(__WXMAC__) || defined(__WXMSW__)
    wxColour bg_colour;
    wxColour active_tab_colour;
    GetTabColours(colours, style, &active_tab_colour, &bg_colour);
    bool is_dark = DrawingUtils::IsDark(bg_colour);
    dc.SetPen(is_dark ? *wxBLACK_PEN : wxPen(bg_colour.ChangeLightness(90)));
    if (!(style & kNotebook_BottomTabs)) {
        // draw black dark line from top left side to the right side
        // of the notebook bar, skipping the active tab
        wxPoint from, to;
        from = clientRect.GetTopLeft();
        to = activeTabRect.GetTopLeft();
        to.x -= 1;

        dc.DrawLine(from, to);

        from = activeTabRect.GetTopRight();
        from.x += 1;

        to = clientRect.GetTopRight();

        dc.DrawLine(from, to);

        // Draw a black line at the bottom of the active tab
        dc.DrawLine(activeTabRect.GetBottomLeft(), activeTabRect.GetBottomRight());
    }
#endif
}

void clTabRenderer::AdjustColours(clTabColours& colours, size_t style)
{
    wxUnusedVar(style);
    wxUnusedVar(colours);
}

void clTabRenderer::DrawMarker(wxDC& dc, const clTabInfo& tabInfo, const clTabColours& colours, size_t style)
{
    if ((style & kNotebook_UnderlineActiveTab) == 0) {
        return;
    }

    bool is_dark = DrawingUtils::IsDark(colours.tabAreaColour);
#ifdef __WXMAC__
    wxColour marker_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
#else
    wxColour marker_colour = colours.tabAreaColour.ChangeLightness(is_dark ? 130 : 70);
#endif
    wxPen markerPen(marker_colour);

    // Draw marker line if needed
    // wxRect confinedRect = parent->GetClientRect();
    bool isGTK3 = true;
    wxDirection direction;
    wxPoint p1, p2;
    if (style & kNotebook_BottomTabs) {
        // Bottom tabs
        if (isGTK3) {
            direction = wxDOWN;
            p1 = tabInfo.GetRect().GetTopLeft();
            p2 = tabInfo.GetRect().GetTopRight();
        } else {
            direction = wxRIGHT;
            p1 = tabInfo.GetRect().GetTopLeft();
            p2 = tabInfo.GetRect().GetBottomLeft();
            p1.x--;
            p2.x--;
        }
        dc.SetPen(markerPen);
        DrawMarkerLine(dc, p1, p2, direction);
    } else {
        // Top tabs
        if (isGTK3) {
            direction = wxUP;
            p1 = tabInfo.GetRect().GetBottomLeft();
            p2 = tabInfo.GetRect().GetBottomRight();

        } else {
            direction = wxRIGHT;
            p1 = tabInfo.GetRect().GetTopLeft();
            p2 = tabInfo.GetRect().GetBottomLeft();
            p1.x--;
            p2.x--;
        }
        dc.SetPen(markerPen);
        DrawMarkerLine(dc, p1, p2, direction);
    }
}

void clTabRenderer::DrawMarkerLine(wxDC& dc, const wxPoint& p1, const wxPoint& p2, wxDirection direction)
{
    const int width = GetMarkerWidth();
    wxPoint point1 = p1;
    wxPoint point2 = p2;
    for (int i = 0; i < width; ++i) {
        dc.DrawLine(point1, point2);
        if (direction == wxDOWN) {
            point1.y++;
            point2.y++;
        } else if (direction == wxUP) {
            point1.y--;
            point2.y--;
        } else if (direction == wxLEFT) {
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

void clTabRenderer::RegisterRenderer(clTabRenderer* renderer)
{
    if (!renderer) {
        return;
    }
    if (ms_Renderes.count(renderer->GetName())) {
        return;
    }
    ms_Renderes.insert({ renderer->GetName(), renderer });
}

clTabRenderer* clTabRenderer::Create(const wxWindow* parent, const wxString& name)
{
    if (ms_Renderes.count(name) == 0) {
        return nullptr;
    }
    return ms_Renderes.find(name)->second->New(parent);
}
