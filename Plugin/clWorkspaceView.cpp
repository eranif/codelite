#include "clWorkspaceView.h"
#include "macros.h"
#include "event_notifier.h"
#include "codelite_events.h"

clWorkspaceView::clWorkspaceView(wxSimplebook* book)
    : m_simpleBook(book)
    , m_defaultPage("Default")
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
    CHECK_COND_RET_NULL(index != wxString::npos);

    return m_simpleBook->GetPage(index);
    return NULL;
}

void clWorkspaceView::AddPage(wxWindow* page, const wxString& name) { m_simpleBook->AddPage(page, name); }

void clWorkspaceView::SelectPage(const wxString& name)
{
    size_t index = GetPageIndex(name);
    CHECK_COND_RET(index != wxString::npos);
    m_simpleBook->ChangeSelection(index);
}

void clWorkspaceView::RemovePage(const wxString& name)
{
    size_t index = GetPageIndex(name);
    CHECK_COND_RET(index != wxString::npos);
}

void clWorkspaceView::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    SelectPage(GetDefaultPage());
}
