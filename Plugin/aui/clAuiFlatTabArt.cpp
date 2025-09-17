#include "clAuiFlatTabArt.hpp"

#include "drawingutils.h"

#include <wx/app.h>
#include <wx/bitmap.h>
#include <wx/bmpbndl.h>
#include <wx/settings.h>

namespace
{
static const char* const left_bitmap_data = R"svg(
<svg version="1.0" xmlns="http://www.w3.org/2000/svg" width="16" height="16">
    <polygon points="3, 8 9, 3 9, 13" stroke="currentColor" fill="currentColor" stroke-width="0"/>
</svg>
)svg";

static const char* const right_bitmap_data = R"svg(
<svg version="1.0" xmlns="http://www.w3.org/2000/svg" width="16" height="16">
    <polygon points="13, 8 7, 3 7, 13" stroke="currentColor" fill="currentColor" stroke-width="0"/>
</svg>
)svg";

const char* const list_bitmap_data = R"svg(
<svg version="1.0" xmlns="http://www.w3.org/2000/svg" width="16" height="16">
    <rect x="4.5" y="7" width="7" height="1" stroke="currentColor" fill="currentColor" stroke-width="0"/>
    <polygon points="4.5, 9 11.5 9 8, 13" stroke="currentColor" fill="currentColor" stroke-width="0"/>
</svg>
)svg";

const char* const unpin_bitmap_data = R"svg(
<svg version="1.0" xmlns="http://www.w3.org/2000/svg" width="16" height="16">
    <path d="M 7 5 v 6 v -1 h 6 v -1 h -6 h 6 v -3 h -6 v 2 h -4" stroke="currentColor" fill="none" stroke-width="1"/>
</svg>
)svg";

// Create a "disabled" version of the given colour by adjusting its lightness
// in the direction depending on the theme.
wxColour wxAuiDimColour(wxColour colour, int delta = 30)
{
    // We want to make light colours darker for dark themes and lighter for
    // light ones and vice versa for the dark colours, so start with the
    // default lightness of 100 and adjust it.
    const bool isDarkColour = colour.GetLuminance() < 0.5;

    int ialpha = 100;
    if (isDarkColour != wxSystemSettings::GetAppearance().IsDark())
        ialpha += delta;
    else
        ialpha -= delta;

    return colour.ChangeLightness(ialpha);
}

void IndentPressedBitmap(wxWindow* wnd, wxRect* rect, int button_state)
{
    if (button_state == wxAUI_BUTTON_STATE_PRESSED) {
        rect->Offset(wnd->FromDIP(wxPoint(1, 1)));
    }
}

wxBitmapBundle wxAuiCreateBitmap(const char* svgData, int w, int h, const wxColour& color)
{
    // All data starts with a new line, use +1 to skip it.
    wxString s = wxString::FromAscii(svgData + 1);
    s.Replace("currentColor", color.GetAsString(wxC2S_HTML_SYNTAX));
    return wxBitmapBundle::FromSVG(s.ToAscii(), wxSize(w, h));
}

wxBitmapBundle wxAuiCreateCloseButtonBitmap(const wxColour& color)
{
    static const char* const close_bitmap_data = R"svg(
<svg version="1.0" xmlns="http://www.w3.org/2000/svg" width="16" height="16">
    <line x1="4" y1="4" x2="11" y2="11" stroke="currentColor" fill="none" stroke-linecap="round" stroke-width="1.5"/>
    <line x1="4" y1="11" x2="11" y2="4" stroke="currentColor" fill="none" stroke-linecap="round" stroke-width="1.5"/>
</svg>
)svg";
    return wxAuiCreateBitmap(close_bitmap_data, 16, 16, color);
}

wxBitmapBundle wxAuiCreatePinButtonBitmap(const wxColour& color)
{
    static const char* const pin_bitmap_data = R"svg(
<svg version="1.0" xmlns="http://www.w3.org/2000/svg" width="16" height="16">
    <path d="M 5 9 h 6 h -1 v -6 h -1 v 6 v -6 h -3 v 6 h 2 v 4" stroke="currentColor" fill="none" stroke-width="1"/>
</svg>
)svg";
    return wxAuiCreateBitmap(pin_bitmap_data, 16, 16, color);
}

} // namespace
//======================================================
/// clAuiTabArtBase
//======================================================

clAuiTabArtBase::clAuiTabArtBase()
    : m_normalFont(*wxNORMAL_FONT)
    , m_selectedFont(m_normalFont)
{
    m_selectedFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_measuringFont = m_selectedFont;

    m_fixedTabWidth = wxWindow::FromDIP(100, nullptr);
}

void clAuiTabArtBase::SetFlags(unsigned int flags) { m_flags = flags; }

void clAuiTabArtBase::SetSizingInfo(const wxSize& tab_ctrl_size, size_t tab_count, wxWindow* wnd)
{
    if (!wnd) {
        // This is only allowed for backwards compatibility, we should be
        // really passed a valid window.
        wnd = wxTheApp->GetTopWindow();
        wxCHECK_RET(wnd, "must have some window");
    }

    m_fixedTabWidth = wnd->FromDIP(100);

    int tot_width = (int)tab_ctrl_size.x - GetIndentSize() - wnd->FromDIP(4);

    if (m_flags & wxAUI_NB_CLOSE_BUTTON)
        tot_width -= m_activeCloseBmp.GetPreferredLogicalSizeFor(wnd).x;
    if (m_flags & wxAUI_NB_WINDOWLIST_BUTTON)
        tot_width -= m_activeWindowListBmp.GetPreferredLogicalSizeFor(wnd).x;

    if (tab_count > 0) {
        m_fixedTabWidth = tot_width / (int)tab_count;
    }

    m_fixedTabWidth = wxMax(m_fixedTabWidth, wnd->FromDIP(100));

    if (m_fixedTabWidth > tot_width / 2)
        m_fixedTabWidth = tot_width / 2;

    m_fixedTabWidth = wxMin(m_fixedTabWidth, wnd->FromDIP(220));

    // We don't use this member variable ourselves any longer but keep it for
    // compatibility with the existing code, deriving from this class and using
    // it for its own purposes.
    m_tabCtrlHeight = tab_ctrl_size.y;
}

void clAuiTabArtBase::SetNormalFont(const wxFont& font) { m_normalFont = font; }

void clAuiTabArtBase::SetSelectedFont(const wxFont& font) { m_selectedFont = font; }

void clAuiTabArtBase::SetMeasuringFont(const wxFont& font) { m_measuringFont = font; }

wxFont clAuiTabArtBase::GetNormalFont() const { return m_normalFont; }

wxFont clAuiTabArtBase::GetSelectedFont() const { return m_selectedFont; }

void clAuiTabArtBase::InitBitmaps()
{
    m_activeCloseBmp = wxAuiCreateCloseButtonBitmap(GetButtonColour(wxAUI_BUTTON_CLOSE, wxAUI_BUTTON_STATE_NORMAL));
    m_disabledCloseBmp = wxAuiCreateCloseButtonBitmap(GetButtonColour(wxAUI_BUTTON_CLOSE, wxAUI_BUTTON_STATE_DISABLED));

    m_activeLeftBmp =
        wxAuiCreateBitmap(left_bitmap_data, 16, 16, GetButtonColour(wxAUI_BUTTON_LEFT, wxAUI_BUTTON_STATE_NORMAL));
    m_disabledLeftBmp =
        wxAuiCreateBitmap(left_bitmap_data, 16, 16, GetButtonColour(wxAUI_BUTTON_LEFT, wxAUI_BUTTON_STATE_DISABLED));
    m_activeRightBmp =
        wxAuiCreateBitmap(right_bitmap_data, 16, 16, GetButtonColour(wxAUI_BUTTON_RIGHT, wxAUI_BUTTON_STATE_NORMAL));
    m_disabledRightBmp =
        wxAuiCreateBitmap(right_bitmap_data, 16, 16, GetButtonColour(wxAUI_BUTTON_RIGHT, wxAUI_BUTTON_STATE_DISABLED));
    m_activeWindowListBmp = wxAuiCreateBitmap(
        list_bitmap_data, 16, 16, GetButtonColour(wxAUI_BUTTON_WINDOWLIST, wxAUI_BUTTON_STATE_NORMAL));
    m_disabledWindowListBmp = wxAuiCreateBitmap(
        list_bitmap_data, 16, 16, GetButtonColour(wxAUI_BUTTON_WINDOWLIST, wxAUI_BUTTON_STATE_DISABLED));

    // This is a bit confusing, but we use "pin" bitmap to indicate that the
    // tab is currently pinned, i.e. for the "unpin" button, and vice versa.
    m_activePinBmp =
        wxAuiCreateBitmap(unpin_bitmap_data, 16, 16, GetButtonColour(wxAUI_BUTTON_PIN, wxAUI_BUTTON_STATE_NORMAL));
    m_disabledPinBmp =
        wxAuiCreateBitmap(unpin_bitmap_data, 16, 16, GetButtonColour(wxAUI_BUTTON_PIN, wxAUI_BUTTON_STATE_DISABLED));

    m_activeUnpinBmp = wxAuiCreatePinButtonBitmap(GetButtonColour(wxAUI_BUTTON_PIN, wxAUI_BUTTON_STATE_NORMAL));
    m_disabledUnpinBmp = wxAuiCreatePinButtonBitmap(GetButtonColour(wxAUI_BUTTON_PIN, wxAUI_BUTTON_STATE_DISABLED));
}

const wxBitmapBundle* clAuiTabArtBase::GetButtonBitmapBundle(const wxAuiTabContainerButton& button) const
{
    if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
        return nullptr;

    const auto active = button.curState & (wxAUI_BUTTON_STATE_HOVER | wxAUI_BUTTON_STATE_PRESSED);

    switch (button.id) {
    case wxAUI_BUTTON_CLOSE:
        return active ? &m_activeCloseBmp : &m_disabledCloseBmp;

    case wxAUI_BUTTON_PIN:
        return button.curState & wxAUI_BUTTON_STATE_CHECKED ? (active ? &m_activeUnpinBmp : &m_disabledUnpinBmp)
                                                            : (active ? &m_activePinBmp : &m_disabledPinBmp);
    }

    return nullptr;
}

bool clAuiTabArtBase::DoGetButtonRectAndBitmap(wxWindow* wnd,
                                               const wxRect& in_rect,
                                               int bitmap_id,
                                               int button_state,
                                               int orientation,
                                               wxRect* outRect,
                                               wxBitmap* outBitmap)
{
    wxBitmapBundle bb;
    wxRect rect;

    switch (bitmap_id) {
    case wxAUI_BUTTON_CLOSE:
        if (button_state & wxAUI_BUTTON_STATE_DISABLED)
            bb = m_disabledCloseBmp;
        else
            bb = m_activeCloseBmp;
        break;
    case wxAUI_BUTTON_LEFT:
        if (button_state & wxAUI_BUTTON_STATE_DISABLED)
            bb = m_disabledLeftBmp;
        else
            bb = m_activeLeftBmp;
        break;
    case wxAUI_BUTTON_RIGHT:
        if (button_state & wxAUI_BUTTON_STATE_DISABLED)
            bb = m_disabledRightBmp;
        else
            bb = m_activeRightBmp;
        break;
    case wxAUI_BUTTON_WINDOWLIST:
        if (button_state & wxAUI_BUTTON_STATE_DISABLED)
            bb = m_disabledWindowListBmp;
        else
            bb = m_activeWindowListBmp;
        break;
    }

    if (!bb.IsOk())
        return false;

    const wxBitmap bmp = bb.GetBitmapFor(wnd);

    rect = in_rect;

    if (orientation == wxLEFT) {
        rect.SetX(in_rect.x);
        rect.SetY(((in_rect.y + in_rect.height) / 2) - (bmp.GetLogicalHeight() / 2));
        rect.SetSize(bmp.GetLogicalSize());
    } else {
        rect = wxRect(in_rect.x + in_rect.width - bmp.GetLogicalWidth(),
                      ((in_rect.y + in_rect.height) / 2) - (bmp.GetLogicalHeight() / 2),
                      bmp.GetLogicalWidth(),
                      bmp.GetLogicalHeight());
    }

    IndentPressedBitmap(wnd, &rect, button_state);

    if (outRect)
        *outRect = rect;
    if (outBitmap)
        *outBitmap = bmp;

    return true;
}

int clAuiTabArtBase::GetButtonRect(wxReadOnlyDC& WXUNUSED(dc),
                                   wxWindow* wnd,
                                   const wxRect& inRect,
                                   int bitmapId,
                                   int buttonState,
                                   int orientation,
                                   wxRect* outRect)
{
    wxRect rect;
    DoGetButtonRectAndBitmap(wnd, inRect, bitmapId, buttonState, orientation, &rect);
    if (outRect)
        *outRect = rect;

    return rect.width;
}

void clAuiTabArtBase::DrawButtonBitmap(wxDC& dc, const wxRect& rect, const wxBitmap& bmp, int WXUNUSED(buttonState))
{
    dc.DrawBitmap(bmp, rect.x, rect.y, true);
}

void clAuiTabArtBase::DrawButton(
    wxDC& dc, wxWindow* wnd, const wxRect& in_rect, int bitmap_id, int button_state, int orientation, wxRect* out_rect)
{
    wxRect rect;
    wxBitmap bmp;

    if (!DoGetButtonRectAndBitmap(wnd, in_rect, bitmap_id, button_state, orientation, &rect, &bmp))
        return;

    DrawButtonBitmap(dc, rect, bmp, button_state);

    *out_rect = rect;
}

int clAuiTabArtBase::ShowDropDown(wxWindow* wnd, const wxAuiNotebookPageArray& pages, int /*active_idx*/)
{
    wxMenu menuPopup;

    constexpr int MENU_ID_BASE = 1000;
    int id = MENU_ID_BASE;

    for (const auto& page : pages) {
        // Preserve ampersands possibly present in the caption string by
        // escaping them before passing the caption to wxMenuItem.
        wxString caption = wxControl::EscapeMnemonics(page.caption);

        // if there is no caption, make it a space.  This will prevent
        // an assert in the menu code.
        if (caption.IsEmpty())
            caption = wxT(" ");

        wxMenuItem* item = new wxMenuItem(nullptr, id++, caption);
        if (page.bitmap.IsOk())
            item->SetBitmap(page.bitmap.GetBitmapFor(wnd));
        menuPopup.Append(item);
    }

    // find out where to put the popup menu of window items
    wxPoint pt = ::wxGetMousePosition();
    pt = wnd->ScreenToClient(pt);

    // find out the screen coordinate at the bottom of the tab ctrl
    wxRect cli_rect = wnd->GetClientRect();
    pt.y = cli_rect.y + cli_rect.height;

    const int command = wnd->GetPopupMenuSelectionFromUser(menuPopup, pt);

    if (command >= MENU_ID_BASE)
        return command - MENU_ID_BASE;

    return wxNOT_FOUND;
}

int clAuiTabArtBase::GetBorderWidth(wxWindow* wnd)
{
    wxAuiManager* mgr = wxAuiManager::GetManager(wnd);
    if (mgr) {
        wxAuiDockArt* art = mgr->GetArtProvider();
        if (art)
            return art->GetMetricForWindow(wxAUI_DOCKART_PANE_BORDER_SIZE, wnd);
    }
    return 1;
}

int clAuiTabArtBase::GetAdditionalBorderSpace(wxWindow* WXUNUSED(wnd)) { return 0; }

void clAuiTabArtBase::DrawBorder(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    int i, border_width = GetBorderWidth(wnd);

    wxRect theRect(rect);
    for (i = 0; i < border_width; ++i) {
        dc.DrawRectangle(theRect.x, theRect.y, theRect.width, theRect.height);
        theRect.Deflate(1);
    }
}

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

        m_fgNormal = m_fgActive.ChangeLightness(is_dark ? 90 : 110);
        m_bgNormal = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
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
    : clAuiTabArtBase()
    , m_data(new Data())
{
    UpdateColoursFromSystem();
    InitBitmaps();
}

clAuiFlatTabArt::clAuiFlatTabArt(clAuiFlatTabArt* other)
    : clAuiTabArtBase(*other)
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

int clAuiFlatTabArt::DrawPageTab(wxDC& dc, wxWindow* wnd, wxAuiNotebookPage& page, const wxRect& rect)
{
    // Clip everything we do here to the provided rectangle.
    wxDCClipper clip(dc, rect);

    // Compute the size of the tab.
    int xExtent = 0;
    const wxSize size = GetPageTabSize(dc, wnd, page, &xExtent);

    page.rect = wxRect(rect.GetPosition(), size);

    // Draw the tab background and highlight it if it's active.
    const wxColour bg = page.active ? m_data->m_bgActive : m_data->m_bgNormal;
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
    if (page.bitmap.IsOk()) {
        const wxBitmap bmp = page.bitmap.GetBitmapFor(wnd);
        const wxSize bitmapSize = bmp.GetLogicalSize();

        dc.DrawBitmap(bmp, xStart, rect.y + (size.y - bitmapSize.y - 1) / 2, true /* use mask */);

        xStart += bitmapSize.x + wnd->FromDIP(Data::MARGIN);
    }

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

        int buttonX = xEnd;
        for (auto& button : page.buttons) {
            const wxBitmapBundle* const bb = GetButtonBitmapBundle(button);
            if (!bb)
                continue;

            const wxBitmap bmp = bb->GetBitmapFor(wnd);

            const wxSize buttonSize = bmp.GetLogicalSize();

            button.rect.x = buttonX;
            button.rect.y = rect.y + (size.y - buttonSize.y - 1) / 2;
            button.rect.width = buttonSize.x;
            button.rect.height = buttonSize.y;

            IndentPressedBitmap(wnd, &button.rect, button.curState);
            dc.DrawBitmap(bmp, button.rect.GetPosition(), true);

            buttonX += buttonSize.x;
        }

        xEnd -= wnd->FromDIP(Data::MARGIN);
    }

    // Finally draw tab text.
    dc.SetFont(page.active ? m_selectedFont : m_normalFont);
    dc.SetTextForeground(page.active ? m_data->m_fgActive : m_data->m_fgNormal);

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
