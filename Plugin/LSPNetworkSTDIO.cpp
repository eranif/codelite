#include "LSPNetworkSTDIO.h"
#include "JSON.h"
#include <sstream>
#include "file_logger.h"
#include "clcommandlineparser.h"
#include "ChildProcess.h"
#include "processreaderthread.h"
#include "dirsaver.h"

LSPNetworkSTDIO::LSPNetworkSTDIO() {}

LSPNetworkSTDIO::~LSPNetworkSTDIO() { Close(); }

void LSPNetworkSTDIO::Close() { wxDELETE(m_server); }

void LSPNetworkSTDIO::Open(const LSPStartupInfo& siInfo)
{
    m_startupInfo = siInfo;

    // Start the LSP server first
    Close();

    m_server = new ChildProcess();
    m_server->Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &LSPNetworkSTDIO::OnProcessOutput, this);
    m_server->Bind(wxEVT_ASYNC_PROCESS_STDERR, &LSPNetworkSTDIO::OnProcessStderr, this);
    m_server->Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &LSPNetworkSTDIO::OnProcessTerminated, this);

    DirSaver ds;
    if(!siInfo.GetWorkingDirectory().IsEmpty()) { ::wxSetWorkingDirectory(siInfo.GetWorkingDirectory()); }
    m_server->Start(siInfo.GetLspServerCommand());
    
    clCommandEvent evtReady(wxEVT_LSP_NET_CONNECTED);
    AddPendingEvent(evtReady);
}

void LSPNetworkSTDIO::Send(const std::string& data)
{
    if(m_server) {
        clDEBUG() << "LSPNetworkSTDIO:\n" << data;
        m_server->Write(data);
    } else {
        clDEBUG() << "LSPNetworkSTDIO: no process !?";
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
    AddPendingEvent(evt);
}

void LSPNetworkSTDIO::OnProcessStderr(clProcessEvent& event) { clDEBUG() << event.GetOutput(); }
