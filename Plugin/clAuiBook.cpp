#include "clAuiBook.hpp"

#include "clSystemSettings.h"
#include "editor_config.h"
#include "event_notifier.h"

#include <vector>
#include <wx/aui/tabart.h>

class clAuiBookArt : public wxAuiDefaultTabArt
{
public:
    wxAuiTabArt* Clone() override { return new clAuiBookArt(); }

    void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect) override
    {
        wxDCBrushChanger brush_changer(dc, clSystemSettings::GetDefaultPanelColour());
        wxDCPenChanger pen_changer(dc, clSystemSettings::GetDefaultPanelColour());
        dc.DrawRectangle(rect);
    }

    void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page, const wxRect& inRect, int closeButtonState,
                 wxRect* outTabRect, wxRect* outButtonRect, int* xExtent) override
    {
        wxGCDC gcdc;
        wxDC& dcref = DrawingUtils::GetGCDC(dc, gcdc);

        bool is_active = page.active;
        bool is_dark = clSystemSettings::GetAppearance().IsDark();
        wxSize button_size = GetTabSize(dcref, wnd, page.caption, page.bitmap, page.active, closeButtonState, xExtent);

        wxRect tab_rect(inRect.GetTopLeft(), button_size);
        wxRect tab_rect_for_clipping(tab_rect);
        if (tab_rect_for_clipping.GetRight() >= inRect.GetRight()) {
            // the tab overflows. Make room for the window drop down list button
            tab_rect_for_clipping.SetRight(inRect.GetRight() - 10);
        }

        wxDCClipper clipper(dcref, tab_rect_for_clipping);
        wxColour bg_colour = clSystemSettings::GetDefaultPanelColour();
        if (is_active) {
            if (is_dark) {
                bg_colour = bg_colour.ChangeLightness(110);

            } else {
                bg_colour = *wxWHITE;
            }
        } else {
            bg_colour = bg_colour.ChangeLightness(is_dark ? 70 : 90);
        }

#ifdef __WXGTK__
        tab_rect.SetHeight(tab_rect.GetHeight() + wnd->FromDIP(5));
#endif
        wxColour pen_colour = bg_colour;

        *outTabRect = tab_rect;
        dcref.SetPen(pen_colour);
        dcref.SetBrush(bg_colour);
        dcref.DrawRectangle(tab_rect);

        // Draw the text
        {
            wxColour text_colour =
                is_dark ? (page.active ? *wxWHITE : bg_colour.ChangeLightness(170)) : bg_colour.ChangeLightness(30);

            wxDCTextColourChanger text_colour_changer(dcref, text_colour);
            wxDCFontChanger font_changer(dcref, m_normalFont);

            wxRect textRect = dcref.GetTextExtent(page.caption);
            textRect = textRect.CenterIn(tab_rect, wxVERTICAL);
            textRect.SetX(tab_rect.GetX() + wnd->FromDIP(10));
            dcref.DrawText(page.caption, textRect.GetTopLeft());
        }

        if (closeButtonState != wxAUI_BUTTON_STATE_HIDDEN) {
            wxBitmapBundle bb = m_activeCloseBmp;
            const wxBitmap bmp = bb.GetBitmapFor(wnd);

            int offsetY = tab_rect.GetY() - 1;
            if (m_flags & wxAUI_NB_BOTTOM)
                offsetY = 1;

            wxRect button_rect(0, 0, bmp.GetLogicalWidth(), bmp.GetLogicalWidth());
            button_rect.x = tab_rect.GetX() + tab_rect.GetWidth() - button_rect.GetWidth() - wnd->FromDIP(10);
            button_rect = button_rect.CenterIn(tab_rect, wxVERTICAL);
            DoDrawButton(dcref, wnd, button_rect, bg_colour, wxAUI_BUTTON_CLOSE, closeButtonState, wxTOP, &button_rect);

            *outButtonRect = button_rect;
        }
    }

private:
    void DoDrawButton(wxDC& dc, wxWindow* wnd, const wxRect& inRect, const wxColour& tab_bg_colour, int bitmapId,
                      int buttonState, int orientation, wxRect* outRect)
    {
        wxGCDC gcdc;
        wxDC& dcref = DrawingUtils::GetGCDC(dc, gcdc);

        if (bitmapId != wxAUI_BUTTON_CLOSE) {
            wxAuiDefaultTabArt::DrawButton(dc, wnd, inRect, bitmapId, buttonState, orientation, outRect);
            return;
        }

        bool is_dark = clSystemSettings::GetAppearance().IsDark();
        wxColour pen_colour = is_dark ? "ORANGE" : clSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
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

static constexpr size_t BOOK_STYLE = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS |
                                     wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_WINDOWLIST_BUTTON;

clAuiBook::clAuiBook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxAuiNotebook(parent, id, pos, size, style == 0 ? BOOK_STYLE : style)
{
    SetArtProvider(new clAuiBookArt());
    m_history.reset(new clTabHistory());
    Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGING, &clAuiBook::OnPageChanging, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &clAuiBook::OnPageChanged, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &clAuiBook::OnPageClosing, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSED, &clAuiBook::OnPageClosed, this);
    Bind(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, &clAuiBook::OnPageRightDown, this);
    Bind(wxEVT_AUINOTEBOOK_BG_DCLICK, &clAuiBook::OnTabAreaDoubleClick, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_SETTINGS_CHANGED, &clAuiBook::OnPreferences, this);

    wxFont default_font = DrawingUtils::GetDefaultGuiFont();
    SetFont(default_font);
    SetMeasuringFont(default_font);
    SetNormalFont(default_font);
    SetSelectedFont(default_font);
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
    int cursel = GetSelection();
    CHECK_COND_RET(cursel != wxNOT_FOUND);

    wxUnusedVar(newIndex);
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
    // Ensure that the history contains only existing tabs
    std::vector<wxWindow*> windows;
    size_t count = GetPageCount();
    windows.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        windows.emplace_back(GetPage(i));
    }
    m_history->Compact(windows);

    wxBookCtrlEvent event_closed(wxEVT_BOOK_PAGE_CLOSED);
    event_closed.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event_closed);
}

void clAuiBook::OnPageChanged(wxAuiNotebookEvent& event)
{
    // Get the new selection and move it to the top of the hisotry list
    wxWindow* win = GetCurrentPage();
    CHECK_PTR_RET(win);

    m_history->Pop(win);
    m_history->Push(win);

    // Send an event
    wxBookCtrlEvent changed_event(wxEVT_BOOK_PAGE_CHANGED);
    changed_event.SetEventObject(GetParent());
    changed_event.SetSelection(GetSelection());
    GetEventHandler()->ProcessEvent(changed_event);
}

void clAuiBook::OnPageChanging(wxAuiNotebookEvent& event)
{
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
    wxUnusedVar(event);
    wxBookCtrlEvent e(wxEVT_BOOK_TAB_DCLICKED);
    e.SetEventObject(GetParent());
    e.SetSelection(GetSelection());
    GetParent()->GetEventHandler()->AddPendingEvent(e);
}

void clAuiBook::OnTabAreaDoubleClick(wxAuiNotebookEvent& event)
{
    wxUnusedVar(event);
    wxBookCtrlEvent e(wxEVT_BOOK_NEW_PAGE);
    e.SetEventObject(GetParent());
    GetEventHandler()->AddPendingEvent(e);
}

void clAuiBook::OnPageRightDown(wxAuiNotebookEvent& event)
{
    wxBookCtrlEvent menuEvent(wxEVT_BOOK_TAB_CONTEXT_MENU);
    menuEvent.SetEventObject(this);
    menuEvent.SetSelection(GetSelection());
    GetEventHandler()->ProcessEvent(menuEvent);
}

void clAuiBook::OnPageClosing(wxAuiNotebookEvent& event)
{
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

void clAuiBook::OnPreferences(wxCommandEvent& event)
{
    event.Skip();
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    bool show_x_on_tab = options->IsTabHasXButton();

    auto style = GetWindowStyle();
    if (show_x_on_tab) {
        style |= wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
    } else {
        style &= ~wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
    }
    SetWindowStyle(style);
    Refresh();
}
