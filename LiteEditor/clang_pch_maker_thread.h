#ifndef CLANGPCHMAKERTHREAD_H
#define CLANGPCHMAKERTHREAD_H

#if HAS_LIBCLANG

#include "worker_thread.h" // Base class: ThreadRequest
#include "clangpch_cache.h"
#include <clang-c/Index.h>
#include <set>
#include "fileextmanager.h"

extern const wxEventType wxEVT_CLANG_PCH_CACHE_STARTED ;
extern const wxEventType wxEVT_CLANG_PCH_CACHE_ENDED   ;
extern const wxEventType wxEVT_CLANG_PCH_CACHE_CLEARED ;
extern const wxEventType wxEVT_CLANG_TU_CREATE_ERROR ;

enum WorkingContext {
    CTX_CodeCompletion,
    CTX_Calltip,
    CTX_CachePCH,
    CTX_WordCompletion,
    CTX_ReparseTU,
    CTX_GotoDecl,
    CTX_GotoImpl
};

struct ClangThreadReply {
    wxString               filterWord;
    WorkingContext         context;
    wxString               filename;
    wxString               macrosAsString;
    CXCodeCompleteResults *results;
    wxString               errorMessage;
    unsigned               line;
    unsigned               col;
};

class ClangThreadRequest : public ThreadRequest
{
private:
    CXIndex        _index;
    wxString       _fileName;
    wxString       _dirtyBuffer;
    wxArrayString  _compilationArgs;
    wxString       _filterWord;
    WorkingContext _context;
    unsigned       _line;
    unsigned       _column;
    wxString       _pchFile;

public:
    ClangThreadRequest(CXIndex index, const wxString &filename, const wxString &dirtyBuffer, const wxArrayString &compArgs, const wxString &filterWord, WorkingContext context, unsigned line, unsigned column)
        : _index(index)
        , _fileName(filename.c_str())
        , _dirtyBuffer(dirtyBuffer.c_str())
        , _filterWord(filterWord)
        , _context(context)
        , _line(line)
        , _column(column) {
        for(size_t i=0; i<compArgs.GetCount(); i++) {
            _compilationArgs.Add(compArgs.Item(i).c_str());
        }
    }

    void SetPchFile(const wxString& _pchFile) {
        this->_pchFile = _pchFile;
    }
    const wxString& GetPchFile() const {
        return _pchFile;
    }
    unsigned GetColumn() const {
        return _column;
    }
    const wxString& GetFilterWord() const {
        return _filterWord;
    }
    unsigned GetLine() const {
        return _line;
    }
    virtual ~ClangThreadRequest() {}
    const wxString& GetDirtyBuffer() const {
        return _dirtyBuffer;
    }
    const wxString& GetFileName() const {
        return _fileName;
    }
    const wxArrayString& GetCompilationArgs() const {
        return _compilationArgs;
    }
    CXIndex GetIndex() {
        return _index;
    }
    WorkingContext GetContext() const {
        return _context;
    }
};

////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
class CacheReturner;
class ClangWorkerThread : public WorkerThread
{
    friend class CacheReturner;
protected:
    wxCriticalSection m_criticalSection;
    ClangTUCache     m_cache;

public:
    ClangWorkerThread();
    virtual ~ClangWorkerThread();

protected:
    char**             MakeCommandLine(ClangThreadRequest* req, int& argc, FileExtManager::FileType fileType);
    void               DoCacheResult(CXTranslationUnit TU, const wxString &filename, const wxString &pch);
    void DoSetStatusMsg(const wxString &msg);
    bool               DoGotoDefinition(CXTranslationUnit& TU, ClangThreadRequest* request, ClangThreadReply* reply);
    void               PostEvent(int type);
    CXTranslationUnit  DoCreateTU(CXIndex index, ClangThreadRequest *task, bool reparse);
public:
    virtual void ProcessRequest(ThreadRequest* task);
    ClangCacheEntry findEntry(const wxString &filename);
    void              ClearCache();
    bool              IsCacheEmpty();
};

////////////////////////////////////////////////////////////
// A helper class that makes sure that the
// TU is properly cached when done using
class CacheReturner
{
public:
    ClangWorkerThread* m_thr;
    wxString           m_filename;
    CXTranslationUnit  m_tu;
	wxString           m_pch;
	
    CacheReturner(ClangWorkerThread* thr, const wxString &filename, const wxString &pch, CXTranslationUnit TU)
        : m_thr(thr)
        , m_filename(filename.c_str())
        , m_tu(TU)
		, m_pch(pch.c_str())
    {}

    ~CacheReturner() {
        m_thr->DoCacheResult(m_tu, m_filename.c_str(), m_pch.c_str());
    }
};

#endif // HAS_LIBCLANG

#endif // CLANGPCHMAKERTHREAD_H
