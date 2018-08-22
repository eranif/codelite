#include "asyncprocess.h"
#include "clDockerBuilder.h"
#include "clDockerSettings.h"
#include "docker.h"
#include "globals.h"
#include "imanager.h"
#include "processreaderthread.h"
#include "wxterminal.h"
#include <wx/msgdlg.h>

clDockerBuilder::clDockerBuilder(Docker* plugin)
    : m_plugin(plugin)
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
    if(!buildOptions.StartsWith("build")) {
        buildOptions.Prepend("build ");
    }

    command << " " << buildOptions;
    ::WrapInShell(command);
    m_plugin->GetTerminal()->Clear();
    m_plugin->GetTerminal()->AddTextWithEOL(command);
    m_buildProcess = ::CreateAsyncProcess(this, command, IProcessCreateDefault, dockerfile.GetPath());
}

void clDockerBuilder::ExecuteDockerfile(const wxFileName& dockerfile, const clDockerWorkspaceSettings& settings)
{
    if(IsBuildInProgress()) return;
    clDockerfile info;
    if(!settings.GetFileInfo(dockerfile, info)) return;
}

void clDockerBuilder::StopBuild()
{
    if(IsBuildInProgress()) {
        m_buildProcess->Terminate();
    }
}

void clDockerBuilder::OnBuildOutput(clProcessEvent& event) { m_plugin->GetTerminal()->AddTextRaw(event.GetOutput()); }

void clDockerBuilder::OnBuildTerminated(clProcessEvent& event) { wxDELETE(m_buildProcess); }
