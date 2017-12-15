#include "Notebook.h"
#if !USE_AUI_NOTEBOOK
#include "clTabRendererClassic.h"
#include "clTabRendererCurved.h"
#include "clTabRendererSquare.h"
#include <algorithm>
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

#if defined(WXUSINGDLL_CL) || defined(USE_SFTP) || defined(PLUGINS_DIR)
#define CL_BUILD 1
#endif

#if CL_BUILD
#include "ColoursAndFontsManager.h"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
#include "lexer_configuration.h"
#endif

wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CHANGING, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CHANGED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CLOSING, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CLOSED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CLOSE_BUTTON, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_TAB_DCLICKED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_NAVIGATING, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_TABAREA_DCLICKED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_TAB_CONTEXT_MENU, wxBookCtrlEvent);

extern void Notebook_Init_Bitmaps();

Notebook::Notebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                   const wxString& name)
    : wxPanel(parent, id, pos, size, wxNO_BORDER | wxWANTS_CHARS | wxTAB_TRAVERSAL, name)
{
    static bool once = false;

    if(!once) {
        // Add PNG and Bitmap handler
        wxImage::AddHandler(new wxPNGHandler);
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        Notebook_Init_Bitmaps();
        once = true;
    }

    m_tabCtrl = new clTabCtrl(this, style);
    m_windows = new WindowStack(this);

    wxBoxSizer* sizer;
    if(IsVerticalTabs()) {
        sizer = new wxBoxSizer(wxHORIZONTAL);
    } else {
        sizer = new wxBoxSizer(wxVERTICAL);
    }

    SetSizer(sizer);

    if(GetStyle() & kNotebook_BottomTabs) {
        sizer->Add(m_windows, 1, wxEXPAND);
        sizer->Add(m_tabCtrl, 0, wxEXPAND);

    } else {
        sizer->Add(m_tabCtrl, 0, wxEXPAND);
        sizer->Add(m_windows, 1, wxEXPAND);
    }
    Layout();
}

Notebook::~Notebook() {}

void Notebook::AddPage(wxWindow* page, const wxString& label, bool selected, const wxBitmap& bmp)
{
    clTabInfo::Ptr_t tab(new clTabInfo(m_tabCtrl, GetStyle(), page, label, bmp));
    tab->SetActive(selected, GetStyle());
    m_tabCtrl->AddPage(tab);
}

void Notebook::DoChangeSelection(wxWindow* page) { m_windows->Select(page); }

bool Notebook::InsertPage(size_t index, wxWindow* page, const wxString& label, bool selected, const wxBitmap& bmp)
{
    clTabInfo::Ptr_t tab(new clTabInfo(m_tabCtrl, GetStyle(), page, label, bmp));
    tab->SetActive(selected, GetStyle());
    return m_tabCtrl->InsertPage(index, tab);
}

void Notebook::SetStyle(size_t style)
{
    m_tabCtrl->SetStyle(style);
    GetSizer()->Detach(m_windows);
    GetSizer()->Detach(m_tabCtrl);
    wxSizer* sz = NULL;

    // Replace the sizer
    if(IsVerticalTabs()) {
        sz = new wxBoxSizer(wxHORIZONTAL);
    } else {
        sz = new wxBoxSizer(wxVERTICAL);
    }

    if((style & kNotebook_BottomTabs) || (style & kNotebook_RightTabs)) {
        sz->Add(m_windows, 1, wxEXPAND);
        sz->Add(m_tabCtrl, 0, wxEXPAND);

    } else {
        sz->Add(m_tabCtrl, 0, wxEXPAND);
        sz->Add(m_windows, 1, wxEXPAND);
    }
    SetSizer(sz);
    Layout();
    m_tabCtrl->Refresh();
}

wxWindow* Notebook::GetCurrentPage() const
{
    if(m_tabCtrl->GetSelection() == wxNOT_FOUND) return NULL;
    return m_tabCtrl->GetPage(m_tabCtrl->GetSelection());
}

int Notebook::FindPage(wxWindow* page) const { return m_tabCtrl->FindPage(page); }

bool Notebook::RemovePage(size_t page, bool notify) { return m_tabCtrl->RemovePage(page, notify, false); }
bool Notebook::DeletePage(size_t page, bool notify) { return m_tabCtrl->RemovePage(page, notify, true); }
bool Notebook::DeleteAllPages() { return m_tabCtrl->DeleteAllPages(); }

void Notebook::EnableStyle(NotebookStyle style, bool enable)
{
    size_t flags = GetStyle();
    if(enable) {
        flags |= style;
    } else {
        flags &= ~style;
    }
    SetStyle(flags);
}

void Notebook::SetTabDirection(wxDirection d)
{
    size_t flags = GetStyle();
    // Clear all direction styles
    flags &= ~kNotebook_BottomTabs;
    flags &= ~kNotebook_LeftTabs;
    flags &= ~kNotebook_RightTabs;
#if 0
    if(d == wxBOTTOM || d == wxRIGHT) {
        flags |= kNotebook_BottomTabs;
    }
#else
    if(d == wxBOTTOM) {
        flags |= kNotebook_BottomTabs;
    } else if(d == wxRIGHT) {
        flags |= kNotebook_RightTabs;
    } else if(d == wxLEFT) {
        flags |= kNotebook_LeftTabs;
    }
#endif
    SetStyle(flags);
}

//----------------------------------------------------------
// Notebook header
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
{
    SetBackgroundColour(DrawingUtils::GetPanelBgColour());
    SetBackgroundStyle(wxBG_STYLE_PAINT);
#if CL_BUILD
    if(EditorConfigST::Get()->GetOptions()->GetOptions() & OptionsConfig::Opt_TabStyleMinimal) {
        m_art.reset(new clTabRendererSquare);
    } else if(EditorConfigST::Get()->GetOptions()->GetOptions() & OptionsConfig::Opt_TabStyleTRAPEZOID) {
        m_art.reset(new clTabRendererCurved);
    } else {
        // the default
        m_art.reset(new clTabRendererClassic);
    }
#else
    m_art.reset(new clTabRendererClassic); // Default art
#endif
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
    Bind(wxEVT_KEY_DOWN, &clTabCtrl::OnWindowKeyDown, this);
    Bind(wxEVT_LEFT_DCLICK, &clTabCtrl::OnLeftDClick, this);

    notebook->Bind(wxEVT_KEY_DOWN, &clTabCtrl::OnWindowKeyDown, this);
    if(m_style & kNotebook_DarkTabs) {
        m_colours.InitDarkColours();
    } else {
        m_colours.InitLightColours();
    }
    // The history object
    m_history.reset(new clTabHistory());
}

void clTabCtrl::DoSetBestSize()
{
    wxBitmap bmp(1, 1);
    wxMemoryDC memDC(bmp);
    wxFont font = clTabRenderer::GetTabFont();
    memDC.SetFont(font);

    wxSize sz = memDC.GetTextExtent("Tp");

    m_height = sz.GetHeight() + (4 * GetArt()->ySpacer);
    m_vTabsWidth = sz.GetHeight() + (5 * GetArt()->ySpacer);
    if(IsVerticalTabs()) {
        SetSizeHints(wxSize(m_vTabsWidth, -1));
        SetSize(m_vTabsWidth, -1);
    } else {
        SetSizeHints(wxSize(-1, m_height));
        SetSize(-1, m_height);
    }
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
        if(tabs.at(i)->IsActive()) return true;
    }
    return false;
}

bool clTabCtrl::IsActiveTabVisible(const clTabInfo::Vec_t& tabs) const
{
    wxRect clientRect(GetClientRect());
    if((GetStyle() & kNotebook_ShowFileListButton) && !IsVerticalTabs()) {
        clientRect.SetWidth(clientRect.GetWidth() - CHEVRON_SIZE);
    } else if((GetStyle() & kNotebook_ShowFileListButton) && IsVerticalTabs()) {
        // Vertical tabs
        clientRect.SetHeight(clientRect.GetHeight() - CHEVRON_SIZE);
    }

    for(size_t i = 0; i < tabs.size(); ++i) {
        clTabInfo::Ptr_t t = tabs.at(i);
        if(t->IsActive() && ((!IsVerticalTabs() && clientRect.Intersects(t->GetRect())) ||
                             (IsVerticalTabs() && clientRect.Intersects(t->GetRect()))))
            return true;
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
    Unbind(wxEVT_KEY_DOWN, &clTabCtrl::OnWindowKeyDown, this);
    Unbind(wxEVT_LEFT_DCLICK, &clTabCtrl::OnLeftDClick, this);
    Unbind(wxEVT_MOUSEWHEEL, &clTabCtrl::OnMouseScroll, this);
    GetParent()->Unbind(wxEVT_KEY_DOWN, &clTabCtrl::OnWindowKeyDown, this);
}

void clTabCtrl::OnWindowKeyDown(wxKeyEvent& event)
{
    if(GetStyle() & kNotebook_EnableNavigationEvent) {
#ifdef __WXOSX__
        if(event.AltDown())
#else
        if(event.CmdDown())
#endif
        {
            switch(event.GetUnicodeKey()) {
            case WXK_TAB:
            case WXK_PAGEDOWN:
            case WXK_PAGEUP: {
#if CL_BUILD
                CL_DEBUG("Firing navigation event");
#endif
                // Fire the navigation event
                wxBookCtrlEvent e(wxEVT_BOOK_NAVIGATING);
                e.SetEventObject(GetParent());
                GetParent()->GetEventHandler()->AddPendingEvent(e);
                return;
            }
            default:
                break;
            }
        }
    }
    event.Skip();
}

void clTabCtrl::OnSize(wxSizeEvent& event)
{
    event.Skip();
    m_visibleTabs.clear();
    Refresh();
}

void clTabCtrl::OnEraseBG(wxEraseEvent& e) { wxUnusedVar(e); }

void clTabCtrl::OnPaint(wxPaintEvent& e)
{
    wxAutoBufferedPaintDC dc(this);
    PrepareDC(dc);

    wxRect clientRect(GetClientRect());
    if(clientRect.width <= 3) return;
    if(clientRect.height <= 3) return;

    m_chevronRect = wxRect();
    wxRect rect(GetClientRect());

    if((GetStyle() & kNotebook_ShowFileListButton)) {
        if(IsVerticalTabs()) {
            int width = rect.GetWidth() > CHEVRON_SIZE ? CHEVRON_SIZE : rect.GetWidth();
            int x = (rect.GetWidth() - width) / 2;
            wxPoint topLeft = rect.GetTopLeft();
            topLeft.x = x;
            m_chevronRect = wxRect(topLeft, wxSize(width, CHEVRON_SIZE));
            rect.y = m_chevronRect.GetBottomLeft().y;
            rect.SetHeight(rect.GetHeight() - m_chevronRect.GetHeight());
        } else {
            wxPoint rightPoint = rect.GetRightTop();
            rightPoint.x -= CHEVRON_SIZE;
            m_chevronRect = wxRect(rightPoint, wxSize(CHEVRON_SIZE, rect.GetHeight()));
            rect.SetWidth(rect.GetWidth() - CHEVRON_SIZE);
        }
    }

    if(m_tabs.empty()) {
        // Draw the default bg colour
        dc.SetPen(DrawingUtils::GetPanelBgColour());
        dc.SetBrush(DrawingUtils::GetPanelBgColour());
        dc.DrawRectangle(GetClientRect());
        return;
    }

    // Draw the tab area background colours
    clTabInfo::Ptr_t active_tab = GetActiveTabInfo();
    wxColour tabAreaBgCol = m_colours.tabAreaColour;
    clTabColours activeTabColours = m_colours;

#if CL_BUILD
    if(active_tab && (GetStyle() & kNotebook_EnableColourCustomization)) {
        // the background colour is set according to the active tab colour
        clColourEvent colourEvent(wxEVT_COLOUR_TAB);
        colourEvent.SetPage(active_tab->GetWindow());
        if(EventNotifier::Get()->ProcessEvent(colourEvent)) {
            tabAreaBgCol = colourEvent.GetBgColour();
            activeTabColours.InitFromColours(colourEvent.GetBgColour(), colourEvent.GetFgColour());
        }
    }
#endif

    // Draw background
    dc.SetPen(tabAreaBgCol);
    dc.SetBrush(tabAreaBgCol);
#ifdef __WXOSX__
    clientRect.Inflate(1, 1);
#endif
    dc.DrawRectangle(clientRect);

    for(size_t i = 0; i < m_tabs.size(); ++i) {
        m_tabs.at(i)->CalculateOffsets(GetStyle());
    }

    if(rect.GetSize().x > 0 && rect.GetSize().y > 0) {
        wxGCDC gcdc(dc);
        PrepareDC(gcdc);

        if(!IsVerticalTabs()) {
            gcdc.SetClippingRegion(clientRect.x, clientRect.y, clientRect.width - CHEVRON_SIZE, clientRect.height);
        }
        gcdc.SetPen(tabAreaBgCol);
        gcdc.SetBrush(tabAreaBgCol);
        gcdc.DrawRectangle(rect.GetSize());
        UpdateVisibleTabs();

        int activeTabInex = wxNOT_FOUND;
        for(int i = (m_visibleTabs.size() - 1); i >= 0; --i) {
            clTabInfo::Ptr_t tab = m_visibleTabs.at(i);
            if(tab->IsActive()) { activeTabInex = i; }

#if CL_BUILD
            // send event per tab to get their colours
            clColourEvent colourEvent(wxEVT_COLOUR_TAB);
            colourEvent.SetPage(tab->GetWindow());
            clTabColours* pColours = &m_colours;
            clTabColours user_colours;
            if((GetStyle() & kNotebook_EnableColourCustomization) && EventNotifier::Get()->ProcessEvent(colourEvent)) {
                user_colours.InitFromColours(colourEvent.GetBgColour(), colourEvent.GetFgColour());
                pColours = &user_colours;
            }

#ifdef __WXGTK__
            m_art->Draw(this, gcdc, dc, *tab.get(), (*pColours), m_style);
#else
            m_art->Draw(this, gcdc, gcdc, *tab.get(), (*pColours), m_style);
#endif
#else
            // Under GTK there is a problem with HiDPI screens and wxGCDC for drawing text
            // the text is rendered too small. Use the wxPaintDC instead of the wxGCDC just for
            // drawing text
#ifdef __WXGTK__
            m_art->Draw(this, gcdc, dc, *tab.get(), m_colours, m_style);
#else
            m_art->Draw(this, gcdc, gcdc, *tab.get(), m_colours, m_style);
#endif
#endif
        }

        // Redraw the active tab
        if(activeTabInex != wxNOT_FOUND) {
            clTabInfo::Ptr_t activeTab = m_visibleTabs.at(activeTabInex);
#ifdef __WXGTK__
            m_art->Draw(this, gcdc, dc, *activeTab.get(), activeTabColours, m_style);
#else
            m_art->Draw(this, gcdc, gcdc, *activeTab.get(), activeTabColours, m_style);
#endif
        }
        if(!IsVerticalTabs()) { gcdc.DestroyClippingRegion(); }
        if(activeTabInex != wxNOT_FOUND) {
            clTabInfo::Ptr_t activeTab = m_visibleTabs.at(activeTabInex);
            if(!(GetStyle() & kNotebook_VerticalButtons)) {
                DoDrawBottomBox(activeTab, clientRect, gcdc, activeTabColours);
            }
        }

        if((GetStyle() & kNotebook_ShowFileListButton)) {
            // Draw the chevron
            gcdc.SetPen(m_colours.inactiveTabPenColour);
            gcdc.DrawLine(m_chevronRect.GetTopLeft(), m_chevronRect.GetTopRight());
            m_art->DrawChevron(this, gcdc, m_chevronRect, m_colours);
        }

    } else {
        m_visibleTabs.clear();
    }
}

void clTabCtrl::DoUpdateCoordiantes(clTabInfo::Vec_t& tabs)
{
    int majorDimension = GetArt()->majorCurveWidth ? 5 : 0;
    if(IsVerticalTabs()) { majorDimension = (GetStyle() & kNotebook_ShowFileListButton) ? 20 : 0; }

    for(size_t i = 0; i < tabs.size(); ++i) {
        clTabInfo::Ptr_t tab = tabs.at(i);
        if(IsVerticalTabs()) {
            if(GetStyle() & kNotebook_VerticalButtons) {
                tab->GetRect().SetX(0);
            } else {
                tab->GetRect().SetX(GetStyle() & kNotebook_LeftTabs ? 1 : 0);
            }
            tab->GetRect().SetY(majorDimension);
            tab->GetRect().SetWidth(tab->GetWidth());
            tab->GetRect().SetHeight(tab->GetHeight());
            majorDimension += tab->GetHeight() - GetArt()->verticalOverlapWidth;
        } else {
            tab->GetRect().SetX(majorDimension);
            tab->GetRect().SetY(0);
            tab->GetRect().SetWidth(tab->GetWidth());
            tab->GetRect().SetHeight(tab->GetHeight());
            majorDimension += tab->GetWidth() - GetArt()->overlapWidth;
        }
    }
}

void clTabCtrl::UpdateVisibleTabs()
{
    // don't update the list if we don't need to
    if(!IsVerticalTabs()) {
        if(IsActiveTabInList(m_visibleTabs) && IsActiveTabVisible(m_visibleTabs)) return;
    }

    // set the physical coords for each tab (we do this for all the tabs)
    DoUpdateCoordiantes(m_tabs);

    // Start shifting right tabs until the active tab is visible
    m_visibleTabs = m_tabs;
    if(!IsVerticalTabs()) {
        while(!IsActiveTabVisible(m_visibleTabs)) {
            if(!ShiftRight(m_visibleTabs)) break;
        }
    } else {
        while(!IsActiveTabVisible(m_visibleTabs)) {
            if(!ShiftBottom(m_visibleTabs)) break;
        }
    }
}

void clTabCtrl::OnLeftDown(wxMouseEvent& event)
{
    event.Skip();
    m_closeButtonClickedIndex = wxNOT_FOUND;

    if((GetStyle() & kNotebook_ShowFileListButton) && m_chevronRect.Contains(event.GetPosition())) {
        // we will handle this later in the "Mouse Up" event
        return;
    }

    int tabHit, realPos;
    eDirection align;
    TestPoint(event.GetPosition(), realPos, tabHit, align);
    if(tabHit == wxNOT_FOUND) return;

    // Did we hit the active tab?
    bool clickWasOnActiveTab = (GetSelection() == realPos);

    // If the click was not on the active tab, set the clicked
    // tab as the new selection and leave this function
    if(!clickWasOnActiveTab) {
        SetSelection(realPos);
        return;
    }

    // If we clicked on the active and we have a close button - handle it here
    if((GetStyle() & kNotebook_CloseButtonOnActiveTab) && clickWasOnActiveTab) {
        // we clicked on the selected index
        clTabInfo::Ptr_t t = m_visibleTabs.at(tabHit);
        wxRect xRect(t->GetRect().x + t->GetBmpCloseX(), t->GetRect().y + t->GetBmpCloseY(), 16, 16);
        if(xRect.Contains(event.GetPosition())) {
            m_closeButtonClickedIndex = tabHit;
            return;
        }
    }

    // We clicked on the active tab, start DnD operation
    if((m_style & kNotebook_AllowDnD) && clickWasOnActiveTab) {
        // We simply drag the active tab index
        wxString dragText;
        dragText << "{Class:Notebook,TabIndex:" << GetSelection() << "}{";
#if CL_BUILD
        IEditor* activeEditor = clGetManager()->GetActiveEditor();
        wxWindow* activePage = NULL;
        if(GetSelection() != wxNOT_FOUND) { activePage = GetPage(GetSelection()); }

        if(activeEditor && ((void*)activeEditor->GetCtrl() == (void*)activePage)) {
            // The current Notebook is the main editor
            dragText << activeEditor->GetFileName().GetFullPath();
        }
        dragText << "}";
#endif
        wxTextDataObject dragContent(dragText);
        wxDropSource dragSource(this);
        dragSource.SetData(dragContent);
        wxDragResult result = dragSource.DoDragDrop(true);
        wxUnusedVar(result);
    }
}

int clTabCtrl::ChangeSelection(size_t tabIdx)
{
    wxWindowUpdateLocker locker(GetParent());
    int oldSelection = GetSelection();
    if(!IsIndexValid(tabIdx)) return oldSelection;

    for(size_t i = 0; i < m_tabs.size(); ++i) {
        clTabInfo::Ptr_t tab = m_tabs.at(i);
        tab->SetActive((i == tabIdx), GetStyle());
    }

    clTabInfo::Ptr_t activeTab = GetActiveTabInfo();
    if(activeTab) {
        static_cast<Notebook*>(GetParent())->DoChangeSelection(activeTab->GetWindow());
        activeTab->GetWindow()->CallAfter(&wxWindow::SetFocus);
    }

    Refresh();
    return oldSelection;
}

int clTabCtrl::SetSelection(size_t tabIdx)
{
#ifdef __WXMSW__
    DoChangeSelection(tabIdx);
#else
    CallAfter(&clTabCtrl::DoChangeSelection, tabIdx);
#endif
    return wxNOT_FOUND;
}

int clTabCtrl::GetSelection() const
{
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        clTabInfo::Ptr_t tab = m_tabs.at(i);
        if(tab->IsActive()) return i;
    }
    return wxNOT_FOUND;
}

clTabInfo::Ptr_t clTabCtrl::GetTabInfo(size_t index)
{
    if(!IsIndexValid(index)) return clTabInfo::Ptr_t(NULL);
    return m_tabs.at(index);
}

clTabInfo::Ptr_t clTabCtrl::GetTabInfo(size_t index) const
{
    if(!IsIndexValid(index)) return clTabInfo::Ptr_t(NULL);
    return m_tabs.at(index);
}

clTabInfo::Ptr_t clTabCtrl::GetTabInfo(wxWindow* page)
{
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        clTabInfo::Ptr_t tab = m_tabs.at(i);
        if(tab->GetWindow() == page) return tab;
    }
    return clTabInfo::Ptr_t(NULL);
}

bool clTabCtrl::SetPageText(size_t page, const wxString& text)
{
    clTabInfo::Ptr_t tab = GetTabInfo(page);
    if(!tab) return false;

    int oldWidth = tab->GetWidth();
    tab->SetLabel(text, GetStyle());
    int newWidth = tab->GetWidth();
    int diff = (newWidth - oldWidth);

    // Update the width of the tabs from the current tab by "diff"
    DoUpdateXCoordFromPage(tab->GetWindow(), diff);

    // Redraw the tab control
    Refresh();
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
        if(m_tabs.at(i)->IsActive()) { return m_tabs.at(i); }
    }
    return clTabInfo::Ptr_t(NULL);
}

void clTabCtrl::AddPage(clTabInfo::Ptr_t tab) { InsertPage(m_tabs.size(), tab); }

WindowStack* clTabCtrl::GetStack() { return reinterpret_cast<Notebook*>(GetParent())->m_windows; }

bool clTabCtrl::InsertPage(size_t index, clTabInfo::Ptr_t tab)
{
    int oldSelection = GetSelection();
    if(index > m_tabs.size()) return false;
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

    tab->GetWindow()->Bind(wxEVT_KEY_DOWN, &clTabCtrl::OnWindowKeyDown, this);
    m_history->Push(tab->GetWindow());
    Refresh();
    return true;
}

wxString clTabCtrl::GetPageText(size_t page) const
{
    clTabInfo::Ptr_t tab = GetTabInfo(page);
    if(tab) return tab->GetLabel();
    return "";
}

wxBitmap clTabCtrl::GetPageBitmap(size_t index) const
{
    clTabInfo::Ptr_t tab = GetTabInfo(index);
    if(tab) return tab->GetBitmap();
    return wxNullBitmap;
}

void clTabCtrl::SetPageBitmap(size_t index, const wxBitmap& bmp)
{
    clTabInfo::Ptr_t tab = GetTabInfo(index);
    if(!tab) return;

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

    // First check if the chevron was clicked. We do this because the chevron could overlap the buttons drawing
    // area
    if((GetStyle() & kNotebook_ShowFileListButton) && m_chevronRect.Contains(event.GetPosition())) {
        // Show the drop down list
        CallAfter(&clTabCtrl::DoShowTabList);

    } else {
        int tabHit, realPos;
        eDirection align;
        TestPoint(event.GetPosition(), realPos, tabHit, align);
        if(tabHit != wxNOT_FOUND) {
            if((GetStyle() & kNotebook_CloseButtonOnActiveTab) && m_visibleTabs.at(tabHit)->IsActive()) {
                // we clicked on the selected index
                clTabInfo::Ptr_t t = m_visibleTabs.at(tabHit);
                wxRect xRect(t->GetRect().x + t->GetBmpCloseX(), t->GetRect().y + t->GetBmpCloseY(), 16, 16);
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
}

void clTabCtrl::OnMouseMotion(wxMouseEvent& event)
{
    event.Skip();
    int realPos, tabHit;
    wxString curtip = GetToolTipText();
    eDirection align;
    TestPoint(event.GetPosition(), realPos, tabHit, align);
    if(tabHit == wxNOT_FOUND || realPos == wxNOT_FOUND) {
        if(!curtip.IsEmpty()) { SetToolTip(""); }
    } else {
        wxString pagetip = m_tabs.at(realPos)->GetTooltip();
        if(pagetip != curtip) { SetToolTip(pagetip); }
    }
}

void clTabCtrl::TestPoint(const wxPoint& pt, int& realPosition, int& tabHit, eDirection& align)
{
    realPosition = wxNOT_FOUND;
    tabHit = wxNOT_FOUND;
    align = eDirection::kInvalid;

    if(m_visibleTabs.empty()) return;

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
            if(IsVerticalTabs()) {
                if(pt.y > ((r.GetHeight() / 2) + r.GetY())) {
                    // the point is on the RIGHT side
                    align = eDirection::kUp;
                } else {
                    align = eDirection::kDown;
                }
            } else {
                if(pt.x > ((r.GetWidth() / 2) + r.GetX())) {
                    // the point is on the RIGHT side
                    align = eDirection::kRight;
                } else {
                    align = eDirection::kLeft;
                }
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
        SetSizeHints(wxSize(m_vTabsWidth, -1));
        SetSize(m_vTabsWidth, -1);
    } else {
        SetSizeHints(wxSize(-1, m_height));
        SetSize(-1, m_height);
    }

    if(style & kNotebook_DarkTabs) {
        m_colours.InitDarkColours();
        //    if(dynamic_cast<clTabRendererClassic*>(m_art.get())) { clTabRendererClassic::InitDarkColours(m_colours); }
    } else {
        m_colours.InitLightColours();
        //    if(dynamic_cast<clTabRendererClassic*>(m_art.get())) { clTabRendererClassic::InitLightColours(m_colours);
        //    }
    }

    for(size_t i = 0; i < m_tabs.size(); ++i) {
        m_tabs.at(i)->CalculateOffsets(GetStyle());
    }

#if CL_BUILD
    if(m_style & kNotebook_DynamicColours) {
        wxString globalTheme = ColoursAndFontsManager::Get().GetGlobalTheme();
        if(!globalTheme.IsEmpty()) {
            LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++", globalTheme);
            if(lexer && lexer->IsDark()) {
                // Dark theme, update all the colours
                m_colours.activeTabBgColour = lexer->GetProperty(0).GetBgColour();
                m_colours.activeTabInnerPenColour = m_colours.activeTabBgColour;
                m_colours.activeTabPenColour = m_colours.activeTabBgColour.ChangeLightness(110);
                m_colours.activeTabTextColour = *wxWHITE;
            } else if(lexer) {
                // Light theme
                m_colours.activeTabBgColour = lexer->GetProperty(0).GetBgColour();
                m_colours.activeTabInnerPenColour = m_colours.activeTabBgColour;
                m_colours.activeTabTextColour = *wxBLACK;
            }
        }
    }
#endif

    m_visibleTabs.clear();
    Layout();
    Refresh();
}

wxWindow* clTabCtrl::GetPage(size_t index) const
{
    clTabInfo::Ptr_t tab = GetTabInfo(index);
    if(tab) return tab->GetWindow();
    return NULL;
}

bool clTabCtrl::IsIndexValid(size_t index) const { return (index < m_tabs.size()); }

int clTabCtrl::FindPage(wxWindow* page) const
{
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        if(m_tabs.at(i)->GetWindow() == page) { return i; }
    }
    return wxNOT_FOUND;
}

bool clTabCtrl::RemovePage(size_t page, bool notify, bool deletePage)
{
    wxWindow* nextSelection = NULL;
    if(!IsIndexValid(page)) return false;
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
    tab->GetWindow()->Unbind(wxEVT_KEY_DOWN, &clTabCtrl::OnWindowKeyDown, this);

    // Remove the tabs from the visible tabs list
    clTabInfo::Vec_t::iterator iter = std::find_if(m_visibleTabs.begin(), m_visibleTabs.end(), [&](clTabInfo::Ptr_t t) {
        if(t->GetWindow() == tab->GetWindow()) { return true; }
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

    // Now remove the page from the notebook. We will delete the page
    // ourself, so there is no need to call DeletePage
    GetStack()->Remove(tab->GetWindow());
    if(deletePage) {
        // Destory the page
        tab->GetWindow()->Destroy();
    }

    if(notify) {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSED);
        event.SetEventObject(GetParent());
        event.SetSelection(GetSelection());
        GetParent()->GetEventHandler()->ProcessEvent(event);
        if(!event.IsAllowed()) {
            // Vetoed
            return false;
        }
    }

    // Choose the next page
    if(deletingSelection) {
        // Always make sure we have something to select...
        if(!nextSelection && !m_tabs.empty()) { nextSelection = m_tabs.at(0)->GetWindow(); }

        int nextSel = DoGetPageIndex(nextSelection);
        if(nextSel != wxNOT_FOUND) {
            ChangeSelection(nextSel);
            if(notify) {
                wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGED);
                event.SetEventObject(GetParent());
                event.SetSelection(GetSelection());
                GetParent()->GetEventHandler()->ProcessEvent(event);
            }
        } else {
            Refresh();
        }
    } else {
        Refresh();
    }
    return true;
}

int clTabCtrl::DoGetPageIndex(wxWindow* win) const
{
    if(!win) return wxNOT_FOUND;
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        if(m_tabs.at(i)->GetWindow() == win) return i;
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
        if(realPos != wxNOT_FOUND) { CallAfter(&clTabCtrl::DoDeletePage, realPos); }
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
            menuEvent.SetEventObject(this);
            menuEvent.SetSelection(realPos);
            GetParent()->GetEventHandler()->ProcessEvent(menuEvent);
        }
    }
}

void clTabCtrl::DoShowTabList()
{
    if(m_tabs.empty()) return;

    int curselection = GetSelection();
    wxMenu menu;
    const int firstTabPageID = 13457;
    int pageMenuID = firstTabPageID;
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        clTabInfo::Ptr_t tab = m_tabs.at(i);
        wxMenuItem* item = new wxMenuItem(&menu, pageMenuID, tab->GetLabel(), "", wxITEM_CHECK);
        menu.Append(item);
        item->Check(tab->IsActive());
        pageMenuID++;
    }

    int selection = GetPopupMenuSelectionFromUser(menu, m_chevronRect.GetBottomLeft());
    if(selection != wxID_NONE) {
        selection -= firstTabPageID;
        // don't change the selection unless the selection is really changing
        if(curselection != selection) { SetSelection(selection); }
    }
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
        if(m_tabs.at(i)->GetLabel() == label) return i;
    }
    return wxNOT_FOUND;
}

void clTabCtrl::DoChangeSelection(size_t index)
{
    // sanity
    if(index >= m_tabs.size()) return;

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

    // Keep this page
    m_history->Push(GetPage(index));

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
    if(newIndex == wxNOT_FOUND) return false;
    if(activeTabInex == wxNOT_FOUND) return false;
    if((newIndex < 0) || (newIndex >= (int)m_tabs.size())) return false;

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

    if(!movingTab) return false;

    // Step 1:
    // Remove the tab from both the active and from the visible tabs array
    clTabInfo::Vec_t::iterator iter = std::find_if(m_visibleTabs.begin(), m_visibleTabs.end(), [&](clTabInfo::Ptr_t t) {
        if(t->GetWindow() == movingTab->GetWindow()) { return true; }
        return false;
    });
    if(iter != m_visibleTabs.end()) { m_visibleTabs.erase(iter); }
    iter = std::find_if(m_tabs.begin(), m_tabs.end(), [&](clTabInfo::Ptr_t t) {
        if(t->GetWindow() == movingTab->GetWindow()) { return true; }
        return false;
    });
    if(iter != m_tabs.end()) { m_tabs.erase(iter); }

    // Step 2:
    // Insert 'tab' in its new position (in both arrays)
    iter = std::find_if(m_tabs.begin(), m_tabs.end(), [&](clTabInfo::Ptr_t t) {
        if(t->GetWindow() == insertBeforeTab->GetWindow()) { return true; }
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
            if(t->GetWindow() == insertBeforeTab->GetWindow()) { return true; }
            return false;
        });
        ++iter;
        if(iter != m_visibleTabs.end()) {
            m_visibleTabs.insert(iter, movingTab);
        } else {
            m_visibleTabs.push_back(movingTab);
        }
    } else {
        if(iter != m_tabs.end()) { m_tabs.insert(iter, movingTab); }

        iter = std::find_if(m_visibleTabs.begin(), m_visibleTabs.end(), [&](clTabInfo::Ptr_t t) {
            if(t->GetWindow() == insertBeforeTab->GetWindow()) { return true; }
            return false;
        });
        if(iter != m_visibleTabs.end()) { m_visibleTabs.insert(iter, movingTab); }
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
        wxBookCtrlEvent e(wxEVT_BOOK_TABAREA_DCLICKED);
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

bool clTabCtrl::IsVerticalTabs() const { return (m_style & kNotebook_RightTabs) || (m_style & kNotebook_LeftTabs); }

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
    if((m_style & kNotebook_DarkTabs)) {
        m_colours.InitDarkColours();
    } else {
        m_colours.InitLightColours();
    }
    DoSetBestSize();
    Refresh();
}

void clTabCtrl::OnMouseScroll(wxMouseEvent& event)
{
    event.Skip();
    if(GetStyle() & kNotebook_MouseScrollSwitchTabs) {
        size_t curSelection = GetSelection();
        if(event.GetWheelRotation() > 0) {
            if(curSelection > 0) { SetSelection(curSelection - 1); }
        } else {
            if(curSelection < GetTabs().size()) { SetSelection(curSelection + 1); }
        }
    }
}

// ---------------------------------------------------------------------------
// DnD
// ---------------------------------------------------------------------------

clTabCtrlDropTarget::clTabCtrlDropTarget(clTabCtrl* tabCtrl)
    : m_tabCtrl(tabCtrl)
{
}

clTabCtrlDropTarget::~clTabCtrlDropTarget() {}

bool clTabCtrlDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data)
{
    // Extract the content dragged using regular expression
    static wxRegEx re("\\{Class:Notebook,TabIndex:([0-9]+)\\}\\{.*?\\}", wxRE_ADVANCED);
    if(!re.Matches(data)) return false;

    wxString tabIndex = re.GetMatch(data, 1);
    long nTabIndex = wxNOT_FOUND;
    tabIndex.ToCLong(&nTabIndex);
    // Sanity
    if(nTabIndex == wxNOT_FOUND) return false;

    // Test the drop tab index
    int realPos, tabHit;
    eDirection align;
    m_tabCtrl->TestPoint(wxPoint(x, y), realPos, tabHit, align);

    // if the tab being dragged and the one we drop it on are the same
    // return false
    if(nTabIndex == realPos) return false;
    m_tabCtrl->MoveActiveToIndex(realPos, align);
    return true;
}

#endif // USE_AUI_NOTEBOOK
