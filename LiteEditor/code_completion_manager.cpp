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

#include "ServiceProviderManager.h"
#include "bitmap_loader.h"
#include "cl_editor.h"
#include "code_completion_api.h"
#include "code_completion_manager.h"
#include "compilation_database.h"
#include "compiler_command_line_parser.h"
#include "ctags_manager.h"
#include "entry.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "frame.h"
#include "globals.h"
#include "imanager.h"
#include "language.h"
#include "manager.h"
#include "parse_thread.h"
#include "plugin.h"
#include "tags_options_data.h"
#include "wxCodeCompletionBox.h"
#include "wxCodeCompletionBoxManager.h"
#include <algorithm>
#include <vector>

static CodeCompletionManager* ms_CodeCompletionManager = NULL;

// Helper class
struct EditorDimmerDisabler {
    clEditor* m_editor;
    EditorDimmerDisabler(clEditor* editor)
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
    : ServiceProvider("BuiltIn C++ Code Completion", eServiceType::kCodeCompletion)
    , m_options(CC_CTAGS_ENABLED)
    , m_wordCompletionRefreshNeeded(false)
    , m_buildInProgress(false)
{
    SetPriority(75);
    EventNotifier::Get()->Connect(wxEVT_BUILD_STARTED, clBuildEventHandler(CodeCompletionManager::OnBuildStarted), NULL,
                                  this);
    EventNotifier::Get()->Bind(wxEVT_COMPILE_COMMANDS_JSON_GENERATED,
                               &CodeCompletionManager::OnCompileCommandsFileGenerated, this);

    EventNotifier::Get()->Connect(wxEVT_FILE_SAVED, clCommandEventHandler(CodeCompletionManager::OnFileSaved), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_FILE_LOADED, clCommandEventHandler(CodeCompletionManager::OnFileLoaded), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CONFIG_CHANGED,
                                  wxCommandEventHandler(CodeCompletionManager::OnWorkspaceConfig), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CMD_PROJ_SETTINGS_SAVED,
                                  wxCommandEventHandler(CodeCompletionManager::OnWorkspaceConfig), NULL, this);
    wxTheApp->Bind(wxEVT_ACTIVATE_APP, &CodeCompletionManager::OnAppActivated, this);

    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED,
                                  wxCommandEventHandler(CodeCompletionManager::OnWorkspaceClosed), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_ENVIRONMENT_VARIABLES_MODIFIED,
                               &CodeCompletionManager::OnEnvironmentVariablesModified, this);
    EventNotifier::Get()->Bind(wxEVT_CC_BLOCK_COMMENT_CODE_COMPLETE, &CodeCompletionManager::OnBlockCommentCodeComplete,
                               this);
    EventNotifier::Get()->Bind(wxEVT_CC_BLOCK_COMMENT_WORD_COMPLETE, &CodeCompletionManager::OnBlockCommentWordComplete,
                               this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_ENDED, &CodeCompletionManager::OnBuildEnded, this);
    EventNotifier::Get()->Bind(wxEVT_PROJ_FILE_ADDED, &CodeCompletionManager::OnFilesAdded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &CodeCompletionManager::OnWorkspaceLoaded, this);

    // Connect ourself to the cc event system
    Bind(wxEVT_CC_CODE_COMPLETE, &CodeCompletionManager::OnCodeCompletion, this);
    Bind(wxEVT_CC_FIND_SYMBOL, &CodeCompletionManager::OnFindSymbol, this);
    Bind(wxEVT_CC_FIND_SYMBOL_DECLARATION, &CodeCompletionManager::OnFindDecl, this);
    Bind(wxEVT_CC_FIND_SYMBOL_DEFINITION, &CodeCompletionManager::OnFindImpl, this);
    Bind(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &CodeCompletionManager::OnFunctionCalltip, this);
    Bind(wxEVT_CC_TYPEINFO_TIP, &CodeCompletionManager::OnTypeInfoToolTip, this);

    // Start the worker threads
    m_preProcessorThread.Start();
    m_usingNamespaceThread.Start();
    m_compileCommandsGenerator.reset(new CompileCommandsGenerator());
}

CodeCompletionManager::~CodeCompletionManager()
{
    m_preProcessorThread.Stop();
    m_usingNamespaceThread.Stop();
    EventNotifier::Get()->Unbind(wxEVT_PROJ_FILE_ADDED, &CodeCompletionManager::OnFilesAdded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &CodeCompletionManager::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_BLOCK_COMMENT_CODE_COMPLETE,
                                 &CodeCompletionManager::OnBlockCommentCodeComplete, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_BLOCK_COMMENT_WORD_COMPLETE,
                                 &CodeCompletionManager::OnBlockCommentWordComplete, this);

    EventNotifier::Get()->Unbind(wxEVT_BUILD_ENDED, &CodeCompletionManager::OnBuildEnded, this);
    EventNotifier::Get()->Disconnect(wxEVT_BUILD_STARTED, clBuildEventHandler(CodeCompletionManager::OnBuildStarted),
                                     NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_COMPILE_COMMANDS_JSON_GENERATED,
                                 &CodeCompletionManager::OnCompileCommandsFileGenerated, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED, clCommandEventHandler(CodeCompletionManager::OnFileSaved), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_LOADED, clCommandEventHandler(CodeCompletionManager::OnFileLoaded),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CONFIG_CHANGED,
                                     wxCommandEventHandler(CodeCompletionManager::OnWorkspaceConfig), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_PROJ_SETTINGS_SAVED,
                                     wxCommandEventHandler(CodeCompletionManager::OnWorkspaceConfig), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED,
                                     wxCommandEventHandler(CodeCompletionManager::OnWorkspaceClosed), NULL, this);
    wxTheApp->Unbind(wxEVT_ACTIVATE_APP, &CodeCompletionManager::OnAppActivated, this);
    EventNotifier::Get()->Unbind(wxEVT_ENVIRONMENT_VARIABLES_MODIFIED,
                                 &CodeCompletionManager::OnEnvironmentVariablesModified, this);

    Unbind(wxEVT_CC_CODE_COMPLETE, &CodeCompletionManager::OnCodeCompletion, this);
    Unbind(wxEVT_CC_FIND_SYMBOL, &CodeCompletionManager::OnFindSymbol, this);
    Unbind(wxEVT_CC_FIND_SYMBOL_DECLARATION, &CodeCompletionManager::OnFindDecl, this);
    Unbind(wxEVT_CC_FIND_SYMBOL_DEFINITION, &CodeCompletionManager::OnFindImpl, this);
    Unbind(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &CodeCompletionManager::OnFunctionCalltip, this);
    Unbind(wxEVT_CC_TYPEINFO_TIP, &CodeCompletionManager::OnTypeInfoToolTip, this);

    if(m_compileCommandsThread) {
        m_compileCommandsThread->join();
        wxDELETE(m_compileCommandsThread);
    }
}

bool CodeCompletionManager::WordCompletion(clEditor* editor, const wxString& expr, const wxString& word)
{
    wxString expression = expr;
    wxString tmp;

    DoUpdateOptions();

    // Trim whitespace from right and left
    static wxString trimString(wxT("!<>=(){};\r\n\t\v "));

    expression = expression.erase(0, expression.find_first_not_of(trimString));
    expression = expression.erase(expression.find_last_not_of(trimString) + 1);

    if(expression.EndsWith(word, &tmp)) { expression = tmp; }
    return DoCtagsWordCompletion(editor, expr, word);
}

CodeCompletionManager& CodeCompletionManager::Get()
{
    if(!ms_CodeCompletionManager) { ms_CodeCompletionManager = new CodeCompletionManager; }
    return *ms_CodeCompletionManager;
}

bool CodeCompletionManager::DoCtagsWordCompletion(clEditor* editor, const wxString& expr, const wxString& word)
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

bool CodeCompletionManager::DoCtagsCalltip(clEditor* editor, int line, const wxString& expr, const wxString& text,
                                           const wxString& word)
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

bool CodeCompletionManager::Calltip(clEditor* editor, int line, const wxString& expr, const wxString& text,
                                    const wxString& word)
{
    DoUpdateOptions();
    if(::IsCppKeyword(word)) return false;
    return DoCtagsCalltip(editor, line, expr, text, word);
}

bool CodeCompletionManager::CodeComplete(clEditor* editor, int line, const wxString& expr, const wxString& text)
{
    DoUpdateOptions();
    return DoCtagsCodeComplete(editor, line, expr, text);
}

bool CodeCompletionManager::DoCtagsCodeComplete(clEditor* editor, int line, const wxString& expr, const wxString& text)
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
    if((m_options & CC_CLANG_FIRST) && (m_options & CC_CLANG_ENABLED)) { m_options &= ~CC_CTAGS_ENABLED; }
}

void CodeCompletionManager::ProcessMacros(clEditor* editor)
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

void CodeCompletionManager::GotoImpl(clEditor* editor)
{
    DoUpdateOptions();

    // Let the plugins handle this first
    clCodeCompletionEvent event(wxEVT_CC_FIND_SYMBOL_DEFINITION);
    event.SetEditor(editor);
    ServiceProviderManager::Get().ProcessEvent(event);
}

bool CodeCompletionManager::DoCtagsGotoImpl(clEditor* editor)
{
    TagEntryPtr tag = editor->GetContext()->GetTagAtCaret(true, true);
    if(tag) {
        clEditor* editor =
            clMainFrame::Get()->GetMainBook()->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine() - 1);
        if(!editor) { return false; }
        // Use the async funtion here. Synchronously usually works but, if the file wasn't loaded, sometimes the
        // EnsureVisible code is called too early and fails
        editor->FindAndSelectV(tag->GetPattern(), tag->GetName(), editor->PosFromLine(tag->GetLine() - 1));
        return true;
    }
    return false;
}

bool CodeCompletionManager::DoCtagsGotoDecl(clEditor* editor)
{
    TagEntryPtr tag = editor->GetContext()->GetTagAtCaret(true, false);
    if(tag) {
        clEditor* editor =
            clMainFrame::Get()->GetMainBook()->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine() - 1);
        if(!editor) { return false; }
        // Use the async funtion here. Synchronously usually works but, if the file wasn't loaded, sometimes the
        // EnsureVisible code is called too early and fails
        if(!editor->FindAndSelect(tag->GetPattern(), tag->GetName(), editor->PosFromLine(tag->GetLine() - 1),
                                  NavMgr::Get())) {
            editor->SetCaretAt(editor->PosFromLine(tag->GetLine() - 1));
            editor->CenterLineIfNeeded(tag->GetLine() - 1);
        }
        return true;
    }
    return false;
}

void CodeCompletionManager::GotoDecl(clEditor* editor)
{
    DoUpdateOptions();
    clCodeCompletionEvent event(wxEVT_CC_FIND_SYMBOL_DECLARATION);
    event.SetEditor(editor);
    ServiceProviderManager::Get().ProcessEvent(event);
}

void CodeCompletionManager::OnBuildEnded(clBuildEvent& e)
{
    e.Skip();
    m_compileCommandsGenerator->GenerateCompileCommands();
    m_buildInProgress = false;
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
    clDEBUG() << "-- Code Completion Manager: process file" << event.GetFileName();
    this->CompileCommandsFileProcessed(event.GetStrings());
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
    clEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(filename);
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
    clEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(event.GetFileName());
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

    if(editor && (ccFlags & CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING)) { ProcessUsingNamespace(editor); }
}

void CodeCompletionManager::RefreshPreProcessorColouring()
{
    bool enableBlockColouring = TagsManagerST::Get()->GetCtagsOptions().GetCcColourFlags() & CC_COLOUR_MACRO_BLOCKS;
    clEditor::Vec_t editors;
    clMainFrame::Get()->GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    if(!enableBlockColouring) {
        for(size_t i = 0; i < editors.size(); ++i) {
            clEditor* editor = editors.at(i);
            editor->SetPreProcessorsWords("");
            editor->SetProperty("lexer.cpp.track.preprocessor", "0");
            editor->SetProperty("lexer.cpp.update.preprocessor", "0");
        }
    } else {
        for(size_t i = 0; i < editors.size(); ++i) {
            clEditor* editor = editors.at(i);
            editor->SetProperty("lexer.cpp.track.preprocessor", "0");
            editor->SetProperty("lexer.cpp.update.preprocessor", "0");
            ProcessMacros(editor);
        }
    }
}

void CodeCompletionManager::OnWorkspaceConfig(wxCommandEvent& event)
{
    event.Skip();
    if(clCxxWorkspaceST::Get()->IsOpen()) { clCxxWorkspaceST::Get()->ClearBacktickCache(); }
    RefreshPreProcessorColouring();
    // Update the compile_flags.txt file
    if(m_compileCommandsGenerator) { 
        clDEBUG() << "Workspace configuration changed. Re-Generating compile_flags.txt file";
        m_compileCommandsGenerator->GenerateCompileCommands(); 
    }
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

void CodeCompletionManager::ProcessUsingNamespace(clEditor* editor)
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

bool CodeCompletionManager::GetDefinitionsAndSearchPaths(clEditor* editor, wxArrayString& searchPaths,
                                                         wxArrayString& definitions)
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
}

void CodeCompletionManager::OnEnvironmentVariablesModified(clCommandEvent& event)
{
    event.Skip();
    if(clCxxWorkspaceST::Get()->IsOpen()) { clCxxWorkspaceST::Get()->ClearBacktickCache(); }
    RefreshPreProcessorColouring();
}

void CodeCompletionManager::DoProcessCompileCommands()
{
    // return; // for now, dont use it
    if(m_compileCommandsThread) { return; }

    // Create a thread that will process the current workspace folder and search for any compile_commands.json file
    m_compileCommandsThread = new std::thread(&CodeCompletionManager::ThreadProcessCompileCommandsEntry, this,
                                              clCxxWorkspaceST::Get()->GetFileName().GetPath());
}

void CodeCompletionManager::ThreadProcessCompileCommandsEntry(CodeCompletionManager* owner, const wxString& rootFolder)
{
    // Search for compile_commands file, process it and send back the results to the main thread
    wxArrayString includePaths = CompilationDatabase::FindIncludePaths(rootFolder, owner->m_compileCommands,
                                                                       owner->m_compileCommandsLastModified);
    owner->CallAfter(&CodeCompletionManager::CompileCommandsFileProcessed, includePaths);
}

void CodeCompletionManager::CompileCommandsFileProcessed(const wxArrayString& includePaths)
{
    if(m_compileCommandsThread) {
        m_compileCommandsThread->join();
        wxDELETE(m_compileCommandsThread);
    }
    if(includePaths.IsEmpty()) { return; }
    ParseThreadST::Get()->AddPaths(includePaths, {});

    // Update the parser search paths
    wxArrayString inc, exc;
    ParseThreadST::Get()->GetSearchPaths(inc, exc);
    clDEBUG() << "Parser thread search paths are now updated to:" << inc;

    // Trigger a quick parse
    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(event);
}

void CodeCompletionManager::OnBlockCommentCodeComplete(clCodeCompletionEvent& event)
{
    event.Skip();
    wxStyledTextCtrl* ctrl = clGetManager()->GetActiveEditor()->GetCtrl();
    CHECK_PTR_RET(ctrl);

    wxCodeCompletionBoxEntry::Vec_t entries;
    if(CreateBlockCommentKeywordsList(entries) == 0) { return; }
    wxCodeCompletionBox::BmpVec_t bitmaps;
    bitmaps.push_back(clGetManager()->GetStdIcons()->LoadBitmap("cpp_keyword"));

    int startPos = ctrl->WordStartPosition(ctrl->GetCurrentPos(), true);
    wxCodeCompletionBoxManager::Get().ShowCompletionBox(ctrl, entries, bitmaps, wxCodeCompletionBox::kRefreshOnKeyType,
                                                        startPos);
}

void CodeCompletionManager::OnBlockCommentWordComplete(clCodeCompletionEvent& event)
{
    event.Skip();
    wxStyledTextCtrl* ctrl = clGetManager()->GetActiveEditor()->GetCtrl();
    CHECK_PTR_RET(ctrl);

    wxCodeCompletionBoxEntry::Vec_t entries;
    if(CreateBlockCommentKeywordsList(entries) == 0) { return; }
    wxCodeCompletionBox::BmpVec_t bitmaps;
    bitmaps.push_back(clGetManager()->GetStdIcons()->LoadBitmap("cpp_keyword"));

    int startPos = ctrl->WordStartPosition(ctrl->GetCurrentPos(), true);
    wxCodeCompletionBoxManager::Get().ShowCompletionBox(ctrl, entries, bitmaps, wxCodeCompletionBox::kRefreshOnKeyType,
                                                        startPos);
}

size_t CodeCompletionManager::CreateBlockCommentKeywordsList(wxCodeCompletionBoxEntry::Vec_t& entries) const
{
    entries.clear();
    std::vector<wxString> keywords = { "api",        "author",   "brief",         "category",       "copyright",
                                       "deprecated", "example",  "filesource",    "global",         "ignore",
                                       "internal",   "license",  "link",          "method",         "package",
                                       "param",      "property", "property-read", "property-write", "return",
                                       "see",        "since",    "source",        "subpackage",     "throws",
                                       "todo",       "uses",     "var",           "version" };
    std::for_each(keywords.begin(), keywords.end(),
                  [&](const wxString& keyword) { entries.push_back(wxCodeCompletionBoxEntry::New(keyword, 0)); });
    return entries.size();
}

void CodeCompletionManager::UpdateParserPaths()
{
    if(clCxxWorkspaceST::Get()->IsOpen()) { DoProcessCompileCommands(); }
}

void CodeCompletionManager::OnFilesAdded(clCommandEvent& e)
{
    e.Skip();
    m_compileCommandsGenerator->GenerateCompileCommands();
}

void CodeCompletionManager::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
    m_compileCommandsGenerator->GenerateCompileCommands();
}

void CodeCompletionManager::OnCodeCompletion(clCodeCompletionEvent& event)
{
    event.Skip();
    clEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor(true);
    CHECK_PTR_RET(editor);

    // This class only handles C++/C code completion
    if(!FileExtManager::IsCxxFile(editor->GetFileName())) { return; }

    // Try to code complete
    bool completionSucceed = event.GetTriggerKind() == LSP::CompletionItem::kTriggerCharacter
                                 ? editor->GetContext()->CodeComplete()
                                 : editor->GetContext()->CompleteWord();

    // Skip the event if we managed to process
    event.Skip(!completionSucceed);
}

void CodeCompletionManager::OnFindSymbol(clCodeCompletionEvent& event)
{
    event.Skip();
    clEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor(true);
    CHECK_PTR_RET(editor);

    // This class only handles C++/C code completion
    if(!FileExtManager::IsCxxFile(editor->GetFileName())) { return; }

    // Try to code complete
    bool completionSucceed = editor->GetContext()->GotoDefinition();

    // Skip the event if we managed to process
    event.Skip(!completionSucceed);
}

void CodeCompletionManager::OnWordCompletion(clCodeCompletionEvent& event)
{
    event.Skip();
    clEditor* editor = dynamic_cast<clEditor*>(event.GetEditor());
    CHECK_PTR_RET(editor);

    // This class only handles C++/C code completion
    if(!FileExtManager::IsCxxFile(editor->GetFileName())) { return; }

    // Dont attempt to code complete when inside comment blocks
    if(editor->GetContext()->IsCommentOrString(editor->GetCurrentPos())) {
        event.Skip(false);
        return;
    }

    // Try to code complete
    bool completionSucceed = editor->GetContext()->CompleteWord();

    // Skip the event if we managed to process
    event.Skip(!completionSucceed);
}

void CodeCompletionManager::OnFindDecl(clCodeCompletionEvent& event)
{
    event.Skip();
    clEditor* editor = dynamic_cast<clEditor*>(event.GetEditor());
    bool res = editor && FileExtManager::IsCxxFile(editor->GetFileName()) && DoCtagsGotoDecl(editor);
    event.Skip(!res);
}
void CodeCompletionManager::OnFindImpl(clCodeCompletionEvent& event)
{
    event.Skip();
    clEditor* editor = dynamic_cast<clEditor*>(event.GetEditor());
    bool res = editor && FileExtManager::IsCxxFile(editor->GetFileName()) && DoCtagsGotoImpl(editor);
    event.Skip(!res);
}

void CodeCompletionManager::OnFunctionCalltip(clCodeCompletionEvent& event)
{
    event.Skip();
    clEditor* editor = dynamic_cast<clEditor*>(event.GetEditor());
    bool res = editor && FileExtManager::IsCxxFile(editor->GetFileName()) &&
               editor->GetContext()->CodeComplete(event.GetPosition());
    event.Skip(!res);
}

void CodeCompletionManager::OnTypeInfoToolTip(clCodeCompletionEvent& event)
{
    event.Skip();
    clEditor* editor = dynamic_cast<clEditor*>(event.GetEditor());
    bool res = editor && FileExtManager::IsCxxFile(editor->GetFileName()) &&
               editor->GetContext()->GetHoverTip(event.GetPosition());
    event.Skip(!res);
}
