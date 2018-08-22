#include "DockerOutputPane.h"
#include "asyncprocess.h"
#include "clDockerDriver.h"
#include "clDockerSettings.h"
#include "docker.h"
#include "globals.h"
#include "imanager.h"
#include "processreaderthread.h"
#include "wxterminal.h"
#include <wx/msgdlg.h>

clDockerDriver::clDockerDriver(Docker* plugin)
    : m_plugin(plugin)
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clDockerDriver::OnBuildOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clDockerDriver::OnBuildTerminated, this);
}

clDockerDriver::~clDockerDriver()
{
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &clDockerDriver::OnBuildOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clDockerDriver::OnBuildTerminated, this);
}

void clDockerDriver::BuildDockerfile(const wxFileName& dockerfile, const clDockerWorkspaceSettings& settings)
{
    if(IsBuildInProgress()) return;
    clDockerfile info;
    if(!settings.GetFileInfo(dockerfile, info)) return;

    clDockerSettings dockerSettings;
    dockerSettings.Load();

    const wxFileName& dockerCommand = dockerSettings.GetDocker();
    if(!dockerCommand.FileExists()) {
        ::wxMessageBox(_("Can't find docker executable. Please install docker and let me know where is it"), "CodeLite",
                       wxICON_WARNING | wxOK | wxOK_DEFAULT);
        return;
    }

    clGetManager()->ShowOutputPane(_("Docker"));

    wxString command;
    command << dockerCommand.GetFullPath();
    ::WrapWithQuotes(command);

    wxString buildOptions = info.GetBuildOptions();
    buildOptions.Trim().Trim(false);
    if(!buildOptions.StartsWith("build")) { buildOptions.Prepend("build "); }

    command << " " << buildOptions;
    ::WrapInShell(command);
    m_plugin->GetTerminal()->Clear();
    m_plugin->GetTerminal()->AddOutputTextWithEOL(command);
    m_buildProcess = ::CreateAsyncProcess(this, command, IProcessCreateDefault, dockerfile.GetPath());
}

void clDockerDriver::ExecuteDockerfile(const wxFileName& dockerfile, const clDockerWorkspaceSettings& settings)
{
    if(IsBuildInProgress()) return;
    clDockerfile info;
    if(!settings.GetFileInfo(dockerfile, info)) return;
}

void clDockerDriver::StopBuild()
{
    if(IsBuildInProgress()) { m_buildProcess->Terminate(); }
}

void clDockerDriver::OnBuildOutput(clProcessEvent& event)
{
    m_plugin->GetTerminal()->AddOutputTextRaw(event.GetOutput());
}

void clDockerDriver::OnBuildTerminated(clProcessEvent& event) { wxDELETE(m_buildProcess); }
