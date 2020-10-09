//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : parse_thread.h
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
#ifndef CODELITE_PARSE_THREAD_H
#define CODELITE_PARSE_THREAD_H

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "entry.h"
#include "istorage.h"
#include "procutils.h"
#include "singleton.h"
#include "tag_tree.h"
#include "tags_options_data.h"
#include "worker_thread.h"
#include <map>
#include <memory>
#include <vector>
#include <wx/stopwatch.h>

class ITagsStorage;

/**
 * @class ParseRequest
 * @author eran
 * @date 10/04/09
 * @file parse_thread.h
 * @brief a class representing a parsing request
 */
class WXDLLIMPEXP_CL ParseRequest : public ThreadRequest
{
protected:
    wxString _file;
    wxString _dbfile;
    wxString _tags;
    int _type;
    wxArrayString m_definitions;
    wxArrayString m_includePaths;
    wxEvtHandler* m_parent;
    wxArrayString _workspaceFiles;
    bool _quickRetag;
    int _uid;

public:
    enum {
        PR_INVALID = -1,
        PR_FILESAVED,
        PR_PARSEINCLUDES,
        PR_PARSE_AND_STORE,
        PR_DELETE_TAGS_OF_FILES,
        PR_PARSE_FILE_NO_INCLUDES,
        PR_PARSE_INCLUDE_STATEMENTS,
        PR_SOURCE_TO_TAGS,
    };

public:
    // ctor/dtor
    ParseRequest(wxEvtHandler* handler)
        : _type(PR_FILESAVED)
        , m_parent(handler)
        , _quickRetag(false)
        , _uid(-1)
    {
    }
    virtual ~ParseRequest();

    // accessors
    void SetDefinitions(const wxArrayString& definitions) { this->m_definitions = definitions; }
    void SetIncludePaths(const wxArrayString& includePaths) { this->m_includePaths = includePaths; }
    const wxArrayString& GetDefinitions() const { return m_definitions; }
    const wxArrayString& GetIncludePaths() const { return m_includePaths; }
    void setFile(const wxString& file);
    void setDbFile(const wxString& dbfile);
    void setTags(const wxString& tags);

    // Getters
    void SetDbfile(const wxString& _dbfile) { this->_dbfile = _dbfile; }
    void SetParent(wxEvtHandler* _evtHandler) { this->m_parent = _evtHandler; }
    void SetFile(const wxString& _file) { this->_file = _file; }
    void SetQuickRetag(bool _quickRetag) { this->_quickRetag = _quickRetag; }
    void SetTags(const wxString& _tags) { this->_tags = _tags; }
    void SetType(int _type) { this->_type = _type; }
    void SetUid(int _uid) { this->_uid = _uid; }
    void SetWorkspaceFiles(const wxArrayString& _workspaceFiles) { this->_workspaceFiles = _workspaceFiles; }
    const wxString& GetDbfile() const { return _dbfile; }
    wxEvtHandler* GetParent() { return m_parent; }
    const wxString& GetFile() const { return _file; }
    bool IsQuickRetag() const { return _quickRetag; }
    const wxString& GetTags() const { return _tags; }
    int GetType() const { return _type; }
    int GetUid() const { return _uid; }

    const wxArrayString& GetWorkspaceFiles() const { return _workspaceFiles; }
    // copy ctor
    ParseRequest(const ParseRequest& rhs);
    // assignment operator
    ParseRequest& operator=(const ParseRequest& rhs);
};

/**
 * @class ParseThread
 * @author eran
 * @date 10/04/09
 * @file parse_thread.h
 * @brief
 */
class WXDLLIMPEXP_CL ParseThread : public WorkerThread
{
    friend class ParseThreadST;
    wxStopWatch m_watch;
    wxArrayString m_searchPaths;
    wxArrayString m_excludePaths;
    bool m_crawlerEnabled;
    wxCriticalSection m_cs;
    TagsOptionsData m_tod;

public:
    void SetCrawlerEnabeld(bool b);
    void SetSearchPaths(const wxArrayString& paths, const wxArrayString& exlucdePaths);
    void GetSearchPaths(wxArrayString& paths, wxArrayString& excludePaths);
    bool IsCrawlerEnabled();
    /**
     * @brief add search paths to the parser. This method is thread safe
     */
    void AddPaths(const wxArrayString& inc, const wxArrayString& exc);

    /**
     * @brief clear the current search paths
     */
    void ClearPaths();

private:
    /**
     * Default constructor.
     */
    ParseThread();

    /**
     * @brief post message to the parent window
     */
    void PostStatusMessage(wxEvtHandler* parent, const wxString& message);

    /**
     * Destructor.
     */
    virtual ~ParseThread();

    void DoStoreTags(const wxString& tags, const wxString& filename, int& count, ITagsStoragePtr db);
    TagTreePtr DoTreeFromTags(const wxString& tags, int& count);
    void DoNotifyReady(wxEvtHandler* caller, int requestType);

private:
    /**
     * Process request from the editor.
     * \param request the request to process
     */
    void ProcessRequest(ThreadRequest* request);
    /**
     * @brief parse include files and retrieve a list of all
     * include files that should be tagged and inserted into
     * the external database
     * @param filename
     */
    void ParseIncludeFiles(ParseRequest* req, const wxString& filename, ITagsStoragePtr db);
    void FilterBinaryFiles(wxArrayString& files);
    void ProcessSingleFile(ParseRequest* req);
    void ProcessIncludes(ParseRequest* req);
    void ProcessParseAndStore(ParseRequest* req);
    void ProcessDeleteTagsOfFiles(ParseRequest* req);
    void ProcessSimpleNoIncludes(ParseRequest* req);
    void ProcessIncludeStatements(ParseRequest* req);
    void GetFileListToParse(const wxString& filename, wxArrayString& arrFiles);
    void ParseAndStoreFiles(wxEvtHandler* parent, const wxArrayString& arrFiles, ITagsStoragePtr db);
    void FindIncludedFiles(ParseRequest* req, wxArrayString& files);
};

class WXDLLIMPEXP_CL ParseThreadST
{
public:
    static void Free();
    static ParseThread* Get();
};

// Parse thread event
class WXDLLIMPEXP_CL clParseThreadEvent : public clCommandEvent
{
    wxArrayString m_files;
    bool m_quickRetag = false;
    int m_progressPercentage = 0;
    int m_requestUID = 0;
    wxEvtHandler* m_caller = nullptr;
    int m_requestType = ParseRequest::PR_INVALID;

private:
    // Make GetInt private
    int GetInt() const { return m_commandInt; }

public:
    clParseThreadEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clParseThreadEvent(const clParseThreadEvent& event);
    clParseThreadEvent& operator=(const clParseThreadEvent& src);
    virtual ~clParseThreadEvent();
    virtual wxEvent* Clone() const { return new clParseThreadEvent(*this); }
    const wxArrayString& GetFiles() const { return m_files; }
    void SetFiles(const wxArrayString& files) { m_files = files; }
    bool IsQuickParse() const { return m_quickRetag; }
    void SetQuickParse(bool b) { m_quickRetag = b; }
    int GetProgressPercentage() const { return m_progressPercentage; }
    void SetProgressPercentage(int p) { m_progressPercentage = p; }
    int GetRequestUID() const { return m_requestUID; }
    void SetRequestUID(int uid) { m_requestUID = uid; }
    void SetCaller(wxEvtHandler* caller) { m_caller = caller; }
    wxEvtHandler* GetCaller() const { return m_caller; }
    void SetRequestType(int requestType) { m_requestType = requestType; }
    int GetRequestType() const { return m_requestType; }
};

typedef void (wxEvtHandler::*clParseThreadEventFunction)(clParseThreadEvent&);
#define clParseThreadEventHandler(func) wxEVENT_HANDLER_CAST(clParseThreadEventFunction, func)

// ClientData is set to wxString* which must be deleted by the handler
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxPARSE_THREAD_MESSAGE, clParseThreadEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxPARSE_THREAD_SCAN_INCLUDES_DONE, clParseThreadEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxPARSE_THREAD_CLEAR_TAGS_CACHE, clParseThreadEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxPARSE_THREAD_RETAGGING_PROGRESS, clParseThreadEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxPARSE_THREAD_RETAGGING_COMPLETED, clParseThreadEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxPARSE_INCLUDE_STATEMENTS_DONE, clParseThreadEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxPARSE_THREAD_READY, clParseThreadEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxPARSE_THREAD_SUGGEST_COLOUR_TOKENS, clParseThreadEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxPARSE_THREAD_SOURCE_TAGS, clParseThreadEvent);

#endif // CODELITE_PARSE_THREAD_H
