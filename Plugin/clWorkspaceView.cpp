#include "clWorkspaceView.h"

#include "codelite_events.h"
#include "event_notifier.h"
#include "macros.h"

#include <algorithm>

clWorkspaceView::clWorkspaceView(wxSimplebook* book)
    : m_simpleBook(book)
    , m_defaultPage(_("Default"))
{
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clWorkspaceView::OnWorkspaceClosed, this);
}

clWorkspaceView::~clWorkspaceView()
{
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &clWorkspaceView::OnWorkspaceClosed, this);
}

size_t clWorkspaceView::GetPageIndex(const wxString& name) const
{
    for(size_t i = 0; i < m_simpleBook->GetPageCount(); ++i) {
        if(m_simpleBook->GetPageText(i) == name) {
            return i;
        }
    }
    return wxString::npos;
}

wxWindow* clWorkspaceView::GetPage(const wxString& name) const
{
    size_t index = GetPageIndex(name);
    if(index != wxString::npos) {
        return m_simpleBook->GetPage(index);
    } else if(m_windows.count(name)) {
        return m_windows.find(name)->second;
    }
    return NULL;
}

void clWorkspaceView::AddPage(wxWindow* page, const wxString& name, bool addToBook)
{
    if(addToBook) {
        m_simpleBook->AddPage(page, name);
    } else {
        m_windows.insert(std::make_pair(name, page));
    }
}

void clWorkspaceView::SelectPage(const wxString& name)
{
    size_t index = GetPageIndex(name);
    if(index != wxString::npos) {
        m_simpleBook->ChangeSelection(index);
    }
}

void clWorkspaceView::RemovePage(const wxString& name)
{
    size_t index = GetPageIndex(name);
    if(index != wxString::npos) {
        m_simpleBook->RemovePage(index);
    } else {
        // try to locate it in the internal map
        if(m_windows.count(name)) {
            m_windows.erase(name);
        }
    }
}

void clWorkspaceView::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    SelectPage(GetDefaultPage());
}

std::map<wxString, wxWindow*> clWorkspaceView::GetAllPages() const
{
    std::map<wxString, wxWindow*> pages;
    for(size_t i = 0; i < m_simpleBook->GetPageCount(); ++i) {
        if(m_simpleBook->GetPageText(i) == _("Default"))
            continue;
        pages.insert(std::make_pair(m_simpleBook->GetPageText(i), m_simpleBook->GetPage(i)));
    }
    pages.insert(m_windows.begin(), m_windows.end());
    return pages;
}
