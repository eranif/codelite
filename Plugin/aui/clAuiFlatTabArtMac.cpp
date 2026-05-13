#if defined(__WXMAC__)
#include "clAuiFlatTabArt.hpp"

#if wxCHECK_VERSION(3, 3, 0)
#include "drawingutils.h"
#include "editor_config.h"

#include <wx/app.h>
#include <wx/bitmap.h>
#include <wx/bmpbndl.h>
#include <wx/settings.h>

namespace
{
/// Thin vertical divider colour between inactive tabs.
wxColour GetDividerColour(bool dark)
{
    if (dark)
        return wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE).ChangeLightness(110);
    else
        return wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE).ChangeLightness(95);
}
} // namespace

// ---------------------------------------------------------------------------
// clAuiFlatTabArt  –  iTerm2-Minimal-inspired tab art for macOS
// ---------------------------------------------------------------------------
//
// Visual design:
//   • The tab bar is a uniform surface (wxSYS_COLOUR_3DFACE).
//   • The active tab is drawn with the editor/content background so it
//     visually merges with the area below.  A thin accent-coloured underline
//     marks it.
//   • Inactive tabs share the bar colour; their text is dimmed.
//   • Tabs are separated only by a subtle 1 px vertical divider – no
//     borders, outlines, or 3-D effects.
// ---------------------------------------------------------------------------

struct clAuiFlatTabArt::Data {
    Data() = default;
    Data(const Data&) = default;
    Data& operator=(const Data&) = delete;

    void InitColours()
    {
        bool dark = DrawingUtils::IsDark(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
        m_bgActive = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
        m_fgActive = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
        m_fgNormal = dark ? wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT)
                          : wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT).ChangeLightness(97);
        m_fgHilite = dark ? *wxBLACK : wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT);
        m_HilitePenWidth = dark ? 1 : 2;
        m_fgDimmed = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);

        // The tab bar and inactive tabs are drawn darker than the active tab
        // so the active tab (which matches the editor content area) stands out.
        m_bgActive = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).ChangeLightness(dark ? 110 : 100);
        wxColour face = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        m_bgNormal = face.ChangeLightness(dark ? 80 : 92);
        m_bgWindow = m_bgNormal;
    }

    wxColour m_fgActive;
    wxColour m_bgActive;
    wxColour m_fgNormal;
    wxColour m_bgNormal;
    wxColour m_bgWindow;
    wxColour m_fgHilite;
    wxColour m_fgDimmed;
    int m_HilitePenWidth{1};
    static const int MARGIN = 3;
    static const int PADDING_X = 10;
};

// ---------------------------------------------------------------------------
// Life-cycle
// ---------------------------------------------------------------------------

clAuiFlatTabArt::clAuiFlatTabArt()
    : wxAuiTabArtBase()
    , m_data(new Data())
{
    UpdateColoursFromSystem();
    InitBitmaps();
}

clAuiFlatTabArt::clAuiFlatTabArt(clAuiFlatTabArt* other)
    : wxAuiTabArtBase(*other)
    , m_data(new Data(*other->m_data))
{
}

clAuiFlatTabArt::~clAuiFlatTabArt() { delete m_data; }

void clAuiFlatTabArt::UpdateColoursFromSystem() { m_data->InitColours(); }

void clAuiFlatTabArt::SetColour(const wxColour& colour) { m_data->m_fgNormal = colour; }

void clAuiFlatTabArt::SetActiveColour(const wxColour& colour) { m_data->m_fgActive = colour; }

wxColour clAuiFlatTabArt::GetButtonColour(wxAuiButtonId WXUNUSED(button), wxAuiPaneButtonState state) const
{
    switch (state) {
    case wxAUI_BUTTON_STATE_NORMAL:
        return m_data->m_fgActive;
    case wxAUI_BUTTON_STATE_DISABLED:
        return m_data->m_fgDimmed;
    default:
        wxFAIL_MSG("unsupported button state");
    }
    return {};
}

wxAuiTabArt* clAuiFlatTabArt::Clone() { return new clAuiFlatTabArt(this); }

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------

static int kThickness = 4;
static int GetThinRectY(const wxRect& rect, bool bottom_tabs)
{
    int y = bottom_tabs ? rect.GetBottom() - kThickness + 1 : rect.GetY() + kThickness - 1;
    return y;
}

static int GetThinRectTo(const wxRect& rect, bool bottom_tabs) { return bottom_tabs ? rect.GetY() : rect.GetBottom(); }

static wxColour GetDarkBorderColour(const wxColour& bgColour)
{
    wxColour border_colour =
        DrawingUtils::IsDark(bgColour) ? *wxBLACK : wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW);
    return border_colour;
}

void clAuiFlatTabArt::DoDrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect, bool with_bg)
{
    // Uniform tab-bar surface.
    if (with_bg) {
        dc.SetBrush(m_data->m_bgWindow);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(rect);
    }
    /*
        bool bottom_tabs = (m_flags & wxAUI_NB_BOTTOM);
        wxColour dark_pen = GetDarkBorderColour(m_data->m_bgWindow);
        {
            dc.SetPen(dark_pen);
            dc.SetBrush(m_data->m_bgActive);
            wxRect thin_rect = rect;
            thin_rect.SetHeight(kThickness);
            thin_rect.y -= 1;
            thin_rect.x -= 2;
            thin_rect.width += 4;
            dc.DrawRectangle(thin_rect);
        }
        {
            int y = GetThinRectTo(rect, bottom_tabs);
            dc.SetPen(wxPen(dark_pen, 1));
            dc.DrawLine(rect.GetLeft(), y, rect.GetRight(), y);
        }
    */
}

void clAuiFlatTabArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    DoDrawBackground(dc, wnd, rect, true);
}

void clAuiFlatTabArt::DrawBorder(wxDC& dc, wxWindow* WXUNUSED(wnd), const wxRect& rect)
{
    // No visible border – iTerm2 minimal style.
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetPen(m_data->m_bgWindow);
    dc.DrawRectangle(rect);
}

int clAuiFlatTabArt::DrawPageTab(wxDC& dc, wxWindow* wnd, wxAuiNotebookPage& page, const wxRect& rect)
{
    wxDCClipper clip(dc, rect);

    int xExtent = 0;
    const wxSize size = GetPageTabSize(dc, wnd, page, &xExtent);
    page.rect = wxRect(rect.GetPosition(), size);

    const bool dark = DrawingUtils::IsDark(m_data->m_bgWindow);

    // --- Determine the tab background colour ---
    wxColour bg = page.active ? m_data->m_bgActive : m_data->m_bgNormal;
    wxColour textColour = page.active ? m_data->m_fgActive : m_data->m_fgNormal;

    wxRect tab_rect = page.rect;
    tab_rect.Deflate(1);
    tab_rect.SetY(tab_rect.GetY() - 1);

    if (page.active) {
        bool is_dark = DrawingUtils::IsDark(m_data->m_bgWindow);
        // --- Fill the tab ---
        dc.SetBrush(bg);

        wxColour bar = m_data->m_bgActive;
        wxColour pen_colour = bar.ChangeLightness(dark ? 110 : 90);

        dc.SetPen(pen_colour);
        dc.DrawRoundedRectangle(tab_rect, static_cast<double>(tab_rect.GetHeight()) / 2.0);
    }

    // --- Close / action buttons ---
    int xStart = rect.x + wnd->FromDIP(Data::PADDING_X);

    int buttonsWidth = 0;
    for (const auto& button : page.buttons) {
        const wxBitmapBundle* const bb = GetButtonBitmapBundle(button);
        if (!bb)
            continue;
        buttonsWidth += bb->GetBitmapFor(wnd).GetLogicalWidth();
    }

    int xEnd = rect.x + size.x - wnd->FromDIP(Data::PADDING_X);
    if (buttonsWidth) {
        xEnd -= buttonsWidth;
        wxRect all_buttons_rect(xEnd, rect.GetY(), buttonsWidth, rect.GetHeight());
        all_buttons_rect.SetWidth(all_buttons_rect.GetWidth() + Data::PADDING_X + Data::MARGIN);
        all_buttons_rect = all_buttons_rect.CenterIn(tab_rect, wxVERTICAL);

        int buttonX = xEnd;
        for (auto& button : page.buttons) {
            const wxBitmapBundle* const bb = GetButtonBitmapBundle(button);
            if (!bb)
                continue;

            const wxBitmap bmp = bb->GetBitmapFor(wnd);
            const wxSize buttonSize = bmp.GetLogicalSize();

            button.rect.x = buttonX;
            button.rect.width = buttonSize.x;
            button.rect.height = buttonSize.y;

            auto button_rect = button.rect.CentreIn(all_buttons_rect, wxBOTH);
            button_rect.y += 1;

            dc.DrawBitmap(bmp, button_rect.GetPosition(), true);
            buttonX += buttonSize.x;
        }
        xEnd -= wnd->FromDIP(Data::MARGIN);
    }

    // --- Tab text ---
    dc.SetFont(page.active ? m_selectedFont : m_normalFont);
    dc.SetTextForeground(textColour);

    const wxString& text = wxControl::Ellipsize(page.caption, dc, wxELLIPSIZE_END, xEnd - xStart);
    const wxSize text_size = dc.GetTextExtent(text);
    const int textHeight = text_size.y;
    const int textWidth = text_size.x;
    wxRect text_rect{xStart, rect.y + (size.y - textHeight - 1) / 2, textWidth, textHeight};
    if (buttonsWidth == 0) {
        // No buttons, centre the text
        text_rect = text_rect.CenterIn(tab_rect);
    }
    dc.DrawText(text, text_rect.GetTopLeft());
    return xExtent;
}

// ---------------------------------------------------------------------------
// Sizing
// ---------------------------------------------------------------------------

wxSize clAuiFlatTabArt::GetPageTabSize(wxReadOnlyDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page, int* xExtent)
{
    dc.SetFont(m_measuringFont);
    wxSize size = dc.GetTextExtent(page.caption);

    bool firstButton = true;
    for (const auto& button : page.buttons) {
        const wxBitmapBundle* const bb = GetButtonBitmapBundle(button);
        if (!bb)
            continue;
        if (firstButton) {
            size.x += wnd->FromDIP(Data::MARGIN);
            firstButton = false;
        }
        size.x += bb->GetBitmapFor(wnd).GetLogicalWidth();
    }

    if (page.bitmap.IsOk()) {
        const wxSize bitmapSize = page.bitmap.GetPreferredLogicalSizeFor(wnd);
        size.x += bitmapSize.x + wnd->FromDIP(Data::MARGIN);
        size.IncTo(bitmapSize);
    }

    // Add padding around the contents. The vertical padding is derived from
    // the user-configurable "Notebook Tab Height" preference
    // (Settings -> Preferences -> Tabs) so tabs honour the chosen size.
    // The formula mirrors clTabRenderer.cpp: ySpacer = option + 2.
    const int paddingY = EditorConfigST::Get()->GetOptions()->GetNotebookTabHeight() + 2;
    size += wnd->FromDIP(wxSize(2 * Data::PADDING_X, 2 * paddingY));

    if (m_flags & wxAUI_NB_TAB_FIXED_WIDTH)
        size.x = m_fixedTabWidth;

    if (xExtent)
        *xExtent = size.x;
    return size;
}

int clAuiFlatTabArt::GetBestTabCtrlSize(wxWindow* wnd,
                                        const wxAuiNotebookPageArray& pages,
                                        const wxSize& requiredBmpSize)
{
    wxInfoDC dc(wnd);
    dc.SetFont(m_measuringFont);

    wxBitmapBundle measureBmp;
    if (requiredBmpSize.IsFullySpecified())
        measureBmp = wxBitmap(requiredBmpSize);

    wxSize size;
    for (auto page : pages) {
        if (measureBmp.IsOk())
            page.bitmap = measureBmp;
        page.active = true;
        size.IncTo(GetPageTabSize(dc, wnd, page));
    }
    return size.y;
}

int clAuiFlatTabArt::GetIndentSize() { return 0; }

#endif
#endif
