#include "clAuiBook.hpp"

#include <vector>

clAuiBook::clAuiBook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxAuiNotebook(parent, id, pos, size, style)
{
    m_history.reset(new clTabHistory());
    Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGING, &clAuiBook::OnPageChanging, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &clAuiBook::OnPageChanged, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &clAuiBook::OnPageClosing, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSED, &clAuiBook::OnPageClosed, this);
    Bind(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, &clAuiBook::OnPageRightDown, this);

    wxFont default_font = DrawingUtils::GetDefaultGuiFont();
    SetFont(default_font);
    SetMeasuringFont(default_font);
    SetNormalFont(default_font);
    SetSelectedFont(default_font);
}

clAuiBook::~clAuiBook()
{
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CHANGING, &clAuiBook::OnPageChanging, this);
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &clAuiBook::OnPageChanged, this);
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &clAuiBook::OnPageClosing, this);
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CLOSED, &clAuiBook::OnPageClosed, this);
    Unbind(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, &clAuiBook::OnPageRightDown, this);
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

void clAuiBook::MoveActivePage(int newIndex)
{
    int cursel = GetSelection();
    CHECK_COND_RET(cursel != wxNOT_FOUND);

    wxUnusedVar(newIndex);
}

int clAuiBook::SetSelection(size_t newPage)
{
    size_t res = wxAuiNotebook::SetSelection(newPage);
    m_history->Push(GetCurrentPage());
    return res;
}

int clAuiBook::ChangeSelection(size_t n)
{
    size_t res = wxAuiNotebook::ChangeSelection(n);
    m_history->Push(GetCurrentPage());
    return res;
}

wxBorder clAuiBook::GetDefaultBorder() const { return wxBORDER_NONE; }

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
    GetEventHandler()->ProcessEvent(event_closed);
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
    GetEventHandler()->ProcessEvent(changed_event);
}

void clAuiBook::OnPageChanging(wxAuiNotebookEvent& event)
{
    // wrap this event with our own event
    wxBookCtrlEvent event_changing(wxEVT_BOOK_PAGE_CHANGING);
    event_changing.SetEventObject(this);
    event_changing.SetSelection(event.GetSelection());
    event_changing.SetOldSelection(GetSelection());
    GetEventHandler()->ProcessEvent(event_changing);
    if (!event_changing.IsAllowed()) {
        // Vetoed by the user, veto the original event and return
        event.Veto();
        return;
    }
    // Allow changing
    event.Skip();
}

void clAuiBook::OnPageRightDown(wxAuiNotebookEvent& event)
{
    wxBookCtrlEvent menuEvent(wxEVT_BOOK_TAB_CONTEXT_MENU);
    menuEvent.SetEventObject(this);
    menuEvent.SetSelection(GetSelection());
    GetEventHandler()->ProcessEvent(menuEvent);
}

void clAuiBook::OnPageClosing(wxAuiNotebookEvent& event)
{
    wxBookCtrlEvent eventClosing(wxEVT_BOOK_PAGE_CLOSING);
    eventClosing.SetEventObject(this);
    eventClosing.SetSelection(GetSelection());
    GetEventHandler()->ProcessEvent(eventClosing);
    if (!eventClosing.IsAllowed()) {
        // Vetoed
        event.Veto();
        return;
    }
    event.Skip();
}
