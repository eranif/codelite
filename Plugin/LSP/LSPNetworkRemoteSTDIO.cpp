#include "LSPNetworkRemoteSTDIO.hpp"
#if USE_SFTP

#include "LSP/basic_types.h"
#include "clModuleLogger.hpp"
#include "clRemoteHost.hpp"

INITIALISE_MODULE_LOG(LOG, "LSPNetworkRemoteSTDIO", "lsp.log");

LSPNetworkRemoteSTDIO::LSPNetworkRemoteSTDIO() { LSP::Initialise(); }
LSPNetworkRemoteSTDIO::~LSPNetworkRemoteSTDIO() { m_process.reset(); }

void LSPNetworkRemoteSTDIO::Close() { DoClose(); }
void LSPNetworkRemoteSTDIO::Open(const LSPStartupInfo& info)
{
    m_startupInfo = info;

    // Start the LSP server first
    Close();
    DoStartRemoteProcess();
}

void LSPNetworkRemoteSTDIO::DoClose()
{
    if(m_process) {
        m_process->Terminate();
        m_process = nullptr;
    }
}

void LSPNetworkRemoteSTDIO::DoStartRemoteProcess()
{
    LOG_DEBUG(LOG) << "Starting remote process:" << endl;
    LOG_DEBUG(LOG) << m_startupInfo.GetLspServerCommand() << endl;
    LOG_DEBUG(LOG) << m_startupInfo.GetWorkingDirectory() << endl;
    for(const auto& p : m_startupInfo.GetEnv()) {
        LOG_DEBUG(LOG) << p.first << "=" << p.second << endl;
    }

    m_process = clRemoteHost::Instance()->run_interactive_process(
        this, m_startupInfo.GetLspServerCommand(), IProcessStderrEvent, m_startupInfo.GetWorkingDirectory(),
        m_startupInfo.GetEnv());
    BindEvents();

    clCommandEvent evtReady(m_process ? wxEVT_LSP_NET_CONNECTED : wxEVT_LSP_NET_ERROR);
    AddPendingEvent(evtReady);
}

void LSPNetworkRemoteSTDIO::Send(const std::string& data)
{
    LOG_IF_DEBUG { LOG_DEBUG(LOG) << ">" << data << endl; }
    if(!m_process) {
        LOG_WARNING(LOG) << "remote server is not running" << endl;
        return;
    }
    m_process->WriteRaw(data);
}

bool LSPNetworkRemoteSTDIO::IsConnected() const { return m_process != nullptr; }

void LSPNetworkRemoteSTDIO::OnProcessTerminated(clProcessEvent& event)
{
    m_process.reset();
    LOG_IF_DEBUG { LSP_DEBUG() << "program terminated:" << m_startupInfo.GetLspServerCommand() << endl; }
    LOG_IF_TRACE { LSP_TRACE() << event.GetStringRaw() << endl; }
    clCommandEvent evt(wxEVT_LSP_NET_ERROR);
    AddPendingEvent(evt);
}

void LSPNetworkRemoteSTDIO::OnProcessOutput(clProcessEvent& event)
{
    const wxString& dataRead = event.GetOutput();
    const std::string& dataReadRaw = event.GetOutputRaw();

    clCommandEvent evt(wxEVT_LSP_NET_DATA_READY);
    evt.SetString(dataRead);
    evt.SetStringRaw(dataReadRaw);

    LOG_IF_TRACE { LSP_TRACE() << dataRead << endl; }
    AddPendingEvent(evt);
}

void LSPNetworkRemoteSTDIO::OnProcessStderr(clProcessEvent& event)
{
    LOG_IF_TRACE { LSP_TRACE() << "[**STDERR**]" << event.GetOutput() << endl; }
}

void LSPNetworkRemoteSTDIO::BindEvents()
{
    if(!m_process) {
        LOG_WARNING(LOG) << "failed to bind events. process is not running" << endl;
        return;
    }
    if(!m_eventsBound) {
        m_eventsBound = true;
        m_process->Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &LSPNetworkRemoteSTDIO::OnProcessOutput, this);
        m_process->Bind(wxEVT_ASYNC_PROCESS_STDERR, &LSPNetworkRemoteSTDIO::OnProcessStderr, this);
        m_process->Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &LSPNetworkRemoteSTDIO::OnProcessTerminated, this);
    }
}
#endif // USE_SFTP
