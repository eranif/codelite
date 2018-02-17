//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clang_driver.cpp
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

#if HAS_LIBCLANG

#include "asyncprocess.h"
#include "browse_record.h"
#include "clang_code_completion.h"
#include "clang_driver.h"
#include "clangpch_cache.h"
#include "compilation_database.h"
#include "compiler_command_line_parser.h"
#include "configuration_mapping.h"
#include "ctags_manager.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "frame.h"
#include "globals.h"
#include "includepathlocator.h"
#include "localworkspace.h"
#include "macromanager.h"
#include "mainbook.h"
#include "manager.h"
#include "navigationmanager.h"
#include "plugin.h"
#include "pluginmanager.h"
#include "processreaderthread.h"
#include "procutils.h"
#include "project.h"
#include "tags_options_data.h"
#include "wxCodeCompletionBoxManager.h"
#include <algorithm>
#include <memory>
#include <set>
#include <wx/regex.h>
#include <wx/tokenzr.h>

static bool wxIsWhitespace(wxChar ch)
{
    return ch == wxT(' ') || ch == wxT('\t') || ch == wxT('\r') || ch == wxT('\n');
}

#define cstr(x) x.mb_str(wxConvUTF8).data()
#define CHECK_CLANG_ENABLED() \
    if(!(TagsManagerST::Get()->GetCtagsOptions().GetClangOptions() & CC_CLANG_ENABLED)) return;

ClangDriver::ClangDriver()
    : m_isBusy(false)
    , m_activeEditor(NULL)
    , m_position(wxNOT_FOUND)
{
    m_index = clang_createIndex(0, 0);
    m_pchMakerThread.Start();

#ifdef __WXMSW__
    m_clangCleanerThread.Start();
#endif
    EventNotifier::Get()->Connect(wxEVT_CLANG_PCH_CACHE_ENDED, wxCommandEventHandler(ClangDriver::OnPrepareTUEnded),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CLANG_PCH_CACHE_CLEARED, wxCommandEventHandler(ClangDriver::OnCacheCleared),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CLANG_TU_CREATE_ERROR, wxCommandEventHandler(ClangDriver::OnTUCreateError),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(ClangDriver::OnWorkspaceLoaded), NULL,
                                  this);
}

ClangDriver::~ClangDriver()
{
    // Disconnect all events before we perform anything elase
    EventNotifier::Get()->Disconnect(wxEVT_CLANG_PCH_CACHE_ENDED, wxCommandEventHandler(ClangDriver::OnPrepareTUEnded),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CLANG_PCH_CACHE_CLEARED, wxCommandEventHandler(ClangDriver::OnCacheCleared),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CLANG_TU_CREATE_ERROR, wxCommandEventHandler(ClangDriver::OnTUCreateError),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(ClangDriver::OnWorkspaceLoaded),
                                     NULL, this);

    m_pchMakerThread.Stop();
    m_pchMakerThread.ClearCache(); // clear cache and dispose all translation units
#ifdef __WXMSW__
    m_clangCleanerThread.Stop();
#endif
    clang_disposeIndex(m_index);
}

ClangThreadRequest* ClangDriver::DoMakeClangThreadRequest(IEditor* editor, WorkingContext context)
{
    /////////////////////////////////////////////////////////////////
    // Prepare all the buffers required by the thread
    const wxFileName& fnFileName = editor->GetFileName();

    wxFileName tmpFileName(fnFileName);
    tmpFileName.SetFullName(tmpFileName.GetName() + "-clang." + tmpFileName.GetExt());
    tmpFileName.SetPath(clStandardPaths::Get().GetTempDir());

    // Create a temporary file with the same content as our file
    wxString currentBuffer = editor->GetTextRange(0, editor->GetLength());
    wxString filterWord;

    // Copy the content of current editor to the temp file
    if(!FileUtils::WriteFileContent(tmpFileName, currentBuffer)) {
        clWARNING() << "Clang: Unable to create temporary file:" << tmpFileName;
        return nullptr;
    }

    // Move backward until we found our -> or :: or .
    m_position = editor->GetCurrentPosition();
    wxString tmpBuffer = editor->GetTextRange(0, editor->GetCurrentPosition());
    while(!tmpBuffer.IsEmpty()) {
        // Context word complete and we found a whitespace - break the search
        if((context == CTX_WordCompletion || context == CTX_Calltip) && wxIsWhitespace(tmpBuffer.Last())) { break; }
        if(context == CTX_WordCompletion && !IsValidCppIndetifier(tmpBuffer.Last())) { break; }
        if(tmpBuffer.EndsWith(wxT("->")) || tmpBuffer.EndsWith(wxT(".")) || tmpBuffer.EndsWith(wxT("::"))) {
            break;

        } else {
            filterWord.Prepend(tmpBuffer.Last());
            tmpBuffer.RemoveLast();
        }
    }

    // Get the current line's starting pos
    int lineStartPos = editor->PosFromLine(editor->GetCurrentLine());
    int column = editor->GetCurrentPosition() - lineStartPos + 1;
    int lineNumber = editor->GetCurrentLine() + 1;

    if(context == CTX_GotoDecl || context == CTX_GotoImpl) {
        wxString sel = editor->GetSelection();
        if(sel.IsEmpty()) {
            filterWord = editor->GetWordAtCaret();

        } else {
            filterWord = sel;
            column = editor->GetSelectionStart() - lineStartPos + 1;
        }

    } else {
        column -= (int)filterWord.Length();
    }

    // Column can not be lower than 1
    switch(context) {
    case CTX_Calltip:
    case CTX_WordCompletion:
    case CTX_CodeCompletion:
        if(column < 1) {
            CL_DEBUG(wxT("Clang: column can not be lower than 1"));
            m_isBusy = false;
            return NULL;
        }
        break;
    default:
        break;
    }

    wxString projectPath;
    FileTypeCmpArgs_t compileFlags =
        DoPrepareCompilationArgs(editor->GetProjectName(), fnFileName.GetFullPath(), projectPath);
    ClangThreadRequest* request =
        new ClangThreadRequest(m_index, tmpFileName.GetFullPath(), currentBuffer, compileFlags, filterWord, context,
                               lineNumber, column, DoCreateListOfModifiedBuffers(editor));
    request->SetFileName(tmpFileName.GetFullPath());

    // Keep the real file name
    if(m_filesTable.count(tmpFileName.GetFullPath())) { m_filesTable.erase(tmpFileName.GetFullPath()); }
    m_filesTable.insert({ tmpFileName.GetFullPath(), fnFileName.GetFullPath() });
    return request;
}

void ClangDriver::CodeCompletion(IEditor* editor)
{
    if(m_isBusy) { return; }
    clDEBUG() << " ==========> ClangDriver::CodeCompletion() started <==============";

    if(!editor) {
        clWARNING() << "ClangDriver::CodeCompletion() called with NULL editor!";
        return;
    }

    m_activeEditor = editor;
    m_isBusy = true;
    ClangThreadRequest* request = DoMakeClangThreadRequest(m_activeEditor, GetContext());

    // Failed to prepare request?
    if(request == NULL) {
        m_isBusy = false;
        m_activeEditor = NULL;
        return;
    }

    /////////////////////////////////////////////////////////////////
    // Put a request on the parsing thread
    //
    m_pchMakerThread.Add(request);
}

void ClangDriver::Abort() { DoCleanup(); }

FileTypeCmpArgs_t ClangDriver::DoPrepareCompilationArgs(const wxString& projectName, const wxString& sourceFile,
                                                        wxString& projectPath)
{
    FileTypeCmpArgs_t cmpArgs;

    cmpArgs.insert(std::make_pair(FileExtManager::TypeSourceC, wxArrayString()));
    cmpArgs.insert(std::make_pair(FileExtManager::TypeSourceCpp, wxArrayString()));

    wxArrayString args;
    wxString errMsg;

    wxArrayString& cppCompileArgs = cmpArgs[FileExtManager::TypeSourceCpp];
    wxArrayString& cCompileArgs = cmpArgs[FileExtManager::TypeSourceC];

    // Build the TU file name
    wxFileName fnSourceFile(sourceFile);
    CompilationDatabase cdb;
    static bool once = false;
    if(!cdb.IsOk() && !once) {
        once = true;

        wxString msg;
        msg << _("Could not locate compilation database or database version is not up-to-date: ")
            << cdb.GetFileName().GetFullPath() << wxT("\n\n")
            << _("This file should be created automatically for you.\nIf you don't have it, please run a full rebuild "
                 "of your workspace\n\n")
            << _("If this is a custom build project (i.e. project that uses a custom makefile),\nplease set the CXX "
                 "and CC environment variables like this:\n")
            << _("CXX=codelite-cc g++\n") << _("CC=codelite-cc gcc\n\n");

        clMainFrame::Get()->GetMainBook()->ShowMessage(
            msg, true, PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("messages/48/tip")), ButtonDetails(),
            ButtonDetails(), ButtonDetails(), CheckboxDetails(wxT("CodeCompletionMissingCompilationDB")));

    } else {
        cdb.Open();
        if(cdb.IsOpened()) {
            CL_DEBUG(wxT("Loading compilation flags for file: %s"), fnSourceFile.GetFullPath().c_str());
            wxString compilationLine, cwd;
            cdb.CompilationLine(fnSourceFile.GetFullPath(), compilationLine, cwd);
            cdb.Close();

            CompilerCommandLineParser cclp(compilationLine, cwd);
            cclp.MakeAbsolute(cwd);
            CL_DEBUG(wxT("Loaded compilation flags: %s"), compilationLine.c_str());

            args.insert(args.end(), cclp.GetIncludesWithPrefix().begin(), cclp.GetIncludesWithPrefix().end());
            args.insert(args.end(), cclp.GetMacrosWithPrefix().begin(), cclp.GetMacrosWithPrefix().end());
            args.Add(cclp.GetStandardWithPrefix());
        }
    }

    const TagsOptionsData& options = TagsManagerST::Get()->GetCtagsOptions();

    ///////////////////////////////////////////////////////////////////////
    // add global clang include paths
    wxString strGlobalIncludes = options.GetClangSearchPaths();

    // expand any macros from the include paths
    strGlobalIncludes = MacroManager::Instance()->Expand(strGlobalIncludes, PluginManager::Get(), projectName);

    wxArrayString globalIncludes = wxStringTokenize(strGlobalIncludes, wxT("\n\r"), wxTOKEN_STRTOK);
    for(size_t i = 0; i < globalIncludes.GetCount(); i++) {
        wxFileName fn(globalIncludes.Item(i).Trim().Trim(false), wxT(""));
        fn.MakeAbsolute(projectPath);

        cppCompileArgs.Add(wxString::Format(wxT("-I%s"), fn.GetPath().c_str()));
        cCompileArgs.Add(wxString::Format(wxT("-I%s"), fn.GetPath().c_str()));
    }

    ///////////////////////////////////////////////////////////////////////
    // Workspace setting additional flags
    ///////////////////////////////////////////////////////////////////////

    // Include paths
    wxArrayString workspaceIncls, dummy;
    LocalWorkspaceST::Get()->GetParserPaths(workspaceIncls, dummy);
    for(size_t i = 0; i < workspaceIncls.GetCount(); i++) {
        wxFileName fn(workspaceIncls.Item(i).Trim().Trim(false), wxT(""));
        fn.MakeAbsolute(clCxxWorkspaceST::Get()->GetWorkspaceFileName().GetPath());
        cppCompileArgs.Add(wxString::Format(wxT("-I%s"), fn.GetPath().c_str()));
        cCompileArgs.Add(wxString::Format(wxT("-I%s"), fn.GetPath().c_str()));
    }

    // Macros
    wxString strWorkspaceMacros;
    LocalWorkspaceST::Get()->GetParserMacros(strWorkspaceMacros);
    wxArrayString workspaceMacros = wxStringTokenize(strWorkspaceMacros, wxT("\n\r"), wxTOKEN_STRTOK);
    for(size_t i = 0; i < workspaceMacros.GetCount(); i++) {
        cppCompileArgs.Add(wxString::Format(wxT("-D%s"), workspaceMacros.Item(i).Trim().Trim(false).c_str()));
        cCompileArgs.Add(wxString::Format(wxT("-D%s"), workspaceMacros.Item(i).Trim().Trim(false).c_str()));
    }

    // C++ 11 / 14
    size_t workspaceFlags = LocalWorkspaceST::Get()->GetParserFlags();
    if(workspaceFlags & LocalWorkspace::EnableCpp11) {
        cppCompileArgs.Add(wxT("-std=c++11"));
        // cCompileArgs.Add(wxT("-std=c++11"));
    }
    if(workspaceFlags & LocalWorkspace::EnableCpp14) {
        cppCompileArgs.Add(wxT("-std=c++14"));
        // cCompileArgs.Add(wxT("-std=c++14"));
    }
	
	if(workspaceFlags & LocalWorkspace::EnableCpp17) {
        cppCompileArgs.Add(wxT("-std=c++17"));
        // cCompileArgs.Add(wxT("-std=c++14"));
    }

    ///////////////////////////////////////////////////////////////////////
    // Project setting additional flags
    ///////////////////////////////////////////////////////////////////////

    ProjectPtr proj = clCxxWorkspaceST::Get()->GetProject(projectName);
    if(proj) {
        // Append the project include paths (this includes the C/C++ compiler options that may include
        // any "pkg-config" style commands
        wxArrayString compileIncludePaths = proj->GetIncludePaths();
        std::for_each(compileIncludePaths.begin(), compileIncludePaths.end(), [&](const wxString& path) {
            cppCompileArgs.Add(wxString::Format(wxT("-I%s"), path.c_str()));
            cCompileArgs.Add(wxString::Format(wxT("-I%s"), path.c_str()));
        });

        // Append the macros from the build settings
        // TODO :: add here the -U options from the options. Do this for both c and c++
        wxArrayString macros = proj->GetPreProcessors();
        std::for_each(macros.begin(), macros.end(), [&](const wxString& macro) {
            cppCompileArgs.Add(wxString::Format(wxT("-D%s"), macro.c_str()));
            cCompileArgs.Add(wxString::Format(wxT("-D%s"), macro.c_str()));
        });

        wxArrayString cUndefMacros = proj->GetCUnPreProcessors();
        std::for_each(cUndefMacros.begin(), cUndefMacros.end(),
                      [&](const wxString& macro) { cCompileArgs.Add(wxString::Format(wxT("-U%s"), macro.c_str())); });

        wxArrayString cxxUndefMacros = proj->GetCxxUnPreProcessors();
        std::for_each(cxxUndefMacros.begin(), cxxUndefMacros.end(),
                      [&](const wxString& macro) { cppCompileArgs.Add(wxString::Format(wxT("-U%s"), macro.c_str())); });
    }

    BuildConfigPtr buildConf = ManagerST::Get()->GetCurrentBuildConf();
    if(buildConf) {

        // User custom code completion paths
        wxString projSearchPaths = buildConf->GetCcSearchPaths();
        wxArrayString projectIncludePaths = wxStringTokenize(projSearchPaths, wxT("\r\n"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < projectIncludePaths.GetCount(); i++) {
            wxFileName fn(MacroManager::Instance()->Expand(projectIncludePaths.Item(i), PluginManager::Get(),
                                                           ManagerST::Get()->GetActiveProjectName()),
                          wxT(""));
            fn.MakeAbsolute(clCxxWorkspaceST::Get()->GetWorkspaceFileName().GetPath());
            cppCompileArgs.Add(wxString::Format(wxT("-I%s"), fn.GetPath().c_str()));
            cCompileArgs.Add(wxString::Format(wxT("-I%s"), fn.GetPath().c_str()));
        }

        // Use Clang pre-processor flags
        wxString strProjectMacros = buildConf->GetClangPPFlags();
        wxArrayString projectMacros = wxStringTokenize(strProjectMacros, wxT("\n\r"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < projectMacros.GetCount(); i++) {
            cppCompileArgs.Add(wxString::Format(wxT("-D%s"), projectMacros.Item(i).Trim().Trim(false).c_str()));
            cCompileArgs.Add(wxString::Format(wxT("-D%s"), projectMacros.Item(i).Trim().Trim(false).c_str()));
        }

        // Enale C++11?
        if(buildConf->IsClangC11()) { cppCompileArgs.Add(wxT("-std=c++11")); }

        // Enale C++14?
        if(buildConf->IsClangC14()) { cppCompileArgs.Add(wxT("-std=c++14")); }
    }

    cppCompileArgs.insert(cppCompileArgs.end(), args.begin(), args.end());
    cCompileArgs.insert(cCompileArgs.end(), args.begin(), args.end());

    // Remove some of the flags which are known to cause problems to clang
    int where = wxNOT_FOUND;

    where = cppCompileArgs.Index(wxT("-fno-strict-aliasing"));
    if(where != wxNOT_FOUND) cppCompileArgs.RemoveAt(where);

    where = cppCompileArgs.Index(wxT("-mthreads"));
    if(where != wxNOT_FOUND) cppCompileArgs.RemoveAt(where);

    where = cppCompileArgs.Index(wxT("-pipe"));
    if(where != wxNOT_FOUND) cppCompileArgs.RemoveAt(where);

    where = cppCompileArgs.Index(wxT("-fmessage-length=0"));
    if(where != wxNOT_FOUND) cppCompileArgs.RemoveAt(where);

    where = cppCompileArgs.Index(wxT("-fPIC"));
    if(where != wxNOT_FOUND) cppCompileArgs.RemoveAt(where);

    // Now do the same for the "C" arguments
    where = cCompileArgs.Index(wxT("-fno-strict-aliasing"));
    if(where != wxNOT_FOUND) cCompileArgs.RemoveAt(where);

    where = cCompileArgs.Index(wxT("-mthreads"));
    if(where != wxNOT_FOUND) cCompileArgs.RemoveAt(where);

    where = cCompileArgs.Index(wxT("-pipe"));
    if(where != wxNOT_FOUND) cCompileArgs.RemoveAt(where);

    where = cCompileArgs.Index(wxT("-fmessage-length=0"));
    if(where != wxNOT_FOUND) cCompileArgs.RemoveAt(where);

    where = cCompileArgs.Index(wxT("-fPIC"));
    if(where != wxNOT_FOUND) cCompileArgs.RemoveAt(where);

    return cmpArgs;
}

void ClangDriver::ClearCache()
{
    m_pchMakerThread.ClearCache();
    wxLogNull NoLog;
    std::for_each(m_filesTable.begin(), m_filesTable.end(), [&](const wxStringMap_t::value_type& vt) {
        // Delete the temp files (the keys in the cache)
        clDEBUG() << "Clang: deleting temp file:" << vt.first;
        clRemoveFile(vt.first);
    });
}

bool ClangDriver::IsCacheEmpty() { return m_pchMakerThread.IsCacheEmpty(); }

void ClangDriver::DoCleanup()
{
    m_isBusy = false;
    m_activeEditor = NULL;
}

void ClangDriver::DoParseCompletionString(CXCompletionString str, int depth, wxString& entryName, wxString& signature,
                                          wxString& completeString, wxString& returnValue)
{

    bool collectingSignature = false;
    int numOfChunks = clang_getNumCompletionChunks(str);
    for(int j = 0; j < numOfChunks; j++) {

        CXString chunkText = clang_getCompletionChunkText(str, j);
        CXCompletionChunkKind chunkKind = clang_getCompletionChunkKind(str, j);

        switch(chunkKind) {
        case CXCompletionChunk_TypedText:
            entryName = wxString(clang_getCString(chunkText), wxConvUTF8);
            completeString += entryName;
            break;

        case CXCompletionChunk_ResultType:
            completeString += wxString(clang_getCString(chunkText), wxConvUTF8);
            completeString += wxT(" ");
            returnValue = wxString(clang_getCString(chunkText), wxConvUTF8);
            break;

        case CXCompletionChunk_Optional: {
            // Optional argument
            CXCompletionString optStr = clang_getCompletionChunkCompletionString(str, j);
            wxString optionalString;
            wxString dummy;
            // Once we hit the 'Optional Chunk' only the 'completeString' is matter
            DoParseCompletionString(optStr, depth + 1, dummy, dummy, optionalString, dummy);
            if(collectingSignature) { signature += optionalString; }
            completeString += optionalString;
        } break;
        case CXCompletionChunk_LeftParen:
            collectingSignature = true;
            signature += wxT("(");
            completeString += wxT("(");
            break;

        case CXCompletionChunk_RightParen:
            collectingSignature = true;
            signature += wxT(")");
            completeString += wxT(")");
            break;

        default:
            if(collectingSignature) { signature += wxString(clang_getCString(chunkText), wxConvUTF8); }
            completeString += wxString(clang_getCString(chunkText), wxConvUTF8);
            break;
        }
        clang_disposeString(chunkText);
    }

    // To make this tag compatible with ctags one, we need to place
    // a /^ and $/ in the pattern string (we add this only to the top level completionString)
    if(depth == 0) {
        completeString.Prepend(wxT("/^ "));
        completeString.Append(wxT(" $/"));
    }
}

void ClangDriver::OnPrepareTUEnded(wxCommandEvent& e)
{
    // Our thread is done
    m_isBusy = false;

    // Sanity
    ClangThreadReply* reply = (ClangThreadReply*)e.GetClientData();
    if(!reply) { return; }

    // Make sure we delete the reply at the end...
    std::shared_ptr<ClangThreadReply> ap(reply);

    // Delete the fake file...
    DoDeleteTempFile(reply->filename);

    // Replace "filename" with the real file
    if(m_filesTable.count(reply->filename)) {
        wxString tempFileName = reply->filename;
        clDEBUG() << "Clang: setting real file name:" << tempFileName << "->" << m_filesTable[tempFileName];
        reply->filename = m_filesTable[tempFileName];
        m_filesTable.erase(tempFileName);
    }

    // Just a notification without real info?
    if(reply->context == CTX_None) { return; }

    if(reply->context == ::CTX_CachePCH || reply->context == ::CTX_ReparseTU) {
        return; // Nothing more to be done
    }

    if((reply->context == CTX_GotoDecl) || (reply->context == CTX_GotoImpl)) {
        // Unlike other context's the 'filename' specified here
        // does not belong to an editor (it could, but it is not necessarily true)
        DoGotoDefinition(reply);
        return;
    }

    // Adjust the activeEditor to fit the filename
    IEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(reply->filename);
    if(!editor) {
        clDEBUG() << "Could not find an editor for file:" << reply->filename;
        return;
    }

    m_activeEditor = editor;

    // What should we do with the TU?
    switch(reply->context) {
    case CTX_CachePCH:
        // Nothing more to be done
        return;
    default:
        break;
    }

    if(!reply->results) {
        if(!reply->errorMessage.IsEmpty()) {
            // Notify about this error
            clCommandEvent event(wxEVT_CLANG_CODE_COMPLETE_MESSAGE);
            event.SetString(reply->errorMessage);
            event.SetInt(1); // indicates that this is an error message
            EventNotifier::Get()->AddPendingEvent(event);
        }
        return;
    }

    if(m_activeEditor->GetCurrentPosition() < m_position) {
        CL_DEBUG(wxT("Current position is lower than the starting position, ignoring completion"));
        clang_disposeCodeCompleteResults(reply->results);
        return;
    }

    wxString typedString;
    if(m_activeEditor->GetCurrentPosition() > m_position) {
        // User kept on typing while the completion thread was working
        typedString = m_activeEditor->GetTextRange(m_position, m_activeEditor->GetCurrentPosition());
        if(typedString.find_first_not_of(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_")) !=
           wxString::npos) {
            // User typed some non valid identifier char, cancel code completion
            CL_DEBUG(
                wxT("User typed: %s since the completion thread started working until it ended, ignoring completion"),
                typedString.c_str());
            clang_disposeCodeCompleteResults(reply->results);
            return;
        }
    }

    // update the filter word
    reply->filterWord.Append(typedString);
    CL_DEBUG(wxT("clang completion: filter word is %s"), reply->filterWord.c_str());

    // For the Calltip, remove the opening brace from the filter string
    wxString filterWord = reply->filterWord;
    if(GetContext() == CTX_Calltip && filterWord.EndsWith(wxT("("))) filterWord.RemoveLast();

    wxString lowerCaseFilter = filterWord;
    lowerCaseFilter.MakeLower();

    unsigned numResults = reply->results->NumResults;
    clang_sortCodeCompletionResults(reply->results->Results, reply->results->NumResults);
    std::vector<TagEntryPtr> tags;
    for(unsigned i = 0; i < numResults; i++) {
        CXCompletionResult result = reply->results->Results[i];
        CXCompletionString str = result.CompletionString;
        CXCursorKind kind = result.CursorKind;

        if(kind == CXCursor_NotImplemented) continue;

        wxString entryName, entrySignature, entryPattern, entryReturnValue;
        DoParseCompletionString(str, 0, entryName, entrySignature, entryPattern, entryReturnValue);

        wxString lowerCaseName = entryName;
        lowerCaseName.MakeLower();

        if(!lowerCaseFilter.IsEmpty() && !lowerCaseName.StartsWith(lowerCaseFilter)) continue;

        if(clang_getCompletionAvailability(str) != CXAvailability_Available) continue;

        TagEntry* t = new TagEntry();
        TagEntryPtr tag(t);
        tag->SetIsClangTag(true);
        tag->SetName(entryName);
        tag->SetPattern(entryPattern);
        tag->SetSignature(entrySignature);

// Add support for clang comment parsing
#if HAS_LIBCLANG_BRIEFCOMMENTS
        CXString BriefComment = clang_getCompletionBriefComment(str);
        const char* comment = clang_getCString(BriefComment);
        if(comment && comment[0] != '\0') { tag->SetComment(wxString(comment, wxConvUTF8)); }

        clang_disposeString(BriefComment);
#endif

        switch(kind) {
        case CXCursor_EnumConstantDecl:
            tag->SetKind(wxT("enumerator"));
            break;

        case CXCursor_EnumDecl:
            tag->SetKind(wxT("enum"));
            break;

        case CXCursor_CXXMethod:
        case CXCursor_Constructor:
        case CXCursor_Destructor:
        case CXCursor_FunctionDecl:
        case CXCursor_FunctionTemplate:
            tag->SetKind(wxT("prototype"));
            break;

        case CXCursor_MacroDefinition:
            tag->SetKind(wxT("macro"));
            break;

        case CXCursor_Namespace:
            tag->SetKind(wxT("namespace"));
            break;

        case CXCursor_ClassDecl:
        case CXCursor_ClassTemplate:
        case CXCursor_ClassTemplatePartialSpecialization:
            tag->SetKind(wxT("class"));
            break;

        case CXCursor_StructDecl:
            tag->SetKind(wxT("struct"));
            break;

        case CXCursor_TypeRef:
        case CXCursor_TypedefDecl:
            tag->SetKind(wxT("typedef"));
            tag->SetKind(wxT("typedef"));
            break;
        default:
            tag->SetKind(wxT("variable"));
            break;
        }

        tags.push_back(tag);
    }

    clang_disposeCodeCompleteResults(reply->results);

    CL_DEBUG(wxT("Building completion results... done "));
    if(GetContext() == CTX_Calltip) {
        std::vector<TagEntryPtr> tips;
        TagsManagerST::Get()->GetFunctionTipFromTags(tags, filterWord, tips);
        m_activeEditor->ShowCalltip(new clCallTip(tips));

    } else {
        wxCodeCompletionBoxManager::Get().ShowCompletionBox(m_activeEditor->GetCtrl(), tags, wxCodeCompletionBox::kNone,
                                                            wxNOT_FOUND);
    }
}

void ClangDriver::QueueRequest(IEditor* editor, WorkingContext context)
{
    if(!editor) return;

    switch(context) {
    case CTX_CachePCH:
        break;
    default:
        CL_DEBUG(wxT("Context %d id not allowed to be queued"), (int)context);
        return;
    }
    m_pchMakerThread.Add(DoMakeClangThreadRequest(editor, context));
}

void ClangDriver::ReparseFile(const wxString& filename) { wxUnusedVar(filename); }

void ClangDriver::OnCacheCleared(wxCommandEvent& e) { e.Skip(); }

void ClangDriver::DoGotoDefinition(ClangThreadReply* reply)
{
    CHECK_CLANG_ENABLED();
    LEditor* editor = clMainFrame::Get()->GetMainBook()->OpenFile(reply->filename, wxEmptyString, reply->line);
    if(editor) {
        int pos = editor->PositionFromLine(reply->line - 1);
        editor->FindAndSelectV(reply->filterWord, reply->filterWord, pos, NULL);
    }
}

void ClangDriver::OnTUCreateError(wxCommandEvent& e)
{
    e.Skip();
    ClangThreadReply* reply = reinterpret_cast<ClangThreadReply*>(e.GetClientData());
    if(reply) {
        DoDeleteTempFile(reply->filename);
        wxDELETE(reply);
    }
    DoCleanup();
}

void ClangDriver::OnWorkspaceLoaded(wxCommandEvent& event) { event.Skip(); }

ClangThreadRequest::List_t ClangDriver::DoCreateListOfModifiedBuffers(IEditor* excludeEditor)
{
    // Collect all modified buffers and pass them to clang as well
    ClangThreadRequest::List_t modifiedBuffers;
    std::vector<LEditor*> editors;
    clMainFrame::Get()->GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        if(editors.at(i) == excludeEditor || !editors.at(i)->IsModified()) { continue; }
        modifiedBuffers.push_back({ editors.at(i)->GetFileName().GetFullPath(), editors.at(i)->GetText() });
    }
    return modifiedBuffers;
}

void ClangDriver::DoDeleteTempFile(const wxString& fileName) { wxUnusedVar(fileName); }

#endif // HAS_LIBCLANG
