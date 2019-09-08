#ifndef CLFILESYSTEMWORKSPACEVIEW_HPP
#define CLFILESYSTEMWORKSPACEVIEW_HPP

#include "clTreeCtrlPanel.h"
#include "cl_config.h"
#include "cl_command_event.h"

class clThemedButton;
class WXDLLIMPEXP_SDK clFileSystemWorkspaceView : public clTreeCtrlPanel
{
    clConfig m_config;
    clThemedButton* m_buttonConfigs = nullptr;
    wxArrayString m_configs;

protected:
    void OnFolderDropped(clCommandEvent& event);
    void OnContextMenu(clContextMenuEvent& event);
    void OnCloseFolder(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnShowConfigsMenu(wxCommandEvent& event);

public:
    clFileSystemWorkspaceView(wxWindow* parent, const wxString& viewName);
    virtual ~clFileSystemWorkspaceView();
    void UpdateConfigs(const wxArrayString& configs, const wxString& selectedConfig);
};

#endif // CLFILESYSTEMWORKSPACEVIEW_HPP
