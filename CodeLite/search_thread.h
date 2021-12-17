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

#include "JSON.h"
#include "codelite_exports.h"
#include "singleton.h"
#include "worker_thread.h"
#include "wx/event.h"
#include "wx/filename.h"
#include "wxStringHash.h"
#include <deque>
#include <list>
#include <map>
#include <wx/regex.h>
#include <wx/string.h>

class wxEvtHandler;
class SearchResult;
class SearchThread;

//----------------------------------------------------------
// The searched data class to be passed to the search thread
//----------------------------------------------------------
// Possible search data options:
enum {
    wxSD_MATCHCASE = 0x00000001,
    wxSD_MATCHWHOLEWORD = 0x00000002,
    wxSD_REGULAREXPRESSION = 0x00000004,
    wxSD_SEARCH_BACKWARD = 0x00000008,
    wxSD_USE_EDITOR_ENCODING = 0x00000010,
    wxSD_PRINT_SCOPE = 0x00000020,
    wxSD_SKIP_COMMENTS = 0x00000040,
    wxSD_SKIP_STRINGS = 0x00000080,
    wxSD_COLOUR_COMMENTS = 0x00000100,
    wxSD_WILDCARD = 0x00000200,
    wxSD_ENABLE_PIPE_SUPPORT = 0x00000400,
};

class WXDLLIMPEXP_CL SearchData : public ThreadRequest
{
    wxArrayString m_rootDirs;
    wxString m_findString;
    wxString m_replaceWith;
    size_t m_flags;
    wxString m_validExt;
    wxArrayString m_files;
    bool m_newTab;
    wxEvtHandler* m_owner;
    wxString m_encoding;
    wxArrayString m_excludePatterns;
    friend class SearchThread;

private:
    // An internal helper function that set/remove an option bit
    void SetOption(int option, bool set)
    {
        if(set) {
            m_flags |= option;
        } else {
            m_flags &= ~(option);
        }
    }

public:
    // Ctor-Dtor
    SearchData()
        : ThreadRequest()
        , m_findString(wxEmptyString)
        , m_flags(0)
        , m_newTab(false)
        , m_owner(NULL)
    {
    }

    SearchData(const SearchData& rhs) { Copy(rhs); }
    SearchData& operator=(const SearchData& rhs);

    virtual ~SearchData() {}
    SearchData& Copy(const SearchData& other);

public:
    //------------------------------------------
    // Setters / Getters
    //------------------------------------------
    bool IsMatchCase() const { return m_flags & wxSD_MATCHCASE ? true : false; }
    bool IsEnablePipeSupport() const { return m_flags & wxSD_ENABLE_PIPE_SUPPORT; }
    void SetEnablePipeSupport(bool b) { SetOption(wxSD_ENABLE_PIPE_SUPPORT, b); }
    bool IsMatchWholeWord() const { return m_flags & wxSD_MATCHWHOLEWORD ? true : false; }
    bool IsRegularExpression() const { return m_flags & wxSD_REGULAREXPRESSION ? true : false; }
    const wxArrayString& GetRootDirs() const { return m_rootDirs; }
    void SetMatchCase(bool matchCase) { SetOption(wxSD_MATCHCASE, matchCase); }
    void SetMatchWholeWord(bool matchWholeWord) { SetOption(wxSD_MATCHWHOLEWORD, matchWholeWord); }
    void SetRegularExpression(bool re) { SetOption(wxSD_REGULAREXPRESSION, re); }
    void SetExtensions(const wxString& exts) { m_validExt = exts; }
    void SetRootDirs(const wxArrayString& rootDirs) { m_rootDirs = rootDirs; }
    const wxString& GetExtensions() const;
    const wxString& GetFindString() const { return m_findString; }
    void SetFindString(const wxString& findString) { m_findString = findString; }
    void SetFiles(const wxArrayString& files) { m_files = files; }
    const wxArrayString& GetFiles() const { return m_files; }
    void SetExcludePatterns(const wxArrayString& excludePatterns) { this->m_excludePatterns = excludePatterns; }
    const wxArrayString& GetExcludePatterns() const { return m_excludePatterns; }
    void UseNewTab(bool useNewTab) { m_newTab = useNewTab; }
    bool UseNewTab() const { return m_newTab; }
    void SetEncoding(const wxString& encoding) { this->m_encoding = encoding.c_str(); }
    const wxString& GetEncoding() const { return this->m_encoding; }
    bool GetDisplayScope() const { return m_flags & wxSD_PRINT_SCOPE ? true : false; }
    void SetDisplayScope(bool d) { SetOption(wxSD_PRINT_SCOPE, d); }
    void SetOwner(wxEvtHandler* owner) { this->m_owner = owner; }
    wxEvtHandler* GetOwner() const { return m_owner; }
    bool HasCppOptions() const
    {
        return (m_flags & wxSD_SKIP_COMMENTS) || (m_flags & wxSD_SKIP_STRINGS) || (m_flags & wxSD_COLOUR_COMMENTS);
    }

    void SetSkipComments(bool d) { SetOption(wxSD_SKIP_COMMENTS, d); }
    void SetSkipStrings(bool d) { SetOption(wxSD_SKIP_STRINGS, d); }
    void SetColourComments(bool d) { SetOption(wxSD_COLOUR_COMMENTS, d); }
    bool GetSkipComments() const { return (m_flags & wxSD_SKIP_COMMENTS); }
    bool GetSkipStrings() const { return (m_flags & wxSD_SKIP_STRINGS); }
    bool GetColourComments() const { return (m_flags & wxSD_COLOUR_COMMENTS); }
    const wxString& GetReplaceWith() const { return m_replaceWith; }
    void SetReplaceWith(const wxString& replaceWith) { this->m_replaceWith = replaceWith; }
};

//------------------------------------------
// class containing the search result
//------------------------------------------
class WXDLLIMPEXP_CL SearchResult : public wxObject
{
    wxString m_pattern;
    int m_position;
    int m_lineNumber;
    int m_column;
    wxString m_fileName;
    int m_len;
    wxString m_findWhat;
    size_t m_flags;
    int m_columnInChars;
    int m_lenInChars;
    wxString m_scope;
    wxArrayString m_regexCaptures;

public:
    // ctor-dtor, copy constructor and assignment operator
    SearchResult() {}

    virtual ~SearchResult() {}

    SearchResult(const SearchResult& rhs) { *this = rhs; }

    SearchResult& operator=(const SearchResult& rhs)
    {
        if(this == &rhs)
            return *this;
        m_position = rhs.m_position;
        m_column = rhs.m_column;
        m_lineNumber = rhs.m_lineNumber;
        m_pattern = rhs.m_pattern.c_str();
        m_fileName = rhs.m_fileName.c_str();
        m_len = rhs.m_len;
        m_findWhat = rhs.m_findWhat.c_str();
        m_flags = rhs.m_flags;
        m_columnInChars = rhs.m_columnInChars;
        m_lenInChars = rhs.m_lenInChars;
        m_scope = rhs.m_scope.c_str();
        m_regexCaptures = rhs.m_regexCaptures;
        return *this;
    }

    JSONItem ToJSON() const;
    void FromJSON(const JSONItem& json);

    //------------------------------------------------------
    // Setters/getters

    void SetFlags(const size_t& flags) { this->m_flags = flags; }

    const size_t& GetFlags() const { return m_flags; }

    void SetPattern(const wxString& pat) { m_pattern = pat.c_str(); }
    void SetPosition(const int& position) { m_position = position; }
    void SetLineNumber(const int& line) { m_lineNumber = line; }
    void SetColumn(const int& col) { m_column = col; }
    void SetFileName(const wxString& fileName) { m_fileName = fileName.c_str(); }

    const int& GetPosition() const { return m_position; }
    const int& GetLineNumber() const { return m_lineNumber; }
    const int& GetColumn() const { return m_column; }
    const wxString& GetPattern() const { return m_pattern; }
    const wxString& GetFileName() const { return m_fileName; }

    void SetLen(const int& len) { this->m_len = len; }
    const int& GetLen() const { return m_len; }

    // Setters
    void SetFindWhat(const wxString& findWhat) { this->m_findWhat = findWhat.c_str(); }
    // Getters
    const wxString& GetFindWhat() const { return m_findWhat; }

    void SetColumnInChars(const int& col) { this->m_columnInChars = col; }
    const int& GetColumnInChars() const { return m_columnInChars; }

    void SetLenInChars(const int& len) { this->m_lenInChars = len; }
    const int& GetLenInChars() const { return m_lenInChars; }

    void SetScope(const wxString& scope) { this->m_scope = scope.c_str(); }
    const wxString& GetScope() const { return m_scope; }

    void SetRegexCaptures(const wxArrayString& regexCaptures) { this->m_regexCaptures = regexCaptures; }
    const wxArrayString& GetRegexCaptures() const { return m_regexCaptures; }
    wxString GetRegexCapture(size_t backref) const
    {
        if(m_regexCaptures.size() > backref) {
            return m_regexCaptures[backref];
        } else {
            return wxEmptyString;
        }
    }

    // return a foramtted message
    wxString GetMessage() const
    {
        wxString msg;
        msg << GetFileName() << wxT("(") << GetLineNumber() << wxT(",") << GetColumn() << wxT(",") << GetLen()
            << wxT("): ") << GetPattern();
        return msg;
    }
};

typedef std::list<SearchResult> SearchResultList;

class WXDLLIMPEXP_CL SearchSummary : public wxObject
{
    int m_fileScanned;
    int m_matchesFound;
    int m_elapsed;
    wxArrayString m_failedFiles;
    wxString m_findWhat;
    wxString m_replaceWith;

public:
    SearchSummary()
        : m_fileScanned(0)
        , m_matchesFound(0)
        , m_elapsed(0)
    {
    }

    virtual ~SearchSummary() {}

    SearchSummary(const SearchSummary& rhs) { *this = rhs; }

    SearchSummary& operator=(const SearchSummary& rhs)
    {
        if(this == &rhs)
            return *this;

        m_fileScanned = rhs.m_fileScanned;
        m_matchesFound = rhs.m_matchesFound;
        m_elapsed = rhs.m_elapsed;
        m_failedFiles = rhs.m_failedFiles;
        m_findWhat = rhs.m_findWhat;
        m_replaceWith = rhs.m_replaceWith;
        return *this;
    }

    JSONItem ToJSON() const;
    void FromJSON(const JSONItem& json);

    void SetFindWhat(const wxString& findWhat) { this->m_findWhat = findWhat; }
    void SetReplaceWith(const wxString& replaceWith) { this->m_replaceWith = replaceWith; }
    const wxString& GetFindWhat() const { return m_findWhat; }
    const wxString& GetReplaceWith() const { return m_replaceWith; }
    const wxArrayString& GetFailedFiles() const { return m_failedFiles; }
    wxArrayString& GetFailedFiles() { return m_failedFiles; }

    int GetNumFileScanned() const { return m_fileScanned; }
    int GetNumMatchesFound() const { return m_matchesFound; }

    void SetNumFileScanned(const int& num) { m_fileScanned = num; }
    void SetNumMatchesFound(const int& num) { m_matchesFound = num; }
    void SetElapsedTime(long elapsed) { m_elapsed = elapsed; }
    wxString GetMessage() const
    {
        wxString msg(wxString(wxT("====== ")) + _("Number of files scanned: "));
        msg << m_fileScanned << wxT(",");
        msg << _(" Matches found: ");
        msg << m_matchesFound;
        int secs = m_elapsed / 1000;
        int msecs = m_elapsed % 1000;

        msg << _(", elapsed time: ") << secs << wxT(".") << msecs << _(" seconds") << wxT(" ======");
        if(!m_failedFiles.IsEmpty()) {
            msg << "\n";
            msg << "====== " << _("Failed to open the following files for scan:") << "\n";
            for(size_t i = 0; i < m_failedFiles.size(); ++i) {
                msg << m_failedFiles.Item(i) << "\n";
            }
        }
        return msg;
    }
};

//----------------------------------------------------------
// The search thread
//----------------------------------------------------------

class WXDLLIMPEXP_CL SearchThread : public WorkerThread
{
    friend class SearchThreadST;
    wxString m_wordChars;
    std::unordered_map<wxChar, bool> m_wordCharsMap; //< Internal
    SearchResultList m_results;
    bool m_stopSearch;
    SearchSummary m_summary;
    wxString m_reExpr;
    wxRegEx m_regex;
    bool m_matchCase;
    wxCriticalSection m_cs;
    int m_counter = 0;

public:
    /**
     * Default constructor.
     */
    SearchThread();

    /**
     * Destructor.
     */
    virtual ~SearchThread();

    /**
     * Process request from caller
     */
    void ProcessRequest(ThreadRequest* req);

    /**
     * Add a request to the search thread to start
     * \param data SearchData class
     */
    void PerformSearch(const SearchData& data);

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
    void SetWordChars(const wxString& chars);

private:
    /**
     * Return files to search
     * \param files output
     * \param data search data
     */
    void GetFiles(const SearchData* data, wxArrayString& files);

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
    void DoSearchFiles(ThreadRequest* data);

    // Perform search on a single file
    void DoSearchFile(const wxString& fileName, const SearchData* data);

    // Perform search on a line
    void DoSearchLine(const wxString& line, const int lineNum, const int lineOffset, const wxString& fileName,
                      const SearchData* data, const wxString& findWhat, const wxArrayString& filters);

    // Perform search on a line using regular expression
    void DoSearchLineRE(const wxString& line, const int lineNum, const int lineOffset, const wxString& fileName,
                        const SearchData* data);

    // Send an event to the notified window
    void SendEvent(wxEventType type, wxEvtHandler* owner);

    // return a compiled regex object for the expression
    wxRegEx& GetRegex(const wxString& expr, bool matchCase);

    // Internal function
    bool AdjustLine(wxString& line, int& pos, const wxString& findString);

    // filter 'files' according to the files spec
    void FilterFiles(wxArrayString& files, const SearchData* data);
};

class WXDLLIMPEXP_CL SearchThreadST
{
public:
    static SearchThread* Get();
    static void Free();
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SEARCH_THREAD_MATCHFOUND, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SEARCH_THREAD_SEARCHEND, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SEARCH_THREAD_SEARCHCANCELED, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SEARCH_THREAD_SEARCHSTARTED, wxCommandEvent);

#endif // SEARCH_THREAD_H
