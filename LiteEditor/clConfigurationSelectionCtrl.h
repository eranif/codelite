#ifndef CLCONFIGURATIONSELECTIONCTRL_H
#define CLCONFIGURATIONSELECTIONCTRL_H

#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"

#include <wx/arrstr.h>
#include <wx/panel.h>

#define OPEN_CONFIG_MGR_STR _("Open Workspace Configuration Manager...")

class clConfigurationSelectionCtrl : public wxPanel
{
    wxArrayString m_projects;
    wxArrayString m_configurations;
    wxString m_activeProject;
    wxString m_activeConfiguration;
    wxChoice* m_choice = nullptr;

protected:
    void OnChoice(wxCommandEvent& event);
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnProjectAdded(clCommandEvent& event);
    void OnProjectRemoved(clCommandEvent& event);
    void OnConfigurationManager(wxCommandEvent& e);
    void OnActiveProjectChanged(clProjectSettingsEvent& event);

protected:
    void DoUpdateChoiceWithProjects();
    void DoWorkspaceConfig();
    void DoOpenConfigurationManagerDlg();
    void DoConfigChanged(const wxString& newConfigName);

public:
    clConfigurationSelectionCtrl(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                                 const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
    virtual ~clConfigurationSelectionCtrl();

    /**
     * @brief update the list of projects and configurations available
     */
    void Update(const wxArrayString& projects, const wxArrayString& configurations);

    /**
     * @brief clear everything
     */
    void Clear();

    void SetActiveConfiguration(const wxString& activeConfiguration);
    void SetConfigurations(const wxArrayString& configurations, const wxString& activeConfig);

    const wxString& GetActiveConfiguration() const { return m_activeConfiguration; }
    const wxString& GetActiveProject() const { return m_activeProject; }
    const wxArrayString& GetConfigurations() const { return m_configurations; }
    const wxArrayString& GetProjects() const { return m_projects; }
};

#endif // CLCONFIGURATIONSELECTIONCTRL_H
