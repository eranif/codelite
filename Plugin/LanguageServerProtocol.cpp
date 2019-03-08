#include "LanguageServerProtocol.h"
#include "file_logger.h"
#include "processreaderthread.h"
#include "LSP/clJSONRPC.h"
#include "LSP/GotoDefinitionRequest.h"
#include "LSP/DidOpenTextDocumentRequest.h"
#include "LSP/DidCloseTextDocumentRequest.h"
#include "LSP/DidSaveTextDocumentRequest.h"
#include "LSP/DidChangeTextDocumentRequest.h"
#include "LSP/InitializeRequest.h"
#include "LSP/ResponseMessage.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include "fileextmanager.h"
#include "ieditor.h"
#include "globals.h"
#include "imanager.h"
#include <wx/stc/stc.h>

LanguageServerProtocol::LanguageServerProtocol(wxEvtHandler* owner)
    : m_owner(owner)
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &LanguageServerProtocol::OnProcessTerminated, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &LanguageServerProtocol::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_STDERR, &LanguageServerProtocol::OnProcessStderr, this);

    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &LanguageServerProtocol::OnFileSaved, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_CLOSED, &LanguageServerProtocol::OnFileClosed, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_LOADED, &LanguageServerProtocol::OnFileLoaded, this);
}

LanguageServerProtocol::~LanguageServerProtocol()
{
    if(!m_goingDown) {
        Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &LanguageServerProtocol::OnProcessTerminated, this);
        Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &LanguageServerProtocol::OnProcessOutput, this);
        Unbind(wxEVT_ASYNC_PROCESS_STDERR, &LanguageServerProtocol::OnProcessStderr, this);
    }
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &LanguageServerProtocol::OnFileSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_CLOSED, &LanguageServerProtocol::OnFileClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_LOADED, &LanguageServerProtocol::OnFileLoaded, this);
    DoClear();
}

void LanguageServerProtocol::DoStart()
{
    m_process = ::CreateAsyncProcess(this, m_command, IProcessCreateDefault | IProcessStderrEvent, m_workingDirectory);
    if(!m_process) {
        clWARNING() << "Failed to start Language Server:" << m_command;
        return;
    }
    // The process started successfully
    // Send the 'initialize' request
    LSP::InitializeRequest::Ptr_t req(new LSP::InitializeRequest());
    req->Send(this);
    m_requestsSent.insert({ req->GetId(), req });
}

void LanguageServerProtocol::Start(const wxString& command, const wxString& workingDirectory,
                                   const wxArrayString& languages)
{
    if(m_process) { return; }
    DoClear();
    m_languages.clear();
    std::for_each(languages.begin(), languages.end(), [&](const wxString& lang) { m_languages.insert(lang); });
    m_goingDown = false;
    m_command = command;
    m_workingDirectory = workingDirectory;
    DoStart();
}

void LanguageServerProtocol::OnProcessTerminated(clProcessEvent& event)
{
    clDEBUG() << "Langauge Server Terminated";
    wxDELETE(m_process);
    DoClear();
    if(!m_goingDown) { DoStart(); }
}

void LanguageServerProtocol::OnProcessStderr(clProcessEvent& event) { clDEBUG1() << event.GetOutput(); }

void LanguageServerProtocol::OnProcessOutput(clProcessEvent& event)
{
    clDEBUG() << event.GetOutput();
    const wxString& buffer = event.GetOutput();
    m_outputBuffer << buffer;

    while(true) {
        LSP::ResponseMessage res(m_outputBuffer);
        if(res.IsOk() && m_requestsSent.count(res.GetId())) {
            // let the originating request to handle it
            m_requestsSent[res.GetId()]->OnReponse(res, m_owner);
            m_requestsSent.erase(res.GetId());

            // If we got more in the buffer, try to process another message
            if(!m_outputBuffer.IsEmpty()) { continue; }
        }
        break;
    }
}

void LanguageServerProtocol::Send(const std::string& message)
{
    if(m_process) { m_process->Write(message); }
}

void LanguageServerProtocol::Stop(bool goingDown)
{
    m_goingDown = goingDown;
    if(m_goingDown) {
        // Unbound the events so we dont get the 'terminated' process event
        Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &LanguageServerProtocol::OnProcessTerminated, this);
        Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &LanguageServerProtocol::OnProcessOutput, this);
        Unbind(wxEVT_ASYNC_PROCESS_STDERR, &LanguageServerProtocol::OnProcessStderr, this);
    }
    if(m_process) {
        m_process->Terminate();
        if(m_goingDown) {
            wxString dummy;
            m_process->WaitForTerminate(dummy);
            wxDELETE(m_process);
        }
    }
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
    LSP::GotoDefinitionRequest::Ptr_t req(new LSP::GotoDefinitionRequest(filename, line, column));
    req->Send(this);
    m_requestsSent.insert({ req->GetId(), req });
}

void LanguageServerProtocol::FileOpened(const wxFileName& filename, const wxString& fileContent,
                                        const wxString& languageId)
{
    if(m_filesSent.count(filename.GetFullPath())) {
        // The file is already opened on the server side, so just notify about 'change' event
        wxUnusedVar(languageId);
        FileChanged(filename, fileContent);
    } else {

        LSP::DidOpenTextDocumentRequest req(filename, fileContent, languageId);
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

    LSP::DidCloseTextDocumentRequest req(filename);
    req.Send(this);
    m_filesSent.erase(filename.GetFullPath());
}

void LanguageServerProtocol::OnFileLoaded(clCommandEvent& event)
{
    event.Skip();
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(editor) {
        if(ShouldHandleFile(editor)) {
            FileOpened(editor->GetFileName(), editor->GetCtrl()->GetText(), GetLanguageId(editor->GetFileName()));
        }
    }
}

void LanguageServerProtocol::OnFileClosed(clCommandEvent& event)
{
    event.Skip();
    FileClosed(event.GetFileName());
}

void LanguageServerProtocol::DoClear()
{
    m_filesSent.clear();
    m_requestsSent.clear();
    m_outputBuffer.clear();
}

void LanguageServerProtocol::OnFileSaved(clCommandEvent& event)
{
    event.Skip();
    const wxString& filename = event.GetFileName();
    IEditor* editor = clGetManager()->FindEditor(filename);
    if(editor) {
        if(ShouldHandleFile(editor)) { FileChanged(editor->GetFileName(), editor->GetCtrl()->GetText()); }
    }
}

void LanguageServerProtocol::FileChanged(const wxFileName& filename, const wxString& fileContent)
{
    LSP::DidChangeTextDocumentRequest req(filename, fileContent);
    req.Send(this);
}

void LanguageServerProtocol::FileSaved(const wxFileName& filename, const wxString& fileContent)
{
    LSP::DidSaveTextDocumentRequest req(filename, fileContent);
    req.Send(this);
}

bool LanguageServerProtocol::IsRunning() const { return m_process != nullptr; }

bool LanguageServerProtocol::CanHandle(const wxFileName& filename) const
{
    wxString lang = GetLanguageId(filename);
    return m_languages.count(lang) != 0;
}

const std::set<wxString>& LanguageServerProtocol::GetSupportedLanguages()
{
    static std::set<wxString> S;
    if(S.empty()) {
        S.insert("bat");
        S.insert("bibtex");
        S.insert("clojure");
        S.insert("coffeescript");
        S.insert("c");
        S.insert("cpp");
        S.insert("csharp");
        S.insert("css");
        S.insert("diff");
        S.insert("dart");
        S.insert("dockerfile");
        S.insert("fsharp");
        S.insert("git-commit");
        S.insert("git-rebase");
        S.insert("go");
        S.insert("groovy");
        S.insert("handlebars");
        S.insert("html");
        S.insert("ini");
        S.insert("java");
        S.insert("javascript");
        S.insert("json");
        S.insert("latex");
        S.insert("less");
        S.insert("lua");
        S.insert("makefile");
        S.insert("markdown");
        S.insert("objective-c");
        S.insert("objective-cpp");
        S.insert("perl and perl6");
        S.insert("php");
        S.insert("powershell");
        S.insert("jade");
        S.insert("python");
        S.insert("r");
        S.insert("razor");
        S.insert("ruby");
        S.insert("rust");
        S.insert("scss");
        S.insert("sass");
        S.insert("scala");
        S.insert("shaderlab");
        S.insert("shellscript");
        S.insert("sql");
        S.insert("swift");
        S.insert("typescript");
        S.insert("tex");
        S.insert("vb");
        S.insert("xml");
        S.insert("xsl");
        S.insert("yaml");
    }
    return S;
}

void LanguageServerProtocol::Restart()
{
    if(IsRunning()) {
        Stop(false);
    } else {
        DoStart();
    }
}

bool LanguageServerProtocol::ShouldHandleFile(const wxFileName& fn) const
{
    wxString lang = GetLanguageId(fn);
    return (m_languages.count(lang) != 0);
}

bool LanguageServerProtocol::ShouldHandleFile(IEditor* editor) const
{
    return editor && ShouldHandleFile(editor->GetFileName());
}
