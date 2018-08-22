#ifndef CLDOCKERBUILDER_H
#define CLDOCKERBUILDER_H

#include "clDockerWorkspaceSettings.h"
#include "cl_command_event.h"
#include <wx/event.h>
#include <wx/filename.h>

class IProcess;
class clDockerBuilder : public wxEvtHandler
{
    IProcess* m_buildProcess = nullptr;

protected:
    void OnBuildOutput(clProcessEvent& event);
    void OnBuildTerminated(clProcessEvent& event);

public:
    clDockerBuilder();
    virtual ~clDockerBuilder();
    void BuildDockerfile(const wxFileName& dockerfile, const clDockerWorkspaceSettings& settings);
    void ExecuteDockerfile(const wxFileName& dockerfile, const clDockerWorkspaceSettings& settings);

    bool IsBuildInProgress() const { return m_buildProcess != nullptr; }
    void StopBuild();
};

#endif // CLDOCKERBUILDER_H
