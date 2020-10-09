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
#include "CTags.hpp"
#include "CxxScannerTokens.h"
#include "CxxVariableScanner.h"
#include "cl_command_event.h"
#include "cl_standard_paths.h"
#include "cpp_scanner.h"
#include "crawler_include.h"
#include "ctags_manager.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "istorage.h"
#include "parse_thread.h"
#include "pp_include.h"
#include "pptable.h"
#include "precompiled_header.h"
#include "tags_storage_sqlite3.h"
#include "wxStringHash.h"
#include <functional>
#include <set>
#include <tags_options_data.h>
#include <unordered_set>
#include <wx/ffile.h>
#include <wx/stopwatch.h>
#include <wx/tokenzr.h>

#define DEBUG_MESSAGE(x) CL_DEBUG1(x.c_str())

#define TEST_DESTROY()                                                                                        \
    {                                                                                                         \
        if(TestDestroy()) {                                                                                   \
            DEBUG_MESSAGE(wxString::Format(wxT("ParseThread::ProcessIncludes -> received 'TestDestroy()'"))); \
            return;                                                                                           \
        }                                                                                                     \
    }

// ClientData is set to wxString* which must be deleted by the handler
wxDEFINE_EVENT(wxPARSE_THREAD_MESSAGE, clParseThreadEvent);
wxDEFINE_EVENT(wxPARSE_THREAD_SCAN_INCLUDES_DONE, clParseThreadEvent);
wxDEFINE_EVENT(wxPARSE_THREAD_CLEAR_TAGS_CACHE, clParseThreadEvent);
wxDEFINE_EVENT(wxPARSE_THREAD_RETAGGING_PROGRESS, clParseThreadEvent);
wxDEFINE_EVENT(wxPARSE_THREAD_RETAGGING_COMPLETED, clParseThreadEvent);
wxDEFINE_EVENT(wxPARSE_INCLUDE_STATEMENTS_DONE, clParseThreadEvent);
wxDEFINE_EVENT(wxPARSE_THREAD_READY, clParseThreadEvent);
wxDEFINE_EVENT(wxPARSE_THREAD_SUGGEST_COLOUR_TOKENS, clParseThreadEvent);
wxDEFINE_EVENT(wxPARSE_THREAD_SOURCE_TAGS, clParseThreadEvent);

// ------------------------------------------------------
// ------------------------------------------------------

clParseThreadEvent::clParseThreadEvent(const clParseThreadEvent& event) { *this = event; }

clParseThreadEvent::clParseThreadEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clParseThreadEvent::~clParseThreadEvent() {}

clParseThreadEvent& clParseThreadEvent::operator=(const clParseThreadEvent& src)
{
    clCommandEvent::operator=(src);
    m_files = src.m_files;
    m_quickRetag = src.m_quickRetag;
    m_progressPercentage = src.m_progressPercentage;
    m_requestUID = src.m_requestUID;
    m_caller = src.m_caller;
    m_requestType = src.m_requestType;
    return *this;
}

// ------------------------------------------------------
// ------------------------------------------------------

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

    if(wxFileName::FileExists(filepath)) {
        return filepath;
    }
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
    clConfig config("code-completion.conf");
    config.ReadItem(&m_tod);

    // request is delete by the parent WorkerThread after this method is completed
    ParseRequest* req = (ParseRequest*)request;
    FileLogger::RegisterThread(wxThread::GetCurrentId(), "C++ Parser Thread");

    // Exclude all files found in the exclude folders
    wxArrayString inc, exc;
    GetSearchPaths(inc, exc);

    std::function<bool(const wxString&)> fnIsExcluded = [&](const wxString& file_path) -> bool {
        for(const wxString& dirpath : exc) {
            if(file_path.StartsWith(dirpath)) {
                clDEBUG1() << "Excluding file" << file_path;
                return true;
            }
        }
        return false;
    };

    // Filter non C++ files
    if(!req->GetWorkspaceFiles().IsEmpty()) {
        wxArrayString filtered_list;
        filtered_list.reserve(req->GetWorkspaceFiles().GetCount());
        for(const wxString& filename : req->GetWorkspaceFiles()) {
            if(FileExtManager::IsCxxFile(filename) && !fnIsExcluded(filename)) {
                filtered_list.push_back(filename);
            }
        }
        req->SetWorkspaceFiles(filtered_list);
    }

    clDEBUG1() << "include paths:" << inc;
    clDEBUG1() << "exclude paths:" << exc;

    switch(req->GetType()) {
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
    default:
    case ParseRequest::PR_FILESAVED:
        ProcessSingleFile(req);
        break;
    }

    // Always notify when ready
    DoNotifyReady(req->GetParent(), req->GetType());
}

void ParseThread::ParseIncludeFiles(ParseRequest* req, const wxString& filename, ITagsStoragePtr db)
{
    wxArrayString arrFiles;
    GetFileListToParse(filename, arrFiles);
    TagsManagerST::Get()->FilterNonNeededFilesForRetaging(arrFiles, db);
    ParseAndStoreFiles(req->GetParent(), arrFiles, db);
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
    if(paths.empty() && exlucdePaths.empty()) {
        return;
    }
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
    wxArrayString files;
    FindIncludedFiles(req, files);

    CHECK_PTR_RET(req->GetParent());

    // collect the results
    clParseThreadEvent event(wxPARSE_THREAD_SCAN_INCLUDES_DONE);
    event.SetFiles(files);
    event.SetQuickParse(req->IsQuickRetag());
    req->GetParent()->AddPendingEvent(event);
}

void ParseThread::ProcessSingleFile(ParseRequest* req)
{
    const wxString& dbfile = req->GetDbfile();
    const wxString& file_name = req->GetFile();

    clDEBUG1() << "Parsing saved file:" << file_name << clEndl;
    // Skip binary file

    if(TagsManagerST::Get()->IsBinaryFile(file_name, m_tod)) {
        clDEBUG1() << "File:" << file_name << "is binady and will be skipped" << clEndl;
        return;
    }

    // convert the file to tags
    TagsManager* tagmgr = TagsManagerST::Get();
    ITagsStoragePtr db(new TagsStorageSQLite());
    db->OpenDatabase(dbfile);

    // convert the file content into tags
    wxString tags;
    tagmgr->SourceToTags(file_name, tags);

    clDEBUG1() << "Parsed file output: [" << tags << "]" << clEndl;

    int count;
    DoStoreTags(tags, file_name, count, db);

    db->Begin();
    // update the file retag timestamp
    db->InsertFileEntry(file_name, (int)time(NULL));

    // Parse and store the macros found in this file
    PPTable::Instance()->Clear();
    PPScan(file_name, true);
    db->StoreMacros(PPTable::Instance()->GetTable());
    PPTable::Instance()->Clear();
    db->Commit();

    // Parse the saved file to get a list of files to include
    ParseIncludeFiles(req, file_name, db);
    CHECK_PTR_RET(req->GetParent());

    clParseThreadEvent clearCacheEvent(wxPARSE_THREAD_CLEAR_TAGS_CACHE);
    req->GetParent()->AddPendingEvent(clearCacheEvent);

    clParseThreadEvent retaggingCompletedEvent(wxPARSE_THREAD_RETAGGING_COMPLETED);
    req->GetParent()->AddPendingEvent(retaggingCompletedEvent);
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
        if(TagsManagerST::Get()->IsBinaryFile(filename, m_tod)) {
            DEBUG_MESSAGE(wxString::Format(wxT("Skipping binary file %s"), filename.c_str()));
            return;
        }

        // Before using the 'crawlerScan' we lock it, since it is not mt-safe
        crawlerScan(cfile.data());
    }

    const fcFileOpener::Set_t& fileSet = fcFileOpener::Get()->GetResults();
    arrFiles.Alloc(fileSet.size()); // Make enough room
    for(const wxString& file : fileSet) {
        wxFileName fn(file);
        fn.MakeAbsolute();
        arrFiles.Add(fn.GetFullPath());
    }
}

void ParseThread::ParseAndStoreFiles(wxEvtHandler* parent, const wxArrayString& arrFiles, ITagsStoragePtr db)
{

    // Generate ctags files under the tmp folder
    PostStatusMessage(parent, _("Generating ctags file..."));
    wxString ctagsFileDir = db->GetDatabaseFileName().GetPath();
    if(!CTags::Generate(arrFiles, ctagsFileDir)) {
        return;
    }
    PostStatusMessage(parent, _("Generating ctags file...success"));

    // Storing the files into the database
    wxStringSet_t updatedFiles;
    CTags ctags(ctagsFileDir);
    if(!ctags.IsOpened()) {
        clWARNING() << "Failed to open ctags file under:" << ctagsFileDir << clEndl;
        return;
    }

    wxString curfile;
    PostStatusMessage(parent, _("Updating database..."));
    TagTreePtr ttp = ctags.GetTagsTreeForFile(curfile);
    db->Begin();
    size_t tagsCount = 0;
    size_t maxVal = arrFiles.size();
    while(ttp) {
        ++tagsCount;
        // give a shutdown request a chance
        if(TestDestroy()) {
            // Do an ordered shutdown:
            // rollback any transaction
            // and close the database
            db->Rollback();
            return;
        }

        // Send notification to the main window with our progress report
        db->Store(ttp, {}, false);
        if(db->InsertFileEntry(curfile, (int)time(NULL)) == TagExist) {
            db->UpdateFileEntry(curfile, (int)time(NULL));
        }

        if((tagsCount % 1000) == 0) {
            PostStatusMessage(parent, _("Flushing to disk.."));
            // Commit what we got so far
            db->Commit();
            // Start a new transaction
            db->Begin();
        }
        PostStatusMessage(parent, wxString() << _("Parsing file ") << tagsCount << "/" << maxVal);
        ttp = ctags.GetTagsTreeForFile(curfile);
    }

    // Commit whats left
    db->Commit();
    PostStatusMessage(parent, _("Done"));
    CHECK_PTR_RET(parent);

    // if we added new symbols to the database, send an even to the main thread
    // to clear the tags cache
    if(tagsCount) {
        clParseThreadEvent clearCacheEvent(wxPARSE_THREAD_CLEAR_TAGS_CACHE);
        parent->AddPendingEvent(clearCacheEvent);
    }
}

void ParseThread::ProcessDeleteTagsOfFiles(ParseRequest* req)
{
    if(req->GetWorkspaceFiles().IsEmpty())
        return;

    const wxString& dbfile = req->GetDbfile();
    ITagsStoragePtr db(new TagsStorageSQLite());
    db->OpenDatabase(dbfile);
    db->Begin();
    for(const wxString& filename : req->GetWorkspaceFiles()) {
        db->DeleteByFileName({}, filename, false);
    }
    db->DeleteFromFiles(req->GetWorkspaceFiles());
    db->Commit();
}

void ParseThread::ProcessParseAndStore(ParseRequest* req)
{
    clDEBUG() << "ProcessParseAndStore is called" << clEndl;
    const wxString& dbfile = req->GetDbfile();
    clDEBUG() << "Parsing" << req->GetWorkspaceFiles().size() << "files" << clEndl;
    // convert the file to tags
    double maxVal = req->GetWorkspaceFiles().size();
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

    // Prepend our hack file to the list of files to parse
    const wxString& hackfile = WriteCodeLiteCCHelperFile();
    wxArrayString filesList = req->GetWorkspaceFiles();
    filesList.Insert(hackfile, 0);

    // filter binary files
    FilterBinaryFiles(filesList);
    ParseAndStoreFiles(req->GetParent(), filesList, db);

    // Send notification to the main window with our progress report
    if(req->GetParent()) {
        clParseThreadEvent retaggingCompletedEvent(wxPARSE_THREAD_RETAGGING_COMPLETED);
        retaggingCompletedEvent.SetFiles(req->GetWorkspaceFiles());
        req->GetParent()->AddPendingEvent(retaggingCompletedEvent);
    }
}

void ParseThread::FindIncludedFiles(ParseRequest* req, wxArrayString& files)
{
    wxArrayString searchPaths, excludePaths, filteredFileList;
    GetSearchPaths(searchPaths, excludePaths);

    if(!req->GetWorkspaceFiles().empty()) {
        filteredFileList.Alloc(req->GetWorkspaceFiles().size());
    }

    for(const wxString& filename : req->GetWorkspaceFiles()) {
        wxFileName fn(filename);
        fn.MakeAbsolute();
        fn.Normalize();
        wxString fullpath = fn.GetFullPath();
        if(TagsManagerST::Get()->IsBinaryFile(fullpath, m_tod))
            continue;
        filteredFileList.Add(fullpath);
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
            clDEBUG1() << "Using include path:" << searchPaths.Item(i);
            fcFileOpener::Get()->AddSearchPath(path.data());
        }

        for(size_t i = 0; i < excludePaths.GetCount(); i++) {
            const wxCharBuffer path = _C(excludePaths.Item(i));
            clDEBUG1() << "Using exclude path:" << excludePaths.Item(i);
            fcFileOpener::Get()->AddExcludePath(path.data());
        }

        for(size_t i = 0; i < filteredFileList.GetCount(); i++) {
            const wxCharBuffer cfile = filteredFileList.Item(i).mb_str(wxConvUTF8);
            crawlerScan(cfile.data());
            if(TestDestroy()) {
                return;
            }
        }
        files.Alloc(fcFileOpener::Get()->GetResults().size());
        const auto& result_set = fcFileOpener::Get()->GetResults();
        for(const wxString& file : result_set) {
            files.Add(file);
        }
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
    SetFile(rhs._file);
    SetDbfile(rhs._dbfile);
    SetTags(rhs._tags);
    SetType(rhs._type);
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
    if(gs_theParseThread == NULL)
        gs_theParseThread = new ParseThread;
    return gs_theParseThread;
}

void ParseThread::ProcessSimpleNoIncludes(ParseRequest* req)
{
    wxArrayString files = req->GetWorkspaceFiles();
    const wxString& dbfile = req->GetDbfile();

    // Filter binary files
    FilterBinaryFiles(files);

    // convert the file to tags
    ITagsStoragePtr db(new TagsStorageSQLite());
    db->OpenDatabase(dbfile);

    TagsManagerST::Get()->FilterNonNeededFilesForRetaging(files, db);
    ParseAndStoreFiles(req->GetParent(), files, db);

    CHECK_PTR_RET(req->GetParent());
    clParseThreadEvent e(wxPARSE_THREAD_RETAGGING_COMPLETED);
    req->GetParent()->AddPendingEvent(e);
}

void ParseThread::ProcessIncludeStatements(ParseRequest* req)
{
    {
        const wxString& file = req->GetFile();
        // Retrieve the "include" files on this file only
        wxCriticalSectionLocker locker(TagsManagerST::Get()->m_crawlerLocker);
        // Cleanup
        fcFileOpener::Get()->ClearResults();
        fcFileOpener::Get()->ClearSearchPath();
        crawlerScan(file.mb_str(wxConvUTF8).data());

        const fcFileOpener::Set_t& incls = fcFileOpener::Get()->GetIncludeStatements();
        wxArrayString files;
        files.Alloc(incls.size());
        for(const wxString& filename : incls) {
            files.Add(filename);
        }
        // Cleanup
        fcFileOpener::Get()->ClearResults();
        fcFileOpener::Get()->ClearSearchPath();
    }

    CHECK_PTR_RET(req->GetParent());
    clParseThreadEvent event(wxPARSE_INCLUDE_STATEMENTS_DONE);
    event.SetRequestUID(req->GetUid());
    req->GetParent()->AddPendingEvent(event);
}

void ParseThread::DoNotifyReady(wxEvtHandler* caller, int requestType)
{
    if(m_notifiedWindow) {
        clParseThreadEvent event(wxPARSE_THREAD_READY);
        event.SetCaller(caller);
        event.SetRequestType(requestType);
        m_notifiedWindow->AddPendingEvent(event);
    }
}

void ParseThread::AddPaths(const wxArrayString& inc, const wxArrayString& exc)
{
    // Remove all duplicates
    wxCriticalSectionLocker locker(m_cs);
    if(!inc.IsEmpty()) {
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

    if(!exc.IsEmpty()) {
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

void ParseThread::ClearPaths() { SetSearchPaths({}, {}); }

void ParseThread::PostStatusMessage(wxEvtHandler* parent, const wxString& message)
{
    CHECK_PTR_RET(parent);
    clParseThreadEvent event(wxPARSE_THREAD_MESSAGE);
    event.SetString(message);
    parent->AddPendingEvent(event);
}

void ParseThread::FilterBinaryFiles(wxArrayString& files)
{
    // filter binary files
    wxArrayString tmpfilesList;
    tmpfilesList.Alloc(files.size());
    for(const wxString& filename : files) {
        wxFileName curFile = filename;
        if(curFile.IsRelative()) {
            curFile.MakeAbsolute();
        }
        if(TagsManagerST::Get()->IsBinaryFile(filename, m_tod)) {
            continue;
        }
        tmpfilesList.Add(curFile.GetFullPath());
    }
    files.swap(tmpfilesList);
}
