#include "ChildProcess.h"
#include "JSON.h"
#include "LSPNetworkSTDIO.h"
#include "asyncprocess.h"
#include "cl_exception.h"
#include "clcommandlineparser.h"
#include "dirsaver.h"
#include "file_logger.h"
#include "processreaderthread.h"
#include "sftp_settings.h"
#include <sstream>

LSPNetworkSTDIO::LSPNetworkSTDIO() {}

LSPNetworkSTDIO::~LSPNetworkSTDIO() { Close(); }

void LSPNetworkSTDIO::Close() { wxDELETE(m_server); }

void LSPNetworkSTDIO::Open(const LSPStartupInfo& siInfo)
{
    m_startupInfo = siInfo;

    // Start the LSP server first
    Close();
    DoStartLocalProcess();
}

void LSPNetworkSTDIO::Send(const std::string& data)
{
    if(m_server) {
        clDEBUG() << "LSPNetworkSTDIO:\n" << data << endl;
        m_server->Write(data);
    } else {
        clWARNING() << "LSPNetworkSTDIO: no process !?" << endl;
    }
}

bool LSPNetworkSTDIO::IsConnected() const { return m_server != nullptr; }

void LSPNetworkSTDIO::OnProcessTerminated(clProcessEvent& event)
{
    wxDELETE(m_server);
    clDEBUG() << "LSPNetworkSTDIO: program terminated:" << m_startupInfo.GetLspServerCommand();
    clCommandEvent evt(wxEVT_LSP_NET_ERROR);
    AddPendingEvent(evt);
}

void LSPNetworkSTDIO::OnProcessOutput(clProcessEvent& event)
{
    const wxString& dataRead = event.GetOutput();
    clDEBUG() << "[stdout]" << event.GetOutput() << endl;
    clCommandEvent evt(wxEVT_LSP_NET_DATA_READY);
    evt.SetString(dataRead);
    AddPendingEvent(evt);
}

void LSPNetworkSTDIO::OnProcessStderr(clProcessEvent& event) { clDEBUG() << "[stderr]" << event.GetOutput() << endl; }
using namespace std;
void LSPNetworkSTDIO::DoStartLocalProcess()
{
    m_server = new ChildProcess();
    BindEvents();

    DirSaver ds;
    if(!m_startupInfo.GetWorkingDirectory().IsEmpty()) {
        ::wxSetWorkingDirectory(m_startupInfo.GetWorkingDirectory());
    }
    wxArrayString args = m_startupInfo.GetLspServerCommand();
    if(m_startupInfo.GetFlags() & LSPStartupInfo::kRemoteLSP) {
#if USE_SFTP
        // wrap the command in ssh
        vector<wxString> command = { "ssh", "-o", "ServerAliveInterval=10", "-o", "StrictHostKeyChecking=no" };
        // add user@host
        SFTPSettings s;
        SSHAccountInfo accountInfo;
        s.Load();
        if(!s.GetAccount(m_startupInfo.GetAccountName(), accountInfo)) {
            throw clException(_("LSP: could not locate SSH account ") + m_startupInfo.GetAccountName());
        }
        command.push_back(accountInfo.GetUsername() + "@" + accountInfo.GetHost());
        command.push_back("-p");
        command.push_back(wxString() << accountInfo.GetPort());
        command.push_back(BuildCommand(args));
        args.clear();
        args.reserve(command.size());
        for(const wxString& arg : command) {
            args.Add(arg);
        }
#endif
    }
    m_server->Start(args);
    clCommandEvent evtReady(wxEVT_LSP_NET_CONNECTED);
    AddPendingEvent(evtReady);
}

void LSPNetworkSTDIO::BindEvents()
{
    m_server->Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &LSPNetworkSTDIO::OnProcessOutput, this);
    m_server->Bind(wxEVT_ASYNC_PROCESS_STDERR, &LSPNetworkSTDIO::OnProcessStderr, this);
    m_server->Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &LSPNetworkSTDIO::OnProcessTerminated, this);
}