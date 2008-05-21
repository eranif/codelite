#ifndef CODELITE_PARSE_THREAD_H 
#define CODELITE_PARSE_THREAD_H

#include "entry.h"
#include "singleton.h"
#include <map>
#include <vector>
#include <memory>
#include <wx/stopwatch.h>
#include "worker_thread.h"

#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_CL
#endif // WXMAKINGDLL_CODELITE

class TagsDatabase;

class WXDLLIMPEXP_CL ParseRequest : public ThreadRequest 
{
	wxChar *_file;
	wxChar *_dbfile;
	wxChar *_tags;
public:
	
	// ctor/dtor
	ParseRequest() : _file(NULL), _dbfile(NULL), _tags(NULL) {}
	virtual ~ParseRequest() ;
	
	// accessors
	void setFile(const wxString &file) ;
	
	wxChar* getFile() { return _file; }
	
	void setDbFile(const wxString &dbfile) ;
	wxChar* getDbFile() { return _dbfile; }
	
	void setTags(const wxString &tags) ;
	wxChar* getTags() { return _tags; }
	
	// copy ctor
	ParseRequest(const ParseRequest& rhs) ;
	// assignment operator
	ParseRequest &operator=(const ParseRequest& rhs);
};

class WXDLLIMPEXP_CL ParseThread : public WorkerThread
{
	friend class Singleton<ParseThread>;
	std::auto_ptr<TagsDatabase> m_pDb;
	
	wxStopWatch m_watch;

private:
	/**
	 * Default constructor.
	 */
	ParseThread();

	/**
	 * Destructor.
	 */
	virtual ~ParseThread();

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
	void SendEvent(int evtType, std::vector<std::pair<wxString, TagEntry> >  &items);
};

typedef Singleton<ParseThread> ParseThreadST;

/**
 * Holds information about events associated with SymbolTree object.
 */
class WXDLLIMPEXP_CL SymbolTreeEvent : public wxNotifyEvent
{
	DECLARE_DYNAMIC_CLASS(SymbolTreeEvent)
	std::vector<std::pair<wxString, TagEntry> >  m_items;
	wxString m_project;

public:
	/**
	 * Constructor
	 * \param commandType Event type
	 * \param winid Window ID
	 * \param key Item key
	 * \param data Item data
	 */
	SymbolTreeEvent(std::vector<std::pair<wxString, TagEntry> >  &items, wxEventType commandType = wxEVT_NULL, int winid = 0)
		: wxNotifyEvent(commandType, winid)
		, m_items(items)
	{
	}

	/**
	 * Construct event with project name
	 * \param project project name
	 * \param commandType Event type
	 * \param winid Window ID
	 */
	SymbolTreeEvent(const wxString& project, wxEventType commandType = wxEVT_NULL, int winid = 0)
		: wxNotifyEvent(commandType, winid)
		, m_project(project)
	{
	}

	/**
	 * Copy constructor
	 * \param rhs Right hand side
	 */
	SymbolTreeEvent(const SymbolTreeEvent& rhs)
		: wxNotifyEvent(rhs.GetEventType(), rhs.GetId())
		, m_items(rhs.m_items)
		, m_project(rhs.m_project)
	{
	}

	/**
	 * Clone method to allow the event to be posted between threads.
	 * \return
	 */
	wxEvent *Clone(void) const { return new SymbolTreeEvent(*this); }

	SymbolTreeEvent(wxEventType commandType = wxEVT_NULL, int winid = 0)
		: wxNotifyEvent(commandType, winid)
	{}

	std::vector<std::pair<wxString, TagEntry> >& GetItems() { return m_items; }
	const wxString& GetProject() const { return m_project; }
};


BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_CL, wxEVT_COMMAND_SYMBOL_TREE_UPDATE_ITEM, 50300)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_CL, wxEVT_COMMAND_SYMBOL_TREE_DELETE_ITEM, 50301)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_CL, wxEVT_COMMAND_SYMBOL_TREE_ADD_ITEM, 50302)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_CL, wxEVT_COMMAND_SYMBOL_TREE_DELETE_PROJECT, 50303)

END_DECLARE_EVENT_TYPES()

extern const wxEventType wxEVT_PARSE_THREAD_UPDATED_FILE_SYMBOLS;


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
