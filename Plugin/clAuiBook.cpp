#include "clAuiBook.hpp"

#include "clSystemSettings.h"
#include "editor_config.h"
#include "event_notifier.h"

#include <vector>
#include <wx/aui/tabart.h>

#ifdef __WXGTK__
static constexpr size_t X_SPACER = 15;
#else
static constexpr size_t X_SPACER = 10;
#endif

// Configurable via Settings -> Preferences -> Tabs
static size_t Y_SPACER = 5;

namespace
{
class clAuiBookArt : public wxAuiDefaultTabArt
{
public:
    wxAuiTabArt* Clone() override { return new clAuiBookArt(); }
    int GetBorderWidth(wxWindow* wnd) override { return 0; }
    int GetAdditionalBorderSpace(wxWindow* wnd) override { return 0; }
    int GetIndentSize() override { return 0; }

    void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect) override
    {
        DrawingUtils::DrawTabBackgroundArea(dc, wnd, rect);
    }

    void DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& in_rect, int bitmap_id, int button_state, int orientation,
                    wxRect* out_rect) override
    {
        wxAuiGenericTabArt::DrawButton(dc, wnd, in_rect, bitmap_id, button_state, orientation, out_rect);
    }

    wxSize GetTabSize(wxDC& dcRef, wxWindow* wnd, const wxString& caption, const wxBitmapBundle& bitmap,
                      bool WXUNUSED(active), int close_button_state, int* x_extent) override
    {
        wxGCDC gcdc;
        wxDC& dc = DrawingUtils::GetGCDC(dcRef, gcdc);
        wxCoord measured_textx, measured_texty, tmp;

        const int xPadding = wnd->FromDIP(X_SPACER);
        wxFont font = clTabRenderer::GetTabFont(true);
        dc.SetFont(font);
        dc.GetTextExtent(caption, &measured_textx, &measured_texty);
        dc.GetTextExtent(wxT("ABCDEFXj"), &tmp, &measured_texty);

        // add padding around the text
        // [ _ | text | _ | bmp | _ | x | _ ]
        wxCoord tab_width = xPadding + measured_textx + xPadding;
        wxCoord tab_height = measured_texty;

        // if there's a bitmap, add space for it
        if (bitmap.IsOk()) {
            // we need the correct size of the bitmap to be used on this window in
            // logical dimensions for drawing
            const wxSize bitmapSize = bitmap.GetPreferredLogicalSizeFor(wnd);

            // increase by bitmap plus right side bitmap padding
            tab_width += bitmapSize.x + xPadding;
        }

        // if the close button is showing, add space for it
        if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN) {
            // increase by button size plus the padding
            tab_width += m_activeCloseBmp.GetBitmapFor(wnd).GetLogicalWidth() + xPadding;
        }

        // add padding
        tab_height = DrawingUtils::GetTabHeight(dc, wnd, Y_SPACER);

        if (m_flags & wxAUI_NB_TAB_FIXED_WIDTH) {
            tab_width = m_fixedTabWidth;
        }

        *x_extent = tab_width;
        return wxSize(tab_width, tab_height);
    }

    void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page, const wxRect& inRect, int closeButtonState,
                 wxRect* outTabRect, wxRect* outButtonRect, int* xExtent) override
    {
        wxGCDC gcdc;
        wxDC& dcref = DrawingUtils::GetGCDC(dc, gcdc);
        wxFont font = clTabRenderer::GetTabFont(false);

        dcref.SetFont(font);

        bool is_modified = false;
        bool is_active = page.active;
        wxColour active_tab_bg_colour = wxNullColour;
        if (page.window) {
            auto stc = dynamic_cast<wxStyledTextCtrl*>(page.window);
            if (stc) {
                is_modified = stc->GetModify();

                if (is_active) {
                    active_tab_bg_colour = stc->StyleGetBackground(0);
                }
            }
        }

        if (is_active) {
            dcref.SetFont(m_selectedFont);
        }

        bool is_dark = clSystemSettings::GetAppearance().IsDark();
        wxSize button_size = GetTabSize(dcref, wnd, page.caption, page.bitmap, page.active, closeButtonState, xExtent);

        wxRect tab_rect(inRect.GetTopLeft(), button_size);
        tab_rect.SetHeight(wnd->GetClientRect().GetHeight());
        wxRect tab_rect_for_clipping(tab_rect);
        if (tab_rect_for_clipping.GetRight() >= inRect.GetRight()) {
            // the tab overflows. Make room for the window drop down list button
            tab_rect_for_clipping.SetRight(inRect.GetRight() - 10);
        }

        wxDCClipper clipper(dcref, tab_rect_for_clipping);
        wxColour bg_colour = clSystemSettings::GetDefaultPanelColour();
        if (is_active) {
            if (active_tab_bg_colour.IsOk()) {
                // If we haev an editor background colour, use this
                // as the tab active background colour
                bg_colour = active_tab_bg_colour;
            } else {
                if (is_dark) {
                    bg_colour = bg_colour.ChangeLightness(110);

                } else {
                    bg_colour = *wxWHITE;
                }
            }
        } else {
            bg_colour = bg_colour.ChangeLightness(is_dark ? 70 : 90);
        }

        if (is_active) {
            tab_rect.SetHeight(wnd->GetClientRect().GetHeight());
        }

        *outTabRect = tab_rect;
        dcref.SetPen(bg_colour);
        dcref.SetBrush(bg_colour);
        dcref.DrawRectangle(tab_rect);

        if (is_active) {
            dcref.SetPen(GetPenColour());

            wxPoint bottom_right = tab_rect.GetBottomRight();
            wxPoint top_right = tab_rect.GetTopRight();
            bottom_right.y += 1;
            bottom_right.x += 1;
            top_right.x += 1;

            wxPoint bottom_left = tab_rect.GetBottomLeft();
            wxPoint top_left = tab_rect.GetTopLeft();
            bottom_left.y += 1;
            bottom_left.x -= 1;
            top_left.x -= 1;

            dcref.DrawLine(top_left, bottom_left);
            dcref.DrawLine(top_right, bottom_right);

        } else {
            dcref.SetPen(GetPenColour());
            dcref.DrawLine(tab_rect.GetBottomLeft(), tab_rect.GetBottomRight());
        }

        // Draw the text
        {
            bool is_bg_dark = DrawingUtils::IsDark(bg_colour);
            wxColour text_colour =
                is_bg_dark ? (page.active ? *wxWHITE : bg_colour.ChangeLightness(170)) : bg_colour.ChangeLightness(30);

            if (is_modified) {
                text_colour = is_bg_dark ? "PINK" : "RED";
            }

            wxDCTextColourChanger text_colour_changer(dcref, text_colour);
            wxDCFontChanger font_changer(dcref, font);
            wxRect textRect = dcref.GetTextExtent(page.caption);

            textRect = textRect.CenterIn(tab_rect, wxVERTICAL);
            textRect.SetX(tab_rect.GetX() + wnd->FromDIP(X_SPACER));
            dcref.DrawText(page.caption, textRect.GetTopLeft());
        }

        if (closeButtonState != wxAUI_BUTTON_STATE_HIDDEN) {
            wxBitmapBundle bb = m_activeCloseBmp;
            const wxBitmap bmp = bb.GetBitmapFor(wnd);

            int offsetY = tab_rect.GetY() - 1;
            if (m_flags & wxAUI_NB_BOTTOM)
                offsetY = 1;

            wxRect button_rect(0, 0, bmp.GetLogicalWidth(), bmp.GetLogicalWidth());
            button_rect.x = tab_rect.GetX() + tab_rect.GetWidth() - button_rect.GetWidth() - wnd->FromDIP(X_SPACER);
            button_rect = button_rect.CenterIn(tab_rect, wxVERTICAL);
            DoDrawButton(dcref, wnd, button_rect, bg_colour, wxAUI_BUTTON_CLOSE, closeButtonState, wxTOP, &button_rect);

            *outButtonRect = button_rect;
        }
    }

private:
    wxColour GetPenColour() const
    {
        bool is_dark = clSystemSettings::GetAppearance().IsDark();
        wxColour pen_colour;
        if (is_dark) {
            pen_colour = *wxBLACK;
        } else {
            pen_colour = clSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
        }
        return pen_colour;
    }

    void DoDrawButton(wxDC& dc, wxWindow* wnd, const wxRect& inRect, const wxColour& tab_bg_colour, int bitmapId,
                      int buttonState, int orientation, wxRect* outRect)
    {
        wxGCDC gcdc;
        wxDC& dcref = DrawingUtils::GetGCDC(dc, gcdc);

        if (bitmapId != wxAUI_BUTTON_CLOSE) {
            wxAuiGenericTabArt::DrawButton(dc, wnd, inRect, bitmapId, buttonState, orientation, outRect);
            return;
        }

        wxColour pen_colour =
            DrawingUtils::IsDark(tab_bg_colour) ? "ORANGE" : clSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
        eButtonState button_state = eButtonState::kNormal;
        switch (buttonState) {
        case wxAUI_BUTTON_STATE_HOVER:
            button_state = eButtonState::kHover;
            break;
        case wxAUI_BUTTON_STATE_PRESSED:
            button_state = eButtonState::kPressed;
            break;
        case wxAUI_BUTTON_STATE_HIDDEN:
            return;
        default:
        case wxAUI_BUTTON_STATE_NORMAL:
            break;
        }
        DrawingUtils::DrawButtonX(dcref, wnd, inRect, pen_colour, tab_bg_colour, button_state);
        *outRect = inRect;
    }
};
} // namespace

clAuiBook::clAuiBook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxAuiNotebook(parent, id, pos, size, style)
{
    SetBookArt();

    wxFont font = clTabRenderer::GetTabFont(false);
    SetFont(font);
    SetMeasuringFont(font);
    SetNormalFont(font);
    SetSelectedFont(font);

    m_history.reset(new clTabHistory());
    Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGING, &clAuiBook::OnPageChanging, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &clAuiBook::OnPageChanged, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &clAuiBook::OnPageClosing, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSED, &clAuiBook::OnPageClosed, this);
    Bind(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, &clAuiBook::OnPageRightDown, this);
    Bind(wxEVT_AUINOTEBOOK_BG_DCLICK, &clAuiBook::OnTabAreaDoubleClick, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_SETTINGS_CHANGED, &clAuiBook::OnPreferences, this);
    CallAfter(&clAuiBook::UpdatePreferences);
}

clAuiBook::~clAuiBook()
{
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CHANGING, &clAuiBook::OnPageChanging, this);
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &clAuiBook::OnPageChanged, this);
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &clAuiBook::OnPageClosing, this);
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CLOSED, &clAuiBook::OnPageClosed, this);
    Unbind(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, &clAuiBook::OnPageRightDown, this);
    Unbind(wxEVT_AUINOTEBOOK_BG_DCLICK, &clAuiBook::OnTabAreaDoubleClick, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_SETTINGS_CHANGED, &clAuiBook::OnPreferences, this);
}

size_t clAuiBook::GetAllTabs(clTabInfo::Vec_t& tabs)
{
    size_t count = GetPageCount();
    tabs.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        clTabInfo::Ptr_t info(new clTabInfo(nullptr, 0, GetPage(i), GetPageText(i), wxNOT_FOUND));
        tabs.emplace_back(info);
    }
    return tabs.size();
}

void clAuiBook::MoveActivePage(int newIndex)
{
    // freeze the UI
    wxWindowUpdateLocker locker{ this };

    // no events while we are doing the tab movement
    clAuiBookEventsDisabler disabler{ this };

    int cursel = GetSelection();
    CHECK_COND_RET(cursel != wxNOT_FOUND);

    wxWindow* page = GetCurrentPage();
    CHECK_PTR_RET(page);

    wxString label = GetPageText(cursel);
    wxBitmap bmp = GetPageBitmap(cursel);
    if (RemovePage(cursel)) {
        InsertPage(newIndex, page, label, true, bmp);
    }
}

int clAuiBook::SetSelection(size_t newPage)
{
    size_t res = wxAuiNotebook::SetSelection(newPage);
    m_history->Push(GetCurrentPage());
    return res;
}

int clAuiBook::ChangeSelection(size_t n)
{
    size_t res = wxAuiNotebook::ChangeSelection(n);
    m_history->Push(GetCurrentPage());
    return res;
}

wxBorder clAuiBook::GetDefaultBorder() const { return wxBORDER_NONE; }

void clAuiBook::OnPageClosed(wxAuiNotebookEvent& event)
{
    event.Skip();
    if (!m_eventsEnabled) {
        return;
    }
    // Ensure that the history contains only existing tabs
    std::vector<wxWindow*> windows;
    size_t count = GetPageCount();
    windows.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        windows.emplace_back(GetPage(i));
    }
    m_history->Compact(windows, false);

    wxBookCtrlEvent event_closed(wxEVT_BOOK_PAGE_CLOSED);
    event_closed.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(event_closed);
}

void clAuiBook::OnPageChanged(wxAuiNotebookEvent& event)
{
    wxUnusedVar(event);
    if (!m_eventsEnabled) {
        return;
    }

    // Get the new selection and move it to the top of the hisotry list
    wxWindow* win = GetCurrentPage();
    CHECK_PTR_RET(win);

    m_history->Pop(win);
    m_history->Push(win);

    // Send an event
    wxBookCtrlEvent changed_event(wxEVT_BOOK_PAGE_CHANGED);
    changed_event.SetEventObject(GetParent());
    changed_event.SetSelection(GetSelection());
    GetEventHandler()->AddPendingEvent(changed_event);
}

void clAuiBook::OnPageChanging(wxAuiNotebookEvent& event)
{
    if (!m_eventsEnabled) {
        // Allow changing
        event.Skip();
        return;
    }

    // wrap this event with our own event
    wxBookCtrlEvent event_changing(wxEVT_BOOK_PAGE_CHANGING);
    event_changing.SetEventObject(this);
    event_changing.SetSelection(event.GetSelection());
    event_changing.SetOldSelection(GetSelection());
    GetEventHandler()->ProcessEvent(event_changing);
    if (!event_changing.IsAllowed()) {
        // Vetoed by the user, veto the original event and return
        event.Veto();
        return;
    }
    // Allow changing
    event.Skip();
}

void clAuiBook::OnPageDoubleClick(wxAuiNotebookEvent& event)
{
    if (!m_eventsEnabled) {
        // Allow changing
        event.Skip();
        return;
    }
    wxUnusedVar(event);
    wxBookCtrlEvent e(wxEVT_BOOK_TAB_DCLICKED);
    e.SetEventObject(GetParent());
    e.SetSelection(GetSelection());
    GetParent()->GetEventHandler()->AddPendingEvent(e);
}

void clAuiBook::OnTabAreaDoubleClick(wxAuiNotebookEvent& event)
{
    if (!m_eventsEnabled) {
        // Allow changing
        event.Skip();
        return;
    }

    wxBookCtrlEvent e(wxEVT_BOOK_NEW_PAGE);
    e.SetEventObject(GetParent());
    GetEventHandler()->AddPendingEvent(e);
}

void clAuiBook::OnPageRightDown(wxAuiNotebookEvent& event)
{
    if (!m_eventsEnabled) {
        // Allow changing
        event.Skip();
        return;
    }

    wxBookCtrlEvent menuEvent(wxEVT_BOOK_TAB_CONTEXT_MENU);
    menuEvent.SetEventObject(this);
    menuEvent.SetSelection(GetSelection());
    GetEventHandler()->ProcessEvent(menuEvent);
}

void clAuiBook::OnPageClosing(wxAuiNotebookEvent& event)
{
    if (!m_eventsEnabled) {
        // Allow changing
        event.Skip();
        return;
    }

    wxBookCtrlEvent eventClosing(wxEVT_BOOK_PAGE_CLOSING);
    eventClosing.SetEventObject(this);
    eventClosing.SetSelection(GetSelection());
    GetEventHandler()->ProcessEvent(eventClosing);
    if (!eventClosing.IsAllowed()) {
        // Vetoed
        event.Veto();
        return;
    }
    event.Skip();
}

void clAuiBook::UpdatePreferences()
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    bool show_x_on_tab = options->IsTabHasXButton();

    auto style = GetWindowStyle();
    style &= ~wxAUI_NB_CLOSE_ON_ALL_TABS;
    if (m_canHaveCloseButton) {
        if (show_x_on_tab) {
            style |= wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
        } else {
            style &= ~wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
        }
    } else {
        style &= ~wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
    }

    // update the tab height
    int tabHeight = options->GetNotebookTabHeight();
    switch (tabHeight) {
    case OptionsConfig::nbTabHt_Tiny:
        Y_SPACER = tabHeight + 2;
        break;
    case OptionsConfig::nbTabHt_Short:
        Y_SPACER = tabHeight + 2;
        break;
    default:
    case OptionsConfig::nbTabHt_Medium:
        Y_SPACER = tabHeight + 2;
        break;
    case OptionsConfig::nbTabHt_Tall:
        Y_SPACER = tabHeight + 2;
        break;
    }

    // update the art
    SetBookArt();
    SetWindowStyle(style);
    Refresh();
}

void clAuiBook::SetBookArt()
{
    wxFont font = clTabRenderer::GetTabFont(false);
    // auto art = new wxAuiDefaultTabArt();
    auto art = new clAuiBookArt();
    art->SetMeasuringFont(font);
    art->SetNormalFont(font);
    art->SetSelectedFont(font);
    SetArtProvider(art);
}

void clAuiBook::OnPreferences(wxCommandEvent& event)
{
    event.Skip();
    UpdatePreferences();
}

void clAuiBook::EnableEvents(bool b) { m_eventsEnabled = b; }

void clAuiBook::UpdateHistory()
{
    // Ensure that the history contains only existing tabs
    std::vector<wxWindow*> windows;
    size_t count = GetPageCount();
    windows.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        windows.emplace_back(GetPage(i));
    }
    m_history->Compact(windows, true);
}

int clAuiBook::GetPageIndex(wxWindow* win) const { return FindPage(win); }

int clAuiBook::GetPageIndex(const wxString& name) const
{
    for (size_t i = 0; i < GetPageCount(); ++i) {
        if (GetPageText(i) == name) {
            return i;
        }
    }
    return wxNOT_FOUND;
}
