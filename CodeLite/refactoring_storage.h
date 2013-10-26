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
    enum CacheStatus {
        CACHE_NOT_READY,
        CACHE_IN_PROGRESS,
        CACHE_READY
    };
protected:
    
    wxSQLite3Database         m_db;
    wxString                  m_cacheDb;
    CacheStatus               m_cacheStatus;
    wxString                  m_workspaceFile;
    CppTokenCacheMakerThread* m_thread;
    
    friend class CppTokenCacheMakerThread;
public:
    RefactoringStorage();
    virtual ~RefactoringStorage();

protected:
    void DoUpdateFileTimestamp(const wxString &filename);
    void DoDeleteFile(const wxString &filename);
    bool IsFileUpToDate(const wxString &filename);
    
    void OnWorkspaceLoaded(wxCommandEvent &e);
    void OnWorkspaceClosed(wxCommandEvent &e);
    void OnThreadStatus(wxCommandEvent &e);
    void Open(const wxString &workspacePath);
    void Begin();
    void Commit();
    void Rollback();
    
    void JoinWorkerThread();
    
public:
    bool IsCacheReady() const {
        return m_cacheStatus == CACHE_READY;
    }
    void StoreTokens( const wxString &filename, const CppToken::List_t& tokens , bool startTx);
    void Match(const wxString &symname, const wxString &filename, CppTokensMap& matches);
    void InitializeCache(const wxFileList_t& files);
    wxFileList_t FilterUpToDateFiles(const wxFileList_t& files);
    CppToken::List_t GetTokens(const wxString& symname, const wxFileList_t& filelist = wxFileList_t());
};

#endif // REFACTORINGSTORAGE_H
