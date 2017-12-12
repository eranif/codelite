#include "clAuiNotebook.h"

#if USE_AUI_NOTEBOOK

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
    : wxAuiNotebook(parent, id, pos, size, style)
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
    event.SetEventObject(GetParent());
    event.SetSelection(evt.GetSelection());
    event.SetOldSelection(evt.GetOldSelection());
    GetParent()->GetEventHandler()->ProcessEvent(event);
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
    event.SetEventObject(GetParent());
    event.SetSelection(evt.GetSelection());
    event.SetOldSelection(evt.GetOldSelection());
    GetParent()->GetEventHandler()->AddPendingEvent(event);
    m_history->Push(GetPage(evt.GetSelection()));
}

void clAuiNotebook::OnAuiPageClosing(wxAuiNotebookEvent& evt)
{
    evt.Skip();
    wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSING);
    event.SetEventObject(GetParent());
    event.SetSelection(evt.GetSelection());
    GetParent()->GetEventHandler()->ProcessEvent(event);
    if(!event.IsAllowed()) {
        // Vetoed by the user
        evt.Veto();
        return;
    } else {
        m_history->Pop(GetPage(evt.GetSelection()));
    }
}

void clAuiNotebook::OnAuiPageClosed(wxAuiNotebookEvent& evt)
{
    evt.Skip();
    wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSED);
    event.SetEventObject(GetParent());
    GetParent()->GetEventHandler()->AddPendingEvent(event);
}

void clAuiNotebook::OnAuiTabContextMenu(wxAuiNotebookEvent& evt)
{
    evt.Skip();
    wxBookCtrlEvent menuEvent(wxEVT_BOOK_TAB_CONTEXT_MENU);
    menuEvent.SetEventObject(GetParent());
    menuEvent.SetSelection(evt.GetSelection());
    GetParent()->GetEventHandler()->AddPendingEvent(menuEvent);
}

void clAuiNotebook::OnTabCloseButton(wxAuiNotebookEvent& evt)
{
    wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSE_BUTTON);
    event.SetEventObject(GetParent());
    event.SetSelection(evt.GetSelection());
    GetParent()->GetEventHandler()->AddPendingEvent(event);
}

void clAuiNotebook::OnTabMiddleClicked(wxAuiNotebookEvent& evt)
{
    if(GetWindowStyle() & kNotebook_MouseMiddleClickClosesTab) {
        OnTabCloseButton(evt);
    }
}

int clAuiNotebook::GetPageIndex(const wxString& label) const
{
    for(size_t i = 0; i < GetPageCount(); ++i) {
        if(GetPageText(i) == label) {
            return static_cast<int>(i);
        }
    }
    return wxNOT_FOUND;
}

int clAuiNotebook::GetPageIndex(wxWindow* page) const
{
    for(size_t i = 0; i < GetPageCount(); ++i) {
        if(GetPage(i) == page) {
            return static_cast<int>(i);
        }
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
#endif
