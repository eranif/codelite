#include "clLanguageServer.h"
#include "file_logger.h"
#include "processreaderthread.h"

clLanguageServer::clLanguageServer()
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clLanguageServer::OnProcessTerminated, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clLanguageServer::OnProcessOutput, this);
}

clLanguageServer::~clLanguageServer()
{
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clLanguageServer::OnProcessTerminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clLanguageServer::OnProcessOutput, this);
}

void clLanguageServer::Start(const wxString& command, const wxString& workingDirectory)
{
    if(m_process) { return; }
    m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault, workingDirectory);
    if(!m_process) { clWARNING() << "Failed to start Language Server:" << command; }
}

void clLanguageServer::FindDefinition(const wxFileName& filename, size_t line, size_t column)
{
    wxUnusedVar(filename);
    wxUnusedVar(line);
    wxUnusedVar(column);
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

    clJSONRPCRequest req;
    req.SetMethod("textDocument/definition");

    // Build the params part of the request
    clJSONRPC_Params params;
    clJSONRPC_TextDocumentIdentifier text;
    clJSONRPC_Position pos;
    text.SetFilename(filename);
    params.AddParam("textDocument", text);

    pos.SetCharacter(column).SetLine(line);
    params.AddParam("position", pos);

    // Set the params
    req.SetParams(params);
}

void clLanguageServer::OnProcessTerminated(clProcessEvent& event) { clDEBUG() << "Langauge Server Terminated"; }

void clLanguageServer::OnProcessOutput(clProcessEvent& event) { clDEBUG() << event.GetOutput(); }

void clLanguageServer::Send(const wxString& message)
{
    if(m_process) { m_process->Write(message); }
}
