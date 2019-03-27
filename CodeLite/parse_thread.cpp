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
#include "CxxScannerTokens.h"
#include "CxxVariableScanner.h"
#include "cl_command_event.h"
#include "cl_standard_paths.h"
#include "cpp_scanner.h"
#include "crawler_include.h"
#include "ctags_manager.h"
#include "file_logger.h"
#include "fileutils.h"
#include "istorage.h"
#include "parse_thread.h"
#include "pp_include.h"
#include "pptable.h"
#include "precompiled_header.h"
#include "tags_storage_sqlite3.h"
#include <set>
#include <tags_options_data.h>
#include <wx/ffile.h>
#include <wx/stopwatch.h>
#include <wx/tokenzr.h>
#include "fileextmanager.h"

#define DEBUG_MESSAGE(x) CL_DEBUG1(x.c_str())

#define TEST_DESTROY()                                                                                        \
    {                                                                                                         \
        if(TestDestroy()) {                                                                                   \
            DEBUG_MESSAGE(wxString::Format(wxT("ParseThread::ProcessIncludes -> received 'TestDestroy()'"))); \
            return;                                                                                           \
        }                                                                                                     \
    }

// ClientData is set to wxString* which must be deleted by the handler
wxDEFINE_EVENT(wxEVT_PARSE_THREAD_MESSAGE, wxCommandEvent);
// ClientData is set to std::set<std::string> *newSet which must deleted by the handler
wxDEFINE_EVENT(wxEVT_PARSE_THREAD_SCAN_INCLUDES_DONE, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_PARSE_THREAD_CLEAR_TAGS_CACHE, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_PARSE_THREAD_RETAGGING_PROGRESS, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_PARSE_THREAD_RETAGGING_COMPLETED, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_PARSE_INCLUDE_STATEMENTS_DONE, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_PARSE_THREAD_READY, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_PARSE_THREAD_SUGGEST_COLOUR_TOKENS, clCommandEvent);
wxDEFINE_EVENT(wxEVT_PARSE_THREAD_SOURCE_TAGS, clCommandEvent);

static const wxString& WriteCodeLiteCCHelperFile()
{
    // Due to heavy changes to shared_ptr in GCC 7.X and later
    // we need to simplify the shared_ptr class
    // we do this by adding our own small shared_ptr class (these entries will be added to the
    // ones that were actuall parsed from the original std::shared_ptr class as defined by libstd++)
    // We might expand this hack in the future in favour of other changes
    static wxString buffer = "namespace std { template<typename _Tp> class shared_ptr {\n"
                             "    _Tp* operator->();\n"
                             "    void reset( Y* ptr );\n"
                             "    void reset( Y* ptr, Deleter d );\n"
                             "    void reset( Y* ptr, Deleter d, Alloc alloc );\n"
                             "    _T* get() const;\n"
                             "};\n"
                             "} // namespace std\n";
    static wxString filepath;
    if(filepath.IsEmpty()) {
        wxFileName tmpFile(clStandardPaths::Get().GetUserDataDir(), "codelite_templates.hpp");
        tmpFile.AppendDir("tmp");
        tmpFile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        filepath = tmpFile.GetFullPath();
    }

    if(wxFileName::FileExists(filepath)) { return filepath; }
    FileUtils::WriteFileContent(wxFileName(filepath), buffer);
    return filepath;
}

ParseThread::ParseThread()
    : WorkerThread()
{
}

ParseThread::~ParseThread() {}

void ParseThread::ProcessRequest(ThreadRequest* request)
{
    // request is delete by the parent WorkerThread after this method is completed
    ParseRequest* req = (ParseRequest*)request;
    FileLogger::RegisterThread(wxThread::GetCurrentId(), "C++ Parser Thread");

    // Filter non C++ files
    if(!req->_workspaceFiles.empty()) {
        std::vector<std::string> filtered_list;
        filtered_list.reserve(req->_workspaceFiles.size());
        for(std::string& filename : req->_workspaceFiles) {
            if(FileExtManager::IsCxxFile(wxString() << filename)) { filtered_list.push_back(std::move(filename)); }
        }
        req->_workspaceFiles.swap(filtered_list);
    }

    wxArrayString inc, exc;
    GetSearchPaths(inc, exc);

    clDEBUG1() << "include paths:\n" << inc;

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
    case ParseRequest::PR_SOURCE_TO_TAGS:
        ProcessSourceToTags(req);
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
    std::set<wxString>* newSet = new std::set<wxString>();
    FindIncludedFiles(req, newSet);

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
    if(!this->IsCrawlerEnabled()) { return; }

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

    const fcFileOpener::Set_t& fileSet = fcFileOpener::Get()->GetResults();
    arrFiles.Alloc(fileSet.size()); // Make enough room
    std::for_each(fileSet.begin(), fileSet.end(), [&](const wxString& file) {
        wxFileName fn(file);
        fn.MakeAbsolute();
        arrFiles.Add(fn.GetFullPath());
    });
}

void ParseThread::ParseAndStoreFiles(ParseRequest* req, const wxArrayString& arrFiles, int initalCount,
                                     ITagsStoragePtr db)
{
    // Loop over the files and parse them
    int totalSymbols(0);
    DEBUG_MESSAGE(wxString::Format(wxT("Parsing and saving files to database....")));
    for(size_t i = 0; i < arrFiles.GetCount(); i++) {

        // give a shutdown request a chance
        TEST_DESTROY();

        wxString tags; // output
        TagsManagerST::Get()->SourceToTags(arrFiles.Item(i), tags);

        if(tags.IsEmpty() == false) { DoStoreTags(tags, arrFiles.Item(i), totalSymbols, db); }
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
    if(maxVal == 0.0) { return; }

    // we report every 10%
    double reportingPoint = maxVal / 100.0;
    reportingPoint = ceil(reportingPoint);
    if(reportingPoint == 0.0) { reportingPoint = 1.0; }

    ITagsStoragePtr db(new TagsStorageSQLite());
    db->OpenDatabase(dbfile);

    // We commit every 10 files
    db->Begin();
    int precent(0);
    int lastPercentageReported(0);

    // Prepend our hack file to the list of files to parse
    const wxString& hackfile = WriteCodeLiteCCHelperFile();
    req->_workspaceFiles.insert(req->_workspaceFiles.begin(), hackfile.ToStdString());
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

        wxFileName curFile(wxString(req->_workspaceFiles[i].c_str(), wxConvUTF8));

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
            if(TestDestroy()) { return; }
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
    if(this == &rhs) { return; }
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
    if(gs_theParseThread) { delete gs_theParseThread; }
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
    fcFileOpener::Set_t* matches = new fcFileOpener::Set_t;
    {
        wxString file = req->getFile();
        // Retrieve the "include" files on this file only
        wxCriticalSectionLocker locker(TagsManagerST::Get()->m_crawlerLocker);
        // Cleanup
        fcFileOpener::Get()->ClearResults();
        fcFileOpener::Get()->ClearSearchPath();
        crawlerScan(file.mb_str(wxConvUTF8).data());

        fcFileOpener::Set_t& incls = fcFileOpener::Get()->GetIncludeStatements();
        matches->swap(incls);
        // Cleanup
        fcFileOpener::Get()->ClearResults();
        fcFileOpener::Get()->ClearSearchPath();
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
    CxxTokenizer tokenizer;
    // read the file content
    wxString content;
    if(FileUtils::ReadFileContent(req->getFile(), content)) {
        wxString flatStrLocals, flatClasses;

        tokenizer.Reset(content);

        // lex the file and collect all tokens of type IDENTIFIER
        wxStringSet_t tokens;
        CxxLexerToken tok;
        while(tokenizer.NextToken(tok)) {
            if(tok.GetType() == T_IDENTIFIER) { tokens.insert(tok.GetWXString()); }
        }

        std::vector<wxString> tokensArr;
        tokensArr.reserve(tokens.size());
        std::for_each(tokens.begin(), tokens.end(), [&](const wxString& str) { tokensArr.push_back(str); });
        std::sort(tokensArr.begin(), tokensArr.end());

        // did we find anything?
        if(tokensArr.empty()) { return; }

        // Open the database
        ITagsStoragePtr db(new TagsStorageSQLite());
        db->OpenDatabase(req->getDbfile());

        std::vector<wxString> nonWorkspaceSymbols, workspaceSymbols;
        db->RemoveNonWorkspaceSymbols(tokensArr, workspaceSymbols, nonWorkspaceSymbols);

        // Convert the output to a space delimited array
        std::for_each(workspaceSymbols.begin(), workspaceSymbols.end(),
                      [&](const wxString& token) { flatClasses << token << " "; });
        std::for_each(nonWorkspaceSymbols.begin(), nonWorkspaceSymbols.end(),
                      [&](const wxString& token) { flatStrLocals << token << " "; });

        if(req->_evtHandler) {
            clCommandEvent event(wxEVT_PARSE_THREAD_SUGGEST_COLOUR_TOKENS);
            wxArrayString res;
            res.Add(flatClasses);
            res.Add(flatStrLocals);
            event.SetStrings(res);
            event.SetFileName(req->getFile());
            req->_evtHandler->AddPendingEvent(event);
        }
    }
}

void ParseThread::ProcessSourceToTags(ParseRequest* req)
{
    wxFileName filename(req->getFile());
    if(TagsManagerST::Get()->IsBinaryFile(filename.GetFullPath())) { return; }

    wxString strTags;
    TagsManagerST::Get()->SourceToTags(filename, strTags);

    // Fire the event
    clCommandEvent event(wxEVT_PARSE_THREAD_SOURCE_TAGS);
    event.SetFileName(filename.GetFullPath());
    event.SetString(strTags);
    event.SetInt(req->_uid); // send back the unique ID
    req->_evtHandler->AddPendingEvent(event);
}

void ParseThread::AddPaths(const wxArrayString& inc, const wxArrayString& exc)
{
    // remove all duplicates
    wxCriticalSectionLocker locker(m_cs);
    {
        wxStringSet_t unique;
        wxArrayString tmp;
        for(const wxString& path : inc) {
            wxFileName fnPath(path, "");
            wxString canonPath = fnPath.GetPath();
            if(FileUtils::IsDirectory(canonPath)) {
                if(unique.count(canonPath) == 0) {
                    unique.insert(canonPath);
                    tmp.Add(canonPath);
                }
            }
        }
        for(const wxString& path : m_searchPaths) {
            wxFileName fnPath(path, "");
            wxString canonPath = fnPath.GetPath();
            if(FileUtils::IsDirectory(canonPath)) {
                if(unique.count(canonPath) == 0) {
                    unique.insert(canonPath);
                    tmp.Add(canonPath);
                }
            }
        }
        m_searchPaths.swap(tmp);
        std::sort(m_searchPaths.begin(), m_searchPaths.end());
    }
    
    {
        wxStringSet_t unique;
        wxArrayString tmp;
        for(const wxString& path : exc) {
            wxFileName fnPath(path, "");
            wxString canonPath = fnPath.GetPath();
            if(FileUtils::IsDirectory(canonPath)) {
                if(unique.count(canonPath) == 0) {
                    unique.insert(canonPath);
                    tmp.Add(canonPath);
                }
            }
        }
        for(const wxString& path : m_excludePaths) {
            wxFileName fnPath(path, "");
            wxString canonPath = fnPath.GetPath();
            if(FileUtils::IsDirectory(canonPath)) {
                if(unique.count(canonPath) == 0) {
                    unique.insert(canonPath);
                    tmp.Add(canonPath);
                }
            }
        }
        m_excludePaths.swap(tmp);
        std::sort(m_excludePaths.begin(), m_excludePaths.end());
    }
}

void ParseThread::ClearPaths() {}
