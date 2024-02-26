#include "Notebook.h"

wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CHANGING, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CHANGED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CLOSING, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CLOSED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CLOSE_BUTTON, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_TAB_DCLICKED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_NEW_PAGE, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_TAB_CONTEXT_MENU, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_FILELIST_BUTTON_CLICKED, clContextMenuEvent);

clAuiBook::clAuiBook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxAuiNotebook(parent, id, pos, size, style)
{
    m_history.reset(new clTabHistory());
    Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &clAuiBook::OnPageChanged, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSED, &clAuiBook::OnPageClosed, this);
}

clAuiBook::~clAuiBook()
{
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &clAuiBook::OnPageChanged, this);
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CLOSED, &clAuiBook::OnPageClosed, this);
}

size_t clAuiBook::GetAllTabs(clTabInfo::Vec_t& tabs)
{
    size_t count = GetPageCount();
    tabs.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        clTabInfo::Ptr_t info(new clTabInfo(nullptr, 0, GetPage(i), GetPageText(i), wxNOT_FOUND));
        tabs.emplace_back(info);
    }
    return tabs.size();
}

void clAuiBook::OnPageClosed(wxAuiNotebookEvent& event)
{
    event.Skip();
    // Ensure that the history contains only existing tabs
    std::vector<wxWindow*> windows;
    size_t count = GetPageCount();
    windows.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        windows.emplace_back(GetPage(i));
    }
    m_history->Compact(windows);

    wxBookCtrlEvent event_closed(wxEVT_BOOK_PAGE_CLOSED);
    event_closed.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(event_closed);
}

void clAuiBook::OnPageChanged(wxAuiNotebookEvent& event)
{
    // Get the new selection and move it to the top of the hisotry list
    wxWindow* win = GetCurrentPage();
    CHECK_PTR_RET(win);

    m_history->Pop(win);
    m_history->Push(win);

    // Send an event
    wxBookCtrlEvent changed_event(wxEVT_BOOK_PAGE_CHANGED);
    changed_event.SetEventObject(GetParent());
    changed_event.SetSelection(GetSelection());
    GetEventHandler()->AddPendingEvent(changed_event);
}

void clAuiBook::MoveActivePage(int newIndex)
{
    int cursel = GetSelection();
    CHECK_COND_RET(cursel != wxNOT_FOUND);

    wxUnusedVar(newIndex);
}
