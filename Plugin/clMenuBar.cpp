#include "clMenuBar.hpp"
#if !wxUSE_NATIVE_MENUBAR
#if CL_BUILD
#include "file_logger.h"
#endif
#include <deque>
#include <drawingutils.h>
#include <wx/dc.h>
#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/dcgraph.h>
#include <wx/frame.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/xrc/xmlres.h>

clMenuBar::clMenuBar() {}

clMenuBar::clMenuBar(wxWindow* parent, size_t n, wxMenu* menus[], const wxString titles[], long style)
    : wxWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
    m_colours.InitDefaults();
    wxUnusedVar(style);
    m_menus.reserve(n);
    for(size_t i = 0; i < n; ++i) {
        DoAppend(menus[i], titles[i]);
    }

    Bind(wxEVT_PAINT, &clMenuBar::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &clMenuBar::OnEraseBg, this);
    Bind(wxEVT_LEFT_DOWN, &clMenuBar::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &clMenuBar::OnLeftUp, this);
    Bind(wxEVT_MOTION, &clMenuBar::OnMotion, this);
    Bind(wxEVT_ENTER_WINDOW, &clMenuBar::OnEnterWindow, this);
    Bind(wxEVT_LEAVE_WINDOW, &clMenuBar::OnLeaveWindow, this);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    DoSetBestSize();
    UpdateAccelerators();
}

clMenuBar::~clMenuBar()
{
    Unbind(wxEVT_PAINT, &clMenuBar::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, &clMenuBar::OnEraseBg, this);
    Unbind(wxEVT_LEFT_DOWN, &clMenuBar::OnLeftDown, this);
    Unbind(wxEVT_LEFT_UP, &clMenuBar::OnLeftUp, this);
    Unbind(wxEVT_MOTION, &clMenuBar::OnMotion, this);
    Unbind(wxEVT_ENTER_WINDOW, &clMenuBar::OnEnterWindow, this);
    Unbind(wxEVT_LEAVE_WINDOW, &clMenuBar::OnLeaveWindow, this);

    // free the menus
    for(auto& mi : m_menus) {
        if(mi.menu) {
            wxDELETE(mi.menu);
        }
    }
    m_menus.clear();
}

wxMenuItem* clMenuBar::DoFindMenuItem(int id, wxMenu** parent) const
{
    wxMenuItem* mi = nullptr;
    for(auto d : m_menus) {
        if((mi = d.menu->FindItem(id, nullptr))) {
            *parent = d.menu;
            return mi;
        }
    }
    return nullptr;
}

clMenuBar::menu_info* clMenuBar::DoFindMenu(wxMenu* menu) const
{
    for(auto& d : m_menus) {
        if(menu == d.menu) {
            return const_cast<menu_info*>(&d);
        }
    }
    return nullptr;
}

bool clMenuBar::DoAppend(wxMenu* menu, const wxString& title)
{
    for(auto d : m_menus) {
        if(d.menu == menu) {
            return false;
        }
    }

    menu_info mi = { title, menu, true };
    m_menus.emplace_back(mi);
    return true;
}

bool clMenuBar::Append(wxMenu* menu, const wxString& title)
{
    if(!DoAppend(menu, title)) {
        return false;
    }

    UpdateAccelerators();
    Refresh();
    return true;
}

void clMenuBar::Check(int id, bool check)
{
    auto item = DoFindMenuItem(id);
    if(item) {
        item->Check(check);
    }
}

void clMenuBar::Enable(int id, bool enable)
{
    auto item = DoFindMenuItem(id);
    if(item) {
        item->Enable(enable);
    }
    Refresh();
}

bool clMenuBar::IsEnabledTop(size_t pos) const
{
    if(pos >= m_menus.size()) {
        return false;
    }
    return m_menus[pos].is_enabled;
}

void clMenuBar::EnableTop(size_t pos, bool enable)
{
    if(pos >= m_menus.size()) {
        return;
    }
    m_menus[pos].is_enabled = enable;
    Refresh();
}

wxMenuItem* clMenuBar::FindItem(int id, wxMenu** menu) const { return DoFindMenuItem(id, menu); }

int clMenuBar::FindMenu(const wxString& title) const
{
    for(size_t i = 0; i < m_menus.size(); ++i) {
        if(m_menus[i].text == title) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

wxString clMenuBar::GetHelpString(int id) const
{
    auto item = DoFindMenuItem(id);
    if(item == nullptr) {
        return wxEmptyString;
    }
    return item->GetHelp();
}

wxString clMenuBar::GetLabel(int id) const
{
    auto item = DoFindMenuItem(id);
    if(item == nullptr) {
        return wxEmptyString;
    }
    return item->GetItemLabel();
}

wxString clMenuBar::GetLabelTop(size_t pos) const
{
    if(pos >= m_menus.size()) {
        return wxEmptyString;
    }
    return m_menus[pos].text;
}

wxMenu* clMenuBar::GetMenu(size_t menuIndex) const
{
    if(menuIndex >= m_menus.size()) {
        return nullptr;
    }
    return m_menus[menuIndex].menu;
}

size_t clMenuBar::GetMenuCount() const { return m_menus.size(); }

wxString clMenuBar::GetMenuLabel(size_t pos) const
{
    if(pos >= m_menus.size()) {
        return wxEmptyString;
    }
    return m_menus[pos].full_text;
}

wxString clMenuBar::GetMenuLabelText(size_t pos) const
{
    if(pos >= m_menus.size()) {
        return wxEmptyString;
    }
    return m_menus[pos].text;
}

bool clMenuBar::Insert(size_t pos, wxMenu* menu, const wxString& title)
{
    if(pos > m_menus.size()) {
        return false;
    }

    auto info = DoFindMenu(menu);
    if(info) {
        // already exist
        return false;
    }
    menu_info mi(title, menu, true);
    m_menus.insert(m_menus.begin() + pos, mi);
    Refresh();
    return true;
}

bool clMenuBar::IsChecked(int id) const
{
    auto mi = DoFindMenuItem(id);
    if(!mi) {
        return false;
    }
    return mi->IsChecked();
}

bool clMenuBar::IsEnabled(int id) const
{
    auto mi = DoFindMenuItem(id);
    if(!mi) {
        return false;
    }
    return mi->IsEnabled();
}

wxMenu* clMenuBar::Remove(size_t pos)
{
    if(pos >= m_menus.size()) {
        return nullptr;
    }
    auto mi = m_menus[pos];
    m_menus.erase(m_menus.begin() + pos);
    UpdateAccelerators();
    Refresh();
    return mi.menu;
}

wxMenu* clMenuBar::Replace(size_t pos, wxMenu* menu, const wxString& title)
{
    if(pos >= m_menus.size()) {
        return nullptr;
    }
    auto& mi = m_menus[pos];
    std::swap(mi.menu, menu);
    mi.set_label(title);
    Refresh();
    return menu;
}

void clMenuBar::SetHelpString(int id, const wxString& helpString)
{
    auto mi = DoFindMenuItem(id);
    if(!mi) {
        return;
    }
    mi->SetHelp(helpString);
}

void clMenuBar::SetLabel(int id, const wxString& label)
{
    auto mi = DoFindMenuItem(id);
    if(!mi) {
        return;
    }
    mi->SetItemLabel(label);
}

void clMenuBar::SetLabelTop(size_t pos, const wxString& label) { SetMenuLabel(pos, label); }

void clMenuBar::SetMenuLabel(size_t pos, const wxString& label)
{
    if(pos >= m_menus.size()) {
        return;
    }
    m_menus[pos].set_label(label);
    Refresh();
}

void clMenuBar::OnPaint(wxPaintEvent& e)
{
    wxUnusedVar(e);
    wxAutoBufferedPaintDC abdc(this);
    wxGCDC dc(abdc);
    PrepareDC(dc);

    wxRect rect = GetClientRect();
    rect.Inflate(1);
    dc.SetBrush(m_colours.GetBgColour());
    dc.SetPen(m_colours.GetBgColour());
    dc.DrawRectangle(rect);

    UpdateRects(dc);
    for(size_t i = 0; i < m_menus.size(); ++i) {
        size_t flags = k_normal;
        DrawButton(dc, i, flags);
    }
}

void clMenuBar::OnEraseBg(wxEraseEvent& e) { wxUnusedVar(e); }

void clMenuBar::SetColours(const clColours& colours)
{
    m_colours = colours;
    Refresh();
}

void clMenuBar::DoSetBestSize()
{
    wxClientDC dc(this);
    dc.SetFont(DrawingUtils::GetDefaultGuiFont());

    int buttonHeight = 0;
    {
        wxRect r = dc.GetTextExtent("Tp");
#if wxVERSION_NUMBER >= 3100
        r.Inflate(FromDIP(4));
#else
        r.Inflate(4);
#endif
        buttonHeight = r.GetHeight();
    }
    SetSizeHints(wxSize(-1, buttonHeight));
}

void clMenuBar::DrawButton(wxDC& dc, size_t index, size_t flags)
{
    wxUnusedVar(flags);
    auto& mi = m_menus[index];
    dc.SetTextForeground(m_colours.GetItemTextColour());
    if(mi.drawing_flags & k_disabled) {
        dc.SetTextForeground(m_colours.GetGrayText());
    } else if(mi.drawing_flags & k_pressed) {
        dc.SetBrush(m_colours.GetSelItemBgColour());
        dc.SetPen(m_colours.GetSelItemBgColour());
        dc.DrawRectangle(mi.m_rect);
        dc.SetTextForeground(m_colours.GetSelItemTextColour());
    } else if(mi.drawing_flags & k_hover) {
        dc.SetBrush(m_colours.GetSelItemBgColour());
        dc.SetPen(m_colours.GetSelItemBgColour());
        dc.DrawRectangle(mi.m_rect);
        dc.SetTextForeground(m_colours.GetSelItemTextColour());
    }
    dc.DrawLabel(mi.text, mi.m_text_rect, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_HORIZONTAL);
}

void clMenuBar::UpdateRects(wxDC& dc)
{
#if wxVERSION_NUMBER >= 3100
    int x_spacer = FromDIP(10);
#else
    int x_spacer = 10;
#endif
    int y_spacer = 0;
    int x = 0;
    wxRect client_rect = GetClientRect();
    for(auto& mi : m_menus) {
        wxSize sz = dc.GetTextExtent(mi.text);

        // update the button rect
        mi.m_rect.height = client_rect.GetHeight();
        mi.m_rect.x = x;
        mi.m_rect.width = x_spacer + sz.GetWidth() + x_spacer;
        mi.m_rect.y = 0;

        // update the text rect
        mi.m_text_rect.x = x_spacer;
        mi.m_text_rect.width = x_spacer + sz.GetWidth() + x_spacer;
        mi.m_text_rect.y = 0;
        mi.m_text_rect.height = y_spacer + sz.GetHeight() + y_spacer;
        mi.m_text_rect = mi.m_text_rect.CenterIn(mi.m_rect, wxBOTH);

        x += mi.m_text_rect.width;
    }
}

int clMenuBar::HitTest(const wxPoint& pt) const
{
    for(size_t i = 0; i < m_menus.size(); ++i) {
        if(m_menus[i].m_rect.Contains(pt)) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

bool clMenuBar::UpdateFlags(const wxPoint& pt)
{
    int index = HitTest(pt);
    bool left_is_down = wxGetMouseState().LeftIsDown();

    bool refresh_needed = false;
    for(size_t i = 0; i < m_menus.size(); ++i) {
        size_t orig_flags = m_menus[i].drawing_flags;
        m_menus[i].drawing_flags = 0;
        if(m_menus[i].is_enabled == false) {
            m_menus[i].drawing_flags |= k_disabled;
        } else if(i == (size_t)index) {
            m_menus[i].drawing_flags |= (left_is_down ? k_hover : k_pressed);
        }

        if(!refresh_needed && orig_flags != m_menus[i].drawing_flags) {
            refresh_needed = true;
        }
    }
    return refresh_needed;
}

void clMenuBar::OnLeftDown(wxMouseEvent& e)
{
    bool refresh_needed = UpdateFlags(e.GetPosition());
    if(refresh_needed) {
        Refresh();
    }
}

void clMenuBar::OnMotion(wxMouseEvent& e)
{
    bool refresh_needed = UpdateFlags(e.GetPosition());
    if(refresh_needed) {
        Refresh();
    }
}

void clMenuBar::OnEnterWindow(wxMouseEvent& e)
{
    if(UpdateFlags(e.GetPosition())) {
        Refresh();
    }
}

void clMenuBar::OnLeaveWindow(wxMouseEvent& e)
{
    wxUnusedVar(e);
    if(!m_menu_is_up) {
        UpdateFlags(wxPoint(-1, -1));
        Refresh();
    }
}

void clMenuBar::OnLeftUp(wxMouseEvent& e)
{
    UpdateFlags(e.GetPosition());
    int where = HitTest(e.GetPosition());
    if(where != wxNOT_FOUND) {
        auto& mi = m_menus[where];
        if(mi.menu) {
            m_menu_is_up = true;
            wxPoint pt = mi.m_rect.GetBottomLeft();
            pt.y += 1;
            PopupMenu(mi.menu, pt);
            m_menu_is_up = false;
            UpdateFlags(::wxGetMousePosition());
            Refresh();
        }
    }
}

void clMenuBar::UpdateAccelerators()
{
    std::vector<wxAcceleratorEntry*> accels;
    accels.reserve(100); // make enough room
    std::deque<wxMenu*> Q;
    for(const auto& mi : m_menus) {
        Q.push_back(mi.menu);
        while(!Q.empty()) {
            auto menu = Q.back();
            Q.pop_back();
            const auto& items = menu->GetMenuItems();
            for(auto menu_item : items) {
                if(menu_item->IsSubMenu()) {
                    Q.push_back(menu_item->GetSubMenu());
                } else {
                    wxAcceleratorEntry* accel = wxAcceleratorEntry::Create(menu_item->GetItemLabel());
                    if(accel && accel->IsOk()) {
                        accel->Set(accel->GetFlags(), accel->GetKeyCode(), menu_item->GetId());
                        accels.push_back(accel);
                    } else {
                        wxDELETE(accel);
                    }
                }
            }
        }
    }

    wxAcceleratorEntry entries[accels.size()];
    for(size_t i = 0; i < accels.size(); ++i) {
        entries[i] = *accels[i];
        wxDELETE(accels[i]);
    }

    wxAcceleratorTable table(accels.size(), entries);
    GetParent()->SetAcceleratorTable(table);
}

void clMenuBar::FromMenuBar(wxMenuBar* mb)
{
    while(mb->GetMenuCount()) {
        wxString label = mb->GetMenuLabel(0);
        wxMenu* menu = mb->Remove(0);
        DoAppend(menu, label);
    }

    if(mb->IsAttached()) {
        wxFrame* frame = mb->GetFrame();
        frame->SetMenuBar(nullptr);
        mb->Detach();
    }
    UpdateAccelerators();
    Refresh();
}
#endif
