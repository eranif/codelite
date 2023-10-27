#include "clTabTogglerHelper.h"

#include "Notebook.h"
#include "cl_defs.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"

clTabTogglerHelper::clTabTogglerHelper(const wxString& outputTabName, wxWindow* outputTab,
                                       const wxString& workspaceTabName, wxWindow* workspaceTab)
    : m_outputTabName(outputTabName)
    , m_outputTab(outputTab)
    , m_workspaceTabName(workspaceTabName)
    , m_workspaceTab(workspaceTab)
{
    if(m_workspaceTab && !m_workspaceTabName.IsEmpty()) {
        EventNotifier::Get()->Bind(wxEVT_SHOW_WORKSPACE_TAB, &clTabTogglerHelper::OnToggleWorkspaceTab, this);
        clGetManager()->AddWorkspaceTab(m_workspaceTabName);
    }
    if(m_outputTab && !m_outputTabName.IsEmpty()) {
        EventNotifier::Get()->Bind(wxEVT_SHOW_OUTPUT_TAB, &clTabTogglerHelper::OnToggleOutputTab, this);
        clGetManager()->AddOutputTab(m_outputTabName);
    }
}

clTabTogglerHelper::~clTabTogglerHelper()
{
    if(m_workspaceTab && !m_workspaceTabName.IsEmpty()) {
        EventNotifier::Get()->Unbind(wxEVT_SHOW_WORKSPACE_TAB, &clTabTogglerHelper::OnToggleWorkspaceTab, this);
    }
    if(m_outputTab && !m_outputTabName.IsEmpty()) {
        EventNotifier::Get()->Unbind(wxEVT_SHOW_OUTPUT_TAB, &clTabTogglerHelper::OnToggleOutputTab, this);
    }
}

void clTabTogglerHelper::OnToggleOutputTab(clCommandEvent& event)
{
    if(event.GetString() != m_outputTabName) {
        event.Skip();
        return;
    }
    DoShowTab(event.IsSelected(), PaneId::BOTTOM_BAR, m_outputTab, m_outputTabName);
}

void clTabTogglerHelper::OnToggleWorkspaceTab(clCommandEvent& event)
{
    wxUnusedVar(event);
#if 0
    if(event.GetString() != m_workspaceTabName) {
        event.Skip();
        return;
    }
    DoShowTab(event.IsSelected(), PaneId::SIDE_BAR, m_workspaceTab, m_workspaceTabName);
#endif
}

bool clTabTogglerHelper::IsTabInNotebook(PaneId pane_id, const wxString& tabname)
{
    return clGetManager()->BookGetPage(pane_id, tabname) != nullptr;
}

void clTabTogglerHelper::DoShowTab(bool show, PaneId pane_id, wxWindow* tab, const wxString& label)
{
    if(show) {
        // show it
        if(!IsTabInNotebook(pane_id, label)) {
            // Only show it if it does not exists in the notebook
            clGetManager()->BookAddPage(pane_id, tab, label);
        } else {
            // The tab already in the notebook, just select it
            clGetManager()->BookSelectPage(pane_id, label);
        }
    } else {
        clGetManager()->BookRemovePage(pane_id, label);
    }
}
