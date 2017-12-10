//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : refactoring_storage.cpp
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

#include "refactoring_storage.h"
#include "event_notifier.h"
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/crt.h>
#include "cppwordscanner.h"
#include "refactorengine.h"
#include "ctags_manager.h"
#include "codelite_events.h"
#include "file_logger.h"
#include "fileextmanager.h"

class CppTokenCacheMakerThread : public wxThread
{
protected:
    RefactoringStorage* m_storage;
    wxString m_workspaceFile;
    wxFileList_t m_files;

public:
    CppTokenCacheMakerThread(RefactoringStorage* storage, const wxString& workspaceFile, const wxFileList_t& files)
        : wxThread(wxTHREAD_JOINABLE)
        , m_storage(storage)
        , m_workspaceFile(workspaceFile.c_str())
    {
        m_files.insert(m_files.end(), files.begin(), files.end());
    }

    virtual ~CppTokenCacheMakerThread() {}

    /**
     * @brief stop the working thread
     * this function shdould be called from the main thread only
     */
    void Stop()
    {
        if(IsAlive()) {
            Delete(NULL, wxTHREAD_WAIT_BLOCK);

        } else {
            Wait(wxTHREAD_WAIT_BLOCK);
        }
    }

    void* Entry()
    {
        RefactoringStorage storage;
        storage.Open(m_workspaceFile);
        storage.m_cacheStatus = RefactoringStorage::CACHE_READY;

        wxCommandEvent evtStatus1(wxEVT_REFACTORING_ENGINE_CACHE_INITIALIZING);
        evtStatus1.SetInt(0);
        evtStatus1.SetString(m_workspaceFile);
        EventNotifier::Get()->AddPendingEvent(evtStatus1);

        size_t count = 0;

        storage.Begin();
        wxFileList_t::const_iterator iter = m_files.begin();
        for(; iter != m_files.end(); ++iter) {

            if(TestDestroy()) {
                // we requested to stop
                storage.Commit();
                wxCommandEvent evtStatus2(wxEVT_REFACTORING_ENGINE_CACHE_INITIALIZING);
                evtStatus2.SetInt(100);
                evtStatus2.SetString(m_workspaceFile);
                EventNotifier::Get()->AddPendingEvent(evtStatus2);
                return NULL;
            }

            if(!TagsManagerST::Get()->IsValidCtagsFile((*iter))) {
                continue;
            }

            ++count;
            if(count % 100 == 0) {
                storage.Commit();
                storage.Begin();
            }

            wxString fullpath = iter->GetFullPath();
            if(!storage.IsFileUpToDate(fullpath)) {
                CppWordScanner scanner(fullpath);
                CppToken::Vec_t tokens = scanner.tokenize();

                if(false) {
                    CppToken::Vec_t::iterator tokIter = tokens.begin();
                    for(; tokIter != tokens.end(); ++tokIter) {
                        wxPrintf("%s | %s | %d\n",
                                 tokIter->getFilename().c_str(),
                                 tokIter->getName().c_str(),
                                 (int)tokIter->getOffset());
                    }
                }

                storage.StoreTokens(fullpath, tokens, false);
            }
        }

        storage.Commit();
        wxCommandEvent evtStatus2(wxEVT_REFACTORING_ENGINE_CACHE_INITIALIZING);
        evtStatus2.SetInt(100);
        evtStatus2.SetString(m_workspaceFile);
        EventNotifier::Get()->AddPendingEvent(evtStatus2);
        return NULL;
    }
};

RefactoringStorage::RefactoringStorage()
    : m_cacheStatus(CACHE_NOT_READY)
    , m_thread(NULL)
{
    if(wxThread::IsMain()) {
        EventNotifier::Get()->Connect(
            wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(RefactoringStorage::OnWorkspaceLoaded), NULL, this);
        EventNotifier::Get()->Connect(
            wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(RefactoringStorage::OnWorkspaceClosed), NULL, this);
        EventNotifier::Get()->Connect(wxEVT_REFACTORING_ENGINE_CACHE_INITIALIZING,
                                      wxCommandEventHandler(RefactoringStorage::OnThreadStatus),
                                      NULL,
                                      this);
    }
}

RefactoringStorage::~RefactoringStorage()
{
    if(wxThread::IsMain()) {
        EventNotifier::Get()->Disconnect(
            wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(RefactoringStorage::OnWorkspaceLoaded), NULL, this);
        EventNotifier::Get()->Disconnect(
            wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(RefactoringStorage::OnWorkspaceClosed), NULL, this);
        EventNotifier::Get()->Disconnect(wxEVT_REFACTORING_ENGINE_CACHE_INITIALIZING,
                                         wxCommandEventHandler(RefactoringStorage::OnThreadStatus),
                                         NULL,
                                         this);

        JoinWorkerThread();
    }
}

void RefactoringStorage::StoreTokens(const wxString& filename, const CppToken::Vec_t& tokens, bool startTx)
{
    if(!IsCacheReady() || !m_db.IsOpen()) {
        return;
    }

    if(!m_db.IsOpen()) return;

    try {
        if(startTx) {
            Begin();
        }
        
        wxLongLong fileId = GetFileID(filename);
        if(fileId != wxNOT_FOUND) {
            DoDeleteFile(fileId);
        } 
        
        // Insert a match to the FILES table
        fileId = DoUpdateFileTimestamp(filename);
        CppToken::Vec_t::const_iterator iter = tokens.begin();
        for(; iter != tokens.end(); ++iter) {
            iter->store(&m_db, fileId);
        }

        if(startTx) {
            Commit();
        }

    } catch(wxSQLite3Exception& e) {
        if(startTx) {
            Rollback();
        }
        wxUnusedVar(e);
    }
}

void RefactoringStorage::Open(const wxString& workspacePath)
{
    wxFileName fnWorkspace(workspacePath);
    fnWorkspace.AppendDir(".codelite");
    fnWorkspace.SetFullName("refactoring.db");
    fnWorkspace.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    if(m_db.IsOpen()) {
        m_db.Close();
    }

    static const wxString CURR_SCHEMA = "1.0.0";
    m_db.Open(fnWorkspace.GetFullPath());
    if(GetSchemaVersion() != CURR_SCHEMA) {
        // Drop the tables and recreate the schema
        try {
            m_db.ExecuteUpdate("drop table if exists REFACTORING_SCHEMA");
            m_db.ExecuteUpdate("drop table if exists TOKENS_TABLE");
            m_db.ExecuteUpdate("drop table if exists FILES");
            m_db.ExecuteUpdate("drop index if exists TOKENS_TABLE_IDX1");
            m_db.ExecuteUpdate("drop index if exists TOKENS_TABLE_IDX2");
            m_db.ExecuteUpdate("drop index if exists FILES_IDX1");
        } catch(wxSQLite3Exception& e) {
            wxUnusedVar(e);
        }
    }

    m_cacheDb = fnWorkspace.GetFullPath();

    // Create the schema
    try {
        m_db.ExecuteUpdate("PRAGMA journal_mode= OFF");
        m_db.ExecuteUpdate("PRAGMA synchronous = OFF");

        m_db.ExecuteUpdate("PRAGMA temp_store = MEMORY");
        m_db.ExecuteUpdate("create table if not exists REFACTORING_SCHEMA (VERSION string primary key)");

        m_db.ExecuteUpdate("create table if not exists TOKENS_TABLE(ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                           "                                        NAME VARCHAR(128),"
                           "                                        OFFSET INTEGER,"
                           "                                        FILE_ID INTEGER,"
                           "                                        LINE_NUMBER INTEGER)");
        m_db.ExecuteUpdate("create index if not exists TOKENS_TABLE_IDX1 on TOKENS_TABLE(NAME)");
        m_db.ExecuteUpdate("create index if not exists TOKENS_TABLE_IDX2 on TOKENS_TABLE(FILE_ID)");

        m_db.ExecuteUpdate("create table if not exists FILES (ID INTEGER PRIMARY KEY AUTOINCREMENT, FILE_NAME "
                           "VARCHAR(256), LAST_UPDATED INTEGER)");
        m_db.ExecuteUpdate("create unique index if not exists FILES_IDX1 on FILES(FILE_NAME)");
        // set the schema version
        wxString sql = wxString(wxT("replace into REFACTORING_SCHEMA values ('")) << CURR_SCHEMA << wxT("')");
        m_db.ExecuteUpdate(sql);

    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}

wxLongLong RefactoringStorage::DoUpdateFileTimestamp(const wxString& filename)
{
    try {
        wxSQLite3Statement st =
            m_db.PrepareStatement("REPLACE INTO FILES (ID, FILE_NAME, LAST_UPDATED) VALUES ( NULL, ?, ?)");
        st.Bind(1, filename);
        st.Bind(2, (int)time(NULL));
        st.ExecuteUpdate();
        return m_db.GetLastRowId();
        
    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
    return wxNOT_FOUND;
}

void RefactoringStorage::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
    m_workspaceFile = e.GetString();
    if(m_workspaceFile.IsEmpty()) {
        return;
    }
    
    if(FileExtManager::GetType(m_workspaceFile) != FileExtManager::TypeWorkspace) {
        m_workspaceFile.Clear();
        return; // Not a C++ workspace, nothing to be done here
    }
    
    m_cacheStatus = CACHE_NOT_READY;
    Open(m_workspaceFile);
}

void RefactoringStorage::DoDeleteFile(wxLongLong fileID)
{
    try {

        // delete fhe file entry
        {
            wxSQLite3Statement st = m_db.PrepareStatement("DELETE FROM FILES WHERE ID=?");
            st.Bind(1, fileID);
            st.ExecuteUpdate();
        }

        // remove all tokens for the given file
        {
            wxSQLite3Statement st = m_db.PrepareStatement("delete from TOKENS_TABLE where FILE_ID=?");
            st.Bind(1, fileID);
            st.ExecuteUpdate();
        }
    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}

void RefactoringStorage::Match(const wxString& symname, const wxString& filename, CppTokensMap& matches)
{
    if(!IsCacheReady() || !m_db.IsOpen()) {
        return;
    }

    if(!IsFileUpToDate(filename)) {
        // update the cache
        CppWordScanner tmpScanner(filename);
        CppToken::Vec_t tokens_list = tmpScanner.tokenize();
        StoreTokens(filename, tokens_list, true);
    }
    
    wxLongLong fileID = GetFileID(filename);
    if(fileID == wxNOT_FOUND) return;
    CppToken::Vec_t list = CppToken::loadByNameAndFile(&m_db, symname, fileID);
    matches.addToken(symname, list);
}

bool RefactoringStorage::IsFileUpToDate(const wxString& filename)
{
    if(!wxFileName(filename).FileExists()) {
        return true;
    }

    time_t last_updated_in_db = 0;
    time_t lastModified = wxFileName(filename).GetModificationTime().GetTicks();
    try {
        wxString sql("SELECT LAST_UPDATED from FILES WHERE FILE_NAME=?");
        wxSQLite3Statement st = m_db.PrepareStatement(sql);
        st.Bind(1, filename);
        wxSQLite3ResultSet res = st.ExecuteQuery();
        if(res.NextRow()) {
            last_updated_in_db = res.GetInt(0);
        }
        return last_updated_in_db >= lastModified;
    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
    return false;
}

/**
 * @brief
 * @param files
 */
void RefactoringStorage::InitializeCache(const wxFileList_t& files)
{
    if(m_thread == NULL) {
        m_cacheStatus = CACHE_IN_PROGRESS;
        m_thread = new CppTokenCacheMakerThread(this, m_workspaceFile, files);
        m_thread->Create();
        m_thread->Run();
    }
}

wxFileList_t RefactoringStorage::FilterUpToDateFiles(const wxFileList_t& files)
{
    if(!IsCacheReady() || !m_db.IsOpen()) {
        return files;
    }
    wxFileList_t res;
    wxFileList_t::const_iterator iter = files.begin();
    for(; iter != files.end(); ++iter) {
        if(TagsManagerST::Get()->IsValidCtagsFile((*iter)) && !IsFileUpToDate(iter->GetFullPath())) {
            res.push_back(*iter);
        }
    }
    return res;
}

CppToken::Vec_t RefactoringStorage::GetTokens(const wxString& symname, const wxFileList_t& filelist)
{
    if(!IsCacheReady() || !m_db.IsOpen()) {
        return CppToken::Vec_t();
    }

    CppToken::Vec_t tokens = CppToken::loadByName(&m_db, symname);

    // Include only tokens which belongs to the current list of files
    // unless the filelist is empty and in this case, we ignore this filter
    if(filelist.empty()) {
        return tokens;
    }

    CppToken::Vec_t::iterator iter = std::remove_if(tokens.begin(), tokens.end(), CppToken::RemoveIfNotIn(filelist));
    tokens.resize(std::distance(tokens.begin(), iter));
    return tokens;
}

void RefactoringStorage::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    m_cacheStatus = CACHE_NOT_READY;
    try {
        JoinWorkerThread();

        m_db.Close();
        m_workspaceFile.Clear();
        m_cacheDb.Clear();

    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}

void RefactoringStorage::OnThreadStatus(wxCommandEvent& e)
{
    e.Skip();
    if(e.GetInt() == 100) {

        // Release the worker thread
        JoinWorkerThread();

        // completed
        if(e.GetString() == m_workspaceFile && m_cacheStatus == CACHE_IN_PROGRESS) {
            // same file
            m_cacheStatus = CACHE_READY;
        }
    }
}

void RefactoringStorage::Begin()
{
    try {
        m_db.Begin();
    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}

void RefactoringStorage::Commit()
{
    try {
        m_db.Commit();
    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}

void RefactoringStorage::Rollback()
{
    try {
        m_db.Rollback();
    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}

void RefactoringStorage::JoinWorkerThread()
{
    if(m_thread) {
        m_thread->Stop();
        m_thread = NULL;
    }
}

wxString RefactoringStorage::GetSchemaVersion()
{
    try {
        wxString sql("SELECT VERSION from REFACTORING_SCHEMA");
        wxSQLite3Statement st = m_db.PrepareStatement(sql);
        wxSQLite3ResultSet res = st.ExecuteQuery();
        if(res.NextRow()) {
            return res.GetString(0);
        }

    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
    return wxEmptyString;
}

wxLongLong RefactoringStorage::GetFileID(const wxFileName& filename) { return GetFileID(filename.GetFullPath()); }

wxLongLong RefactoringStorage::GetFileID(const wxString& filename) 
{
    try {
        wxLongLong fileId = wxNOT_FOUND;
        wxSQLite3Statement st = m_db.PrepareStatement("SELECT ID FROM FILES WHERE FILE_NAME=? LIMIT 1");
        st.Bind(1, filename);
        wxSQLite3ResultSet res = st.ExecuteQuery();
        if(res.NextRow()) {
            fileId = res.GetInt64(0);
        }
        return fileId;
    } catch(wxSQLite3Exception& e) {
        CL_ERROR("RefactoringStorage::GetFileID: %s", e.GetMessage());
        return wxNOT_FOUND;
    }
}
