#include "clConfigurationSelectionCtrl.h"

#include "bitmap_loader.h"
#include "clThemedChoice.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "configuration_manager_dlg.h"
#include "event_notifier.h"
#include "frame.h"
#include "globals.h"
#include "imanager.h"
#include "manager.h"
#include "wxStringHash.h"

clConfigurationSelectionCtrl::clConfigurationSelectionCtrl(wxWindow* parent, wxWindowID winid, const wxPoint& pos,
                                                           const wxSize& size, long style)
    : wxPanel(parent, winid, pos, size, style)
{
    SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_choice = new wxChoice(this, wxID_ANY, pos, size, {});
    m_choice->Bind(wxEVT_CHOICE, &clConfigurationSelectionCtrl::OnChoice, this);
    GetSizer()->Add(m_choice, 1, wxEXPAND | wxALL, 5);

    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clConfigurationSelectionCtrl::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clConfigurationSelectionCtrl::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_PROJ_ADDED, &clConfigurationSelectionCtrl::OnProjectAdded, this);
    EventNotifier::Get()->Bind(wxEVT_PROJ_REMOVED, &clConfigurationSelectionCtrl::OnProjectRemoved, this);
    wxTheApp->Bind(wxEVT_MENU, &clConfigurationSelectionCtrl::OnConfigurationManager, this,
                   XRCID("configuration_manager"));
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_PROJECT_CHANGED, &clConfigurationSelectionCtrl::OnActiveProjectChanged,
                               this);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, [this](clCommandEvent& e) {
        e.Skip();
        SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
        Refresh();
    });
}

clConfigurationSelectionCtrl::~clConfigurationSelectionCtrl()
{
    m_choice->Unbind(wxEVT_CHOICE, &clConfigurationSelectionCtrl::OnChoice, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &clConfigurationSelectionCtrl::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &clConfigurationSelectionCtrl::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_PROJ_ADDED, &clConfigurationSelectionCtrl::OnProjectAdded, this);
    EventNotifier::Get()->Unbind(wxEVT_PROJ_REMOVED, &clConfigurationSelectionCtrl::OnProjectRemoved, this);
    wxTheApp->Unbind(wxEVT_MENU, &clConfigurationSelectionCtrl::OnConfigurationManager, this,
                     XRCID("configuration_manager"));
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_PROJECT_CHANGED, &clConfigurationSelectionCtrl::OnActiveProjectChanged,
                                 this);
}

void clConfigurationSelectionCtrl::Update(const wxArrayString& projects, const wxArrayString& configurations)
{
    m_projects = projects;
    m_configurations = configurations;
    m_configurations.push_back(OPEN_CONFIG_MGR_STR);
    m_choice->Set(configurations);
}

void clConfigurationSelectionCtrl::SetActiveConfiguration(const wxString& activeConfiguration)
{
    m_activeConfiguration = activeConfiguration;

    int where = m_choice->FindString(m_activeConfiguration);
    if(where != wxNOT_FOUND) {
        m_choice->SetSelection(where);
    }
}

void clConfigurationSelectionCtrl::OnChoice(wxCommandEvent& event)
{
    int where = event.GetSelection();
    if(where == wxNOT_FOUND)
        return;

    wxString selectedString = m_choice->GetString(where);
    if(selectedString != OPEN_CONFIG_MGR_STR) {

        SetActiveConfiguration(selectedString);
        DoConfigChanged(selectedString);

        // Fire 'config-changed' event
        clCommandEvent changeEvent(wxEVT_WORKSPACE_BUILD_CONFIG_CHANGED);
        changeEvent.SetString(selectedString);
        EventNotifier::Get()->AddPendingEvent(changeEvent);
    } else {
        DoOpenConfigurationManagerDlg();
    }
}

void clConfigurationSelectionCtrl::Clear() {}

void clConfigurationSelectionCtrl::SetConfigurations(const wxArrayString& configurations, const wxString& activeConfig)
{
    m_configurations = configurations;
    m_activeConfiguration = activeConfig;
    m_configurations.push_back(OPEN_CONFIG_MGR_STR);
    m_choice->Set(m_configurations);
    int where = m_choice->FindString(m_activeConfiguration);
    if(where != wxNOT_FOUND) {
        m_choice->SetSelection(where);
    }
}

void clConfigurationSelectionCtrl::OnWorkspaceLoaded(clWorkspaceEvent& event)
{
    event.Skip();
    if(ManagerST::Get()->IsWorkspaceOpen()) {
        Enable(true);
        DoWorkspaceConfig();
        DoUpdateChoiceWithProjects();
    }
}

void clConfigurationSelectionCtrl::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    Clear();
    Enable(false);
}

void clConfigurationSelectionCtrl::OnProjectAdded(clCommandEvent& event)
{
    event.Skip();
    DoUpdateChoiceWithProjects();
}

void clConfigurationSelectionCtrl::OnProjectRemoved(clCommandEvent& event)
{
    event.Skip();
    DoUpdateChoiceWithProjects();
}

void clConfigurationSelectionCtrl::DoUpdateChoiceWithProjects() {}

void clConfigurationSelectionCtrl::DoWorkspaceConfig()
{
    // Update the workspace configuration
    BuildMatrixPtr matrix = clCxxWorkspaceST::Get()->GetBuildMatrix();
    auto confs = matrix->GetConfigurations();

    confs.sort([](WorkspaceConfigurationPtr one, WorkspaceConfigurationPtr two) {
        return one->GetName().CmpNoCase(two->GetName()) < 0;
    });

    wxArrayString configurations;
    std::for_each(confs.begin(), confs.end(),
                  [&](WorkspaceConfigurationPtr conf) { configurations.push_back(conf->GetName()); });

    wxString activeConfig = configurations.IsEmpty() ? "" : matrix->GetSelectedConfigurationName();
    SetConfigurations(configurations, activeConfig);
    clMainFrame::Get()->SelectBestEnvSet();
}

void clConfigurationSelectionCtrl::OnConfigurationManager(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoOpenConfigurationManagerDlg();
}

void clConfigurationSelectionCtrl::DoOpenConfigurationManagerDlg()
{
    ConfigurationManagerDlg dlg(EventNotifier::Get()->TopFrame());
    dlg.ShowModal();

    // in case user added configurations, update the choice control
    DoWorkspaceConfig();
    DoUpdateChoiceWithProjects();

    BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
    SetActiveConfiguration(matrix->GetSelectedConfigurationName());
}

void clConfigurationSelectionCtrl::DoConfigChanged(const wxString& newConfigName)
{
    wxBusyCursor bc;
    BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
    matrix->SetSelectedConfigurationName(newConfigName);
    ManagerST::Get()->SetWorkspaceBuildMatrix(matrix);

    // Set the focus to the active editor if any
    clEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if(editor) {
        editor->SetActive();
    }

    ManagerST::Get()->UpdateParserPaths(true);
}

void clConfigurationSelectionCtrl::OnActiveProjectChanged(clProjectSettingsEvent& event) { event.Skip(); }
