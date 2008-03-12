#ifndef SEARCH_THREAD_H
#define SEARCH_THREAD_H

#include <deque>
#include <list>
#include <wx/string.h>
#include <map>
#include "singleton.h"
#include "wx/event.h"
#include "wx/filename.h"
#include <wx/regex.h>
#include "worker_thread.h"

// Possible search data options:
enum {
	wxSD_MATCHCASE         = 1,
	wxSD_MATCHWHOLEWORD    = 2,
	wxSD_REGULAREXPRESSION = 4	
};

class wxEvtHandler;

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK
//----------------------------------------------------------
// The searched data class to be passed to the search thread
//----------------------------------------------------------

class WXDLLIMPEXP_LE_SDK SearchData : public ThreadRequest
{
	wxString m_rootDir;
	wxString m_findString;
	int m_flags;
	wxString m_validExt;
	wxArrayString m_files;
	int m_outputTab;
	
private:
	// An internal helper function that set/remove an option bit
	void SetOption(int option, bool set){
		if(set){
			m_flags |= option;
		} else {
			m_flags &= ~(option);
		}
	}

public:
	// Ctor-Dtor
	SearchData() 
		: ThreadRequest()
		, m_rootDir(wxEmptyString)
		, m_findString(wxEmptyString) 
		, m_flags(0)
		, m_outputTab(0)
	{}

	SearchData(const SearchData &rhs){
		*this = rhs;
	}

	virtual ~SearchData()
	{}

	SearchData& operator=(const SearchData &rhs){
		if(this == &rhs)
			return *this;

		m_findString = rhs.m_findString;
		m_flags = rhs.m_flags;
		m_validExt = rhs.m_validExt;
		m_rootDir = rhs.m_rootDir;
		m_files = rhs.m_files;
		m_outputTab = rhs.m_outputTab;
		return *this;
	}
	
public:
	//------------------------------------------
	// Setters / Getters
	//------------------------------------------
	bool IsMatchCase() const { return m_flags & wxSD_MATCHCASE ? true : false; }
	bool IsMatchWholeWord() const { return m_flags & wxSD_MATCHWHOLEWORD ? true : false; }
	bool IsRegularExpression() const { return m_flags & wxSD_REGULAREXPRESSION ? true : false; }
	const wxString &GetRootDir() const { return m_rootDir; }
	void SetMatchCase(bool matchCase){ SetOption(wxSD_MATCHCASE, matchCase); }
	void SetMatchWholeWord(bool matchWholeWord){ SetOption(wxSD_MATCHWHOLEWORD, matchWholeWord); }
	void SetRegularExpression(bool re){ SetOption(wxSD_REGULAREXPRESSION, re); }
	void SetExtensions(const wxString &exts) { m_validExt = exts; }
	void SetRootDir(const wxString &rootDir) { m_rootDir = rootDir; }
	const wxString& GetExtensions() const;
	const wxString &GetFindString() const { return m_findString; }
	void SetFindString(const wxString &findString){ m_findString = findString; }
	void SetFiles(const wxArrayString &files){m_files = files;}
	const wxArrayString &GetFiles() const{return m_files;}
	void SetOutputTab(const int& outputTab) {this->m_outputTab = outputTab;}
	const int& GetOutputTab() const {return m_outputTab;}
};

//------------------------------------------
// class containing the search result
//------------------------------------------
class WXDLLIMPEXP_LE_SDK SearchResult : public wxObject {
	wxString m_pattern;
	int m_lineNumber;
	int m_column;
	wxString m_fileName;

public:
	//ctor-dtor, copy constructor and assignment operator
	SearchResult(){}

	virtual ~SearchResult(){}

	SearchResult(const SearchResult &rhs) { *this = rhs; }

	SearchResult& operator=(const SearchResult &rhs){
		if(this == &rhs)
			return *this;
		m_column = rhs.m_column;
		m_lineNumber = rhs.m_lineNumber;
		m_pattern = rhs.m_pattern;
		m_fileName = rhs.m_fileName;
		return *this;
	}
	
	//------------------------------------------------------
	// Setters/getters
	void SetPattern(const wxString &pat){ m_pattern = pat;}
	void SetLineNumber(const int &line){ m_lineNumber = line; }
	void SetColumn(const int &col){ m_column = col;}
	void SetFileName(const wxString &fileName) { m_fileName = fileName; }

	const int& GetLineNumber() const { return m_lineNumber; }
	const int& GetColumn() const { return m_column; }
	const wxString &GetPattern() const { return m_pattern; }
	const wxString &GetFileName() const { return m_fileName; }

	// return a foramtted message
	wxString GetMessage() const 
	{
		wxString msg;
		msg << GetFileName()
			<< wxT("(")
			<< GetLineNumber() 
			<< wxT(",")
			<< GetColumn()
			<< wxT("): ")
			<< GetPattern();
		return msg;
	}
};

typedef std::list<SearchResult> SearchResultList;


class WXDLLIMPEXP_LE_SDK SearchSummary : public wxObject {
	int m_fileScanned;
	int m_matchesFound;
	int m_elapsed;

public:
	SearchSummary() 
		: m_fileScanned(0)
		, m_matchesFound(0) 
		, m_elapsed(0)
	{
	}

	virtual ~SearchSummary()
	{}

	SearchSummary(const SearchSummary& rhs)
	{
		*this = rhs;
	}

	SearchSummary& operator=(const SearchSummary &rhs)
	{
		if(this == &rhs)
			return *this;

		m_fileScanned = rhs.m_fileScanned;
		m_matchesFound = rhs.m_matchesFound;
		m_elapsed = rhs.m_elapsed;
		return *this;
	}

	int GetNumFileScanned() const { return m_fileScanned; }
	int GetNumMatchesFound() const { return m_matchesFound; }
	
	void SetNumFileScanned(const int &num) { m_fileScanned = num; }
	void SetNumMatchesFound(const int &num) { m_matchesFound = num; }
	void SetElapsedTime(long elapsed) {m_elapsed = elapsed;}
	wxString GetMessage() const { 
		wxString msg(wxT("====== Number of files scanned: "));
		msg << m_fileScanned << wxT(",");
		msg << wxT(" Matches found: ");
		msg << m_matchesFound;
		int secs  = m_elapsed / 1000;
		int msecs = m_elapsed % 1000;

		msg << wxT(", elapsed time: ") << secs << wxT(":") << msecs << wxT(" sec ======"); 
		return msg;
	}

};

//----------------------------------------------------------
// The search thread 
//----------------------------------------------------------

class WXDLLIMPEXP_LE_SDK SearchThread : public WorkerThread
{
	friend class Singleton<SearchThread>;
	wxString m_wordChars;
	std::map<wxChar, bool> m_wordCharsMap; //< Internal
	SearchResultList m_results;
	bool m_stopSearch;
	SearchSummary m_summary;
	wxString m_reExpr;
	wxRegEx m_regex;
	bool m_matchCase;

private:
	/**
	 * Default constructor.
	 */
	SearchThread();

	/**
	 * Destructor.
	 */
	virtual ~SearchThread();

public:
	/**
	 * Process request from caller
	 */
	void ProcessRequest(ThreadRequest *req);

	/**
	 * Add a request to the search thread to start
	 * \param data SearchData class 
	 */
	void PerformSearch(const SearchData &data);

	/**
	 * Stops the current search operation
	 * \note This call must be called from the context of other thread (e.g. main thread)
	 */
	void StopSearch(bool stop = true);

	/**
	 *  The search thread has several functions that operate on words, 
	 *  which are defined to be contiguous sequences of characters from a particular set of characters. 
	 *  Defines which characters are members of that set. The default is set to:
	 * "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"
	 * \param chars sequence of characters that are considered part of a word
	 */
	void SetWordChars(const wxString &chars);

private:

	/**
	 * Return files to search 
	 * \param files output
	 * \param data search data
	 */
	void GetFiles(const SearchData *data, wxArrayString &files);

	/**
	 * Index the word chars from the array into a map
	 */
	void IndexWordChars();

	// Test to see if user asked to cancel the search
	bool TestStopSearch();

	/**
	 * Do the actual search operation
	 * \param data inpunt contains information about the search
	 */
	void DoSearchFiles(ThreadRequest *data);

	// Perform search on a single file
	void DoSearchFile(const wxString &fileName, const SearchData *data);
	
	// Perform search on a line
	void DoSearchLine(const wxString &line, const int lineNum, const wxString &fileName, const SearchData *data);

	// Perform search on a line using regular expression
	void DoSearchLineRE(const wxString &line, const int lineNum, const wxString &fileName, const SearchData *data);

	// Send an event to the notified window
	void SendEvent(wxEventType type);

	// return a compiled regex object for the expression
	wxRegEx &GetRegex(const wxString &expr, bool matchCase);

	// Internal function 
	bool AdjustLine(wxString &line, int &pos, wxString &findString);

};

typedef Singleton<SearchThread> SearchThreadST;

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_LE_SDK, wxEVT_SEARCH_THREAD_MATCHFOUND, wxID_ANY)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_LE_SDK, wxEVT_SEARCH_THREAD_SEARCHEND, wxID_ANY)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_LE_SDK, wxEVT_SEARCH_THREAD_SEARCHCANCELED, wxID_ANY)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_LE_SDK, wxEVT_SEARCH_THREAD_SEARCHSTARTED, wxID_ANY)
END_DECLARE_EVENT_TYPES()

#endif // SEARCH_THREAD_H

