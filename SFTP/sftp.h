#ifndef __SFTP__
#define __SFTP__

#include "plugin.h"
#include "sftp_workspace_settings.h"
#include "cl_command_event.h"

class SFTPStatusPage;

class SFTP : public IPlugin
{
    wxFileName            m_workspaceFile;
    SFTPWorkspaceSettings m_workspaceSettings;
    SFTPStatusPage*       m_outputPane;
    
public:
    SFTP(IManager *manager);
    ~SFTP();

protected:
    void OnSettings(wxCommandEvent &e);
    void OnSetupWorkspaceMirroring(wxCommandEvent &e);
    void OnDisableWorkspaceMirroring(wxCommandEvent &e);
    void OnDisableWorkspaceMirroringUI(wxUpdateUIEvent &e);
    void OnWorkspaceOpened(wxCommandEvent &e);
    void OnWorkspaceClosed(wxCommandEvent &e);
    void OnFileSaved(wxCommandEvent &e);
    
    bool IsWorkspaceOpened() const {
        return m_workspaceFile.IsOk();
    }
    
    // API calls
    void OnSaveFile(clCommandEvent &e);
    
public:
    //--------------------------------------------
    //Abstract methods
    //--------------------------------------------
    virtual clToolBar *CreateToolBar(wxWindow *parent);
    virtual void CreatePluginMenu(wxMenu *pluginsMenu);
    virtual void HookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnPlug();
    
    // Callbacks
    void OnFileWriteOK(const wxString &message);
    void OnFileWriteError(const wxString &errorMessage);
};

#endif //SFTP
