//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : parse_thread.cpp
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
#include "precompiled_header.h"
#include "tags_storage_sqlite3.h"
#include "pp_include.h"
#include "pptable.h"
#include "file_logger.h"
#include <wx/tokenzr.h>
#include "crawler_include.h"
#include "parse_thread.h"
#include "ctags_manager.h"
#include "istorage.h"
#include <wx/stopwatch.h>
#include <wx/xrc/xmlres.h>
#include <wx/ffile.h>
#include "cpp_scanner.h"
#include <set>
#include "cl_command_event.h"
#include <tags_options_data.h>
#include "CxxVariableScanner.h"

#define DEBUG_MESSAGE(x) CL_DEBUG1(x.c_str())

#define TEST_DESTROY()                                                                                        \
    {                                                                                                         \
        if(TestDestroy()) {                                                                                   \
            DEBUG_MESSAGE(wxString::Format(wxT("ParseThread::ProcessIncludes -> received 'TestDestroy()'"))); \
            return;                                                                                           \
        }                                                                                                     \
    }

// ClientData is set to wxString* which must be deleted by the handler
const wxEventType wxEVT_PARSE_THREAD_MESSAGE = XRCID("parse_thread_update_status_bar");

// ClientData is set to std::set<std::string> *newSet which must deleted by the handler
const wxEventType wxEVT_PARSE_THREAD_SCAN_INCLUDES_DONE = XRCID("parse_thread_scan_includes_done");

const wxEventType wxEVT_PARSE_THREAD_CLEAR_TAGS_CACHE = XRCID("parse_thread_clear_tags_cache");

const wxEventType wxEVT_PARSE_THREAD_RETAGGING_PROGRESS = XRCID("parse_thread_clear_retagging_progress");

// ClientData might contains std::vector<std::string>*, if it is, handler must delete it
const wxEventType wxEVT_PARSE_THREAD_RETAGGING_COMPLETED = XRCID("parse_thread_clear_retagging_compelted");

// ClientData is set to fcFileOpeneer::List_t* which must be deleted by the handler
const wxEventType wxEVT_PARSE_INCLUDE_STATEMENTS_DONE = XRCID("wxEVT_PARSE_INCLUDE_STATEMENTS_DONE");

// Send a "Ready" event
const wxEventType wxEVT_PARSE_THREAD_READY = XRCID("wxEVT_PARSE_THREAD_READY");

// Event type: clCommandEvent
const wxEventType wxEVT_PARSE_THREAD_SUGGEST_COLOUR_TOKENS = XRCID("wxEVT_PARSE_THREAD_SUGGEST_COLOUR_TOKENS");

ParseThread::ParseThread()
    : WorkerThread()
{
}

ParseThread::~ParseThread() {}

void ParseThread::ProcessRequest(ThreadRequest* request)
{
    // request is delete by the parent WorkerThread after this method is completed
    ParseRequest* req = (ParseRequest*)request;

    switch(req->getType()) {
    case ParseRequest::PR_PARSEINCLUDES:
        ProcessIncludes(req);
        break;
    case ParseRequest::PR_PARSE_AND_STORE:
        ProcessParseAndStore(req);
        break;
    case ParseRequest::PR_DELETE_TAGS_OF_FILES:
        ProcessDeleteTagsOfFiles(req);
        break;
    case ParseRequest::PR_PARSE_FILE_NO_INCLUDES:
        ProcessSimpleNoIncludes(req);
        break;
    case ParseRequest::PR_PARSE_INCLUDE_STATEMENTS:
        ProcessIncludeStatements(req);
        break;
    case ParseRequest::PR_SUGGEST_HIGHLIGHT_WORDS:
        ProcessColourRequest(req);
        break;
    default:
    case ParseRequest::PR_FILESAVED:
        ProcessSimple(req);
        break;
    }

    // Always notify when ready
    DoNotifyReady(req->_evtHandler, req->getType());
}

void ParseThread::ParseIncludeFiles(ParseRequest* req, const wxString& filename, ITagsStoragePtr db)
{
    wxArrayString arrFiles;
    GetFileListToParse(filename, arrFiles);
    int initalCount = arrFiles.GetCount();

    TEST_DESTROY();

    DEBUG_MESSAGE(wxString::Format(wxT("Files that need parse %u"), (unsigned int)arrFiles.GetCount()));
    TagsManagerST::Get()->FilterNonNeededFilesForRetaging(arrFiles, db);
    DEBUG_MESSAGE(wxString::Format(wxT("Actual files that need parse %u"), (unsigned int)arrFiles.GetCount()));

    ParseAndStoreFiles(req, arrFiles, initalCount, db);
}

TagTreePtr ParseThread::DoTreeFromTags(const wxString& tags, int& count)
{
    return TagsManagerST::Get()->TreeFromTags(tags, count);
}

void ParseThread::DoStoreTags(const wxString& tags, const wxString& filename, int& count, ITagsStoragePtr db)
{
    TagTreePtr ttp = DoTreeFromTags(tags, count);
    db->Begin();
    db->DeleteByFileName(wxFileName(), filename, false);
    db->Store(ttp, wxFileName(), false);
    db->Commit();
}

void ParseThread::SetCrawlerEnabeld(bool b)
{
    wxCriticalSectionLocker locker(m_cs);
    m_crawlerEnabled = b;
}

void ParseThread::SetSearchPaths(const wxArrayString& paths, const wxArrayString& exlucdePaths)
{
    wxCriticalSectionLocker locker(m_cs);
    m_searchPaths.Clear();
    m_excludePaths.Clear();
    for(size_t i = 0; i < paths.GetCount(); i++) {
        m_searchPaths.Add(paths.Item(i).c_str());
    }

    for(size_t i = 0; i < exlucdePaths.GetCount(); i++) {
        m_excludePaths.Add(exlucdePaths.Item(i).c_str());
    }
}

bool ParseThread::IsCrawlerEnabled()
{
    wxCriticalSectionLocker locker(m_cs);
    return m_crawlerEnabled;
}

void ParseThread::GetSearchPaths(wxArrayString& paths, wxArrayString& excludePaths)
{
    wxCriticalSectionLocker locker(m_cs);
    for(size_t i = 0; i < m_searchPaths.GetCount(); i++) {
        paths.Add(m_searchPaths.Item(i).c_str());
    }

    for(size_t i = 0; i < m_excludePaths.GetCount(); i++) {
        excludePaths.Add(m_excludePaths.Item(i).c_str());
    }
}

void ParseThread::ProcessIncludes(ParseRequest* req)
{
    DEBUG_MESSAGE(wxString::Format(wxT("ProcessIncludes -> started")));

    std::set<wxString>* newSet = new std::set<wxString>();
    FindIncludedFiles(req, newSet);

#ifdef PARSE_THREAD_DBG
    std::set<wxString>::iterator iter = newSet->begin();
    for(; iter != newSet->end(); iter++) {
        wxString fileN((*iter).c_str(), wxConvUTF8);
        DEBUG_MESSAGE(wxString::Format(wxT("ParseThread::ProcessIncludes -> %s"), fileN.c_str()));
    }
#endif

    // collect the results
    if(req->_evtHandler) {
        wxCommandEvent event(wxEVT_PARSE_THREAD_SCAN_INCLUDES_DONE);
        event.SetClientData(newSet);
        event.SetInt((int)req->_quickRetag);
        req->_evtHandler->AddPendingEvent(event);
    }
}

void ParseThread::ProcessSimple(ParseRequest* req)
{
    wxString dbfile = req->getDbfile();
    wxString file = req->getFile();

    clDEBUG1() << "Parsing saved file:" << file << clEndl;
    // Skip binary file
    if(TagsManagerST::Get()->IsBinaryFile(file)) {
        clDEBUG1() << "File:" << file << "is binady and will be skipped" << clEndl;
        return;
    }

    // convert the file to tags
    TagsManager* tagmgr = TagsManagerST::Get();
    ITagsStoragePtr db(new TagsStorageSQLite());
    db->OpenDatabase(dbfile);

    // convert the file content into tags
    wxString tags;
    wxString file_name(req->getFile());
    tagmgr->SourceToTags(file_name, tags);

    clDEBUG1() << "Parsed file output: [" << tags << "]" << clEndl;

    int count;
    DoStoreTags(tags, file_name, count, db);

    db->Begin();
    ///////////////////////////////////////////
    // update the file retag timestamp
    ///////////////////////////////////////////
    db->InsertFileEntry(file, (int)time(NULL));

    ////////////////////////////////////////////////
    // Parse and store the macros found in this file
    ////////////////////////////////////////////////
    PPTable::Instance()->Clear();
    PPScan(file, true);
    db->StoreMacros(PPTable::Instance()->GetTable());
    PPTable::Instance()->Clear();

    db->Commit();

    // Parse the saved file to get a list of files to include
    ParseIncludeFiles(req, file, db);

    // If there is no event handler set to handle this comaprison
    // results, then nothing more to be done
    if(req->_evtHandler) {
        wxCommandEvent clearCacheEvent(wxEVT_PARSE_THREAD_CLEAR_TAGS_CACHE);
        req->_evtHandler->AddPendingEvent(clearCacheEvent);

        wxCommandEvent retaggingCompletedEvent(wxEVT_PARSE_THREAD_RETAGGING_COMPLETED);
        retaggingCompletedEvent.SetClientData(NULL);
        req->_evtHandler->AddPendingEvent(retaggingCompletedEvent);
    }
}

void ParseThread::GetFileListToParse(const wxString& filename, wxArrayString& arrFiles)
{
    if(!this->IsCrawlerEnabled()) {
        return;
    }

    {
        wxCriticalSectionLocker locker(TagsManagerST::Get()->m_crawlerLocker);

        wxArrayString includePaths, excludePaths;
        GetSearchPaths(includePaths, excludePaths);

        fcFileOpener::Get()->ClearResults();
        fcFileOpener::Get()->ClearSearchPath();
        for(size_t i = 0; i < includePaths.GetCount(); i++) {
            fcFileOpener::Get()->AddSearchPath(includePaths.Item(i).mb_str(wxConvUTF8).data());
        }

        for(size_t i = 0; i < excludePaths.GetCount(); i++) {
            fcFileOpener::Get()->AddExcludePath(excludePaths.Item(i).mb_str(wxConvUTF8).data());
        }

        // Invoke the crawler
        const wxCharBuffer cfile = filename.mb_str(wxConvUTF8);

        // Skip binary files
        if(TagsManagerST::Get()->IsBinaryFile(filename)) {
            DEBUG_MESSAGE(wxString::Format(wxT("Skipping binary file %s"), filename.c_str()));
            return;
        }

        // Before using the 'crawlerScan' we lock it, since it is not mt-safe
        crawlerScan(cfile.data());
    }

    std::set<wxString> fileSet = fcFileOpener::Get()->GetResults();
    std::set<wxString>::iterator iter = fileSet.begin();
    for(; iter != fileSet.end(); iter++) {
        wxFileName fn(*iter);
        fn.MakeAbsolute();
        if(arrFiles.Index(fn.GetFullPath()) == wxNOT_FOUND) {
            arrFiles.Add(fn.GetFullPath());
        }
    }
}

void ParseThread::ParseAndStoreFiles(
    ParseRequest* req, const wxArrayString& arrFiles, int initalCount, ITagsStoragePtr db)
{
    // Loop over the files and parse them
    int totalSymbols(0);
    DEBUG_MESSAGE(wxString::Format(wxT("Parsing and saving files to database....")));
    for(size_t i = 0; i < arrFiles.GetCount(); i++) {

        // give a shutdown request a chance
        TEST_DESTROY();

        wxString tags; // output
        TagsManagerST::Get()->SourceToTags(arrFiles.Item(i), tags);

        if(tags.IsEmpty() == false) {
            DoStoreTags(tags, arrFiles.Item(i), totalSymbols, db);
        }
    }

    DEBUG_MESSAGE(wxString(wxT("Done")));

    // Update the retagging timestamp
    TagsManagerST::Get()->UpdateFilesRetagTimestamp(arrFiles, db);

    if(req->_evtHandler) {
        wxCommandEvent e(wxEVT_PARSE_THREAD_MESSAGE);
        wxString message;
        if(initalCount != -1) message << wxT("INFO: Found ") << initalCount << wxT(" system include files. ");
        message << arrFiles.GetCount() << wxT(" needed to be parsed. Stored ") << totalSymbols
                << wxT(" new tags to the database");

        e.SetClientData(new wxString(message.c_str()));
        req->_evtHandler->AddPendingEvent(e);

        // if we added new symbols to the database, send an even to the main thread
        // to clear the tags cache
        if(totalSymbols) {
            wxCommandEvent clearCacheEvent(wxEVT_PARSE_THREAD_CLEAR_TAGS_CACHE);
            req->_evtHandler->AddPendingEvent(clearCacheEvent);
        }
    }
}

void ParseThread::ProcessDeleteTagsOfFiles(ParseRequest* req)
{
    DEBUG_MESSAGE(wxString(wxT("ParseThread::ProcessDeleteTagsOfFile")));
    if(req->_workspaceFiles.empty()) return;

    wxString dbfile = req->getDbfile();
    ITagsStoragePtr db(new TagsStorageSQLite());

    db->OpenDatabase(dbfile);
    db->Begin();

    wxArrayString file_array;

    for(size_t i = 0; i < req->_workspaceFiles.size(); i++) {
        wxString filename(req->_workspaceFiles.at(i).c_str(), wxConvUTF8);
        db->DeleteByFileName(wxFileName(), filename, false);
        file_array.Add(filename);
    }

    db->DeleteFromFiles(file_array);
    db->Commit();
    DEBUG_MESSAGE(wxString(wxT("ParseThread::ProcessDeleteTagsOfFile - completed")));
}

void ParseThread::ProcessParseAndStore(ParseRequest* req)
{
    wxString dbfile = req->getDbfile();

    // convert the file to tags
    double maxVal = (double)req->_workspaceFiles.size();
    if(maxVal == 0.0) {
        return;
    }

    // we report every 10%
    double reportingPoint = maxVal / 100.0;
    reportingPoint = ceil(reportingPoint);
    if(reportingPoint == 0.0) {
        reportingPoint = 1.0;
    }

    ITagsStoragePtr db(new TagsStorageSQLite());
    db->OpenDatabase(dbfile);

    // We commit every 10 files
    db->Begin();
    int precent(0);
    int lastPercentageReported(0);

    PPTable::Instance()->Clear();

    for(size_t i = 0; i < maxVal; i++) {

        // give a shutdown request a chance
        if(TestDestroy()) {
            // Do an ordered shutdown:
            // rollback any transaction
            // and close the database
            db->Rollback();
            return;
        }

        wxFileName curFile(wxString(req->_workspaceFiles.at(i).c_str(), wxConvUTF8));

        // Skip binary files
        if(TagsManagerST::Get()->IsBinaryFile(curFile.GetFullPath())) {
            DEBUG_MESSAGE(wxString::Format(wxT("Skipping binary file %s"), curFile.GetFullPath().c_str()));
            continue;
        }

        // Send notification to the main window with our progress report
        precent = (int)((i / maxVal) * 100);

        if(req->_evtHandler && lastPercentageReported != precent) {
            lastPercentageReported = precent;
            wxCommandEvent retaggingProgressEvent(wxEVT_PARSE_THREAD_RETAGGING_PROGRESS);
            retaggingProgressEvent.SetInt((int)precent);
            req->_evtHandler->AddPendingEvent(retaggingProgressEvent);

        } else if(lastPercentageReported != precent) {
            wxPrintf(wxT("parsing: %%%d completed\n"), precent);
        }

        TagTreePtr tree = TagsManagerST::Get()->ParseSourceFile(curFile);
        PPScan(curFile.GetFullPath(), false);

        db->Store(tree, wxFileName(), false);
        if(db->InsertFileEntry(curFile.GetFullPath(), (int)time(NULL)) == TagExist) {
            db->UpdateFileEntry(curFile.GetFullPath(), (int)time(NULL));
        }

        if(i % 50 == 0) {
            // Commit what we got so far
            db->Commit();
            // Start a new transaction
            db->Begin();
        }
    }

    // Process the macros
    // PPTable::Instance()->Squeeze();
    const std::map<wxString, PPToken>& table = PPTable::Instance()->GetTable();

    // Store the macros
    db->StoreMacros(table);

    // Commit whats left
    db->Commit();

    // Clear the results
    PPTable::Instance()->Clear();

    /// Send notification to the main window with our progress report
    if(req->_evtHandler) {
        wxCommandEvent retaggingCompletedEvent(wxEVT_PARSE_THREAD_RETAGGING_COMPLETED);
        std::vector<std::string>* arrFiles = new std::vector<std::string>;
        *arrFiles = req->_workspaceFiles;
        retaggingCompletedEvent.SetClientData(arrFiles);
        req->_evtHandler->AddPendingEvent(retaggingCompletedEvent);
    }
}

void ParseThread::FindIncludedFiles(ParseRequest* req, std::set<wxString>* newSet)
{
    wxArrayString searchPaths, excludePaths, filteredFileList;
    GetSearchPaths(searchPaths, excludePaths);

    DEBUG_MESSAGE(
        wxString::Format(wxT("Initial workspace files count is %u"), (unsigned int)req->_workspaceFiles.size()));

    for(size_t i = 0; i < req->_workspaceFiles.size(); i++) {
        wxString name(req->_workspaceFiles.at(i).c_str(), wxConvUTF8);
        wxFileName fn(name);
        fn.MakeAbsolute();

        if(TagsManagerST::Get()->IsBinaryFile(fn.GetFullPath())) continue;

        filteredFileList.Add(fn.GetFullPath());
    }

    wxArrayString arrFiles;

    // Clear the results once
    {
        // Before using the 'crawlerScan' we lock it, since it is not mt-safe
        wxCriticalSectionLocker locker(TagsManagerST::Get()->m_crawlerLocker);

        fcFileOpener::Get()->ClearResults();
        fcFileOpener::Get()->ClearSearchPath();

        for(size_t i = 0; i < searchPaths.GetCount(); i++) {
            const wxCharBuffer path = _C(searchPaths.Item(i));
            DEBUG_MESSAGE(wxString::Format(wxT("ParseThread: Using Search Path: %s "), searchPaths.Item(i).c_str()));
            fcFileOpener::Get()->AddSearchPath(path.data());
        }

        for(size_t i = 0; i < excludePaths.GetCount(); i++) {
            const wxCharBuffer path = _C(excludePaths.Item(i));
            DEBUG_MESSAGE(wxString::Format(wxT("ParseThread: Using Exclude Path: %s "), excludePaths.Item(i).c_str()));
            fcFileOpener::Get()->AddExcludePath(path.data());
        }

        for(size_t i = 0; i < filteredFileList.GetCount(); i++) {
            const wxCharBuffer cfile = filteredFileList.Item(i).mb_str(wxConvUTF8);
            crawlerScan(cfile.data());
            if(TestDestroy()) {
                return;
            }
        }
        newSet->insert(fcFileOpener::Get()->GetResults().begin(), fcFileOpener::Get()->GetResults().end());
    }
}

//--------------------------------------------------------------------------------------
// Parse Request Class
//--------------------------------------------------------------------------------------
void ParseRequest::setDbFile(const wxString& dbfile) { _dbfile = dbfile.c_str(); }

void ParseRequest::setTags(const wxString& tags) { _tags = tags.c_str(); }

ParseRequest::ParseRequest(const ParseRequest& rhs)
{
    if(this == &rhs) {
        return;
    }
    *this = rhs;
}

ParseRequest& ParseRequest::operator=(const ParseRequest& rhs)
{
    setFile(rhs._file.c_str());
    setDbFile(rhs._dbfile.c_str());
    setTags(rhs._tags);
    setType(rhs._type);
    return *this;
}

void ParseRequest::setFile(const wxString& file) { _file = file.c_str(); }

ParseRequest::~ParseRequest() {}

// Adaptor to the parse thread
static ParseThread* gs_theParseThread = NULL;

void ParseThreadST::Free()
{
    if(gs_theParseThread) {
        delete gs_theParseThread;
    }
    gs_theParseThread = NULL;
}

ParseThread* ParseThreadST::Get()
{
    if(gs_theParseThread == NULL) gs_theParseThread = new ParseThread;
    return gs_theParseThread;
}

void ParseThread::ProcessSimpleNoIncludes(ParseRequest* req)
{
    std::vector<std::string> files = req->_workspaceFiles;
    wxString dbfile = req->getDbfile();

    // Filter binary files
    std::vector<std::string> filteredFiles;
    wxArrayString filesArr;
    for(size_t i = 0; i < files.size(); i++) {
        wxString filename = wxString(files.at(i).c_str(), wxConvUTF8);
        if(TagsManagerST::Get()->IsBinaryFile(filename)) continue;
        filesArr.Add(filename);
    }

    // convert the file to tags
    ITagsStoragePtr db(new TagsStorageSQLite());
    db->OpenDatabase(dbfile);

    TagsManagerST::Get()->FilterNonNeededFilesForRetaging(filesArr, db);
    ParseAndStoreFiles(req, filesArr, -1, db);

    if(req->_evtHandler) {
        wxCommandEvent e(wxEVT_PARSE_THREAD_RETAGGING_COMPLETED);
        e.SetClientData(NULL);
        req->_evtHandler->AddPendingEvent(e);
    }
}

void ParseThread::ProcessIncludeStatements(ParseRequest* req)
{
    fcFileOpener::List_t* matches = new fcFileOpener::List_t;
    {
        wxString file = req->getFile();
        // Retrieve the "include" files on this file only
        wxCriticalSectionLocker locker(TagsManagerST::Get()->m_crawlerLocker);
        fcFileOpener::Get()->ClearResults();
        fcFileOpener::Get()->ClearSearchPath();
        crawlerScan(file.mb_str(wxConvUTF8).data());

        const fcFileOpener::List_t& incls = fcFileOpener::Get()->GetIncludeStatements();
        matches->insert(matches->end(), incls.begin(), incls.end());
    }

    if(req->_evtHandler) {
        wxCommandEvent event(wxEVT_PARSE_INCLUDE_STATEMENTS_DONE);
        event.SetClientData(matches);
        event.SetInt(req->_uid);
        req->_evtHandler->AddPendingEvent(event);
    }
}

void ParseThread::DoNotifyReady(wxEvtHandler* caller, int requestType)
{
    if(m_notifiedWindow) {
        wxCommandEvent event(wxEVT_PARSE_THREAD_READY);
        event.SetClientData(caller);
        event.SetInt(requestType);
        m_notifiedWindow->AddPendingEvent(event);
    }
}

void ParseThread::ProcessColourRequest(ParseRequest* req)
{
    CppScanner scanner;
    // read the file content
    wxFFile fp(req->getFile(), "rb");
    if(fp.IsOpened()) {
        wxString flatStrLocals, flatClasses;
        wxString content;
        fp.ReadAll(&content);
        fp.Close();

        // lex the file and collect all tokens of type IDENTIFIER
        scanner.SetText(content.mb_str(wxConvUTF8).data());
        std::set<wxString> tokens;

        int type = scanner.yylex();

        while(type != 0) {
            if(type == IDENTIFIER) {
                tokens.insert(scanner.YYText());
            }
            type = scanner.yylex();
        }

        // Convert the set into array
        wxArrayString tokensArr;
        std::set<wxString>::iterator iter = tokens.begin();
        for(; iter != tokens.end(); ++iter) {
            tokensArr.Add(*iter);
        }

        // did we find anything?
        if(tokensArr.IsEmpty()) return;

        tokensArr.Sort();
        // Open the database
        ITagsStoragePtr db(new TagsStorageSQLite());
        db->OpenDatabase(req->getDbfile());

        wxArrayString kinds;
        const size_t colourOptions = TagsManagerST::Get()->GetCtagsOptions().GetCcColourFlags();
        if(colourOptions & CC_COLOUR_CLASS) kinds.Add("class");
        if(colourOptions & CC_COLOUR_ENUM) kinds.Add("enum");
        if(colourOptions & CC_COLOUR_ENUMERATOR) kinds.Add("enumerator");
        if(colourOptions & CC_COLOUR_FUNCTION) kinds.Add("prototype");
        if(colourOptions & CC_COLOUR_MACRO) kinds.Add("macro");
        if(colourOptions & CC_COLOUR_MEMBER) kinds.Add("member");
        if(colourOptions & CC_COLOUR_NAMESPACE) kinds.Add("namespace");
        if(colourOptions & CC_COLOUR_PROTOTYPE) kinds.Add("prototype");
        if(colourOptions & CC_COLOUR_STRUCT) kinds.Add("struct");
        if(colourOptions & CC_COLOUR_TYPEDEF) kinds.Add("typedef");

        db->RemoveNonWorkspaceSymbols(tokensArr, kinds);

        // Now, get the locals
        {
            CxxVariableScanner scanner(content);
            CxxVariable::List_t vars = scanner.GetVariables();

            std::for_each(vars.begin(), vars.end(), [&](CxxVariable::Ptr_t var) {
                if(var->IsUsing()) {
                    tokensArr.Add(var->GetName());
                } else {
                    flatStrLocals << var->GetName() << " ";
                }
            });
        }

        // Convert the class types into a flat string
        tokensArr.Sort();

        // Convert the output to a space delimited array
        std::for_each(tokensArr.begin(), tokensArr.end(), [&](const wxString& token) { flatClasses << token << " "; });

        if(req->_evtHandler) {
            clCommandEvent event(wxEVT_PARSE_THREAD_SUGGEST_COLOUR_TOKENS);
            tokensArr.clear();
            tokensArr.Add(flatClasses);
            tokensArr.Add(flatStrLocals);
            event.SetStrings(tokensArr);
            event.SetFileName(req->getFile());
            req->_evtHandler->AddPendingEvent(event);
        }
    }
}
