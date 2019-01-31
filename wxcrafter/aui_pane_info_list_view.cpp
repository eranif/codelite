#include "aui_pane_info_list_view.h"
#include "wxc_project_metadata.h"
#include "wxgui_defs.h"
#include "wxguicraft_main_view.h"
#include <event_notifier.h>

AuiPaneInfoListView::AuiPaneInfoListView()
    : m_wxcWidget(NULL)
{
}

AuiPaneInfoListView::~AuiPaneInfoListView() {}

void AuiPaneInfoListView::Construct(wxPropertyGrid* pg, wxcWidget* wb)
{
    pg->Clear();
    m_wxcWidget = wb;
    CHECK_POINTER(m_wxcWidget);

    if(m_wxcWidget->IsAuiPane() == false) return;

    m_wxcWidget->GetAuiPaneInfo().Construct(pg);
}

void AuiPaneInfoListView::Changed(wxPropertyGrid* pg, wxPropertyGridEvent& e)
{
    wxUnusedVar(pg);
    CHECK_POINTER(m_wxcWidget);
    m_wxcWidget->GetAuiPaneInfo().OnChanged(e);

    // Notify about modifications
    wxCommandEvent evt(wxEVT_PROPERTIES_MODIFIED);
    EventNotifier::Get()->AddPendingEvent(evt);
}
