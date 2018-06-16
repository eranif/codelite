#include "clToolBar.h"
#include "clToolBarButton.h"
#include "clToolBarButtonBase.h"
#include "clToolBarMenuButton.h"
#include "clToolBarSeparator.h"
#include "clToolBarToggleButton.h"
#include <algorithm>
#include <wx/dcbuffer.h>
#include <wx/dcmemory.h>
#include <wx/settings.h>
#include "globals.h"
#include "drawingutils.h"

static wxColour GetMenuBarColour() { return DrawingUtils::GetMenuBarBgColour(); }

clToolBar::clToolBar(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
                     const wxString& name)
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

    for(size_t i = 0; i < m_buttons.size(); ++i) {
        delete m_buttons[i];
    }
    m_buttons.clear();
}

void clToolBar::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC dc(this);
    wxRect clientRect = GetClientRect();
    DrawingUtils::FillMenuBarBgColour(dc, clientRect);
    int xx = 0;
    std::for_each(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* button) {
        wxSize buttonSize = button->CalculateSize(dc);
        wxRect r(xx, 0, buttonSize.GetWidth(), clientRect.GetHeight());
        button->Render(dc, r);
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
    for(size_t i = 0; i < m_buttons.size(); ++i) {
        clToolBarButtonBase* btn = m_buttons[i];
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
                    GetEventHandler()->AddPendingEvent(clicked);
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

void clToolBar::OnLeftDown(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    for(size_t i = 0; i < m_buttons.size(); ++i) {
        m_buttons[i]->ClearRenderFlags();
        if(m_buttons[i]->Contains(pos)) { m_buttons[i]->SetPressed(true); }
    }
    Refresh();
}

void clToolBar::OnMotion(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    bool refreshNeeded = false;
    for(size_t i = 0; i < m_buttons.size(); ++i) {
        if(m_buttons[i]->Contains(pos)) {
            if(!m_buttons[i]->IsHover()) {
                // a refresh is needed
                refreshNeeded = true;
            }
            m_buttons[i]->SetHover(true);
            SetToolTip(m_buttons[i]->GetLabel());
        } else {
            if(m_buttons[i]->IsHover()) {
                // a refresh is needed
                refreshNeeded = true;
            }
            m_buttons[i]->ClearRenderFlags();
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
    std::vector<clToolBarButtonBase*>::iterator iter =
        std::find_if(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == where); });
    if(iter == m_buttons.end()) { return NULL; }
    m_buttons.insert(iter, button);
    return button;
}

clToolBarButtonBase* clToolBar::InsertAfter(wxWindowID where, clToolBarButtonBase* button)
{
    std::vector<clToolBarButtonBase*>::iterator iter =
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
    std::vector<clToolBarButtonBase*>::const_iterator iter =
        std::find_if(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == id); });
    if(iter == m_buttons.end()) { return NULL; }
    return (*iter);
}

bool clToolBar::DeleteById(wxWindowID id)
{
    std::vector<clToolBarButtonBase*>::iterator iter =
        std::find_if(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == id); });
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
    for(size_t i = 0; i < m_buttons.size(); ++i) {
        wxUpdateUIEvent event(m_buttons[i]->GetId());
        if(GetEventHandler()->ProcessEvent(event)) {
            m_buttons[i]->Enable(event.GetEnabled());
            m_buttons[i]->Check(event.GetChecked());
        }
    }
    Refresh();
}
