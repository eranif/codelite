//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : search_thread.cpp
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
#include <set>
#include "editor_config.h"
#include "search_thread.h"
#include "wx/event.h"
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include "cppwordscanner.h"
#include <iostream>
#include <wx/tokenzr.h>
#include <wx/dir.h>
#include <wx/fontmap.h>
#include <wx/log.h>
#include "dirtraverser.h"
#include "macros.h"
#include "workspace.h"
#include "globals.h"
#include <algorithm>
#include "fileutils.h"
#include "clFilesCollector.h"

const wxEventType wxEVT_SEARCH_THREAD_MATCHFOUND = wxNewEventType();
const wxEventType wxEVT_SEARCH_THREAD_SEARCHEND = wxNewEventType();
const wxEventType wxEVT_SEARCH_THREAD_SEARCHCANCELED = wxNewEventType();
const wxEventType wxEVT_SEARCH_THREAD_SEARCHSTARTED = wxNewEventType();

#define SEND_ST_EVENT()                       \
    if(owner) {                               \
        wxPostEvent(owner, event);            \
    } else if(m_notifiedWindow) {             \
        wxPostEvent(m_notifiedWindow, event); \
    }                                         \
    wxThread::Sleep(1);

//----------------------------------------------------------------
// SearchData
//----------------------------------------------------------------

const wxString& SearchData::GetExtensions() const { return m_validExt; }

//----------------------------------------------------------------
// SearchThread
//----------------------------------------------------------------

SearchThread::SearchThread()
    : WorkerThread()
    , m_wordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"))
    , m_reExpr(wxT(""))
{
    IndexWordChars();
}

SearchThread::~SearchThread() {}

void SearchThread::IndexWordChars()
{
    m_wordCharsMap.clear();
    for(size_t i = 0; i < m_wordChars.Length(); i++) {
        m_wordCharsMap[m_wordChars.GetChar(i)] = true;
    }
}

void SearchThread::SetWordChars(const wxString& chars)
{
    m_wordChars = chars;
    IndexWordChars();
}

wxRegEx& SearchThread::GetRegex(const wxString& expr, bool matchCase)
{
    if(m_reExpr == expr && matchCase == m_matchCase) {
        return m_regex;
    } else {
        m_reExpr = expr;
        m_matchCase = matchCase;
#ifndef __WXMAC__
        int flags = wxRE_ADVANCED;
#else
        int flags = wxRE_DEFAULT;
#endif

        if(!matchCase) flags |= wxRE_ICASE;
        m_regex.Compile(m_reExpr, flags);
    }
    return m_regex;
}

void SearchThread::PerformSearch(const SearchData& data) { Add(new SearchData(data)); }

void SearchThread::ProcessRequest(ThreadRequest* req)
{
    wxStopWatch sw;
    m_summary = SearchSummary();
    DoSearchFiles(req);
    m_summary.SetElapsedTime(sw.Time());

    SearchData* sd = (SearchData*)req;
    m_summary.SetFindWhat(sd->GetFindString());
    m_summary.SetReplaceWith(sd->GetReplaceWith());

    // Send search end event
    SendEvent(wxEVT_SEARCH_THREAD_SEARCHEND, sd->GetOwner());
}

void SearchThread::GetFiles(const SearchData* data, wxArrayString& files)
{
    wxStringSet_t scannedFiles;

    const wxArrayString& rootDirs = data->GetRootDirs();
    files = data->GetFiles();

    // Remove files that do not match our search criteria
    FilterFiles(files, data);

    // Populate "scannedFiles" with list of files to scan
    scannedFiles.insert(files.begin(), files.end());

    for(size_t i = 0; i < rootDirs.size(); ++i) {
        // make sure it's really a dir (not a fifo, etc.)
        clFilesScanner scanner;
        std::vector<wxString> filesV;
        if(scanner.Scan(rootDirs.Item(i), filesV, data->GetExtensions())) {
            std::for_each(filesV.begin(), filesV.end(), [&](const wxString& file) { scannedFiles.insert(file); });
        }
    }

    files.clear();
    files.Alloc(scannedFiles.size());
    std::for_each(scannedFiles.begin(), scannedFiles.end(), [&](const wxString& file) { files.Add(file); });
}

void SearchThread::DoSearchFiles(ThreadRequest* req)
{
    SearchData* data = static_cast<SearchData*>(req);

    // Get all files
    if(data->GetFindString().IsEmpty()) {
        SendEvent(wxEVT_SEARCH_THREAD_SEARCHSTARTED, data->GetOwner());
        return;
    }

    StopSearch(false);
    wxArrayString fileList;
    GetFiles(data, fileList);

    wxStopWatch sw;

    // Send startup message to main thread
    if(m_notifiedWindow || data->GetOwner()) {
        wxCommandEvent event(wxEVT_SEARCH_THREAD_SEARCHSTARTED, GetId());
        event.SetClientData(new SearchData(*data));
        if(data->GetOwner()) {
            ::wxPostEvent(data->GetOwner(), event);
        } else {
            // since we are in if ( m_notifiedWindow || data->GetOwner() ) block...
            ::wxPostEvent(m_notifiedWindow, event);
        }
    }

    for(size_t i = 0; i < fileList.Count(); i++) {
        m_summary.SetNumFileScanned((int)i + 1);

        // give user chance to cancel the search ...
        if(TestStopSearch()) {
            // Send cancel event
            SendEvent(wxEVT_SEARCH_THREAD_SEARCHCANCELED, data->GetOwner());
            StopSearch(false);
            break;
        }
        DoSearchFile(fileList.Item(i), data);
    }
}

bool SearchThread::TestStopSearch()
{
    bool stop = false;
    {
        wxCriticalSectionLocker locker(m_cs);
        stop = m_stopSearch;
    }
    return stop;
}

void SearchThread::StopSearch(bool stop)
{
    wxCriticalSectionLocker locker(m_cs);
    m_stopSearch = stop;
}

void SearchThread::DoSearchFile(const wxString& fileName, const SearchData* data)
{
    // Process single lines
    int lineNumber = 1;
    if(!wxFileName::FileExists(fileName)) {
        return;
    }

    wxFFile thefile(fileName, wxT("rb"));
    if(!thefile.IsOpened()) {
        // failed to open the file, probably because of permissions
        m_summary.GetFailedFiles().Add(fileName);
        return;
    }

    wxFileOffset size = thefile.Length();
    wxString fileData;
    fileData.Alloc(size);

    // support for other encoding
    wxFontEncoding enc = wxFontMapper::GetEncodingFromName(data->GetEncoding().c_str());
    wxCSConv fontEncConv(enc);
    if(!thefile.ReadAll(&fileData, fontEncConv)) {
        m_summary.GetFailedFiles().Add(fileName);
        return;
    }

    // take a wild guess and see if we really need to construct
    // a TextStatesPtr object (it is quite an expensive operation)
    bool shouldCreateStates(true);
    if(data->IsMatchCase() && !data->IsRegularExpression()) {
        shouldCreateStates = (fileData.Find(data->GetFindString()) != wxNOT_FOUND);

    } else if(!data->IsMatchCase() && !data->IsRegularExpression()) {
        // !data->IsMatchCase()
        wxString tmpData = fileData;
        shouldCreateStates = (tmpData.MakeLower().Find(data->GetFindString()) != wxNOT_FOUND);
    }

    wxStringTokenizer tkz(fileData, wxT("\n"), wxTOKEN_RET_EMPTY_ALL);

    // Incase one of the C++ options is enabled,
    // create a text states object
    TextStatesPtr states(NULL);
    if(data->HasCppOptions() && shouldCreateStates && false) {
        CppWordScanner scanner("", fileData.mb_str().data(), 0);
        states = scanner.states();
    }

    int lineOffset = 0;
    if(data->IsRegularExpression()) {
        // regular expression search
        while(tkz.HasMoreTokens()) {
            // Read the next line
            wxString line = tkz.NextToken();
            DoSearchLineRE(line, lineNumber, lineOffset, fileName, data, states);
            lineOffset += line.Length() + 1;
            lineNumber++;
        }
    } else {
        // simple search
        wxString findString;
        wxArrayString filters;
        findString = data->GetFindString();
        if(data->IsEnablePipeSupport()) {
            if(data->GetFindString().Find('|') != wxNOT_FOUND) {
                findString = data->GetFindString().BeforeFirst('|');

                wxString filtersString = data->GetFindString().AfterFirst('|');
                filters = ::wxStringTokenize(filtersString, "|", wxTOKEN_STRTOK);
                if(!data->IsMatchCase()) {
                    for(size_t i = 0; i < filters.size(); ++i) {
                        filters.Item(i).MakeLower();
                    }
                }
            }
        }

        if(!data->IsMatchCase()) {
            findString.MakeLower();
        }

        while(tkz.HasMoreTokens()) {

            // Read the next line
            wxString line = tkz.NextToken();
            DoSearchLine(line, lineNumber, lineOffset, fileName, data, findString, filters, states);
            lineOffset += line.Length() + 1;
            lineNumber++;
        }
    }

    if(m_results.empty() == false) SendEvent(wxEVT_SEARCH_THREAD_MATCHFOUND, data->GetOwner());
}
void SearchThread::DoSearchLineRE(const wxString& line,
                                  const int lineNum,
                                  const int lineOffset,
                                  const wxString& fileName,
                                  const SearchData* data,
                                  TextStatesPtr statesPtr)
{
    wxRegEx& re = GetRegex(data->GetFindString(), data->IsMatchCase());
    size_t col = 0;
    int iCorrectedCol = 0;
    int iCorrectedLen = 0;
    wxString modLine = line;
    if(re.IsValid()) {
        while(re.Matches(modLine)) {
            size_t start, len;
            re.GetMatch(&start, &len);
            col += start;

            // Notify our match
            // correct search Pos and Length owing to non plain ASCII multibyte characters
            iCorrectedCol = clUTF8Length(line.c_str(), col);
            iCorrectedLen = clUTF8Length(line.c_str(), col + len) - iCorrectedCol;
            SearchResult result;
            result.SetPosition(lineOffset + col);
            result.SetColumnInChars((int)col);
            result.SetColumn(iCorrectedCol);
            result.SetLineNumber(lineNum);
            result.SetPattern(line);
            result.SetFileName(fileName);
            result.SetLenInChars((int)len);
            result.SetLen(iCorrectedLen);
            result.SetFlags(data->m_flags);
            result.SetFindWhat(data->GetFindString());

            // Make sure our match is not on a comment
            int position(wxNOT_FOUND);
            bool canAdd(true);

            if(statesPtr) {
                position = statesPtr->LineToPos(lineNum - 1);
                position += iCorrectedCol;
            }

            if(statesPtr && position != wxNOT_FOUND && data->GetSkipComments()) {
                if(statesPtr->states.size() > (size_t)position) {
                    short state = statesPtr->states.at(position).state;
                    if(state == CppWordScanner::STATE_CPP_COMMENT || state == CppWordScanner::STATE_C_COMMENT) {
                        canAdd = false;
                    }
                }
            }

            if(statesPtr && position != wxNOT_FOUND && data->GetSkipStrings()) {
                if(statesPtr->states.size() > (size_t)position) {
                    short state = statesPtr->states.at(position).state;
                    if(state == CppWordScanner::STATE_DQ_STRING || state == CppWordScanner::STATE_SINGLE_STRING) {
                        canAdd = false;
                    }
                }
            }

            result.SetMatchState(CppWordScanner::STATE_NORMAL);
            if(canAdd && statesPtr && position != wxNOT_FOUND && data->GetColourComments()) {
                // set the match state
                if(statesPtr->states.size() > (size_t)position) {
                    short state = statesPtr->states.at(position).state;
                    if(state == CppWordScanner::STATE_C_COMMENT || state == CppWordScanner::STATE_CPP_COMMENT) {
                        result.SetMatchState(state);
                    }
                }
            }

            if(canAdd) {
                m_results.push_back(result);
                m_summary.SetNumMatchesFound(m_summary.GetNumMatchesFound() + 1);
            }

            col += len;

            // adjust the line
            if(line.Length() - col <= 0) break;
            modLine = modLine.Right(line.Length() - col);
        }
    }
}

void SearchThread::DoSearchLine(const wxString& line,
                                const int lineNum,
                                const int lineOffset,
                                const wxString& fileName,
                                const SearchData* data,
                                const wxString& findWhat,
                                const wxArrayString& filters,
                                TextStatesPtr statesPtr)
{
    wxString modLine = line;

    if(!data->IsMatchCase()) {
        modLine.MakeLower();
    }

    int pos = 0;
    int col = 0;
    int iCorrectedCol = 0;
    int iCorrectedLen = 0;
    while(pos != wxNOT_FOUND) {
        pos = modLine.Find(findWhat);
        if(pos != wxNOT_FOUND) {
            col += pos;

            // Pipe support
            bool allFiltersOK = true;
            if(!filters.IsEmpty()) {
                // Apply the filters
                for(size_t i = 0; i < filters.size() && allFiltersOK; ++i) {
                    allFiltersOK = (modLine.Find(filters.Item(i)) != wxNOT_FOUND);
                }
            }

            // Pipe filtes OK?
            if(!allFiltersOK) return;

            // we have a match
            if(data->IsMatchWholeWord()) {

                // make sure that the word before is not in the wordChars map
                if((pos > 0) && (m_wordCharsMap.find(modLine.GetChar(pos - 1)) != m_wordCharsMap.end())) {
                    if(!AdjustLine(modLine, pos, findWhat)) {

                        break;
                    } else {
                        col += (int)findWhat.Length();
                        continue;
                    }
                }
                // if we have more characters to the right, make sure that the first char does not match any
                // in the wordCharsMap
                if(pos + findWhat.Length() <= modLine.Length()) {
                    wxChar nextCh = modLine.GetChar(pos + findWhat.Length());
                    if(m_wordCharsMap.find(nextCh) != m_wordCharsMap.end()) {
                        if(!AdjustLine(modLine, pos, findWhat)) {

                            break;
                        } else {
                            col += (int)findWhat.Length();
                            continue;
                        }
                    }
                }
            }

            // Notify our match
            // correct search Pos and Length owing to non plain ASCII multibyte characters
            iCorrectedCol = clUTF8Length(line.c_str(), col);
            iCorrectedLen = clUTF8Length(findWhat.c_str(), findWhat.Length());
            SearchResult result;
            result.SetPosition(lineOffset + col);
            result.SetColumnInChars(col);
            result.SetColumn(iCorrectedCol);
            result.SetLineNumber(lineNum);
            // Dont use match pattern larger than 500 chars
            result.SetPattern(line.length() > 500 ? line.Mid(0, 500) : line);
            result.SetFileName(fileName);
            result.SetLenInChars((int)findWhat.Length());
            result.SetLen(iCorrectedLen);
            result.SetFindWhat(data->GetFindString());
            result.SetFlags(data->m_flags);

            int position(wxNOT_FOUND);
            bool canAdd(true);

            if(statesPtr) {
                position = statesPtr->LineToPos(lineNum - 1);
                position += iCorrectedCol;
            }

            // Make sure our match is not on a comment
            if(statesPtr && position != wxNOT_FOUND && data->GetSkipComments()) {
                if(statesPtr->states.size() > (size_t)position) {
                    short state = statesPtr->states.at(position).state;
                    if(state == CppWordScanner::STATE_CPP_COMMENT || state == CppWordScanner::STATE_C_COMMENT) {
                        canAdd = false;
                    }
                }
            }

            if(statesPtr && position != wxNOT_FOUND && data->GetSkipStrings()) {
                if(statesPtr->states.size() > (size_t)position) {
                    short state = statesPtr->states.at(position).state;
                    if(state == CppWordScanner::STATE_DQ_STRING || state == CppWordScanner::STATE_SINGLE_STRING) {
                        canAdd = false;
                    }
                }
            }

            result.SetMatchState(CppWordScanner::STATE_NORMAL);
            if(canAdd && statesPtr && position != wxNOT_FOUND && data->GetColourComments()) {
                // set the match state
                if(statesPtr->states.size() > (size_t)position) {
                    short state = statesPtr->states.at(position).state;
                    if(state == CppWordScanner::STATE_C_COMMENT || state == CppWordScanner::STATE_CPP_COMMENT) {
                        result.SetMatchState(state);
                    }
                }
            }

            if(canAdd) {
                m_results.push_back(result);
                m_summary.SetNumMatchesFound(m_summary.GetNumMatchesFound() + 1);
            }

            if(!AdjustLine(modLine, pos, findWhat)) {
                break;
            }
            col += (int)findWhat.Length();
        }
    }
}

bool SearchThread::AdjustLine(wxString& line, int& pos, const wxString& findString)
{
    // adjust the current line
    if(line.Length() - (pos + findString.Length()) >= findString.Length()) {
        line = line.Right(line.Length() - (pos + findString.Length()));
        pos += (int)findString.Length();
        return true;
    } else {
        return false;
    }
}

void SearchThread::SendEvent(wxEventType type, wxEvtHandler* owner)
{
    if(!m_notifiedWindow && !owner) return;

    static int counter(0);

    wxCommandEvent event(type, GetId());

    if(type == wxEVT_SEARCH_THREAD_MATCHFOUND && counter == 10) {
        // match found and we scanned 10 files
        counter = 0;
        event.SetClientData(new SearchResultList(m_results));
        m_results.clear();
        SEND_ST_EVENT();

    } else if(type == wxEVT_SEARCH_THREAD_MATCHFOUND) {
        // a match event, but we did not meet the minimum number of files
        counter++;
        wxThread::Sleep(10);

    } else if(type == wxEVT_SEARCH_THREAD_SEARCHEND) {
        // search eneded, if we got any matches "buffed" send them before the
        // the summary event
        if(m_results.empty() == false) {
            wxCommandEvent evt(wxEVT_SEARCH_THREAD_MATCHFOUND, GetId());
            evt.SetClientData(new SearchResultList(m_results));
            m_results.clear();
            counter = 0;

            if(owner) {
                wxPostEvent(owner, evt);
            } else if(m_notifiedWindow) {
                wxPostEvent(m_notifiedWindow, evt);
            }
        }

        // Now send the summary event
        event.SetClientData(new SearchSummary(m_summary));
        SEND_ST_EVENT();

    } else if(type == wxEVT_SEARCH_THREAD_SEARCHCANCELED) {
        // search cancelled, we dont care about any matches which haven't
        // been reported yet
        event.SetClientData(new wxString(wxT("Search cancelled by user")));
        m_results.clear();
        counter = 0;

        SEND_ST_EVENT();
    }
}

void SearchThread::FilterFiles(wxArrayString& files, const SearchData* data)
{
    wxArrayString tmpFiles;
    std::set<wxString> uniqueFiles;
    const wxString& mask = data->GetExtensions();
    std::for_each(files.begin(), files.end(), [&](wxString& filename) {
        if(uniqueFiles.count(filename)) return;
        uniqueFiles.insert(filename);
        if(FileUtils::WildMatch(mask, filename)) {
            tmpFiles.Add(filename);
        }
    });
    files.swap(tmpFiles);
}

static SearchThread* gs_SearchThread = NULL;
void SearchThreadST::Free()
{
    if(gs_SearchThread) {
        delete gs_SearchThread;
    }
    gs_SearchThread = NULL;
}

SearchThread* SearchThreadST::Get()
{
    if(gs_SearchThread == NULL) gs_SearchThread = new SearchThread;
    return gs_SearchThread;
}
