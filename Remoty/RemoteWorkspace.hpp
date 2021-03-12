#ifndef REMOTEWORKSPACE_HPP
#define REMOTEWORKSPACE_HPP

#include "IWorkspace.h" // Base class: IWorkspace
#include "clFileSystemWorkspaceConfig.hpp"
#include "cl_command_event.h"
#include "ssh_account_info.h"

#define WORKSPACE_TYPE_NAME "Remote over SSH"

class RemotyWorkspaceView;
class RemoteWorkspace : public IWorkspace
{
    bool m_eventsConnected = false;
    RemotyWorkspaceView* m_view = nullptr;
    SSHAccountInfo m_account;
    wxString m_remoteWorkspaceFile;
    wxString m_localWorkspaceFile;
    clFileSystemWorkspaceSettings m_settings;

public:
    RemoteWorkspace();
    RemoteWorkspace(bool dummy);
    virtual ~RemoteWorkspace();

protected:
    void BindEvents();
    void UnbindEvents();
    void Initialise();
    void OnOpenWorkspace(clCommandEvent& event);
    void OnCloseWorkspace(clCommandEvent& event);
    void DoClose(bool notify);

public:
    // IWorkspace
    virtual wxString GetActiveProjectName() const { return wxEmptyString; }
    virtual wxFileName GetFileName() const;
    virtual wxString GetFilesMask() const;
    virtual wxFileName GetProjectFileName(const wxString& projectName) const;
    virtual void GetProjectFiles(const wxString& projectName, wxArrayString& files) const;
    virtual wxString GetProjectFromFile(const wxFileName& filename) const;
    virtual void GetWorkspaceFiles(wxArrayString& files) const;
    virtual wxArrayString GetWorkspaceProjects() const;
    virtual bool IsBuildSupported() const;
    virtual bool IsProjectSupported() const;

    // API
    bool IsOpened() const;
    const SSHAccountInfo& GetAccount() const { return m_account; }
    const wxString& GetLocalWorkspaceFile() const { return m_localWorkspaceFile; }
    const wxString& GetRemoteWorkspaceFile() const { return m_remoteWorkspaceFile; }
    const clFileSystemWorkspaceSettings& GetSettings() const { return m_settings; }
    clFileSystemWorkspaceSettings& GetSettings() { return m_settings; }
};

#endif // RemoteWorkspace_HPP
