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
    if(!m_goingDown) {
        Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &LanguageServerProtocol::OnProcessTerminated, this);
        Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &LanguageServerProtocol::OnProcessOutput, this);
    }
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &LanguageServerProtocol::OnFileSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_CLOSED, &LanguageServerProtocol::OnFileClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_LOADED, &LanguageServerProtocol::OnFileLoaded, this);
}

void LanguageServerProtocol::DoStart()
{
    m_process = ::CreateAsyncProcess(this, m_command, IProcessCreateDefault, m_workingDirectory);
    if(!m_process) { clWARNING() << "Failed to start Language Server:" << m_command; }
}

void LanguageServerProtocol::Start(const wxString& command, const wxString& workingDirectory,
                                   const wxArrayString& languages)
{
    if(m_process) { return; }
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

void LanguageServerProtocol::OnProcessOutput(clProcessEvent& event) { clDEBUG() << event.GetOutput(); }

void LanguageServerProtocol::Send(const wxString& message)
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
