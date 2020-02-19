#include "clRemoteBuilder.hpp"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
#include "processreaderthread.h"
#include <wx/msgdlg.h>

#if USE_SFTP
#include "SSHRemoteProcess.hpp"
#include "sftp_settings.h"
#endif

clRemoteBuilder::clRemoteBuilder()
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clRemoteBuilder::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clRemoteBuilder::OnProcessTerminated, this);
}

clRemoteBuilder::~clRemoteBuilder()
{
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &clRemoteBuilder::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clRemoteBuilder::OnProcessTerminated, this);
    wxDELETE(m_remoteProcess);
}

bool clRemoteBuilder::IsRunning() const { return m_remoteProcess != nullptr; }

void clRemoteBuilder::Build(const wxString& sshAccount, const wxString& command, const wxString& workingDirectory)
{
#if USE_SFTP
    if(m_remoteProcess) {
        return;
    }
    SFTPSettings settings;
    settings.Load();
    SSHAccountInfo account;
    if(!settings.GetAccount(sshAccount, account)) {
        ::wxMessageBox(wxString() << _("Could not find SSH account: ") << sshAccount, "CodeLite",
                       wxICON_WARNING | wxCENTER);
        return;
    }
    try {
        if(m_ssh) {
            m_ssh->Close();
        }
        m_ssh.reset(new clSSH());

        // Establish SSH connection and launch the build
        m_ssh->SetUsername(account.GetUsername());
        m_ssh->SetPassword(account.GetPassword());
        m_ssh->SetHost(account.GetHost());
        m_ssh->SetPort(account.GetPort());
        m_ssh->Connect();
        m_ssh->Login();

    } catch(clException& e) {
        clERROR() << e.What();
        ::wxMessageBox(wxString() << _("Failed to start remote build\n") << e.What(), "CodeLite",
                       wxICON_ERROR | wxCENTRE);
        return;
    }

    // SSH is connected

    // Prepare the command
    wxString cmd;
    cmd << "/bin/bash -c 'cd " << workingDirectory << " && " << command << "'";
    clGetManager()->ClearOutputTab(kOutputTab_Build);
    clGetManager()->AppendOutputTabText(
        kOutputTab_Build, wxString() << "Remote build started using ssh account: " << account.GetAccountName() << "\n");
    clGetManager()->AppendOutputTabText(kOutputTab_Build, cmd + "\n");
    m_remoteProcess = SSHRemoteProcess::Create(this, m_ssh, cmd, false);

    clBuildEvent eventStart(wxEVT_BUILD_STARTED);
    EventNotifier::Get()->AddPendingEvent(eventStart);

#else
    wxUnusedVar(sshAccount);
    wxUnusedVar(command);
    wxUnusedVar(workingDirectory);
#endif
}

void clRemoteBuilder::OnProcessOutput(clProcessEvent& event)
{
    clGetManager()->AppendOutputTabText(kOutputTab_Build, event.GetOutput());
}

void clRemoteBuilder::OnProcessTerminated(clProcessEvent& event)
{
    clGetManager()->AppendOutputTabText(kOutputTab_Build, "==== Done ====\n");
    wxDELETE(m_remoteProcess);

    clBuildEvent eventStopped(wxEVT_BUILD_ENDED);
    EventNotifier::Get()->AddPendingEvent(eventStopped);

#if USE_SFTP
    m_ssh.reset(nullptr);
#endif
}

void clRemoteBuilder::Stop() { m_remoteProcess->Terminate(); }
