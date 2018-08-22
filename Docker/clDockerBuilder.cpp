#include "asyncprocess.h"
#include "clDockerBuilder.h"
#include "globals.h"
#include "imanager.h"
#include "processreaderthread.h"

clDockerBuilder::clDockerBuilder()
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clDockerBuilder::OnBuildOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clDockerBuilder::OnBuildTerminated, this);
}

clDockerBuilder::~clDockerBuilder()
{
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &clDockerBuilder::OnBuildOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clDockerBuilder::OnBuildTerminated, this);
}

void clDockerBuilder::BuildDockerfile(const wxFileName& dockerfile, const clDockerWorkspaceSettings& settings)
{
    if(IsBuildInProgress()) return;
    clDockerfile info;
    if(!settings.GetFileInfo(dockerfile, info)) return;
}

void clDockerBuilder::ExecuteDockerfile(const wxFileName& dockerfile, const clDockerWorkspaceSettings& settings)
{
    if(IsBuildInProgress()) return;
    clDockerfile info;
    if(!settings.GetFileInfo(dockerfile, info)) return;
}

void clDockerBuilder::StopBuild()
{
    if(IsBuildInProgress()) { m_buildProcess->Terminate(); }
}

void clDockerBuilder::OnBuildOutput(clProcessEvent& event)
{
    clGetManager()->AppendOutputTabText(kOutputTab_Build, event.GetOutput());
}

void clDockerBuilder::OnBuildTerminated(clProcessEvent& event) { wxDELETE(m_buildProcess); }
