#include "LanguageServerLogView.h"

#include "JSON.h"
#include "LanguageServerCluster.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"

LanguageServerLogView::LanguageServerLogView(wxWindow* parent, LanguageServerCluster* cluster)
    : LanguageServerLogViewBase(parent)
    , m_cluster(cluster)
{
    m_dvListCtrl->Bind(wxEVT_CONTEXT_MENU, [this](wxContextMenuEvent& event) {
        wxMenu menu;
        menu.Append(wxID_CLEAR);
        menu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent& event) {
                wxUnusedVar(event);
                // clear the view
                m_dvListCtrl->DeleteAllItems();
            },
            wxID_CLEAR);
        m_dvListCtrl->PopupMenu(&menu);
    });
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &LanguageServerLogView::OnWorkspaceClosed, this);
}

LanguageServerLogView::~LanguageServerLogView()
{
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &LanguageServerLogView::OnWorkspaceClosed, this);
}

void LanguageServerLogView::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    m_dvListCtrl->DeleteAllItems();
}
