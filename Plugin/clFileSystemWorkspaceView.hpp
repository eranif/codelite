#ifndef CLFILESYSTEMWORKSPACEVIEW_HPP
#define CLFILESYSTEMWORKSPACEVIEW_HPP

#include "clTreeCtrlPanel.h"
#include "cl_command_event.h"
#include "cl_config.h"

class clThemedButton;
class WXDLLIMPEXP_SDK clFileSystemWorkspaceView : public clTreeCtrlPanel
{
    clConfig m_config;
    clThemedButton* m_buttonConfigs = nullptr;
    wxArrayString m_configs;
    bool m_buildInProgress = false;
    bool m_runInProgress = false;

protected:
    void OnFolderDropped(clCommandEvent& event);
    void OnContextMenu(clContextMenuEvent& event);
    void OnCloseFolder(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnShowConfigsMenu(wxCommandEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnBuildStarted(clBuildEvent& event);
    void OnBuildEnded(clBuildEvent& event);
    void OnProgramStarted(clExecuteEvent& event);
    void OnProgramStopped(clExecuteEvent& event);
    void OnBuildActiveProjectDropdown(wxCommandEvent& event);

protected:
    void DoCreateBuildDropDownMenu(wxMenu* menu);

public:
    clFileSystemWorkspaceView(wxWindow* parent, const wxString& viewName);
    virtual ~clFileSystemWorkspaceView();
    void UpdateConfigs(const wxArrayString& configs, const wxString& selectedConfig);
};

#endif // CLFILESYSTEMWORKSPACEVIEW_HPP
