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
#include "codelite_exports.h"

class TagsStorageSQLite;

/**
 * @class ParseThreadEventData
 * @author eran
 * @date 10/04/09
 * @file parse_thread.h
 * @brief
 */
class ParseThreadEventData
{
	wxString m_fileName;
	std::vector<std::pair<wxString, TagEntry> >  m_items;
public:
	ParseThreadEventData() {}
	~ParseThreadEventData() {}

	void SetFileName(const wxString& fileName) {
		this->m_fileName = fileName.c_str();
	}
	void SetItems(const std::vector<std::pair<wxString, TagEntry> >& items) {
		this->m_items = items;
	}
	const wxString& GetFileName() const {
		return m_fileName;
	}
	const std::vector<std::pair<wxString, TagEntry> >& GetItems() const {
		return m_items;
	}
};

/**
 * 
 */
class InterrestingMacrosEventData
{
	wxString m_filename;
	wxString m_macros;
	
public:
	InterrestingMacrosEventData() {}
	
	void SetFileName(const wxString& filename) { m_filename = filename.c_str(); }
	void SetMacros(const wxString& macros) { m_macros = macros.c_str(); }
	
	const wxString& GetFileName() const { return m_filename; }
	const wxString& GetMacros() const { return m_macros; }
};

/**
 * @class ParseRequest
 * @author eran
 * @date 10/04/09
 * @file parse_thread.h
 * @brief a class representing a parsing request
 */
class WXDLLIMPEXP_CL ParseRequest : public ThreadRequest
{
	wxString      _file;
	wxString      _dbfile;
	wxString      _tags;
	int           _type;

public:
	wxEvtHandler*            _evtHandler;
	std::vector<std::string> _workspaceFiles;
	bool                     _quickRetag;
public:
	enum {
		PR_FILESAVED,
		PR_PARSEINCLUDES,
		PR_PARSE_AND_STORE,
		PR_GET_INTERRESTING_MACROS
	};

public:

	// ctor/dtor
	ParseRequest() : _type (PR_FILESAVED), _evtHandler(NULL), _quickRetag(false) {}
	virtual ~ParseRequest() ;

	// accessors
	void setFile     (const wxString &file     );
	void setDbFile   (const wxString &dbfile   );
	void setTags     (const wxString &tags     );

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
	TagsStorageSQLite*               m_pDb;
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

	void       DoStoreTags   (const wxString &tags, const wxString &filename, int &count);
	TagTreePtr DoTreeFromTags(const wxString &tags, int &count);

private:

	/**
	 * Process request from the editor.
	 * \param request the request to process
	 */
	void ProcessRequest(ThreadRequest *request);
	/**
	 * Send an event to the window with an array of items that where changed.
	 * \param evtType Event type to send, one of:
	 * - wxEVT_CMD_DEL_SYMBOL_TREE_ITEMS
	 * - wxEVT_CMD_ADD_SYMBOL_TREE_ITEMS
	 * - wxEVT_CMD_UPD_SYMBOL_TREE_ITEMS
	 * \param items Vector of items that were modified/deleted/added
	 */
	void SendEvent(int evtType, const wxString &fileName, std::vector<std::pair<wxString, TagEntry> >  &items);

	/**
	 * @brief parse include files and retrieve a list of all
	 * include files that should be tagged and inserted into
	 * the external database
	 * @param filename
	 */
	void ParseIncludeFiles(const wxString &filename);

	void ProcessSimple        (ParseRequest *req);
	void ProcessIncludes      (ParseRequest *req);
	void ProcessParseAndStore (ParseRequest *req);
	void ProcessInterrestingMacros(ParseRequest *req);
	void GetFileListToParse(const wxString &filename, wxArrayString &arrFiles);
	void ParseAndStoreFiles(const wxArrayString &arrFiles, int initalCount);

	void FindIncludedFiles(ParseRequest *req);
};

class WXDLLIMPEXP_CL ParseThreadST 
{
public:
	static void Free();
	static ParseThread* Get();
};

/**
 * Holds information about events associated with SymbolTree object.
 */
class WXDLLIMPEXP_CL SymbolTreeEvent : public wxNotifyEvent
{
	std::vector<std::pair<wxString, TagEntry> >  m_items;
	wxString m_project;
	wxString m_fileName;

protected:
	/**
	 * @brief we provide our own 'copy' function for the event items
	 * this is to avoide ref counting of the wxString items
	 * @param items
	 */
	void CopyItems(const std::vector<std::pair<wxString, TagEntry> > & items) {
		m_items.clear();
		for (size_t i=0; i<items.size(); i++) {
			std::pair<wxString, TagEntry> p;
			p.first = items.at(i).first.c_str();
			p.second = items.at(i).second;
			m_items.push_back(p);
		}
	}


public:
	/**
	 * Constructor
	 * \param commandType Event type
	 * \param winid Window ID
	 * \param key Item key
	 * \param data Item data
	 */
	SymbolTreeEvent(std::vector<std::pair<wxString, TagEntry> >  &items, wxEventType commandType = wxEVT_NULL, int winid = 0)
			: wxNotifyEvent(commandType, winid) {
		CopyItems(items);
	}

	/**
	 * Construct event with project name
	 * \param project project name
	 * \param commandType Event type
	 * \param winid Window ID
	 */
	SymbolTreeEvent(const wxString& project, const wxString &fileName, wxEventType commandType = wxEVT_NULL, int winid = 0)
			: wxNotifyEvent(commandType, winid)
			, m_project(project.c_str())
			, m_fileName(fileName.c_str()) {
	}

	/**
	 * Copy constructor
	 * \param rhs Right hand side
	 */
	SymbolTreeEvent(const SymbolTreeEvent& rhs)
			: wxNotifyEvent(rhs.GetEventType(), rhs.GetId())
			, m_project(rhs.m_project.c_str())
			, m_fileName(rhs.m_fileName.c_str()) {
		CopyItems(rhs.m_items);
	}

	/**
	 * Clone method to allow the event to be posted between threads.
	 * \return
	 */
	wxEvent *Clone(void) const {
		return new SymbolTreeEvent(*this);
	}

	SymbolTreeEvent(wxEventType commandType = wxEVT_NULL, int winid = 0)
			: wxNotifyEvent(commandType, winid) {}

	std::vector<std::pair<wxString, TagEntry> >& GetItems() {
		return m_items;
	}

	const wxString& GetProject() const {
		return m_project;
	}

	const wxString& GetFileName() const {
		return m_fileName;
	}

	void SetFileName(const wxChar* fileName) {
		m_fileName = fileName;
	}
};


BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_CL, wxEVT_COMMAND_SYMBOL_TREE_UPDATE_ITEM, 50300)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_CL, wxEVT_COMMAND_SYMBOL_TREE_DELETE_ITEM, 50301)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_CL, wxEVT_COMMAND_SYMBOL_TREE_ADD_ITEM, 50302)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_CL, wxEVT_COMMAND_SYMBOL_TREE_DELETE_PROJECT, 50303)

END_DECLARE_EVENT_TYPES()

extern WXDLLIMPEXP_CL const wxEventType wxEVT_PARSE_THREAD_UPDATED_FILE_SYMBOLS;
extern WXDLLIMPEXP_CL const wxEventType wxEVT_PARSE_THREAD_MESSAGE;
extern WXDLLIMPEXP_CL const wxEventType wxEVT_PARSE_THREAD_SCAN_INCLUDES_DONE;
extern WXDLLIMPEXP_CL const wxEventType wxEVT_PARSE_THREAD_CLEAR_TAGS_CACHE;
extern WXDLLIMPEXP_CL const wxEventType wxEVT_PARSE_THREAD_RETAGGING_PROGRESS;
extern WXDLLIMPEXP_CL const wxEventType wxEVT_PARSE_THREAD_RETAGGING_COMPLETED;
extern WXDLLIMPEXP_CL const wxEventType wxEVT_PARSE_THREAD_INTERESTING_MACROS;

typedef void (wxEvtHandler::*SymbolTreeEventFunction)(SymbolTreeEvent&);

#define SymbolTreeEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(SymbolTreeEventFunction, &func)

#define EVT_SYMBOLTREE_ADD_ITEM(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_SYMBOL_TREE_ADD_ITEM, winid, SymbolTreeEventHandler(fn))

#define EVT_SYMBOLTREE_DELETE_ITEM(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_SYMBOL_TREE_DELETE_ITEM, winid, SymbolTreeEventHandler(fn))

#define EVT_SYMBOLTREE_UPDATE_ITEM(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_SYMBOL_TREE_UPDATE_ITEM, winid, SymbolTreeEventHandler(fn))

#define EVT_SYMBOLTREE_DELETE_PROJECT(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_SYMBOL_TREE_DELETE_PROJECT, winid, SymbolTreeEventHandler(fn))

#endif // CODELITE_PARSE_THREAD_H
