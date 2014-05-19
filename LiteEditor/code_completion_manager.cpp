#include "code_completion_manager.h"
#include "clang_code_completion.h"
#include "tags_options_data.h"
#include <vector>
#include "ctags_manager.h"
#include "entry.h"
#include "frame.h"
#include "clang_compilation_db_thread.h"
#include "compilation_database.h"
#include "event_notifier.h"
#include "plugin.h"
#include "file_logger.h"

static CodeCompletionManager *ms_CodeCompletionManager = NULL;

CodeCompletionManager::CodeCompletionManager()
    : m_options(CC_CTAGS_ENABLED)
    , m_wordCompletionRefreshNeeded(false)
    , m_buildInProgress(false)
{
    EventNotifier::Get()->Connect(wxEVT_BUILD_ENDED, clBuildEventHandler(CodeCompletionManager::OnBuildEnded), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_BUILD_STARTED, clBuildEventHandler(CodeCompletionManager::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_COMPILE_COMMANDS_JSON_GENERATED, &CodeCompletionManager::OnCompileCommandsFileGenerated, this);
    
    wxTheApp->Bind(wxEVT_ACTIVATE_APP, &CodeCompletionManager::OnAppActivated, this );
}

CodeCompletionManager::~CodeCompletionManager()
{
    EventNotifier::Get()->Disconnect(wxEVT_BUILD_ENDED, clBuildEventHandler(CodeCompletionManager::OnBuildEnded), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_BUILD_STARTED, clBuildEventHandler(CodeCompletionManager::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_COMPILE_COMMANDS_JSON_GENERATED, &CodeCompletionManager::OnCompileCommandsFileGenerated, this);
    
    wxTheApp->Unbind(wxEVT_ACTIVATE_APP, &CodeCompletionManager::OnAppActivated, this );
}

void CodeCompletionManager::WordCompletion(LEditor *editor, const wxString& expr, const wxString& word)
{
    wxString expression = expr;
    wxString tmp;

    DoUpdateOptions();

    // Trim whitespace from right and left
    static wxString trimString(wxT("!<>=(){};\r\n\t\v "));

    expression = expression.erase(0, expression.find_first_not_of(trimString));
    expression = expression.erase(expression.find_last_not_of(trimString)+1);

    if(expression.EndsWith(word, &tmp)) {
        expression = tmp;
    }


    if((GetOptions() & CC_CLANG_ENABLED) && (GetOptions() & CC_CLANG_FIRST)) {
        DoClangWordCompletion(editor);

    } else {
        if(!DoCtagsWordCompletion(editor, expr, word) && (GetOptions() & CC_CLANG_ENABLED)) {
            DoClangWordCompletion(editor);
        }
    }
}

CodeCompletionManager& CodeCompletionManager::Get()
{
    if ( !ms_CodeCompletionManager ) {
        ms_CodeCompletionManager = new CodeCompletionManager;
    }
    return *ms_CodeCompletionManager;
}

bool CodeCompletionManager::DoCtagsWordCompletion(LEditor* editor, const wxString& expr, const wxString& word)
{
    std::vector<TagEntryPtr> candidates;
    //get the full text of the current page
    wxString text = editor->GetTextRange    (0, editor->GetCurrentPosition());
    int lineNum   = editor->LineFromPosition(editor->GetCurrentPosition())+1;

    if(TagsManagerST::Get()->WordCompletionCandidates(editor->GetFileName(), lineNum, expr, text, word, candidates) && !candidates.empty()) {
        editor->ShowCompletionBox(candidates, word, false);
        return true;
    }
    return false;
}

void CodeCompletionManager::DoClangWordCompletion(LEditor* editor)
{
#if HAS_LIBCLANG
    DoUpdateOptions();
    if(GetOptions() & CC_CLANG_ENABLED)
        ClangCodeCompletion::Instance()->WordComplete(editor);
#else
    wxUnusedVar(editor);
#endif
}

bool CodeCompletionManager::DoCtagsCalltip(LEditor* editor, int line, const wxString &expr, const wxString &text, const wxString &word)
{
    // Get the calltip
    clCallTipPtr tip = TagsManagerST::Get()->GetFunctionTip(editor->GetFileName(), line, expr, text, word);
    if(!tip || !tip->Count()) {
        // try the Clang engine...
        return false;
    }
    editor->ShowCalltip(tip);
    return true;
}

void CodeCompletionManager::DoClangCalltip(LEditor* editor)
{
#if HAS_LIBCLANG
    DoUpdateOptions();
    if(GetOptions() & CC_CLANG_ENABLED)
        ClangCodeCompletion::Instance()->Calltip(editor);
#else
    wxUnusedVar(editor);
#endif
}

void CodeCompletionManager::Calltip(LEditor* editor, int line, const wxString& expr, const wxString& text, const wxString& word)
{
    bool res (false);
    DoUpdateOptions();

    if(::IsCppKeyword(word))
        return;

    if(GetOptions() & CC_CTAGS_ENABLED) {
        res = DoCtagsCalltip(editor, line, expr, text, word);
    }

    if(!res && (GetOptions() & CC_CLANG_ENABLED)) {
        DoClangCalltip(editor);
    }
}

void CodeCompletionManager::CodeComplete(LEditor* editor, int line, const wxString& expr, const wxString& text)
{
    bool res (false);
    DoUpdateOptions();
    if(GetOptions() & CC_CTAGS_ENABLED) {
        res = DoCtagsCodeComplete(editor, line, expr, text);
    }

    if(!res && (GetOptions() & CC_CLANG_ENABLED)) {
        DoClangCodeComplete(editor);
    }
}

void CodeCompletionManager::DoClangCodeComplete(LEditor* editor)
{
#if HAS_LIBCLANG
    ClangCodeCompletion::Instance()->CodeComplete(editor);
#else
    wxUnusedVar(editor);
#endif
}

bool CodeCompletionManager::DoCtagsCodeComplete(LEditor* editor, int line, const wxString& expr, const wxString& text)
{
    std::vector<TagEntryPtr> candidates;
    if (TagsManagerST::Get()->AutoCompleteCandidates(editor->GetFileName(), line, expr, text, candidates) && !candidates.empty()) {
        editor->ShowCompletionBox(candidates, wxEmptyString, false);
        return true;
    }
    return false;
}

void CodeCompletionManager::DoUpdateOptions()
{
    const TagsOptionsData& options = TagsManagerST::Get()->GetCtagsOptions();
    m_options = options.GetClangOptions();

    m_options |= CC_CTAGS_ENABLED; // For now, we always enables CTAGS

    // Incase CLANG is set as the main CC engine, remove the CTAGS options BUT
    // only if CLANG is enabled...
    if((m_options & CC_CLANG_FIRST) && (m_options & CC_CLANG_ENABLED)) {
        m_options &= ~CC_CTAGS_ENABLED;
    }
}

void CodeCompletionManager::ProcessMacros(LEditor* editor)
{
#if HAS_LIBCLANG
    // Currently only supported when compiled with clang
    ClangCodeCompletion::Instance()->ListMacros(editor);
#endif
}

void CodeCompletionManager::GotoImpl(LEditor* editor)
{
    DoUpdateOptions();

    bool res = false;

    if(GetOptions() & CC_CTAGS_ENABLED) {
        res = DoCtagsGotoImpl(editor);
    }

    if(!res && (GetOptions() & CC_CLANG_ENABLED)) {
        DoClangGotoImpl(editor);
    }
}

void CodeCompletionManager::DoClangGotoImpl(LEditor* editor)
{
    wxUnusedVar(editor);
#if HAS_LIBCLANG
    ClangCodeCompletion::Instance()->GotoImplementation(editor);
#endif
}

bool CodeCompletionManager::DoCtagsGotoImpl(LEditor* editor)
{
    TagEntryPtr tag = editor->GetContext()->GetTagAtCaret(true, true);
    if (tag) {
        LEditor *editor = clMainFrame::Get()->GetMainBook()->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine()-1);
        if(!editor) {
            return false;
        }
        // Use the async funtion here. Synchronously usually works but, if the file wasn't loaded, sometimes the EnsureVisible code is called too early and fails
        editor->FindAndSelectV(tag->GetPattern(), tag->GetName());
        return true;
    }
    return false;
}

void CodeCompletionManager::DoClangGotoDecl(LEditor* editor)
{
    wxUnusedVar(editor);
#if HAS_LIBCLANG
    ClangCodeCompletion::Instance()->GotoDeclaration(editor);
#endif
}

bool CodeCompletionManager::DoCtagsGotoDecl(LEditor* editor)
{
    TagEntryPtr tag = editor->GetContext()->GetTagAtCaret(true, false);
    if (tag) {
        LEditor *editor = clMainFrame::Get()->GetMainBook()->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine()-1);
        if(!editor) {
            return false;
        }
        // Use the async funtion here. Synchronously usually works but, if the file wasn't loaded, sometimes the EnsureVisible code is called too early and fails
        editor->FindAndSelectV(tag->GetPattern(), tag->GetName());
        return true;
    }
    return false;
}

void CodeCompletionManager::GotoDecl(LEditor* editor)
{
    DoUpdateOptions();
    bool res = false;

    if(GetOptions() & CC_CTAGS_ENABLED) {
        res = DoCtagsGotoDecl(editor);
    }

    if(!res && (GetOptions() & CC_CLANG_ENABLED)) {
        DoClangGotoDecl(editor);
    }
}

void CodeCompletionManager::OnBuildEnded(clBuildEvent& e)
{
    e.Skip();
    DoUpdateCompilationDatabase();
    m_buildInProgress = false;
}

void CodeCompletionManager::DoUpdateCompilationDatabase()
{
    // Create a worker thread (detached thread) that 
    // will initialize the database now that the compilation has ended
    CompilationDatabase db;
    ClangCompilationDbThreadST::Get()->AddFile( db.GetFileName().GetFullPath() );
}

void CodeCompletionManager::OnAppActivated(wxActivateEvent& e)
{
    e.Skip();
}

void CodeCompletionManager::Release()
{
    wxDELETE(ms_CodeCompletionManager);
}

void CodeCompletionManager::OnBuildStarted(clBuildEvent& e)
{
    e.Skip();
    m_buildInProgress = true;
}

void CodeCompletionManager::OnCompileCommandsFileGenerated(clCommandEvent& event)
{
    event.Skip();
    CL_DEBUG("-- Code Completion Manager: process file 'compile_commands.json' file" );
    CompilationDatabase db;
    ClangCompilationDbThreadST::Get()->AddFile( db.GetFileName().GetFullPath() );
    clMainFrame::Get()->SetStatusText("Ready");
}
