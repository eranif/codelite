#include "DockerOutputPane.h"
#include "asyncprocess.h"
#include "clDockerDriver.h"
#include "clDockerEvents.h"
#include "clDockerSettings.h"
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
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clDockerDriver::OnBuildOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clDockerDriver::OnBuildTerminated, this);
    EventNotifier::Get()->Bind(wxEVT_DOCKER_KILL_CONTAINER, &clDockerDriver::OnKillContainers, this);
    EventNotifier::Get()->Bind(wxEVT_DOCKER_LIST_CONTAINERS, &clDockerDriver::OnListContainers, this);
    EventNotifier::Get()->Bind(wxEVT_DOCKER_LIST_IMAGES, &clDockerDriver::OnListImages, this);
    EventNotifier::Get()->Bind(wxEVT_DOCKER_CLEAR_UNUSED_IMAGES, &clDockerDriver::OnClearUnusedImages, this);
}

clDockerDriver::~clDockerDriver()
{
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &clDockerDriver::OnBuildOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clDockerDriver::OnBuildTerminated, this);
    EventNotifier::Get()->Unbind(wxEVT_DOCKER_KILL_CONTAINER, &clDockerDriver::OnKillContainers, this);
    EventNotifier::Get()->Unbind(wxEVT_DOCKER_LIST_CONTAINERS, &clDockerDriver::OnListContainers, this);
    EventNotifier::Get()->Unbind(wxEVT_DOCKER_LIST_IMAGES, &clDockerDriver::OnListImages, this);
    EventNotifier::Get()->Unbind(wxEVT_DOCKER_CLEAR_UNUSED_IMAGES, &clDockerDriver::OnClearUnusedImages, this);
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
    if(!buildOptions.StartsWith("build")) {
        buildOptions.Prepend("build ");
    }

    command << " " << buildOptions;
    ::WrapInShell(command);
    m_plugin->GetTerminal()->Clear();
    m_plugin->GetTerminal()->SelectTab("Output");
    m_plugin->GetTerminal()->AddOutputTextWithEOL(command);
    StartProcess(command, dockerfile.GetPath(), IProcessCreateDefault, kBuild);
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
    if(command.IsEmpty()) {
        return;
    }
    
    wxString runOptions = info.GetBuildOptions();
    runOptions.Trim().Trim(false);
    if(!runOptions.StartsWith("run")) {
        command << " run ";
    }
    command << info.GetRunOptions();
    FileUtils::OpenTerminal(dockerfile.GetPath(), command);
}

void clDockerDriver::Stop()
{
    if(IsRunning()) {
        m_process->Terminate();
    }
}

void clDockerDriver::OnBuildOutput(clProcessEvent& event)
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

void clDockerDriver::OnBuildTerminated(clProcessEvent& event)
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
    default:
        break;
    }
}

void clDockerDriver::OnListContainers(clCommandEvent& event)
{
    wxUnusedVar(event);
    DoListContainers();
}

void clDockerDriver::OnKillContainers(clCommandEvent& event)
{
    // Sanity
    if(IsRunning()) return;
    if(event.GetStrings().IsEmpty()) return;

    wxString command = GetDockerExe();
    if(command.IsEmpty()) return;

    const wxArrayString& ids = event.GetStrings();

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
    StartProcess(command, "", IProcessCreateDefault, kKillContainers);
}

void clDockerDriver::StartProcess(const wxString& command, const wxString& wd, size_t flags,
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
    wxArrayString lines = ::wxStringTokenize(m_output, "\n", wxTOKEN_STRTOK);
    clDockerContainer::Vect_t L;
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
    if(IsRunning()) return;

    wxString command = GetDockerExe();
    if(command.IsEmpty()) return;

    command << " ps "
               "--format=\"{{.ID}}|{{.Image}}|{{.Command}}|{{.CreatedAt}}|{{.Status}}|{{.Ports}}|{{.Names}}\" -a";
    ::WrapInShell(command);
    StartProcess(command, "", IProcessCreateDefault, kListContainers);
}

void clDockerDriver::OnListImages(clCommandEvent& event)
{
    wxUnusedVar(event);
    DoListImages();
}

void clDockerDriver::DoListImages()
{
    // Build the command
    if(IsRunning()) return;

    wxString command = GetDockerExe();
    if(command.IsEmpty()) return;

    command << " image ls "
               "--format=\"{{.ID}}|{{.Repository}}|{{.Tag}}|{{.CreatedAt}}|{{.Size}}\" -a";
    ::WrapInShell(command);
    StartProcess(command, "", IProcessCreateDefault, kListImages);
}

void clDockerDriver::OnClearUnusedImages(clCommandEvent& event)
{
    // Build the command
    if(IsRunning()) return;

    wxString command = GetDockerExe();
    if(command.IsEmpty()) return;

    command << " image prune --force";
    clDockerSettings s;
    s.Load();
    if(s.IsRemoveAllImages()) {
        command << " --all";
    }
    ::WrapInShell(command);
    StartProcess(command, "", IProcessCreateDefault, kDeleteUnusedImages);
}
