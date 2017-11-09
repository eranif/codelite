#ifndef CLCONFIGURATIONSELECTIONCTRL_H
#define CLCONFIGURATIONSELECTIONCTRL_H

#include "drawingutils.h"
#include <wx/arrstr.h>
#include <wx/panel.h>

#define OPEN_CONFIG_MGR_STR _("Open Workspace Configuration Manager...")

class clConfigurationSelectionCtrl : public wxPanel
{
    wxArrayString m_projects;
    wxArrayString m_configurations;
    wxString m_activeProject;
    wxString m_activeConfiguration;
    eButtonState m_state;

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
    void SetActiveProject(const wxString& activeProject);
    void SetConfigurations(const wxArrayString& configurations) { this->m_configurations = configurations; }
    void SetProjects(const wxArrayString& projects) { this->m_projects = projects; }
    const wxString& GetActiveConfiguration() const { return m_activeConfiguration; }
    const wxString& GetActiveProject() const { return m_activeProject; }
    const wxArrayString& GetConfigurations() const { return m_configurations; }
    const wxArrayString& GetProjects() const { return m_projects; }

protected:
    void OnPaint(wxPaintEvent& e);
    void OnEraseBG(wxEraseEvent& e);
    void OnLeftDown(wxMouseEvent& e);
    void OnEnterWindow(wxMouseEvent& e);
    void OnLeaveWindow(wxMouseEvent& e);
};

#endif // CLCONFIGURATIONSELECTIONCTRL_H
