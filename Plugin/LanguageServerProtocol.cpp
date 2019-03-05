#include "LanguageServerProtocol.h"
#include "file_logger.h"
#include "processreaderthread.h"
#include "json_rpc/clJSONRPC.h"
#include "json_rpc/GotoDefinitionRequest.h"
#include "json_rpc/DidOpenTextDocumentRequest.h"
#include "json_rpc/DidCloseTextDocumentRequest.h"
#include "json_rpc/DidSaveTextDocumentRequest.h"
#include "json_rpc/DidChangeTextDocumentRequest.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include "fileextmanager.h"
#include "ieditor.h"
#include "globals.h"
#include "imanager.h"
#include <wx/stc/stc.h>

LanguageServerProtocol::LanguageServerProtocol()
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &LanguageServerProtocol::OnProcessTerminated, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &LanguageServerProtocol::OnProcessOutput, this);

    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &LanguageServerProtocol::OnFileSaved, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_CLOSED, &LanguageServerProtocol::OnFileClosed, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_LOADED, &LanguageServerProtocol::OnFileLoaded, this);
}

LanguageServerProtocol::~LanguageServerProtocol()
{
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &LanguageServerProtocol::OnProcessTerminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &LanguageServerProtocol::OnProcessOutput, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &LanguageServerProtocol::OnFileSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_CLOSED, &LanguageServerProtocol::OnFileClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_LOADED, &LanguageServerProtocol::OnFileLoaded, this);
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

wxString LanguageServerProtocol::GetLanguageId(const wxString& fn)
{
    FileExtManager::FileType type = FileExtManager::GetType(fn, FileExtManager::TypeText);
    switch(type) {
    case FileExtManager::TypeSourceC:
        return "c";
    case FileExtManager::TypeSourceCpp:
    case FileExtManager::TypeHeader:
        return "cpp";
    case FileExtManager::TypeJS:
        return "javascript";
    case FileExtManager::TypeDockerfile:
        return "dockerfile";
    case FileExtManager::TypeJava:
        return "java";
    case FileExtManager::TypePython:
        return "python";
    case FileExtManager::TypeMakefile:
        return "makefile";
    case FileExtManager::TypePhp:
        return "php";
    case FileExtManager::TypeSQL:
        return "sql";
    default:
        return "";
    }
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
        wxUnusedVar(languageId);
        FileChanged(filename, fileContent);
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

void LanguageServerProtocol::OnFileLoaded(clCommandEvent& event)
{
    event.Skip();
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(editor) {
        FileOpened(editor->GetFileName(), editor->GetCtrl()->GetText(), GetLanguageId(editor->GetFileName()));
    }
}

void LanguageServerProtocol::OnFileClosed(clCommandEvent& event)
{
    event.Skip();
    FileClosed(event.GetFileName());
}

void LanguageServerProtocol::DoClear() { m_filesSent.clear(); }

void LanguageServerProtocol::OnFileSaved(clCommandEvent& event)
{
    event.Skip();
    const wxString& filename = event.GetFileName();
    IEditor* editor = clGetManager()->FindEditor(filename);
    if(editor) {
        // We should have called here FileSaved(), however, clangd-7 does not support this yet...
        FileChanged(editor->GetFileName(), editor->GetCtrl()->GetText());
    }
}

void LanguageServerProtocol::FileChanged(const wxFileName& filename, const wxString& fileContent)
{
    json_rpc::DidChangeTextDocumentRequest req(filename, fileContent);
    req.Send(this);
}

void LanguageServerProtocol::FileSaved(const wxFileName& filename, const wxString& fileContent)
{
    json_rpc::DidSaveTextDocumentRequest req(filename, fileContent);
    req.Send(this);
}

bool LanguageServerProtocol::IsRunning() const { return m_process != nullptr; }
