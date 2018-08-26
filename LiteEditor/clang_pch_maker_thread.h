//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : clang_pch_maker_thread.h
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

#ifndef CLANGPCHMAKERTHREAD_H
#define CLANGPCHMAKERTHREAD_H

#if HAS_LIBCLANG

#include "clangpch_cache.h"
#include "fileextmanager.h"
#include "worker_thread.h" // Base class: ThreadRequest
#include <clang-c/Index.h>
#include <map>
#include <set>

#define CODELITE_CLANG_FILE_PREFIX "codelite_clang_"

extern const wxEventType wxEVT_CLANG_PCH_CACHE_STARTED;
extern const wxEventType wxEVT_CLANG_PCH_CACHE_ENDED;
extern const wxEventType wxEVT_CLANG_PCH_CACHE_CLEARED;
extern const wxEventType wxEVT_CLANG_TU_CREATE_ERROR;

typedef std::map<FileExtManager::FileType, wxArrayString> FileTypeCmpArgs_t;

enum WorkingContext {
    CTX_None = -1,
    CTX_CodeCompletion,
    CTX_Calltip,
    CTX_CachePCH,
    CTX_WordCompletion,
    CTX_ReparseTU,
    CTX_GotoDecl,
    CTX_GotoImpl
};

struct ClangThreadReply {
    wxString filterWord;
    WorkingContext context;
    wxString filename;
    wxString macrosAsString;
    CXCodeCompleteResults* results;
    wxString errorMessage;
    unsigned line;
    unsigned col;

    ClangThreadReply()
        : context(CTX_None)
        , results(NULL)
        , line(0)
        , col(0)
    {
    }
};

class ClangThreadRequest : public ThreadRequest
{
public:
    typedef std::list<std::pair<wxString, wxString> > List_t;

private:
    CXIndex _index;
    wxString _fileName;
    wxString _dirtyBuffer;
    FileTypeCmpArgs_t _compilationArgs;
    wxString _filterWord;
    WorkingContext _context;
    unsigned _line;
    unsigned _column;
    List_t _modifiedBuffers;

public:
    ClangThreadRequest(CXIndex index, const wxString& filename, const wxString& dirtyBuffer,
                       const FileTypeCmpArgs_t& compArgs, const wxString& filterWord, WorkingContext context,
                       unsigned line, unsigned column, const List_t& modifiedBuffers)
        : _index(index)
        , _fileName(filename.c_str())
        , _dirtyBuffer(dirtyBuffer.c_str())
        , _filterWord(filterWord)
        , _context(context)
        , _line(line)
        , _column(column)
    {
        // Perform a deep copy of the map (as wxWidgets is not known for its wxString thread safety)
        FileTypeCmpArgs_t::const_iterator iter = compArgs.begin();
        for(; iter != compArgs.end(); ++iter) {
            const wxArrayString& opts = iter->second;
            FileExtManager::FileType type = iter->first;

            _compilationArgs.insert(std::make_pair(type, wxArrayString()));
            wxArrayString& arr = _compilationArgs[type];

            for(size_t i = 0; i < opts.GetCount(); i++) {
                arr.Add(opts.Item(i).c_str());
            }
        }

        // Copy the modified buffers
        _modifiedBuffers.clear();
        List_t::const_iterator listIter = modifiedBuffers.begin();
        for(; listIter != modifiedBuffers.end(); ++listIter) {
            _modifiedBuffers.push_back(std::make_pair(listIter->first.c_str(), listIter->second.c_str()));
        }
    }

    void SetFileName(const wxString& filename) { this->_fileName = filename; }

    unsigned GetColumn() const { return _column; }
    const wxString& GetFilterWord() const { return _filterWord; }
    unsigned GetLine() const { return _line; }
    virtual ~ClangThreadRequest() {}
    const wxString& GetDirtyBuffer() const { return _dirtyBuffer; }
    const wxString& GetFileName() const { return _fileName; }
    const FileTypeCmpArgs_t& GetCompilationArgs() const { return _compilationArgs; }
    CXIndex GetIndex() { return _index; }
    WorkingContext GetContext() const { return _context; }
    const List_t& GetModifiedBuffers() const { return _modifiedBuffers; }
};

////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
class CacheReturner;
class ClangWorkerThread : public WorkerThread
{
    friend class CacheReturner;

protected:
    wxCriticalSection m_criticalSection;
    ClangTUCache m_cache;

public:
    ClangWorkerThread();
    virtual ~ClangWorkerThread();

protected:
    char** MakeCommandLine(ClangThreadRequest* req, int& argc, FileExtManager::FileType fileType);
    void DoCacheResult(ClangCacheEntry entry);
    void DoSetStatusMsg(const wxString& msg);
    bool DoGotoDefinition(CXTranslationUnit& TU, ClangThreadRequest* request, ClangThreadReply* reply);
    void PostEvent(int type, const wxString& fileName);
    CXTranslationUnit DoCreateTU(CXIndex index, ClangThreadRequest* task, bool reparse);

public:
    virtual void ProcessRequest(ThreadRequest* task);
    ClangCacheEntry findEntry(const wxString& filename);
    void ClearCache();
    bool IsCacheEmpty();
};

////////////////////////////////////////////////////////////
// A helper class that makes sure that the
// TU is properly cached when done using
class CacheReturner
{
public:
    ClangWorkerThread* m_thr;
    ClangCacheEntry& m_entry;
    bool m_cancelled;

    CacheReturner(ClangWorkerThread* thr, ClangCacheEntry& cacheEntry)
        : m_thr(thr)
        , m_entry(cacheEntry)
        , m_cancelled(false)
    {
    }

    ~CacheReturner()
    {
        if(!IsCancelled()) { m_thr->DoCacheResult(m_entry); }
    }

    void SetCancelled(bool cancelled) { this->m_cancelled = cancelled; }
    bool IsCancelled() const { return m_cancelled; }
};

#endif // HAS_LIBCLANG

#endif // CLANGPCHMAKERTHREAD_H
