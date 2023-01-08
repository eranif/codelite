#include "clGenericNotebook.hpp"

#include "ColoursAndFontsManager.h"
#include "JSON.h"
#include "clColours.h"
#include "clSystemSettings.h"
#include "clTabRendererMinimal.hpp"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
#include "lexer_configuration.h"
#include "wxStringHash.h"

#include <algorithm>
#include <wx/app.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/dnd.h>
#include <wx/image.h>
#include <wx/menu.h>
#include <wx/regex.h>
#include <wx/sizer.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/xrc/xmlres.h>

namespace
{
struct DnD_Data {
    clTabCtrl* srcTabCtrl = nullptr;
    int tabIndex;

    DnD_Data()
        : srcTabCtrl(NULL)
        , tabIndex(wxNOT_FOUND)
    {
    }
    void Clear()
    {
        srcTabCtrl = NULL;
        tabIndex = wxNOT_FOUND;
    }
};

DnD_Data s_clTabCtrlDnD_Data;
const wxString BUTTON_FILE_LIST_SYMBOL = wxT("\u22EF");

wxRect GetFileListButtonRect(wxWindow* win, size_t style, wxDC& dc)
{
    wxDCFontChanger font_changer(dc);
    wxFont font = clTabRenderer::GetTabFont(true);
    dc.SetFont(font);

    wxRect rr = dc.GetTextExtent(BUTTON_FILE_LIST_SYMBOL);
    rr.Inflate(win->FromDIP(5));

    int max_dim = wxMax(rr.GetWidth(), rr.GetHeight());
    rr.SetWidth(max_dim);
    rr.SetHeight(max_dim);

    wxRect client_rect = win->GetClientRect();
    rr.SetX(client_rect.GetX() + client_rect.GetWidth() - rr.GetWidth());
    rr = rr.CenterIn(client_rect, wxVERTICAL);
    return rr;
}

} // namespace

void clGenericNotebook::PositionControls()
{
    size_t style = GetStyle();

    // Detach the controls from the main sizer
    if(GetSizer()) {
        GetSizer()->Detach(m_windows);
        GetSizer()->Detach(m_tabCtrl);
    }

    // Set new sizer
    if(style & kNotebook_BottomTabs) {
        // Tabs are placed under the pages
        SetSizer(new wxBoxSizer(wxVERTICAL));
        GetSizer()->Add(m_windows, 1, wxEXPAND);
        GetSizer()->Add(m_tabCtrl, 0, wxEXPAND);
    } else {
        // Tabs are placed on top of the pages
        SetSizer(new wxBoxSizer(wxVERTICAL));
        GetSizer()->Add(m_tabCtrl, 0, wxEXPAND);
        GetSizer()->Add(m_windows, 1, wxEXPAND);
    }
    m_tabCtrl->Refresh();
    Layout();
}

clGenericNotebook::clGenericNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                     long style, const wxString& name)
    : wxPanel(parent, id, pos, size, wxWANTS_CHARS | wxTAB_TRAVERSAL | (style & wxWINDOW_STYLE_MASK), name)
{
    static bool once = false;
    if(!once) {
        // Add PNG and Bitmap handler
        wxImage::AddHandler(new wxPNGHandler);
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        once = true;
    }
    style = (style & ~wxWINDOW_STYLE_MASK); // filter out wxWindow styles
    Bind(wxEVT_SIZE, &clGenericNotebook::OnSize, this);
    Bind(wxEVT_SIZING, &clGenericNotebook::OnSize, this);
    m_tabCtrl = new clTabCtrl(this, style);
    m_windows = new WindowStack(this, wxID_ANY, true);

    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clGenericNotebook::OnColoursChanged, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_SETTINGS_CHANGED, &clGenericNotebook::OnPreferencesChanged, this);
    PositionControls();
}

clGenericNotebook::~clGenericNotebook()
{
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clGenericNotebook::OnColoursChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_SETTINGS_CHANGED, &clGenericNotebook::OnPreferencesChanged, this);
}

void clGenericNotebook::AddPage(wxWindow* page, const wxString& label, bool selected, int bitmapId,
                                const wxString& shortLabel)
{
    InsertPage(GetPageCount(), page, label, selected, bitmapId, shortLabel);
}

void clGenericNotebook::DoChangeSelection(wxWindow* page) { m_windows->Select(page); }

bool clGenericNotebook::InsertPage(size_t index, wxWindow* page, const wxString& label, bool selected, int bitmapId,
                                   const wxString& shortLabel)
{
    clTabInfo::Ptr_t tab(new clTabInfo(m_tabCtrl, GetStyle(), page, label, bitmapId));
    tab->SetShortLabel(shortLabel.IsEmpty() ? label : shortLabel);
    tab->SetActive(selected, GetStyle());
    return m_tabCtrl->InsertPage(index, tab);
}

void clGenericNotebook::SetStyle(size_t style)
{
    m_tabCtrl->SetStyle(style);
    PositionControls();
    m_tabCtrl->Refresh();
}

wxWindow* clGenericNotebook::GetCurrentPage() const
{
    if(m_tabCtrl->GetSelection() == wxNOT_FOUND)
        return NULL;
    return m_tabCtrl->GetPage(m_tabCtrl->GetSelection());
}

int clGenericNotebook::FindPage(wxWindow* page) const { return m_tabCtrl->FindPage(page); }

bool clGenericNotebook::RemovePage(size_t page, bool notify) { return m_tabCtrl->RemovePage(page, notify, false); }
bool clGenericNotebook::DeletePage(size_t page, bool notify) { return m_tabCtrl->RemovePage(page, notify, true); }
bool clGenericNotebook::DeleteAllPages() { return m_tabCtrl->DeleteAllPages(); }

void clGenericNotebook::EnableStyle(NotebookStyle style, bool enable)
{
    size_t flags = GetStyle();
    if(enable) {
        flags |= style;
    } else {
        flags &= ~style;
    }
    SetStyle(flags);
}

void clGenericNotebook::SetTabDirection(wxDirection d)
{
    size_t flags = GetStyle();
    // Unless wxBOTTOM, use UP tabs (default, no flag is set)
    flags &= ~kNotebook_BottomTabs;
    if(d == wxBOTTOM) {
        flags |= kNotebook_BottomTabs;
    }
    SetStyle(flags);
}

bool clGenericNotebook::MoveActivePage(int newIndex)
{
    return m_tabCtrl->MoveActiveToIndex(newIndex, GetSelection() > newIndex ? eDirection::kLeft : eDirection::kRight);
}

void clGenericNotebook::OnSize(wxSizeEvent& event)
{
    event.Skip();
    // CallAfter(&clGenericNotebook::PositionControls);
}

void clGenericNotebook::OnPreferencesChanged(wxCommandEvent& event)
{
    event.Skip();
    // update the renderer
    SetArt(clTabRenderer::CreateRenderer(this, GetStyle()));

    // Enable tab switching using the mouse scrollbar
    EnableStyle(kNotebook_MouseScrollSwitchTabs, EditorConfigST::Get()->GetOptions()->IsMouseScrollSwitchTabs());
}

void clGenericNotebook::OnColoursChanged(clCommandEvent& event)
{
    event.Skip();
    SetBackgroundColour(clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    m_tabCtrl->m_colours.UpdateColours(GetStyle());
    Refresh();
}

//----------------------------------------------------------
// clGenericNotebook header
//----------------------------------------------------------
// -------------------------------------------------------------------------------
// clTabCtrl class.
// This is where things are actually getting done
// -------------------------------------------------------------------------------
clTabCtrl::clTabCtrl(wxWindow* notebook, size_t style)
    : wxPanel(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxWANTS_CHARS | wxTAB_TRAVERSAL)
    , m_style(style)
    , m_closeButtonClickedIndex(wxNOT_FOUND)
    , m_contextMenu(NULL)
    , m_dragStartTime((time_t)-1)
{
    m_bitmaps = new clBitmapList;
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_art = clTabRenderer::CreateRenderer(this, m_style);
    DoSetBestSize();

    SetDropTarget(new clTabCtrlDropTarget(this));
    Bind(wxEVT_PAINT, &clTabCtrl::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &clTabCtrl::OnEraseBG, this);
    Bind(wxEVT_SIZE, &clTabCtrl::OnSize, this);
    Bind(wxEVT_LEFT_DOWN, &clTabCtrl::OnLeftDown, this);
    Bind(wxEVT_RIGHT_UP, &clTabCtrl::OnRightUp, this);
    Bind(wxEVT_LEFT_UP, &clTabCtrl::OnLeftUp, this);
    Bind(wxEVT_MOTION, &clTabCtrl::OnMouseMotion, this);
    Bind(wxEVT_MIDDLE_UP, &clTabCtrl::OnMouseMiddleClick, this);
    Bind(wxEVT_MOUSEWHEEL, &clTabCtrl::OnMouseScroll, this);
    Bind(wxEVT_CONTEXT_MENU, &clTabCtrl::OnContextMenu, this);
    Bind(wxEVT_LEFT_DCLICK, &clTabCtrl::OnLeftDClick, this);

    // call refresh when leaving the window
    Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& e) {
        e.Skip();
        // reset the close button state
        for(auto tab : m_tabs) {
            tab->m_xButtonState = eButtonState::kNormal;
        }
        Refresh();
    });

    wxTheApp->Bind(wxEVT_ACTIVATE_APP, &clTabCtrl::OnActivateApp, this);

    SetStyle(m_style);
    m_colours.UpdateColours(m_style);

    // The history object
    m_history.reset(new clTabHistory());
}

void clTabCtrl::OnActivateApp(wxActivateEvent& e)
{
    e.Skip(); // reset the close button state
    for(auto tab : m_tabs) {
        tab->m_xButtonState = eButtonState::kNormal;
    }
    Refresh();
}

void clTabCtrl::DoSetBestSize()
{
    wxClientDC dc(this);

    wxFont font = clTabRenderer::GetTabFont(true);
    dc.SetFont(font);

    wxString text;
    for(clTabInfo::Ptr_t ti : m_tabs) {
        if(text.length() < ti->GetBestLabel(m_style).length()) {
            text = ti->GetBestLabel(m_style);
        }
    }
    if(text.empty()) {
        text = "__WORKSPACE__";
    }
    wxSize sz = dc.GetTextExtent(text);
    int bmpHeight = clTabRenderer::GetDefaultBitmapHeight(GetArt()->ySpacer);

    m_nHeight = sz.GetHeight() + (4 * GetArt()->ySpacer);
    m_nHeight = wxMax(m_nHeight, bmpHeight);
    m_nWidth = sz.GetWidth();

    SetSizeHints(wxSize(-1, m_nHeight));
    SetSize(-1, m_nHeight);
    PositionFilelistButton();
}

bool clTabCtrl::ShiftRight(clTabInfo::Vec_t& tabs)
{
    // Move the first tab from the list and adjust the remainder
    // of the tabs x coordiate
    if(!tabs.empty()) {
        clTabInfo::Ptr_t t = tabs.at(0);
        int width = t->GetWidth();
        tabs.erase(tabs.begin() + 0);

        for(size_t i = 0; i < tabs.size(); ++i) {
            clTabInfo::Ptr_t t = tabs.at(i);
            t->GetRect().SetX(t->GetRect().x - width + GetArt()->overlapWidth);
        }
        return true;
    }
    return false;
}

bool clTabCtrl::IsActiveTabInList(const clTabInfo::Vec_t& tabs) const
{
    for(size_t i = 0; i < tabs.size(); ++i) {
        if(tabs.at(i)->IsActive())
            return true;
    }
    return false;
}

bool clTabCtrl::IsActiveTabVisible(const clTabInfo::Vec_t& tabs) const
{
    wxRect clientRect(GetClientRect());
    if(GetStyle() & kNotebook_ShowFileListButton) {
        clientRect.SetWidth(clientRect.GetWidth() - CHEVRON_SIZE);
    }

    for(size_t i = 0; i < tabs.size(); ++i) {
        clTabInfo::Ptr_t t = tabs.at(i);
        wxRect tabRect = t->GetRect();
        tabRect.SetWidth(tabRect.GetWidth() * 0.5); // The tab does not need to be fully shown, but at least 50% of it
        if(t->IsActive() && clientRect.Contains(tabRect)) {
            return true;
        }
    }
    return false;
}

clTabCtrl::~clTabCtrl()
{
    wxDELETE(m_contextMenu);
    Unbind(wxEVT_PAINT, &clTabCtrl::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, &clTabCtrl::OnEraseBG, this);
    Unbind(wxEVT_SIZE, &clTabCtrl::OnSize, this);
    Unbind(wxEVT_LEFT_DOWN, &clTabCtrl::OnLeftDown, this);
    Unbind(wxEVT_RIGHT_UP, &clTabCtrl::OnRightUp, this);
    Unbind(wxEVT_LEFT_UP, &clTabCtrl::OnLeftUp, this);
    Unbind(wxEVT_MOTION, &clTabCtrl::OnMouseMotion, this);
    Unbind(wxEVT_MIDDLE_UP, &clTabCtrl::OnMouseMiddleClick, this);
    Unbind(wxEVT_CONTEXT_MENU, &clTabCtrl::OnContextMenu, this);
    Unbind(wxEVT_LEFT_DCLICK, &clTabCtrl::OnLeftDClick, this);
    Unbind(wxEVT_MOUSEWHEEL, &clTabCtrl::OnMouseScroll, this);
    wxTheApp->Unbind(wxEVT_ACTIVATE_APP, &clTabCtrl::OnActivateApp, this);
    wxDELETE(m_bitmaps);
}

void clTabCtrl::OnSize(wxSizeEvent& event)
{
    event.Skip();
    m_visibleTabs.clear();
    PositionFilelistButton();
    Refresh();
}

void clTabCtrl::OnEraseBG(wxEraseEvent& e) { wxUnusedVar(e); }

void clTabCtrl::OnPaint(wxPaintEvent& e)
{
    wxPaintDC dc(this);
    wxGCDC gcdc;
    DrawingUtils::GetGCDC(dc, gcdc);
    PrepareDC(gcdc);

    wxRect clientRect(GetClientRect());
    if(clientRect.width <= 3 || clientRect.height <= 3)
        return;

    wxRect rect(GetClientRect());
    if(m_tabs.empty()) {
        // Draw the default bg colour
        gcdc.SetPen(clSystemSettings::GetDefaultPanelColour());
        gcdc.SetBrush(clSystemSettings::GetDefaultPanelColour());
        gcdc.DrawRectangle(GetClientRect());
        return;
    }

    // Draw the tab area background colours
    clTabInfo::Ptr_t active_tab = GetActiveTabInfo();
    clTabColours activeTabColours = m_colours;

    m_chevronRect = {};
    if(m_style & kNotebook_ShowFileListButton) {
        m_chevronRect = GetFileListButtonRect(this, m_style, gcdc);
    }

#ifdef __WXOSX__
    clientRect.Inflate(1);
#endif

    GetArt()->DrawBackground(this, gcdc, clientRect, m_colours, m_style);
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        m_tabs[i]->CalculateOffsets(GetStyle(), gcdc);
    }

    // Sanity
    if(!(rect.GetSize().x > 0 && rect.GetSize().y > 0)) {
        m_visibleTabs.clear();
        return;
    }

    SetBackgroundColour(GetArt()->DrawBackground(this, gcdc, rect, m_colours, m_style));
    UpdateVisibleTabs();
    gcdc.SetClippingRegion(clientRect.x, clientRect.y, clientRect.width - m_chevronRect.GetWidth(), clientRect.height);

    // check if the mouse is over a visible tab
    int tabHit, realPos;
    eDirection align;
    auto mousePos = GetParent()->ScreenToClient(::wxGetMousePosition());
    TestPoint(mousePos, realPos, tabHit, align);

    wxUnusedVar(realPos);
    wxUnusedVar(align);

    clTabInfo::Ptr_t activeTab;
    int activeTabIndex = wxNOT_FOUND;
    for(int i = (m_visibleTabs.size() - 1); i >= 0; --i) {
        clTabInfo::Ptr_t tab = m_visibleTabs[i];
        if(tab->IsActive() && !activeTab) {
            activeTab = tab;
            activeTabIndex = i;
        }

        clTabColours* pColours = &m_colours;
        clTabColours user_colours;

        m_art->Draw(this, gcdc, gcdc, *tab.get(), i, (*pColours), m_style,
                    tabHit == i ? eButtonState::kHover : eButtonState::kNormal, tab->m_xButtonState);
    }

    if(!activeTab) {
        m_tabs[0]->SetActive(true, GetStyle());
        activeTab = m_tabs[0]; // make the first tab, the default one
        activeTabIndex = 0;
    }

    // Redraw the active tab
    m_art->Draw(this, gcdc, gcdc, *activeTab.get(), activeTabIndex, activeTabColours, m_style, eButtonState::kNormal,
                activeTab->m_xButtonState);
    gcdc.DestroyClippingRegion();

    m_art->FinaliseBackground(this, gcdc, clientRect, activeTab->GetRect(), m_colours, m_style);
}

void clTabCtrl::DoUpdateCoordiantes(clTabInfo::Vec_t& tabs)
{
    int xx = 0;
    wxRect clientRect = GetClientRect();
    for(size_t i = 0; i < tabs.size(); ++i) {
        clTabInfo::Ptr_t tab = tabs[i];
        tab->GetRect().SetX(xx);
        tab->GetRect().SetY(0);
        tab->GetRect().SetWidth(tab->GetWidth());
        tab->GetRect().SetHeight(clientRect.GetHeight());
        xx += tab->GetWidth();
    }
}

void clTabCtrl::UpdateVisibleTabs(bool forceReshuffle)
{
    // don't update the list if we don't need to
    if(IsActiveTabInList(m_visibleTabs) && IsActiveTabVisible(m_visibleTabs) && !forceReshuffle)
        return;

    // set the physical coords for each tab (we do this for all the tabs)
    DoUpdateCoordiantes(m_tabs);

    // Start shifting right tabs until the active tab is visible
    m_visibleTabs = m_tabs;
    if(!IsVerticalTabs()) {
        while(!IsActiveTabVisible(m_visibleTabs)) {
            if(!ShiftRight(m_visibleTabs))
                break;
        }
    } else {
        while(!IsActiveTabVisible(m_visibleTabs)) {
            if(!ShiftBottom(m_visibleTabs))
                break;
        }
    }
}

void clTabCtrl::OnLeftDown(wxMouseEvent& event)
{
    event.Skip();
    const wxPoint& evetPos = event.GetPosition();
    m_closeButtonClickedIndex = wxNOT_FOUND;

    int tabHit, realPos;
    eDirection align;
    TestPoint(evetPos, realPos, tabHit, align);
    if(tabHit == wxNOT_FOUND)
        return;

    // Did we hit the active tab?
    bool clickWasOnActiveTab = (GetSelection() == realPos);

    if(GetStyle() & kNotebook_CloseButtonOnActiveTab) {
        // we clicked on the selected index
        clTabInfo::Ptr_t t = m_visibleTabs.at(tabHit);
        wxRect xRect = t->GetCloseButtonRect();

        if(xRect.Contains(evetPos)) {
            m_closeButtonClickedIndex = tabHit;
            t->m_xButtonState = eButtonState::kPressed;
            Refresh();
            return;
        }
    }

    // If the click was not on the active tab, set the clicked
    // tab as the new selection and leave this function
    if(!clickWasOnActiveTab) {
        SetSelection(realPos);
    }

    // We clicked on a tab, so prepare to start DnD operation
    if((m_style & kNotebook_AllowDnD)) {
        wxCHECK_RET(!m_dragStartTime.IsValid(), "A leftdown event when Tab DnD was already starting/started");
        m_dragStartTime = wxDateTime::UNow();
        m_dragStartPos = wxPoint(event.GetX(), event.GetY());
    }
}

int clTabCtrl::ChangeSelection(size_t tabIdx)
{
    // wxWindowUpdateLocker locker(GetParent());
    int oldSelection = GetSelection();
    if(!IsIndexValid(tabIdx))
        return oldSelection;

    for(size_t i = 0; i < m_tabs.size(); ++i) {
        clTabInfo::Ptr_t tab = m_tabs.at(i);
        tab->SetActive((i == tabIdx), GetStyle());
        if(i == tabIdx) {
            m_history->Push(tab->GetWindow());
        }
    }

    clTabInfo::Ptr_t activeTab = GetActiveTabInfo();
    if(activeTab) {
        static_cast<clGenericNotebook*>(GetParent())->DoChangeSelection(activeTab->GetWindow());
        // Only SetFocus if !Wayland, otherwise it hangs; see #2457
        if(!clIsWaylandSession())
            activeTab->GetWindow()->CallAfter(&wxWindow::SetFocus);
    }
    Refresh();
    return oldSelection;
}

int clTabCtrl::SetSelection(size_t tabIdx)
{
    DoChangeSelection(tabIdx);
    return wxNOT_FOUND;
}

int clTabCtrl::GetSelection() const
{
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        clTabInfo::Ptr_t tab = m_tabs.at(i);
        if(tab->IsActive())
            return i;
    }
    return wxNOT_FOUND;
}

clTabInfo::Ptr_t clTabCtrl::GetTabInfo(size_t index)
{
    if(!IsIndexValid(index))
        return clTabInfo::Ptr_t(NULL);
    return m_tabs.at(index);
}

clTabInfo::Ptr_t clTabCtrl::GetTabInfo(size_t index) const
{
    if(!IsIndexValid(index))
        return clTabInfo::Ptr_t(NULL);
    return m_tabs.at(index);
}

clTabInfo::Ptr_t clTabCtrl::GetTabInfo(wxWindow* page)
{
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        clTabInfo::Ptr_t tab = m_tabs.at(i);
        if(tab->GetWindow() == page)
            return tab;
    }
    return clTabInfo::Ptr_t(NULL);
}

bool clTabCtrl::SetPageText(size_t page, const wxString& text)
{
    clTabInfo::Ptr_t tab = GetTabInfo(page);
    if(!tab) {
        return false;
    }
    tab->SetLabel(text, GetStyle());

    // trigger an "on-size" event
    PostSizeEventToParent();
    return true;
}

void clTabCtrl::DoUpdateXCoordFromPage(wxWindow* page, int diff)
{
    // Update the coordinates starting from the current tab
    bool foundActiveTab = false;
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        if(!foundActiveTab && (m_tabs.at(i)->GetWindow() == page)) {
            foundActiveTab = true;
        } else if(foundActiveTab) {
            m_tabs.at(i)->GetRect().SetX(m_tabs.at(i)->GetRect().GetX() + diff);
        }
    }
}

clTabInfo::Ptr_t clTabCtrl::GetActiveTabInfo()
{
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        if(m_tabs.at(i)->IsActive()) {
            return m_tabs.at(i);
        }
    }
    return clTabInfo::Ptr_t(NULL);
}

void clTabCtrl::AddPage(clTabInfo::Ptr_t tab) { InsertPage(m_tabs.size(), tab); }

WindowStack* clTabCtrl::GetStack() { return reinterpret_cast<clGenericNotebook*>(GetParent())->m_windows; }

bool clTabCtrl::InsertPage(size_t index, clTabInfo::Ptr_t tab)
{
    int oldSelection = GetSelection();
    if(index > m_tabs.size())
        return false;
    m_tabs.insert(m_tabs.begin() + index, tab);
    bool sendPageChangedEvent = (oldSelection == wxNOT_FOUND) || tab->IsActive();

    int tabIndex = index;
    GetStack()->Add(tab->GetWindow(), tab->IsActive());
    if(sendPageChangedEvent) {
        ChangeSelection(tabIndex);

        // Send an event
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGED);
        event.SetEventObject(GetParent());
        event.SetSelection(GetSelection());
        event.SetOldSelection(oldSelection);
        GetParent()->GetEventHandler()->ProcessEvent(event);
    }
    m_history->Push(tab->GetWindow());
    // DoSetBestSize();
    Refresh();
    return true;
}

wxString clTabCtrl::GetPageText(size_t page) const
{
    clTabInfo::Ptr_t tab = GetTabInfo(page);
    if(tab)
        return tab->GetLabel();
    return "";
}

const wxBitmap& clTabCtrl::GetPageBitmap(size_t index) const
{
    clTabInfo::Ptr_t tab = GetTabInfo(index);
    if(tab) {
        return GetBitmaps()->Get(tab->GetBitmap(), false);
    }
    return wxNullBitmap;
}

int clTabCtrl::GetPageBitmapIndex(size_t index) const
{
    clTabInfo::Ptr_t tab = GetTabInfo(index);
    if(tab) {
        return tab->GetBitmap();
    }
    return wxNOT_FOUND;
}

void clTabCtrl::SetPageBitmap(size_t index, int bmp)
{
    clTabInfo::Ptr_t tab = GetTabInfo(index);
    if(!tab)
        return;

    // Set the new bitmap and calc the difference
    int oldWidth = tab->GetWidth();
    tab->SetBitmap(bmp, GetStyle());
    int newWidth = tab->GetWidth();
    int diff = (newWidth - oldWidth);

    // Update the width of the tabs from the current tab by "diff"
    DoUpdateXCoordFromPage(tab->GetWindow(), diff);

    // Redraw the tab control
    Refresh();
}

void clTabCtrl::OnLeftUp(wxMouseEvent& event)
{
    event.Skip();

    m_dragStartTime.Set((time_t)-1); // Not considering D'n'D so reset any saved values
    m_dragStartPos = wxPoint();
    for(clTabInfo::Ptr_t t : m_tabs) {
        t->m_xButtonState = t->IsActive() ? eButtonState::kNormal : eButtonState::kDisabled;
    }

    // First check if the chevron was clicked. We do this because the chevron could overlap the buttons drawing
    // area
    int tabHit, realPos;
    eDirection align;
    TestPoint(event.GetPosition(), realPos, tabHit, align);
    if(tabHit != wxNOT_FOUND) {
        if((GetStyle() & kNotebook_CloseButtonOnActiveTab)) {
            // we clicked on the selected index
            clTabInfo::Ptr_t t = m_visibleTabs.at(tabHit);
            wxRect xRect = t->GetCloseButtonRect();
            xRect.Inflate(2); // don't be picky if we did not click exactly on the 16x16 bitmap...
            if(m_closeButtonClickedIndex == tabHit && xRect.Contains(event.GetPosition())) {
                if(GetStyle() & kNotebook_CloseButtonOnActiveTabFireEvent) {
                    // let the user process this
                    wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSE_BUTTON);
                    event.SetEventObject(GetParent());
                    event.SetSelection(realPos);
                    GetParent()->GetEventHandler()->AddPendingEvent(event);
                } else {
                    CallAfter(&clTabCtrl::DoDeletePage, realPos);
                }
            }
        }
    }
}

void clTabCtrl::OnMouseMotion(wxMouseEvent& event)
{
    event.Skip();
    int realPos, tabHit;
    wxString curtip = GetToolTipText();
    eDirection align;
    TestPoint(event.GetPosition(), realPos, tabHit, align);
    if(tabHit == wxNOT_FOUND || realPos == wxNOT_FOUND) {
        if(!curtip.IsEmpty()) {
            SetToolTip("");
        }
    } else {
        clTabInfo::Ptr_t tabInfo = m_tabs.at(realPos);
        const wxString& pagetip = tabInfo->GetTooltip().empty() ? tabInfo->GetLabel() : tabInfo->GetTooltip();
        if(pagetip != curtip) {
            SetToolTip(pagetip);
        }
    }

    if(m_dragStartTime.IsValid()) { // If we're tugging on the tab, consider starting D'n'D
        wxTimeSpan diff = wxDateTime::UNow() - m_dragStartTime;
        if(diff.GetMilliseconds() > 100 && // We need to check both x and y distances as tabs may be vertical
           ((abs(m_dragStartPos.x - event.GetX()) > 10) || (abs(m_dragStartPos.y - event.GetY()) > 10))) {
            OnBeginDrag(); // Sufficient time and distance since the LeftDown for a believable D'n'D start
        }
    }

    // Refresh if hovering the close button state
    for(clTabInfo::Ptr_t t : m_tabs) {
        t->m_xButtonState = t->IsActive() ? eButtonState::kNormal : eButtonState::kDisabled;
    }
    if((realPos != wxNOT_FOUND) && (GetStyle() & kNotebook_CloseButtonOnActiveTab)) {
        clTabInfo::Ptr_t t = m_tabs[realPos];
        wxRect xRect = t->GetCloseButtonRect();
        if(xRect.Contains(event.GetPosition())) {
            t->m_xButtonState = event.LeftIsDown() ? eButtonState::kPressed : eButtonState::kHover;
        }
    }
    Refresh();
}

void clTabCtrl::TestPoint(const wxPoint& pt, int& realPosition, int& tabHit, eDirection& align)
{
    realPosition = wxNOT_FOUND;
    tabHit = wxNOT_FOUND;
    align = eDirection::kInvalid;

    if(m_visibleTabs.empty())
        return;

    // Because the tabs are overlapping, we need to test
    // the active tab first
    clTabInfo::Ptr_t activeTab = GetActiveTabInfo();
    if(activeTab && activeTab->GetRect().Contains(pt)) {
        for(size_t i = 0; i < m_visibleTabs.size(); ++i) {
            if(m_visibleTabs.at(i)->GetWindow() == activeTab->GetWindow()) {
                tabHit = i;
                realPosition = DoGetPageIndex(m_visibleTabs.at(i)->GetWindow());
                return;
            }
        }
    }

    for(size_t i = 0; i < m_visibleTabs.size(); ++i) {
        clTabInfo::Ptr_t tab = m_visibleTabs.at(i);
        wxRect r(tab->GetRect());
        if(r.Contains(pt)) {
            if(pt.x > ((r.GetWidth() / 2) + r.GetX())) {
                // the point is on the RIGHT side
                align = eDirection::kRight;
            } else {
                align = eDirection::kLeft;
            }
            tabHit = i;
            realPosition = DoGetPageIndex(tab->GetWindow());
            return;
        }
    }
}

void clTabCtrl::SetStyle(size_t style)
{
    this->m_style = style;
    if(IsVerticalTabs()) {
        SetSizeHints(wxSize(m_nWidth, -1));
        SetSize(m_nWidth, -1);
    } else {
        SetSizeHints(wxSize(-1, m_nHeight));
        SetSize(-1, m_nHeight);
    }
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        m_tabs.at(i)->CalculateOffsets(GetStyle());
    }
    m_visibleTabs.clear();
    Layout();
    if(GetStyle() & kNotebook_HideTabBar) {
        Hide();
    } else {
        if(((GetStyle() & kNotebook_HideTabBar) == 0) && !IsShown()) {
            Show();
        }
    }
    Refresh();
}

wxWindow* clTabCtrl::GetPage(size_t index) const
{
    clTabInfo::Ptr_t tab = GetTabInfo(index);
    if(tab)
        return tab->GetWindow();
    return NULL;
}

bool clTabCtrl::IsIndexValid(size_t index) const { return (index < m_tabs.size()); }

int clTabCtrl::FindPage(wxWindow* page) const
{
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        if(m_tabs.at(i)->GetWindow() == page) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

bool clTabCtrl::RemovePage(size_t page, bool notify, bool deletePage)
{
    wxWindow* nextSelection = NULL;
    if(!IsIndexValid(page))
        return false;
    bool deletingSelection = ((int)page == GetSelection());

    if(notify) {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSING);
        event.SetEventObject(GetParent());
        event.SetSelection(page);
        GetParent()->GetEventHandler()->ProcessEvent(event);
        if(!event.IsAllowed()) {
            // Vetoed
            return false;
        }
    }

    // Remove the tab from the "all-tabs" list
    clTabInfo::Ptr_t tab = m_tabs.at(page);
    m_tabs.erase(m_tabs.begin() + page);

    // Remove this page from the history
    m_history->Pop(tab->GetWindow());

    // Remove the tabs from the visible tabs list
    clTabInfo::Vec_t::iterator iter = std::find_if(m_visibleTabs.begin(), m_visibleTabs.end(), [&](clTabInfo::Ptr_t t) {
        if(t->GetWindow() == tab->GetWindow()) {
            return true;
        }
        return false;
    });
    if(iter != m_visibleTabs.end()) {
        iter = m_visibleTabs.erase(iter);

        for(; iter != m_visibleTabs.end(); ++iter) {
            // update the remainding tabs coordinates
            if(IsVerticalTabs()) {
                (*iter)->GetRect().SetY((*iter)->GetRect().GetY() - tab->GetHeight() + GetArt()->verticalOverlapWidth);
            } else {
                (*iter)->GetRect().SetX((*iter)->GetRect().GetX() - tab->GetWidth() + GetArt()->overlapWidth);
            }
        }
    }

    // Choose a new selection, but only if we are deleting the active tab
    nextSelection = NULL;
    if(deletingSelection) {
        while(!m_history->GetHistory().empty() && !nextSelection) {
            nextSelection = m_history->PrevPage();
            if(!GetTabInfo(nextSelection)) {
                // The history contains a tab that no longer exists
                m_history->Pop(nextSelection);
                nextSelection = NULL;
            }
        }
        // It is OK to end up with a null next selection, we will handle it later
    }

    // If we are deleting the active page, select the new one first and _then_ remove the
    // current one. This removes any possible flicker
    if(deletingSelection) {
        // Always make sure we have something to select...
        if(!nextSelection && !m_tabs.empty()) {
            nextSelection = m_tabs.at(0)->GetWindow();
        }

        int nextSel = DoGetPageIndex(nextSelection);
        if(nextSel != wxNOT_FOUND) {
            ChangeSelection(nextSel);
            if(notify) {
                wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGED);
                event.SetEventObject(GetParent());
                event.SetSelection(GetSelection());
                GetParent()->GetEventHandler()->ProcessEvent(event);
            }
        }
    }

    // Now remove the page from the notebook. We will delete the page
    // ourself, so there is no need to call DeletePage
    GetStack()->Remove(tab->GetWindow());
    if(deletePage) {
        // Destory the page
        tab->GetWindow()->Destroy();
    } else {
        // Just hide it
        tab->GetWindow()->Hide();
    }

    if(notify) {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSED);
        event.SetEventObject(GetParent());
        GetParent()->GetEventHandler()->ProcessEvent(event);
    }
    UpdateVisibleTabs(true);
    Refresh();
    return true;
}

int clTabCtrl::DoGetPageIndex(wxWindow* win) const
{
    if(!win)
        return wxNOT_FOUND;
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        if(m_tabs.at(i)->GetWindow() == win)
            return i;
    }
    return wxNOT_FOUND;
}

bool clTabCtrl::DeleteAllPages()
{
    GetStack()->Clear();
    m_tabs.clear();
    m_visibleTabs.clear();
    m_history->Clear();
    Refresh();
    return true;
}

void clTabCtrl::OnMouseMiddleClick(wxMouseEvent& event)
{
    event.Skip();
    if(GetStyle() & kNotebook_MouseMiddleClickClosesTab) {
        int realPos, tabHit;
        eDirection align;
        TestPoint(event.GetPosition(), realPos, tabHit, align);
        if(realPos != wxNOT_FOUND) {
            CallAfter(&clTabCtrl::DoDeletePage, realPos);
        }
    } else if(GetStyle() & kNotebook_MouseMiddleClickFireEvent) {
        int realPos, tabHit;
        eDirection align;
        TestPoint(event.GetPosition(), realPos, tabHit, align);
        if(realPos != wxNOT_FOUND) {
            // Just fire an event
            wxBookCtrlEvent e(wxEVT_BOOK_PAGE_CLOSE_BUTTON);
            e.SetEventObject(GetParent());
            e.SetSelection(realPos);
            GetParent()->GetEventHandler()->AddPendingEvent(e);
        }
    }
}

void clTabCtrl::GetAllPages(std::vector<wxWindow*>& pages)
{
    std::for_each(m_tabs.begin(), m_tabs.end(),
                  [&](clTabInfo::Ptr_t tabInfo) { pages.push_back(tabInfo->GetWindow()); });
}

void clTabCtrl::SetMenu(wxMenu* menu)
{
    wxDELETE(m_contextMenu);
    m_contextMenu = menu;
}

void clTabCtrl::OnContextMenu(wxContextMenuEvent& event)
{
    event.Skip();

    wxPoint pt = ::wxGetMousePosition();
    pt = ScreenToClient(pt);
    int realPos, tabHit;
    eDirection align;
    TestPoint(pt, realPos, tabHit, align);

    if((realPos != wxNOT_FOUND)) {
        // Show context menu for active tabs only
        if(m_contextMenu && (realPos == GetSelection())) {
            PopupMenu(m_contextMenu);
        } else {
            // fire an event for the selected tab
            wxBookCtrlEvent menuEvent(wxEVT_BOOK_TAB_CONTEXT_MENU);
            menuEvent.SetEventObject(GetParent());
            menuEvent.SetSelection(realPos);
            GetParent()->GetEventHandler()->ProcessEvent(menuEvent);
        }
    }
}

void clTabCtrl::DoShowTabList()
{
    // sanity
    if(!m_fileListButton) {
        return;
    }

    const int curselection = GetSelection();
    wxMenu menu;
    const int firstTabPageID = 13457;
    int pageMenuID = firstTabPageID;

    // Do we have pages opened?

    // Optionally make a sorted view of tabs.
    std::vector<size_t> sortedIndexes(m_tabs.size());
    {
        // std is C++11 at the moment, so no generalized capture.
        size_t index = 0;
        std::generate(sortedIndexes.begin(), sortedIndexes.end(), [&index]() { return index++; });
    }

    if(EditorConfigST::Get()->GetOptions()->IsSortTabsDropdownAlphabetically()) {
        std::sort(sortedIndexes.begin(), sortedIndexes.end(),
                  [this](size_t i1, size_t i2) { return m_tabs[i1]->m_label.CmpNoCase(m_tabs[i2]->m_label) < 0; });
    }

    for(auto sortedIndex : sortedIndexes) {
        clTabInfo::Ptr_t tab = m_tabs.at(sortedIndex);
        wxWindow* pWindow = tab->GetWindow();
        wxString label = tab->GetLabel();
        wxMenuItem* item = new wxMenuItem(&menu, pageMenuID, label, "", wxITEM_CHECK);
        menu.Append(item);
        item->Check(tab->IsActive());
        menu.Bind(
            wxEVT_MENU,
            [=](wxCommandEvent& event) {
                clGenericNotebook* book = dynamic_cast<clGenericNotebook*>(this->GetParent());
                int newSelection = book->GetPageIndex(pWindow);
                if(newSelection != curselection) {
                    book->SetSelection(newSelection);
                }
            },
            pageMenuID);
        pageMenuID++;
    }

    // Let others handle this event as well
    clContextMenuEvent menuEvent(wxEVT_BOOK_FILELIST_BUTTON_CLICKED);
    menuEvent.SetMenu(&menu);
    menuEvent.SetEventObject(GetParent()); // The clGenericNotebook
    GetParent()->GetEventHandler()->ProcessEvent(menuEvent);

    if(menu.GetMenuItemCount() == 0) {
        return;
    }
    m_fileListButton->ShowMenu(menu);
}

bool clTabCtrl::SetPageToolTip(size_t page, const wxString& tooltip)
{
    clTabInfo::Ptr_t tab = GetTabInfo(page);
    if(tab) {
        tab->SetTooltip(tooltip);
        return true;
    }
    return false;
}

int clTabCtrl::DoGetPageIndex(const wxString& label) const
{
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        if(m_tabs.at(i)->GetLabel() == label)
            return i;
    }
    return wxNOT_FOUND;
}

void clTabCtrl::DoChangeSelection(size_t index)
{
    // sanity
    if(index >= m_tabs.size())
        return;

    int oldSelection = GetSelection();
    /// Do nothing if the tab is already selected
    if(oldSelection == (int)index) {
        ChangeSelection(index);
        return;
    }

    {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGING);
        event.SetEventObject(GetParent());
        event.SetSelection(oldSelection);
        event.SetOldSelection(wxNOT_FOUND);
        GetParent()->GetEventHandler()->ProcessEvent(event);

        if(!event.IsAllowed()) {
            return; // Vetoed by the user
        }
    }
    ChangeSelection(index);

    // Fire an event
    {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGED);
        event.SetEventObject(GetParent());
        event.SetSelection(GetSelection());
        event.SetOldSelection(oldSelection);
        GetParent()->GetEventHandler()->ProcessEvent(event);
    }
}

bool clTabCtrl::MoveActiveToIndex(int newIndex, eDirection direction)
{
    int activeTabInex = GetSelection();

    // Sanity
    if(newIndex == wxNOT_FOUND)
        return false;
    if(activeTabInex == wxNOT_FOUND)
        return false;
    if((newIndex < 0) || (newIndex >= (int)m_tabs.size()))
        return false;

    bool movingTabRight;
    if(direction == eDirection::kInvalid) {
        movingTabRight = (newIndex > activeTabInex);
    } else if((direction == eDirection::kRight) || (direction == eDirection::kUp)) {
        movingTabRight = true;
    } else {
        movingTabRight = false;
    }

    clTabInfo::Ptr_t movingTab = GetActiveTabInfo();
    clTabInfo::Ptr_t insertBeforeTab = m_tabs.at(newIndex);

    if(!movingTab)
        return false;

    // Step 1:
    // Remove the tab from both the active and from the visible tabs array
    clTabInfo::Vec_t::iterator iter = std::find_if(m_visibleTabs.begin(), m_visibleTabs.end(), [&](clTabInfo::Ptr_t t) {
        if(t->GetWindow() == movingTab->GetWindow()) {
            return true;
        }
        return false;
    });
    if(iter != m_visibleTabs.end()) {
        m_visibleTabs.erase(iter);
    }
    iter = std::find_if(m_tabs.begin(), m_tabs.end(), [&](clTabInfo::Ptr_t t) {
        if(t->GetWindow() == movingTab->GetWindow()) {
            return true;
        }
        return false;
    });
    if(iter != m_tabs.end()) {
        m_tabs.erase(iter);
    }

    // Step 2:
    // Insert 'tab' in its new position (in both arrays)
    iter = std::find_if(m_tabs.begin(), m_tabs.end(), [&](clTabInfo::Ptr_t t) {
        if(t->GetWindow() == insertBeforeTab->GetWindow()) {
            return true;
        }
        return false;
    });

    if(movingTabRight) {
        ++iter;
        // inser the new tab _after_
        if(iter != m_tabs.end()) {
            m_tabs.insert(iter, movingTab);
        } else {
            m_tabs.push_back(movingTab);
        }

        iter = std::find_if(m_visibleTabs.begin(), m_visibleTabs.end(), [&](clTabInfo::Ptr_t t) {
            if(t->GetWindow() == insertBeforeTab->GetWindow()) {
                return true;
            }
            return false;
        });
        ++iter;
        if(iter != m_visibleTabs.end()) {
            m_visibleTabs.insert(iter, movingTab);
        } else {
            m_visibleTabs.push_back(movingTab);
        }
    } else {
        if(iter != m_tabs.end()) {
            m_tabs.insert(iter, movingTab);
        }

        iter = std::find_if(m_visibleTabs.begin(), m_visibleTabs.end(), [&](clTabInfo::Ptr_t t) {
            if(t->GetWindow() == insertBeforeTab->GetWindow()) {
                return true;
            }
            return false;
        });
        if(iter != m_visibleTabs.end()) {
            m_visibleTabs.insert(iter, movingTab);
        }
    }
    // Step 3:
    // Update the visible tabs coordinates
    DoUpdateCoordiantes(m_visibleTabs);

    // And finally: Refresh
    Refresh();
    return true;
}

void clTabCtrl::OnLeftDClick(wxMouseEvent& event)
{
    event.Skip();
    int realPos, tabHit;
    eDirection align;
    TestPoint(event.GetPosition(), realPos, tabHit, align);
    if(tabHit == wxNOT_FOUND) {
        // Fire background d-clicked event
        wxBookCtrlEvent e(wxEVT_BOOK_NEW_PAGE);
        e.SetEventObject(GetParent());
        GetParent()->GetEventHandler()->AddPendingEvent(e);
    } else {
        // a tab was hit
        wxBookCtrlEvent e(wxEVT_BOOK_TAB_DCLICKED);
        e.SetEventObject(GetParent());
        e.SetSelection(realPos);
        GetParent()->GetEventHandler()->AddPendingEvent(e);
    }
}

void clTabCtrl::DoDrawBottomBox(clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc,
                                const clTabColours& colours)
{
    GetArt()->DrawBottomRect(this, activeTab, clientRect, dc, colours, GetStyle());
}

bool clTabCtrl::IsVerticalTabs() const { return false; }

bool clTabCtrl::ShiftBottom(clTabInfo::Vec_t& tabs)
{
    // Move the first tab from the list and adjust the remainder
    // of the tabs y coordiate
    if(!tabs.empty()) {
        clTabInfo::Ptr_t t = tabs.at(0);
        int height = t->GetHeight();
        tabs.erase(tabs.begin() + 0);

        for(size_t i = 0; i < tabs.size(); ++i) {
            clTabInfo::Ptr_t t = tabs.at(i);
            t->GetRect().SetY(t->GetRect().y - height + GetArt()->verticalOverlapWidth);
        }
        return true;
    }
    return false;
}

void clTabCtrl::OnRightUp(wxMouseEvent& event) { event.Skip(); }

void clTabCtrl::SetArt(clTabRenderer::Ptr_t art)
{
    m_art = art;
    DoSetBestSize();
    Refresh();
}

void clTabCtrl::OnMouseScroll(wxMouseEvent& event)
{
    event.Skip();
    if(GetStyle() & kNotebook_MouseScrollSwitchTabs) {
        size_t curSelection = GetSelection();
        if(event.GetWheelRotation() > 0) {
            if(curSelection > 0) {
                SetSelection(curSelection - 1);
            }
        } else {
            if(curSelection < GetTabs().size()) {
                SetSelection(curSelection + 1);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// DnD
// ---------------------------------------------------------------------------

void clTabCtrl::OnBeginDrag()
{
    m_dragStartTime.Set((time_t)-1); // Reset the saved values
    m_dragStartPos = wxPoint();

    // We simply drag the active tab index
    s_clTabCtrlDnD_Data.srcTabCtrl = this;
    s_clTabCtrlDnD_Data.tabIndex = GetSelection();

    wxTextDataObject dragContent("clTabCtrl");
    wxDropSource dragSource(this);
    dragSource.SetData(dragContent);
    wxDragResult result = dragSource.DoDragDrop(true);
    wxUnusedVar(result);
}

void clTabCtrl::PositionFilelistButton()
{
    if((m_style & kNotebook_ShowFileListButton) == 0) {
        return;
    }

    wxClientDC cdc(this);
    wxRect button_rect_base = GetFileListButtonRect(this, m_style, cdc);
    m_chevronRect = button_rect_base;

    wxRect button_rect = button_rect_base;
    button_rect.Deflate(2);
    button_rect = button_rect.CenterIn(m_chevronRect);

    if(m_fileListButton == nullptr) {
        m_fileListButton =
            new clButton(this, wxID_ANY, BUTTON_FILE_LIST_SYMBOL, wxDefaultPosition, button_rect.GetSize());
        m_fileListButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) {
            wxUnusedVar(event);
            DoShowTabList();
        });
    }

    clColours colours;
    colours.InitFromColour(clSystemSettings::GetDefaultPanelColour());
    colours.SetBgColour(GetBackgroundColour());
    colours.SetBorderColour(GetBackgroundColour());
    m_fileListButton->SetColours(colours);
    m_fileListButton->SetSize(button_rect.GetSize());
    m_fileListButton->Move(button_rect.GetTopLeft());
}

clTabCtrlDropTarget::clTabCtrlDropTarget(clTabCtrl* tabCtrl)
    : m_tabCtrl(tabCtrl)
    , m_notebook(NULL)
{
}

clTabCtrlDropTarget::clTabCtrlDropTarget(clGenericNotebook* notebook)
    : m_tabCtrl(NULL)
    , m_notebook(notebook)
{
}

clTabCtrlDropTarget::~clTabCtrlDropTarget() {}

bool clTabCtrlDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data)
{
    int nTabIndex = s_clTabCtrlDnD_Data.tabIndex;
    clTabCtrl* ctrl = s_clTabCtrlDnD_Data.srcTabCtrl;
    s_clTabCtrlDnD_Data.Clear();

    if(m_tabCtrl == ctrl) {
        // Test the drop tab index
        int realPos, tabHit;
        eDirection align;
        m_tabCtrl->TestPoint(wxPoint(x, y), realPos, tabHit, align);

        // if the tab being dragged and the one we drop it on are the same
        // return false
        if(nTabIndex == realPos)
            return false;
        m_tabCtrl->MoveActiveToIndex(realPos, align);
    } else if(ctrl) {
        int realPos, tabHit;
        eDirection align;
        m_tabCtrl->TestPoint(wxPoint(x, y), realPos, tabHit, align);

        // DnD to another notebook control
        clTabInfo::Ptr_t movingTab = ctrl->GetTabInfo(nTabIndex);
        clGenericNotebook* sourceBook = dynamic_cast<clGenericNotebook*>(ctrl->GetParent());
        clGenericNotebook* targetBook = dynamic_cast<clGenericNotebook*>(m_tabCtrl->GetParent());
        if(!sourceBook || !targetBook)
            return false;

        // To allow moving tabs betwen notebooks, both must have the kNotebook_AllowForeignDnD style bit enabled
        if(!(sourceBook->GetStyle() & kNotebook_AllowForeignDnD))
            return false;
        if(!(targetBook->GetStyle() & kNotebook_AllowForeignDnD))
            return false;

        sourceBook->RemovePage(nTabIndex, false);
        if(realPos == wxNOT_FOUND) {
            targetBook->AddPage(movingTab->GetWindow(), movingTab->GetLabel(), true, movingTab->GetBitmap());
        } else {
            targetBook->InsertPage(realPos, movingTab->GetWindow(), movingTab->GetLabel(), true,
                                   movingTab->GetBitmap());
        }
        return true;
    } else if(m_notebook) {
        wxWindowUpdateLocker locker(wxTheApp->GetTopWindow());
        int where = m_notebook->HitTest(m_notebook->ScreenToClient(wxGetMousePosition()));
        // If the drop tab and the source tab are the same, do nothing
        if(where == nTabIndex) {
            return false;
        }
        // Get the tab info before we remove it
        wxWindow* movingPage = m_notebook->GetPage(nTabIndex);
        wxWindow* dropPage = m_notebook->GetPage(where);
        if(!movingPage || !dropPage) {
            return false;
        }
        wxString label = m_notebook->GetPageText(nTabIndex);
        int bmp = m_notebook->GetPageBitmapIndex(nTabIndex);

        // Remove the page and insert it at the drop index
        m_notebook->RemovePage(nTabIndex, false);
        // Locate the drop index (since we removed a page, the drop index is no longer correct)
        where = m_notebook->GetPageIndex(dropPage);
        m_notebook->InsertPage(where, movingPage, label, true, bmp);
    }
    return true;
}

void clTabCtrl::SetPageModified(size_t page, bool modified)
{
    clTabInfo::Ptr_t tab = GetTabInfo(page);
    if(!tab) {
        return;
    }

    tab->m_isModified = modified;
    Refresh();
}

bool clTabCtrl::IsModified(size_t page) const
{
    clTabInfo::Ptr_t tab = GetTabInfo(page);
    if(!tab) {
        return false;
    }
    return tab->IsModified();
}
