//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : notebook_ex.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "cl_defs.h"
#if !CL_USE_NATIVEBOOK

#include <wx/app.h>
#include "cl_aui_notebook_art.h"
#include "drawingutils.h"
#include <wx/xrc/xmlres.h>
#include "editor_config.h"
#include <wx/choicebk.h>
#include <wx/notebook.h>
#include "notebook_ex_nav_dlg.h"
#include "notebook_ex.h"
#include <wx/button.h>
#include "wx/sizer.h"
#include <wx/log.h>
#include <wx/wupdlock.h>
#include <set>

#ifdef __WXMSW__
#    include <wx/button.h>
#    include <wx/imaglist.h>
#    include <wx/image.h>
#elif defined(__WXGTK__)
#    include <wx/imaglist.h>
#endif

const wxEventType wxEVT_COMMAND_BOOK_PAGE_CHANGING        = XRCID("notebook_page_changing");
const wxEventType wxEVT_COMMAND_BOOK_PAGE_CHANGED         = XRCID("notebook_page_changed");
const wxEventType wxEVT_COMMAND_BOOK_PAGE_CLOSING         = XRCID("notebook_page_closing");
const wxEventType wxEVT_COMMAND_BOOK_PAGE_CLOSED          = XRCID("notebook_page_closed");
const wxEventType wxEVT_COMMAND_BOOK_PAGE_MIDDLE_CLICKED  = XRCID("notebook_page_middle_clicked");
const wxEventType wxEVT_COMMAND_BOOK_PAGE_X_CLICKED       = XRCID("notebook_page_x_btn_clicked");
const wxEventType wxEVT_COMMAND_BOOK_BG_DCLICK            = XRCID("notebook_page_bg_dclick");

#define SHOW_POPUP_MENU 1234534

#define X_IMG_NONE    -1
#define X_IMG_NORMAL   0
#define X_IMG_PRESSED  1
#define X_IMG_DISABLED 2

#define CHECK_OWNERSHIP(e) {\
    if(e.GetEventObject() != this) {\
        e.Skip();\
        return;\
    }\
}
    
Notebook::Notebook(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
    : wxAuiNotebook(parent, id, pos, size, style | wxNO_BORDER | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_SPLIT)
    , m_popupWin(NULL)
    , m_contextMenu(NULL)
    , m_style(style)
    , m_leftDownTabIdx(npos)
    , m_notify (true)
{
    Initialize();

    m_leftDownPos = wxPoint();

    // Connect events
    Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED,       wxAuiNotebookEventHandler(Notebook::OnInternalPageChanged),  NULL, this);
    Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING,      wxAuiNotebookEventHandler(Notebook::OnInternalPageChanging), NULL, this);
    Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED,        wxAuiNotebookEventHandler(Notebook::OnInternalPageClosed),   NULL, this);
    Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE,         wxAuiNotebookEventHandler(Notebook::OnInternalPageClosing),  NULL, this);
    Connect(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN,    wxAuiNotebookEventHandler(Notebook::OnTabMiddle),            NULL, this);
    Connect(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN,     wxAuiNotebookEventHandler(Notebook::OnTabRightDown),         NULL, this);
    Connect(wxEVT_COMMAND_AUINOTEBOOK_BUTTON,             wxAuiNotebookEventHandler(Notebook::OnTabButton),            NULL, this);
    Connect(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP,       wxAuiNotebookEventHandler(Notebook::OnTabRightUp),           NULL, this);
    Connect(wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK,          wxAuiNotebookEventHandler(Notebook::OnBgDclick),             NULL, this);
    Connect(wxEVT_COMMAND_AUINOTEBOOK_END_DRAG,           wxAuiNotebookEventHandler(Notebook::OnEndDrag),              NULL, this);

#ifdef __WXMSW__
    Connect(wxEVT_SET_FOCUS,                              wxFocusEventHandler(Notebook::OnFocus),                      NULL, this);
#endif
    Connect(wxEVT_NAVIGATION_KEY,                         wxNavigationKeyEventHandler(Notebook::OnNavigationKey),      NULL, this);
    Connect(SHOW_POPUP_MENU, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Notebook::OnInternalMenu),             NULL, this);

    // On Linux, we disable the transparent drag since it will lead to crash when used with compiz / KDE4
#if defined (__WXGTK__) && defined (__WXDEBUG__)
    m_mgr.SetFlags(wxAUI_MGR_ALLOW_FLOATING|wxAUI_MGR_ALLOW_ACTIVE_PANE|wxAUI_MGR_TRANSPARENT_DRAG|wxAUI_MGR_RECTANGLE_HINT);

#elif defined(__WXGTK__)
    m_mgr.SetFlags((m_mgr.GetFlags() | wxAUI_MGR_VENETIAN_BLINDS_HINT) & ~wxAUI_MGR_TRANSPARENT_HINT);

#endif  //

    // Set the notebook theme
    wxAuiTabArt *artProvider;
    size_t flags = EditorConfigST::Get()->GetOptions()->GetOptions();
    if(flags & OptionsConfig::TabCurved) {
        artProvider = new wxAuiSimpleTabArt;
    } else {
        artProvider = new clAuiGlossyTabArt;
    }
    SetArtProvider(artProvider);
}

Notebook::~Notebook()
{
    Disconnect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED,    wxAuiNotebookEventHandler(Notebook::OnInternalPageChanged),  NULL, this);
    Disconnect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING,   wxAuiNotebookEventHandler(Notebook::OnInternalPageChanging), NULL, this);
    Disconnect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED,     wxAuiNotebookEventHandler(Notebook::OnInternalPageClosed),   NULL, this);
    Disconnect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE,      wxAuiNotebookEventHandler(Notebook::OnInternalPageClosing),  NULL, this);
    Disconnect(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN, wxAuiNotebookEventHandler(Notebook::OnTabMiddle),            NULL, this);
    Disconnect(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN,  wxAuiNotebookEventHandler(Notebook::OnTabRightDown),         NULL, this);
    Disconnect(wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK,       wxAuiNotebookEventHandler(Notebook::OnBgDclick),             NULL, this);
    Disconnect(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP,    wxAuiNotebookEventHandler(Notebook::OnTabRightUp),           NULL, this);
    Disconnect(wxEVT_COMMAND_AUINOTEBOOK_END_DRAG,        wxAuiNotebookEventHandler(Notebook::OnEndDrag),              NULL, this);

    Disconnect(wxEVT_NAVIGATION_KEY,                 wxNavigationKeyEventHandler(Notebook::OnNavigationKey),  NULL, this);
#ifdef __WXMSW__
    Disconnect(wxEVT_SET_FOCUS,                              wxFocusEventHandler(Notebook::OnFocus),                      NULL, this);
#endif
    Disconnect(SHOW_POPUP_MENU, wxEVT_COMMAND_MENU_SELECTED,          wxCommandEventHandler(Notebook::OnInternalMenu),   NULL, this);
}

bool Notebook::AddPage(wxWindow *win, const wxString &text, bool selected, const wxBitmap& bmp)
{
    if(wxAuiNotebook::AddPage(win, text, selected, bmp)) {
        win->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(Notebook::OnKeyDown),  NULL, this);
        PushPageHistory(win);
        return true;
    }
    return false;
}

bool Notebook::InsertPage(size_t index, wxWindow* win, const wxString& text, bool selected, const wxBitmap &bmp)
{
    if(wxAuiNotebook::InsertPage(index, win, text, selected, bmp)) {
        win->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(Notebook::OnKeyDown),  NULL, this);
        PushPageHistory(win);
        return true;
    }
    return false;
}


void Notebook::Initialize()
{
}

void Notebook::SetSelection(size_t page, bool notify)
{
    if (page >= GetPageCount())
        return;

    m_notify = notify;
    wxAuiNotebook::SetSelection(page);
    m_notify = true;

    PushPageHistory(GetPage(page));
}

wxWindow* Notebook::GetPage(size_t page) const
{
    if (page >= GetPageCount())
        return NULL;

    return wxAuiNotebook::GetPage(page);
}

bool Notebook::RemovePage(size_t page, bool notify)
{
    if (notify) {
        //send event to noitfy that the page has changed
        NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CLOSING, GetId());
        event.SetSelection( page );
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent(event);

        if (!event.IsAllowed()) {
            return false;
        }
    }

    wxWindow* win = GetPage(page);
    win->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(Notebook::OnKeyDown),  NULL, this);

    bool rc = wxAuiNotebook::RemovePage(page);
    if (rc) {
        PopPageHistory(win);
    }

    if (rc && notify) {
        //send event to noitfy that the page has been closed
        NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CLOSED, GetId());
        event.SetSelection( page );
        event.SetEventObject( this );
        GetEventHandler()->AddPendingEvent(event);
    }

    return rc;
}

bool Notebook::DeletePage(size_t page, bool notify)
{
    if (page >= GetPageCount())
        return false;

    if (notify) {
        //send event to noitfy that the page has changed
        NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CLOSING, GetId());
        event.SetSelection( page );
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent(event);

        if (!event.IsAllowed()) {
            return false;
        }
    }

    wxWindow* win = GetPage(page);
    win->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(Notebook::OnKeyDown),  NULL, this);

    bool rc = wxAuiNotebook::DeletePage(page);
    if (rc) {
        PopPageHistory(win);
    }

    if (rc && notify) {
        //send event to noitfy that the page has been closed
        NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CLOSED, GetId());
        event.SetSelection( page );
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent(event);
    }

    return rc;

}

wxString Notebook::GetPageText(size_t page) const
{
    if (page >= GetPageCount())
        return wxT("");

    return wxAuiNotebook::GetPageText(page);
}

void Notebook::OnNavigationKey(wxNavigationKeyEvent &e)
{
    if ( e.IsWindowChange() ) {
        if (DoNavigate())
            return;
    }
    e.Skip();
}

const wxArrayPtrVoid& Notebook::GetHistory() const
{
    return m_history;
}

void Notebook::SetRightClickMenu(wxMenu* menu)
{
    m_contextMenu = menu;
}

wxWindow* Notebook::GetCurrentPage() const
{
    size_t selection = GetSelection();
    if (selection != Notebook::npos) {
        return GetPage(selection);
    }
    return NULL;
}

size_t Notebook::GetPageIndex(wxWindow *page) const
{
    if ( !page )
        return Notebook::npos;

    for (size_t i=0; i< GetPageCount(); i++) {
        if (GetPage(i) == page) {
            return i;
        }
    }
    return Notebook::npos;
}

size_t Notebook::GetPageIndex(const wxString& text) const
{
    for (size_t i=0; i< GetPageCount(); i++) {

        if (GetPageText(i) == text) {
            return i;
        }
    }
    return Notebook::npos;
}

// wxAuiNotebook provides an array of its contained pages, accessible with GetPages()
// However it doesn't update this array when the tab-order is altered using D'n'D
// So, if serialise used GetPages(), the user would lose his preferred order of tabs.
// GetEditorsInOrder() works around the problem
void Notebook::GetEditorsInOrder(std::vector<wxWindow*> &editors)
{
    editors.clear();
    if (GetPageCount() == 0) {
        return;
    }
    std::vector<wxWindow*> all_pages;// Use a vector here: it's more hassle than a set, but retains the order

    // wxAuiNotebook stores its wxAuiTabCtrls in an internal class wxTabFrame, defined in the cpp file, and so unavailable
    // The only way I can see to acquire them is to FindTab() on every page to find its wxAuiTabCtrl. Yuck!
    std::set<wxAuiTabCtrl*> ctrls;
    for (size_t pg = 0; pg < GetPageCount(); ++pg) {
        wxAuiTabCtrl* ctrl;
        int ctrl_idx;
        wxWindow* win = GetPage(pg);
        if (win && FindTab(win, &ctrl, &ctrl_idx)) {
            ctrls.insert(ctrl);
            all_pages.push_back(win);	// Store in the all-pages vector: we may need it later
        }
    }

    // For every tabctrl that we found, iterate through its tabs using TabHitTest()
    // This is the only way to get their current order within the tabcrl, afaict
    std::set<wxAuiTabCtrl*>::const_iterator it;
    for (it = ctrls.begin(); it != ctrls.end(); ++it) {
        wxAuiTabCtrl* ctrl = *it;
        // Aim for the centre of the tab ;)
        int y = ctrl->GetRect().height / 2;
        // 2 is arbitrary, to ensure we're within the first tab but shouldn't overshoot
        int x_offset = ctrl->GetArtProvider()->GetIndentSize() + 2;
        wxWindow* previouspage = NULL;

        // The elegant way to do this would be to add the width of each tab to the cumulative width
        // However the value produced by GetArtProvider() is consistently too large,
        // so eventually we overshoot a tab (or the whole ctrl!)
        // Instead, keep hit-testing every 10 x-pixels until the page changes
        do {
            wxWindow* page;
            int x = 0;
            do {
                page = NULL;
                x += 10;	// Another arbitrary value, but it's unlikely that there'll be a tab smaller than this
                bool hit = ctrl->TabHitTest(x_offset+x, y, &page);
                // If there's space between tabs, we might be hittesting this, returning hit==false
                // Therefore ignore misses if we're still within the width of ctrl
                if (!hit && x_offset+x >= ctrl->GetRect().width) {
                    break;
                }
            } while ((!page) || page == previouspage);

            if (!page) {
                // We've reached the end of the tabctrl
                previouspage = NULL;
                break;
            }
            editors.push_back(page);
            // Remove it from the pages-not-yet-found vector
            for (size_t p=0; p < all_pages.size(); ++p) {
                if (page == *(all_pages.begin()+p)) {
                    all_pages.erase(all_pages.begin()+p);
                    break;
                }
                wxCHECK_RET(p+1 < all_pages.size(), wxT("Trying to erase a page not in the vector"));
            }

            x_offset += x;
            previouspage = page;
        } while (x_offset < ctrl->GetRect().width);	// Continue until we reach the end of the visible ctrl
    }

    // If there are pages the hittest missed, they *might* be detached pages, or hitttest failure
    // but they're most likely to be scrolled-off-the-screen tabs.
    // I'd guess these are more likely to be from the beginning of the tabctrl, so prepend them to the vector
    if (!all_pages.empty()) {
        editors.insert(editors.begin(), all_pages.begin(), all_pages.end());
    }
}

size_t Notebook::GetVisibleEditorIndex()
{
    wxAuiTabCtrl* ctrl = GetActiveTabCtrl();
    if (ctrl) {
        return ctrl->GetIdxFromWindow(GetCurrentPage());
    }
    return Notebook::npos;
}

bool Notebook::SetPageText(size_t index, const wxString &text)
{
    if (index >= GetPageCount())
        return false;
    return wxAuiNotebook::SetPageText(index, text);
}

bool Notebook::DeleteAllPages(bool notify)
{
    bool res = true;
    size_t count = GetPageCount();
    for (size_t i=0; i<count && res; i++) {
        res = this->DeletePage(0, notify);

    }
    return res;
}

void Notebook::PushPageHistory(wxWindow *page)
{
    if (page == NULL)
        return;

    int where = m_history.Index(page);
    //remove old entry of this page and re-insert it as first
    if (where != wxNOT_FOUND) {
        m_history.Remove(page);
    }
    m_history.Insert(page, 0);
}

void Notebook::PopPageHistory(wxWindow *page)
{
    if( !page )
        return;

    int where = m_history.Index(page);
    while (where != wxNOT_FOUND) {
        wxWindow *tab = static_cast<wxWindow *>(m_history.Item(where));
        m_history.Remove(tab);

        //remove all appearances of this page
        where = m_history.Index(page);
    }
}

wxWindow* Notebook::GetPreviousSelection()
{
    if (m_history.empty()) {
        return NULL;
    }
    //return the top of the heap
    return static_cast<wxWindow*>( m_history.Item(0));
}

void Notebook::OnInternalPageChanged(wxAuiNotebookEvent &e)
{
    CHECK_OWNERSHIP(e);
    DoPageChangedEvent(e);
}

void Notebook::OnInternalPageChanging(wxAuiNotebookEvent &e)
{
    CHECK_OWNERSHIP(e);
    DoPageChangingEvent(e);
}

void Notebook::DoPageChangedEvent(wxAuiNotebookEvent& e)
{
    CHECK_OWNERSHIP(e);

    if (!m_notify) {
        e.Skip();
        return;
    }

    //send event to noitfy that the page is changing
    NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CHANGED, GetId());
    event.SetSelection   ( e.GetSelection()    );
    event.SetOldSelection( e.GetOldSelection() );
    event.SetEventObject ( this );
    GetEventHandler()->ProcessEvent(event);

    PushPageHistory( GetPage(e.GetSelection()) );
    e.Skip();
}

void Notebook::DoPageChangingEvent(wxAuiNotebookEvent& e)
{
    CHECK_OWNERSHIP(e);

    if (!m_notify) {
        e.Skip();
        return;
    }

    //send event to noitfy that the page is changing
    NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CHANGING, GetId());
    event.SetSelection   ( e.GetSelection()    );
    event.SetOldSelection( e.GetOldSelection() );
    event.SetEventObject ( this );
    GetEventHandler()->ProcessEvent(event);

    if ( !event.IsAllowed() ) {
        e.Veto();
    }
    e.Skip();
}

void Notebook::OnKeyDown(wxKeyEvent& e)
{
    if(e.m_controlDown) {
        switch(e.GetKeyCode()) {
        case WXK_TAB:
            DoNavigate(); // smart tabbing
            return;

        case WXK_PAGEDOWN:
            AdvanceSelection(false);
            return;

        case WXK_PAGEUP:
            AdvanceSelection(true);
            return;

        default:
            break;
        }
    }
    e.Skip();
}

bool Notebook::DoNavigate()
{
    if ( !m_popupWin && GetPageCount() > 1) {

        m_popupWin = new NotebookNavDialog( this );
        m_popupWin->ShowModal();

        wxWindow *page = m_popupWin->GetSelection();
        m_popupWin->Destroy();
        m_popupWin = NULL;

        SetSelection( GetPageIndex(page), true );


        return true;
    }
    return false;
}

void Notebook::OnFocus(wxFocusEvent& e)
{
    e.Skip();
}

void Notebook::OnInternalPageClosing(wxAuiNotebookEvent& e)
{
    CHECK_OWNERSHIP(e);
    e.Skip();

    NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CLOSING, GetId());
    event.SetSelection( (int)GetSelection() );
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent(event);

    if (!event.IsAllowed()) {
        e.Veto();

    } else {
        // This page is likely to be removed, remove it from the history
        PopPageHistory( GetPage( static_cast<size_t>( GetSelection() ) ) );
    }
}

void Notebook::OnInternalPageClosed(wxAuiNotebookEvent& e)
{
    CHECK_OWNERSHIP(e);
    e.Skip();

    NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CLOSED, GetId());
    event.SetSelection( (int) GetSelection() );
    event.SetEventObject( this );
    GetEventHandler()->AddPendingEvent(event);

}

void Notebook::OnTabMiddle(wxAuiNotebookEvent& e)
{
    CHECK_OWNERSHIP(e);
    if (e.GetSelection() != wxNOT_FOUND && HasCloseMiddle()) {
        //send event to noitfy that the page is changing
        NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_MIDDLE_CLICKED, GetId());
        event.SetSelection   ( e.GetSelection() );
        event.SetOldSelection( wxNOT_FOUND );
        event.SetEventObject ( this );
        GetEventHandler()->AddPendingEvent(event);
    }
}

void Notebook::OnTabRightDown(wxAuiNotebookEvent& e)
{
    CHECK_OWNERSHIP(e);
    if(m_contextMenu) {

        int where = e.GetSelection();
        if(where != wxNOT_FOUND && where == static_cast<int>(GetSelection())) {

            PopupMenu(m_contextMenu);

        } else {

            e.Skip();

        }
    } else {

        e.Skip();

    }
}

void Notebook::OnInternalMenu(wxCommandEvent& e)
{
    if(m_contextMenu) {
        PopupMenu(m_contextMenu);
    }
}

void Notebook::OnBgDclick(wxAuiNotebookEvent& e)
{
    CHECK_OWNERSHIP(e);
    NotebookEvent event(wxEVT_COMMAND_BOOK_BG_DCLICK, GetId());
    event.SetSelection   ( wxNOT_FOUND );
    event.SetOldSelection( wxNOT_FOUND );
    event.SetEventObject ( this );
    GetEventHandler()->AddPendingEvent(event);
}

void Notebook::OnTabRightUp(wxAuiNotebookEvent& e)
{
    CHECK_OWNERSHIP(e);
    e.Skip();
}

void Notebook::OnTabButton(wxAuiNotebookEvent& e)
{
    CHECK_OWNERSHIP(e);
    
    // Notebook button was clicked
    NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_X_CLICKED, GetId());
    event.SetSelection   ( GetSelection() );
    event.SetOldSelection( wxNOT_FOUND );
    event.SetEventObject ( this );
    GetEventHandler()->AddPendingEvent(event);
}

std::set<wxAuiTabCtrl*> Notebook::GetAllTabControls()
{
    // The only way I can see to acquire them is to FindTab() on every page to find its wxAuiTabCtrl. Yuck!
    std::set<wxAuiTabCtrl*> ctrls;
    for (size_t pg = 0; pg < GetPageCount(); ++pg) {
        wxAuiTabCtrl* ctrl;
        int ctrl_idx;
        wxWindow* win = GetPage(pg);
        if (win && FindTab(win, &ctrl, &ctrl_idx)) {
            ctrls.insert(ctrl);
        }
    }
    return ctrls;
}

wxArrayString Notebook::GetPagesTextInOrder() const
{
    wxArrayString labels;
    for(size_t i=0; i<GetPageCount(); ++i) {
        labels.Add( GetPageText(i) );
    }
    return labels;
}

void Notebook::OnEndDrag(wxAuiNotebookEvent& event)
{
    event.Skip();
    wxPostEvent(wxTheApp, event); // The Workspace View 'Tabs' pane grabs it from wxTheApp
}

#endif // !CL_USE_NATIVEBOOK
