#include "DockerOutputPane.h"
#include "asyncprocess.h"
#include "clDockerDriver.h"
#include "clDockerEvents.h"
#include "clDockerSettings.h"
#include "clDockerWorkspace.h"
#include "docker.h"
#include "event_notifier.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"
#include "processreaderthread.h"
#include "wxterminal.h"
#include <wx/msgdlg.h>

clDockerDriver::clDockerDriver(Docker* plugin)
    : m_plugin(plugin)
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clDockerDriver::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clDockerDriver::OnProcessTerminated, this);
}

clDockerDriver::~clDockerDriver()
{
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &clDockerDriver::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clDockerDriver::OnProcessTerminated, this);
}

void clDockerDriver::BuildDockerfile(const wxFileName& dockerfile, const clDockerWorkspaceSettings& settings)
{
    if(IsRunning()) return;
    clDockerfile info;
    if(!settings.GetFileInfo(dockerfile, info) || info.GetBuildOptions().IsEmpty()) {
        wxMessageBox(wxString() << _("Don't know how to build '") << dockerfile.GetFullPath() << "'\n"
                                << _("Please set the 'Build' options for this file"),
                     "CodeLite", wxICON_WARNING | wxOK | wxOK_DEFAULT);
        return;
    }

    wxString command = GetDockerExe();
    if(command.IsEmpty()) return;

    clGetManager()->ShowOutputPane(_("Docker"));

    wxString buildOptions = info.GetBuildOptions();
    buildOptions.Trim().Trim(false);
    if(!buildOptions.StartsWith("build")) { buildOptions.Prepend("build "); }

    command << " " << buildOptions;
    ::WrapInShell(command);
    m_plugin->GetTerminal()->Clear();
    m_plugin->GetTerminal()->SelectTab("Output");
    m_plugin->GetTerminal()->AddOutputTextWithEOL(command);
    StartProcessAsync(command, dockerfile.GetPath(), IProcessCreateDefault, kBuild);
}

void clDockerDriver::ExecuteDockerfile(const wxFileName& dockerfile, const clDockerWorkspaceSettings& settings)
{
    clDockerfile info;
    if(!settings.GetFileInfo(dockerfile, info) || info.GetRunOptions().IsEmpty()) {
        wxMessageBox(wxString() << _("Don't know how to execute '") << dockerfile.GetFullPath() << "'\n"
                                << _("Please set the 'Run' options for this file"),
                     "CodeLite", wxICON_WARNING | wxOK | wxOK_DEFAULT);
        return;
    }

    wxString command = GetDockerExe();
    if(command.IsEmpty()) { return; }

    wxString runOptions = info.GetBuildOptions();
    runOptions.Trim().Trim(false);
    if(!runOptions.StartsWith("run")) { command << " run "; }
    command << info.GetRunOptions();
    FileUtils::OpenTerminal(dockerfile.GetPath(), command);
}

void clDockerDriver::Stop()
{
    if(IsRunning()) { m_process->Terminate(); }
}

void clDockerDriver::OnProcessOutput(clProcessEvent& event)
{
    switch(m_context) {
    case kNone:
        break; //??
    case kRun:
    case kBuild:
    case kDeleteUnusedImages:
    case kKillContainers:
        m_plugin->GetTerminal()->AddOutputTextRaw(event.GetOutput());
        break;
    case kListContainers:
    case kListImages:
        m_output << event.GetOutput();
        break;
    }
}

void clDockerDriver::OnProcessTerminated(clProcessEvent& event)
{
    wxDELETE(m_process);
    switch(m_context) {
    case kListImages:
        ProcessListImagesCommand();
        break;
    case kListContainers:
        ProcessListContainersCommand();
        break;
    case kKillContainers:
        CallAfter(&clDockerDriver::DoListContainers);
        break;
    case kDeleteUnusedImages:
        CallAfter(&clDockerDriver::DoListImages);
        break;
    case kContext_StartContainer:
    case kContext_StopContainer:
        CallAfter(&clDockerDriver::DoListContainers);
        break;
    default:
        break;
    }
}

void clDockerDriver::ListContainers() { DoListContainers(); }

void clDockerDriver::RemoveContainers(const wxArrayString& ids)
{
    // Sanity
    if(IsRunning()) return;
    if(ids.IsEmpty()) return;

    wxString command = GetDockerExe();
    if(command.IsEmpty()) return;

    wxString message;
    message << _("Choosing 'Yes' will remove ") << ids.size() << _(" container(s)\nContinue?");
    if(::wxMessageBox(message, "CodeLite", wxICON_WARNING | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT,
                      EventNotifier::Get()->TopFrame()) != wxYES) {
        return;
    }
    command << " rm --force ";
    for(size_t i = 0; i < ids.size(); ++i) {
        command << " " << ids[i];
    }
    ::WrapInShell(command);
    StartProcessAsync(command, "", IProcessCreateDefault, kKillContainers);
}

void clDockerDriver::StartProcessAsync(const wxString& command, const wxString& wd, size_t flags,
                                       clDockerDriver::eContext context)
{
    m_output.Clear();
    m_context = context;
    m_process = ::CreateAsyncProcess(this, command, flags, wd);
}

wxString clDockerDriver::GetDockerExe() const
{
    clDockerSettings dockerSettings;
    dockerSettings.Load();

    const wxFileName& dockerCommand = dockerSettings.GetDocker();
    if(!dockerCommand.FileExists()) {
        clGetManager()->SetStatusMessage(
            _("Can't find docker executable\nPlease install docker and let me know where it is"), 3);
        return "";
    }
    wxString exepath = dockerCommand.GetFullPath();
    ::WrapWithQuotes(exepath);
    return exepath;
}

void clDockerDriver::ProcessListContainersCommand()
{
    clDockerContainer::Vect_t L;
    wxArrayString lines = ::wxStringTokenize(m_output, "\n", wxTOKEN_STRTOK);
    for(size_t i = 0; i < lines.size(); ++i) {
        clDockerContainer container;
        if(container.Parse(lines.Item(i))) { L.push_back(container); }
    }
    m_plugin->GetTerminal()->SetContainers(L);
}

void clDockerDriver::ProcessListImagesCommand()
{
    wxArrayString lines = ::wxStringTokenize(m_output, "\n", wxTOKEN_STRTOK);
    clDockerImage::Vect_t L;
    for(size_t i = 0; i < lines.size(); ++i) {
        clDockerImage image;
        if(image.Parse(lines.Item(i))) { L.push_back(image); }
    }
    m_plugin->GetTerminal()->SetImages(L);
}

void clDockerDriver::DoListContainers()
{
    // Build the command
    if(IsRunning()) return;

    wxString command = GetDockerExe();
    if(command.IsEmpty()) return;

    command << " ps "
               "--format=\"{{.ID}}|{{.Image}}|{{.Command}}|{{.CreatedAt}}|{{.Status}}|{{.Ports}}|{{.Names}}\" -a";
    ::WrapInShell(command);

    // Get list of all containers
    StartProcessAsync(command, "", IProcessCreateDefault, kListContainers);
}

void clDockerDriver::ListImages() { DoListImages(); }

void clDockerDriver::DoListImages()
{
    // Build the command
    if(IsRunning()) return;

    wxString command = GetDockerExe();
    if(command.IsEmpty()) return;

    command << " image ls "
               "--format=\"{{.ID}}|{{.Repository}}|{{.Tag}}|{{.CreatedAt}}|{{.Size}}\" -a";
    ::WrapInShell(command);
    StartProcessAsync(command, "", IProcessCreateDefault, kListImages);
}

void clDockerDriver::ClearUnusedImages()
{
    // Build the command
    if(IsRunning()) return;

    wxString command = GetDockerExe();
    if(command.IsEmpty()) return;

    command << " image prune --force";
    clDockerSettings s;
    s.Load();
    if(s.IsRemoveAllImages()) { command << " --all"; }
    ::WrapInShell(command);
    StartProcessAsync(command, "", IProcessCreateDefault, kDeleteUnusedImages);
}

void clDockerDriver::AttachTerminal(const wxArrayString& names)
{
    // Sanity
    if(IsRunning()) return;
    if(names.IsEmpty()) return;

    wxString command = GetDockerExe();
    if(command.IsEmpty()) return;

    for(size_t i = 0; i < names.size(); ++i) {
        wxString message;
        command << " exec -i " << names.Item(i) << " /bin/bash -i";
        FileUtils::OpenTerminal(clDockerWorkspace::Get()->GetFileName().GetPath(), command);
    }
}

wxString clDockerDriver::StartProcessSync(const wxString& command, const wxString& wd, size_t flags)
{
    wxString outputString;
    IProcess::Ptr_t proc(::CreateSyncProcess(command, flags, wd));
    if(proc) { proc->WaitForTerminate(outputString); }
    return outputString;
}

void clDockerDriver::ExecContainerCommand(const wxString& containerName, const wxString& containerCommand)
{
    wxString command = GetDockerExe();
    if(command.IsEmpty()) return;

    command << " " << containerCommand << " " << containerName;
    ::WrapInShell(command);
    StartProcessSync(command, "", IProcessCreateDefault);
}

void clDockerDriver::StopContainer(const wxString& containerName)
{
    if(IsRunning()) return;
    
    wxString command = GetDockerExe();
    if(command.IsEmpty()) return;

    command << " stop " << containerName;
    ::WrapInShell(command);
    StartProcessAsync(command, "", IProcessCreateDefault, kContext_StopContainer);
}

void clDockerDriver::StartContainer(const wxString& containerName)
{
    if(IsRunning()) return;
    
    wxString command = GetDockerExe();
    if(command.IsEmpty()) return;

    command << " restart " << containerName;
    ::WrapInShell(command);
    StartProcessAsync(command, "", IProcessCreateDefault, kContext_StartContainer);
}
