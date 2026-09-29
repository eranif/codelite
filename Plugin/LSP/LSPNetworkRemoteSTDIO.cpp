#include "LSPNetworkRemoteSTDIO.hpp"
#if USE_SFTP

#include "LSP/basic_types.h"
#include "clModuleLogger.hpp"
#include "clRemoteHost.hpp"
#include "event_notifier.h"
#include "macromanager.h"

INITIALISE_MODULE_LOG(LOG, "LSPNetworkRemoteSTDIO", "lsp.log");

LSPNetworkRemoteSTDIO::LSPNetworkRemoteSTDIO()
{
    LSP::Initialise();
    EventNotifier::Get()->Bind(wxEVT_REMOTEHOST_SESSION_CREATED, &LSPNetworkRemoteSTDIO::OnRemoteSessionCreateOk, this);
    EventNotifier::Get()->Bind(
        wxEVT_REMOTEHOST_SESSION_CREATE_ERROR, &LSPNetworkRemoteSTDIO::OnRemoteSessionCreateError, this);
}

LSPNetworkRemoteSTDIO::~LSPNetworkRemoteSTDIO()
{
    m_process.reset();
    EventNotifier::Get()->Unbind(
        wxEVT_REMOTEHOST_SESSION_CREATED, &LSPNetworkRemoteSTDIO::OnRemoteSessionCreateOk, this);
    EventNotifier::Get()->Unbind(
        wxEVT_REMOTEHOST_SESSION_CREATE_ERROR, &LSPNetworkRemoteSTDIO::OnRemoteSessionCreateError, this);
}

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
    if (m_process) {
        m_process->Terminate();
        m_process = nullptr;
    }
    m_createRequestId.reset();
}

void LSPNetworkRemoteSTDIO::DoStartRemoteProcess()
{
    LOG_DEBUG(LOG()) << "Starting remote process:" << endl;
    LOG_DEBUG(LOG()) << m_startupInfo.GetLspServerCommand() << endl;
    LOG_DEBUG(LOG()) << m_startupInfo.GetWorkingDirectory() << endl;

    clEnvList_t expanded_env;
    for (const auto& p : m_startupInfo.GetEnv()) {
        auto key = p.first;
        auto value = MacroManager::Instance()->ExpandNoEnv(p.second, wxEmptyString, wxEmptyString);
        expanded_env.push_back({key, value});
        LOG_DEBUG(LOG()) << key << "=" << value << endl;
    }

    m_startupInfo.SetEnv(expanded_env);
    auto result = clRemoteHost::Instance()->AsyncCreateSession();
    if (!result.ok()) {
        LOG_WARNING(LOG()) << "Failed to create Remote SSH session." << result.error_message() << endl;
        return;
    }
    m_createRequestId = result.value();
}

void LSPNetworkRemoteSTDIO::OnRemoteSessionCreateOk(clRemoteHostEvent& event)
{
    if (!m_createRequestId || event.GetRequestId() != *m_createRequestId) {
        // Not ours to handle
        event.Skip();
        return;
    }

    // Ours... this handler runs on the main thread and only while this object is alive, so it is safe
    // to use `this` as the parent of the process here.
    m_createRequestId.reset();
    auto result = clRemoteHost::Instance()->CreateInteractiveProcess(this,
                                                                     event.GetSession(),
                                                                     m_startupInfo.GetLspServerCommand(),
                                                                     IProcessStderrEvent,
                                                                     m_startupInfo.GetWorkingDirectory(),
                                                                     m_startupInfo.GetEnv());
    if (!result.ok()) {
        LOG_WARNING(LOG()) << "Failed to create Remote STDIO process." << result.error_message() << endl;
        clCommandEvent evtError(wxEVT_LSP_NET_ERROR);
        AddPendingEvent(evtError);
        return;
    }
    m_process = result.value();

    BindEvents();
    clCommandEvent evtReady(wxEVT_LSP_NET_CONNECTED);
    AddPendingEvent(evtReady);
}

void LSPNetworkRemoteSTDIO::OnRemoteSessionCreateError(clRemoteHostEvent& event)
{
    if (!m_createRequestId || event.GetRequestId() != *m_createRequestId) {
        // Not ours to handle
        event.Skip();
        return;
    }

    // Ours...
    m_createRequestId.reset();
    clCommandEvent evtReady(wxEVT_LSP_NET_ERROR);
    AddPendingEvent(evtReady);
}

void LSPNetworkRemoteSTDIO::Send(const std::string& data)
{
    LOG_IF_DEBUG { LOG_DEBUG(LOG()) << ">" << data << endl; }
    if (!m_process) {
        LOG_WARNING(LOG()) << "remote server is not running" << endl;
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
    if (!m_process) {
        LOG_WARNING(LOG()) << "failed to bind events. process is not running" << endl;
        return;
    }
    if (!m_eventsBound) {
        m_eventsBound = true;
        m_process->Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &LSPNetworkRemoteSTDIO::OnProcessOutput, this);
        m_process->Bind(wxEVT_ASYNC_PROCESS_STDERR, &LSPNetworkRemoteSTDIO::OnProcessStderr, this);
        m_process->Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &LSPNetworkRemoteSTDIO::OnProcessTerminated, this);
    }
}
#endif // USE_SFTP
