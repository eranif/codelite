#include "clLanguageServer.h"
#include "file_logger.h"
#include "processreaderthread.h"
#include "json_rpc/clJSONRPC.h"
#include "json_rpc/GotoDefinitionRequest.h"

clLanguageServer::clLanguageServer()
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clLanguageServer::OnProcessTerminated, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clLanguageServer::OnProcessOutput, this);
}

clLanguageServer::~clLanguageServer()
{
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clLanguageServer::OnProcessTerminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &clLanguageServer::OnProcessOutput, this);
}

void clLanguageServer::Start(const wxString& command, const wxString& workingDirectory)
{
    if(m_process) { return; }
    m_goingDown = false;
    m_command = command;
    m_workingDirectory = workingDirectory;
    m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault, workingDirectory);
    if(!m_process) { clWARNING() << "Failed to start Language Server:" << command; }
}

void clLanguageServer::FindDefinition(const wxFileName& filename, size_t line, size_t column)
{
    //{
    //  "jsonrpc": "2.0",
    //  "id": 1,
    //  "method": "textDocument/definition",
    //  "params": {
    //    "textDocument": {
    //      "uri": "file:///Users/NSHipster/Example.swift"
    //    },
    //    "position": {
    //      "line": 1,
    //      "character": 13
    //    }
    //  }
    //}
    json_rpc::GotoDefinitionRequest req(filename, line, column);
    req.Send(this);
}

void clLanguageServer::OnProcessTerminated(clProcessEvent& event)
{
    clDEBUG() << "Langauge Server Terminated";
    wxDELETE(m_process);
    if(!m_goingDown) { Start(m_command, m_workingDirectory); }
}

void clLanguageServer::OnProcessOutput(clProcessEvent& event) { clDEBUG() << event.GetOutput(); }

void clLanguageServer::Send(const wxString& message)
{
    if(m_process) { m_process->Write(message); }
}

void clLanguageServer::Stop()
{
    m_goingDown = true;
    if(m_process) { m_process->Terminate(); }
}
