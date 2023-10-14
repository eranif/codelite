#include "clTabTogglerHelper.h"

#include "Notebook.h"
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

    Notebook* book = clGetManager()->GetOutputBook();
    if(event.IsSelected()) {
        // show it
        int where = IsTabInNotebook(book, m_outputTabName);
        if(where == wxNOT_FOUND) {
            // Only show it if it does not exists in the notebook
            clGetManager()->GetOutputBook()->AddPage(m_outputTab, m_outputTabName, true, m_outputTabBmp);
        } else {
            clGetManager()->GetOutputBook()->SetSelection(where);
        }
    } else {
        int where = clGetManager()->GetOutputBook()->GetPageIndex(m_outputTabName);
        if(where != wxNOT_FOUND) {
            clGetManager()->GetOutputBook()->RemovePage(where);
        }
    }
}

void clTabTogglerHelper::OnToggleWorkspaceTab(clCommandEvent& event)
{
    if(event.GetString() != m_workspaceTabName) {
        event.Skip();
        return;
    }

    Notebook* book = clGetManager()->GetSidebarBook();
    if(event.IsSelected()) {
        // show it
        int where = IsTabInNotebook(book, m_workspaceTabName);
        if(where == wxNOT_FOUND) {
            // Only show it if it does not exists in the notebook
            clGetManager()->GetSidebarBook()->AddPage(m_workspaceTab, m_workspaceTabName, true,
                                                                m_workspaceTabBmp);
        } else {
            // The tab already in the notebook, just select it
            clGetManager()->GetSidebarBook()->SetSelection(where);
        }
    } else {
        int where = clGetManager()->GetSidebarBook()->GetPageIndex(m_workspaceTabName);
        if(where != wxNOT_FOUND) {
            clGetManager()->GetSidebarBook()->RemovePage(where);
        }
    }
}

int clTabTogglerHelper::IsTabInNotebook(Notebook* book, const wxString& tabname)
{
    for(size_t i = 0; i < book->GetPageCount(); ++i) {
        if(book->GetPageText(i) == tabname)
            return i;
    }
    return wxNOT_FOUND;
}
