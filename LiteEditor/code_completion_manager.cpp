//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : code_completion_manager.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

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
#include "fileextmanager.h"
#include "cl_editor.h"
#include "compilation_database.h"
#include "compiler_command_line_parser.h"
#include "language.h"
#include "code_completion_api.h"

static CodeCompletionManager* ms_CodeCompletionManager = NULL;

// Helper class
struct EditorDimmerDisabler {
    LEditor* m_editor;
    EditorDimmerDisabler(LEditor* editor)
        : m_editor(editor)
    {
        if(m_editor) {
            m_editor->SetPreProcessorsWords("");
            m_editor->GetCtrl()->SetProperty(wxT("lexer.cpp.track.preprocessor"), wxT("0"));
            m_editor->GetCtrl()->SetProperty(wxT("lexer.cpp.update.preprocessor"), wxT("0"));
            m_editor->GetCtrl()->Colourise(0, wxSTC_INVALID_POSITION);
        }
    }

    ~EditorDimmerDisabler() {}
};

CodeCompletionManager::CodeCompletionManager()
    : m_options(CC_CTAGS_ENABLED)
    , m_wordCompletionRefreshNeeded(false)
    , m_buildInProgress(false)
{
    EventNotifier::Get()->Connect(
        wxEVT_BUILD_ENDED, clBuildEventHandler(CodeCompletionManager::OnBuildEnded), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_BUILD_STARTED, clBuildEventHandler(CodeCompletionManager::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Bind(
        wxEVT_COMPILE_COMMANDS_JSON_GENERATED, &CodeCompletionManager::OnCompileCommandsFileGenerated, this);

    EventNotifier::Get()->Connect(
        wxEVT_FILE_SAVED, clCommandEventHandler(CodeCompletionManager::OnFileSaved), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_FILE_LOADED, clCommandEventHandler(CodeCompletionManager::OnFileLoaded), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_WORKSPACE_CONFIG_CHANGED, wxCommandEventHandler(CodeCompletionManager::OnWorkspaceConfig), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_PROJ_SETTINGS_SAVED, wxCommandEventHandler(CodeCompletionManager::OnWorkspaceConfig), NULL, this);
    wxTheApp->Bind(wxEVT_ACTIVATE_APP, &CodeCompletionManager::OnAppActivated, this);

    EventNotifier::Get()->Connect(
        wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(CodeCompletionManager::OnWorkspaceClosed), NULL, this);
    EventNotifier::Get()->Bind(
        wxEVT_ENVIRONMENT_VARIABLES_MODIFIED, &CodeCompletionManager::OnEnvironmentVariablesModified, this);
    // Start the worker threads
    m_preProcessorThread.Start();
    m_usingNamespaceThread.Start();
}

CodeCompletionManager::~CodeCompletionManager()
{
    m_preProcessorThread.Stop();
    m_usingNamespaceThread.Stop();
    EventNotifier::Get()->Disconnect(
        wxEVT_BUILD_ENDED, clBuildEventHandler(CodeCompletionManager::OnBuildEnded), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_BUILD_STARTED, clBuildEventHandler(CodeCompletionManager::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Unbind(
        wxEVT_COMPILE_COMMANDS_JSON_GENERATED, &CodeCompletionManager::OnCompileCommandsFileGenerated, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_FILE_SAVED, clCommandEventHandler(CodeCompletionManager::OnFileSaved), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_FILE_LOADED, clCommandEventHandler(CodeCompletionManager::OnFileLoaded), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_WORKSPACE_CONFIG_CHANGED, wxCommandEventHandler(CodeCompletionManager::OnWorkspaceConfig), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_PROJ_SETTINGS_SAVED, wxCommandEventHandler(CodeCompletionManager::OnWorkspaceConfig), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(CodeCompletionManager::OnWorkspaceClosed), NULL, this);
    wxTheApp->Unbind(wxEVT_ACTIVATE_APP, &CodeCompletionManager::OnAppActivated, this);
    EventNotifier::Get()->Unbind(
        wxEVT_ENVIRONMENT_VARIABLES_MODIFIED, &CodeCompletionManager::OnEnvironmentVariablesModified, this);
}

void CodeCompletionManager::WordCompletion(LEditor* editor, const wxString& expr, const wxString& word)
{
    wxString expression = expr;
    wxString tmp;

    DoUpdateOptions();

    // Trim whitespace from right and left
    static wxString trimString(wxT("!<>=(){};\r\n\t\v "));

    expression = expression.erase(0, expression.find_first_not_of(trimString));
    expression = expression.erase(expression.find_last_not_of(trimString) + 1);

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
    if(!ms_CodeCompletionManager) {
        ms_CodeCompletionManager = new CodeCompletionManager;
    }
    return *ms_CodeCompletionManager;
}

bool CodeCompletionManager::DoCtagsWordCompletion(LEditor* editor, const wxString& expr, const wxString& word)
{
    std::vector<TagEntryPtr> candidates;
    // get the full text of the current page
    wxString text = editor->GetTextRange(0, editor->GetCurrentPosition());
    int lineNum = editor->LineFromPosition(editor->GetCurrentPosition()) + 1;

    if(TagsManagerST::Get()->WordCompletionCandidates(editor->GetFileName(), lineNum, expr, text, word, candidates) &&
        !candidates.empty()) {
        editor->ShowCompletionBox(candidates, word);
        return true;
    }
    return false;
}

void CodeCompletionManager::DoClangWordCompletion(LEditor* editor)
{
#if HAS_LIBCLANG
    DoUpdateOptions();
    if(GetOptions() & CC_CLANG_ENABLED) ClangCodeCompletion::Instance()->WordComplete(editor);
#else
    wxUnusedVar(editor);
#endif
}

bool CodeCompletionManager::DoCtagsCalltip(
    LEditor* editor, int line, const wxString& expr, const wxString& text, const wxString& word)
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
    if(GetOptions() & CC_CLANG_ENABLED) ClangCodeCompletion::Instance()->Calltip(editor);
#else
    wxUnusedVar(editor);
#endif
}

void CodeCompletionManager::Calltip(
    LEditor* editor, int line, const wxString& expr, const wxString& text, const wxString& word)
{
    bool res(false);
    DoUpdateOptions();

    if(::IsCppKeyword(word)) return;

    if(GetOptions() & CC_CTAGS_ENABLED) {
        res = DoCtagsCalltip(editor, line, expr, text, word);
    }

    if(!res && (GetOptions() & CC_CLANG_ENABLED)) {
        DoClangCalltip(editor);
    }
}

void CodeCompletionManager::CodeComplete(LEditor* editor, int line, const wxString& expr, const wxString& text)
{
    bool res(false);
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
    if(TagsManagerST::Get()->AutoCompleteCandidates(editor->GetFileName(), line, expr, text, candidates) &&
        !candidates.empty()) {
        editor->ShowCompletionBox(candidates, wxEmptyString);
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
    // Sanity
    CHECK_PTR_RET(editor);

    /// disable the editor pre-processor dimming
    EditorDimmerDisabler eds(editor);

    wxArrayString macros;
    wxArrayString includePaths;
    if(!GetDefinitionsAndSearchPaths(editor, includePaths, macros)) return;

    // Queue this request in the worker thread
    m_preProcessorThread.QueueFile(editor->GetFileName().GetFullPath(), macros, includePaths);
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
    if(tag) {
        LEditor* editor =
            clMainFrame::Get()->GetMainBook()->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine() - 1);
        if(!editor) {
            return false;
        }
        // Use the async funtion here. Synchronously usually works but, if the file wasn't loaded, sometimes the
        // EnsureVisible code is called too early and fails
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
    if(tag) {
        LEditor* editor =
            clMainFrame::Get()->GetMainBook()->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine() - 1);
        if(!editor) {
            return false;
        }
        // Use the async funtion here. Synchronously usually works but, if the file wasn't loaded, sometimes the
        // EnsureVisible code is called too early and fails
        if(!editor->FindAndSelect(tag->GetPattern(), tag->GetName())) {
            editor->SetCaretAt(editor->PosFromLine(tag->GetLine() - 1));
            editor->CenterLineIfNeeded(tag->GetLine() - 1);
        }
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
    ClangCompilationDbThreadST::Get()->AddFile(db.GetFileName().GetFullPath());
}

void CodeCompletionManager::OnAppActivated(wxActivateEvent& e) { e.Skip(); }

void CodeCompletionManager::Release() { wxDELETE(ms_CodeCompletionManager); }

void CodeCompletionManager::OnBuildStarted(clBuildEvent& e)
{
    e.Skip();
    m_buildInProgress = true;
}

void CodeCompletionManager::OnCompileCommandsFileGenerated(clCommandEvent& event)
{
    event.Skip();
    CL_DEBUG("-- Code Completion Manager: process file 'compile_commands.json' file");
    CompilationDatabase db;
    ClangCompilationDbThreadST::Get()->AddFile(db.GetFileName().GetFullPath());
    clMainFrame::Get()->SetStatusText("Ready");
}

void CodeCompletionManager::OnParseThreadCollectedMacros(const wxArrayString& definitions, const wxString& filename)
{
    CL_DEBUG("Parser thread returned %d macros\n", (int)definitions.GetCount());
    // We got a list of macros from the parser thead
    // prepare a space delimited list out of it
    wxString macrosAsString;
    for(size_t i = 0; i < definitions.GetCount(); ++i) {
        // CL_DEBUG("%s\n", definitions.Item(i));
        macrosAsString << definitions.Item(i) << " ";
    }
    LEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(filename);
    if(editor) {
        CL_DEBUG("Updating editor colours...");
        // its the same file that triggered the request, update its pre processor colouring
        // turn off the macro colouring (until new set is arrived)
        editor->SetPreProcessorsWords(macrosAsString);
        editor->GetCtrl()->SetProperty(wxT("lexer.cpp.track.preprocessor"), wxT("1"));
        editor->GetCtrl()->SetProperty(wxT("lexer.cpp.update.preprocessor"), wxT("1"));
        editor->GetCtrl()->SetKeyWords(4, macrosAsString);
        editor->GetCtrl()->Colourise(0, wxSTC_INVALID_POSITION);
        CL_DEBUG("Updating editor colours...done");
    }
}

void CodeCompletionManager::OnFileSaved(clCommandEvent& event)
{
    event.Skip();
    if(TagsManagerST::Get()->GetCtagsOptions().GetCcColourFlags() & CC_COLOUR_MACRO_BLOCKS) {
        ProcessMacros(clMainFrame::Get()->GetMainBook()->FindEditor(event.GetFileName()));
    }
}

void CodeCompletionManager::OnFileLoaded(clCommandEvent& event)
{
    event.Skip();
    LEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(event.GetFileName());
    CHECK_PTR_RET(editor);

    // Handle Pre Processor block colouring
    const size_t colourOptions = TagsManagerST::Get()->GetCtagsOptions().GetCcColourFlags();
    const size_t ccFlags = TagsManagerST::Get()->GetCtagsOptions().GetFlags();
    if(!(colourOptions & CC_COLOUR_MACRO_BLOCKS)) {
        editor->SetPreProcessorsWords("");
        editor->SetProperty("lexer.cpp.track.preprocessor", "0");
        editor->SetProperty("lexer.cpp.update.preprocessor", "0");
    } else {
        ProcessMacros(editor);
    }

    if(editor && (ccFlags & CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING)) {
        ProcessUsingNamespace(editor);
    }
}

void CodeCompletionManager::RefreshPreProcessorColouring()
{
    bool enableBlockColouring = TagsManagerST::Get()->GetCtagsOptions().GetCcColourFlags() & CC_COLOUR_MACRO_BLOCKS;
    LEditor::Vec_t editors;
    clMainFrame::Get()->GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    if(!enableBlockColouring) {
        for(size_t i = 0; i < editors.size(); ++i) {
            LEditor* editor = editors.at(i);
            editor->SetPreProcessorsWords("");
            editor->SetProperty("lexer.cpp.track.preprocessor", "0");
            editor->SetProperty("lexer.cpp.update.preprocessor", "0");
        }
    } else {
        for(size_t i = 0; i < editors.size(); ++i) {
            LEditor* editor = editors.at(i);
            editor->SetProperty("lexer.cpp.track.preprocessor", "0");
            editor->SetProperty("lexer.cpp.update.preprocessor", "0");
            ProcessMacros(editor);
        }
    }
}

void CodeCompletionManager::OnWorkspaceConfig(wxCommandEvent& event)
{
    event.Skip();
    Project::ClearBacktickCache();
    RefreshPreProcessorColouring();
}

void CodeCompletionManager::OnFindUsingNamespaceDone(const wxArrayString& usingNamespace, const wxString& filename)
{
    CL_DEBUG("OnFindUsingNamespaceDone called");

    CL_DEBUG("Found the following 'using namespace' statements for file %s", filename);
    CL_DEBUG_ARR(usingNamespace);

    // We got a list of macros from the parser thead
    // prepare a space delimited list out of it
    std::vector<wxString> additionalScopes;
    additionalScopes.insert(additionalScopes.end(), usingNamespace.begin(), usingNamespace.end());

    LanguageST::Get()->UpdateAdditionalScopesCache(filename, additionalScopes);
}

void CodeCompletionManager::ProcessUsingNamespace(LEditor* editor)
{
    // Sanity
    CHECK_PTR_RET(editor);

    /// disable the editor pre-processor dimming
    EditorDimmerDisabler eds(editor);

    wxArrayString macros;
    wxArrayString includePaths;
    if(!GetDefinitionsAndSearchPaths(editor, includePaths, macros)) return;

    wxUnusedVar(macros);
    // Queue this request in the worker thread
    m_usingNamespaceThread.QueueFile(editor->GetFileName().GetFullPath(), includePaths);
}

bool CodeCompletionManager::GetDefinitionsAndSearchPaths(
    LEditor* editor, wxArrayString& searchPaths, wxArrayString& definitions)
{
    // Sanity
    CHECK_PTR_RET_FALSE(editor);

    if(editor->GetProjectName().IsEmpty()) return false;
    if(!clCxxWorkspaceST::Get()->IsOpen()) return false;

    // Support only C/C++ files
    if(!FileExtManager::IsCxxFile(editor->GetFileName().GetFullName())) return false;

    // Get the file's project and get the build configuration settings
    // for it
    ProjectPtr proj = clCxxWorkspaceST::Get()->GetProject(editor->GetProjectName());
    CHECK_PTR_RET_FALSE(proj);

    BuildConfigPtr buildConf = proj->GetBuildConfiguration();
    CHECK_PTR_RET_FALSE(buildConf);

    CompilerPtr compiler = buildConf->GetCompiler();
    CHECK_PTR_RET_FALSE(compiler);

#if 0
    if(buildConf->IsCustomBuild()) {
        definitions = proj->GetPreProcessors();
        CL_DEBUG("CxxPreProcessor will use the following macros:");
        CL_DEBUG_ARR(definitions);
        // Custom builds are handled differently
        CompilationDatabase compileDb;
        compileDb.Open();
        if(compileDb.IsOpened()) {
            // we have compilation database for this workspace
            wxString compileLine, cwd;
            compileDb.CompilationLine(editor->GetFileName().GetFullPath(), compileLine, cwd);

            CL_DEBUG("Pre Processor dimming: %s\n", compileLine);
            CompilerCommandLineParser cclp(compileLine, cwd);
            searchPaths = cclp.GetIncludes();

            // get the mcros
            definitions << cclp.GetMacros();
        }
    }
#endif
    // get the include paths based on the project settings (this is per build configuration)
    searchPaths = proj->GetIncludePaths();
    CL_DEBUG("CxxPreProcessor will use the following include paths:");
    CL_DEBUG_ARR(searchPaths);

    // get the compiler include paths
    // wxArrayString compileIncludePaths = compiler->GetDefaultIncludePaths();

    // includePaths.insert(includePaths.end(), compileIncludePaths.begin(), compileIncludePaths.end());
    definitions = proj->GetPreProcessors();

    // get macros out of workspace
    wxString strWorkspaceMacros = clCxxWorkspaceST::Get()->GetParserMacros();
    wxArrayString workspaceMacros = wxStringTokenize(strWorkspaceMacros, wxT("\n\r"), wxTOKEN_STRTOK);
    for(size_t i = 0; i < workspaceMacros.GetCount(); i++)
        definitions.Add(workspaceMacros.Item(i).Trim().Trim(false).c_str());

    CL_DEBUG("CxxPreProcessor will use the following macros:");
    CL_DEBUG_ARR(definitions);

    // Append the compiler builtin macros
    wxArrayString builtinMacros = compiler->GetBuiltinMacros();
    definitions.insert(definitions.end(), builtinMacros.begin(), builtinMacros.end());

    return true;
}

void CodeCompletionManager::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    LanguageST::Get()->ClearAdditionalScopesCache();
    Project::ClearBacktickCache();
}

void CodeCompletionManager::OnEnvironmentVariablesModified(clCommandEvent& event)
{
    event.Skip();
    Project::ClearBacktickCache();
    RefreshPreProcessorColouring();
}
