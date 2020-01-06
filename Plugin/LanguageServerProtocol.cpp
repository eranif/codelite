#include "LSP/CompletionRequest.h"
#include "LSP/DidChangeTextDocumentRequest.h"
#include "LSP/DidCloseTextDocumentRequest.h"
#include "LSP/DidOpenTextDocumentRequest.h"
#include "LSP/DidSaveTextDocumentRequest.h"
#include "LSP/GotoDeclarationRequest.h"
#include "LSP/GotoDefinitionRequest.h"
#include "LSP/GotoImplementationRequest.h"
#include "LSP/InitializeRequest.h"
#include "LSP/LSPEvent.h"
#include "LSP/Request.h"
#include "LSP/ResponseError.h"
#include "LSP/ResponseMessage.h"
#include "LSP/SignatureHelpRequest.h"
#include "LSPNetworkSTDIO.h"
#include "LSPNetworkSocketClient.h"
#include "LanguageServerProtocol.h"
#include "clWorkspaceManager.h"
#include "cl_exception.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "processreaderthread.h"
#include <iomanip>
#include <sstream>
#include <wx/filesys.h>
#include <wx/stc/stc.h>

LanguageServerProtocol::LanguageServerProtocol(const wxString& name, eNetworkType netType, wxEvtHandler* owner,
                                               IPathConverter::Ptr_t pathConverter)
    : ServiceProvider(wxString() << "LSP: " << name, eServiceType::kCodeCompletion)
    , m_name(name)
    , m_owner(owner)
    , m_pathConverter(pathConverter)
{
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &LanguageServerProtocol::OnFileSaved, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_CLOSED, &LanguageServerProtocol::OnFileClosed, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_LOADED, &LanguageServerProtocol::OnFileLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &LanguageServerProtocol::OnEditorChanged, this);

    Bind(wxEVT_CC_FIND_SYMBOL, &LanguageServerProtocol::OnFindSymbol, this);
    Bind(wxEVT_CC_FIND_SYMBOL_DECLARATION, &LanguageServerProtocol::OnFindSymbolDecl, this);
    Bind(wxEVT_CC_FIND_SYMBOL_DEFINITION, &LanguageServerProtocol::OnFindSymbolImpl, this);
    Bind(wxEVT_CC_CODE_COMPLETE, &LanguageServerProtocol::OnCodeComplete, this);
    Bind(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &LanguageServerProtocol::OnFunctionCallTip, this);

    // Use sockets here
    switch(netType) {
    case eNetworkType::kStdio:
        m_network.reset(new LSPNetworkSTDIO());
        break;
    case eNetworkType::kTcpIP:
        m_network.reset(new LSPNetworkSocketClient());
        break;
    }
    m_network->Bind(wxEVT_LSP_NET_DATA_READY, &LanguageServerProtocol::OnNetDataReady, this);
    m_network->Bind(wxEVT_LSP_NET_ERROR, &LanguageServerProtocol::OnNetError, this);
    m_network->Bind(wxEVT_LSP_NET_CONNECTED, &LanguageServerProtocol::OnNetConnected, this);
}

LanguageServerProtocol::~LanguageServerProtocol()
{
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &LanguageServerProtocol::OnFileSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_CLOSED, &LanguageServerProtocol::OnFileClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_LOADED, &LanguageServerProtocol::OnFileLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &LanguageServerProtocol::OnEditorChanged, this);
    Unbind(wxEVT_CC_FIND_SYMBOL, &LanguageServerProtocol::OnFindSymbol, this);
    Unbind(wxEVT_CC_FIND_SYMBOL_DECLARATION, &LanguageServerProtocol::OnFindSymbolDecl, this);
    Unbind(wxEVT_CC_FIND_SYMBOL_DEFINITION, &LanguageServerProtocol::OnFindSymbolImpl, this);
    Unbind(wxEVT_CC_CODE_COMPLETE, &LanguageServerProtocol::OnCodeComplete, this);
    Unbind(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &LanguageServerProtocol::OnFunctionCallTip, this);
    DoClear();
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
    case FileExtManager::TypeCSS:
        return "css";
    case FileExtManager::TypeCMake:
        return "cmake";
    case FileExtManager::TypeLua:
        return "lua";
    case FileExtManager::TypeRust:
        return "rust";
    default:
        return "";
    }
}

std::set<wxString> LanguageServerProtocol::GetSupportedLanguages()
{
    return { "bat",        "bibtex",     "clojure",     "coffeescript",  "c",
             "cpp",        "csharp",     "css",         "diff",          "dart",
             "dockerfile", "fsharp",     "git-commit",  "git-rebase",    "go",
             "groovy",     "handlebars", "html",        "ini",           "java",
             "javascript", "json",       "latex",       "less",          "lua",
             "makefile",   "markdown",   "objective-c", "objective-cpp", "perl and perl6",
             "php",        "powershell", "jade",        "python",        "r",
             "razor",      "ruby",       "rust",        "scss",          "sass",
             "scala",      "shaderlab",  "shellscript", "sql",           "swift",
             "typescript", "tex",        "vb",          "xml",           "xsl",
             "yaml" };
}

void LanguageServerProtocol::QueueMessage(LSP::MessageWithParams::Ptr_t request)
{
    if(!IsInitialized()) { return; }
    if(request->As<LSP::CompletionRequest>()) {
        m_lastCompletionRequestId = request->As<LSP::CompletionRequest>()->GetId();
    }
    m_Queue.Push(request);
    ProcessQueue();
}

bool LanguageServerProtocol::DoStart()
{
    DoClear();

    clDEBUG() << GetLogPrefix() << "Starting...";
    clDEBUG() << GetLogPrefix() << "Command:" << m_lspCommand;
    clDEBUG() << GetLogPrefix() << "Root folder:" << m_rootFolder;
    for(const wxString& lang : m_languages) {
        clDEBUG() << GetLogPrefix() << "Language:" << lang;
    }
    LSPStartupInfo info;
    info.SetLspServerCommand(m_lspCommand);
    info.SetWorkingDirectory(m_workingDirectory);
    info.SetConnectioString(m_connectionString);
    info.SetFlags(m_createFlags);
    try {
        m_network->Open(info);
        return true;
    } catch(clException& e) {
        clWARNING() << e.What();
        return false;
    }
}

bool LanguageServerProtocol::Start(const wxArrayString& lspCommand, const wxString& connectionString,
                                   const wxString& workingDirectory, const wxString& rootFolder,
                                   const wxArrayString& languages, size_t flags)
{
    if(IsRunning()) { return true; }
    DoClear();
    m_languages.clear();
    std::for_each(languages.begin(), languages.end(), [&](const wxString& lang) { m_languages.insert(lang); });
    m_lspCommand = lspCommand;
    m_workingDirectory = workingDirectory;
    m_rootFolder = rootFolder;
    m_connectionString = connectionString;
    m_createFlags = flags;
    return DoStart();
}

bool LanguageServerProtocol::Start()
{
    if(IsRunning()) { return true; }
    return DoStart();
}

void LanguageServerProtocol::DoClear()
{
    m_filesSent.clear();
    m_outputBuffer.clear();
    m_state = kUnInitialized;
    m_initializeRequestID = wxNOT_FOUND;
    m_Queue.Clear();
    m_lastCompletionRequestId = wxNOT_FOUND;
    // Destory the current connection
    m_network->Close();
}

bool LanguageServerProtocol::IsRunning() const { return m_network->IsConnected(); }

bool LanguageServerProtocol::CanHandle(const wxFileName& filename) const
{
    wxString lang = GetLanguageId(filename);
    return m_languages.count(lang) != 0;
}

bool LanguageServerProtocol::ShouldHandleFile(const wxFileName& fn) const
{
    wxString lang = GetLanguageId(fn);
    clDEBUG() << "Language ID for file" << fn << "->" << lang;
    return (m_languages.count(lang) != 0);
}

bool LanguageServerProtocol::ShouldHandleFile(IEditor* editor) const
{
    return editor && ShouldHandleFile(editor->GetFileName());
}

void LanguageServerProtocol::OnFunctionCallTip(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = dynamic_cast<IEditor*>(event.GetEditor());
    CHECK_PTR_RET(editor);
    if(CanHandle(editor->GetFileName())) {
        event.Skip(false);
        FunctionHelp(editor);
    }
}

void LanguageServerProtocol::OnCodeComplete(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = dynamic_cast<IEditor*>(event.GetEditor());
    CHECK_PTR_RET(editor);
    if(event.IsInsideCommentOrString()) { return; }
    if(CanHandle(editor->GetFileName())) {
        event.Skip(false);
        CodeComplete(editor);
    }
}

void LanguageServerProtocol::OnFindSymbolDecl(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = dynamic_cast<IEditor*>(event.GetEditor());
    CHECK_PTR_RET(editor);

    if(CanHandle(editor->GetFileName())) {
        // this event is ours to handle
        event.Skip(false);
        FindDeclaration(editor);
    }
}

void LanguageServerProtocol::OnFindSymbolImpl(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = dynamic_cast<IEditor*>(event.GetEditor());
    CHECK_PTR_RET(editor);

    if(CanHandle(editor->GetFileName())) {
        // this event is ours to handle
        event.Skip(false);
        FindImplementation(editor);
    }
}

void LanguageServerProtocol::OnFindSymbol(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = dynamic_cast<IEditor*>(event.GetEditor());
    CHECK_PTR_RET(editor);

    if(CanHandle(editor->GetFileName())) {
        // this event is ours to handle
        event.Skip(false);
        FindDefinition(editor);
    }
}

//===--------------------------------------------------
// Protocol implementation
//===--------------------------------------------------

void LanguageServerProtocol::FindDefinition(IEditor* editor)
{
    CHECK_PTR_RET(editor);
    CHECK_COND_RET(ShouldHandleFile(editor));

    // If the editor is modified, we need to tell the LSP to reparse the source file
    const wxFileName& filename = editor->GetFileName();
    if(m_filesSent.count(filename.GetFullPath()) && editor->IsModified()) {
        // we already sent this file over, ask for change parse
        std::string fileContent;
        editor->GetEditorTextRaw(fileContent);
        SendChangeRequest(filename, fileContent);

    } else if(m_filesSent.count(filename.GetFullPath()) == 0) {
        std::string fileContent;
        editor->GetEditorTextRaw(fileContent);
        SendOpenRequest(filename, fileContent, GetLanguageId(filename));
    }

    LSP::GotoDefinitionRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(new LSP::GotoDefinitionRequest(
        editor->GetFileName(), editor->GetCurrentLine(), editor->GetCtrl()->GetColumn(editor->GetCurrentPosition())));
    QueueMessage(req);
}

void LanguageServerProtocol::SendOpenRequest(const wxFileName& filename, const std::string& fileContent,
                                             const wxString& languageId)
{
    LSP::DidOpenTextDocumentRequest::Ptr_t req =
        LSP::MessageWithParams::MakeRequest(new LSP::DidOpenTextDocumentRequest(filename, fileContent, languageId));
#ifndef __WXOSX__
    req->SetStatusMessage(wxString() << GetLogPrefix() << " parsing file: " << filename.GetFullName());
#endif
    QueueMessage(req);
}

void LanguageServerProtocol::SendCloseRequest(const wxFileName& filename)
{
    if(m_filesSent.count(filename.GetFullPath()) == 0) {
        clDEBUG() << GetLogPrefix() << "LanguageServerProtocol::FileClosed(): file" << filename << "is not opened";
        return;
    }

    LSP::DidCloseTextDocumentRequest::Ptr_t req =
        LSP::MessageWithParams::MakeRequest(new LSP::DidCloseTextDocumentRequest(filename));
    QueueMessage(req);
    m_filesSent.erase(filename.GetFullPath());
}

void LanguageServerProtocol::SendChangeRequest(const wxFileName& filename, const std::string& fileContent)
{
    LSP::DidChangeTextDocumentRequest::Ptr_t req =
        LSP::MessageWithParams::MakeRequest(new LSP::DidChangeTextDocumentRequest(filename, fileContent));
#ifndef __WXOSX__
    req->SetStatusMessage(wxString() << GetLogPrefix() << " re-parsing file: " << filename.GetFullName());
#endif
    QueueMessage(req);
}

void LanguageServerProtocol::SendSaveRequest(const wxFileName& filename, const std::string& fileContent)
{
    // LSP::DidSaveTextDocumentRequest req(filename, fileContent);
    // req.Send(this);
    // clGetManager()->SetStatusMessage(wxString() << "[LSP] re-parsing file: " << filename.GetFullName(), 1);

    // For now: report a change event
    SendChangeRequest(filename, fileContent);
}

void LanguageServerProtocol::SendCodeCompleteRequest(const wxFileName& filename, size_t line, size_t column)
{
    if(ShouldHandleFile(filename)) {
        LSP::CompletionRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(
            new LSP::CompletionRequest(LSP::TextDocumentIdentifier(filename), LSP::Position(line, column)));
        QueueMessage(req);
    }
}

//===------------------------------------------------------------------
// Event handlers
//===------------------------------------------------------------------
void LanguageServerProtocol::OnFileLoaded(clCommandEvent& event)
{
    event.Skip();
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);
    OpenEditor(editor);
}

void LanguageServerProtocol::OnFileClosed(clCommandEvent& event)
{
    event.Skip();
    SendCloseRequest(event.GetFileName());
}

void LanguageServerProtocol::OnFileSaved(clCommandEvent& event)
{
    event.Skip();
    // For now, it does the same as 'OnFileLoaded'
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);
    if(ShouldHandleFile(editor)) {
        std::string fileContent;
        editor->GetEditorTextRaw(fileContent);
        SendSaveRequest(editor->GetFileName(), fileContent);
    }
}

wxString LanguageServerProtocol::GetLogPrefix() const { return wxString() << "[" << GetName() << "] "; }

void LanguageServerProtocol::OpenEditor(IEditor* editor)
{
    clDEBUG() << "OpenEditor is called for" << editor->GetFileName();
    if(!IsInitialized()) { return; }
    if(editor && ShouldHandleFile(editor)) {
        std::string fileContent;
        editor->GetEditorTextRaw(fileContent);

        if(m_filesSent.count(editor->GetFileName().GetFullPath())) {
            clDEBUG() << "OpenEditor->SendChangeRequest called for:" << editor->GetFileName().GetFullName();
            SendChangeRequest(editor->GetFileName(), fileContent);
        } else {

            clDEBUG() << "OpenEditor->SendOpenRequest called for:" << editor->GetFileName().GetFullName();
            SendOpenRequest(editor->GetFileName(), fileContent, GetLanguageId(editor->GetFileName()));
        }
    }
}

void LanguageServerProtocol::FunctionHelp(IEditor* editor)
{
    // sanity
    CHECK_PTR_RET(editor);
    CHECK_COND_RET(ShouldHandleFile(editor));
    // If the editor is modified, we need to tell the LSP to reparse the source file
    const wxFileName& filename = editor->GetFileName();
    if(m_filesSent.count(filename.GetFullPath()) && editor->IsModified()) {
        // we already sent this file over, ask for change parse
        std::string fileContent;
        editor->GetEditorTextRaw(fileContent);
        SendChangeRequest(filename, fileContent);
    } else if(m_filesSent.count(filename.GetFullPath()) == 0) {
        std::string fileContent;
        editor->GetEditorTextRaw(fileContent);
        SendOpenRequest(filename, fileContent, GetLanguageId(filename));
    }

    if(ShouldHandleFile(filename)) {
        LSP::SignatureHelpRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(new LSP::SignatureHelpRequest(
            filename, editor->GetCurrentLine(), editor->GetCtrl()->GetColumn(editor->GetCurrentPosition())));
        QueueMessage(req);
    }
}

void LanguageServerProtocol::CodeComplete(IEditor* editor)
{
    // sanity
    CHECK_PTR_RET(editor);
    CHECK_COND_RET(ShouldHandleFile(editor));
    // If the editor is modified, we need to tell the LSP to reparse the source file
    const wxFileName& filename = editor->GetFileName();

    std::string text;
    editor->GetEditorTextRaw(text);

    if(m_filesSent.count(filename.GetFullPath()) && editor->IsModified()) {
        // we already sent this file over, ask for change parse
        SendChangeRequest(filename, text);
    } else if(m_filesSent.count(filename.GetFullPath()) == 0) {
        SendOpenRequest(filename, text, GetLanguageId(filename));
    }

    // Now request the for code completion
    SendCodeCompleteRequest(editor->GetFileName(), editor->GetCurrentLine(),
                            editor->GetCtrl()->GetColumn(editor->GetCurrentPosition()));
}

void LanguageServerProtocol::ProcessQueue()
{
    if(m_Queue.IsEmpty()) { return; }
    if(m_Queue.IsWaitingReponse()) {
        clDEBUG() << "LSP is busy, will not send message";
        return;
    }
    LSP::MessageWithParams::Ptr_t req = m_Queue.Get();
    if(!IsRunning()) {
        clDEBUG() << GetLogPrefix() << "is down.";
        return;
    }

    // Write the message length as string of 10 bytes
    m_network->Send(req->ToString(m_pathConverter));
    m_Queue.SetWaitingReponse(true);
    m_Queue.Pop();
    if(!req->GetStatusMessage().IsEmpty()) { clGetManager()->SetStatusMessage(req->GetStatusMessage(), 1); }
}

void LanguageServerProtocol::CloseEditor(IEditor* editor)
{
    if(!IsInitialized()) { return; }
    if(editor && ShouldHandleFile(editor)) { SendCloseRequest(editor->GetFileName()); }
}

void LanguageServerProtocol::FindDeclaration(IEditor* editor)
{
    if(m_unimplementedMethods.count("textDocument/declaration")) {
        // this method is not implemented
        // use 'definition' instead
        FindDefinition(editor);
    } else {
        CHECK_PTR_RET(editor);
        CHECK_COND_RET(ShouldHandleFile(editor));

        // If the editor is modified, we need to tell the LSP to reparse the source file
        const wxFileName& filename = editor->GetFileName();
        if(m_filesSent.count(filename.GetFullPath()) && editor->IsModified()) {
            // we already sent this file over, ask for change parse
            std::string content;
            editor->GetEditorTextRaw(content);
            SendChangeRequest(filename, content);
        } else if(m_filesSent.count(filename.GetFullPath()) == 0) {
            std::string content;
            editor->GetEditorTextRaw(content);
            SendOpenRequest(filename, content, GetLanguageId(filename));
        }

        LSP::GotoDeclarationRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(
            new LSP::GotoDeclarationRequest(editor->GetFileName(), editor->GetCurrentLine(),
                                            editor->GetCtrl()->GetColumn(editor->GetCurrentPosition())));
        QueueMessage(req);
    }
}

void LanguageServerProtocol::OnNetConnected(clCommandEvent& event)
{
    // The process started successfully
    // Send the 'initialize' request
    LSP::InitializeRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(new LSP::InitializeRequest());
    req->As<LSP::InitializeRequest>()->SetRootUri(m_rootFolder);

    clDEBUG() << GetLogPrefix() << "Sending initialize request...";

    // Temporarly set the state to "kInitialized" so we can send out the "initialize" request
    m_state = kInitialized;
    QueueMessage(req);
    m_state = kUnInitialized;
    m_initializeRequestID = req->As<LSP::InitializeRequest>()->GetId();
}

void LanguageServerProtocol::OnNetError(clCommandEvent& event)
{
    clDEBUG() << GetLogPrefix() << "Socket error." << event.GetString();
    DoClear();
    LSPEvent restartEvent(wxEVT_LSP_RESTART_NEEDED);
    restartEvent.SetServerName(GetName());
    m_owner->AddPendingEvent(restartEvent);
}

void LanguageServerProtocol::OnNetDataReady(clCommandEvent& event)
{
    clDEBUG() << GetLogPrefix() << event.GetString();
    wxString buffer = std::move(event.GetString());
    m_outputBuffer << buffer;
    m_Queue.SetWaitingReponse(false);

    while(true) {
        // Did we get a complete message?
        LSP::ResponseMessage res(m_outputBuffer, m_pathConverter);
        if(res.IsOk()) {
            if(IsInitialized()) {
                LSP::MessageWithParams::Ptr_t msg_ptr = m_Queue.TakePendingReplyMessage(res.GetId());
                // Is this an error message?
                if(res.Has("error")) {
                    clDEBUG() << GetLogPrefix() << "received an error message";
                    LSP::ResponseError errMsg(res.GetMessageString(), m_pathConverter);
                    switch(errMsg.GetErrorCode()) {
                    case LSP::ResponseError::kErrorCodeInternalError:
                    case LSP::ResponseError::kErrorCodeInvalidRequest: {
                        // Restart this server
                        LSPEvent restartEvent(wxEVT_LSP_RESTART_NEEDED);
                        restartEvent.SetServerName(GetName());
                        m_owner->AddPendingEvent(restartEvent);
                        break;
                    }
                    case LSP::ResponseError::kErrorCodeMethodNotFound: {
                        // User requested a mesasge which is not supported by this server
                        clGetManager()->SetStatusMessage(wxString() << GetLogPrefix() << _("method: ")
                                                                    << msg_ptr->GetMethod() << _(" is not supported"));
                        m_unimplementedMethods.insert(msg_ptr->GetMethod());

                        // Report this missing event
                        LSPEvent eventMethodNotFound(wxEVT_LSP_METHOD_NOT_FOUND);
                        eventMethodNotFound.SetServerName(GetName());
                        eventMethodNotFound.SetString(msg_ptr->GetMethod());
                        m_owner->AddPendingEvent(eventMethodNotFound);

                    } break;
                    case LSP::ResponseError::kErrorCodeInvalidParams: {
                        // Recreate this AST (in other words: reparse), by default we reparse the current editor
                        LSPEvent reparseEvent(wxEVT_LSP_REPARSE_NEEDED);
                        reparseEvent.SetServerName(GetName());
                        m_owner->AddPendingEvent(reparseEvent);
                        break;
                    }
                    default:
                        break;
                    }
                } else {
                    if(msg_ptr && msg_ptr->As<LSP::Request>()) {
                        clDEBUG() << GetLogPrefix() << "received a response";
                        // Check if the reply is still valid
                        IEditor* editor = clGetManager()->GetActiveEditor();
                        if(editor) {
                            LSP::Request* preq = msg_ptr->As<LSP::Request>();
                            if(preq->As<LSP::CompletionRequest>() && (preq->GetId() < m_lastCompletionRequestId)) {
                                clDEBUG() << "Received a response for completion message ID#" << preq->GetId()
                                          << ". However, a newer completion request with ID#"
                                          << m_lastCompletionRequestId << "was already sent. Dropping response";
                                return;
                            }
                            // let the originating request to handle it
                            const wxFileName& filename = editor->GetFileName();
                            size_t line = editor->GetCurrentLine();
                            size_t column = editor->GetCtrl()->GetColumn(editor->GetCurrentPosition());
                            if(false && preq->IsPositionDependantRequest() &&
                               !preq->IsValidAt(filename, line, column)) {
                                clDEBUG() << "Response is no longer valid. Discarding its result";
                            } else {
                                preq->OnResponse(res, m_owner, m_pathConverter);
                            }
                        }

                    } else if(res.IsPushDiagnostics()) {
                        // Get the URI
                        clDEBUG() << GetLogPrefix() << "Received diagnostic message";
                        wxFileName fn(wxFileSystem::URLToFileName(res.GetDiagnosticsUri(m_pathConverter)));
                        fn.Normalize();
#ifndef __WXOSX__
                        // Don't show this message on macOS as it appears in the middle of the screen...
                        clGetManager()->SetStatusMessage(
                            wxString() << GetLogPrefix() << "parsing of file: " << fn.GetFullName() << " is completed",
                            1);
#endif
                        std::vector<LSP::Diagnostic> diags = res.GetDiagnostics(m_pathConverter);
                        if(!diags.empty() && IsDisaplayDiagnostics()) {
                            // report the diagnostics
                            LSPEvent eventSetDiags(wxEVT_LSP_SET_DIAGNOSTICS);
                            eventSetDiags.GetLocation().SetUri(fn.GetFullPath());
                            eventSetDiags.SetDiagnostics(diags);
                            m_owner->AddPendingEvent(eventSetDiags);
                        } else if(diags.empty()) {
                            // clear all diagnostics
                            LSPEvent eventClearDiags(wxEVT_LSP_CLEAR_DIAGNOSTICS);
                            eventClearDiags.GetLocation().SetUri(fn.GetFullPath());
                            m_owner->AddPendingEvent(eventClearDiags);
                        }
                    } else {
                        clDEBUG() << GetLogPrefix() << "received an unsupported message";
                    }
                }
            } else {
                // we only accept initialization responses here
                if(res.GetId() == m_initializeRequestID) {
                    clDEBUG() << GetLogPrefix() << "initialization completed";
                    m_initializeRequestID = wxNOT_FOUND;
                    m_state = kInitialized;

                    // Notify about this
                    LSPEvent initEvent(wxEVT_LSP_INITIALIZED);
                    initEvent.SetServerName(GetName());
                    m_owner->AddPendingEvent(initEvent);
                } else {
                    clDEBUG() << GetLogPrefix() << "Server not initialized. This message is ignored";
                }
            }
            // A message was consumed from the buffer, see if we got more messages
            if(!m_outputBuffer.empty()) { continue; }
        }
        break;
    }
    ProcessQueue();
}

void LanguageServerProtocol::Stop()
{
    clDEBUG() << GetLogPrefix() << "Going down";
    m_network->Close();
}

void LanguageServerProtocol::OnEditorChanged(wxCommandEvent& event)
{
    event.Skip();
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(editor) { OpenEditor(editor); }
}

void LanguageServerProtocol::FindImplementation(IEditor* editor)
{
    if(m_unimplementedMethods.count("textDocument/implementation")) {
        // this method is not implemented
        // use 'definition' instead
        FindDefinition(editor);
    } else {
        CHECK_PTR_RET(editor);
        CHECK_COND_RET(ShouldHandleFile(editor));

        // If the editor is modified, we need to tell the LSP to reparse the source file
        const wxFileName& filename = editor->GetFileName();
        if(m_filesSent.count(filename.GetFullPath()) && editor->IsModified()) {
            // we already sent this file over, ask for change parse
            std::string content;
            editor->GetEditorTextRaw(content);
            SendChangeRequest(filename, content);
        } else if(m_filesSent.count(filename.GetFullPath()) == 0) {
            std::string content;
            editor->GetEditorTextRaw(content);
            SendOpenRequest(filename, content, GetLanguageId(filename));
        }

        LSP::GotoImplementationRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(
            new LSP::GotoImplementationRequest(editor->GetFileName(), editor->GetCurrentLine(),
                                               editor->GetCtrl()->GetColumn(editor->GetCurrentPosition())));
        QueueMessage(req);
    }
}

wxString LanguageServerProtocol::GetLanguageId(const wxFileName& fn) { return GetLanguageId(fn.GetFullPath()); }

//===------------------------------------------------------------------
// LSPRequestMessageQueue
//===------------------------------------------------------------------

void LSPRequestMessageQueue::Push(LSP::MessageWithParams::Ptr_t message)
{
    m_Queue.push(message);

    // Messages of type 'Request' require responses from the server
    LSP::Request* req = message->As<LSP::Request>();
    if(req) { m_pendingReplyMessages.insert({ req->GetId(), message }); }
}

void LSPRequestMessageQueue::Pop()
{
    if(!m_Queue.empty()) { m_Queue.pop(); }
    SetWaitingReponse(false);
}

LSP::MessageWithParams::Ptr_t LSPRequestMessageQueue::Get()
{
    if(m_Queue.empty()) { return LSP::MessageWithParams::Ptr_t(nullptr); }
    return m_Queue.front();
}

void LSPRequestMessageQueue::Clear()
{
    while(!m_Queue.empty()) {
        m_Queue.pop();
    }
    SetWaitingReponse(false);
    m_pendingReplyMessages.clear();
}

LSP::MessageWithParams::Ptr_t LSPRequestMessageQueue::TakePendingReplyMessage(int msgid)
{
    if(m_pendingReplyMessages.empty()) { return LSP::MessageWithParams::Ptr_t(nullptr); }
    if(m_pendingReplyMessages.count(msgid) == 0) { return LSP::MessageWithParams::Ptr_t(nullptr); }
    LSP::MessageWithParams::Ptr_t msgptr = m_pendingReplyMessages[msgid];
    m_pendingReplyMessages.erase(msgid);
    return msgptr;
}
