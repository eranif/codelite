#include "cLToolBarControl.h"
#include "clToolBar.h"
#include "clToolBarButton.h"
#include "clToolBarButtonBase.h"
#include "clToolBarMenuButton.h"
#include "clToolBarSeparator.h"
#include "clToolBarToggleButton.h"
#include <algorithm>
#include <wx/dcbuffer.h>
#include <wx/dcmemory.h>
#include <wx/renderer.h>
#include <wx/settings.h>

clToolBar::clToolBar(
    wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxPanel(parent, winid, pos, size, style, name)
    , m_popupShown(false)
    , m_flags(0)
{
    Bind(wxEVT_PAINT, &clToolBar::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &clToolBar::OnEraseBackground, this);
    Bind(wxEVT_LEFT_UP, &clToolBar::OnLeftUp, this);
    Bind(wxEVT_LEFT_DOWN, &clToolBar::OnLeftDown, this);
    Bind(wxEVT_MOTION, &clToolBar::OnMotion, this);
    Bind(wxEVT_ENTER_WINDOW, &clToolBar::OnEnterWindow, this);
    Bind(wxEVT_LEAVE_WINDOW, &clToolBar::OnLeaveWindow, this);
}

clToolBar::~clToolBar()
{
    Unbind(wxEVT_PAINT, &clToolBar::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, &clToolBar::OnEraseBackground, this);
    Unbind(wxEVT_LEFT_UP, &clToolBar::OnLeftUp, this);
    Unbind(wxEVT_MOTION, &clToolBar::OnMotion, this);
    Unbind(wxEVT_ENTER_WINDOW, &clToolBar::OnEnterWindow, this);
    Unbind(wxEVT_LEAVE_WINDOW, &clToolBar::OnLeaveWindow, this);
    Unbind(wxEVT_LEFT_DOWN, &clToolBar::OnLeftDown, this);

    for(size_t i = 0; i < m_buttons.size(); ++i) { delete m_buttons[i]; }
    m_buttons.clear();
}
#define CL_TOOL_BAR_CHEVRON_SIZE 16

void clToolBar::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC dc(this);
    m_overflowButtons.clear();
    m_visibleButtons.clear();
    m_chevronRect = wxRect();

    wxRect clientRect = GetClientRect();
    clToolBarButtonBase::FillMenuBarBgColour(dc, clientRect);
    clientRect.SetWidth(clientRect.GetWidth() - CL_TOOL_BAR_CHEVRON_SIZE);
    clToolBarButtonBase::FillMenuBarBgColour(dc, clientRect);
    int xx = 0;
    std::for_each(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* button) {
        wxSize buttonSize = button->CalculateSize(dc);
        if((xx + buttonSize.GetWidth()) >= clientRect.GetRight()) {
            if(button->IsControl()) {
                clToolBarControl* control = button->Cast<clToolBarControl>();
                control->GetControl()->Hide();
            }
            m_overflowButtons.push_back(button);
        } else {
            wxRect r(xx, 0, buttonSize.GetWidth(), clientRect.GetHeight());
            button->Render(dc, r);
            m_visibleButtons.push_back(button);
        }
        xx += buttonSize.GetWidth();
    });

    wxRect chevronRect = GetClientRect();
    chevronRect.SetX(chevronRect.GetX() + (chevronRect.GetWidth() - CL_TOOL_BAR_CHEVRON_SIZE));
    chevronRect.SetWidth(CL_TOOL_BAR_CHEVRON_SIZE);

    // If we have overflow buttons, draw an arrow to the right
    if(!m_overflowButtons.empty()) {
        wxRendererNative::Get().DrawDropArrow(this, dc, chevronRect, wxCONTROL_CURRENT);
        m_chevronRect = chevronRect;
    }
}

void clToolBar::OnEraseBackground(wxEraseEvent& event) { wxUnusedVar(event); }

wxRect clToolBar::CalculateRect(wxDC& dc) const
{
    wxRect rect;
    std::for_each(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* button) {
        wxSize buttonSize = button->CalculateSize(dc);
        rect.width += buttonSize.GetWidth();
        rect.height = wxMax(rect.GetHeight(), buttonSize.GetHeight());
    });
    return rect;
}

void clToolBar::Realize()
{
    wxBitmap bmp(1, 1);
    wxMemoryDC dc(bmp);
    dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    SetSizeHints(CalculateRect(dc).GetSize());
    Refresh();
}

void clToolBar::OnLeftUp(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    if(!m_overflowButtons.empty() && m_chevronRect.Contains(pos)) {
        DoShowOverflowMenu();
    } else {
        for(size_t i = 0; i < m_visibleButtons.size(); ++i) {
            clToolBarButtonBase* btn = m_visibleButtons[i];
            if(btn->Contains(pos)) {
                if(!btn->IsEnabled()) {
                    // events are not fired for disabled buttons
                    return;
                }
                if(btn->IsToggle()) {
                    // Change the button state
                    btn->Check(!btn->IsChecked());
                    // Fire an event with proper IsChecked() set
                    wxCommandEvent clicked(wxEVT_TOOL, btn->GetId());
                    clicked.SetEventObject(this);
                    clicked.SetInt(btn->IsChecked() ? 1 : 0);
                    GetEventHandler()->AddPendingEvent(clicked);
                } else if(btn->InsideMenuButton(pos)) {
                    wxMenu* menu = FindMenuById(btn->GetId());
                    if(menu) {
                        // We got the menu, show it
                        ShowMenuForButton(btn->GetId(), menu);
                    } else {
                        wxCommandEvent clicked(wxEVT_TOOL_DROPDOWN, btn->GetId());
                        clicked.SetEventObject(this);
                        GetEventHandler()->ProcessEvent(clicked);
                    }
                } else {
                    wxCommandEvent clicked(wxEVT_TOOL, btn->GetId());
                    clicked.SetEventObject(this);
                    GetEventHandler()->AddPendingEvent(clicked);
                }
                break;
            }
        }
    }
}

void clToolBar::OnLeftDown(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    for(size_t i = 0; i < m_visibleButtons.size(); ++i) {
        m_visibleButtons[i]->ClearRenderFlags();
        if(m_visibleButtons[i]->Contains(pos)) { m_visibleButtons[i]->SetPressed(true); }
    }
    Refresh();
}

void clToolBar::OnMotion(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    bool refreshNeeded = false;
    for(size_t i = 0; i < m_visibleButtons.size(); ++i) {
        if(m_visibleButtons[i]->Contains(pos)) {
            if(!m_visibleButtons[i]->IsHover()) {
                // a refresh is needed
                refreshNeeded = true;
            }
            m_visibleButtons[i]->SetHover(true);
            SetToolTip(m_visibleButtons[i]->GetLabel());
        } else {
            if(m_visibleButtons[i]->IsHover()) {
                // a refresh is needed
                refreshNeeded = true;
            }
            m_visibleButtons[i]->ClearRenderFlags();
        }
    }
    if(refreshNeeded) { Refresh(); }
}

void clToolBar::OnEnterWindow(wxMouseEvent& event) { OnMotion(event); }

void clToolBar::OnLeaveWindow(wxMouseEvent& event)
{
    if(!m_popupShown) {
        for(size_t i = 0; i < m_buttons.size(); ++i) { m_buttons[i]->ClearRenderFlags(); }
        Refresh();
    }
}

clToolBarButtonBase* clToolBar::AddButton(wxWindowID id, const wxBitmap& bmp, const wxString& label)
{
    clToolBarButtonBase* button = new clToolBarButton(this, id, bmp, label);
    return Add(button);
}

clToolBarButtonBase* clToolBar::AddMenuButton(wxWindowID id, const wxBitmap& bmp, const wxString& label)
{
    clToolBarButtonBase* button = new clToolBarMenuButton(this, id, bmp, label);
    return Add(button);
}

clToolBarButtonBase* clToolBar::AddToggleButton(wxWindowID id, const wxBitmap& bmp, const wxString& label)
{
    clToolBarButtonBase* button = new clToolBarToggleButton(this, id, bmp, label);
    return Add(button);
}

clToolBarButtonBase* clToolBar::InsertBefore(wxWindowID where, clToolBarButtonBase* button)
{
    std::vector<clToolBarButtonBase*>::iterator iter = std::find_if(
        m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == where); });
    if(iter == m_buttons.end()) { return NULL; }
    m_buttons.insert(iter, button);
    return button;
}

clToolBarButtonBase* clToolBar::InsertAfter(wxWindowID where, clToolBarButtonBase* button)
{
    std::vector<clToolBarButtonBase*>::iterator iter = std::find_if(
        m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == where); });
    if(iter == m_buttons.end()) { return NULL; }
    ++iter; // can be end()
    m_buttons.insert(iter, button);
    return button;
}

clToolBarButtonBase* clToolBar::Add(clToolBarButtonBase* button)
{
    m_buttons.push_back(button);
    return button;
}

void clToolBar::ShowMenuForButton(wxWindowID buttonID, wxMenu* menu)
{
    std::vector<clToolBarButtonBase*>::iterator iter = std::find_if(
        m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == buttonID); });
    if(iter == m_buttons.end()) { return; }
    clToolBarButtonBase* button = *iter;
    m_popupShown = true;
    wxPoint menuPos = button->GetButtonRect().GetBottomLeft();
#ifdef __WXOSX__
    menuPos.y += 5;
#endif

    PopupMenu(menu, menuPos);
    m_popupShown = false;

    wxPoint pt = ::wxGetMousePosition();
    pt = ScreenToClient(pt);
    if(!GetClientRect().Contains(pt)) {
        wxMouseEvent dummy;
        OnLeaveWindow(dummy);
    }
}

clToolBarButtonBase* clToolBar::FindById(wxWindowID id) const
{
    std::vector<clToolBarButtonBase*>::const_iterator iter
        = std::find_if(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == id); });
    if(iter == m_buttons.end()) { return NULL; }
    return (*iter);
}

bool clToolBar::DeleteById(wxWindowID id)
{
    std::vector<clToolBarButtonBase*>::iterator iter
        = std::find_if(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == id); });
    if(iter == m_buttons.end()) { return false; }
    clToolBarButtonBase* button = (*iter);
    delete button;
    m_buttons.erase(iter);
    return true;
}

clToolBarButtonBase* clToolBar::AddSeparator() { return Add(new clToolBarSeparator(this)); }

void clToolBar::SetDropdownMenu(wxWindowID buttonID, wxMenu* menu)
{
    if(m_menus.count(buttonID)) {
        // we already have a menu here, delete it
        delete m_menus[buttonID];
    }
    m_menus[buttonID] = menu;
}

wxMenu* clToolBar::FindMenuById(wxWindowID buttonID) const
{
    std::unordered_map<int, wxMenu*>::const_iterator iter = m_menus.find(buttonID);
    return (iter == m_menus.end() ? NULL : (iter->second));
}

void clToolBar::ToggleTool(wxWindowID buttonID, bool toggle)
{
    clToolBarButtonBase* button = FindById(buttonID);
    if(button) { button->Check(toggle); }
}

void clToolBar::UpdateWindowUI(long flags)
{
    // Call update UI event per button
    if(flags & wxUPDATE_UI_FROMIDLE) { DoIdleUpdate(); }

    wxPanel::UpdateWindowUI(flags);
}

void clToolBar::DoIdleUpdate()
{
    for(size_t i = 0; i < m_visibleButtons.size(); ++i) {
        wxUpdateUIEvent event(m_visibleButtons[i]->GetId());
        event.Enable(true);
        if(GetEventHandler()->ProcessEvent(event)) {
            m_visibleButtons[i]->Check(event.GetChecked());
            m_visibleButtons[i]->Enable(event.GetEnabled());
        }
    }
    Refresh();
}

void clToolBar::DoShowOverflowMenu()
{
    // Show the drop down menu
    wxMenu menu;
    std::vector<int> checkedItems;
    for(size_t i = 0; i < m_overflowButtons.size(); ++i) {
        clToolBarButtonBase* button = m_overflowButtons[i];
        if(button->IsSeparator()) {
            menu.AppendSeparator();
        } else if(!button->IsControl()) {
            // Show all non-control buttons
            wxMenuItem* menuItem = new wxMenuItem(&menu, button->GetId(), button->GetLabel(), button->GetLabel(),
                button->IsToggle() ? wxITEM_CHECK : wxITEM_NORMAL);
            if(button->GetBmp().IsOk()) { menuItem->SetBitmap(button->GetBmp()); }
            if(button->IsToggle() && button->IsChecked()) { checkedItems.push_back(button->GetId()); }
            menuItem->Enable(button->IsEnabled());
            menu.Append(menuItem);
        }
    }

    // Show the menu
    m_popupShown = true;
    wxPoint menuPos = m_chevronRect.GetBottomLeft();
#ifdef __WXOSX__
    menuPos.y += 5;
#endif
    for(size_t i = 0; i < checkedItems.size(); ++i) { menu.Check(checkedItems[i], true); }
    menu.Bind(wxEVT_MENU, &clToolBar::OnOverflowItem, this, wxID_ANY);
    PopupMenu(&menu, menuPos);
    menu.Unbind(wxEVT_MENU, &clToolBar::OnOverflowItem, this, wxID_ANY);
    m_popupShown = false;

    wxPoint pt = ::wxGetMousePosition();
    pt = ScreenToClient(pt);
    if(!GetClientRect().Contains(pt)) {
        wxMouseEvent dummy;
        OnLeaveWindow(dummy);
    }
}

void clToolBar::OnOverflowItem(wxCommandEvent& event)
{
    // Update our button
    clToolBarButtonBase* button = FindById(event.GetId());
    if(button && button->IsToggle()) { button->Check(event.IsChecked()); }

    // Call the default action
    event.Skip();
}

void clToolBar::OnSize(wxSizeEvent& event)
{
    event.Skip();
    Refresh();
}

clToolBarButtonBase* clToolBar::AddControl(wxWindow* control) { return Add(new clToolBarControl(this, control)); }

int clToolBar::GetMenuSelectionFromUser(wxWindowID buttonID, wxMenu* menu)
{
    std::vector<clToolBarButtonBase*>::iterator iter = std::find_if(
        m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == buttonID); });
    if(iter == m_buttons.end()) { return wxID_NONE; }
    clToolBarButtonBase* button = *iter;
    m_popupShown = true;
    wxPoint menuPos = button->GetButtonRect().GetBottomLeft();
#ifdef __WXOSX__
    menuPos.y += 5;
#endif

    int selection = GetPopupMenuSelectionFromUser(*menu, menuPos);
    m_popupShown = false;

    wxPoint pt = ::wxGetMousePosition();
    pt = ScreenToClient(pt);
    if(!GetClientRect().Contains(pt)) {
        wxMouseEvent dummy;
        OnLeaveWindow(dummy);
    }
    return selection;
}
