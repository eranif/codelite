#include "LanguageServerProtocol.h"

#include "LSP/CodeActionRequest.hpp"
#include "LSP/CompletionRequest.h"
#include "LSP/DidChangeTextDocumentRequest.h"
#include "LSP/DidCloseTextDocumentRequest.h"
#include "LSP/DidOpenTextDocumentRequest.h"
#include "LSP/DidSaveTextDocumentRequest.h"
#include "LSP/DocumentSymbolsRequest.hpp"
#include "LSP/FindReferencesRequest.hpp"
#include "LSP/GotoDeclarationRequest.h"
#include "LSP/GotoDefinitionRequest.h"
#include "LSP/HoverRequest.hpp"
#include "LSP/InitializeRequest.h"
#include "LSP/InitializedNotification.hpp"
#include "LSP/LSPEvent.h"
#include "LSP/LSPNetworkRemoteSTDIO.hpp"
#include "LSP/LSPNetworkSTDIO.h"
#include "LSP/LSPNetworkSocketClient.h"
#include "LSP/RenameRequest.hpp"
#include "LSP/Request.h"
#include "LSP/ResponseError.h"
#include "LSP/ResponseMessage.h"
#include "LSP/SemanticTokensRquest.hpp"
#include "LSP/SignatureHelpRequest.h"
#include "LSP/WorkspaceExecuteCommand.hpp"
#include "LSP/WorkspaceSymbolRequest.hpp"
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

#include <unordered_map>
#include <wx/filesys.h>
#include <wx/stc/stc.h>
#include <wx/textdlg.h>

thread_local wxString emptyString;
FileExtManager::FileType LanguageServerProtocol::workspace_file_type = FileExtManager::TypeOther;

LanguageServerProtocol::LanguageServerProtocol(const wxString& name, eNetworkType netType, wxEvtHandler* owner)
    : m_name(name)
    , m_cluster(owner)
{
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &LanguageServerProtocol::OnFileSaved, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_CLOSED, &LanguageServerProtocol::OnFileClosed, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_LOADED, &LanguageServerProtocol::OnFileLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &LanguageServerProtocol::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &LanguageServerProtocol::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &LanguageServerProtocol::OnWorkspaceClosed, this);

    EventNotifier::Get()->Bind(wxEVT_CC_FIND_SYMBOL, &LanguageServerProtocol::OnFindSymbol, this);
    EventNotifier::Get()->Bind(wxEVT_CC_FIND_SYMBOL_DECLARATION, &LanguageServerProtocol::OnFindSymbolDecl, this);
    EventNotifier::Get()->Bind(wxEVT_CC_FIND_SYMBOL_DEFINITION, &LanguageServerProtocol::OnFindSymbolImpl, this);
    EventNotifier::Get()->Bind(wxEVT_CC_CODE_COMPLETE, &LanguageServerProtocol::OnCodeComplete, this);
    EventNotifier::Get()->Bind(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &LanguageServerProtocol::OnFunctionCallTip,
                               this);
    EventNotifier::Get()->Bind(wxEVT_CC_TYPEINFO_TIP, &LanguageServerProtocol::OnTypeInfoToolTip, this);
    EventNotifier::Get()->Bind(wxEVT_CC_SEMANTICS_HIGHLIGHT, &LanguageServerProtocol::OnSemanticHighlights, this);
    EventNotifier::Get()->Bind(wxEVT_CC_WORKSPACE_SYMBOLS, &LanguageServerProtocol::OnWorkspaceSymbols, this);
    EventNotifier::Get()->Bind(wxEVT_CC_FIND_HEADER_FILE, &LanguageServerProtocol::OnFindHeaderFile, this);
    EventNotifier::Get()->Bind(wxEVT_CC_JUMP_HYPER_LINK, &LanguageServerProtocol::OnQuickJump, this);
    EventNotifier::Get()->Bind(wxEVT_CC_SHOW_QUICK_OUTLINE, &LanguageServerProtocol::OnQuickOutline, this);

    // Use sockets here
    switch(netType) {
    case eNetworkType::kStdio:
#if USE_SFTP
        if(clWorkspaceManager::Get().GetWorkspace() && clWorkspaceManager::Get().GetWorkspace()->IsRemote()) {
            m_network.reset(new LSPNetworkRemoteSTDIO());
        } else {
            m_network.reset(new LSPNetworkSTDIO());
        }
#else
        m_network.reset(new LSPNetworkSTDIO());
#endif
        break;
    case eNetworkType::kTcpIP:
        m_network.reset(new LSPNetworkSocketClient());
        break;
    }
    m_network->Bind(wxEVT_LSP_NET_DATA_READY, &LanguageServerProtocol::EventMainLoop, this);
    m_network->Bind(wxEVT_LSP_NET_ERROR, &LanguageServerProtocol::OnNetError, this);
    m_network->Bind(wxEVT_LSP_NET_CONNECTED, &LanguageServerProtocol::OnNetConnected, this);
    m_network->Bind(wxEVT_LSP_NET_LOGMSG, &LanguageServerProtocol::OnNetLogMessage, this);
}

LanguageServerProtocol::~LanguageServerProtocol()
{
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &LanguageServerProtocol::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &LanguageServerProtocol::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &LanguageServerProtocol::OnFileSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_CLOSED, &LanguageServerProtocol::OnFileClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_LOADED, &LanguageServerProtocol::OnFileLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &LanguageServerProtocol::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_FIND_SYMBOL, &LanguageServerProtocol::OnFindSymbol, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_FIND_SYMBOL_DECLARATION, &LanguageServerProtocol::OnFindSymbolDecl, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_FIND_SYMBOL_DEFINITION, &LanguageServerProtocol::OnFindSymbolImpl, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_CODE_COMPLETE, &LanguageServerProtocol::OnCodeComplete, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &LanguageServerProtocol::OnFunctionCallTip,
                                 this);
    EventNotifier::Get()->Unbind(wxEVT_CC_TYPEINFO_TIP, &LanguageServerProtocol::OnTypeInfoToolTip, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_SEMANTICS_HIGHLIGHT, &LanguageServerProtocol::OnSemanticHighlights, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_WORKSPACE_SYMBOLS, &LanguageServerProtocol::OnWorkspaceSymbols, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_FIND_HEADER_FILE, &LanguageServerProtocol::OnFindHeaderFile, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_JUMP_HYPER_LINK, &LanguageServerProtocol::OnQuickJump, this);

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
             "cmake",      "yaml" };
}

void LanguageServerProtocol::QueueMessage(LSP::MessageWithParams::Ptr_t request)
{
    if(!IsInitialized()) {
        if(request->GetMethod() == "textDocument/semanticTokens/full" ||
           request->GetMethod() == "textDocument/didOpen") {
            // store the request for later processing
            m_pendingQueue.Push(request);
        }
        return;
    }

    LSP_DEBUG() << "Sending" << request->GetMethod() << "request..." << endl;
    if(request->As<LSP::CompletionRequest>()) {
        m_lastCompletionRequestId = request->As<LSP::CompletionRequest>()->GetId();
    }
    m_Queue.Push(request);
    ProcessQueue();
}

bool LanguageServerProtocol::DoStart()
{
    DoClear();

    LSP_DEBUG() << GetLogPrefix() << "Starting..." << endl;
    LSP_DEBUG() << GetLogPrefix() << "Command:" << m_startupInfo.GetLspServerCommand() << endl;
    LSP_DEBUG() << GetLogPrefix() << "Root folder:" << m_rootFolder << endl;
    for(const wxString& lang : m_languages) {
        LSP_DEBUG() << GetLogPrefix() << "Language:" << lang << endl;
    }
    try {
        // apply global environment variables
        EnvSetter env;
        // apply this lsp specific environment variables
        clEnvironment localEnv(&m_env);
        m_startupInfo.SetEnv(m_env);
        m_network->Open(m_startupInfo);
        return true;
    } catch(clException& e) {
        LSP_WARNING() << e.What();
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
    m_filesTracker.clear();
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
    return IsRunning() && m_languages.count(lang) != 0;
}

bool LanguageServerProtocol::CanHandle(FileExtManager::FileType file_type) const
{
    wxString lang = GetLanguageId(file_type);
    return IsRunning() && m_languages.count(lang) != 0;
}

bool LanguageServerProtocol::ShouldHandleFile(IEditor* editor) const
{
    // always use the local file path to determine the file path
    // this to ensure that we can open it in case we will need
    // to determine the file type based on its content
    return CanHandle(editor);
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

    if(editor->GetCtrl() != wxWindow::FindFocus()) {
        return;
    }

    if(event.GetTriggerKind() != LSP::CompletionItem::kTriggerUser && event.IsInsideCommentOrString()) {
        return;
    }

    if(CanHandle(editor)) {
        event.Skip(false);
        CodeComplete(editor, event.GetTriggerKind() == LSP::CompletionItem::kTriggerUser);
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
    wxString fileContent = editor->GetEditorText();
    SendOpenOrChangeRequest(editor, fileContent, GetLanguageId(editor));

    LSP::GotoDefinitionRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(new LSP::GotoDefinitionRequest(
        GetEditorFilePath(editor), editor->GetCurrentLine(), editor->GetColumnInChars(editor->GetCurrentPosition())));
    QueueMessage(req);
}

void LanguageServerProtocol::SendOpenOrChangeRequest(IEditor* editor, const wxString& fileContent,
                                                     const wxString& languageId)
{
    CHECK_PTR_RET(editor);
    wxString filename = GetEditorFilePath(editor);

    wxString preContent;
    if(m_filesTracker.exists(filename) && m_filesTracker.get_last_content(filename, &preContent)) {
        // we already did "open" for this, see if there are changes to report back to the language server
        auto changes = m_filesTracker.changes_from(preContent, fileContent);
        if(changes.empty()) {
            // everything is up-to-date
            LOG_IF_TRACE { LSP_TRACE() << GetLogPrefix() << "No changes detected in file:" << filename << endl; }
            return;
        }

        LSP_DEBUG() << "Sending textDocument/didChange request" << endl;
        // we have changes, send "change request" - by default we construct this request with a single
        // "text" field -> the entire document
        LSP::DidChangeTextDocumentRequest::Ptr_t req =
            LSP::MessageWithParams::MakeRequest(new LSP::DidChangeTextDocumentRequest(filename, fileContent));

        // incremental changes are supported, send them
        if(IsIncrementalChangeSupported()) {
            // only send the changes
            LSP_DEBUG() << "textDocument/didChange: using incremental changes:" << changes.size() << "changes" << endl;
            req->GetParams()->As<LSP::DidChangeTextDocumentParams>()->SetContentChanges(changes);
        } else {
            LSP_DEBUG() << "textDocument/didChange: using full change request" << endl;
        }
        QueueMessage(req);
    } else {
        LSP_DEBUG() << "Sending textDocument/didOpen request" << endl;
        // first time opening this file
        LSP::DidOpenTextDocumentRequest::Ptr_t req =
            LSP::MessageWithParams::MakeRequest(new LSP::DidOpenTextDocumentRequest(filename, fileContent, languageId));
        QueueMessage(req);

        // send a semantic request
        SendSemanticTokensRequest(editor);
    }

    // update the content for the file
    m_filesTracker.update_content(filename, fileContent);
}

void LanguageServerProtocol::SendCloseRequest(const wxString& filename)
{
    if(!m_filesTracker.exists(filename)) {
        return;
    }

    LSP::DidCloseTextDocumentRequest::Ptr_t req =
        LSP::MessageWithParams::MakeRequest(new LSP::DidCloseTextDocumentRequest(filename));
    QueueMessage(req);
    m_filesTracker.erase(filename);
}

void LanguageServerProtocol::SendSaveRequest(IEditor* editor, const wxString& fileContent)
{
    CHECK_PTR_RET(editor);
    // For now: report a change event
    wxString filename = GetEditorFilePath(editor);
    if(ShouldHandleFile(editor)) {

        // before sending the save request, send a change request
        LSP_DEBUG() << "Flushing changes before save" << endl;
        SendOpenOrChangeRequest(editor, fileContent, GetLanguageId(editor));

        LSP::CompletionRequest::Ptr_t req =
            LSP::MessageWithParams::MakeRequest(new LSP::DidSaveTextDocumentRequest(filename, fileContent));
        QueueMessage(req);

        // update the tracking by removing the file
        SendSemanticTokensRequest(editor);
    }
}

namespace
{
LSP::Range GetFileRange(wxStyledTextCtrl* ctrl)
{
    int last_line = ctrl->LineFromPosition(ctrl->GetLastPosition());
    int last_line_len = ctrl->LineLength(last_line);
    LSP::Position start_pos{ 0, 0 };
    LSP::Position end_pos{ last_line, last_line_len };
    return LSP::Range{ start_pos, end_pos };
}
} // namespace

void LanguageServerProtocol::SendCodeActionRequest(IEditor* editor, const std::vector<LSP::Diagnostic>& diags)
{
    if(ShouldHandleFile(editor)) {
        wxString filename = GetEditorFilePath(editor);
        LSP::CodeActionRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(
            new LSP::CodeActionRequest(LSP::TextDocumentIdentifier(filename), GetFileRange(editor->GetCtrl()), diags));
        QueueMessage(req);
    }
}

void LanguageServerProtocol::SendCodeCompleteRequest(IEditor* editor, size_t line, size_t column, bool userTriggered)
{
    CHECK_PTR_RET(editor);
    wxString filename = GetEditorFilePath(editor);
    if(ShouldHandleFile(editor)) {
        LSP::CompletionRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(new LSP::CompletionRequest(
            LSP::TextDocumentIdentifier(filename), LSP::Position(line, column), userTriggered));
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

    // starting fresh
    m_filesTracker.erase(GetEditorFilePath(editor));
    OpenEditor(editor);
}

void LanguageServerProtocol::OnFileClosed(clCommandEvent& event)
{
    event.Skip();
    SendCloseRequest(event.GetFileName());
    m_filesTracker.erase(event.GetFileName());
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
    LOG_IF_TRACE { LSP_TRACE() << "OpenEditor is called for" << GetEditorFilePath(editor) << endl; }
    if(!IsInitialized()) {
        LSP_TRACE() << "OpenEditor: server is still not initialized. server:" << GetName()
                    << ", file:" << GetEditorFilePath(editor) << endl;
        return;
    }

    if(editor && ShouldHandleFile(editor)) {
        wxString fileContent = editor->GetEditorText();
        SendOpenOrChangeRequest(editor, fileContent, GetLanguageId(editor));
        SendSemanticTokensRequest(editor);
        // cache symbols
        DocumentSymbols(editor, LSP::DocumentSymbolsRequest::CONTEXT_QUICK_OUTLINE |
                                    LSP::DocumentSymbolsRequest::CONTEXT_OUTLINE_VIEW);
    }
}

void LanguageServerProtocol::FunctionHelp(IEditor* editor)
{
    // sanity
    CHECK_PTR_RET(editor);
    CHECK_COND_RET(ShouldHandleFile(editor));

    // If the editor is modified, we need to tell the LSP to reparse the source file
    wxString fileContent = editor->GetEditorText();
    SendOpenOrChangeRequest(editor, fileContent, GetLanguageId(editor));
    const wxString& filename = GetEditorFilePath(editor);
    LSP::SignatureHelpRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(new LSP::SignatureHelpRequest(
        filename, editor->GetCurrentLine(), editor->GetColumnInChars(editor->GetCurrentPosition())));
    QueueMessage(req);
}

void LanguageServerProtocol::HoverTip(IEditor* editor)
{
    // sanity
    CHECK_PTR_RET(editor);
    CHECK_COND_RET(ShouldHandleFile(editor));

    // If the editor is modified, we need to tell the LSP to reparse the source file
    const wxString& filename = GetEditorFilePath(editor);
    wxString fileContent = editor->GetEditorText();
    SendOpenOrChangeRequest(editor, fileContent, GetLanguageId(editor));

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

void LanguageServerProtocol::CodeComplete(IEditor* editor, bool userTriggered)
{
    // sanity
    CHECK_PTR_RET(editor);
    CHECK_COND_RET(ShouldHandleFile(editor));
    // If the editor is modified, we need to tell the LSP to reparse the source file
    const wxString& filename = GetEditorFilePath(editor);

    wxString fileContent = editor->GetEditorText();
    SendOpenOrChangeRequest(editor, fileContent, GetLanguageId(editor));

    // Now request the for code completion
    SendCodeCompleteRequest(editor, editor->GetCurrentLine(), editor->GetColumnInChars(editor->GetCurrentPosition()),
                            userTriggered);
}

void LanguageServerProtocol::ProcessQueue()
{
    if(m_Queue.IsEmpty()) {
        return;
    }
    if(m_Queue.IsWaitingReponse()) {
        LSP_DEBUG() << "LSP is busy, will not send message";
        return;
    }
    LSP::MessageWithParams::Ptr_t req = m_Queue.Get();
    if(!IsRunning()) {
        LSP_DEBUG() << GetLogPrefix() << "is down.";
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
        LSP_DEBUG() << GetLogPrefix() << "message `textDocument/declaration` is not supported" << endl;
        return;
    }

    LSP_DEBUG() << GetLogPrefix() << "FindDeclaration() is called" << endl;
    CHECK_PTR_RET(editor);
    CHECK_COND_RET(ShouldHandleFile(editor));

    // If the editor is modified, we need to tell the LSP to reparse the source file
    const wxString& filename = GetEditorFilePath(editor);
    wxString fileContent = editor->GetEditorText();
    SendOpenOrChangeRequest(editor, fileContent, GetLanguageId(editor));

    LSP_DEBUG() << GetLogPrefix() << "Sending GotoDeclarationRequest" << endl;
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
    LSP_DEBUG() << GetLogPrefix() << "Sending initialize request...";

    // Temporarly set the state to "kInitialized" so we can send out the "initialize" request
    m_state = kInitialized;
    QueueMessage(req);
    m_state = kUnInitialized;
    m_initializeRequestID = req->As<LSP::InitializeRequest>()->GetId();
}

void LanguageServerProtocol::OnNetError(clCommandEvent& event)
{
    LSP_DEBUG() << GetLogPrefix() << "Socket error." << event.GetString();
    DoClear();
    LSPEvent restartEvent(wxEVT_LSP_RESTART_NEEDED);
    restartEvent.SetServerName(GetName());
    m_cluster->AddPendingEvent(restartEvent);
}

void LanguageServerProtocol::EventMainLoop(clCommandEvent& event)
{
    m_outputBuffer.append(event.GetStringRaw());
    LSP_DEBUG() << "Received data from LSP server of size:" << m_outputBuffer.size() << "bytes" << endl;

    m_Queue.SetWaitingReponse(false);
    while(!m_outputBuffer.empty()) {
        // attempt to consume a complete JSON payload from the aggregated network buffer
        auto json = LSP::Message::GetJSONPayload(m_outputBuffer);
        if(!json) {
            LOG_IF_TRACE { LSP_TRACE() << "Unable to read JSON payload" << endl; }
            LOG_IF_DEBUG
            {
                // for debugging purposes,
                // once we exceeded 1MB of data, something is broken...
                // dump the output buffer into a file and continue
                // we only dump 3 files per CodeLite session
                static size_t dumps_count = 0;
                if(dumps_count < 3 && (m_outputBuffer.size() > (1024 * 1024 * 1024))) {
                    dumps_count++;
                    auto tmp_filename =
                        FileUtils::CreateTempFileName(clStandardPaths::Get().GetTempDir(), "cl_lsp", "txt");
                    FileUtils::WriteFileContent(tmp_filename, m_outputBuffer);
                    LSP_SYSTEM() << "Output buffer exceeds 1MB (" << m_outputBuffer.size() << "Bytes)" << endl;
                    LSP_SYSTEM() << "Dumped m_outputBuffer into:" << tmp_filename.GetFullPath() << endl;
                }
            }
            break;
        }

        auto json_item = json->toElement();
        // check the message type
        wxString message_method = json_item["method"].toString();
        // LSP_TRACE() << "-- LSP:" << json_item.format(false) << endl;
        LOG_IF_TRACE { LSP_TRACE() << "-- LSP: Message Method is:" << message_method << endl; }

        if(message_method == "window/logMessage" || message_method == "window/showMessage") {
            // log this message
            LSPEvent log_event(wxEVT_LSP_LOGMESSAGE);
            log_event.SetServerName(GetName());
            log_event.SetMessage(json_item["params"]["message"].toString());
            log_event.SetLogMessageSeverity(json_item["params"]["type"].toInt());
            m_cluster->AddPendingEvent(log_event);

        } else if(message_method == "telemetry/event") {
            // show dialog to the user
            // log this message
            LSPEvent log_event(wxEVT_LSP_LOGMESSAGE);
            log_event.SetServerName(GetName());
            log_event.SetMessage(json_item["params"].toString());
            m_cluster->AddPendingEvent(log_event);
        } else if(message_method == "workspace/applyEdit") {

            // the server is requesting us to apply an edit
            HandleWorkspaceEdit(json_item["params"]["edit"]);

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
                        LSP_DEBUG() << GetLogPrefix() << "Server semantic tokens are:" << m_semanticTokensTypes << endl;
                    }

                    CheckCapability(res, "documentSymbolProvider", "textDocument/documentSymbol");
                    CheckCapability(res, "declarationProvider", "textDocument/declaration");
                    CheckCapability(res, "workspaceSymbolProvider", "workspace/symbol");
                    CheckCapability(res, "renameProvider", "textDocument/rename");
                    CheckCapability(res, "referencesProvider", "textDocument/references");
                    // Check for textDocumentSync capability
                    // https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/#textDocumentSyncOptions
                    if(res["result"]["capabilities"]["textDocumentSync"]["change"].toInt(wxNOT_FOUND) == 2) {
                        m_incrementalChangeSupported = true;
                    }

                    LSP_DEBUG() << GetLogPrefix() << "Sending InitializedNotification" << endl;

                    LSP::InitializedNotification::Ptr_t initNotification =
                        LSP::MessageWithParams::MakeRequest(new LSP::InitializedNotification());
                    QueueMessage(initNotification);

                    // Send LSP::InitializedNotification to the server
                    LSP_DEBUG() << GetLogPrefix() << "initialization completed" << endl;
                    m_initializeRequestID = wxNOT_FOUND;

                    // Notify about this
                    LSPEvent initEvent(wxEVT_LSP_INITIALIZED);
                    initEvent.SetServerName(GetName());
                    m_cluster->AddPendingEvent(initEvent);

                    // Move the content of the pending queue into the main queue
                    m_Queue.Move(m_pendingQueue);

                } else {
                    LSP_DEBUG() << GetLogPrefix() << "Server not initialized. This message is ignored";
                }
            }
        }
    }
    ProcessQueue();
}

void LanguageServerProtocol::Stop()
{
    LSP_DEBUG() << GetLogPrefix() << "Going down";
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

    LOG_IF_TRACE { LSP_TRACE() << "LanguageServerProtocol::OnQuickOutline called" << endl; }
    IEditor* editor = GetEditor(event);
    CHECK_PTR_RET(editor);

    if(CanHandle(editor) && IsDocumentSymbolsSupported()) {
        // this event is ours to handle
        event.Skip(false);
        DocumentSymbols(editor, LSP::DocumentSymbolsRequest::CONTEXT_QUICK_OUTLINE |
                                    LSP::DocumentSymbolsRequest::CONTEXT_OUTLINE_VIEW);
        // dont wait for the response, but fire an event to load the dialog
        LSPEvent show_quick_outline_dlg_event(wxEVT_LSP_SHOW_QUICK_OUTLINE_DLG);
        m_cluster->AddPendingEvent(show_quick_outline_dlg_event);
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
    LSP_DEBUG() << GetLogPrefix() << "Sending workspace symbol request" << endl;
    LSP::WorkspaceSymbolRequest::Ptr_t req =
        LSP::MessageWithParams::MakeRequest(new LSP::WorkspaceSymbolRequest(query_string));
    QueueMessage(req);
}

void LanguageServerProtocol::SendSemanticTokensRequest(IEditor* editor)
{
    CHECK_PTR_RET(editor);
    wxString filepath = GetEditorFilePath(editor);

    // check if this is implemented by the server
    if(IsSemanticTokensSupported()) {
        LSP::DidChangeTextDocumentRequest::Ptr_t req =
            LSP::MessageWithParams::MakeRequest(new LSP::SemanticTokensRquest(filepath));
        QueueMessage(req);

    } else if(IsDocumentSymbolsSupported()) {
        // Use DocumentSymbol instead
        DocumentSymbols(editor, LSP::DocumentSymbolsRequest::CONTEXT_SEMANTIC_HIGHLIGHT);
    }
}

void LanguageServerProtocol::HandleResponseError(LSP::ResponseMessage& response, LSP::MessageWithParams::Ptr_t msg_ptr)
{
    LSP_DEBUG() << GetLogPrefix() << "received an error message:" << response.ToString() << endl;
    LSP::ResponseError errMsg(response.ToString());
    switch(errMsg.GetErrorCode()) {
    case LSP::ResponseError::kErrorCodeInternalError:
    case LSP::ResponseError::kErrorCodeInvalidRequest: {
        // Restart this server
        LSPEvent restartEvent(wxEVT_LSP_RESTART_NEEDED);
        restartEvent.SetServerName(GetName());
        m_cluster->AddPendingEvent(restartEvent);
    } break;
    case LSP::ResponseError::kErrorCodeMethodNotFound: {
        // Report this missing event
        LSPEvent eventMethodNotFound(wxEVT_LSP_METHOD_NOT_FOUND);
        eventMethodNotFound.SetServerName(GetName());
        eventMethodNotFound.SetString(msg_ptr->GetMethod());
        m_cluster->AddPendingEvent(eventMethodNotFound);

        // Log this message
        LSPEvent log_event(wxEVT_LSP_LOGMESSAGE);
        log_event.SetServerName(GetName());
        log_event.SetMessage(_("Method: `") + msg_ptr->GetMethod() + _("` is not supported"));
        log_event.SetLogMessageSeverity(LSP_LOG_WARNING); // warning
        m_cluster->AddPendingEvent(log_event);

    } break;
    case LSP::ResponseError::kErrorCodeInvalidParams: {
        // Recreate this AST (in other words: reparse), by default we reparse the current editor
        LSPEvent reparseEvent(wxEVT_LSP_REPARSE_NEEDED);
        reparseEvent.SetServerName(GetName());
        m_cluster->AddPendingEvent(reparseEvent);
    } break;
    case LSP::ResponseError::kErrorCodeUnknownErrorCode:
    default: {
        // log it
        LSPEvent log_event(wxEVT_LSP_LOGMESSAGE);
        log_event.SetServerName(GetName());
        log_event.SetMessage(errMsg.GetMessage());
        log_event.SetLogMessageSeverity(LSP_LOG_ERROR);
        m_cluster->AddPendingEvent(log_event);
    } break;
    }

    // finally, call the request handler
    if(msg_ptr->As<LSP::Request>()) {
        msg_ptr->As<LSP::Request>()->OnError(response, m_cluster);
    }
}

void LanguageServerProtocol::HandleResponse(LSP::ResponseMessage& response, LSP::MessageWithParams::Ptr_t msg_ptr)
{
    if(msg_ptr && msg_ptr->As<LSP::Request>()) {
        LOG_IF_TRACE { LSP_TRACE() << GetLogPrefix() << "received a response"; }
        LSP::Request* preq = msg_ptr->As<LSP::Request>();
        if(preq->As<LSP::CompletionRequest>() && (preq->GetId() < m_lastCompletionRequestId)) {
            LSP_TRACE() << "Received a response for completion message ID#" << preq->GetId()
                        << ". However, a newer completion request with ID#" << m_lastCompletionRequestId
                        << "was already sent. Dropping response";
            return;
        }
        preq->SetServerName(GetName());
        LSP_DEBUG() << "Processing response for request:" << preq->GetMethod() << endl;
        preq->OnResponse(response, m_cluster);

    } else if(response.IsPushDiagnostics()) {
        // Get the URI
        LSP_DEBUG() << "Received diagnostic message:" << endl;
        wxString fn = FileUtils::FilePathFromURI(response.GetDiagnosticsUri());

        // Don't show this message on macOS as it appears in the middle of the screen...
        clGetManager()->SetStatusMessage(wxString() << GetLogPrefix() << " parsing of file: " << fn << " is completed",
                                         1);

        std::vector<LSP::Diagnostic> diags = response.GetDiagnostics();
        if(!diags.empty() && IsDisaplayDiagnostics()) {
            // report the diagnostics
            LSPEvent eventSetDiags(wxEVT_LSP_SET_DIAGNOSTICS);
            eventSetDiags.SetFileName(fn);
            eventSetDiags.GetLocation().SetPath(fn);
            eventSetDiags.SetDiagnostics(diags);
            EventNotifier::Get()->AddPendingEvent(eventSetDiags);
        } else if(diags.empty()) {
            // clear all diagnostics
            LSPEvent eventClearDiags(wxEVT_LSP_CLEAR_DIAGNOSTICS);
            eventClearDiags.SetFileName(fn);
            eventClearDiags.GetLocation().SetPath(fn);
            EventNotifier::Get()->AddPendingEvent(eventClearDiags);
        }
    }
}

void LanguageServerProtocol::OnFindHeaderFile(clCodeCompletionEvent& event)
{
    LSP_DEBUG() << GetLogPrefix() << "OnFindHeaderFile() is called" << endl;
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
    LSP_DEBUG() << GetLogPrefix() << "Sending `find references` request" << endl;

    LSP::FindReferencesRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(
        new LSP::FindReferencesRequest(GetEditorFilePath(editor), editor->GetCurrentLine(),
                                       editor->GetColumnInChars(editor->GetCurrentPosition()), false));
    QueueMessage(req);

    // Notify that operation started
    LSPEvent event_start{ wxEVT_LSP_REFERENCES_INPROGRESS };
    EventNotifier::Get()->AddPendingEvent(event_start);
}

void LanguageServerProtocol::RenameSymbol(IEditor* editor)
{
    CHECK_PTR_RET(editor);
    CHECK_EXPECTED_RETURN(IsRenameSupported(), true);
    LSP_DEBUG() << GetLogPrefix() << "Sending `rename symbol` request" << endl;

    wxString old_name = editor->GetWordAtCaret();
    old_name.Trim().Trim(false);

    if(old_name.empty()) {
        // nothing to be done here
        return;
    }

    wxString title;
    title << _("Rename Symbol: '") << old_name << "'";
    wxString newname = wxGetTextFromUser(_("New name:"), title, old_name);
    if(newname.empty() || newname == old_name) {
        return;
    }

    LSP::RenameRequest::Ptr_t req = LSP::MessageWithParams::MakeRequest(
        new LSP::RenameRequest(newname, GetEditorFilePath(editor), editor->GetCurrentLine(),
                               editor->GetColumnInChars(editor->GetCurrentPosition())));
    QueueMessage(req);
}

void LanguageServerProtocol::OnSemanticHighlights(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = event.GetFileName().empty() ? clGetManager()->GetActiveEditor()
                                                  : clGetManager()->FindEditor(event.GetFileName());
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

void LSPRequestMessageQueue::Move(LSPRequestMessageQueue& other)
{
    while(!other.m_Queue.empty()) {
        m_Queue.push(other.m_Queue.front());
        other.m_Queue.pop();
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

bool LanguageServerProtocol::IsIncrementalChangeSupported() const { return m_incrementalChangeSupported; }

void LanguageServerProtocol::SendWorkspaceExecuteCommand(const wxString& filepath, const LSP::Command& command)
{
    auto editor = clGetManager()->FindEditor(filepath);
    if(!editor) {
        LSP_ERROR() << "Could not send workspace/executeCommand: could not locate editor for file:" << filepath << endl;
        return;
    }

    if(ShouldHandleFile(editor)) {
        LSP_DEBUG() << "Sending `workspace/executeCommand`" << endl;
        wxString filename = GetEditorFilePath(editor);
        LSP::WorkspaceExecuteCommand::Ptr_t req =
            LSP::MessageWithParams::MakeRequest(new LSP::WorkspaceExecuteCommand(filename, command));
        QueueMessage(req);
    }
}

void LanguageServerProtocol::HandleWorkspaceEdit(const JSONItem& changes)
{
    auto edits = LSP::ParseWorkspaceEdit(changes);

    LSPEvent edit_event{ wxEVT_LSP_EDIT_FILES };
    edit_event.SetChanges(edits);
    edit_event.SetAnswer(false); // Do not prompt the user
    m_cluster->AddPendingEvent(edit_event);
}

void LanguageServerProtocol::OnNetLogMessage(clCommandEvent& event)
{
    LSPEvent log_event{ wxEVT_LSP_LOGMESSAGE };
    log_event.SetServerName(GetName());
    log_event.SetMessage(event.GetString());
    log_event.SetLogMessageSeverity(event.GetInt());
    m_cluster->AddPendingEvent(log_event);
}
