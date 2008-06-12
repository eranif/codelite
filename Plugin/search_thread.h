//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : search_thread.h
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
#include "stringsearcher.h"

class wxEvtHandler;
class SearchResult;
class SearchThread;

//----------------------------------------------------------
// The searched data class to be passed to the search thread
//----------------------------------------------------------

class SearchData : public ThreadRequest
{
	wxString m_rootDir;
	wxString m_findString;
	size_t m_flags;
	wxString m_validExt;
	wxArrayString m_files;
	int m_outputTab;
	
	friend class SearchThread;
	
private:
	// An internal helper function that set/remove an option bit
	void SetOption(int option, bool set) {
		if (set) {
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
			, m_outputTab(0) {}

	SearchData(const SearchData &rhs) {
		*this = rhs;
	}

	virtual ~SearchData() {}

	SearchData& operator=(const SearchData &rhs) {
		if (this == &rhs)
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
	bool IsMatchCase() const {
		return m_flags & wxSD_MATCHCASE ? true : false;
	}

	bool IsMatchWholeWord() const {
		return m_flags & wxSD_MATCHWHOLEWORD ? true : false;
	}

	bool IsRegularExpression() const {
		return m_flags & wxSD_REGULAREXPRESSION ? true : false;
	}

	const wxString &GetRootDir() const {
		return m_rootDir;
	}

	void SetMatchCase(bool matchCase) {
		SetOption(wxSD_MATCHCASE, matchCase);
	}

	void SetMatchWholeWord(bool matchWholeWord) {
		SetOption(wxSD_MATCHWHOLEWORD, matchWholeWord);
	}

	void SetRegularExpression(bool re) {
		SetOption(wxSD_REGULAREXPRESSION, re);
	}

	void SetExtensions(const wxString &exts) {
		m_validExt = exts;
	}

	void SetRootDir(const wxString &rootDir) {
		m_rootDir = rootDir;
	}

	const wxString& GetExtensions() const;

	const wxString &GetFindString() const {
		return m_findString;
	}

	void SetFindString(const wxString &findString) {
		m_findString = findString;
	}

	void SetFiles(const wxArrayString &files) {
		m_files = files;
	}

	const wxArrayString &GetFiles() const {
		return m_files;
	}

	void SetOutputTab(const int& outputTab) {
		this->m_outputTab = outputTab;
	}

	const int& GetOutputTab() const {
		return m_outputTab;
	}
	
	bool UseEditorFontConfig() const {
		return m_flags & wxSD_USE_EDITOR_ENCODING ? true : false;
	}
	
	void SetUseEditorFontConfig(bool use) {
		SetOption(wxSD_USE_EDITOR_ENCODING, use);
	}
};

//------------------------------------------
// class containing the search result
//------------------------------------------
class SearchResult : public wxObject
{
	wxString m_pattern;
	int m_lineNumber;
	int m_column;
	wxString m_fileName;
	int m_len;
	wxString m_findWhat;
	size_t m_flags;
	int m_columnInChars;
	int m_lenInChars;

public:
	//ctor-dtor, copy constructor and assignment operator
	SearchResult() {}

	virtual ~SearchResult() {}

	SearchResult(const SearchResult &rhs) {
		*this = rhs;
	}

	SearchResult& operator=(const SearchResult &rhs) {
		if (this == &rhs)
			return *this;
			
		m_column = rhs.m_column;
		m_lineNumber = rhs.m_lineNumber;
		m_pattern = rhs.m_pattern.c_str();
		m_fileName = rhs.m_fileName.c_str();
		m_len = rhs.m_len;
		m_findWhat = rhs.m_findWhat.c_str();
		m_flags = rhs.m_flags;
		m_columnInChars = rhs.m_columnInChars;
		m_lenInChars = rhs.m_lenInChars;
		return *this;
	}

	//------------------------------------------------------
	// Setters/getters

	void SetFlags(const size_t& flags) {
		this->m_flags = flags;
	}
	
	const size_t& GetFlags() const {
		return m_flags;
	}

	void SetPattern(const wxString &pat) {
		m_pattern = pat.c_str();
	}
	void SetLineNumber(const int &line) {
		m_lineNumber = line;
	}
	void SetColumn(const int &col) {
		m_column = col;
	}
	void SetFileName(const wxString &fileName) {
		m_fileName = fileName.c_str();
	}

	const int& GetLineNumber() const {
		return m_lineNumber;
	}
	const int& GetColumn() const {
		return m_column;
	}
	const wxString &GetPattern() const {
		return m_pattern;
	}
	const wxString &GetFileName() const {
		return m_fileName;
	}


	void SetLen(const int& len) {
		this->m_len = len;
	}
	const int& GetLen() const {
		return m_len;
	}


	//Setters
	void SetFindWhat(const wxString& findWhat) {
		this->m_findWhat = findWhat.c_str();
	}
	//Getters
	const wxString& GetFindWhat() const {
		return m_findWhat;
	}
	
	void SetColumnInChars(const int& col) {
		this->m_columnInChars = col;
	}
	const int& GetColumnInChars() const {
		return m_columnInChars;
	}
	
	void SetLenInChars(const int& len) {
		this->m_lenInChars = len;
	}
	const int& GetLenInChars() const {
		return m_lenInChars;
	}

	// return a foramtted message
	wxString GetMessage() const {
		wxString msg;
		msg << GetFileName()
		<< wxT("(")
		<< GetLineNumber()
		<< wxT(",")
		<< GetColumn()
		<< wxT(",")
		<< GetLen()
		<< wxT("): ")
		<< GetPattern();
		return msg;
	}
};

typedef std::list<SearchResult> SearchResultList;


class SearchSummary : public wxObject
{
	int m_fileScanned;
	int m_matchesFound;
	int m_elapsed;

public:
	SearchSummary()
			: m_fileScanned(0)
			, m_matchesFound(0)
			, m_elapsed(0) {
	}

	virtual ~SearchSummary() {}

	SearchSummary(const SearchSummary& rhs) {
		*this = rhs;
	}

	SearchSummary& operator=(const SearchSummary &rhs) {
		if (this == &rhs)
			return *this;

		m_fileScanned = rhs.m_fileScanned;
		m_matchesFound = rhs.m_matchesFound;
		m_elapsed = rhs.m_elapsed;
		return *this;
	}

	int GetNumFileScanned() const {
		return m_fileScanned;
	}
	int GetNumMatchesFound() const {
		return m_matchesFound;
	}

	void SetNumFileScanned(const int &num) {
		m_fileScanned = num;
	}
	void SetNumMatchesFound(const int &num) {
		m_matchesFound = num;
	}
	void SetElapsedTime(long elapsed) {
		m_elapsed = elapsed;
	}
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

class SearchThread : public WorkerThread
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

extern const wxEventType wxEVT_SEARCH_THREAD_MATCHFOUND;
extern const wxEventType wxEVT_SEARCH_THREAD_SEARCHEND;
extern const wxEventType wxEVT_SEARCH_THREAD_SEARCHCANCELED;
extern const wxEventType wxEVT_SEARCH_THREAD_SEARCHSTARTED;

#endif // SEARCH_THREAD_H
