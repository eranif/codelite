#include "clSideBarCtrl.hpp"

#include "bitmap_loader.h"
#include "clSystemSettings.h"

#include <wx/anybutton.h>
#include <wx/dcbuffer.h>
#include <wx/gdicmn.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/toolbar.h>
#include <wx/tooltip.h>

wxDEFINE_EVENT(wxEVT_SIDEBAR_SELECTION_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_SIDEBAR_CONTEXT_MENU, wxContextMenuEvent);

#define CHECK_POINTER_RETURN(ptr, WHAT) \
    if (!ptr)                           \
        return WHAT;

#define CHECK_POSITION_RETURN(pos, WHAT)         \
    if ((size_t)pos >= m_book->GetPageCount()) { \
        return WHAT;                             \
    }

#if USE_NATIVETOOLBAR
#define TOOL_GET_USER_DATA(tool) m_toolbar->GetUserData(tool)
#define TOOL_SET_USER_DATA(tool, data) m_toolbar->SetUserData(tool, data)
#define TOOL_SET_BITMAP(tool, bmp) tool->SetNormalBitmap(bmp)
#define TOOL_IS_CHECKED(tool) tool->IsToggled()
#else
#define TOOL_GET_USER_DATA(tool) tool->GetUserData()
#define TOOL_SET_BITMAP(tool, bmp) tool->SetBitmap(bmp)
#define TOOL_SET_USER_DATA(tool, data) tool->SetUserData(data)
#define TOOL_IS_CHECKED(tool) tool->IsActive()
#endif

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

/// Return true if we are running under Windows 11 in dark mode
bool IsWindows11DarkMode()
{
#ifdef __WXMSW__
    static int major = wxNOT_FOUND;
    static int min = wxNOT_FOUND;
    static int build = wxNOT_FOUND;
    if (major == wxNOT_FOUND) {
        ::wxGetOsVersion(&major, &min, &build);
    }
    return major == 10 && build >= 22000 && clSystemSettings::GetAppearance().IsDark();
#else
    return false;
#endif
}
} // namespace

#if USE_NATIVETOOLBAR
// SideBarToolBar
SideBarToolBar::SideBarToolBar(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    long tb_style = wxTB_NODIVIDER;
    wxSize sz = wxDefaultSize;
    wxBitmap dark_theme_bmp, light_theme_bmp;
    ::clLoadSidebarBitmap("workspace-button", parent, &light_theme_bmp, &dark_theme_bmp);
    if (style & wxAUI_TB_VERTICAL) {
        tb_style |= wxTB_VERTICAL;
        sz.SetWidth(dark_theme_bmp.GetScaledWidth());
    } else if (style & wxAUI_TB_HORIZONTAL) {
        tb_style |= wxTB_HORIZONTAL;
        sz.SetHeight(dark_theme_bmp.GetScaledHeight());
    }

    wxUnusedVar(size);
    wxToolBar::Create(parent, id, pos, wxDefaultSize, tb_style);
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
    Bind(wxEVT_DPI_CHANGED, &clSideBarCtrl::OnDPIChangedEvent, this);
}

clSideBarCtrl::~clSideBarCtrl()
{
    Unbind(wxEVT_SIZE, &clSideBarCtrl::OnSize, this);
    Unbind(wxEVT_DPI_CHANGED, &clSideBarCtrl::OnDPIChangedEvent, this);
}

const clSideBarToolData* clSideBarCtrl::GetToolData(long id) const
{
    if (m_toolDataMap.count(id) == 0) {
        return nullptr;
    }
    return &m_toolDataMap.find(id)->second;
}

void clSideBarCtrl::DeleteToolData(long id) { m_toolDataMap.erase(id); }

long clSideBarCtrl::AddToolData(clSideBarToolData data)
{
    static long tool_data_id = 0;

    long next_id = ++tool_data_id;
    m_toolDataMap.insert({ next_id, data });
    return next_id;
}

void clSideBarCtrl::ClearAllToolData() { m_toolDataMap.clear(); }

void clSideBarCtrl::PlaceButtons()
{
    GetSizer()->Detach(m_book);
    bool vertical_toolbar = (m_buttonsPosition == wxLEFT || m_buttonsPosition == wxRIGHT);

    long tb_style = wxAUI_TB_DEFAULT_STYLE;
    if (vertical_toolbar) {
        tb_style |= wxAUI_TB_VERTICAL;
        tb_style |= wxAUI_TB_PLAIN_BACKGROUND;
    } else {
        tb_style |= wxAUI_TB_HORIZONTAL;
    }

    tb_style |= wxBORDER_NONE;
    if (!m_toolbar) {
        m_toolbar = new SideBarToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, tb_style);

    } else {
        GetSizer()->Detach(m_toolbar);
    }

    // need to re-create the toolbar
    SideBarToolBar* old_toolbar = new SideBarToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, tb_style);
    wxSwap(old_toolbar, m_toolbar);

    size_t tools_count = old_toolbar->GetToolCount();
    for (size_t i = 0; i < tools_count; ++i) {
        auto tool = old_toolbar->FindToolByIndex(i);
        auto tool_data_id = TOOL_GET_USER_DATA(tool);
        const clSideBarToolData* cd = GetToolData(tool_data_id);
        AddTool(tool->GetLabel(), cd ? cd->data : wxString(), i);
        DeleteToolData(tool_data_id);
    }
    wxDELETE(old_toolbar);

    // Reconnect the event
#if USE_NATIVETOOLBAR
    m_toolbar->Bind(wxEVT_TOOL_RCLICKED, &clSideBarCtrl::OnContextMenu, this);
#else
    m_toolbar->Bind(wxEVT_AUITOOLBAR_RIGHT_CLICK, &clSideBarCtrl::OnContextMenu, this);
#endif
    m_toolbar->Realize();

    // adjust the sizer orientation
    m_mainSizer->SetOrientation((m_buttonsPosition == wxLEFT || m_buttonsPosition == wxRIGHT) ? wxHORIZONTAL
                                                                                              : wxVERTICAL);
    int border = 0;
    switch (m_buttonsPosition) {
    case wxRIGHT:
    case wxBOTTOM:
        GetSizer()->Add(m_book, 1, wxEXPAND);
        GetSizer()->Add(m_toolbar, 0, wxEXPAND | wxALL, border);
        break;
    default:
    case wxLEFT:
    case wxTOP:
        GetSizer()->Add(m_toolbar, 0, wxEXPAND | wxALL, border);
        GetSizer()->Add(m_book, 1, wxEXPAND);
        break;
    }
    GetSizer()->Layout();
}

void clSideBarCtrl::MSWUpdateToolbarBitmaps(int new_selection, int old_selection)
{
    wxUnusedVar(new_selection);
    wxUnusedVar(old_selection);
}

void clSideBarCtrl::AddTool(const wxString& label, const wxString& bmpname, size_t book_index)
{
    wxBitmap dark_theme_bmp, light_theme_bmp;
    ::clLoadSidebarBitmap(bmpname, this, &light_theme_bmp, &dark_theme_bmp);
    if (!light_theme_bmp.IsOk() || !dark_theme_bmp.IsOk()) {
        clWARNING() << "clSideBarCtrl::AddPage(): Invalid bitmap:" << bmpname << endl;
    }

    const wxBitmap& bmp = clSystemSettings::GetAppearance().IsDark()
                              // Under Windows 11, the toolbar selection is "very light"
                              // so use the light theme bitmap
                              ? (IsWindows11DarkMode() ? light_theme_bmp : dark_theme_bmp)
                              : light_theme_bmp;

    auto tool = m_toolbar->AddTool(wxID_ANY, label, wxBitmapBundle(bmp), label, wxITEM_CHECK);
    auto tool_id = tool->GetId();
    long tool_data_id = AddToolData(clSideBarToolData(bmpname));
    TOOL_SET_USER_DATA(tool, tool_data_id);

    m_toolbar->Bind(
        wxEVT_TOOL,
        [label, this](wxCommandEvent& event) {
            wxUnusedVar(event);
            int book_index = GetPageIndex(label);
            ChangeSelection(book_index);
        },
        tool->GetId());

    m_toolbar->Bind(
        wxEVT_UPDATE_UI,
        [label, tool_id, this](wxUpdateUIEvent& event) {
            int book_index = GetPageIndex(label);
            bool is_checked = m_book->GetSelection() == book_index;
            event.Check(is_checked);
        },
        tool->GetId());
}

void clSideBarCtrl::AddPage(wxWindow* page, const wxString& label, const wxString& bmpname, bool selected)
{
    page->Reparent(m_book);
    size_t page_index = m_book->GetPageCount();
    m_book->AddPage(page, label, selected);
    AddTool(label, bmpname, page_index);
}

void clSideBarCtrl::DoRemovePage(size_t pos, bool delete_it)
{
    // remove the button and return the associated page
    auto tool = m_toolbar->FindToolByIndex(pos);
    CHECK_POINTER_RETURN(tool, );
    if (delete_it) {
        m_book->DeletePage(pos);
    } else {
        m_book->RemovePage(pos);
    }

    int tool_id = tool->GetId();
    bool was_selection = TOOL_IS_CHECKED(tool);

    m_toolbar->DeleteTool(tool->GetId());

    // sync the selection between the book and the button bar
    if (was_selection && m_book->GetPageCount()) {
        ChangeSelection(0);
    }
}

void clSideBarCtrl::RemovePage(size_t pos) { DoRemovePage(pos, false); }

void clSideBarCtrl::DeletePage(size_t pos) { DoRemovePage(pos, true); }

void clSideBarCtrl::SetSelection(size_t pos) { ChangeSelection(pos); }

int clSideBarCtrl::GetToolIdForBookPos(int book_index) const
{
    if (book_index < 0 || book_index >= (int)m_book->GetPageCount()) {
        return wxNOT_FOUND;
    }

    wxString label = m_book->GetPageText(book_index);
    for (size_t i = 0; i < m_toolbar->GetToolCount(); ++i) {
#if USE_NATIVETOOLBAR
        auto tool = m_toolbar->GetToolByPos(i);
#else
        auto tool = m_toolbar->FindToolByIndex(i);
#endif
        if (tool->GetLabel() == label) {
            return tool->GetId();
        }
    }
    return wxNOT_FOUND;
}

void clSideBarCtrl::ChangeSelection(size_t pos)
{
    if (pos >= m_book->GetPageCount()) {
        return;
    }

    int new_tool_id = GetToolIdForBookPos(pos);
    int old_tool_id = GetToolIdForBookPos(m_book->GetSelection());
    CallAfter(&clSideBarCtrl::MSWUpdateToolbarBitmaps, new_tool_id, old_tool_id);
    m_selectedToolId = new_tool_id;
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
    CHECK_POSITION_RETURN(pos, wxEmptyString);
    auto tool = m_toolbar->FindToolByIndex(pos);

    CHECK_POINTER_RETURN(tool, wxEmptyString);
    auto tool_data = GetToolData(TOOL_GET_USER_DATA(tool));
    CHECK_POINTER_RETURN(tool_data, wxEmptyString);

    return tool_data->data;
}

void clSideBarCtrl::SetPageBitmap(size_t pos, const wxString& bmpname)
{
    CHECK_POSITION_RETURN(pos, );

    auto tool = m_toolbar->FindToolByIndex(pos);
    CHECK_POINTER_RETURN(tool, );

    long tool_data_idx = TOOL_GET_USER_DATA(tool);
    const clSideBarToolData* cd = GetToolData(tool_data_idx);
    if (cd) {
        const_cast<clSideBarToolData*>(cd)->data = bmpname;
    }

    wxBitmap light_theme_bmp, dark_theme_bmp;
    ::clLoadSidebarBitmap(bmpname, m_toolbar, &light_theme_bmp, &dark_theme_bmp);

    const wxBitmap& bmp = clSystemSettings::GetAppearance().IsDark()
                              // Under Windows 11, the toolbar selection is "very light"
                              // so use the light theme bitmap
                              ? (IsWindows11DarkMode() ? light_theme_bmp : dark_theme_bmp)
                              : light_theme_bmp;
    TOOL_SET_BITMAP(tool, bmp);
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

void clSideBarCtrl::Realize()
{
    m_toolbar->Realize();
    SendSizeEvent(wxSEND_EVENT_POST);
}

void clSideBarCtrl::OnContextMenu(
#if USE_NATIVETOOLBAR
    wxCommandEvent& event
#else
    wxAuiToolBarEvent& event
#endif
)
{
    event.Skip();
#if USE_NATIVETOOLBAR
    int tool_id = event.GetId();
#else
    int tool_id = event.GetToolId();
#endif

    CHECK_COND_RET(tool_id != wxNOT_FOUND);

    auto tool = m_toolbar->FindTool(tool_id);
    CHECK_POINTER_RETURN(tool, );

    int book_index = GetPageIndex(tool->GetLabel());
    CHECK_COND_RET(book_index != wxNOT_FOUND);

    wxContextMenuEvent menu_event{ wxEVT_SIDEBAR_CONTEXT_MENU };
    menu_event.SetEventObject(this);
    menu_event.SetInt(book_index);
    GetEventHandler()->ProcessEvent(menu_event);
}

void clSideBarCtrl::OnDPIChangedEvent(wxDPIChangedEvent& event)
{
    event.Skip();
    clDEBUG() << "DPI changed event captured. Rebuilding SideBar control" << endl;
    // clear the bitmaps cache and rebuild the control
    ::clClearSidebarBitmapCache();
    PlaceButtons();
}
