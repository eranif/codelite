#include "clMultiBook.h"
#include "clTabRendererClassic.h"
#include "clTabRendererCurved.h"
#include "clTabRendererSquare.h"
#include "editor_config.h"
#include "file_logger.h"
#include "macros.h"
#include <algorithm>
#include <wx/app.h>
#include <wx/wupdlock.h>

clMultiBook::clMultiBook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                         const wxString& name)
    : wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL, name)
    , m_style(style)
    , m_selection(wxNOT_FOUND)
{
    wxTheApp->Bind(wxEVT_SET_FOCUS, &clMultiBook::OnFocus, this);
    SetSizer(new wxBoxSizer(wxHORIZONTAL));
    m_history.reset(new clTabHistory());
}

clMultiBook::~clMultiBook() { wxTheApp->Unbind(wxEVT_SET_FOCUS, &clMultiBook::OnFocus, this); }

//-------------------------------------
// Helper functions
//-------------------------------------
bool clMultiBook::GetActiveBook(Notebook** book, size_t& bookIndex) const
{
    if(m_selection == wxNOT_FOUND) return false;
    size_t modPageIndex;
    return GetBookByPageIndex(m_selection, book, bookIndex, modPageIndex);
}

bool clMultiBook::GetBookByPageIndex(size_t pageIndex, Notebook** book, size_t& bookIndex, size_t& modPageIndex) const
{
    for(size_t i = 0; i < m_books.size(); ++i) {
        Notebook* b = m_books[i];
        if(pageIndex < b->GetPageCount()) {
            *book = b;
            bookIndex = i;
            modPageIndex = pageIndex;
            return b;
        }
        pageIndex -= b->GetPageCount();
    }
    return false;
}

bool clMultiBook::GetActivePageBook(Notebook** book, size_t& bookIndex, size_t& modPageIndex) const
{
    return GetBookByPageIndex(m_selection, book, bookIndex, modPageIndex);
}

void clMultiBook::MovePageToNotebook(Notebook* srcbook, size_t index, Notebook* destbook)
{
    if(!srcbook || !destbook) return;
    wxString text = srcbook->GetPageText(index);
    wxBitmap bmp = srcbook->GetPageBitmap(index);
    wxWindow* page = srcbook->GetPage(index);

    srcbook->RemovePage(index, false);
    destbook->AddPage(page, text, true, bmp);
    
    // Make the newly added tab the focused one
    page->CallAfter(&wxWindow::SetFocus);
    
    // Update the view if the source notebook is now empty
    if(srcbook->GetPageCount() == 0) { CallAfter(&clMultiBook::UpdateView); }
}

void clMultiBook::UpdateView()
{
    wxWindowUpdateLocker locker(this);
    std::vector<Notebook*>::iterator iter = m_books.begin();
    while(iter != m_books.end()) {
        Notebook* b = *iter;
        if(b->GetPageCount() == 0) {
            GetSizer()->Detach(b);
            b->Destroy();
            iter = m_books.erase(iter);
        } else {
            ++iter;
        }
    }
    if(GetPageCount() == 0) { m_selection = wxNOT_FOUND; }
    GetSizer()->Layout();
}

int clMultiBook::BookIndexToGlobalIndex(size_t bookIndex, size_t pageIndex) const
{
    // Sanity
    if(bookIndex >= m_books.size()) { return wxNOT_FOUND; }

    int globalIndex = pageIndex;
    for(size_t i = 0; i < bookIndex; ++i) {
        globalIndex += m_books[i]->GetPageCount();
    }
    return globalIndex;
}

int clMultiBook::BookIndexToGlobalIndex(Notebook* book, size_t pageIndex) const
{
    bool found = false;
    int globalIndex = pageIndex;
    for(size_t i = 0; i < m_books.size(); ++i) {
        if(book == m_books[i]) {
            found = true;
            break;
        } else {
            globalIndex += m_books[i]->GetPageCount();
        }
    }

    // Sanity
    if(!found) { return wxNOT_FOUND; }
    return globalIndex;
}

//-----------------------------------------
// API
//-----------------------------------------
bool clMultiBook::CanMoveToTabGroupLeft() const
{
    Notebook* book;
    size_t bookIndex;
    size_t modPageIndex;
    if(!GetActivePageBook(&book, bookIndex, modPageIndex)) { return false; }
    return (bookIndex > 0); // We are not the right most notebook
}

bool clMultiBook::CanMoveToTabGroupRight() const
{
    Notebook* book;
    size_t bookIndex;
    size_t modPageIndex;
    if(!GetActivePageBook(&book, bookIndex, modPageIndex)) { return false; };
    return book->GetPageCount() > 1; // Only allow to move if we have more than one tab in this notebook
}

void clMultiBook::MoveToRightTabGroup()
{
    if(!CanMoveToTabGroupRight()) { return; }
    Notebook* srcBook = nullptr;
    size_t srcBookIndex;
    size_t modPageIndex;
    if(!GetBookByPageIndex(m_selection, &srcBook, srcBookIndex, modPageIndex)) { return; }

    Notebook* destBook = nullptr;
    size_t destBookIndex = (srcBookIndex + 1);
    if(m_books.size() > destBookIndex) {
        destBook = m_books[destBookIndex];
    } else {
        destBook = AddNotebook();
    }
    MovePageToNotebook(srcBook, modPageIndex, destBook);
}

void clMultiBook::MoveToLeftTabGroup()
{
    if(!CanMoveToTabGroupLeft()) { return; }
    Notebook* srcBook = nullptr;
    size_t srcBookIndex;
    size_t modPageIndex;
    if(!GetBookByPageIndex(m_selection, &srcBook, srcBookIndex, modPageIndex)) { return; }

    Notebook* destBook = nullptr;
    int destBookIndex = (srcBookIndex - 1);
    if(destBookIndex < 0) { return; }
    destBook = m_books[destBookIndex];
    MovePageToNotebook(srcBook, modPageIndex, destBook);
}

void clMultiBook::AddPage(wxWindow* page, const wxString& label, bool selected, const wxBitmap& bmp)
{
    if(m_books.empty()) { AddNotebook(); }
    m_books[0]->AddPage(page, label, selected, bmp);
    m_history->Push(page);
}

bool clMultiBook::InsertPage(size_t index, wxWindow* page, const wxString& label, bool selected, const wxBitmap& bmp)
{
    if(m_books.empty()) {
        AddPage(page, label, selected, bmp);
        return true;

    } else {
        Notebook* b;
        size_t modindex;
        size_t bookindex;
        if(GetBookByPageIndex(index, &b, bookindex, modindex)) {
            bool res = b->InsertPage(modindex, page, label, selected, bmp);
            if(res) { m_history->Push(page); }
            return res;
        } else {
            AddPage(page, label, selected, bmp);
            return true;
        }
    }
    return false;
}

wxWindow* clMultiBook::GetPage(size_t index) const
{
    Notebook* book;
    size_t modIndex;
    size_t bookIndex;
    if(!GetBookByPageIndex(index, &book, bookIndex, modIndex)) { return nullptr; }
    return book->GetPage(modIndex);
}

bool clMultiBook::DeletePage(size_t page, bool notify)
{
    Notebook* book;
    size_t modIndex;
    size_t bookIndex;
    if(!GetBookByPageIndex(page, &book, bookIndex, modIndex)) { return false; }

    // Update the history
    wxWindow* pageToDelete = book->GetPage(modIndex);
    m_history->Pop(pageToDelete);

    // Delete the page
    bool res = book->DeletePage(modIndex, notify);
    UpdateView();
    return res;
}

wxWindow* clMultiBook::GetCurrentPage() const
{
    Notebook* book;
    size_t bookIndex;
    if(GetActiveBook(&book, bookIndex)) { return book->GetCurrentPage(); }
    return nullptr;
}

size_t clMultiBook::GetPageCount() const
{
    size_t count = 0;
    std::for_each(m_books.begin(), m_books.end(), [&](Notebook* b) { count += b->GetPageCount(); });
    return count;
}

int clMultiBook::GetSelection() const { return m_selection; }

size_t clMultiBook::GetAllTabs(clTabInfo::Vec_t& tabs)
{
    tabs.clear();
    clTabInfo::Vec_t all_tabs;
    std::for_each(m_books.begin(), m_books.end(), [&](Notebook* b) {
        clTabInfo::Vec_t t;
        b->GetAllTabs(t);
        all_tabs.insert(all_tabs.end(), t.begin(), t.end());
    });
    tabs.swap(all_tabs);
    return tabs.size();
}

bool clMultiBook::SetPageToolTip(size_t page, const wxString& tooltip)
{
    Notebook* book;
    size_t bookIndex;
    size_t modIndex;
    if(GetBookByPageIndex(page, &book, bookIndex, modIndex)) { return book->SetPageToolTip(modIndex, tooltip); }
    return false;
}

int clMultiBook::SetSelection(size_t tabIdx)
{
    Notebook* book;
    size_t bookIndex;
    size_t modIndex;
    if(GetBookByPageIndex(tabIdx, &book, bookIndex, modIndex)) {
        // Update the current selection
        bool pageChanged = (m_selection != (int)tabIdx);

        // Grab a pointer of the page we want to make active
        wxWindow* focusedPage = book->GetPage(modIndex);

        // And perform the actual selection change
        int oldSelection = book->GetSelection();
        if(oldSelection != (int)modIndex) {
            m_selection = tabIdx;
            m_history->Pop(focusedPage);
            m_history->Push(focusedPage);
            return book->SetSelection(modIndex);
        } else {
            // There is no point on calling Notebook::SetSelection since it is already selected
            // However, in the term of 'multi book' control, we might need to generate the events ourselves here
            // But only if we actually modified the selection
            if(pageChanged) {
                wxBookCtrlEvent changingEvent(wxEVT_BOOK_PAGE_CHANGING);
                changingEvent.SetEventObject(this);
                changingEvent.SetOldSelection(m_selection);
                changingEvent.SetSelection(tabIdx);
                GetEventHandler()->ProcessEvent(changingEvent);
                if(!changingEvent.IsAllowed()) { return wxNOT_FOUND; } // User vetoed
                
                // Update the history
                m_history->Pop(focusedPage);
                m_history->Push(focusedPage);
                
                // Update the selection before we fire the event again
                // Or we might end up with stackoverflow...
                m_selection = tabIdx;
                wxBookCtrlEvent changedEvent(wxEVT_BOOK_PAGE_CHANGED);
                changedEvent.SetEventObject(this);
                changedEvent.SetOldSelection(m_selection);
                changedEvent.SetSelection(tabIdx);
                GetEventHandler()->ProcessEvent(changedEvent);
            }
        }
    }
    return wxNOT_FOUND;
}

bool clMultiBook::SetPageText(size_t page, const wxString& text)
{
    Notebook* book;
    size_t bookIndex;
    size_t modIndex;
    if(GetBookByPageIndex(page, &book, bookIndex, modIndex)) { return book->SetPageText(modIndex, text); }
    return false;
}

wxString clMultiBook::GetPageText(size_t page) const
{
    Notebook* book;
    size_t bookIndex;
    size_t modIndex;
    if(GetBookByPageIndex(page, &book, bookIndex, modIndex)) { return book->GetPageText(modIndex); }
    return wxEmptyString;
}

bool clMultiBook::DeleteAllPages()
{
    std::for_each(m_books.begin(), m_books.end(), [&](Notebook* book) { book->DeleteAllPages(); });
    // Update the history
    m_history->Clear();
    UpdateView();
    return true;
}

void clMultiBook::SetStyle(size_t style)
{
    m_style = style;
    std::for_each(m_books.begin(), m_books.end(), [&](Notebook* book) { book->SetStyle(m_style); });
}

size_t clMultiBook::GetStyle() const { return m_style; }

int clMultiBook::GetPageIndex(wxWindow* window) const
{
    for(size_t i = 0; i < m_books.size(); ++i) {
        int index = m_books[i]->GetPageIndex(window);
        if(index != wxNOT_FOUND) { return BookIndexToGlobalIndex(i, index); }
    }
    return wxNOT_FOUND;
}

int clMultiBook::GetPageIndex(const wxString& label) const
{
    for(size_t i = 0; i < m_books.size(); ++i) {
        int index = m_books[i]->GetPageIndex(label);
        if(index != wxNOT_FOUND) { return BookIndexToGlobalIndex(i, index); }
    }
    return wxNOT_FOUND;
}

void clMultiBook::SetPageBitmap(size_t index, const wxBitmap& bmp)
{
    Notebook* book;
    size_t bookIndex;
    size_t modIndex;
    if(GetBookByPageIndex(index, &book, bookIndex, modIndex)) { book->SetPageBitmap(modIndex, bmp); }
}

bool clMultiBook::RemovePage(size_t page, bool notify)
{
    Notebook* book;
    size_t bookIndex;
    size_t modIndex;
    if(GetBookByPageIndex(page, &book, bookIndex, modIndex)) {
        // Make sure that after we remove the page from its notebook
        // it has a valid parent (UpdateView() below might destory its parent
        // Notebook control)
        wxWindow* removedPage = book->GetPage(modIndex);

        // Update the history
        m_history->Pop(removedPage);

        removedPage->Reparent(this);
        bool res = book->RemovePage(modIndex, notify);
        UpdateView();
        return res;
    }
    return false;
}

bool clMultiBook::MoveActivePage(int newIndex)
{
    // FIXME: implement this
    return false;
}

Notebook* clMultiBook::AddNotebook()
{
    wxWindowUpdateLocker locker(this);

    Notebook* book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_style);
    book->SetStyle(m_style);

    m_books.push_back(book);
    GetSizer()->Add(book, 1, wxEXPAND, 0);

    book->Bind(wxEVT_BOOK_PAGE_CLOSING, &clMultiBook::OnEventProxy, this);
    book->Bind(wxEVT_BOOK_PAGE_CLOSED, &clMultiBook::OnEventProxy, this);
    book->Bind(wxEVT_BOOK_PAGE_CHANGED, &clMultiBook::OnEventProxy, this);
    book->Bind(wxEVT_BOOK_PAGE_CHANGING, &clMultiBook::OnEventProxy, this);
    book->Bind(wxEVT_BOOK_PAGE_CLOSE_BUTTON, &clMultiBook::OnEventProxy, this);
    book->Bind(wxEVT_BOOK_TABAREA_DCLICKED, &clMultiBook::OnEventProxy, this);
    book->Bind(wxEVT_BOOK_TAB_DCLICKED, &clMultiBook::OnEventProxy, this);
    book->Bind(wxEVT_BOOK_TAB_CONTEXT_MENU, &clMultiBook::OnEventProxy, this);
    GetSizer()->Layout();
    return book;
}

void clMultiBook::OnEventProxy(wxBookCtrlEvent& event)
{
    if(event.GetEventObject() == this) {
        // Avoid loops
        event.Skip();
        return;
    }
    int selection = event.GetSelection();
    int oldSelection = event.GetOldSelection();

    Notebook* book = dynamic_cast<Notebook*>(event.GetEventObject());
    if(!book) {
        clWARNING() << "clMultiBook::OnEventProxy no notebook event object!";
        return;
    }

    // Convert the event
    wxBookCtrlEvent proxyEvent(event.GetEventType());
    proxyEvent.SetEventObject(this);
    proxyEvent.SetSelection(wxNOT_FOUND);
    proxyEvent.SetOldSelection(wxNOT_FOUND);
    if(selection != wxNOT_FOUND) { proxyEvent.SetSelection(BookIndexToGlobalIndex(book, selection)); }
    if(oldSelection != wxNOT_FOUND) { proxyEvent.SetOldSelection(BookIndexToGlobalIndex(book, oldSelection)); }

    // Process the event
    if((event.GetEventType() == wxEVT_BOOK_TAB_CONTEXT_MENU) || (event.GetEventType() == wxEVT_BOOK_PAGE_CHANGED)) {
        // Use ProcessEvent
        GetEventHandler()->ProcessEvent(proxyEvent);
    } else if((event.GetEventType() == wxEVT_BOOK_PAGE_CLOSING) || (event.GetEventType() == wxEVT_BOOK_PAGE_CHANGING)) {
        // Handle with ProcessEvent with Veto option
        GetEventHandler()->ProcessEvent(proxyEvent);
        if(!proxyEvent.IsAllowed()) { event.Veto(); }
    } else {
        // Handle with AddPendingEvent
        GetEventHandler()->AddPendingEvent(proxyEvent);
        if(event.GetEventType() == wxEVT_BOOK_PAGE_CLOSED) {
            // A page was closed
            CallAfter(&clMultiBook::UpdateView);
        }
    }
}

void clMultiBook::OnFocus(wxFocusEvent& e)
{
    e.Skip();
    wxWindow* focusedWindow = wxWindow::FindFocus();
    CHECK_PTR_RET(focusedWindow);

    wxWindow* parent = focusedWindow->GetParent();
    while(parent) {
        Notebook* book = dynamic_cast<Notebook*>(parent);
        if(book && IsOurNotebook(book)) {
            // This book is one of ours...
            int index = book->GetSelection();
            if(index != wxNOT_FOUND) {
                int oldSelection = m_selection;
                m_selection = BookIndexToGlobalIndex(book, index);
                if((m_selection != wxNOT_FOUND) && (m_selection != oldSelection)) {
                    // Selection has changed, notify about this
                    wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGED);
                    event.SetEventObject(this);
                    event.SetSelection(m_selection);
                    event.SetOldSelection(oldSelection);
                    GetEventHandler()->ProcessEvent(event);
                }
            }
            break;
        }
        parent = parent->GetParent();
    }
}

bool clMultiBook::IsOurNotebook(Notebook* book) const
{
    for(size_t i = 0; i < m_books.size(); ++i) {
        if(book == m_books[i]) { return true; }
    }
    return false;
}

wxBitmap clMultiBook::GetPageBitmap(size_t page) const
{
    Notebook* book;
    size_t bookIndex;
    size_t modIndex;
    if(GetBookByPageIndex(page, &book, bookIndex, modIndex)) { return book->GetPageBitmap(modIndex); }
    return wxNullBitmap;
}
