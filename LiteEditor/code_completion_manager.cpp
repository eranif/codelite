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
#include "CxxScannerTokens.h"
#include "CxxVariableScanner.h"
#include "ServiceProviderManager.h"
#include "bitmap_loader.h"
#include "cl_editor.h"
#include "code_completion_api.h"
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

    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &CodeCompletionManager::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_ENVIRONMENT_VARIABLES_MODIFIED,
                               &CodeCompletionManager::OnEnvironmentVariablesModified, this);
    EventNotifier::Get()->Bind(wxEVT_CC_BLOCK_COMMENT_CODE_COMPLETE, &CodeCompletionManager::OnBlockCommentCodeComplete,
                               this);
    EventNotifier::Get()->Bind(wxEVT_CC_BLOCK_COMMENT_WORD_COMPLETE, &CodeCompletionManager::OnBlockCommentWordComplete,
                               this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_ENDED, &CodeCompletionManager::OnBuildEnded, this);
    EventNotifier::Get()->Bind(wxEVT_PROJ_FILE_ADDED, &CodeCompletionManager::OnFilesAdded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &CodeCompletionManager::OnWorkspaceLoaded, this);

    // Start the worker threads
    m_compileCommandsGenerator.reset(new CompileCommandsGenerator());
}

CodeCompletionManager::~CodeCompletionManager()
{
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
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &CodeCompletionManager::OnWorkspaceClosed, this);
    wxTheApp->Unbind(wxEVT_ACTIVATE_APP, &CodeCompletionManager::OnAppActivated, this);
    EventNotifier::Get()->Unbind(wxEVT_ENVIRONMENT_VARIABLES_MODIFIED,
                                 &CodeCompletionManager::OnEnvironmentVariablesModified, this);

    if(m_compileCommandsThread) {
        m_compileCommandsThread->join();
        wxDELETE(m_compileCommandsThread);
    }
}

CodeCompletionManager& CodeCompletionManager::Get()
{
    if(!ms_CodeCompletionManager) {
        ms_CodeCompletionManager = new CodeCompletionManager;
    }
    return *ms_CodeCompletionManager;
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
    clMainFrame::Get()->SetStatusText("Ready");
}

void CodeCompletionManager::OnFileSaved(clCommandEvent& event) { event.Skip(); }

void CodeCompletionManager::OnFileLoaded(clCommandEvent& event) { event.Skip(); }

void CodeCompletionManager::OnWorkspaceConfig(wxCommandEvent& event)
{
    event.Skip();
    if(clCxxWorkspaceST::Get()->IsOpen()) {
        clCxxWorkspaceST::Get()->ClearBacktickCache();
    }

    // Update the compile_flags.txt file
    if(m_compileCommandsGenerator) {
        clDEBUG() << "Workspace configuration changed. Re-Generating compile_flags.txt file";
        m_compileCommandsGenerator->GenerateCompileCommands();
    }
}

void CodeCompletionManager::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    LanguageST::Get()->ClearAdditionalScopesCache();
}

void CodeCompletionManager::OnEnvironmentVariablesModified(clCommandEvent& event)
{
    event.Skip();
    if(clCxxWorkspaceST::Get()->IsOpen()) {
        clCxxWorkspaceST::Get()->ClearBacktickCache();
    }
}

void CodeCompletionManager::DoProcessCompileCommands()
{
    // return; // for now, dont use it
    if(m_compileCommandsThread) {
        return;
    }

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
}

void CodeCompletionManager::OnBlockCommentCodeComplete(clCodeCompletionEvent& event)
{
    event.Skip();
    wxStyledTextCtrl* ctrl = clGetManager()->GetActiveEditor()->GetCtrl();
    CHECK_PTR_RET(ctrl);

    wxCodeCompletionBoxEntry::Vec_t entries;
    if(CreateBlockCommentKeywordsList(entries) == 0) {
        return;
    }
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
    if(CreateBlockCommentKeywordsList(entries) == 0) {
        return;
    }
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

void CodeCompletionManager::OnFilesAdded(clCommandEvent& e)
{
    e.Skip();
    m_compileCommandsGenerator->GenerateCompileCommands();
}

void CodeCompletionManager::OnWorkspaceLoaded(clWorkspaceEvent& e)
{
    e.Skip();
    m_compileCommandsGenerator->GenerateCompileCommands();
}
