#ifndef CLDOCKERBUILDER_H
#define CLDOCKERBUILDER_H

#include "clDockerWorkspaceSettings.h"
#include "cl_command_event.h"
#include <wx/event.h>
#include <wx/filename.h>

class Docker;
class IProcess;
class clDockerDriver : public wxEvtHandler
{
protected:
    enum eContext {
        kNone = -1,
        kBuild,
        kRun,
        kListContainers,
        kKillContainers,
    };

protected:
    IProcess* m_process = nullptr;
    Docker* m_plugin;
    eContext m_context = kNone;
    wxString m_output;

protected:
    void OnBuildOutput(clProcessEvent& event);
    void OnBuildTerminated(clProcessEvent& event);
    void OnListContainers(clCommandEvent& event);
    void OnKillContainers(clCommandEvent& event);

protected:
    void StartProcess(const wxString& command, const wxString& wd, size_t flags, clDockerDriver::eContext context);
    wxString GetDockerExe() const;
    void ProcessListContainersCommand();
    void DoListContainers();

public:
    clDockerDriver(Docker* plugin);
    virtual ~clDockerDriver();
    void BuildDockerfile(const wxFileName& dockerfile, const clDockerWorkspaceSettings& settings);
    void ExecuteDockerfile(const wxFileName& dockerfile, const clDockerWorkspaceSettings& settings);

    bool IsRunning() const { return m_process != nullptr; }
    void StopBuild();
};

#endif // CLDOCKERBUILDER_H
