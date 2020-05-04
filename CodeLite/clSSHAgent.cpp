#include "asyncprocess.h"
#include "clSSHAgent.hpp"
#include "file_logger.h"
#include "fileutils.h"
#include "procutils.h"
#include <wx/filename.h>
#include <wx/utils.h>

#ifndef __WXMSW__
static wxString& AddQuotesIfNeeded(wxString& str)
{
    if(str.Contains(" ")) {
        str.Prepend("\"").Append("\"");
    }
    return str;
}
#endif

clSSHAgent::clSSHAgent() { Start(); }

clSSHAgent::~clSSHAgent() { Stop(); }

void clSSHAgent::Start()
{
    wxFileName sshAgent;
    if(!FileUtils::FindExe("ssh-agent", sshAgent)) {
        clDEBUG() << "Could not find ssh-agent executable";
        return;
    }
    clDEBUG() << "Found ssh-agent:" << sshAgent;

    // Check if an instance of ssh-agent is already running
#ifdef __WXMSW__
    PidVec_t P = ProcUtils::PS("ssh-agent");
    if(P.empty()) {
        clDEBUG() << "Could not find a running instance of ssh-agent, starting one...";
        m_process = ::CreateAsyncProcess(nullptr, sshAgent.GetFullPath(),
                                         IProcessCreateWithHiddenConsole | IProcessCreateDefault);
        if(m_process) {
            clDEBUG() << "Started" << sshAgent << "with process ID:" << m_process->GetPid() << clEndl;
        } else {
            clWARNING() << "Failed to start" << sshAgent << "ssh-agent daemon" << clEndl;
        }
    } else {
        clDEBUG() << "Found ssh-agent running at pid:" << P.begin()->pid;
    }
#else
    wxFileName fnSocketPath(clStandardPaths::Get().GetUserDataDir(), "ssh-agent.");
    fnSocketPath.AppendDir("tmp");
    fnSocketPath.SetFullName(fnSocketPath.GetFullName() + (wxString() << ::wxGetProcessId()));
    fnSocketPath.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    wxString socketPath = fnSocketPath.GetFullPath();
    AddQuotesIfNeeded(socketPath);

    wxString command = sshAgent.GetFullPath();
    AddQuotesIfNeeded(command);
    command << " -D -a " << socketPath;

    m_process = ::CreateAsyncProcess(nullptr, command);
    if(!m_process) {
        clWARNING() << "Failed to launch" << command << clEndl;
    } else {
        clDEBUG() << "Starting ssh-agent:" << command << ". pid:" << m_process->GetPid();
        // Call it on the next event loop, this will give the ssh-agent time to start

        wxString SSH_AUTH_SOCK;
        wxString SSH_AGENT_PID;
        SSH_AGENT_PID << m_process->GetPid();
        SSH_AUTH_SOCK = fnSocketPath.GetFullPath();
        ::wxSetEnv("SSH_AUTH_SOCK", SSH_AUTH_SOCK);
        ::wxSetEnv("SSH_AGENT_PID", SSH_AGENT_PID);
        clDEBUG() << "SSH_AUTH_SOCK is set to:" << SSH_AUTH_SOCK;
        clDEBUG() << "SSH_AGENT_PID is set to:" << SSH_AGENT_PID;
        // Run ssh-add
    }
#endif

    // Execute ssh-add
    sshAgent.SetFullName("ssh-add");
    ProcUtils::SafeExecuteCommand(sshAgent.GetFullPath());
}

void clSSHAgent::Stop()
{
    if(m_process) {
        m_process->Terminate();
        wxDELETE(m_process);
    }
}
