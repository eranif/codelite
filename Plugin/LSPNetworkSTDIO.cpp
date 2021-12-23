#include "LSPNetworkSTDIO.h"

#include "ChildProcess.h"
#include "JSON.h"
#include "asyncprocess.h"
#include "cl_exception.h"
#include "clcommandlineparser.h"
#include "dirsaver.h"
#include "file_logger.h"
#include "processreaderthread.h"
#include "sftp_settings.h"

#include <sstream>

using namespace std;
static const char separator_str[] = "\n************\n";
static size_t separator_str_len = sizeof(separator_str) - 1;

LSPNetworkSTDIO::LSPNetworkSTDIO()
{
    wxFileName logfile(clStandardPaths::Get().GetUserDataDir(), "codelite-lsp.log");
    m_log.Open(logfile.GetFullPath(), "a+b");
}

LSPNetworkSTDIO::~LSPNetworkSTDIO()
{
    Close();
    m_log.Close();
}

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
        m_server->Write(data);
        if(FileLogger::CanLog(FileLogger::Dbg)) {
            m_log.Write(separator_str, separator_str_len);
            m_log.Write(data.c_str(), data.length());
            m_log.Flush();
        }
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
    clCommandEvent evt(wxEVT_LSP_NET_DATA_READY);
    evt.SetString(dataRead);

    if(FileLogger::CanLog(FileLogger::Dbg)) {
        m_log.Write(separator_str, separator_str_len);
        m_log.Write(dataRead);
        m_log.Flush();
    }

    AddPendingEvent(evt);
}

void LSPNetworkSTDIO::OnProcessStderr(clProcessEvent& event)
{
    if(FileLogger::CanLog(FileLogger::Dbg)) {
        m_log.Write(wxString() << "[**STDERR**]");
        m_log.Write(event.GetOutput());
    }
}
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

    if(FileLogger::CanLog(FileLogger::Dbg)) {
        m_log.Write(wxString("\n\n  =============== Process Started ===============  \n\n"));
        m_log.Flush();
    }
}

void LSPNetworkSTDIO::BindEvents()
{
    m_server->Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &LSPNetworkSTDIO::OnProcessOutput, this);
    m_server->Bind(wxEVT_ASYNC_PROCESS_STDERR, &LSPNetworkSTDIO::OnProcessStderr, this);
    m_server->Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &LSPNetworkSTDIO::OnProcessTerminated, this);
}