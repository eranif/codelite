#include "LanguageServerProtocol.h"
#include "file_logger.h"
#include "processreaderthread.h"
#include "json_rpc/clJSONRPC.h"
#include "json_rpc/GotoDefinitionRequest.h"
#include "json_rpc/DidOpenTextDocumentRequest.h"
#include "json_rpc/DidCloseTextDocumentRequest.h"

LanguageServerProtocol::LanguageServerProtocol()
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &LanguageServerProtocol::OnProcessTerminated, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &LanguageServerProtocol::OnProcessOutput, this);
}

LanguageServerProtocol::~LanguageServerProtocol()
{
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &LanguageServerProtocol::OnProcessTerminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &LanguageServerProtocol::OnProcessOutput, this);
}

void LanguageServerProtocol::Start(const wxString& command, const wxString& workingDirectory)
{
    if(m_process) { return; }
    m_goingDown = false;
    m_command = command;
    m_workingDirectory = workingDirectory;
    m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault, workingDirectory);
    if(!m_process) { clWARNING() << "Failed to start Language Server:" << command; }
}

void LanguageServerProtocol::OnProcessTerminated(clProcessEvent& event)
{
    clDEBUG() << "Langauge Server Terminated";
    wxDELETE(m_process);
    DoClear();
    if(!m_goingDown) { Start(m_command, m_workingDirectory); }
}

void LanguageServerProtocol::OnProcessOutput(clProcessEvent& event) { clDEBUG() << event.GetOutput(); }

void LanguageServerProtocol::Send(const wxString& message)
{
    if(m_process) { m_process->Write(message); }
}

void LanguageServerProtocol::Stop()
{
    m_goingDown = true;
    if(m_process) { m_process->Terminate(); }
}

//===--------------------------------------------------
// Protocol implementation
//===--------------------------------------------------

void LanguageServerProtocol::FindDefinition(const wxFileName& filename, size_t line, size_t column)
{
    json_rpc::GotoDefinitionRequest req(filename, line, column);
    req.Send(this);
}

void LanguageServerProtocol::FileOpened(const wxFileName& filename, const wxString& fileContent,
                                        const wxString& languageId)
{
    if(m_filesSent.count(filename.GetFullPath())) {
        // The file is already opened on the server side, so just notify about 'change' event
        // FileChanged(filename, fileContent, languageId);
    } else {

        json_rpc::DidOpenTextDocumentRequest req(filename, fileContent, languageId);
        req.Send(this);
        m_filesSent.insert(filename.GetFullPath());
    }
}

void LanguageServerProtocol::FileClosed(const wxFileName& filename)
{
    if(m_filesSent.count(filename.GetFullPath()) == 0) {
        clDEBUG() << "LanguageServerProtocol::FileClosed(): file" << filename << "is not opened";
        return;
    }

    json_rpc::DidCloseTextDocumentRequest req(filename);
    req.Send(this);
    m_filesSent.erase(filename.GetFullPath());
}

void LanguageServerProtocol::OnFileLoaded(clCommandEvent& event) { event.Skip(); }

void LanguageServerProtocol::OnFileClosed(clCommandEvent& event) { event.Skip(); }

void LanguageServerProtocol::DoClear() { m_filesSent.clear(); }
