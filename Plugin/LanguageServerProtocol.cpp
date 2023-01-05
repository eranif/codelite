#include "LanguageServerProtocol.h"

#include "LSP/CompletionRequest.h"
#include "LSP/DidChangeTextDocumentRequest.h"
#include "LSP/DidCloseTextDocumentRequest.h"
#include "LSP/DidOpenTextDocumentRequest.h"
#include "LSP/DidSaveTextDocumentRequest.h"
#include "LSP/DocumentSymbolsRequest.hpp"
#include "LSP/FindReferencesRequest.hpp"
#include "LSP/GotoDeclarationRequest.h"
#include "LSP/GotoDefinitionRequest.h"
#include "LSP/GotoImplementationRequest.h"
#include "LSP/HoverRequest.hpp"
#include "LSP/InitializeRequest.h"
#include "LSP/InitializedNotification.hpp"
#include "LSP/LSPEvent.h"
#include "LSP/Request.h"
#include "LSP/ResponseError.h"
#include "LSP/ResponseMessage.h"
#include "LSP/SemanticTokensRquest.hpp"
#include "LSP/SignatureHelpRequest.h"
#include "LSP/WorkspaceSymbolRequest.hpp"
#include "LSPNetworkSTDIO.h"
#include "LSPNetworkSocketClient.h"
#include "clWorkspaceManager.h"
#include "cl_exception.h"
#include "codelite_events.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "macros.h"
#include "processreaderthread.h"
#include "wxmd5.h"

#include <iomanip>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <wx/filesys.h>
#include <wx/stc/stc.h>

thread_local wxString emptyString;
FileExtManager::FileType LanguageServerProtocol::workspace_file_type = FileExtManager::TypeOther;

LanguageServerProtocol::LanguageServerProtocol(const wxString& name, eNetworkType netType, wxEvtHandler* owner)
    : ServiceProvider(wxString() << "LSP: " << name, eServiceType::kCodeCompletion)
    , m_name(name)
    , m_owner(owner)
{
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &LanguageServerProtocol::OnFileSaved, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_CLOSED, &LanguageServerProtocol::OnFileClosed, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_LOADED, &LanguageServerProtocol::OnFileLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &LanguageServerProtocol::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &LanguageServerProtocol::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &LanguageServerProtocol::OnWorkspaceClosed, this);

    Bind(wxEVT_CC_FIND_SYMBOL, &LanguageServerProtocol::OnFindSymbol, this);
    Bind(wxEVT_CC_FIND_SYMBOL_DECLARATION, &LanguageServerProtocol::OnFindSymbolDecl, this);
    Bind(wxEVT_CC_FIND_SYMBOL_DEFINITION, &LanguageServerProtocol::OnFindSymbolImpl, this);
    Bind(wxEVT_CC_CODE_COMPLETE, &LanguageServerProtocol::OnCodeComplete, this);
    Bind(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &LanguageServerProtocol::OnFunctionCallTip, this);
    Bind(wxEVT_CC_TYPEINFO_TIP, &LanguageServerProtocol::OnTypeInfoToolTip, this);
    Bind(wxEVT_CC_SEMANTICS_HIGHLIGHT, &LanguageServerProtocol::OnSemanticHighlights, this);
    Bind(wxEVT_CC_WORKSPACE_SYMBOLS, &LanguageServerProtocol::OnWorkspaceSymbols, this);
    Bind(wxEVT_CC_FIND_HEADER_FILE, &LanguageServerProtocol::OnFindHeaderFile, this);
    Bind(wxEVT_CC_JUMP_HYPER_LINK, &LanguageServerProtocol::OnQuickJump, this);
    EventNotifier::Get()->Bind(wxEVT_CC_SHOW_QUICK_OUTLINE, &LanguageServerProtocol::OnQuickOutline, this);

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
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &LanguageServerProtocol::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &LanguageServerProtocol::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &LanguageServerProtocol::OnFileSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_CLOSED, &LanguageServerProtocol::OnFileClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_LOADED, &LanguageServerProtocol::OnFileLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &LanguageServerProtocol::OnEditorChanged, this);
    Unbind(wxEVT_CC_FIND_SYMBOL, &LanguageServerProtocol::OnFindSymbol, this);
    Unbind(wxEVT_CC_FIND_SYMBOL_DECLARATION, &LanguageServerProtocol::OnFindSymbolDecl, this);
    Unbind(wxEVT_CC_FIND_SYMBOL_DEFINITION, &LanguageServerProtocol::OnFindSymbolImpl, this);
    Unbind(wxEVT_CC_CODE_COMPLETE, &LanguageServerProtocol::OnCodeComplete, this);
    Unbind(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &LanguageServerProtocol::OnFunctionCallTip, this);
    Unbind(wxEVT_CC_TYPEINFO_TIP, &LanguageServerProtocol::OnTypeInfoToolTip, this);
    Unbind(wxEVT_CC_SEMANTICS_HIGHLIGHT, &LanguageServerProtocol::OnSemanticHighlights, this);
    Unbind(wxEVT_CC_WORKSPACE_SYMBOLS, &LanguageServerProtocol::OnWorkspaceSymbols, this);
    Unbind(wxEVT_CC_FIND_HEADER_FILE, &LanguageServerProtocol::OnFindHeaderFile, this);
    Unbind(wxEVT_CC_JUMP_HYPER_LINK, &LanguageServerProtocol::OnQuickJump, this);

    EventNotifier::Get()->Unbind(wxEVT_CC_SHOW_QUICK_OUTLINE, &LanguageServerProtocol::OnQuickOutline, this);
    DoClear();
}

wxString LanguageServerProtocol::GetLanguageId(FileExtManager::FileType file_type)
{
    switch(file_type) {
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
    case FileExtManager::TypeShellScript:
        return "shellscript";
    case FileExtManager::TypeRuby:
        return "ruby";
    case FileExtManager::TypeDart:
        return "dart";
    default:
        return "";
    }
}

wxString LanguageServerProtocol::GetLanguageId(IEditor* editor)
{
    if(!editor) {
        return wxEmptyString;
    }
    FileExtManager::FileType type =
        FileExtManager::GetType(editor->GetFileName().GetFullPath(), FileExtManager::TypeText);
    return GetLanguageId(type);
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
    if(!IsInitialized()) {
        return;
    }
    if(request->As<LSP::CompletionRequest>()) {
        m_lastCompletionRequestId = request->As<LSP::CompletionRequest>()->GetId();
    }
    m_Queue.Push(request);
    ProcessQueue();
}

bool LanguageServerProtocol::DoStart()
{
    DoClear();

    clDEBUG() << GetLogPrefix() << "Starting..." << endl;
    clDEBUG() << GetLogPrefix() << "Command:" << m_startupInfo.GetLspServerCommand() << endl;
    clDEBUG() << GetLogPrefix() << "Root folder:" << m_rootFolder << endl;
    for(const wxString& lang : m_languages) {
        clDEBUG() << GetLogPrefix() << "Language:" << lang << endl;
    }
    try {
        // apply global environment variables
        EnvSetter env;
        // apply this lsp specific environment variables
        clEnvironment localEnv(&m_env);
        m_network->Open(m_startupInfo);
        return true;
    } catch(clException& e) {
        clWARNING() << e.What();
        return false;
    }
}

bool LanguageServerProtocol::Start(const LSPStartupInfo& startupInfo, const clEnvList_t& env,
                                   const wxString& initOptions, const wxString& rootFolder,
                                   const wxArrayString& languages)
{
    if(IsRunning()) {
        return true;
    }
    DoClear();

    m_languages.clear();
    std::for_each(languages.begin(), languages.end(), [&](const wxString& lang) { m_languages.insert(lang); });
    m_startupInfo = startupInfo;

    m_rootFolder = rootFolder;
    m_initOptions = initOptions;
    m_env = env;
    return DoStart();
}

bool LanguageServerProtocol::Start()
{
    if(IsRunning()) {
        return true;
    }
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

bool LanguageServerProtocol::CanHandle(IEditor* editor) const
{
    // use the local file path
    wxString lang = GetLanguageId(editor);
    return m_languages.count(lang) != 0;
}

bool LanguageServerProtocol::CanHandle(FileExtManager::FileType file_type) const
{
    wxString lang = GetLanguageId(file_type);
    return m_languages.count(lang) != 0;
}

bool LanguageServerProtocol::ShouldHandleFile(IEditor* editor) const
{
    // always use the local file path to determine the file path
    // this to ensure that we can open it in case we will need
    // to determine the file type based on its content
    wxString lang = GetLanguageId(editor);
    return (m_languages.count(lang) != 0);
}

void LanguageServerProtocol::OnFunctionCallTip(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = GetEditor(event);
    CHECK_PTR_RET(editor);
    if(CanHandle(editor)) {
        event.Skip(false);
        FunctionHelp(editor);
    }
}

void LanguageServerProtocol::OnTypeInfoToolTip(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = GetEditor(event);
    CHECK_PTR_RET(editor);
    if(CanHandle(editor)) {
        event.Skip(false);
        HoverTip(editor);
    }
}

void LanguageServerProtocol::OnCodeComplete(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = GetEditor(event);
    CHECK_PTR_RET(editor);

    if(event.GetTriggerKind() != LSP::CompletionItem::kTriggerUser && event.IsInsideCommentOrString()) {
        return;
    }

    if(CanHandle(editor)) {
        event.Skip(false);
        CodeComplete(editor);
    }
}

void LanguageServerProtocol::OnFindSymbolDecl(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = GetEditor(event);
    CHECK_PTR_RET(editor);

    if(CanHandle(editor)) {
        // this event is ours to handle
        event.Skip(false);
        FindDeclaration(editor, false);
    }
}

void LanguageServerProtocol::OnFindSymbolImpl(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = GetEditor(event);
    CHECK_PTR_RET(editor);

    if(CanHandle(editor)) {
        // this event is ours to handle
        event.Skip(false);
        FindImplementation(editor);
    }
}

void LanguageServerProtocol::OnFindSymbol(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = GetEditor(event);
    CHECK_PTR_RET(editor);

    if(CanHandle(editor)) {
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
    wxString filename = GetEditorFilePath(editor);
    if(m_filesSent.count(filename) && editor->IsEditorModified()) {
        // we already sent this file over, ask for change parse
        wxString fileContent = editor->GetEditorText();
        SendChangeRequest(editor, fileContent);

    } else if(m_filesSent.count(filename) == 0) {
        wxString fileContent = editor->GetEditorText();
        SendOpenRequest(editor, fileContent, GetLanguageId(editor));
    }

    LSP::GotoDefinitionRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(new LSP::GotoDefinitionRequest(
        GetEditorFilePath(editor), editor->GetCurrentLine(), editor->GetColumnInChars(editor->GetCurrentPosition())));
    QueueMessage(req);
}

void LanguageServerProtocol::SendOpenRequest(IEditor* editor, const wxString& fileContent, const wxString& languageId)
{
    CHECK_PTR_RET(editor);
    wxString filename = GetEditorFilePath(editor);
    if(!IsFileChangedSinceLastParse(filename, fileContent)) {
        LOG_IF_TRACE { clDEBUG1() << GetLogPrefix() << "No changes detected in file:" << filename << endl; }

        // send a semantic request
        return;
    }
    LSP::DidOpenTextDocumentRequest::Ptr_t req =
        LSP::MessageWithParams::MakeRequest(new LSP::DidOpenTextDocumentRequest(filename, fileContent, languageId));
#ifndef __WXOSX__
    req->SetStatusMessage(wxString() << GetLogPrefix() << " parsing file: " << filename);
#endif
    UpdateFileSent(filename, fileContent);
    QueueMessage(req);

    // send a semantic request
    SendSemanticTokensRequest(editor);
}

void LanguageServerProtocol::SendCloseRequest(const wxString& filename)
{
    if(m_filesSent.count(filename) == 0) {
        LOG_IF_TRACE { clDEBUG1() << GetLogPrefix() << "LanguageServerProtocol::FileClosed(): file" << filename << "is not opened"; }
        return;
    }

    LSP::DidCloseTextDocumentRequest::Ptr_t req =
        LSP::MessageWithParams::MakeRequest(new LSP::DidCloseTextDocumentRequest(filename));
    QueueMessage(req);
    m_filesSent.erase(filename);
}

void LanguageServerProtocol::SendChangeRequest(IEditor* editor, const wxString& fileContent, bool force_reparse)
{
    CHECK_PTR_RET(editor);
    wxString filename = GetEditorFilePath(editor);
    if(!force_reparse && !IsFileChangedSinceLastParse(filename, fileContent)) {
        LOG_IF_TRACE { clDEBUG1() << GetLogPrefix() << "No changes detected in file:" << filename << endl; }

        // always send a semantic request
        // SendSemanticTokensRequest(editor);
        return;
    }

    clDEBUG() << GetLogPrefix() << "Sending ChangeRequest" << endl;

    LSP::DidChangeTextDocumentRequest::Ptr_t req =
        LSP::MessageWithParams::MakeRequest(new LSP::DidChangeTextDocumentRequest(filename, fileContent));
#ifndef __WXOSX__
    req->SetStatusMessage(wxString() << GetLogPrefix() << " re-parsing file: " << filename);
#endif
    UpdateFileSent(filename, fileContent);
    QueueMessage(req);

    // always send a semantic request
    // SendSemanticTokensRequest(editor);
}

void LanguageServerProtocol::SendSaveRequest(IEditor* editor, const wxString& fileContent)
{
    CHECK_PTR_RET(editor);
    // For now: report a change event
    wxString filename = GetEditorFilePath(editor);
    if(ShouldHandleFile(editor)) {
        LSP::CompletionRequest::Ptr_t req =
            LSP::MessageWithParams::MakeRequest(new LSP::DidSaveTextDocumentRequest(filename, fileContent));
        QueueMessage(req);
    }
}

void LanguageServerProtocol::SendCodeCompleteRequest(IEditor* editor, size_t line, size_t column)
{
    CHECK_PTR_RET(editor);
    wxString filename = GetEditorFilePath(editor);
    if(ShouldHandleFile(editor)) {
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
    IEditor* editor = clGetManager()->FindEditor(event.GetFileName());
    CHECK_PTR_RET(editor);
    if(ShouldHandleFile(editor)) {
        SendSaveRequest(editor, editor->GetEditorText());
    }
}

wxString LanguageServerProtocol::GetLogPrefix() const { return wxString() << "[" << GetName() << "]"; }

void LanguageServerProtocol::OpenEditor(IEditor* editor)
{
    LOG_IF_TRACE { clDEBUG1() << "OpenEditor is called for" << GetEditorFilePath(editor) << endl; }
    if(!IsInitialized()) {
        clDEBUG() << "OpenEditor: server is still not initialized. server:" << GetName()
                  << ", file:" << GetEditorFilePath(editor) << endl;
        return;
    }

    if(editor && ShouldHandleFile(editor)) {
        wxString fileContent = editor->GetEditorText();

        if(m_filesSent.count(GetEditorFilePath(editor))) {
            LOG_IF_TRACE { clDEBUG1() << "OpenEditor->SendChangeRequest called for:" << GetEditorFilePath(editor); }
            SendChangeRequest(editor, fileContent);
        } else {
            // If we are about to load a header file, also pass clangd the implementation(s) file
            LOG_IF_TRACE { clDEBUG1() << "OpenEditor->SendOpenRequest called for:" << GetEditorFilePath(editor); }
            SendOpenRequest(editor, fileContent, GetLanguageId(editor));
        }
    }
}

void LanguageServerProtocol::FunctionHelp(IEditor* editor)
{
    // sanity
    CHECK_PTR_RET(editor);
    CHECK_COND_RET(ShouldHandleFile(editor));

    // If the editor is modified, we need to tell the LSP to reparse the source file
    const wxString& filename = GetEditorFilePath(editor);
    if(m_filesSent.count(filename) && editor->IsEditorModified()) {
        // we already sent this file over, ask for change parse
        wxString fileContent = editor->GetEditorText();
        SendChangeRequest(editor, fileContent);
    } else if(m_filesSent.count(filename) == 0) {
        wxString fileContent = editor->GetEditorText();
        SendOpenRequest(editor, fileContent, GetLanguageId(editor));
    }

    if(ShouldHandleFile(editor)) {
        LSP::SignatureHelpRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(new LSP::SignatureHelpRequest(
            filename, editor->GetCurrentLine(), editor->GetColumnInChars(editor->GetCurrentPosition())));
        QueueMessage(req);
    }
}

void LanguageServerProtocol::HoverTip(IEditor* editor)
{
    // sanity
    CHECK_PTR_RET(editor);
    CHECK_COND_RET(ShouldHandleFile(editor));

    // If the editor is modified, we need to tell the LSP to reparse the source file
    const wxString& filename = GetEditorFilePath(editor);
    if(m_filesSent.count(filename) && editor->IsEditorModified()) {
        // we already sent this file over, ask for change parse
        wxString fileContent = editor->GetEditorText();
        SendChangeRequest(editor, fileContent);
    } else if(m_filesSent.count(filename) == 0) {
        wxString fileContent = editor->GetEditorText();
        SendOpenRequest(editor, fileContent, GetLanguageId(editor));
    }

    if(ShouldHandleFile(editor)) {
        int pos = editor->GetPosAtMousePointer();
        // trigger a hover request only when we are hovering something
        if(pos != wxNOT_FOUND && isgraph(editor->GetCharAtPos(pos))) {
            LSP::HoverRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(
                new LSP::HoverRequest(filename, editor->LineFromPos(pos), editor->GetColumnInChars(pos)));
            QueueMessage(req);
        }
    }
}

void LanguageServerProtocol::CodeComplete(IEditor* editor)
{
    // sanity
    CHECK_PTR_RET(editor);
    CHECK_COND_RET(ShouldHandleFile(editor));
    // If the editor is modified, we need to tell the LSP to reparse the source file
    const wxString& filename = GetEditorFilePath(editor);

    if(m_filesSent.count(filename) && editor->IsEditorModified()) {
        wxString fileContent = editor->GetEditorText();
        SendChangeRequest(editor, fileContent, true);

    } else if(m_filesSent.count(filename) == 0) {
        wxString fileContent = editor->GetEditorText();
        SendOpenRequest(editor, fileContent, GetLanguageId(editor));
    }

    // Now request the for code completion
    SendCodeCompleteRequest(editor, editor->GetCurrentLine(), editor->GetColumnInChars(editor->GetCurrentPosition()));
}

void LanguageServerProtocol::ProcessQueue()
{
    if(m_Queue.IsEmpty()) {
        return;
    }
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
    m_network->Send(req->ToString());
    m_Queue.SetWaitingReponse(true);
    m_Queue.Pop();
    if(!req->GetStatusMessage().IsEmpty()) {
        clGetManager()->SetStatusMessage(req->GetStatusMessage(), 1);
    }
}

void LanguageServerProtocol::CloseEditor(IEditor* editor)
{
    if(!IsInitialized()) {
        return;
    }
    if(editor && ShouldHandleFile(editor)) {
        SendCloseRequest(GetEditorFilePath(editor));
    }
}

void LanguageServerProtocol::FindDeclaration(IEditor* editor, bool for_add_missing_header)
{
    if(!IsDeclarationSupported()) {
        clDEBUG() << GetLogPrefix() << "message `textDocument/declaration` is not supported" << endl;
        return;
    }

    clDEBUG() << GetLogPrefix() << "FindDeclaration() is called" << endl;
    CHECK_PTR_RET(editor);
    CHECK_COND_RET(ShouldHandleFile(editor));

    // If the editor is modified, we need to tell the LSP to reparse the source file
    const wxString& filename = GetEditorFilePath(editor);
    wxString fileContent = editor->GetEditorText();

    if(m_filesSent.count(filename) == 0) {
        SendOpenRequest(editor, fileContent, GetLanguageId(editor));
    } else {
        SendChangeRequest(editor, fileContent);
    }
    clDEBUG() << GetLogPrefix() << "Sending GotoDeclarationRequest" << endl;
    LSP::GotoDeclarationRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(new LSP::GotoDeclarationRequest(
        GetEditorFilePath(editor), editor->GetCurrentLine(), editor->GetColumnInChars(editor->GetCurrentPosition()),
        for_add_missing_header));
    QueueMessage(req);
}

void LanguageServerProtocol::OnNetConnected(clCommandEvent& event)
{
    // invoke the custom callback
    if(m_onServerStartedCallback) {
        m_onServerStartedCallback();
    }

    // The process started successfully
    // Send the 'initialize' request
    LSP::InitializeRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(new LSP::InitializeRequest());
    // some LSPs will crash if we path an empty root folder
    wxString root_uri = m_rootFolder;
    if(root_uri.empty()) {
        root_uri = clWorkspaceManager::Get().IsWorkspaceOpened() ? clWorkspaceManager::Get().GetWorkspace()->GetDir()
                                                                 : clStandardPaths::Get().GetTempDir();
    }
    req->As<LSP::InitializeRequest>()->SetRootUri(root_uri);
    req->As<LSP::InitializeRequest>()->SetInitOptions(m_initOptions);
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
    m_outputBuffer << event.GetString();
    clDEBUG() << "Received data from LSP:" << m_outputBuffer.size() << "bytes" << endl;

    m_Queue.SetWaitingReponse(false);
    while(true) {
        // attempt to consume a complete JSON payload from the aggregated network buffer
        auto json = LSP::Message::GetJSONPayload(m_outputBuffer);
        if(!json) {
            LOG_IF_TRACE { clDEBUG1() << "Unable to read JSON payload" << endl; }
            break;
        }

        auto json_item = json->toElement();
        // check the message type
        wxString message_method = json_item["method"].toString();
        // clDEBUG1() << "-- LSP:" << json_item.format(false) << endl;
        LOG_IF_TRACE { clDEBUG1() << "-- LSP: Message Method is:" << message_method << endl; }

        if(message_method == "window/logMessage" || message_method == "window/showMessage") {
            // log this message
            LSPEvent log_event(wxEVT_LSP_LOGMESSAGE);
            log_event.SetServerName(GetName());
            log_event.SetMessage(json_item["params"]["message"].toString());
            log_event.SetLogMessageSeverity(json_item["params"]["type"].toInt());
            m_owner->AddPendingEvent(log_event);

        } else if(message_method == "telemetry/event") {
            // show dialog to the user
            // log this message
            LSPEvent log_event(wxEVT_LSP_LOGMESSAGE);
            log_event.SetServerName(GetName());
            log_event.SetMessage(json_item["params"].toString());
            m_owner->AddPendingEvent(log_event);

        } else {
            // other response
            LSP::ResponseMessage res(std::move(json));
            if(IsInitialized()) {
                LSP::MessageWithParams::Ptr_t msg_ptr = m_Queue.TakePendingReplyMessage(res.GetId());
                // Is this an error message?
                if(res.IsErrorResponse()) {
                    // an error response arrived, handle it
                    HandleResponseError(res, msg_ptr);
                } else {
                    // process a success response from the LSP
                    HandleResponse(res, msg_ptr);
                }
            } else {
                // Server is not initialized yet: only accept initialization responses here
                if(res.GetId() == m_initializeRequestID) {
                    m_state = kInitialized;

                    // Keep the semantic tokens array
                    if(CheckCapability(res, "semanticTokensProvider", "textDocument/semanticTokens/full")) {
                        m_semanticTokensTypes =
                            res["result"]["capabilities"]["semanticTokensProvider"]["legend"]["tokenTypes"]
                                .toArrayString();
                        clDEBUG() << GetLogPrefix() << "Server semantic tokens are:" << m_semanticTokensTypes << endl;
                    }

                    CheckCapability(res, "documentSymbolProvider", "textDocument/documentSymbol");
                    CheckCapability(res, "declarationProvider", "textDocument/declaration");
                    CheckCapability(res, "workspaceSymbolProvider", "workspace/symbol");
                    CheckCapability(res, "renameProvider", "textDocument/rename");
                    CheckCapability(res, "referencesProvider", "textDocument/references");

                    clDEBUG() << GetLogPrefix() << "Sending InitializedNotification" << endl;

                    LSP::InitializedNotification::Ptr_t initNotification =
                        LSP::MessageWithParams::MakeRequest(new LSP::InitializedNotification());
                    QueueMessage(initNotification);

                    // Send LSP::InitializedNotification to the server
                    clDEBUG() << GetLogPrefix() << "initialization completed" << endl;
                    m_initializeRequestID = wxNOT_FOUND;
                    // Notify about this
                    LSPEvent initEvent(wxEVT_LSP_INITIALIZED);
                    initEvent.SetServerName(GetName());
                    m_owner->AddPendingEvent(initEvent);
                } else {
                    clDEBUG() << GetLogPrefix() << "Server not initialized. This message is ignored";
                }
            }
        }
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
    if(editor) {
        OpenEditor(editor);
        // a semantic tokens request will be sent as part of the `OpenEditor` request
    }
}

void LanguageServerProtocol::FindImplementation(IEditor* editor) { FindDefinition(editor); }

void LanguageServerProtocol::OnQuickOutline(clCodeCompletionEvent& event)
{
    event.Skip();

    LOG_IF_TRACE { clDEBUG1() << "LanguageServerProtocol::OnQuickOutline called" << endl; }
    IEditor* editor = GetEditor(event);
    CHECK_PTR_RET(editor);

    if(CanHandle(editor) && IsDocumentSymbolsSupported()) {
        // this event is ours to handle
        event.Skip(false);
        DocumentSymbols(editor, LSP::DocumentSymbolsRequest::CONTEXT_QUICK_OUTLINE |
                                    LSP::DocumentSymbolsRequest::CONTEXT_OUTLINE_VIEW);
        // dont wait for the response, but fire an event to load the dialog
        LSPEvent show_quick_outline_dlg_event(wxEVT_LSP_SHOW_QUICK_OUTLINE_DLG);
        m_owner->AddPendingEvent(show_quick_outline_dlg_event);
    }
}

void LanguageServerProtocol::DocumentSymbols(IEditor* editor, size_t context_flags)
{
    CHECK_PTR_RET(editor);
    CHECK_COND_RET(ShouldHandleFile(editor));

    const wxString& filename = GetEditorFilePath(editor);
    LSP::MessageWithParams::Ptr_t req =
        LSP::MessageWithParams::MakeRequest(new LSP::DocumentSymbolsRequest(filename, context_flags));
    QueueMessage(req);
}

void LanguageServerProtocol::UpdateFileSent(const wxString& filename, const wxString& fileContent)
{
    wxString checksum = wxMD5::GetDigest(fileContent);
    m_filesSent.erase(filename);
    LOG_IF_TRACE { clDEBUG1() << "Caching file:" << filename << "with checksum:" << checksum << endl; }
    m_filesSent.insert({ filename, checksum });
}

bool LanguageServerProtocol::IsFileChangedSinceLastParse(const wxString& filename, const wxString& fileContent) const
{
    if(m_filesSent.count(filename) == 0) {
        return true;
    }
    wxString checksum = wxMD5::GetDigest(fileContent);
    return m_filesSent.find(filename)->second != checksum;
}

wxString LanguageServerProtocol::GetEditorFilePath(IEditor* editor) const
{
    if(editor->IsRemoteFile()) {
        return editor->GetRemotePath();
    } else {
        return editor->GetFileName().GetFullPath();
    }
}

const wxString& LanguageServerProtocol::GetSemanticToken(size_t index) const
{
    if(index >= m_semanticTokensTypes.size()) {
        return emptyString;
    }
    return m_semanticTokensTypes[index];
}

void LanguageServerProtocol::SendWorkspaceSymbolsRequest(const wxString& query_string)
{
    clDEBUG() << GetLogPrefix() << "Sending workspace symbol request" << endl;
    LSP::WorkspaceSymbolRequest::Ptr_t req =
        LSP::MessageWithParams::MakeRequest(new LSP::WorkspaceSymbolRequest(query_string));
    QueueMessage(req);
}

void LanguageServerProtocol::SendSemanticTokensRequest(IEditor* editor)
{
    CHECK_PTR_RET(editor);
    // check if this is implemented by the server
    if(IsSemanticTokensSupported()) {
        clDEBUG() << GetLogPrefix() << "Sending semantic tokens request..." << endl;
        LSP::DidChangeTextDocumentRequest::Ptr_t req =
            LSP::MessageWithParams::MakeRequest(new LSP::SemanticTokensRquest(GetEditorFilePath(editor)));
        QueueMessage(req);
        clDEBUG() << GetLogPrefix() << "Success" << endl;
        // send an extra call for document symbols
        // we will cache this later
        clDEBUG() << GetLogPrefix() << "Sending document symbols request.." << endl;
        DocumentSymbols(editor, LSP::DocumentSymbolsRequest::CONTEXT_OUTLINE_VIEW);
        clDEBUG() << GetLogPrefix() << "Success" << endl;

    } else if(IsDocumentSymbolsSupported()) {
        clDEBUG() << GetLogPrefix() << "Sending semantic tokens request (DocumentSymbols)" << endl;
        // Use DocumentSymbol instead
        DocumentSymbols(editor, LSP::DocumentSymbolsRequest::CONTEXT_SEMANTIC_HIGHLIGHT |
                                    LSP::DocumentSymbolsRequest::CONTEXT_OUTLINE_VIEW);
    }
}

void LanguageServerProtocol::HandleResponseError(LSP::ResponseMessage& response, LSP::MessageWithParams::Ptr_t msg_ptr)
{
    clDEBUG() << GetLogPrefix() << "received an error message:" << response.ToString() << endl;
    LSP::ResponseError errMsg(response.ToString());
    switch(errMsg.GetErrorCode()) {
    case LSP::ResponseError::kErrorCodeInternalError:
    case LSP::ResponseError::kErrorCodeInvalidRequest: {
        // Restart this server
        LSPEvent restartEvent(wxEVT_LSP_RESTART_NEEDED);
        restartEvent.SetServerName(GetName());
        m_owner->AddPendingEvent(restartEvent);
    } break;
    case LSP::ResponseError::kErrorCodeMethodNotFound: {
        // Report this missing event
        LSPEvent eventMethodNotFound(wxEVT_LSP_METHOD_NOT_FOUND);
        eventMethodNotFound.SetServerName(GetName());
        eventMethodNotFound.SetString(msg_ptr->GetMethod());
        m_owner->AddPendingEvent(eventMethodNotFound);

        // Log this message
        LSPEvent log_event(wxEVT_LSP_LOGMESSAGE);
        log_event.SetServerName(GetName());
        log_event.SetMessage(_("Method: `") + msg_ptr->GetMethod() + _("` is not supported"));
        log_event.SetLogMessageSeverity(2); // warning
        m_owner->AddPendingEvent(log_event);

    } break;
    case LSP::ResponseError::kErrorCodeInvalidParams: {
        // Recreate this AST (in other words: reparse), by default we reparse the current editor
        LSPEvent reparseEvent(wxEVT_LSP_REPARSE_NEEDED);
        reparseEvent.SetServerName(GetName());
        m_owner->AddPendingEvent(reparseEvent);
    } break;
    default:
        break;
    }
}

void LanguageServerProtocol::HandleResponse(LSP::ResponseMessage& response, LSP::MessageWithParams::Ptr_t msg_ptr)
{
    if(msg_ptr && msg_ptr->As<LSP::Request>()) {
        LOG_IF_TRACE { clDEBUG1() << GetLogPrefix() << "received a response"; }
        LSP::Request* preq = msg_ptr->As<LSP::Request>();
        if(preq->As<LSP::CompletionRequest>() && (preq->GetId() < m_lastCompletionRequestId)) {
            clDEBUG1() << "Received a response for completion message ID#" << preq->GetId()
                       << ". However, a newer completion request with ID#" << m_lastCompletionRequestId
                       << "was already sent. Dropping response";
            return;
        }
        preq->SetServerName(GetName());
        preq->OnResponse(response, m_owner);

    } else if(response.IsPushDiagnostics()) {
        // Get the URI
        LOG_IF_TRACE { clDEBUG1() << GetLogPrefix() << "Received diagnostic message"; }
        wxString fn = FileUtils::FilePathFromURI(response.GetDiagnosticsUri());

        // Don't show this message on macOS as it appears in the middle of the screen...
        clGetManager()->SetStatusMessage(wxString() << GetLogPrefix() << " parsing of file: " << fn << " is completed",
                                         1);

        std::vector<LSP::Diagnostic> diags = response.GetDiagnostics();
        if(!diags.empty() && IsDisaplayDiagnostics()) {
            // report the diagnostics
            LSPEvent eventSetDiags(wxEVT_LSP_SET_DIAGNOSTICS);
            eventSetDiags.GetLocation().SetPath(fn);
            eventSetDiags.SetDiagnostics(diags);
            m_owner->AddPendingEvent(eventSetDiags);
        } else if(diags.empty()) {
            // clear all diagnostics
            LSPEvent eventClearDiags(wxEVT_LSP_CLEAR_DIAGNOSTICS);
            eventClearDiags.GetLocation().SetPath(fn);
            m_owner->AddPendingEvent(eventClearDiags);
        }
    }
}

void LanguageServerProtocol::OnFindHeaderFile(clCodeCompletionEvent& event)
{
    clDEBUG() << GetLogPrefix() << "OnFindHeaderFile() is called" << endl;
    event.Skip();
    IEditor* editor = ::clGetManager()->FindEditor(event.GetFileName());
    CHECK_PTR_RET(editor);

    if(!ShouldHandleFile(editor)) {
        return;
    }
    event.Skip(false);
    FindDeclaration(editor, true);
}

void LanguageServerProtocol::OnWorkspaceSymbols(clCodeCompletionEvent& event)
{
    event.Skip();
    if(!CanHandle(workspace_file_type)) {
        return;
    }
    event.Skip(false);
    SendWorkspaceSymbolsRequest(event.GetString());
}

void LanguageServerProtocol::FindReferences(IEditor* editor)
{
    CHECK_PTR_RET(editor);
    CHECK_EXPECTED_RETURN(IsReferencesSupported(), true);

    // send "references" message
    clDEBUG() << GetLogPrefix() << "Sending `find references` request" << endl;

    LSP::FindReferencesRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(
        new LSP::FindReferencesRequest(GetEditorFilePath(editor), editor->GetCurrentLine(),
                                       editor->GetColumnInChars(editor->GetCurrentPosition()), false));
    QueueMessage(req);

    // Notify that operation started
    LSPEvent event_start{ wxEVT_LSP_REFERENCES_INPROGRESS };
    EventNotifier::Get()->AddPendingEvent(event_start);
}

void LanguageServerProtocol::RenameSymbol(IEditor* editor) {}

void LanguageServerProtocol::OnSemanticHighlights(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = ::clGetManager()->FindEditor(event.GetFileName());
    CHECK_PTR_RET(editor);

    if(!ShouldHandleFile(editor)) {
        return;
    }

    event.Skip(false); // don't let other services to handle this event
    OpenEditor(editor);
    SendSemanticTokensRequest(editor);
}

bool LanguageServerProtocol::CheckCapability(const LSP::ResponseMessage& res, const wxString& capabilityName,
                                             const wxString& lspRequestName)
{
    bool capabilitySupported = res["result"]["capabilities"].hasNamedObject(capabilityName);
    if(capabilitySupported) {
        m_providers.insert(lspRequestName);
    }
    return capabilitySupported;
}

bool LanguageServerProtocol::IsCapabilitySupported(const wxString& name) const { return m_providers.count(name) > 0; }

bool LanguageServerProtocol::IsDocumentSymbolsSupported() const
{
    return IsCapabilitySupported("textDocument/documentSymbol");
}

bool LanguageServerProtocol::IsDeclarationSupported() const
{
    return IsCapabilitySupported("textDocument/declaration");
}

bool LanguageServerProtocol::IsSemanticTokensSupported() const
{
    return IsCapabilitySupported("textDocument/semanticTokens/full");
}

void LanguageServerProtocol::SetStartedCallback(LSPOnConnectedCallback_t&& cb)
{
    m_onServerStartedCallback = std::move(cb);
}

//===------------------------------------------------------------------
// LSPRequestMessageQueue
//===------------------------------------------------------------------

void LSPRequestMessageQueue::Push(LSP::MessageWithParams::Ptr_t message)
{
    m_Queue.push(message);

    // Messages of type 'Request' require responses from the server
    LSP::Request* req = message->As<LSP::Request>();
    if(req) {
        m_pendingReplyMessages.insert({ req->GetId(), message });
    }
}

void LSPRequestMessageQueue::Pop()
{
    if(!m_Queue.empty()) {
        m_Queue.pop();
    }
    SetWaitingReponse(false);
}

LSP::MessageWithParams::Ptr_t LSPRequestMessageQueue::Get()
{
    if(m_Queue.empty()) {
        return LSP::MessageWithParams::Ptr_t(nullptr);
    }
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
    if(m_pendingReplyMessages.empty()) {
        return LSP::MessageWithParams::Ptr_t(nullptr);
    }
    if(m_pendingReplyMessages.count(msgid) == 0) {
        return LSP::MessageWithParams::Ptr_t(nullptr);
    }
    LSP::MessageWithParams::Ptr_t msgptr = m_pendingReplyMessages[msgid];
    m_pendingReplyMessages.erase(msgid);
    return msgptr;
}

void LanguageServerProtocol::OnWorkspaceLoaded(clWorkspaceEvent& e) { e.Skip(); }

void LanguageServerProtocol::OnWorkspaceClosed(clWorkspaceEvent& e) { e.Skip(); }

IEditor* LanguageServerProtocol::GetEditor(const clCodeCompletionEvent& event) const
{
    auto editor = clGetManager()->FindEditor(event.GetFileName());
    if(editor && editor == clGetManager()->GetActiveEditor()) {
        return editor;
    }
    return nullptr;
}

void LanguageServerProtocol::OnQuickJump(clCodeCompletionEvent& event) { OnFindSymbol(event); }

bool LanguageServerProtocol::IsLanguageSupported(const wxString& lang) const { return m_languages.count(lang) != 0; }

bool LanguageServerProtocol::IsReferencesSupported() const { return IsCapabilitySupported("textDocument/references"); }

bool LanguageServerProtocol::IsRenameSupported() const { return IsCapabilitySupported("textDocument/rename"); }
