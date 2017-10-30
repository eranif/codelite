//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : refactoring_storage.h
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

#ifndef REFACTORINGSTORAGE_H
#define REFACTORINGSTORAGE_H

#include <wx/event.h>
#include <wx/wxsqlite3.h>
#include "cpptoken.h"
#include "codelite_exports.h"
#include <wx/thread.h>
#include <wx/filename.h>
#include <vector>

class CppTokenCacheMakerThread;
class WXDLLIMPEXP_CL RefactoringStorage : public wxEvtHandler
{
public:
    enum CacheStatus { CACHE_NOT_READY, CACHE_IN_PROGRESS, CACHE_READY };

protected:
    wxSQLite3Database m_db;
    wxString m_cacheDb;
    CacheStatus m_cacheStatus;
    wxString m_workspaceFile;
    CppTokenCacheMakerThread* m_thread;

    friend class CppTokenCacheMakerThread;

public:
    RefactoringStorage();
    virtual ~RefactoringStorage();

protected:
    wxLongLong DoUpdateFileTimestamp(const wxString& filename);
    void DoDeleteFile(wxLongLong fileID);
    bool IsFileUpToDate(const wxString& filename);

    void OnWorkspaceLoaded(wxCommandEvent& e);
    void OnWorkspaceClosed(wxCommandEvent& e);
    void OnThreadStatus(wxCommandEvent& e);
    void Open(const wxString& workspacePath);
    void Begin();
    void Commit();
    void Rollback();

    void JoinWorkerThread();
    wxString GetSchemaVersion();
    /**
     * @brief return the file ID from the database
     */
    wxLongLong GetFileID(const wxFileName& filename);
    /**
     * @brief return the file ID from the database
     */
    wxLongLong GetFileID(const wxString& filename);
    
public:
    bool IsCacheReady() const { return m_cacheStatus == CACHE_READY; }
    void StoreTokens(const wxString& filename, const CppToken::Vec_t& tokens, bool startTx);
    void Match(const wxString& symname, const wxString& filename, CppTokensMap& matches);
    void InitializeCache(const wxFileList_t& files);
    wxFileList_t FilterUpToDateFiles(const wxFileList_t& files);
    CppToken::Vec_t GetTokens(const wxString& symname, const wxFileList_t& filelist = wxFileList_t());
};

#endif // REFACTORINGSTORAGE_H
