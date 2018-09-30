#include "cLToolBarControl.h"
#include "clToolBar.h"
#include "clToolBarButton.h"
#include "clToolBarButtonBase.h"
#include "clToolBarMenuButton.h"
#include "clToolBarSeparator.h"
#include "clToolBarSpacer.h"
#include "clToolBarToggleButton.h"
#include "drawingutils.h"
#include <algorithm>
#include <wx/dcbuffer.h>
#include <wx/dcmemory.h>
#include <wx/log.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>

wxDEFINE_EVENT(wxEVT_TOOLBAR_CUSTOMISE, wxCommandEvent);
clToolBar::clToolBar(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
                     const wxString& name)
    : wxPanel(parent, winid, pos, size, style, name)
    , m_popupShown(false)
    , m_flags(0)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMiniToolBar(true);

    Bind(wxEVT_PAINT, &clToolBar::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &clToolBar::OnEraseBackground, this);
    Bind(wxEVT_LEFT_UP, &clToolBar::OnLeftUp, this);
    Bind(wxEVT_LEFT_DOWN, &clToolBar::OnLeftDown, this);
    Bind(wxEVT_MOTION, &clToolBar::OnMotion, this);
    Bind(wxEVT_ENTER_WINDOW, &clToolBar::OnEnterWindow, this);
    Bind(wxEVT_LEAVE_WINDOW, &clToolBar::OnLeaveWindow, this);
    Bind(wxEVT_SIZE, &clToolBar::OnSize, this);
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
    Unbind(wxEVT_SIZE, &clToolBar::OnSize, this);

    for(size_t i = 0; i < m_buttons.size(); ++i) {
        delete m_buttons[i];
    }
    m_buttons.clear();
}
#define CL_TOOL_BAR_CHEVRON_SIZE 16

void clToolBar::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    PrepareDC(dc);
    wxGCDC gcdc(dc);

    m_overflowButtons.clear();
    m_visibleButtons.clear();
    m_chevronRect = wxRect();

    wxRect clientRect = GetClientRect();
    DrawingUtils::FillMenuBarBgColour(gcdc, clientRect, HasFlag(kMiniToolBar));
    clientRect.SetWidth(clientRect.GetWidth() - CL_TOOL_BAR_CHEVRON_SIZE);
    DrawingUtils::FillMenuBarBgColour(gcdc, clientRect, HasFlag(kMiniToolBar));
    std::vector<ToolVect_t> groups;
    SplitGroups(groups);

    int xx = 0;
    for(size_t i = 0; i < groups.size(); ++i) {
        RenderGroup(xx, groups[i], gcdc);
        // Use a spacer of 10 pixels between groups
        xx += GetGroupSapcing();
    }

    wxRect chevronRect = GetClientRect();
    chevronRect.SetX(chevronRect.GetX() + (chevronRect.GetWidth() - CL_TOOL_BAR_CHEVRON_SIZE));
    chevronRect.SetWidth(CL_TOOL_BAR_CHEVRON_SIZE);

    // If we have overflow buttons, draw an arrow to the right
    if(!m_overflowButtons.empty() || IsCustomisationEnabled()) {
        DrawingUtils::DrawDropDownArrow(this, gcdc, chevronRect, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_chevronRect = chevronRect;
    }
}
void clToolBar::RenderGroup(int& xx, clToolBar::ToolVect_t& G, wxDC& gcdc)
{
    wxRect clientRect = GetClientRect();

    // Calculate the group size
    int groupWidth = 0;
    std::for_each(G.begin(), G.end(), [&](clToolBarButtonBase* button) {
        wxSize buttonSize = button->CalculateSize(gcdc);
        groupWidth += buttonSize.GetWidth();
    });

    // Draw a rectangle
#ifdef __WXMSW__
    bool hasGrouping = false;
#else
    bool hasGrouping = !HasFlag(clToolBar::kMiniToolBar);
#endif

    if(hasGrouping) {
        wxRect bgRect = wxRect(wxPoint(xx, 0), wxSize(groupWidth, clientRect.GetHeight() - 2));
        bool isLight = !DrawingUtils::IsDark(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR));
        wxColour bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR);
        if(isLight) {
            wxColour fill_colour = bgColour.ChangeLightness(150);
            wxColour dark_pen = bgColour.ChangeLightness(70);
            wxColour light_pen = *wxWHITE;
            bgRect.Deflate(1);
            bgRect = bgRect.CenterIn(clientRect, wxVERTICAL);

            gcdc.SetPen(light_pen);
            gcdc.SetBrush(fill_colour);
            bgRect.Offset(1, 1);
            gcdc.DrawRectangle(bgRect);

            bgRect.Offset(-1, -1);
            gcdc.SetBrush(fill_colour);
            gcdc.SetPen(dark_pen);
            gcdc.DrawRectangle(bgRect);

        } else {
            wxColour fill_colour = bgColour.ChangeLightness(110);
            wxColour dark_pen = bgColour.ChangeLightness(70);
            wxColour light_pen = bgColour.ChangeLightness(110);
            bgRect.Deflate(1);
            bgRect = bgRect.CenterIn(clientRect, wxVERTICAL);

            gcdc.SetPen(light_pen);
            gcdc.SetBrush(fill_colour);
            bgRect.Offset(1, 1);
            gcdc.DrawRectangle(bgRect);

            bgRect.Offset(-1, -1);
            gcdc.SetBrush(fill_colour);
            gcdc.SetPen(dark_pen);
            gcdc.DrawRectangle(bgRect);
        }
    }

    // Now draw the buttons
    std::for_each(G.begin(), G.end(), [&](clToolBarButtonBase* button) {
        wxSize buttonSize = button->CalculateSize(gcdc);
        if((xx + buttonSize.GetWidth()) >= clientRect.GetRight()) {
            if(button->IsControl()) {
                clToolBarControl* control = button->Cast<clToolBarControl>();
                control->GetControl()->Hide();
            }
            m_overflowButtons.push_back(button);
        } else {
            wxRect r(xx, 0, buttonSize.GetWidth(), clientRect.GetHeight());
            r.Deflate(1, 2);
            r = r.CenterIn(clientRect, wxVERTICAL);
            button->Render(gcdc, r);
            m_visibleButtons.push_back(button);
        }
        xx += buttonSize.GetWidth();
    });
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
    // Always assume that we need the extra space for the chevron button
    rect.width += CL_TOOL_BAR_CHEVRON_SIZE + 2;
    return rect;
}

void clToolBar::Realize()
{
    wxBitmap bmp(1, 1);
    wxMemoryDC dc(bmp);
    wxGCDC gcdc(dc);
    gcdc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    SetSizeHints(CalculateRect(gcdc).GetSize());
    Refresh();
}

void clToolBar::OnLeftUp(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    if(m_chevronRect.Contains(pos)) {
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
                    wxMenu* menu = btn->GetMenu();
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
                    btn->SetPressed(false);
                    Refresh();
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
        clToolBarButtonBase* button = m_visibleButtons[i];
        if(button->Contains(pos)) {
            if(!button->IsHover()) {
                // a refresh is needed
                refreshNeeded = true;
            }
            if(button->IsSeparator() || button->IsSpacer()) {
                // No tooltip for UI elements
                UnsetToolTip();
                ;
            } else {
                button->SetHover(true);
                if(button->GetLabel().IsEmpty()) {
                    UnsetToolTip();
                } else {
                    SetToolTip(button->GetLabel());
                }
            }
        } else {
            if(button->IsHover()) {
                // a refresh is needed
                refreshNeeded = true;
            }
            button->ClearRenderFlags();
        }
    }
    if(refreshNeeded) { Refresh(); }
}

void clToolBar::OnEnterWindow(wxMouseEvent& event) { OnMotion(event); }

void clToolBar::OnLeaveWindow(wxMouseEvent& event)
{
    if(!m_popupShown) {
        for(size_t i = 0; i < m_buttons.size(); ++i) {
            m_buttons[i]->ClearRenderFlags();
        }
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
    ToolVect_t::iterator iter =
        std::find_if(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == where); });
    if(iter == m_buttons.end()) { return NULL; }
    m_buttons.insert(iter, button);
    return button;
}

clToolBarButtonBase* clToolBar::InsertAfter(wxWindowID where, clToolBarButtonBase* button)
{
    ToolVect_t::iterator iter =
        std::find_if(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == where); });
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
    ToolVect_t::iterator iter = std::find_if(m_buttons.begin(), m_buttons.end(),
                                             [&](clToolBarButtonBase* b) { return (b->GetId() == buttonID); });
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
    ToolVect_t::const_iterator iter =
        std::find_if(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == id); });
    if(iter == m_buttons.end()) { return NULL; }
    return (*iter);
}

bool clToolBar::DeleteById(wxWindowID id)
{
    ToolVect_t::iterator iter =
        std::find_if(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == id); });
    if(iter == m_buttons.end()) { return false; }
    clToolBarButtonBase* button = (*iter);
    delete button;
    m_buttons.erase(iter);
    return true;
}

clToolBarButtonBase* clToolBar::AddSeparator() { return Add(new clToolBarSeparator(this)); }

clToolBarButtonBase* clToolBar::AddSpacer() { return Add(new clToolBarSpacer(this)); }

void clToolBar::SetDropdownMenu(wxWindowID buttonID, wxMenu* menu)
{
    clToolBarButtonBase* button = FindById(buttonID);
    if(!button) { return; }
    button->SetMenu(menu);
}

wxMenu* clToolBar::FindMenuById(wxWindowID buttonID) const
{
    clToolBarButtonBase* button = FindById(buttonID);
    if(!button) { return NULL; }
    return button->GetMenu();
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
    bool refreshNeeded = false;
    for(size_t i = 0; i < m_visibleButtons.size(); ++i) {
        clToolBarButtonBase* button = m_visibleButtons.at(i);
        wxUpdateUIEvent event(button->GetId());
        event.Enable(true);
        if(button->IsToggle()) { event.Check(button->IsChecked()); }
        if(GetEventHandler()->ProcessEvent(event)) {
            bool oldCheck = button->IsChecked();
            bool oldEnabled = button->IsEnabled();
            if(button->IsToggle()) { button->Check(event.GetChecked()); }
            button->Enable(event.GetEnabled());

            if(!refreshNeeded) {
                refreshNeeded = (oldCheck != button->IsChecked()) || (oldEnabled != button->IsEnabled());
            }
        }
    }
    if(refreshNeeded) { Refresh(); }
}

void clToolBar::DoShowOverflowMenu()
{
    // Show the drop down menu
    wxMenu menu;
    std::vector<int> checkedItems;
    bool last_was_separator = false;
    for(size_t i = 0; i < m_overflowButtons.size(); ++i) {
        clToolBarButtonBase* button = m_overflowButtons[i];
        if(button->IsSeparator() && !last_was_separator) {
            menu.AppendSeparator();
            last_was_separator = true;
        } else if(!button->IsControl() && !button->IsSpacer()) {
            // Show all non-control buttons
            wxMenuItem* menuItem = new wxMenuItem(&menu, button->GetId(), button->GetLabel(), button->GetLabel(),
                                                  button->IsToggle() ? wxITEM_CHECK : wxITEM_NORMAL);
            if(button->GetBmp().IsOk() && !button->IsToggle()) { menuItem->SetBitmap(button->GetBmp()); }
            if(button->IsToggle() && button->IsChecked()) { checkedItems.push_back(button->GetId()); }
            menu.Append(menuItem);
            menuItem->Enable(button->IsEnabled());
            if(button->IsEnabled()) {
                // Don't draw two separators one after the other
                last_was_separator = false;
            }
        }
    }
    if(IsCustomisationEnabled()) {
        if(menu.GetMenuItemCount() && !last_was_separator) { menu.AppendSeparator(); }
        menu.Append(XRCID("customise_toolbar"), _("Customise..."));
        menu.Bind(wxEVT_MENU,
                  [&](wxCommandEvent& event) {
                      wxCommandEvent evtCustomise(wxEVT_TOOLBAR_CUSTOMISE);
                      evtCustomise.SetEventObject(this);
                      GetEventHandler()->AddPendingEvent(evtCustomise);
                  },
                  XRCID("customise_toolbar"));
    }
    // Show the menu
    m_popupShown = true;
    wxPoint menuPos = m_chevronRect.GetBottomLeft();
#ifdef __WXOSX__
    menuPos.y += 5;
#endif
    for(size_t i = 0; i < checkedItems.size(); ++i) {
        menu.Check(checkedItems[i], true);
    }
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
    Refresh();
    event.Skip();
}

clToolBarButtonBase* clToolBar::AddControl(wxWindow* control) { return Add(new clToolBarControl(this, control)); }

int clToolBar::GetMenuSelectionFromUser(wxWindowID buttonID, wxMenu* menu)
{
    ToolVect_t::iterator iter = std::find_if(m_buttons.begin(), m_buttons.end(),
                                             [&](clToolBarButtonBase* b) { return (b->GetId() == buttonID); });
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

void clToolBar::SplitGroups(std::vector<ToolVect_t>& G)
{
    G.clear();
    ToolVect_t curG;
    for(size_t i = 0; i < m_buttons.size(); ++i) {
        clToolBarButtonBase* button = m_buttons[i];
        if(button->IsHidden()) { continue; }
        if(button->IsSpacer() || button->IsSeparator()) {
            // close this group and start a new one
            if(!curG.empty()) {
                G.push_back(curG);
                curG.clear();
            }
            continue;
        } else {
            curG.push_back(button);
        }
    }

    if(!curG.empty()) { G.push_back(curG); }
}
