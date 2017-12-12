#include "clAuiNotebook.h"

#if USE_AUI_NOTEBOOK
#include "ieditor.h"
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

clAuiNotebook::clAuiNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                             const wxString& name)
    : wxAuiNotebook(parent, id, pos, size, style | wxBORDER_NONE)
{
    Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGING, &clAuiNotebook::OnAuiPageChanging, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &clAuiNotebook::OnAuiPageChanged, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &clAuiNotebook::OnAuiPageClosing, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSED, &clAuiNotebook::OnAuiPageClosed, this);
    Bind(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, &clAuiNotebook::OnAuiTabContextMenu, this);
    Bind(wxEVT_AUINOTEBOOK_BUTTON, &clAuiNotebook::OnTabCloseButton, this);
    Bind(wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN, &clAuiNotebook::OnTabMiddleClicked, this);
    m_history.reset(new clTabHistory());
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
    GetEventHandler()->AddPendingEvent(event);
}

void clAuiNotebook::OnTabMiddleClicked(wxAuiNotebookEvent& evt)
{
    if(GetWindowStyle() & kNotebook_MouseMiddleClickClosesTab) { OnTabCloseButton(evt); }
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
    
    wxWindow* win = GetPage(page);
    bool res = wxAuiNotebook::DeletePage(page);
    if(res) {
        m_history->Pop(win);
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSED);
        event.SetEventObject(this);
        GetEventHandler()->AddPendingEvent(event);
    }
    return res;
}

bool clAuiNotebook::RemovePage(size_t page)
{
    wxWindowUpdateLocker locker(this);
    m_history->Pop(GetPage(page));
    return wxAuiNotebook::RemovePage(page);
}
#endif
