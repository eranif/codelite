#include "clSideBarCtrl.hpp"

#include "drawingutils.h"

#include <wx/anybutton.h>
#include <wx/dcbuffer.h>
#include <wx/gdicmn.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/tooltip.h>

thread_local int BUTTON_ID = 0;

wxDEFINE_EVENT(wxEVT_SIDEBAR_SELECTION_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_SIDEBAR_CONTEXT_MENU, wxContextMenuEvent);

#define CHECK_POINTER_RETURN(ptr, WHAT) \
    if(!ptr)                            \
        return WHAT;

#define CHECK_POSITION_RETURN(pos, WHAT)             \
    if((size_t)pos >= m_mainSizer->GetItemCount()) { \
        return WHAT;                                 \
    }

namespace
{
// return the wxBORDER_SIMPLE that matches the current application theme
wxBorder border_simple_theme_aware_bit()
{
#ifdef __WXMAC__
    return wxBORDER_NONE;
#elif defined(__WXGTK__)
    return wxBORDER_STATIC;
#else
    return wxBORDER_SIMPLE;
#endif
} // DoGetBorderSimpleBit

/// Paint the control background
void paint_background(wxDC& dc, wxWindow* win, const wxRect& rect, wxDirection direction)
{
    wxColour bg_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    bool is_dark = DrawingUtils::IsDark(bg_colour);
    int alpha = is_dark ? 110 : 150;

    wxDirection gradient_direction = wxEAST;
    if(direction == wxBOTTOM || direction == wxTOP) {
        gradient_direction = wxSOUTH;
    }
    dc.GradientFillLinear(rect, bg_colour.ChangeLightness(alpha), bg_colour, gradient_direction);
}
} // namespace

class SideBarButton : public wxControl
{
    friend class clSideBarButtonCtrl;
    friend class clSideBarCtrl;

protected:
    clSideBarButtonCtrl* m_sidebar = nullptr;
    wxBitmap m_bmp;
    bool m_selected = false;
    wxDateTime m_dragStartTime;
    wxPoint m_dragStartPos;
    bool m_dragging = false;
    wxWindow* m_linkedPage = nullptr;
    wxString m_label;
    wxDirection m_direction = wxLEFT;

protected:
    void DoDrop()
    {
        wxPoint pt = ::wxGetMousePosition();
        SideBarButton* target_button = nullptr;
        bool move_after = false;
        auto all_buttons = m_sidebar->GetAllButtons();
        for(auto button : all_buttons) {
            auto target_tab_rect = button->GetScreenRect();

            auto target_tab_upper_rect = target_tab_rect;
            target_tab_upper_rect.SetHeight(target_tab_rect.GetHeight() / 2);

            auto target_tab_lower_rect = target_tab_rect;
            target_tab_lower_rect.SetHeight(target_tab_rect.GetHeight() / 2);
            target_tab_lower_rect.SetY(target_tab_rect.GetY() + (target_tab_rect.GetHeight() / 2));

            if(target_tab_upper_rect.Contains(pt)) {
                // this is our target tab
                if(button != this) {
                    target_button = static_cast<SideBarButton*>(button);
                    move_after = false;
                }
                break;
            } else if(target_tab_lower_rect.Contains(pt)) {
                // this is our target tab
                if(button != this) {
                    target_button = static_cast<SideBarButton*>(button);
                    move_after = true;
                }
                break;
            }
        }

        if(target_button) {
            if(move_after) {
                m_sidebar->CallAfter(&clSideBarButtonCtrl::MoveAfter, this, target_button);
            } else {
                m_sidebar->CallAfter(&clSideBarButtonCtrl::MoveBefore, this, target_button);
            }
        }
    }

    void DoCancelDrag()
    {
        if(m_dragging) {
            // locate the target tab
            DoDrop();
        }
        m_dragStartTime.Set((time_t)-1); // Reset the saved values
        m_dragStartPos = wxPoint();
        SetCursor(wxCURSOR_HAND);
        m_dragging = false;
        if(HasCapture()) {
            ReleaseMouse();
        }
    }

    void DoBeginDrag()
    {
        // Change the cursor indicating DnD in progress
        SetCursor(wxCURSOR_SIZING);
        CaptureMouse();
        m_dragging = true;
    }
    void OnMotion(wxMouseEvent& event)
    {
        event.Skip();
        if(m_dragStartTime.IsValid() && event.LeftIsDown() &&
           !m_dragging) { // If we're tugging on the tab, consider starting D'n'D
            wxTimeSpan diff = wxDateTime::UNow() - m_dragStartTime;
            if(diff.GetMilliseconds() > 100 && // We need to check both x and y distances as tabs may be vertical
               ((abs(m_dragStartPos.x - event.GetX()) > 5) || (abs(m_dragStartPos.y - event.GetY()) > 5))) {
                DoBeginDrag(); // Sufficient time and distance since the LeftDown for a believable D'n'D start
            }
        }
    }

    void OnLeftUp(wxMouseEvent& event)
    {
        event.Skip();
        if(m_dragging) {
            DoCancelDrag();
        }
    }

    void OnLeftDown(wxMouseEvent& event)
    {
        event.Skip();
        if(!IsSeleced()) {
            int this_index = m_sidebar->GetButtonIndex(this);
            if(this_index == wxNOT_FOUND) {
                return;
            }
            int selection_index = m_sidebar->GetSelection();
            if(selection_index == wxNOT_FOUND) {
                return;
            }

            if(selection_index == this_index) {
                // current button is already selected
                return;
            }

            // this will trigger an event + refresh
            static_cast<clSideBarCtrl*>(m_sidebar->GetParent())->SetSelection(this_index);

        } else {
            // Prepare to DnDclTreeCtrl_DnD
            if(event.LeftIsDown()) {
                m_dragStartTime = wxDateTime::UNow();
                m_dragStartPos = wxPoint(event.GetX(), event.GetY());
            }
        }
    }

    void OnPaint(wxPaintEvent& event)
    {
        wxUnusedVar(event);
        wxBufferedPaintDC dc(this);

        wxRect client_rect = GetClientRect();

        // draw the background
        wxColour base_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        bool is_dark = DrawingUtils::IsDark(base_colour);
        paint_background(dc, this, client_rect, m_direction);
        if(IsSeleced()) {
            // draw the selected item using different background colour
            client_rect.Deflate(1);
            wxRect frame_rect = client_rect;
            wxColour colour = base_colour;
            if(is_dark) {
                colour = colour.ChangeLightness(115);
            } else {
                colour = *wxWHITE;
            }
            dc.SetBrush(colour);
            dc.SetPen(colour);
            dc.DrawRectangle(frame_rect);
        }

        wxRect bmp_rect = wxRect(m_bmp.GetLogicalSize()).CenterIn(client_rect);
        dc.DrawBitmap(m_bmp, bmp_rect.GetTopLeft());

        if(IsSeleced()) {
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.SetPen(is_dark ? base_colour.ChangeLightness(150) : base_colour.ChangeLightness(70));
            dc.DrawRectangle(client_rect);
        }
    }

    void OnEraseBg(wxEraseEvent& event) { wxUnusedVar(event); }

    void OnContextMenu(wxContextMenuEvent& event)
    {
        wxUnusedVar(event);
        wxContextMenuEvent menu_event{ wxEVT_SIDEBAR_CONTEXT_MENU };
        menu_event.SetInt(m_sidebar->GetButtonIndex(this));
        menu_event.SetEventObject(m_sidebar->GetParent());
        m_sidebar->GetParent()->GetEventHandler()->ProcessEvent(menu_event);
    }

public:
    explicit SideBarButton(clSideBarButtonCtrl* parent, const wxBitmap bmp)
        : wxControl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
        , m_sidebar(parent)
        , m_bmp(bmp)
    {
        m_dragStartTime = (time_t)-1;
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetCursor(wxCURSOR_HAND);

        wxRect rr = m_bmp.GetScaledSize();
        rr.Inflate(10);
        SetSizeHints(rr.GetWidth(), rr.GetHeight());
        SetSize(rr.GetWidth(), rr.GetHeight());

        Bind(wxEVT_PAINT, &SideBarButton::OnPaint, this);
        Bind(wxEVT_ERASE_BACKGROUND, &SideBarButton::OnEraseBg, this);
        Bind(wxEVT_LEFT_DOWN, &SideBarButton::OnLeftDown, this);
        Bind(wxEVT_MOTION, &SideBarButton::OnMotion, this);
        Bind(wxEVT_LEFT_UP, &SideBarButton::OnLeftUp, this);
        Bind(wxEVT_CONTEXT_MENU, &SideBarButton::OnContextMenu, this);
    }

    virtual ~SideBarButton()
    {
        Unbind(wxEVT_PAINT, &SideBarButton::OnPaint, this);
        Unbind(wxEVT_ERASE_BACKGROUND, &SideBarButton::OnEraseBg, this);
        Unbind(wxEVT_LEFT_DOWN, &SideBarButton::OnLeftDown, this);
        Unbind(wxEVT_MOTION, &SideBarButton::OnMotion, this);
        Unbind(wxEVT_LEFT_UP, &SideBarButton::OnLeftUp, this);
        Unbind(wxEVT_CONTEXT_MENU, &SideBarButton::OnContextMenu, this);
    }

    void SetDirection(const wxDirection& direction) { this->m_direction = direction; }
    const wxDirection& GetDirection() const { return m_direction; }
    void SetLinkedPage(wxWindow* win) { m_linkedPage = win; }
    wxWindow* GetLinkedPage() const { return m_linkedPage; }

    void SetSeleced(bool b) { m_selected = b; }
    bool IsSeleced() const { return m_selected; }
    void SetPageLabel(const wxString& label) { this->m_label = label; }
    const wxString& GetButtonLabel() const { return m_label; }
    const wxBitmap GetButtonBitmap() const { return m_bmp; }
};

clSideBarButtonCtrl::clSideBarButtonCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                         long style)
    : wxControl(parent, id, pos, size, style | wxBORDER_NONE)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Initialise();

    Bind(wxEVT_PAINT, &clSideBarButtonCtrl::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent& e) { wxUnusedVar(e); });
}

clSideBarButtonCtrl::~clSideBarButtonCtrl()
{
    Unbind(wxEVT_PAINT, &clSideBarButtonCtrl::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent& e) { wxUnusedVar(e); });
}

void clSideBarButtonCtrl::Initialise()
{
    if(!m_mainSizer) {
        m_mainSizer = new wxBoxSizer(wxVERTICAL);
    }

    if(IsHorizontalLayout()) {
        m_mainSizer->SetOrientation(wxHORIZONTAL);
    } else {
        m_mainSizer->SetOrientation(wxVERTICAL);
    }

    SetSizer(m_mainSizer);
    GetSizer()->Layout();
}

void clSideBarButtonCtrl::SetButtonsPosition(wxDirection direction)
{
    m_buttonsPosition = direction;
    // update the buttons
    auto all_buttons = GetAllButtons();
    for(auto button : all_buttons) {
        static_cast<SideBarButton*>(button)->SetDirection(direction);
    }
    Initialise();
}

void clSideBarButtonCtrl::OnPaint(wxPaintEvent& event)
{
    wxUnusedVar(event);
    wxBufferedPaintDC dc(this);
    paint_background(dc, this, GetClientRect(), m_buttonsPosition);
}

int clSideBarButtonCtrl::AddButton(const wxBitmap bmp, const wxString& label, wxWindow* linked_page, bool select)
{
    SideBarButton* btn = new SideBarButton(this, bmp);
    if(select) {
        // unselect the old button
        int old_selection = GetSelection();
        if(old_selection != wxNOT_FOUND) {
            GetButton(old_selection)->SetSeleced(false);
            GetButton(old_selection)->Refresh();
        }
    }

    btn->SetSeleced(select);
    btn->SetToolTip(label);
    btn->SetPageLabel(label);
    btn->SetLinkedPage(linked_page);
    btn->SetDirection(m_buttonsPosition);
    btn->Refresh();

    if(IsHorizontalLayout()) {
        m_mainSizer->Add(btn, wxSizerFlags().CenterVertical());
    } else {
        m_mainSizer->Add(btn, wxSizerFlags().CenterHorizontal());
    }

    SetSizeHints(btn->GetSize().GetWidth(), wxNOT_FOUND);
    SetSize(btn->GetSize().GetWidth(), wxNOT_FOUND);
    GetParent()->GetSizer()->Layout();
    return GetButtonIndex(btn);
}

void clSideBarButtonCtrl::Clear()
{
    m_mainSizer->Clear(false);
    m_mainSizer->Layout();
}

wxWindow* clSideBarButtonCtrl::RemoveButton(int pos)
{
    CHECK_POSITION_RETURN(pos, nullptr);

    auto button = static_cast<SideBarButton*>(m_mainSizer->GetItem(pos)->GetWindow());
    auto linked_page = button->GetLinkedPage();
    m_mainSizer->Detach(pos);
    button->Destroy();
    m_mainSizer->Layout();

    if(m_mainSizer->GetItemCount() == 0) {
        return linked_page;
    }

    // select the first item
    SetSelection(0);
    return linked_page;
}

void clSideBarButtonCtrl::MoveAfter(SideBarButton* src, SideBarButton* target)
{
    if(src == target) {
        return;
    }
    m_mainSizer->Detach(src);
    // find the target button index
    size_t target_index = wxString::npos;
    for(size_t i = 0; i < m_mainSizer->GetItemCount(); ++i) {
        if(m_mainSizer->GetItem(i)->GetWindow() == target) {
            target_index = i;
            break;
        }
    }

    if(target_index == wxString::npos) {
        return;
    }

    if(target_index + 1 >= m_mainSizer->GetItemCount()) {
        m_mainSizer->Add(src);
    } else {
        m_mainSizer->Insert(target_index + 1, src);
    }
    m_mainSizer->Layout();
}

void clSideBarButtonCtrl::MoveBefore(SideBarButton* src, SideBarButton* target)
{
    if(src == target) {
        return;
    }
    m_mainSizer->Detach(src);
    // find the target button index
    size_t target_index = GetButtonIndex(target);
    if(target_index == wxString::npos) {
        return;
    }

    m_mainSizer->Insert(target_index, src);
    m_mainSizer->Layout();
}

int clSideBarButtonCtrl::GetButtonIndex(SideBarButton* btn) const
{
    for(size_t i = 0; i < m_mainSizer->GetItemCount(); ++i) {
        if(m_mainSizer->GetItem(i)->GetWindow() == btn) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

std::vector<wxWindow*> clSideBarButtonCtrl::GetAllButtons()
{
    std::vector<wxWindow*> buttons;
    buttons.reserve(m_mainSizer->GetItemCount());
    for(size_t i = 0; i < m_mainSizer->GetItemCount(); ++i) {
        buttons.push_back(m_mainSizer->GetItem(i)->GetWindow());
    }
    return buttons;
}

int clSideBarButtonCtrl::GetSelection() const
{
    for(size_t i = 0; i < m_mainSizer->GetItemCount(); ++i) {
        auto button = static_cast<SideBarButton*>(m_mainSizer->GetItem(i)->GetWindow());
        if(button->IsSeleced()) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

wxWindow* clSideBarButtonCtrl::SetSelection(int pos) { return DoChangeSelection(pos, true); }

wxWindow* clSideBarButtonCtrl::ChangeSelection(int pos) { return DoChangeSelection(pos, false); }

wxWindow* clSideBarButtonCtrl::DoChangeSelection(int pos, bool notify)
{
    CHECK_POSITION_RETURN(pos, nullptr);

    SideBarButton* button = dynamic_cast<SideBarButton*>(m_mainSizer->GetItem((size_t)pos)->GetWindow());
    CHECK_POINTER_RETURN(button, nullptr);

    auto old_selection = GetButton(GetSelection());
    if(old_selection) {
        old_selection->SetSeleced(false);
        old_selection->Refresh();
    }
    button->SetSeleced(true);
    button->Refresh();
    Refresh();

    if(notify) {
        wxCommandEvent selection_changed(wxEVT_SIDEBAR_SELECTION_CHANGED);
        selection_changed.SetEventObject(GetParent());
        selection_changed.SetInt(pos);
        AddPendingEvent(selection_changed);
    }
    return button->GetLinkedPage();
}

size_t clSideBarButtonCtrl::GetButtonCount() const { return m_mainSizer->GetItemCount(); }

wxWindow* clSideBarButtonCtrl::GetSelectionLinkedPage() const
{
    for(size_t i = 0; i < m_mainSizer->GetItemCount(); ++i) {
        SideBarButton* button = static_cast<SideBarButton*>(m_mainSizer->GetItem(i)->GetWindow());
        if(button->IsSeleced()) {
            return button->GetLinkedPage();
        }
    }
    return nullptr;
}

SideBarButton* clSideBarButtonCtrl::GetButton(size_t pos) const
{
    CHECK_POSITION_RETURN(pos, nullptr);
    return static_cast<SideBarButton*>(m_mainSizer->GetItem(pos)->GetWindow());
}

SideBarButton* clSideBarButtonCtrl::GetButton(const wxString& label) const
{
    if(label.empty()) {
        return nullptr;
    }
    for(size_t i = 0; i < m_mainSizer->GetItemCount(); ++i) {
        SideBarButton* button = static_cast<SideBarButton*>(m_mainSizer->GetItem(i)->GetWindow());
        if(button->GetButtonLabel() == label) {
            return button;
        }
    }
    return nullptr;
}

int clSideBarButtonCtrl::GetPageIndex(const wxString& label) const
{
    for(size_t i = 0; i < m_mainSizer->GetItemCount(); ++i) {
        SideBarButton* button = static_cast<SideBarButton*>(m_mainSizer->GetItem(i)->GetWindow());
        if(button->GetButtonLabel() == label) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

int clSideBarButtonCtrl::GetPageIndex(wxWindow* page) const
{
    for(size_t i = 0; i < m_mainSizer->GetItemCount(); ++i) {
        SideBarButton* button = static_cast<SideBarButton*>(m_mainSizer->GetItem(i)->GetWindow());
        if(button->GetLinkedPage() == page) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

bool clSideBarButtonCtrl::IsHorizontalLayout() const
{
    return m_buttonsPosition == wxTOP || m_buttonsPosition == wxBOTTOM;
}

// -----------------------
// SideBar control
// -----------------------

clSideBarCtrl::clSideBarCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, border_simple_theme_aware_bit())
{
    bool vertical_sizer = style & (wxBK_TOP | wxBK_BOTTOM);
    m_mainSizer = new wxBoxSizer(vertical_sizer ? wxVERTICAL : wxHORIZONTAL);

    SetSizer(m_mainSizer);
    Bind(wxEVT_SIZE, &clSideBarCtrl::OnSize, this);
    m_buttons = new clSideBarButtonCtrl(this);
    m_book = new wxSimplebook(this);
    // when we use a vertical sizer (i.e. the buttons are placed under or on top of the book,
    // we set the buttons layout to be horizontal)
    m_buttonsPosition = wxLEFT;
    if(style & wxBK_TOP) {
        m_buttonsPosition = wxTOP;
    } else if(style & wxBK_RIGHT) {
        m_buttonsPosition = wxRIGHT;
    } else if(style & wxBK_BOTTOM) {
        m_buttonsPosition = wxBOTTOM;
    } else /* assume left, the default */ {
        m_buttonsPosition = wxLEFT;
    }
    m_buttons->SetButtonsPosition(m_buttonsPosition);
    PlaceButtons();
}

clSideBarCtrl::~clSideBarCtrl() { Unbind(wxEVT_SIZE, &clSideBarCtrl::OnSize, this); }

void clSideBarCtrl::PlaceButtons()
{
    GetSizer()->Detach(m_book);
    GetSizer()->Detach(m_buttons);

    // adjust the sizer orientation
    m_mainSizer->SetOrientation((m_buttonsPosition == wxLEFT || m_buttonsPosition == wxRIGHT) ? wxHORIZONTAL
                                                                                              : wxVERTICAL);

    switch(m_buttonsPosition) {
    case wxRIGHT:
    case wxBOTTOM:
        GetSizer()->Add(m_book, 1, wxEXPAND);
        GetSizer()->Add(m_buttons, 0, wxEXPAND);
        break;
    default:
    case wxLEFT:
    case wxTOP:
        GetSizer()->Add(m_buttons, 0, wxEXPAND);
        GetSizer()->Add(m_book, 1, wxEXPAND);
        break;
    }
    GetSizer()->Layout();
    GetSizer()->Fit(this);
}

void clSideBarCtrl::AddPage(wxWindow* page, const wxString& label, wxBitmap bmp, bool selected)
{
    page->Reparent(m_book);
    m_book->AddPage(page, label, selected);
    m_buttons->AddButton(bmp, label, page, selected || m_buttons->GetButtonCount() == 0);

    // sync the selection between the book and the button bar
    m_book->SetSelection(m_buttons->GetSelection());
}

void clSideBarCtrl::DoRemovePage(size_t pos, bool delete_it)
{
    // remove the button and return the associated page
    auto page = m_buttons->RemoveButton(pos);
    if(!page) {
        return;
    }

    // locate the page in the book and remove it
    int pos_in_book = SimpleBookGetPageIndex(page);
    if(pos_in_book == wxNOT_FOUND) {
        return;
    }
    if(delete_it) {
        m_book->DeletePage(pos_in_book);
    } else {
        m_book->RemovePage(pos_in_book);
    }

    // sync the selection between the book and the button bar
    m_book->SetSelection(m_buttons->GetSelection());
}

void clSideBarCtrl::RemovePage(size_t pos) { DoRemovePage(pos, false); }
void clSideBarCtrl::RemoveAll() { m_buttons->Clear(); }

void clSideBarCtrl::DeletePage(size_t pos) { DoRemovePage(pos, true); }

void clSideBarCtrl::SetSelection(size_t pos)
{
    // Set the selection in the buttons bar and return the linked page
    wxWindow* page = m_buttons->SetSelection(pos);
    CHECK_POINTER_RETURN(page, );

    // Locate the linked page index in the book and select it
    int index = SimpleBookGetPageIndex(page);
    if(index != wxNOT_FOUND) {
        m_book->SetSelection(index);
    }
}

void clSideBarCtrl::ChangeSelection(size_t pos)
{
    // Set the selection in the buttons bar and return the linked page
    wxWindow* page = m_buttons->ChangeSelection(pos);
    CHECK_POINTER_RETURN(page, );

    // Locate the linked page index in the book and select it
    int index = SimpleBookGetPageIndex(page);
    if(index != wxNOT_FOUND) {
        m_book->ChangeSelection(index);
    }
}

size_t clSideBarCtrl::GetPageCount() const { return m_buttons->GetButtonCount(); }

wxWindow* clSideBarCtrl::GetPage(size_t pos) const
{
    // the index is managed by the buttons bar
    auto button = m_buttons->GetButton(pos);
    CHECK_POINTER_RETURN(button, nullptr);

    return button->GetLinkedPage();
}

wxBitmap clSideBarCtrl::GetPageBitmap(size_t pos) const
{
    auto button = m_buttons->GetButton(pos);
    CHECK_POINTER_RETURN(button, wxNullBitmap);
    return button->GetButtonBitmap();
}

wxString clSideBarCtrl::GetPageText(size_t pos) const
{
    auto button = m_buttons->GetButton(pos);
    CHECK_POINTER_RETURN(button, wxEmptyString);

    return button->GetButtonLabel();
}

wxWindow* clSideBarCtrl::GetPage(const wxString& label) const
{
    // the index is managed by the buttons bar
    for(size_t i = 0; i < m_buttons->GetButtonCount(); ++i) {
        if(m_buttons->GetButton(i)->GetButtonLabel() == label) {
            return m_buttons->GetButton(i)->GetLinkedPage();
        }
    }
    return nullptr;
}

int clSideBarCtrl::SimpleBookGetPageIndex(wxWindow* page) const
{
    for(size_t i = 0; i < m_book->GetPageCount(); ++i) {
        if(m_book->GetPage(i) == page) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

int clSideBarCtrl::GetSelection() const { return m_buttons->GetSelection(); }

int clSideBarCtrl::GetPageIndex(wxWindow* page) const { return m_buttons->GetPageIndex(page); }

int clSideBarCtrl::GetPageIndex(const wxString& label) const { return m_buttons->GetPageIndex(label); }

void clSideBarCtrl::SetButtonPosition(wxDirection direction)
{
    m_buttonsPosition = direction;
    m_buttons->SetButtonsPosition(m_buttonsPosition);
    PlaceButtons();
}

void clSideBarCtrl::MovePageToIndex(const wxString& label, int new_pos)
{
    auto src_button = m_buttons->GetButton(label);
    auto target_button = m_buttons->GetButton(new_pos);

    if(!src_button || !target_button || src_button == target_button) {
        return;
    }
    m_buttons->MoveBefore(src_button, target_button);
}

void clSideBarCtrl::OnSize(wxSizeEvent& event)
{
    event.Skip();
    GetSizer()->Layout();
}
