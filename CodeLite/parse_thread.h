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

#include "entry.h"
#include "singleton.h"
#include <map>
#include <vector>
#include <memory>
#include <wx/stopwatch.h>
#include "worker_thread.h"
#include "procutils.h"
#include "tag_tree.h"
#include "istorage.h"
#include "codelite_exports.h"

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
    wxString _file;
    wxString _dbfile;
    wxString _tags;
    int      _type;

public:
    wxEvtHandler*            _evtHandler;
    std::vector<std::string> _workspaceFiles;
    bool                     _quickRetag;
    int                      _uid;

public:
    enum {
        PR_FILESAVED,
        PR_PARSEINCLUDES,
        PR_PARSE_AND_STORE,
        PR_DELETE_TAGS_OF_FILES,
        PR_PARSE_FILE_NO_INCLUDES,
        PR_PARSE_INCLUDE_STATEMENTS,
        PR_SUGGEST_HIGHLIGHT_WORDS,
    };

public:

    // ctor/dtor
    ParseRequest(wxEvtHandler *handler) : _type (PR_FILESAVED), _evtHandler(handler), _quickRetag(false), _uid(-1) {}
    virtual ~ParseRequest() ;

    // accessors
    void setFile  (const wxString &file     );
    void setDbFile(const wxString &dbfile   );
    void setTags  (const wxString &tags     );

    //Getters
    const wxString& getDbfile() const {
        return _dbfile;
    }

    const wxString& getFile() const {
        return _file;
    }

    const wxString& getTags() const {
        return _tags;
    }

    void setType(int _type) {
        this->_type = _type;
    }
    int getType() const {
        return _type;
    }
    // copy ctor
    ParseRequest(const ParseRequest& rhs) ;

    // assignment operator
    ParseRequest &operator=(const ParseRequest& rhs);
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
    wxStopWatch                 m_watch;
    wxArrayString               m_searchPaths;
    wxArrayString               m_excludePaths;
    bool                        m_crawlerEnabled;

public:
    void SetCrawlerEnabeld (bool b                    );
    void SetSearchPaths    (const wxArrayString &paths, const wxArrayString &exlucdePaths);
    void GetSearchPaths    (wxArrayString &paths, wxArrayString &excludePaths);
    bool IsCrawlerEnabled  ();
private:
    /**
     * Default constructor.
     */
    ParseThread();

    /**
     * Destructor.
     */
    virtual ~ParseThread();

    void       DoStoreTags   (const wxString &tags, const wxString &filename, int &count, ITagsStoragePtr db);
    TagTreePtr DoTreeFromTags(const wxString &tags, int &count);
    void DoNotifyReady(wxEvtHandler* caller, int requestType);

private:

    /**
     * Process request from the editor.
     * \param request the request to process
     */
    void ProcessRequest(ThreadRequest *request);
    /**
     * @brief parse include files and retrieve a list of all
     * include files that should be tagged and inserted into
     * the external database
     * @param filename
     */
    void ParseIncludeFiles(ParseRequest *req, const wxString &filename, ITagsStoragePtr db);

    void ProcessSimple            (ParseRequest *req);
    void ProcessIncludes          (ParseRequest *req);
    void ProcessParseAndStore     (ParseRequest *req);
    void ProcessDeleteTagsOfFiles (ParseRequest *req);
    void ProcessSimpleNoIncludes  (ParseRequest *req);
    void ProcessIncludeStatements (ParseRequest *req);
    void ProcessColourRequest     (ParseRequest *req);
    void GetFileListToParse(const wxString &filename, wxArrayString &arrFiles);
    void ParseAndStoreFiles(ParseRequest *req, const wxArrayString &arrFiles, int initalCount, ITagsStoragePtr db);

    void FindIncludedFiles(ParseRequest *req, std::set<wxString> *newSet);
};

class WXDLLIMPEXP_CL ParseThreadST
{
public:
    static void Free();
    static ParseThread* Get();
};

extern WXDLLIMPEXP_CL const wxEventType wxEVT_PARSE_THREAD_MESSAGE;
extern WXDLLIMPEXP_CL const wxEventType wxEVT_PARSE_THREAD_SCAN_INCLUDES_DONE;
extern WXDLLIMPEXP_CL const wxEventType wxEVT_PARSE_THREAD_CLEAR_TAGS_CACHE;
extern WXDLLIMPEXP_CL const wxEventType wxEVT_PARSE_THREAD_RETAGGING_PROGRESS;
extern WXDLLIMPEXP_CL const wxEventType wxEVT_PARSE_THREAD_RETAGGING_COMPLETED;
extern WXDLLIMPEXP_CL const wxEventType wxEVT_PARSE_INCLUDE_STATEMENTS_DONE;
extern WXDLLIMPEXP_CL const wxEventType wxEVT_PARSE_THREAD_READY;
extern WXDLLIMPEXP_CL const wxEventType wxEVT_PARSE_THREAD_SUGGEST_COLOUR_TOKENS;

#endif // CODELITE_PARSE_THREAD_H
