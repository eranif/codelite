#include "clToolBar.h"

#include "cLToolBarControl.h"
#include "clSystemSettings.h"
#include "clToolBarButton.h"
#include "clToolBarButtonBase.h"
#include "clToolBarMenuButton.h"
#include "clToolBarSeparator.h"
#include "clToolBarSpacer.h"
#include "clToolBarStretchableSpace.h"
#include "clToolBarToggleButton.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "globals.h"

#include <algorithm>
#include <wx/bmpbndl.h>
#include <wx/dcbuffer.h>
#include <wx/dcmemory.h>
#include <wx/log.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>

#if wxUSE_NATIVE_TOOLBAR
clToolBarNative::clToolBarNative(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
                                 const wxString& name)
    : wxToolBar(parent, winid, pos, size, style, name)
{
}

clToolBarNative::~clToolBarNative() {}

wxToolBarToolBase* clToolBarNative::AddTool(wxWindowID id, const wxString& label, size_t bitmapIndex,
                                            const wxString& helpString, wxItemKind kind)
{
    if(m_bitmaps && bitmapIndex < m_bitmaps->size()) {
        auto normal_bmp = m_bitmaps->Get(bitmapIndex, false);
        auto disabled_bmp = m_bitmaps->Get(bitmapIndex, true);
        return wxToolBar::AddTool(id, label, normal_bmp, disabled_bmp, kind);
    } else {
        return wxToolBar::AddTool(id, label, wxNullBitmap, wxNullBitmap, kind);
    }
}

int clToolBarNative::GetMenuSelectionFromUser(wxWindowID buttonID, wxMenu* menu)
{
    wxUnusedVar(buttonID);
    return wxWindow::GetPopupMenuSelectionFromUser(*menu);
}

const wxBitmap& clToolBarNative::GetBitmap(size_t index) const
{
    wxASSERT_MSG(m_bitmaps, "No bitmaps !?");
    return m_bitmaps->Get(index, false);
}

void clToolBarNative::SetBitmaps(clBitmapList* bitmaps)
{
    if(m_bitmaps && m_ownedBitmaps) {
        wxDELETE(m_bitmaps);
    }
    m_ownedBitmaps = false;
    m_bitmaps = bitmaps;
}

void clToolBarNative::AssignBitmaps(clBitmapList* bitmaps)
{
    if(m_bitmaps && m_ownedBitmaps) {
        wxDELETE(m_bitmaps);
    }
    m_ownedBitmaps = true;
    m_bitmaps = bitmaps;
}

clBitmapList* clToolBarNative::GetBitmapsCreateIfNeeded()
{
    if(m_bitmaps) {
        return m_bitmaps;
    }
    m_ownedBitmaps = true;
    m_bitmaps = new clBitmapList;
    return m_bitmaps;
}

void clToolBarNative::ShowMenuForButton(wxWindowID buttonID, wxMenu* menu)
{
    // sanity
    auto tool = FindById(buttonID);
    CHECK_PTR_RET(tool);

    // popup menu
    PopupMenu(menu);
}
#endif // wxUSE_NATIVE_TOOLBAR

wxDEFINE_EVENT(wxEVT_TOOLBAR_CUSTOMISE, wxCommandEvent);
clToolBarGeneric::clToolBarGeneric(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size,
                                   long style, const wxString& name)
    : wxPanel(parent, winid, pos, size, style, name)
    , m_popupShown(false)
    , m_flags(kMiniToolBar)
{
    SetGroupSpacing(30);
    m_bgColour = clSystemSettings::GetDefaultPanelColour();

    SetGroupSpacing(50);
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &clToolBarGeneric::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &clToolBarGeneric::OnEraseBackground, this);
    Bind(wxEVT_LEFT_UP, &clToolBarGeneric::OnLeftUp, this);
    Bind(wxEVT_LEFT_DOWN, &clToolBarGeneric::OnLeftDown, this);
    Bind(wxEVT_LEFT_DCLICK, &clToolBarGeneric::OnLeftDown, this);
    Bind(wxEVT_MOTION, &clToolBarGeneric::OnMotion, this);
    Bind(wxEVT_ENTER_WINDOW, &clToolBarGeneric::OnEnterWindow, this);
    Bind(wxEVT_LEAVE_WINDOW, &clToolBarGeneric::OnLeaveWindow, this);
    Bind(wxEVT_SIZE, &clToolBarGeneric::OnSize, this);

    // to make sure that the toolbar does not get the focus, we restore the focus back to the previous window
    Bind(wxEVT_SET_FOCUS, [](wxFocusEvent& event) {
        event.Skip();
        wxWindow* oldFocus = event.GetWindow();
        if(oldFocus) {
            oldFocus->CallAfter(&wxWindow::SetFocus);
        }
    });

    m_bgColour = clSystemSettings::GetDefaultPanelColour();
    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &clToolBarGeneric::OnColoursChanged, this);
}

clToolBarGeneric::~clToolBarGeneric()
{
    EventNotifier::Get()->Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &clToolBarGeneric::OnColoursChanged, this);
    Unbind(wxEVT_PAINT, &clToolBarGeneric::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, &clToolBarGeneric::OnEraseBackground, this);
    Unbind(wxEVT_LEFT_UP, &clToolBarGeneric::OnLeftUp, this);
    Unbind(wxEVT_MOTION, &clToolBarGeneric::OnMotion, this);
    Unbind(wxEVT_ENTER_WINDOW, &clToolBarGeneric::OnEnterWindow, this);
    Unbind(wxEVT_LEAVE_WINDOW, &clToolBarGeneric::OnLeaveWindow, this);
    Unbind(wxEVT_LEFT_DOWN, &clToolBarGeneric::OnLeftDown, this);
    Unbind(wxEVT_LEFT_DCLICK, &clToolBarGeneric::OnLeftDown, this);
    Unbind(wxEVT_SIZE, &clToolBarGeneric::OnSize, this);

    for(size_t i = 0; i < m_buttons.size(); ++i) {
        delete m_buttons[i];
    }
    m_buttons.clear();
    if(m_bitmaps && m_ownedBitmaps) {
        wxDELETE(m_bitmaps);
    }
}

#define CL_TOOL_BAR_CHEVRON_SIZE 16

void clToolBarGeneric::OnPaint(wxPaintEvent& event)
{
    wxUnusedVar(event);
    wxGCDC gcdc;
    wxPaintDC dc(this);
    DrawingUtils::GetGCDC(dc, gcdc);
    PrepareDC(gcdc);

    m_overflowButtons.clear();
    m_visibleButtons.clear();
    m_chevronRect = wxRect();

    wxRect clientRect = GetClientRect();
#ifdef __WXOSX__
    clientRect.Inflate(1);
#endif

    wxColour tbBgColour;
    DrawingUtils::FillMenuBarBgColour(gcdc, clientRect, HasFlag(kMiniToolBar));
    clientRect.SetWidth(clientRect.GetWidth() - CL_TOOL_BAR_CHEVRON_SIZE);
    DrawingUtils::FillMenuBarBgColour(gcdc, clientRect, HasFlag(kMiniToolBar));
    tbBgColour = DrawingUtils::GetMenuBarBgColour(HasFlag(kMiniToolBar));

    // Prepare for drawings
    std::vector<ToolVect_t> groups;
    PrepareForDrawings(gcdc, groups, clientRect);

    int xx = 0;
    for(size_t i = 0; i < groups.size(); ++i) {
        RenderGroup(xx, groups[i], gcdc, (i == (groups.size() - 1)));
        // Use a spacer of 10 pixels between groups
        xx += GetGroupSpacing();
    }

    wxRect chevronRect = GetClientRect();
    chevronRect.SetX(chevronRect.GetX() + (chevronRect.GetWidth() - CL_TOOL_BAR_CHEVRON_SIZE));
    chevronRect.SetWidth(CL_TOOL_BAR_CHEVRON_SIZE);

    // If we have overflow buttons, draw an arrow to the right
    if(!m_overflowButtons.empty() || IsCustomisationEnabled()) {
        DrawingUtils::DrawDropDownArrow(this, gcdc, chevronRect);
        m_chevronRect = chevronRect;
    }

    if(!(m_windowStyle & wxTB_NODIVIDER)) {
        tbBgColour = tbBgColour.ChangeLightness(70);
        gcdc.SetPen(tbBgColour);
        gcdc.DrawLine(GetClientRect().GetLeftBottom(), GetClientRect().GetRightBottom());
    }
}

void clToolBarGeneric::RenderGroup(int& xx, const clToolBarGeneric::ToolVect_t& G, wxDC& gcdc, bool isLastGroup)
{
    wxUnusedVar(isLastGroup);
    wxRect clientRect = GetClientRect();

    // Calculate the group size
    int groupWidth = 0;
    std::for_each(G.begin(), G.end(), [&](clToolBarButtonBase* button) {
        wxSize buttonSize = button->CalculateSize(gcdc);
        groupWidth += buttonSize.GetWidth();
    });

    // Now draw the buttons
    std::for_each(G.begin(), G.end(), [&](clToolBarButtonBase* button) {
        wxSize buttonSize = button->CalculateSize(gcdc);
        if((xx + buttonSize.GetWidth()) > clientRect.GetRight()) {
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

void clToolBarGeneric::OnEraseBackground(wxEraseEvent& event) { wxUnusedVar(event); }

wxRect clToolBarGeneric::CalculateRect(wxDC& dc) const
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

void clToolBarGeneric::Realize()
{
    wxBitmap bmp;
    bmp.CreateWithDIPSize(wxSize(1, 1), GetDPIScaleFactor());
    wxMemoryDC dc(bmp);
    wxGCDC gcdc(dc);
    gcdc.SetFont(DrawingUtils::GetDefaultGuiFont());
    SetSizeHints(CalculateRect(gcdc).GetSize());
    Refresh();
}

void clToolBarGeneric::OnLeftUp(wxMouseEvent& event)
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

                    // The hover state is the opposite to the check status
                    btn->SetHover(!btn->IsChecked());

                    // Fire an event with proper IsChecked() set
                    wxCommandEvent clicked(wxEVT_TOOL, btn->GetId());
                    clicked.SetEventObject(this);
                    clicked.SetInt(btn->IsChecked() ? 1 : 0);
                    GetEventHandler()->AddPendingEvent(clicked);
                    Refresh();

                } else if(btn->InsideMenuButton(pos)) {
                    wxMenu* menu = btn->GetMenu();
                    if(menu) {
// We got the menu, show it
#ifdef __WXOSX__
                        CallAfter(&clToolBarGeneric::ShowMenuForButton, btn->GetId(), menu);
#else
                        ShowMenuForButton(btn->GetId(), menu);
#endif
                    } else {
                        wxCommandEvent clicked(wxEVT_TOOL_DROPDOWN, btn->GetId());
                        clicked.SetEventObject(this);
#ifdef __WXOSX__
                        GetEventHandler()->AddPendingEvent(clicked);
#else
                        GetEventHandler()->ProcessEvent(clicked);
#endif
                    }
                    btn->SetPressed(false);
                    Refresh();
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

void clToolBarGeneric::OnLeftDown(wxMouseEvent& event)
{
    event.Skip();
    wxPoint pos = event.GetPosition();
    for(size_t i = 0; i < m_visibleButtons.size(); ++i) {
        m_visibleButtons[i]->ClearRenderFlags();
        if(m_visibleButtons[i]->Contains(pos)) {
            m_visibleButtons[i]->SetPressed(true);
        }
    }
    Refresh();
}

void clToolBarGeneric::OnMotion(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    bool refreshNeeded = false;
    bool tooltipValid = false;
    for(size_t i = 0; i < m_visibleButtons.size(); ++i) {
        clToolBarButtonBase* button = m_visibleButtons[i];
        if(button->Contains(pos)) {
            if(!button->IsHover()) {
                // a refresh is needed
                button->SetHover(true);
                refreshNeeded = true;
            }
            if(button->IsSeparator() || button->IsSpacer()) {
                // No tooltip for UI elements
                UnsetToolTip();
            } else {
                // button->SetHover(true);
                tooltipValid = true;
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
            // Clear the hover flag
            button->SetHover(false);
        }
    }
    if(!tooltipValid) {
        UnsetToolTip();
    }
    if(refreshNeeded) {
        Refresh();
    }
}

void clToolBarGeneric::OnEnterWindow(wxMouseEvent& event)
{
    wxUnusedVar(event);
    OnMotion(event);
}

void clToolBarGeneric::OnLeaveWindow(wxMouseEvent& event)
{
    wxUnusedVar(event);
    if(!m_popupShown) {
        for(size_t i = 0; i < m_buttons.size(); ++i) {
            m_buttons[i]->ClearRenderFlags();
        }
        Refresh();
    }
}

clToolBarButtonBase* clToolBarGeneric::AddButton(wxWindowID id, size_t bitmapIndex, const wxString& label)
{
    clToolBarButtonBase* button = new clToolBarButton(this, id, bitmapIndex, label);
    return Add(button);
}

clToolBarButtonBase* clToolBarGeneric::AddMenuButton(wxWindowID id, size_t bitmapIndex, const wxString& label)
{
    clToolBarButtonBase* button = new clToolBarMenuButton(this, id, bitmapIndex, label);
    return Add(button);
}

clToolBarButtonBase* clToolBarGeneric::AddToggleButton(wxWindowID id, size_t bitmapIndex, const wxString& label)
{
    clToolBarButtonBase* button = new clToolBarToggleButton(this, id, bitmapIndex, label);
    return Add(button);
}

clToolBarButtonBase* clToolBarGeneric::InsertBefore(wxWindowID where, clToolBarButtonBase* button)
{
    ToolVect_t::iterator iter =
        std::find_if(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == where); });
    if(iter == m_buttons.end()) {
        return NULL;
    }
    m_buttons.insert(iter, button);
    return button;
}

clToolBarButtonBase* clToolBarGeneric::InsertAfter(wxWindowID where, clToolBarButtonBase* button)
{
    ToolVect_t::iterator iter =
        std::find_if(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == where); });
    if(iter == m_buttons.end()) {
        return NULL;
    }
    ++iter; // can be end()
    m_buttons.insert(iter, button);
    return button;
}

clToolBarButtonBase* clToolBarGeneric::Add(clToolBarButtonBase* button)
{
    m_buttons.push_back(button);
    return button;
}

void clToolBarGeneric::ShowMenuForButton(wxWindowID buttonID, wxMenu* menu)
{
    ToolVect_t::iterator iter = std::find_if(m_buttons.begin(), m_buttons.end(),
                                             [&](clToolBarButtonBase* b) { return (b->GetId() == buttonID); });
    if(iter == m_buttons.end()) {
        return;
    }
    clToolBarButtonBase* button = *iter;
    m_popupShown = true;
    wxPoint menuPos = button->GetButtonRect().GetBottomLeft();
#ifdef __WXOSX__
    menuPos.y += 8;
    menuPos.x -= 2;
#else
    menuPos.y += 2;
    menuPos.x -= 1;
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

clToolBarButtonBase* clToolBarGeneric::FindById(wxWindowID id) const
{
    ToolVect_t::const_iterator iter =
        std::find_if(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == id); });
    if(iter == m_buttons.end()) {
        return NULL;
    }
    return (*iter);
}

bool clToolBarGeneric::DeleteById(wxWindowID id)
{
    ToolVect_t::iterator iter =
        std::find_if(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == id); });
    if(iter == m_buttons.end()) {
        return false;
    }
    clToolBarButtonBase* button = (*iter);
    delete button;
    m_buttons.erase(iter);
    return true;
}

clToolBarButtonBase* clToolBarGeneric::AddSeparator() { return Add(new clToolBarSeparator(this)); }

clToolBarButtonBase* clToolBarGeneric::AddSpacer() { return Add(new clToolBarSpacer(this)); }
clToolBarButtonBase* clToolBarGeneric::AddStretchableSpace() { return Add(new clToolBarStretchableSpace(this)); }

void clToolBarGeneric::SetDropdownMenu(wxWindowID buttonID, wxMenu* menu)
{
    clToolBarButtonBase* button = FindById(buttonID);
    if(!button) {
        return;
    }
    button->SetMenu(menu);
}

wxMenu* clToolBarGeneric::FindMenuById(wxWindowID buttonID) const
{
    clToolBarButtonBase* button = FindById(buttonID);
    if(!button) {
        return NULL;
    }
    return button->GetMenu();
}

void clToolBarGeneric::ToggleTool(wxWindowID buttonID, bool toggle)
{
    clToolBarButtonBase* button = FindById(buttonID);
    if(button) {
        button->Check(toggle);
    }
}

void clToolBarGeneric::UpdateWindowUI(long flags)
{
    // Call update UI event per button
    if(flags & wxUPDATE_UI_FROMIDLE) {
        DoIdleUpdate();
    }

    wxPanel::UpdateWindowUI(flags);
}

void clToolBarGeneric::DoIdleUpdate()
{
    bool refreshNeeded = false;
    for(size_t i = 0; i < m_visibleButtons.size(); ++i) {
        clToolBarButtonBase* button = m_visibleButtons.at(i);
        wxUpdateUIEvent event(button->GetId());
        event.Enable(true);
        if(button->IsToggle()) {
            event.Check(button->IsChecked());
        }
        if(GetEventHandler()->ProcessEvent(event)) {
            bool oldCheck = button->IsChecked();
            bool oldEnabled = button->IsEnabled();
            if(button->IsToggle()) {
                button->Check(event.GetChecked());
            }
            button->Enable(event.GetEnabled());

            if(!refreshNeeded) {
                refreshNeeded = (oldCheck != button->IsChecked()) || (oldEnabled != button->IsEnabled());
            }
        }
    }
    if(refreshNeeded) {
        Refresh();
    }
}

void clToolBarGeneric::DoShowOverflowMenu()
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

            if(button->HasBitmap() && !button->IsToggle()) {
                menuItem->SetBitmap(button->GetBitmap());
            }
            if(button->IsToggle() && button->IsChecked()) {
                checkedItems.push_back(button->GetId());
            }
            menu.Append(menuItem);
            menuItem->Enable(button->IsEnabled());
            if(button->IsEnabled()) {
                // Don't draw two separators one after the other
                last_was_separator = false;
            }
        }
    }
    if(IsCustomisationEnabled()) {
        if(menu.GetMenuItemCount() && !last_was_separator) {
            menu.AppendSeparator();
        }
        menu.Append(XRCID("customise_toolbar"), _("Customise..."));
        menu.Bind(
            wxEVT_MENU,
            [&](wxCommandEvent& event) {
                wxUnusedVar(event);
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
    menu.Bind(wxEVT_MENU, &clToolBarGeneric::OnOverflowItem, this, wxID_ANY);
    PopupMenu(&menu, menuPos);
    menu.Unbind(wxEVT_MENU, &clToolBarGeneric::OnOverflowItem, this, wxID_ANY);
    m_popupShown = false;

    wxPoint pt = ::wxGetMousePosition();
    pt = ScreenToClient(pt);
    if(!GetClientRect().Contains(pt)) {
        wxMouseEvent dummy;
        OnLeaveWindow(dummy);
    }
}

void clToolBarGeneric::OnOverflowItem(wxCommandEvent& event)
{
    // Update our button
    clToolBarButtonBase* button = FindById(event.GetId());
    if(button && button->IsToggle()) {
        button->Check(event.IsChecked());
    }

    // Call the default action
    event.Skip();
}

void clToolBarGeneric::OnSize(wxSizeEvent& event)
{
    Refresh();
    event.Skip();
}

clToolBarButtonBase* clToolBarGeneric::AddControl(wxWindow* control)
{
    return Add(new clToolBarControl(this, control));
}

int clToolBarGeneric::GetMenuSelectionFromUser(wxWindowID buttonID, wxMenu* menu)
{
    ToolVect_t::iterator iter = std::find_if(m_buttons.begin(), m_buttons.end(),
                                             [&](clToolBarButtonBase* b) { return (b->GetId() == buttonID); });
    if(iter == m_buttons.end()) {
        return wxID_NONE;
    }
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

void clToolBarGeneric::PrepareForDrawings(wxDC& dc, std::vector<ToolVect_t>& G, const wxRect& rect)
{
    G.clear();
    int totalWidth = 0;
    int stretchableButtons = 0;
    ToolVect_t curG;
    ToolVect_t spacers;
    for(size_t i = 0; i < m_buttons.size(); ++i) {
        clToolBarButtonBase* button = m_buttons[i];

        // Don't include stretchable buttons in the total width
        if(!button->IsStretchableSpace()) {
            totalWidth += button->CalculateSize(dc).GetWidth();
        }
        if(button->IsHidden()) {
            continue;
        }

        if(button->IsStretchableSpace()) {
            stretchableButtons++;
            spacers.push_back(button);

            // A stretchable space is a one-one-group
            if(!curG.empty()) {
                G.push_back(curG);
                curG.clear();
            }
            curG.push_back(button);
            G.push_back(curG);
            curG.clear();
            continue;
        } else if(button->IsSpacer() || button->IsSeparator()) {
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
    if(!curG.empty()) {
        G.push_back(curG);
    }

    // Set a size to each stretchable button
    if(!spacers.empty()) {
        int spacer_width = ((rect.GetWidth() - totalWidth - ((G.size() - 1) * GetGroupSpacing())) / spacers.size());
        for(clToolBarButtonBase* button : spacers) {
            button->Cast<clToolBarStretchableSpace>()->SetWidth(spacer_width < 0 ? 0 : spacer_width);
        }
    }
}

int clToolBarGeneric::GetXSpacer() const
{
    int spacer = HasFlag(kMiniToolBar) ? 3 : 8;
#if wxCHECK_VERSION(3, 1, 0)
    spacer = FromDIP(spacer);
#endif
    return spacer;
}

int clToolBarGeneric::GetYSpacer() const
{
    int spacer = HasFlag(kMiniToolBar) ? 6 : 10;
#if wxCHECK_VERSION(3, 1, 0)
    spacer = FromDIP(spacer);
#endif
    return spacer;
}

void clToolBarGeneric::OnColoursChanged(clCommandEvent& event)
{
    event.Skip();
    m_bgColour = clSystemSettings::GetDefaultPanelColour();
    Refresh();
}

void clToolBarGeneric::SetGroupSpacing(int spacing) { m_groupSpacing = clGetSize(spacing, this); }

const wxBitmap& clToolBarGeneric::GetBitmap(size_t index) const
{
    wxASSERT_MSG(m_bitmaps, "No bitmaps !?");
    return m_bitmaps->Get(index, false);
}

void clToolBarGeneric::SetBitmaps(clBitmapList* bitmaps)
{
    if(m_bitmaps && m_ownedBitmaps) {
        wxDELETE(m_bitmaps);
    }
    m_ownedBitmaps = false;
    m_bitmaps = bitmaps;
}

void clToolBarGeneric::AssignBitmaps(clBitmapList* bitmaps)
{
    if(m_bitmaps && m_ownedBitmaps) {
        wxDELETE(m_bitmaps);
    }
    m_ownedBitmaps = true;
    m_bitmaps = bitmaps;
}

clBitmapList* clToolBarGeneric::GetBitmapsCreateIfNeeded()
{
    if(m_bitmaps) {
        return m_bitmaps;
    }
    m_ownedBitmaps = true;
    m_bitmaps = new clBitmapList;
    return m_bitmaps;
}
