#include "clAuiNotebook.h"

#if USE_AUI_NOTEBOOK
#include "clAuiMainNotebookTabArt.h"
#include "codelite_events.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "globals.h"
#include "ieditor.h"
#include <wx/aui/tabart.h>
#include <wx/wupdlock.h>

wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CHANGING, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CHANGED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CLOSING, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CLOSED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CLOSE_BUTTON, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_TAB_DCLICKED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_NAVIGATING, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_TABAREA_DCLICKED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_TAB_CONTEXT_MENU, wxBookCtrlEvent);

static int CreateBitmask(int start, int end)
{
    int res = 0;
    for(int i = start; i < end; ++i) {
        res |= (1 << i);
    }
    return res;
}

clAuiNotebook::clAuiNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                             const wxString& name)
{
    // wxWidgets style are from 0->18 (excluding)
    int wxstyle_mask = CreateBitmask(0, 18);
    int custom_mask = CreateBitmask(18, 32);
    
    // The custom flags are from 18->32
    m_customFlags = (style & custom_mask);
    style = (style & wxstyle_mask);
    style |= wxNO_BORDER;
    style &= ~wxAUI_NB_MIDDLE_CLICK_CLOSE;
    
    // Remove the hi-bit styles from the style passed
    wxAuiNotebook::Create(parent, id, pos, size, style);

    Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGING, &clAuiNotebook::OnAuiPageChanging, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &clAuiNotebook::OnAuiPageChanged, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &clAuiNotebook::OnAuiPageClosing, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSED, &clAuiNotebook::OnAuiPageClosed, this);
    Bind(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, &clAuiNotebook::OnAuiTabContextMenu, this);
    Bind(wxEVT_AUINOTEBOOK_BUTTON, &clAuiNotebook::OnTabCloseButton, this);
    Bind(wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN, &clAuiNotebook::OnTabMiddleClicked, this);
    Bind(wxEVT_AUINOTEBOOK_BG_DCLICK, &clAuiNotebook::OnTabBgDClick, this);
    Bind(wxEVT_NAVIGATION_KEY, &clAuiNotebook::OnNavKey, this);

    m_history.reset(new clTabHistory());
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clAuiNotebook::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_SETTINGS_CHANGED, &clAuiNotebook::OnThemeChanged, this);

    // Set the art provider
    size_t options = EditorConfigST::Get()->GetOptions()->GetOptions();
    if(options & OptionsConfig::Opt_TabStyleMinimal) {
        SetArtProvider(new wxAuiDefaultTabArt());
    } else if(options & OptionsConfig::Opt_TabStyleTRAPEZOID) {
        SetArtProvider(new wxAuiSimpleTabArt());
    } else {
        // the default
        clAuiMainNotebookTabArt* art = new clAuiMainNotebookTabArt();
        art->RefreshColours(m_customFlags);
        SetArtProvider(art);
    }
}

clAuiNotebook::~clAuiNotebook()
{
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CHANGING, &clAuiNotebook::OnAuiPageChanging, this);
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &clAuiNotebook::OnAuiPageChanged, this);
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &clAuiNotebook::OnAuiPageClosing, this);
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CLOSED, &clAuiNotebook::OnAuiPageClosed, this);
    Unbind(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, &clAuiNotebook::OnAuiTabContextMenu, this);
    Unbind(wxEVT_AUINOTEBOOK_BUTTON, &clAuiNotebook::OnTabCloseButton, this);
    Unbind(wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN, &clAuiNotebook::OnTabMiddleClicked, this);
    Unbind(wxEVT_AUINOTEBOOK_BG_DCLICK, &clAuiNotebook::OnTabBgDClick, this);
    Unbind(wxEVT_NAVIGATION_KEY, &clAuiNotebook::OnNavKey, this);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clAuiNotebook::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_SETTINGS_CHANGED, &clAuiNotebook::OnThemeChanged, this);
}

void clAuiNotebook::OnAuiPageChanging(wxAuiNotebookEvent& evt)
{
    evt.Skip();
    wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGING);
    event.SetEventObject(this);
    event.SetSelection(evt.GetSelection());
    event.SetOldSelection(evt.GetOldSelection());
    GetEventHandler()->ProcessEvent(event);
    if(!event.IsAllowed()) {
        // Vetoed by the user
        evt.Veto();
        return;
    }
}

void clAuiNotebook::OnAuiPageChanged(wxAuiNotebookEvent& evt)
{
    evt.Skip();
    wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGED);
    event.SetEventObject(this);
    event.SetSelection(evt.GetSelection());
    event.SetOldSelection(evt.GetOldSelection());
    GetEventHandler()->ProcessEvent(event);
    m_history->Push(GetPage(evt.GetSelection()));
}

void clAuiNotebook::OnAuiPageClosing(wxAuiNotebookEvent& evt) { evt.Skip(); }

void clAuiNotebook::OnAuiPageClosed(wxAuiNotebookEvent& evt)
{
    wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSED);
    event.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(event);
}

void clAuiNotebook::OnAuiTabContextMenu(wxAuiNotebookEvent& evt)
{
    wxBookCtrlEvent menuEvent(wxEVT_BOOK_TAB_CONTEXT_MENU);
    menuEvent.SetEventObject(this);
    menuEvent.SetSelection(evt.GetSelection());
    GetEventHandler()->AddPendingEvent(menuEvent);
}

void clAuiNotebook::OnTabCloseButton(wxAuiNotebookEvent& evt)
{
    wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSE_BUTTON);
    event.SetEventObject(this);
    event.SetSelection(evt.GetSelection());
    GetEventHandler()->ProcessEvent(event);
}

void clAuiNotebook::OnTabMiddleClicked(wxAuiNotebookEvent& evt)
{
    if(m_customFlags & kNotebook_MouseMiddleClickClosesTab) { OnTabCloseButton(evt); }
}

int clAuiNotebook::GetPageIndex(const wxString& label) const
{
    for(size_t i = 0; i < GetPageCount(); ++i) {
        if(GetPageText(i) == label) { return static_cast<int>(i); }
    }
    return wxNOT_FOUND;
}

int clAuiNotebook::GetPageIndex(wxWindow* page) const
{
    if(!page) { return wxNOT_FOUND; }
    for(size_t i = 0; i < GetPageCount(); ++i) {
        if(GetPage(i) == page) { return static_cast<int>(i); }
    }
    return wxNOT_FOUND;
}

wxArrayString clAuiNotebook::GetAllTabsLabels()
{
    if(GetPageCount() == 0) { return wxArrayString(); }

    wxArrayString labels;
    labels.Alloc(GetPageCount());
    for(size_t i = 0; i < GetPageCount(); ++i) {
        labels.Add(GetPageText(i));
    }
    return labels;
}

void clAuiNotebook::SetTabDirection(wxDirection d)
{
    if((d == wxBOTTOM) || (d == wxRIGHT)) {
        EnableStyle(kNotebook_BottomTabs, true);
        EnableStyle(kNotebook_TopTabs, false);
    } else {
        EnableStyle(kNotebook_BottomTabs, false);
        EnableStyle(kNotebook_TopTabs, true);
    }
    Refresh();
}

void clAuiNotebook::GetAllTabs(clTab::Vec_t& tabs)
{
    tabs.reserve(GetPageCount());
    for(size_t i = 0; i < GetPageCount(); ++i) {
        wxWindow* win = GetPage(i);
        clTab t;
        t.bitmap = GetPageBitmap(i);
        t.text = GetPageText(i);
        t.window = win;

        IEditor* editor = dynamic_cast<IEditor*>(t.window);
        if(editor) {
            t.isFile = true;
            t.isModified = editor->IsModified();
            t.filename = editor->GetFileName();
        }
        tabs.push_back(t);
    }
}

bool clAuiNotebook::DeletePage(size_t page)
{
    wxWindow* win = GetPage(page);
    // null page?
    if(!win) { return false; }
    wxWindowUpdateLocker locker(this);
    {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSING);
        event.SetEventObject(this);
        event.SetSelection(page);
        GetEventHandler()->ProcessEvent(event);
        if(!event.IsAllowed()) {
            // Vetoed by the user
            return false;
        }
    }

    win->Unbind(wxEVT_NAVIGATION_KEY, &clAuiNotebook::OnNavKey, this);
    m_history->Pop(win);

    // Change the selection before the deletion takes place
    wxWindow* tabToSelect = m_history->PrevPage();
    int newSelection = GetPageIndex(tabToSelect);
    if((newSelection != wxNOT_FOUND) && (tabToSelect != win)) {
        // We need to change the selection
        wxAuiNotebook::SetSelection(newSelection);
    }

    bool res = wxAuiNotebook::DeletePage(page);
    if(res) {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSED);
        event.SetEventObject(this);
        GetEventHandler()->AddPendingEvent(event);
    }
    return res;
}

bool clAuiNotebook::RemovePage(size_t page)
{
    wxWindowUpdateLocker locker(this);
    wxWindow* win = GetPage(page);
    if(win) { win->Unbind(wxEVT_NAVIGATION_KEY, &clAuiNotebook::OnNavKey, this); }
    m_history->Pop(win);
    return wxAuiNotebook::RemovePage(page);
}

void clAuiNotebook::OnThemeChanged(wxCommandEvent& evt)
{
    evt.Skip();
    // Replace the art provider
    if(dynamic_cast<clAuiMainNotebookTabArt*>(GetArtProvider())) {
        clAuiMainNotebookTabArt* art = new clAuiMainNotebookTabArt();
        art->RefreshColours(m_customFlags);
        SetArtProvider(art->Clone());
        Refresh();
    }
}

void clAuiNotebook::OnTabBgDClick(wxAuiNotebookEvent& evt)
{
    wxUnusedVar(evt);
    wxBookCtrlEvent event(wxEVT_BOOK_TABAREA_DCLICKED);
    event.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(event);
}

void clAuiNotebook::OnNavKey(wxNavigationKeyEvent& evt)
{
    wxBookCtrlEvent event(wxEVT_BOOK_NAVIGATING);
    event.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(event);
}

int clAuiNotebook::FindPage(wxWindow* page) const
{
    for(size_t i = 0; i < GetPageCount(); ++i) {
        if(GetPage(i) == page) { return (int)i; }
    }
    return wxNOT_FOUND;
}

bool clAuiNotebook::AddPage(wxWindow* page, const wxString& caption, bool select, const wxBitmap& bitmap)
{
    page->Bind(wxEVT_NAVIGATION_KEY, &clAuiNotebook::OnNavKey, this);
    m_history->Push(page);
    return wxAuiNotebook::AddPage(page, caption, select, bitmap);
}

bool clAuiNotebook::InsertPage(size_t pageIdx, wxWindow* page, const wxString& caption, bool select,
                               const wxBitmap& bitmap)
{
    page->Bind(wxEVT_NAVIGATION_KEY, &clAuiNotebook::OnNavKey, this);
    m_history->Push(page);
    return wxAuiNotebook::InsertPage(pageIdx, page, caption, select, bitmap);
}
#endif
