#include "clSideBarCtrl.hpp"

#include "bitmap_loader.h"

#include <wx/anybutton.h>
#include <wx/dcbuffer.h>
#include <wx/gdicmn.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/toolbar.h>
#include <wx/tooltip.h>

thread_local int BUTTON_ID = 0;
constexpr int BAR_SIZE = 24;

wxDEFINE_EVENT(wxEVT_SIDEBAR_SELECTION_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_SIDEBAR_CONTEXT_MENU, wxContextMenuEvent);

#define CHECK_POINTER_RETURN(ptr, WHAT) \
    if (!ptr)                           \
        return WHAT;

#define CHECK_POSITION_RETURN(pos, WHAT)         \
    if ((size_t)pos >= m_book->GetPageCount()) { \
        return WHAT;                             \
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

void paint_background(wxDC& dc, wxWindow* win, const wxRect& rect)
{
    wxUnusedVar(win);
    wxColour bg_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

    dc.SetPen(bg_colour);
    dc.SetBrush(bg_colour);
    dc.DrawRectangle(rect);
}
} // namespace

#if 0
class SideBarButton
{
    friend class clSideBarButtonCtrl;
    friend class clSideBarCtrl;

protected:
    clSideBarButtonCtrl* m_buttons_bar = nullptr;
    bool m_selected = false;
    wxWindow* m_linkedPage = nullptr;
    wxString m_label;
    wxString m_bmpName;
    wxRect m_clientRect;

public:
    void Render(wxDC& dc, const wxBitmap& bmp, const wxRect& rect)
    {
        m_clientRect = rect;

        // draw the background
        wxColour base_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        bool is_dark = DrawingUtils::IsDark(base_colour);
        paint_background(dc, m_buttons_bar, m_clientRect);
        if (IsSeleced()) {
            // draw the selected item using different background colour
            wxColour colour = base_colour;
            if (is_dark) {
                colour = colour.ChangeLightness(115);
            } else {
                colour = *wxWHITE;
            }
            dc.SetBrush(colour);
            dc.SetPen(colour);
            dc.DrawRectangle(m_clientRect);
        }

        wxRect bmp_rect = wxRect(bmp.GetLogicalSize()).CenterIn(m_clientRect);
        dc.DrawBitmap(bmp, bmp_rect.GetTopLeft());
    }

    void ShowContextMenu(wxContextMenuEvent& event)
    {
        wxUnusedVar(event);
        wxContextMenuEvent menu_event{ wxEVT_SIDEBAR_CONTEXT_MENU };
        menu_event.SetInt(m_buttons_bar->GetButtonIndex(this));
        menu_event.SetEventObject(m_buttons_bar->GetParent());
        m_buttons_bar->GetParent()->GetEventHandler()->ProcessEvent(menu_event);
    }

    explicit SideBarButton(clSideBarButtonCtrl* parent, const wxString& bmpname)
        : m_buttons_bar(parent)
        , m_bmpName(bmpname)
    {
    }

    virtual ~SideBarButton() {}

    bool Contains(const wxPoint& point) const { return m_clientRect.Contains(point); }

    void SetLinkedPage(wxWindow* win) { m_linkedPage = win; }
    wxWindow* GetLinkedPage() const { return m_linkedPage; }

    void SetSeleced(bool b) { m_selected = b; }
    bool IsSeleced() const { return m_selected; }
    void SetPageLabel(const wxString& label) { this->m_label = label; }
    const wxString& GetButtonLabel() const { return m_label; }
    const wxString& GetBitmapName() const { return m_bmpName; }
    void SetBitmapName(const wxString& bmpname) { m_bmpName = bmpname; }
};

clSideBarButtonCtrl::clSideBarButtonCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                         long style)
    : wxControl(parent, id, pos, size, style | wxBORDER_NONE)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &clSideBarButtonCtrl::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &clSideBarButtonCtrl::OnLeftDown, this);
    Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent& e) { wxUnusedVar(e); });
}

clSideBarButtonCtrl::~clSideBarButtonCtrl()
{
    Unbind(wxEVT_PAINT, &clSideBarButtonCtrl::OnPaint, this);
    Unbind(wxEVT_LEFT_DOWN, &clSideBarButtonCtrl::OnLeftDown, this);
    Unbind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent& e) { wxUnusedVar(e); });
}

void clSideBarButtonCtrl::SetButtonsPosition(wxDirection direction)
{
    m_buttonsPosition = direction;
    int dim = FromDIP(BAR_SIZE);
    if (IsHorizontalLayout()) {
        SetSizeHints(wxNOT_FOUND, dim);
    } else {
        SetSizeHints(dim, wxNOT_FOUND);
    }
    Refresh();
}

void clSideBarButtonCtrl::LoadBitmaps()
{
    m_bitmaps.clear();
    for (auto button : m_buttons) {
        m_bitmaps.insert({ button->GetBitmapName(), clLoadSidebarBitmap(button->GetBitmapName(), this) });
    }
}

void clSideBarButtonCtrl::OnLeftDown(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    auto button = GetButtonFromPos(pos);
    CHECK_POINTER_RETURN(button, );
    SetSelection(GetButtonIndex(button));
}

void clSideBarButtonCtrl::OnPaint(wxPaintEvent& event)
{
    wxUnusedVar(event);
    if (m_bitmaps.empty()) {
        LoadBitmaps();
    }

    wxBufferedPaintDC dc(this);
    paint_background(dc, this, GetClientRect());

    int dim = FromDIP(BAR_SIZE);
    int xx = 0;
    int yy = 0;
    for (auto button : m_buttons) {
        wxRect button_rect{ xx, yy, dim, dim };
        const wxBitmap& bmp = m_bitmaps[button->GetBitmapName()];
        button->Render(dc, bmp, button_rect);
        if (IsHorizontalLayout()) {
            xx += dim;
        } else {
            yy += dim;
        }
    }
}

int clSideBarButtonCtrl::AddButton(const wxString& bitmap_name, const wxString& label, wxWindow* linked_page,
                                   bool select)
{
    SideBarButton* btn = new SideBarButton(this, bitmap_name);
    if (select) {
        // unselect the old button
        int old_selection = GetSelection();
        if (old_selection != wxNOT_FOUND) {
            GetButton(old_selection)->SetSeleced(false);
        }
    }

    btn->SetSeleced(select);
    btn->SetPageLabel(label);
    btn->SetLinkedPage(linked_page);

    m_buttons.push_back(btn);
    return m_buttons.size() - 1;
}

void clSideBarButtonCtrl::Clear()
{
    m_bitmaps.clear();
    for (auto button : m_buttons) {
        wxDELETE(button);
    }
    m_buttons.clear();
    Refresh();
}

wxWindow* clSideBarButtonCtrl::RemoveButton(size_t pos)
{
    auto button = GetButton(pos);
    CHECK_POINTER_RETURN(button, nullptr);

    auto linked_page = button->GetLinkedPage();
    m_buttons.erase(m_buttons.begin() + pos);
    m_bitmaps.erase(button->GetBitmapName());
    wxDELETE(button);

    // select the first item
    SetSelection(0);
    return linked_page;
}

void clSideBarButtonCtrl::MoveAfter(SideBarButton* src, SideBarButton* target)
{
    wxUnusedVar(src);
    Refresh();
}

void clSideBarButtonCtrl::MoveBefore(SideBarButton* src, SideBarButton* target)
{
    wxUnusedVar(src);
    Refresh();
}

int clSideBarButtonCtrl::GetButtonIndex(SideBarButton* btn) const
{
    auto iter =
        std::find_if(m_buttons.begin(), m_buttons.end(), [btn](SideBarButton* button) { return button == btn; });
    if (iter == m_buttons.end()) {
        return wxNOT_FOUND;
    }
    return std::distance(m_buttons.begin(), iter);
}

const std::vector<SideBarButton*>& clSideBarButtonCtrl::GetAllButtons() const { return m_buttons; }

int clSideBarButtonCtrl::GetSelection() const
{
    for (size_t i = 0; i < m_buttons.size(); ++i) {
        auto button = m_buttons[i];
        if (button->IsSeleced()) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

SideBarButton* clSideBarButtonCtrl::GetButtonFromPos(const wxPoint& pt) const
{
    for (size_t i = 0; i < m_buttons.size(); ++i) {
        auto button = m_buttons[i];
        if (button->Contains(pt)) {
            return button;
        }
    }
    return nullptr;
}

wxWindow* clSideBarButtonCtrl::SetSelection(int pos) { return DoChangeSelection(pos, true); }

wxWindow* clSideBarButtonCtrl::ChangeSelection(int pos) { return DoChangeSelection(pos, false); }

wxWindow* clSideBarButtonCtrl::DoChangeSelection(int pos, bool notify)
{
    CHECK_POSITION_RETURN(pos, nullptr);

    SideBarButton* button = GetButton(pos);
    CHECK_POINTER_RETURN(button, nullptr);

    auto old_selection = GetButton(GetSelection());
    if (old_selection) {
        old_selection->SetSeleced(false);
    }
    button->SetSeleced(true);
    Refresh();

    if (notify) {
        wxCommandEvent selection_changed(wxEVT_SIDEBAR_SELECTION_CHANGED);
        selection_changed.SetEventObject(GetParent());
        selection_changed.SetInt(pos);
        AddPendingEvent(selection_changed);
    }
    return button->GetLinkedPage();
}

size_t clSideBarButtonCtrl::GetButtonCount() const { return m_buttons.size(); }

wxWindow* clSideBarButtonCtrl::GetSelectionLinkedPage() const
{
    auto button = GetButton(GetSelection());
    CHECK_POINTER_RETURN(button, nullptr);
    return button->GetLinkedPage();
}

SideBarButton* clSideBarButtonCtrl::GetButton(size_t pos) const
{
    CHECK_POSITION_RETURN(pos, nullptr);
    return m_buttons[pos];
}

SideBarButton* clSideBarButtonCtrl::GetButton(const wxString& label) const
{
    if (label.empty()) {
        return nullptr;
    }
    for (size_t i = 0; i < m_buttons.size(); ++i) {
        if (m_buttons[i]->GetButtonLabel() == label) {
            return m_buttons[i];
        }
    }
    return nullptr;
}

int clSideBarButtonCtrl::GetPageIndex(const wxString& label) const
{
    if (label.empty()) {
        return wxNOT_FOUND;
    }
    for (size_t i = 0; i < m_buttons.size(); ++i) {
        if (m_buttons[i]->GetButtonLabel() == label) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

int clSideBarButtonCtrl::GetPageIndex(wxWindow* page) const
{
    if (!page) {
        return wxNOT_FOUND;
    }

    for (size_t i = 0; i < m_buttons.size(); ++i) {
        if (m_buttons[i]->GetLinkedPage() == page) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

bool clSideBarButtonCtrl::IsHorizontalLayout() const
{
    return m_buttonsPosition == wxTOP || m_buttonsPosition == wxBOTTOM;
}
#endif

// -----------------------
// SideBar control
// -----------------------

clSideBarCtrl::clSideBarCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxControl(parent, id, pos, size, border_simple_theme_aware_bit())
{
    bool vertical_sizer = style & (wxBK_TOP | wxBK_BOTTOM);
    m_mainSizer = new wxBoxSizer(vertical_sizer ? wxVERTICAL : wxHORIZONTAL);

    SetSizer(m_mainSizer);
    m_book = new wxSimplebook(this);

    // when we use a vertical sizer (i.e. the buttons are placed under or on top of the book,
    // we set the buttons layout to be horizontal)
    m_buttonsPosition = wxLEFT;
    if (style & wxBK_TOP) {
        m_buttonsPosition = wxTOP;
    } else if (style & wxBK_RIGHT) {
        m_buttonsPosition = wxRIGHT;
    } else if (style & wxBK_BOTTOM) {
        m_buttonsPosition = wxBOTTOM;
    } else /* assume left, the default */ {
        m_buttonsPosition = wxLEFT;
    }
    PlaceButtons();
    Bind(wxEVT_SIZE, &clSideBarCtrl::OnSize, this);
}

clSideBarCtrl::~clSideBarCtrl() { Unbind(wxEVT_SIZE, &clSideBarCtrl::OnSize, this); }

void clSideBarCtrl::PlaceButtons()
{
    GetSizer()->Detach(m_book);
    bool vertical_toolbar = (m_buttonsPosition == wxLEFT || m_buttonsPosition == wxRIGHT);

#if USE_AUI_TOOLBAR
    long tb_style = wxAUI_TB_DEFAULT_STYLE;
    if (vertical_toolbar) {
        tb_style |= wxAUI_TB_VERTICAL;
        tb_style |= wxAUI_TB_PLAIN_BACKGROUND;
    } else {
        tb_style |= wxAUI_TB_HORIZONTAL;
    }
#else
    long tb_style = wxTB_NODIVIDER;
    if (vertical_toolbar) {
        tb_style |= wxTB_VERTICAL;
    }
#endif

    tb_style |= wxBORDER_NONE;
    if (!m_toolbar) {
#if USE_AUI_TOOLBAR
        m_toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, tb_style);
#else
        m_toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, tb_style);
#endif

    } else {
        GetSizer()->Detach(m_toolbar);
    }

#ifdef __WXGTK__
    if (m_toolbar->GetToolsCount()) {
        auto tool = m_toolbar->GetToolByPos(0);
        if (tool) {
            if (vertical_toolbar) {
                m_toolbar->SetSize(wxSize(48, -1));
                m_toolbar->SetSizeHints(wxSize(48, -1));
            } else {
                m_toolbar->SetSize(wxSize(-1, 48));
                m_toolbar->SetSizeHints(wxSize(-1, 48));
            }
        }
    }
#endif

#if USE_AUI_TOOLBAR
    // need to re-create the toolbar
    wxAuiToolBar* old_toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, tb_style);
    wxSwap(old_toolbar, m_toolbar);
    size_t tools_count = old_toolbar->GetToolCount();
    for (size_t i = 0; i < tools_count; ++i) {
        auto tool = old_toolbar->FindToolByIndex(i);
        AddTool(tool->GetLabel(), tool->GetBitmap(), i);
    }
    wxDELETE(old_toolbar);
#else
    m_toolbar->SetWindowStyle(tb_style);
#endif

    m_toolbar->Realize();

    // adjust the sizer orientation
    m_mainSizer->SetOrientation((m_buttonsPosition == wxLEFT || m_buttonsPosition == wxRIGHT) ? wxHORIZONTAL
                                                                                              : wxVERTICAL);

    switch (m_buttonsPosition) {
    case wxRIGHT:
    case wxBOTTOM:
        GetSizer()->Add(m_book, 1, wxEXPAND);
        GetSizer()->Add(m_toolbar, 0, wxEXPAND);
        break;
    default:
    case wxLEFT:
    case wxTOP:
        GetSizer()->Add(m_toolbar, 0, wxEXPAND);
        GetSizer()->Add(m_book, 1, wxEXPAND);
        break;
    }
    GetSizer()->Layout();
}

void clSideBarCtrl::AddTool(const wxString& label, const wxBitmap& bmp, size_t book_index)
{
    auto tool = m_toolbar->AddTool(wxID_ANY, label, bmp, label, wxITEM_CHECK);
    m_toolbar->Bind(
        wxEVT_TOOL,
        [book_index, this](wxCommandEvent& event) {
            wxUnusedVar(event);
            m_book->ChangeSelection(book_index);
        },
        tool->GetId());

    m_toolbar->Bind(
        wxEVT_UPDATE_UI,
        [book_index, this](wxUpdateUIEvent& event) { event.Check(m_book->GetSelection() == book_index); },
        tool->GetId());
}

void clSideBarCtrl::AddPage(wxWindow* page, const wxString& label, const wxString& bmpname, bool selected)
{
    page->Reparent(m_book);
    size_t page_index = m_book->GetPageCount();
    m_book->AddPage(page, label, selected);

    wxBitmap bmp = ::clLoadSidebarBitmap(bmpname, this);
    if (!bmp.IsOk()) {
        clWARNING() << "clSideBarCtrl::AddPage(): Invalid bitmap:" << bmpname << endl;
    }

    m_bitmapByPos.erase(page_index);
    m_bitmapByPos.insert({ page_index, bmpname });

    AddTool(label, bmp, page_index);
}

void clSideBarCtrl::DoRemovePage(size_t pos, bool delete_it)
{
    // remove the button and return the associated page
#if USE_AUI_TOOLBAR
    auto tool = m_toolbar->FindToolByIndex(pos);
#else
    auto tool = m_toolbar->GetToolByPos(pos);
#endif
    CHECK_POINTER_RETURN(tool, );

    if (delete_it) {
        m_book->DeletePage(pos);
    } else {
        m_book->RemovePage(pos);
    }

    int tool_id = tool->GetId();
#if USE_AUI_TOOLBAR
    bool was_selection = tool->IsActive();
#else
    bool was_selection = tool->IsToggled();
#endif

    m_toolbar->DeleteTool(tool->GetId());
    m_bitmapByPos.erase(pos);

    // sync the selection between the book and the button bar
    if (was_selection && m_book->GetPageCount()) {
        m_book->ChangeSelection(0);
    }
}

void clSideBarCtrl::RemovePage(size_t pos) { DoRemovePage(pos, false); }

void clSideBarCtrl::DeletePage(size_t pos) { DoRemovePage(pos, true); }

void clSideBarCtrl::SetSelection(size_t pos) { ChangeSelection(pos); }

void clSideBarCtrl::ChangeSelection(size_t pos)
{
    if (pos >= m_book->GetPageCount()) {
        return;
    }
    m_book->ChangeSelection(pos);
}

size_t clSideBarCtrl::GetPageCount() const { return m_book->GetPageCount(); }

wxWindow* clSideBarCtrl::GetPage(size_t pos) const
{
    CHECK_POSITION_RETURN(pos, nullptr);
    return m_book->GetPage(pos);
}

wxString clSideBarCtrl::GetPageBitmap(size_t pos) const
{
    if (m_bitmapByPos.count(pos) == 0) {
        return wxEmptyString;
    }
    return m_bitmapByPos.find(pos)->second;
}

void clSideBarCtrl::SetPageBitmap(size_t pos, const wxString& bmp)
{
    CHECK_POSITION_RETURN(pos, );

    m_bitmapByPos.erase(pos);
    m_bitmapByPos.insert({ pos, bmp });

#if USE_AUI_TOOLBAR
    auto tool = m_toolbar->FindToolByIndex(pos);
#else
    auto tool = m_toolbar->GetToolByPos(pos);
#endif

    CHECK_POINTER_RETURN(tool, );

#if USE_AUI_TOOLBAR
    tool->SetBitmap(::clLoadSidebarBitmap(bmp, m_toolbar));
#else
    tool->SetNormalBitmap(::clLoadSidebarBitmap(bmp, m_toolbar));
#endif
}

wxString clSideBarCtrl::GetPageText(size_t pos) const { return m_book->GetPageText(pos); }

wxWindow* clSideBarCtrl::GetPage(const wxString& label) const
{
    // the index is managed by the buttons bar
    for (size_t i = 0; i < m_book->GetPageCount(); ++i) {
        if (m_book->GetPageText(i) == label) {
            return m_book->GetPage(i);
        }
    }
    return nullptr;
}

int clSideBarCtrl::SimpleBookGetPageIndex(wxWindow* page) const
{
    for (size_t i = 0; i < m_book->GetPageCount(); ++i) {
        if (m_book->GetPage(i) == page) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

int clSideBarCtrl::GetSelection() const { return m_book->GetSelection(); }

int clSideBarCtrl::GetPageIndex(wxWindow* page) const
{
    for (size_t i = 0; i < m_book->GetPageCount(); ++i) {
        if (m_book->GetPage(i) == page) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

int clSideBarCtrl::GetPageIndex(const wxString& label) const
{
    for (size_t i = 0; i < m_book->GetPageCount(); ++i) {
        if (m_book->GetPageText(i) == label) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

void clSideBarCtrl::SetButtonPosition(wxDirection direction)
{
    m_buttonsPosition = direction;
    PlaceButtons();
}

void clSideBarCtrl::MovePageToIndex(const wxString& label, int new_pos)
{
    wxUnusedVar(label);
    wxUnusedVar(new_pos);
}

void clSideBarCtrl::OnSize(wxSizeEvent& event)
{
    event.Skip();
    GetSizer()->Layout();
}

void clSideBarCtrl::Realize() { m_toolbar->Realize(); }
