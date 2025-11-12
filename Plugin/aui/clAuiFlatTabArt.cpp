#include "clAuiFlatTabArt.hpp"

#if wxCHECK_VERSION(3, 3, 0)
#include "drawingutils.h"

#include <wx/app.h>
#include <wx/bitmap.h>
#include <wx/bmpbndl.h>
#include <wx/settings.h>

namespace
{
void IndentPressedBitmap(wxWindow* wnd, wxRect* rect, int button_state)
{
    if (button_state == wxAUI_BUTTON_STATE_PRESSED) {
        rect->Offset(wnd->FromDIP(wxPoint(1, 1)));
    }
}
} // namespace

// clAuiFlatTabArt
// ----------------------------------------------------------------------------

// This private struct contains colours etc used by clAuiFlatTabArt. Having it
// reduces physical coupling and will allow us to modify its implementation
// without breaking the ABI.
struct clAuiFlatTabArt::Data {
    Data() = default;
    Data(const Data&) = default;

    // It could be made default-assignable too, but there should be no need to
    // ever assign objects of this class, so don't define the assignment
    // operator.
    Data& operator=(const Data&) = delete;

    void InitColours()
    {
        m_fgActive = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
        m_bgActive = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);

        bool is_dark = DrawingUtils::IsDark(m_bgActive);

        wxColour colour_face = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        m_fgNormal = m_fgActive.ChangeLightness(is_dark ? 60 : 115);
        m_bgNormal = colour_face.ChangeLightness(is_dark ? 70 : 100);
        m_bgWindow = m_bgNormal;
        m_fgHilite = wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT);
        m_fgDimmed = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
    }

    // Colours used for drawing.
    wxColour m_fgActive; // Text of the current tab.
    wxColour m_bgActive; // Background of the current tab.
    wxColour m_fgNormal; // Text of the non-current tabs.
    wxColour m_bgNormal; // Background of normal tabs.
    wxColour m_bgWindow; // Background showing through between the tabs.
    wxColour m_fgHilite; // Used for highlighting the current tab.
    wxColour m_fgDimmed; // Text or buttons on the non-current tabs.

    // Extra space in DIPs.
    static const int MARGIN = 3;
    static const int PADDING_X = 8;
    static const int PADDING_Y = 5;
};

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

void clAuiFlatTabArt::DrawBackground(wxDC& dc, wxWindow* WXUNUSED(wnd), const wxRect& rect)
{
    dc.SetBrush(m_data->m_bgWindow);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect);
}

void clAuiFlatTabArt::DrawBorder(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
#if defined(__WXMAC__) || defined(__WXMSW__)
    wxDCBrushChanger brush_changer{dc, *wxTRANSPARENT_BRUSH};
    wxDCPenChanger pen_changer{dc, wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)};
    dc.DrawRectangle(rect);
#else
    wxAuiTabArtBase::DrawBorder(dc, wnd, rect);
#endif
}
int clAuiFlatTabArt::DrawPageTab(wxDC& dc, wxWindow* wnd, wxAuiNotebookPage& page, const wxRect& rect)
{
    // Clip everything we do here to the provided rectangle.
    wxDCClipper clip(dc, rect);

    // Compute the size of the tab.
    int xExtent = 0;
    const wxSize size = GetPageTabSize(dc, wnd, page, &xExtent);

    page.rect = wxRect(rect.GetPosition(), size);

    // Draw the tab background and highlight it if it's active.
    wxColour bg = page.active ? m_data->m_bgActive : m_data->m_bgNormal;
    wxColour activeTabTextColour = wxNullColour;
    if (page.active && page.window && dynamic_cast<wxStyledTextCtrl*>(page.window)) {
        // Use the background colour of the control itself.
        auto stc = dynamic_cast<wxStyledTextCtrl*>(page.window);
        bg = stc->StyleGetBackground(0);
        if (DrawingUtils::IsDark(bg)) {
            activeTabTextColour = wxColour("WHITE").ChangeLightness(90);
        } else {
            activeTabTextColour = wxColour("BLACK").ChangeLightness(110);
        }
    }

    dc.SetBrush(bg);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(page.rect);

    // Draw the outline only on the left/top sides to avoid double borders.
    dc.SetPen(m_data->m_bgWindow);
    dc.DrawLine(page.rect.GetLeft(), page.rect.GetTop(), page.rect.GetLeft(), page.rect.GetBottom());
    dc.DrawLine(page.rect.GetLeft(), page.rect.GetTop(), page.rect.GetRight(), page.rect.GetTop());

    if (page.active) {
        dc.SetBrush(m_data->m_fgHilite);
        dc.SetPen(*wxTRANSPARENT_PEN);

        // 1px border is too thin to be noticeable, so make it thicker.
        const int THICKNESS = wnd->FromDIP(2);

        const int y = m_flags & wxAUI_NB_BOTTOM ? page.rect.GetBottom() - THICKNESS : page.rect.GetTop();

        dc.DrawRectangle(page.rect.GetLeft() + 1, y, page.rect.GetWidth() - 1, THICKNESS);
    }

    // Draw the icon, if any.
    int xStart = rect.x + wnd->FromDIP(Data::PADDING_X);
#if 0
    if (page.bitmap.IsOk()) {
        const wxBitmap bmp = page.bitmap.GetBitmapFor(wnd);
        const wxSize bitmapSize = bmp.GetLogicalSize();

        dc.DrawBitmap(bmp, xStart, rect.y + (size.y - bitmapSize.y - 1) / 2, true /* use mask */);

        xStart += bitmapSize.x + wnd->FromDIP(Data::MARGIN);
    }
#endif

    // Draw buttons: start by computing their total width (note that we don't
    // use any margin between them currently because the bitmaps we use don't
    // need it as they have sufficient padding already).
    int buttonsWidth = 0;
    for (const auto& button : page.buttons) {
        const wxBitmapBundle* const bb = GetButtonBitmapBundle(button);
        if (!bb)
            continue;

        buttonsWidth += bb->GetBitmapFor(wnd).GetLogicalWidth();
    }

    // Now draw them and update their rectangles.
    int xEnd = rect.x + size.x - wnd->FromDIP(Data::PADDING_X);
    if (buttonsWidth) {
        xEnd -= buttonsWidth;
        wxRect all_buttons_rect(xEnd, rect.GetY(), buttonsWidth, rect.GetHeight());
        all_buttons_rect.SetWidth(all_buttons_rect.GetWidth() + Data::PADDING_X + Data::MARGIN);

        int buttonX = xEnd;
        // IMPORTANT: this code assumes only X button.
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
            button_rect.y += 2;

            dc.DrawBitmap(bmp, button_rect.GetPosition(), true);

            buttonX += buttonSize.x;
        }

        xEnd -= wnd->FromDIP(Data::MARGIN);
    }

    // Finally draw tab text.
    dc.SetFont(page.active ? m_selectedFont : m_normalFont);
    dc.SetTextForeground(page.active ? m_data->m_fgActive : m_data->m_fgNormal);
    if (activeTabTextColour.IsOk()) {
        dc.SetTextForeground(activeTabTextColour);
    }

    const wxString& text = wxControl::Ellipsize(page.caption, dc, wxELLIPSIZE_END, xEnd - xStart);

    const int textHeight = dc.GetTextExtent(text).y;
    dc.DrawText(text, xStart, rect.y + (size.y - textHeight - 1) / 2);
    return xExtent;
}

wxSize clAuiFlatTabArt::GetPageTabSize(wxReadOnlyDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page, int* xExtent)
{
    dc.SetFont(m_measuringFont);

    // Start with the text size.
    wxSize size = dc.GetTextExtent(page.caption);

    // Add space for the buttons, if any.
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

    // Add space for the optional bitmap too.
    if (page.bitmap.IsOk()) {
        const wxSize bitmapSize = page.bitmap.GetPreferredLogicalSizeFor(wnd);

        size.x += bitmapSize.x + wnd->FromDIP(Data::MARGIN);

        // Increase the height if necessary (the width is never affected here).
        size.IncTo(bitmapSize);
    }

    // Add padding around the contents.
    size += wnd->FromDIP(2 * wxSize(Data::PADDING_X, Data::PADDING_Y));

    // After going to all the trouble of computing the fitting size, replace it
    // with fixed width if this is what we're supposed to use (but skipping
    // width computation in this case would make the code more complicated for
    // no real gain).
    if (m_flags & wxAUI_NB_TAB_FIXED_WIDTH) {
        size.x = m_fixedTabWidth;
    }

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

    // Enforce the bitmap size if specified.
    wxBitmapBundle measureBmp;
    if (requiredBmpSize.IsFullySpecified())
        measureBmp = wxBitmap(requiredBmpSize);

    wxSize size;

    // Note that we intentionally make a copy of the page as we modify it below.
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
