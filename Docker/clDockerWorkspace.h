#ifndef CLDOCKERWORKSPACE_H
#define CLDOCKERWORKSPACE_H

#include "IWorkspace.h"
#include "clDockerDriver.h"
#include "clDockerWorkspaceSettings.h"
#include "cl_command_event.h"

class clDockerWorkspaceView;
class clDockerWorkspace : public IWorkspace
{
    bool m_bindEvents = false;
    wxFileName m_filename;
    clDockerWorkspaceSettings m_settings;
    bool m_isOpen = false;
    bool m_clangOldFlag = false;
    clDockerWorkspaceView* m_view = nullptr;
    clDockerDriver::Ptr_t m_driver;
    Docker* m_plugin = nullptr;

public:
    virtual wxString GetActiveProjectName() const;
    virtual wxFileName GetFileName() const;
    virtual wxString GetFilesMask() const;
    virtual wxFileName GetProjectFileName(const wxString& projectName) const;
    virtual void GetProjectFiles(const wxString& projectName, wxArrayString& files) const;
    virtual wxString GetProjectFromFile(const wxFileName& filename) const;
    virtual void GetWorkspaceFiles(wxArrayString& files) const;
    virtual wxArrayString GetWorkspaceProjects() const;
    virtual bool IsBuildSupported() const;
    virtual bool IsProjectSupported() const;

    const clDockerWorkspaceSettings& GetSettings() const { return m_settings; }
    clDockerWorkspaceSettings& GetSettings() { return m_settings; }

protected:
    // Event handlers
    void OnOpenWorkspace(clCommandEvent& event);
    void OnCloseWorkspace(clCommandEvent& event);
    void OnNewWorkspace(clCommandEvent& event);

    // Session management
    void OnSaveSession(clCommandEvent& event);

    // Build events
    void OnIsBuildInProgress(clBuildEvent& event);
    void OnBuildStarting(clBuildEvent& event);
    void OnStopBuild(clBuildEvent& event);

    // Run events
    void OnRun(clExecuteEvent& event);
    void OnStop(clExecuteEvent& event);

    //===-----
    //===-----
    void RestoreSession();

public:
    clDockerWorkspace(bool bindEvents, Docker* plugin, clDockerDriver::Ptr_t driver);
    virtual ~clDockerWorkspace();

    clDockerWorkspaceView* GetView() { return m_view; }

    /**
     * @brief return a pointer to the actual docker workspace
     */
    static clDockerWorkspace* Get();
    static void Initialise(Docker* plugin);
    static void Shutdown();

    /**
     * @brief open the workspace
     */
    void Open(const wxFileName& path);
    /**
     * @brief Close the workspace
     */
    void Close();

    /**
     * @brief do we have worksapce opened?
     */
    bool IsOpen() const;

    /**
     * @brief create a workspace
     */
    bool Create(const wxFileName& filename);

    void BuildDockerfile(const wxFileName& dockerfile);
    void BuildDockerCompose(const wxFileName& dockerfile);
    void RunDockerfile(const wxFileName& dockerfile);
    void RunDockerCompose(const wxFileName& dockerfile);
};

#endif // CLDOCKERWORKSPACE_H
