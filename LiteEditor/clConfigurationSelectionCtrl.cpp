#include "bitmap_loader.h"
#include "clConfigurationSelectionCtrl.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "wxStringHash.h"
#include "cl_config.h"
#include "clThemeUpdater.h"
#include "clThemedChoice.h"

clConfigurationSelectionCtrl::clConfigurationSelectionCtrl(wxWindow* parent, wxWindowID winid, const wxPoint& pos,
                                                           const wxSize& size, long style)
    : wxPanel(parent, winid, pos, size, style)
{
    clThemeUpdater::Get().RegisterWindow(this);
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_choice = new clThemedChoice(this, wxID_ANY, pos, size, {});
    m_choice->Bind(wxEVT_CHOICE, &clConfigurationSelectionCtrl::OnChoice, this);
    GetSizer()->Add(m_choice, 1, wxEXPAND);
}

clConfigurationSelectionCtrl::~clConfigurationSelectionCtrl()
{
    m_choice->Unbind(wxEVT_CHOICE, &clConfigurationSelectionCtrl::OnChoice, this);
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
    this->m_activeConfiguration = activeConfiguration;
    m_choice->SetStringSelection(m_activeConfiguration); // this will ensure that the checkbox is placed correctly
    // override the text to include the active project name
    m_choice->SetText(wxString() << m_activeProject << " :: " << m_activeConfiguration);
}

void clConfigurationSelectionCtrl::SetActiveProject(const wxString& activeProject)
{
    this->m_activeProject = activeProject;
    m_choice->SetText(wxString() << m_activeProject << " :: " << m_activeConfiguration);
}

void clConfigurationSelectionCtrl::OnChoice(wxCommandEvent& event)
{
    int where = event.GetSelection();
    if(where != wxNOT_FOUND) {
        wxString selectedString = m_choice->GetString(where);
        if(selectedString != OPEN_CONFIG_MGR_STR) { SetActiveConfiguration(selectedString); }

        // Fire 'config-changed' event
        clCommandEvent changeEvent(wxEVT_WORKSPACE_BUILD_CONFIG_CHANGED);
        changeEvent.SetString(selectedString);
        EventNotifier::Get()->AddPendingEvent(changeEvent);
    }
}

void clConfigurationSelectionCtrl::Clear() {}

void clConfigurationSelectionCtrl::SetConfigurations(const wxArrayString& configurations)
{
    m_configurations = configurations;
    m_configurations.push_back(OPEN_CONFIG_MGR_STR);
    m_choice->Set(m_configurations);
}
