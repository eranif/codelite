//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clang_pch_maker_thread.cpp
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

#include "clang_pch_maker_thread.h"
#include "clang_unsaved_files.h"
#include "clang_utils.h"
#include "codelite_events.h"
#include "cpp_scanner.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "globals.h"
#include "procutils.h"
#include "y.tab.h"
#include <wx/app.h>
#include <wx/regex.h>
#include <wx/stdpaths.h>
#include <wx/thread.h>
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>

#define cstr(x) x.mb_str(wxConvUTF8).data()

const wxEventType wxEVT_CLANG_PCH_CACHE_STARTED = XRCID("clang_pch_cache_started");
const wxEventType wxEVT_CLANG_PCH_CACHE_ENDED = XRCID("clang_pch_cache_ended");
const wxEventType wxEVT_CLANG_PCH_CACHE_CLEARED = XRCID("clang_pch_cache_cleared");
const wxEventType wxEVT_CLANG_TU_CREATE_ERROR = XRCID("clang_pch_create_error");

ClangWorkerThread::ClangWorkerThread() { clang_toggleCrashRecovery(1); }

ClangWorkerThread::~ClangWorkerThread() {}

void ClangWorkerThread::ProcessRequest(ThreadRequest* request)
{
    // Send start event
    PostEvent(wxEVT_CLANG_PCH_CACHE_STARTED, "");

    ClangThreadRequest* task = dynamic_cast<ClangThreadRequest*>(request);
    wxASSERT_MSG(task, "ClangWorkerThread: NULL task");

    {
        // A bit of optimization
        wxCriticalSectionLocker locker(m_criticalSection);
        if(task->GetContext() == CTX_CachePCH && m_cache.Contains(task->GetFileName())) {
            // Nothing to be done here
            PostEvent(wxEVT_CLANG_PCH_CACHE_ENDED, task->GetFileName());
            return;
        }
    }

    CL_DEBUG(wxT("==========> [ ClangPchMakerThread ] ProcessRequest started: %s"), task->GetFileName().c_str());
    CL_DEBUG(wxT("ClangWorkerThread:: processing request %d"), (int)task->GetContext());

    ClangCacheEntry cacheEntry = findEntry(task->GetFileName());
    CXTranslationUnit TU = cacheEntry.TU;
    CL_DEBUG(wxT("ClangWorkerThread:: found cached TU: %p"), (void*)TU);

    bool reparseRequired = true;
    if(!TU) {

        // First time creating the TU
        TU = DoCreateTU(task->GetIndex(), task, true);
        reparseRequired = false;
        cacheEntry.lastReparse = time(NULL);
        cacheEntry.TU = TU;
        cacheEntry.sourceFile = task->GetFileName();
    }

    if(!TU) {
        CL_DEBUG(wxT("Failed to parse Translation UNIT..."));
        PostEvent(wxEVT_CLANG_TU_CREATE_ERROR, task->GetFileName());
        return;
    }

    if(reparseRequired && task->GetContext() == ::CTX_ReparseTU) {
        DoSetStatusMsg(wxString::Format(wxT("clang: re-parsing file %s..."), task->GetFileName().c_str()));

        // We need to reparse the TU
        CL_DEBUG(wxT("Calling clang_reparseTranslationUnit... [CTX_ReparseTU]"));
        if(clang_reparseTranslationUnit(TU, 0, NULL, clang_defaultReparseOptions(TU)) == 0) {
            CL_DEBUG(wxT("Calling clang_reparseTranslationUnit... done [CTX_ReparseTU]"));
            cacheEntry.lastReparse = time(NULL);

        } else {

            CL_DEBUG(wxT("An error occurred during reparsing of the TU for file %s. TU: %p"),
                     task->GetFileName().c_str(), (void*)TU);

            // The only thing that left to be done here, is to dispose the TU
            clang_disposeTranslationUnit(TU);
            PostEvent(wxEVT_CLANG_TU_CREATE_ERROR, task->GetFileName());

            return;
        }
    }

    // Construct a cache-returner class
    // which makes sure that the TU is cached
    // when we leave the current scope
    CacheReturner cr(this, cacheEntry);

    // Prepare the 'End' event
    wxCommandEvent eEnd(wxEVT_CLANG_PCH_CACHE_ENDED);
    ClangThreadReply* reply = new ClangThreadReply;
    reply->context = task->GetContext();
    reply->filterWord = task->GetFilterWord();
    reply->filename = task->GetFileName().c_str();
    reply->results = NULL;

    wxFileName realFileName(reply->filename);
    if(realFileName.GetFullName().StartsWith(CODELITE_CLANG_FILE_PREFIX)) {
        realFileName.SetFullName(realFileName.GetFullName().Mid(strlen(CODELITE_CLANG_FILE_PREFIX)));
    }
    reply->filename = realFileName.GetFullPath();

    if(task->GetContext() == CTX_CodeCompletion || task->GetContext() == CTX_WordCompletion ||
       task->GetContext() == CTX_Calltip) {
        CL_DEBUG(wxT("Calling clang_codeCompleteAt..."));

        ClangThreadRequest::List_t usList = task->GetModifiedBuffers();
        usList.push_back(std::make_pair(task->GetFileName(), task->GetDirtyBuffer()));
        ClangUnsavedFiles usf(usList);

        CL_DEBUG(wxT("Location: %s:%u:%u"), task->GetFileName().c_str(), task->GetLine(), task->GetColumn());
        reply->results = clang_codeCompleteAt(TU, cstr(task->GetFileName()), task->GetLine(), task->GetColumn(),
                                              usf.GetUnsavedFiles(), usf.GetCount(),
                                              clang_defaultCodeCompleteOptions()
#if HAS_LIBCLANG_BRIEFCOMMENTS
                                                  | CXCodeComplete_IncludeBriefComments
#endif
        );

        cacheEntry.lastReparse = time(NULL);

        CL_DEBUG(wxT("Calling clang_codeCompleteAt... done"));
        wxString displayTip;
        bool hasErrors(false);
        if(reply->results) {
            unsigned maxErrorToDisplay = 10;
            std::set<wxString> errorMessages;
            unsigned errorCount = clang_codeCompleteGetNumDiagnostics(reply->results);
            // Collect all errors / fatal errors and report them back to user
            for(unsigned i = 0; i < errorCount; i++) {
                CXDiagnostic diag = clang_codeCompleteGetDiagnostic(reply->results, i);
                CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diag);
                if(!hasErrors) { hasErrors = (severity == CXDiagnostic_Error || severity == CXDiagnostic_Fatal); }

                if(severity == CXDiagnostic_Error || severity == CXDiagnostic_Fatal || severity == CXDiagnostic_Note) {
                    CXString diagStr = clang_getDiagnosticSpelling(diag);
                    wxString wxDiagString = wxString(clang_getCString(diagStr), wxConvUTF8);

                    // Collect up to 10 error messages
                    // and dont collect the same error twice
                    if(errorMessages.find(wxDiagString) == errorMessages.end() &&
                       errorMessages.size() <= maxErrorToDisplay) {
                        errorMessages.insert(wxDiagString);
                        displayTip << wxDiagString.c_str() << wxT("\n");
                    }

                    clang_disposeString(diagStr);
                }
                clang_disposeDiagnostic(diag);
            }

            CL_DEBUG(wxT("Found %u matches"), reply->results->NumResults);
            ClangUtils::printCompletionDiagnostics(reply->results);
        }

        if(!displayTip.IsEmpty() && hasErrors) {
            // Send back the error messages
            reply->errorMessage << "clang: " << displayTip;
            reply->errorMessage.RemoveLast();

            // Free the results
            clang_disposeCodeCompleteResults(reply->results);
            reply->results = NULL;
        }

        // Send the event
        eEnd.SetClientData(reply);
        EventNotifier::Get()->AddPendingEvent(eEnd);

    } else if(task->GetContext() == CTX_GotoDecl || task->GetContext() == CTX_GotoImpl) {

        // Check to see if the file was modified since it was last reparsed
        // If it does, we need to re-parse it again
        wxFileName fnSource(cacheEntry.sourceFile);
        time_t fileModificationTime = fnSource.GetModificationTime().GetTicks();
        time_t lastReparseTime = cacheEntry.lastReparse;

        if(fileModificationTime > lastReparseTime) {

            // The file needs to be re-parsed
            DoSetStatusMsg(wxString::Format(wxT("clang: re-parsing file %s...\n"), cacheEntry.sourceFile));

            // Try reparsing the TU
            ClangThreadRequest::List_t usList = task->GetModifiedBuffers();
            usList.push_back(std::make_pair(task->GetFileName(), task->GetDirtyBuffer()));
            ClangUnsavedFiles usf(usList);

            if(clang_reparseTranslationUnit(TU, usf.GetCount(), usf.GetUnsavedFiles(),
                                            clang_defaultReparseOptions(TU)) != 0) {
                // Failed to reparse
                cr.SetCancelled(true); // cancel the re-caching of the TU

                DoSetStatusMsg(
                    wxString::Format("clang: clang_reparseTranslationUnit '%s' failed\n", cacheEntry.sourceFile));

                clang_disposeTranslationUnit(TU);
                wxDELETE(reply);
                PostEvent(wxEVT_CLANG_TU_CREATE_ERROR, task->GetFileName());
                return;
            }

            DoSetStatusMsg(
                wxString::Format("clang: clang_reparseTranslationUnit '%s' - done\n", cacheEntry.sourceFile));
            // Update the 'lastReparse' field
            cacheEntry.lastReparse = time(NULL);
        }

        bool success = DoGotoDefinition(TU, task, reply);
        if(success) {
            eEnd.SetClientData(reply);
            EventNotifier::Get()->AddPendingEvent(eEnd);

        } else {
            DoSetStatusMsg(wxT("clang: no matches were found"));
            CL_DEBUG(wxT("Clang Goto Decl/Impl: could not find a cursor matching for position %s:%d:%d"),
                     task->GetFileName().c_str(), (int)task->GetLine(), (int)task->GetColumn());

            // Failed, delete the 'reply' allocatd earlier
            wxDELETE(reply);
            PostEvent(wxEVT_CLANG_TU_CREATE_ERROR, task->GetFileName());
        }
    } else {

        wxDELETE(reply);
        PostEvent(wxEVT_CLANG_PCH_CACHE_ENDED, task->GetFileName());
    }
}

ClangCacheEntry ClangWorkerThread::findEntry(const wxString& filename)
{
    wxCriticalSectionLocker locker(m_criticalSection);
    ClangCacheEntry entry = m_cache.GetPCH(filename);
    return entry;
}

void ClangWorkerThread::DoCacheResult(ClangCacheEntry entry)
{
    wxCriticalSectionLocker locker(m_criticalSection);
    m_cache.AddPCH(entry);

    CL_DEBUG(wxT("caching Translation Unit file: %s, %p"), entry.sourceFile.c_str(), (void*)entry.TU);
    CL_DEBUG(wxT(" ==========> [ ClangPchMakerThread ] PCH creation ended successfully <=============="));
}

void ClangWorkerThread::ClearCache()
{
    {
        wxCriticalSectionLocker locker(m_criticalSection);
        m_cache.Clear();
    }

    this->DoSetStatusMsg(wxT("clang: cache cleared"));

    // Notify about cache clear
    wxCommandEvent e(wxEVT_CLANG_PCH_CACHE_CLEARED);
    EventNotifier::Get()->AddPendingEvent(e);
}

bool ClangWorkerThread::IsCacheEmpty()
{
    wxCriticalSectionLocker locker(m_criticalSection);
    return m_cache.IsEmpty();
}

char** ClangWorkerThread::MakeCommandLine(ClangThreadRequest* req, int& argc, FileExtManager::FileType fileType)
{
    bool isHeader = !(fileType == FileExtManager::TypeSourceC || fileType == FileExtManager::TypeSourceCpp);
    wxArrayString tokens;
    const FileTypeCmpArgs_t& compilerArgsMap = req->GetCompilationArgs();

    wxArrayString options;
    if(compilerArgsMap.count(fileType)) {
        // we got commands for this file type
        options = compilerArgsMap.find(fileType)->second;

    } else {
        options = compilerArgsMap.find(FileExtManager::TypeSourceCpp)->second;
    }

    tokens.insert(tokens.end(), options.begin(), options.end());
    if(isHeader) {
        tokens.Add(wxT("-x"));
        tokens.Add(wxT("c++-header"));
    }

    tokens.Add(wxT("-w"));
    tokens.Add(wxT("-ferror-limit=1000"));
    tokens.Add(wxT("-nobuiltininc"));

#ifdef _MSC_VER
    tokens.Add(wxT("-fms-extensions"));
    tokens.Add(wxT("-fdelayed-template-parsing"));
#endif

    //    static bool gotPCH = false;
    //    if(!gotPCH) {
    //        int nArgc(0);
    //        wxArrayString pchTokens;
    //        pchTokens.insert(pchTokens.end(), tokens.begin(), tokens.end());
    //        pchTokens.Add(wxT("-x"));
    //        pchTokens.Add(wxT("c++-header"));
    //        char **pArgv = ClangUtils::MakeArgv(pchTokens, nArgc);
    //        CXTranslationUnit PCH = clang_parseTranslationUnit(req->GetIndex(), "/home/eran/mypch.h", pArgv,
    //        nArgc,
    //                                NULL,
    //                                0,
    //                                CXTranslationUnit_Incomplete);
    //
    //        ClangUtils::FreeArgv(pArgv, nArgc);
    //
    //        if(PCH) {
    //            clang_saveTranslationUnit(PCH, "/home/eran/mypch.pch", clang_defaultSaveOptions(PCH));
    //            clang_disposeTranslationUnit(PCH);
    //
    //            tokens.Add(wxT("-include-pch"));
    //            tokens.Add(wxT("/home/eran/mypch.pch"));
    //        }
    //        gotPCH = true;
    //    }
    char** argv = ClangUtils::MakeArgv(tokens, argc);
    return argv;
}

void ClangWorkerThread::DoSetStatusMsg(const wxString& msg)
{
    clCommandEvent event(wxEVT_CLANG_CODE_COMPLETE_MESSAGE);
    wxString sMsg = msg;
    sMsg.Trim().Append("\n");
    event.SetString(sMsg);
    EventNotifier::Get()->AddPendingEvent(event);
}

bool ClangWorkerThread::DoGotoDefinition(CXTranslationUnit& TU, ClangThreadRequest* request, ClangThreadReply* reply)
{
    // Test to see if we are pointing a function
    CXCursor cur;
    if(ClangUtils::GetCursorAt(TU, request->GetFileName(), request->GetLine(), request->GetColumn(), cur)) {

        if(request->GetContext() == CTX_GotoImpl && !clang_isCursorDefinition(cur)) {
            cur = clang_getCursorDefinition(cur);
        }

        ClangUtils::GetCursorLocation(cur, reply->filename, reply->line, reply->col);
        return true;
    }
    return false;
}

void ClangWorkerThread::PostEvent(int type, const wxString& fileName)
{
    wxCommandEvent e(type);
    if(!fileName.IsEmpty()) {

        wxFileName realFileName(fileName);
        if(realFileName.GetFullName().StartsWith(CODELITE_CLANG_FILE_PREFIX)) {
            realFileName.SetFullName(realFileName.GetFullName().Mid(strlen(CODELITE_CLANG_FILE_PREFIX)));
        }

        ClangThreadReply* reply = new ClangThreadReply;
        reply->filename = realFileName.GetFullPath();
        e.SetClientData(reply);

    } else {
        e.SetClientData(NULL);
    }

    EventNotifier::Get()->AddPendingEvent(e);
}

CXTranslationUnit ClangWorkerThread::DoCreateTU(CXIndex index, ClangThreadRequest* task, bool reparse)
{
    wxFileName fn(task->GetFileName());
    DoSetStatusMsg(wxString::Format(wxT("clang: parsing file %s..."), fn.GetFullName().c_str()));

    FileExtManager::FileType type = FileExtManager::GetType(task->GetFileName());
    int argc(0);
    char** argv = MakeCommandLine(task, argc, type);

    for(int i = 0; i < argc; i++) {
        CL_DEBUG(wxT("Command Line Argument: %s"), wxString(argv[i], wxConvUTF8).c_str());
    }

    std::string c_filename = task->GetFileName().mb_str(wxConvUTF8).data();
    CL_DEBUG(wxT("Calling clang_parseTranslationUnit..."));

    // First time, need to create it
    unsigned flags;
    if(reparse) {
        flags = CXTranslationUnit_CacheCompletionResults | CXTranslationUnit_PrecompiledPreamble |
                CXTranslationUnit_Incomplete | CXTranslationUnit_DetailedPreprocessingRecord |
                CXTranslationUnit_CXXChainedPCH;
    } else {
        flags = CXTranslationUnit_Incomplete |
#if HAS_LIBCLANG_BRIEFCOMMENTS
                CXTranslationUnit_SkipFunctionBodies |
#endif
                CXTranslationUnit_DetailedPreprocessingRecord;
    }

    CXTranslationUnit TU = clang_parseTranslationUnit(index, c_filename.c_str(), argv, argc, NULL, 0, flags);

    CL_DEBUG(wxT("Calling clang_parseTranslationUnit... done"));
    ClangUtils::FreeArgv(argv, argc);
    DoSetStatusMsg(wxString::Format(wxT("clang: parsing file %s...done"), fn.GetFullName().c_str()));
    if(TU && reparse) {
        CL_DEBUG(wxT("Calling clang_reparseTranslationUnit..."));
        if(clang_reparseTranslationUnit(TU, 0, NULL, clang_defaultReparseOptions(TU)) == 0) {
            CL_DEBUG(wxT("Calling clang_reparseTranslationUnit... done"));
            return TU;

        } else {
            CL_DEBUG(wxT("An error occurred during reparsing of the TU for file %s. TU: %p"),
                     task->GetFileName().c_str(), (void*)TU);

            // The only thing that left to be done here, is to dispose the TU
            clang_disposeTranslationUnit(TU);
            PostEvent(wxEVT_CLANG_TU_CREATE_ERROR, task->GetFileName());
            return NULL;
        }
    }
    return TU;
}

#endif // HAS_LIBCLANG
