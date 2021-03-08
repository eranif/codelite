#include "DockerOutputPane.h"
#include "asyncprocess.h"
#include "clConsoleBase.h"
#include "clDockerDriver.h"
#include "clDockerEvents.h"
#include "clDockerSettings.h"
#include "clDockerWorkspace.h"
#include "docker.h"
#include "event_notifier.h"
#include "file_logger.h"
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

void clDockerDriver::Stop()
{
    if(IsRunning()) {
        // Stop each process
        std::for_each(m_processes.begin(), m_processes.end(), [&](IProcess* process) { process->Terminate(); });
    }
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
    default:
        break;
    }
}

void clDockerDriver::OnProcessTerminated(clProcessEvent& event)
{
    if(!event.GetProcess() || m_processes.count(event.GetProcess()) == 0) {
        return;
    } // Not our process !?

    IProcess* process = event.GetProcess();
    m_processes.erase(process);
    wxDELETE(process);
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
    if(IsRunning())
        return;
    if(ids.IsEmpty())
        return;

    wxString command = GetDockerExe();
    if(command.IsEmpty())
        return;

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

    StartProcessAsync(command, "", IProcessCreateDefault | IProcessWrapInShell, kKillContainers);
}

void clDockerDriver::StartProcessAsync(const wxString& command, const wxString& wd, size_t flags,
                                       clDockerDriver::eContext context)
{
    m_output.Clear();
    m_context = context;
    IProcess* process = ::CreateAsyncProcess(this, command, flags, wd);
    if(process) {
        m_processes.insert(process);
    }
}

wxString clDockerDriver::GetDockerExe() const
{
    clDockerSettings dockerSettings;
    dockerSettings.Load();

    const wxFileName& dockerCommand = dockerSettings.GetDocker();
    if(!dockerCommand.FileExists()) {
        clGetManager()->SetStatusMessage(
            _("Can't find docker executable. Please install docker and let me know where it is"), 3);
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
        if(container.Parse(lines.Item(i))) {
            L.push_back(container);
        }
    }
    m_plugin->GetTerminal()->SetContainers(L);
}

void clDockerDriver::ProcessListImagesCommand()
{
    wxArrayString lines = ::wxStringTokenize(m_output, "\n", wxTOKEN_STRTOK);
    clDockerImage::Vect_t L;
    for(size_t i = 0; i < lines.size(); ++i) {
        clDockerImage image;
        if(image.Parse(lines.Item(i))) {
            L.push_back(image);
        }
    }
    m_plugin->GetTerminal()->SetImages(L);
}

void clDockerDriver::DoListContainers()
{
    // Build the command
    if(IsRunning())
        return;

    wxString command = GetDockerExe();
    if(command.IsEmpty())
        return;

    command << " ps "
               "--format=\"{{.ID}}|{{.Image}}|{{.Command}}|{{.CreatedAt}}|{{.Status}}|{{.Ports}}|{{.Names}}\" -a";

    // Get list of all containers
    StartProcessAsync(command, "", IProcessCreateDefault | IProcessWrapInShell, kListContainers);
}

void clDockerDriver::ListImages() { DoListImages(); }

void clDockerDriver::DoListImages()
{
    // Build the command
    if(IsRunning())
        return;

    wxString command = GetDockerExe();
    if(command.IsEmpty())
        return;

    command << " image ls "
               "--format=\"{{.ID}}|{{.Repository}}|{{.Tag}}|{{.CreatedAt}}|{{.Size}}\" -a";

    StartProcessAsync(command, "", IProcessCreateDefault | IProcessWrapInShell, kListImages);
}

void clDockerDriver::ClearUnusedImages()
{
    // Build the command
    if(IsRunning())
        return;

    wxString command = GetDockerExe();
    if(command.IsEmpty())
        return;

    command << " image prune --force";
    clDockerSettings s;
    s.Load();
    if(s.IsRemoveAllImages()) {
        command << " --all";
    }

    StartProcessAsync(command, "", IProcessCreateDefault | IProcessWrapInShell, kDeleteUnusedImages);
}

void clDockerDriver::AttachTerminal(const wxArrayString& names)
{
    // Sanity
    if(IsRunning())
        return;
    if(names.IsEmpty())
        return;

    wxString command = GetDockerExe();
    if(command.IsEmpty())
        return;

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
    if(proc) {
        proc->WaitForTerminate(outputString);
    }
    return outputString;
}

void clDockerDriver::ExecContainerCommand(const wxString& containerName, const wxString& containerCommand)
{
    wxString command = GetDockerExe();
    if(command.IsEmpty())
        return;

    command << " " << containerCommand << " " << containerName;

    StartProcessSync(command, "", IProcessCreateDefault);
}

void clDockerDriver::StopContainer(const wxString& containerName)
{
    if(IsRunning())
        return;

    wxString command = GetDockerExe();
    if(command.IsEmpty())
        return;

    command << " stop " << containerName;

    StartProcessAsync(command, "", IProcessCreateDefault | IProcessWrapInShell, kContext_StopContainer);
}

void clDockerDriver::StartContainer(const wxString& containerName)
{
    if(IsRunning())
        return;

    wxString command = GetDockerExe();
    if(command.IsEmpty())
        return;

    command << " restart " << containerName;

    StartProcessAsync(command, "", IProcessCreateDefault | IProcessWrapInShell, kContext_StartContainer);
}

void clDockerDriver::Build(const wxFileName& filepath, const clDockerWorkspaceSettings& settings)
{
    if(IsRunning())
        return;
    clDockerBuildableFile::Ptr_t info = settings.GetFileInfo(filepath);
    wxString command = info->GetBuildBaseCommand();
    clGetManager()->ShowOutputPane(_("Docker"));

    wxString buildOptions = info->GetBuildOptions();
    buildOptions.Trim().Trim(false);

    // Since we are building from the Dockerfile directory
    // we simply pass "."
    command << " . " << buildOptions;

    clDEBUG() << "Docker build:" << command;

    m_plugin->GetTerminal()->Clear();
    m_plugin->GetTerminal()->SelectTab("Output");
    m_plugin->GetTerminal()->AddOutputTextWithEOL(command);
    StartProcessAsync(command, filepath.GetPath(), IProcessCreateDefault | IProcessWrapInShell, kBuild);
}

void clDockerDriver::Run(const wxFileName& filepath, const clDockerWorkspaceSettings& settings)
{
    clDockerBuildableFile::Ptr_t info = settings.GetFileInfo(filepath);

    // get the base command (docker exe + run/up)
    wxString command, args;
    info->GetRunBaseCommand(command, args);

    // get user defined options
    wxString runOptions = info->GetRunOptions();
    runOptions.Trim().Trim(false);
    if(!runOptions.empty()) {
        args << " " << runOptions;
    }
    clDEBUG() << "Docker run:" << command << " " << args;

    // Open terminal, and execute the command
    clConsoleBase::Ptr_t console = clConsoleBase::GetTerminal();
    console->SetTerminalNeeded(true);
    console->SetAutoTerminate(true);
    console->SetWaitWhenDone(true);
    console->SetCommand(command, args);
    console->Start();
}
