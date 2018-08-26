#ifndef CLDOCKERBUILDER_H
#define CLDOCKERBUILDER_H

#include "clDockerWorkspaceSettings.h"
#include "cl_command_event.h"
#include <wx/event.h>
#include <wx/filename.h>
#include <wx/sharedptr.h>

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
        kListImages,
        kDeleteUnusedImages,
        kContext_StartContainer,
        kContext_StopContainer,
    };

protected:
    IProcess* m_process = nullptr;
    Docker* m_plugin;
    eContext m_context = kNone;
    wxString m_output;

protected:
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);

protected:
    void StartProcessAsync(const wxString& command, const wxString& wd, size_t flags, clDockerDriver::eContext context);
    wxString StartProcessSync(const wxString& command, const wxString& wd, size_t flags);
    wxString GetDockerExe() const;
    void ProcessListContainersCommand();
    void ProcessListImagesCommand();
    void DoListContainers();
    void DoListImages();

public:
    typedef wxSharedPtr<clDockerDriver> Ptr_t;
    clDockerDriver(Docker* plugin);
    virtual ~clDockerDriver();
    void BuildDockerfile(const wxFileName& dockerfile, const clDockerWorkspaceSettings& settings);
    void ExecuteDockerfile(const wxFileName& dockerfile, const clDockerWorkspaceSettings& settings);

    bool IsRunning() const { return m_process != nullptr; }
    void Stop();

    //===------------------
    //  Images API
    //===------------------
    void ClearUnusedImages();
    void ListImages();

    //===------------------
    //  Containers API
    //===------------------
    void ListContainers();
    void RemoveContainers(const wxArrayString& ids);
    void ExecContainerCommand(const wxString& containerName, const wxString& containerCommand);
    void AttachTerminal(const wxArrayString& names);
    void StopContainer(const wxString& containerName);
    void StartContainer(const wxString& containerName);
};

#endif // CLDOCKERBUILDER_H
