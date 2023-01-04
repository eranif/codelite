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
#include "search_thread.h"

#include "clFilesCollector.h"
#include "clWildMatch.hpp"
#include "dirtraverser.h"
#include "file_logger.h"
#include "fileutils.h"
#include "macros.h"

#include <algorithm>
#include <iostream>
#include <set>
#include <wx/dir.h>
#include <wx/event.h>
#include <wx/fontmap.h>
#include <wx/log.h>
#include <wx/stopwatch.h>
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>

#if !wxUSE_GUI
#include "cl_command_event.h" // Needed for the definition of wxCommandEvent
#endif

wxDEFINE_EVENT(wxEVT_SEARCH_THREAD_MATCHFOUND, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_SEARCH_THREAD_SEARCHEND, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_SEARCH_THREAD_SEARCHCANCELED, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_SEARCH_THREAD_SEARCHSTARTED, wxCommandEvent);

#define SEND_ST_EVENT()                       \
    if(owner) {                               \
        wxPostEvent(owner, event);            \
    } else if(m_notifiedWindow) {             \
        wxPostEvent(m_notifiedWindow, event); \
    }

//----------------------------------------------------------------
// SearchData
//----------------------------------------------------------------
namespace
{
bool is_word_char(wxChar ch) { return ch == '_' || wxIsalnum(ch); }

// Minumum of 10ms between events that this thread is sending to the main thread
constexpr long MIN_SEND_INTERVAL_MS = 1;
size_t send_count = 0;

} // namespace

const wxString& SearchData::GetExtensions() const { return m_validExt; }
SearchData& SearchData::operator=(const SearchData& rhs) { return Copy(rhs); }
SearchData& SearchData::Copy(const SearchData& other)
{
    if(this == &other) {
        return *this;
    }
    m_findString = other.m_findString.c_str();
    m_flags = other.m_flags;
    m_validExt = other.m_validExt.c_str();
    m_rootDirs = other.m_rootDirs;
    m_newTab = other.m_newTab;
    m_owner = other.m_owner;
    m_encoding = other.m_encoding.c_str();
    m_replaceWith = other.m_replaceWith;
    m_excludePatterns.clear();
    m_excludePatterns.insert(m_excludePatterns.end(), other.m_excludePatterns.begin(), other.m_excludePatterns.end());
    m_files.clear();
    m_files.reserve(other.m_files.size());
    m_file_scanner_flags = other.m_file_scanner_flags;
    for(size_t i = 0; i < other.m_files.size(); ++i) {
        m_files.Add(other.m_files.Item(i).c_str());
    }
    return *this;
}

//----------------------------------------------------------------
// SearchThread
//----------------------------------------------------------------

SearchThread::SearchThread()
    : WorkerThread()
    , m_reExpr(wxT(""))
{
    m_stopWatch.Start();
}

SearchThread::~SearchThread() {}

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

        if(!matchCase)
            flags |= wxRE_ICASE;
        m_regex.Compile(m_reExpr, flags);
    }
    return m_regex;
}

void SearchThread::PerformSearch(const SearchData& data) { Add(new SearchData(data)); }

void SearchThread::ProcessRequest(ThreadRequest* req)
{
    FileLogger::RegisterThread(wxThread::GetCurrentId(), "Search Thread");
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
    wxStopWatch sw;
    clDEBUG() << "Building list of files ..." << endl;
    wxStringSet_t scannedFiles;

    const wxArrayString& rootDirs = data->GetRootDirs();
    files = data->GetFiles();

    // Populate "scannedFiles" with list of files to scan
    scannedFiles.insert(files.begin(), files.end());
    files.reserve(5000);
    clDEBUG() << "Scanning directories..." << endl;
    sw.Start();

    clFileExtensionMatcher ext_matcher{ data->GetExtensions() };
    clPathExcluder path_excluder{ data->GetExcludePatterns() };

    wxStringSet_t visited_dirs;
    for(size_t i = 0; i < rootDirs.size(); ++i) {
        clDEBUG() << "    scanning root directory:" << rootDirs.Item(i) << endl;
        // collect only unique files that are matching the pattern
        auto on_files = [&](const wxArrayString& paths) {
            files.reserve(files.size() + paths.size());
            for(const wxString& fullpath : paths) {
                if(scannedFiles.insert(fullpath).second && ext_matcher.matches(fullpath)) {
                    files.Add(fullpath);
                }
            }
        };

        // do not traverse into excluded directories or directories that
        // we already visited
        auto on_folder = [&](const wxString& fullpath) -> bool {
            return
                // first time visiting this directory
                visited_dirs.insert(fullpath).second &&
                // is not excluded
                !path_excluder.is_exclude_path(fullpath);
        };

        // make sure it's really a dir (not a fifo, etc.)
        clFilesScanner scanner;
        scanner.ScanWithCallbacks(rootDirs.Item(i), on_folder, on_files, data->GetFileScannerFlags());
        clDEBUG() << "    scanning root directory:" << rootDirs.Item(i) << "..done" << endl;
    }

    wxString duration;
    duration << sw.Time() / 1000 << "." << sw.Time() % 1000;
    clDEBUG() << "Scanning directories... done (" << duration << ")" << endl;
    clDEBUG() << "Found" << files.size() << "files" << endl;

    // sort the files found
    clDEBUG() << "Sorting the matches..." << endl;
    files.Sort([](const wxString& f1, const wxString& f2) -> int { return f1.CmpNoCase(f2); });
    clDEBUG() << "Sorting the matches... done" << endl;
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

    // ignore binary executables
    if(FileUtils::IsBinaryExecutable(fileName)) {
        return;
    }

    size_t size = FileUtils::GetFileSize(fileName);
    if(size == 0) {
        return;
    }
    wxString fileData;
    fileData.Alloc(size);

#if wxUSE_GUI
    // support for other encoding
    wxFontEncoding enc = wxFontMapper::GetEncodingFromName(data->GetEncoding().c_str());
    wxCSConv fontEncConv(enc);
    if(!FileUtils::ReadFileContent(fileName, fileData, fontEncConv)) {
        m_summary.GetFailedFiles().Add(fileName);
        return;
    }
#else
    if(!FileUtils::ReadFileContent(fileName, fileData, wxConvLibc)) {
        m_summary.GetFailedFiles().Add(fileName);
        return;
    }
#endif
    wxArrayString lines = ::wxStringTokenize(fileData, wxT("\n"), wxTOKEN_RET_EMPTY_ALL);

    int lineOffset = 0;
    if(data->IsRegularExpression()) {
        // regular expression search
        for(const wxString& line : lines) {
            // Read the next line
            DoSearchLineRE(line, lineNumber, lineOffset, fileName, data);
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

        // Dont search for empty strings
        if(findString.empty()) {
            return;
        }

        if(!data->IsMatchCase()) {
            findString.MakeLower();
        }
        for(const wxString& line : lines) {
            DoSearchLine(line, lineNumber, lineOffset, fileName, data, findString, filters);
            lineOffset += line.Length() + 1;
            lineNumber++;
        }
    }

    if(m_results.empty() == false) {
        SendEvent(wxEVT_SEARCH_THREAD_MATCHFOUND, data->GetOwner());
    }
}

void SearchThread::DoSearchLineRE(const wxString& line, const int lineNum, const int lineOffset,
                                  const wxString& fileName, const SearchData* data)
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
            iCorrectedCol = FileUtils::UTF8Length(line.c_str(), col);
            iCorrectedLen = FileUtils::UTF8Length(line.c_str(), col + len) - iCorrectedCol;
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
            wxArrayString regexCaptures;
            for(size_t i = 0; i < re.GetMatchCount(); ++i) {
                regexCaptures.Add(re.GetMatch(modLine, i));
            }
            result.SetRegexCaptures(regexCaptures);

            // Make sure our match is not on a comment
            m_results.push_back(result);
            m_summary.SetNumMatchesFound(m_summary.GetNumMatchesFound() + 1);

            col += len;

            // adjust the line
            if(line.Length() - col <= 0)
                break;
            modLine = modLine.Right(line.Length() - col);
        }
    }
}

void SearchThread::DoSearchLine(const wxString& line, const int lineNum, const int lineOffset, const wxString& fileName,
                                const SearchData* data, const wxString& findWhat, const wxArrayString& filters)
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
            if(!allFiltersOK)
                return;

            // we have a match
            if(data->IsMatchWholeWord()) {

                // make sure that the word before is not in the wordChars map
                if(pos > 0 && is_word_char(modLine.GetChar(pos - 1))) {
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
                    if(is_word_char(nextCh)) {
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
            iCorrectedCol = FileUtils::UTF8Length(line.c_str(), col);
            iCorrectedLen = FileUtils::UTF8Length(findWhat.c_str(), findWhat.Length());
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

            m_results.push_back(result);
            m_summary.SetNumMatchesFound(m_summary.GetNumMatchesFound() + 1);

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
    if(!m_notifiedWindow && !owner)
        return;

    wxCommandEvent event(type, GetId());
    if(type == wxEVT_SEARCH_THREAD_MATCHFOUND) {
        // match found and we scanned 10 files
        event.SetClientData(new SearchResultList(m_results));
        m_results.clear();
        SEND_ST_EVENT();

    } else if((type == wxEVT_SEARCH_THREAD_SEARCHEND) || (type == wxEVT_SEARCH_THREAD_SEARCHCANCELED)) {
        // search eneded, if we got any matches "buffered" send them before the
        // the summary event
        if(m_results.empty() == false) {
            wxCommandEvent evt(wxEVT_SEARCH_THREAD_MATCHFOUND, GetId());
            evt.SetClientData(new SearchResultList(m_results));
            if(owner) {
                wxPostEvent(owner, evt);
            } else if(m_notifiedWindow) {
                wxPostEvent(m_notifiedWindow, evt);
            }
        }
        m_results.clear();
        // Now send the summary event
        event.SetClientData(type == wxEVT_SEARCH_THREAD_SEARCHEND ? new SearchSummary(m_summary) : nullptr);
        SEND_ST_EVENT();
    }
    // to avoid flooding the UI with search events, sleep for 1ms after each
    // send_event call
    ++send_count;
    if(send_count >= 10) {
        wxThread::Sleep(1);
        send_count = 0;
    }
}

void SearchThread::FilterFiles(wxArrayString& files, const SearchData* data)
{
    wxArrayString tmpFiles;
    std::set<wxString> uniqueFiles;
    const wxArrayString& excludePatterns = data->GetExcludePatterns();
    const wxString& mask = data->GetExtensions();
    std::for_each(files.begin(), files.end(), [&](const wxString& filename) {
        if(uniqueFiles.count(filename))
            return;
        uniqueFiles.insert(filename);
        if(FileUtils::WildMatch(mask, filename) && !FileUtils::WildMatch(excludePatterns, filename)) {
            tmpFiles.Add(filename);
        }
    });
    files.swap(tmpFiles);
    files.Sort([](const wxString& f1, const wxString& f2) -> int { return f1.CmpNoCase(f2); });
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
    if(gs_SearchThread == NULL)
        gs_SearchThread = new SearchThread;
    return gs_SearchThread;
}

JSONItem SearchResult::ToJSON() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty("file", m_fileName);
    json.addProperty("line", m_lineNumber);
    json.addProperty("col", m_column);
    json.addProperty("pos", m_position);
    json.addProperty("pattern", m_pattern);
    json.addProperty("len", m_len);
    json.addProperty("flags", m_flags);
    json.addProperty("columnInChars", m_columnInChars);
    json.addProperty("lenInChars", m_lenInChars);
    json.addProperty("regexCaptures", m_regexCaptures);
    return json;
}

void SearchResult::FromJSON(const JSONItem& json)
{
    m_position = json.namedObject("pos").toInt(m_position);
    m_column = json.namedObject("col").toInt(m_column);
    m_lineNumber = json.namedObject("line").toInt(m_lineNumber);
    m_pattern = json.namedObject("pattern").toString(m_pattern);
    m_fileName = json.namedObject("file").toString(m_fileName);
    m_len = json.namedObject("len").toInt(m_len);
    m_flags = json.namedObject("flags").toSize_t(m_flags);
    m_columnInChars = json.namedObject("columnInChars").toInt(m_columnInChars);
    m_lenInChars = json.namedObject("lenInChars").toInt(m_lenInChars);
    m_regexCaptures = json.namedObject("regexCaptures").toArrayString();
}

JSONItem SearchSummary::ToJSON() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty("filesScanned", m_fileScanned);
    json.addProperty("matchesFound", m_matchesFound);
    json.addProperty("elapsed", m_elapsed);
    json.addProperty("failedFiles", m_failedFiles);
    json.addProperty("findWhat", m_findWhat);
    json.addProperty("replaceWith", m_replaceWith);
    return json;
}

void SearchSummary::FromJSON(const JSONItem& json)
{
    m_fileScanned = json.namedObject("filesScanned").toInt(m_fileScanned);
    m_matchesFound = json.namedObject("matchesFound").toInt(m_matchesFound);
    m_elapsed = json.namedObject("elapsed").toInt(m_elapsed);
    m_failedFiles = json.namedObject("failedFiles").toArrayString();
    m_findWhat = json.namedObject("findWhat").toString();
    m_replaceWith = json.namedObject("replaceWith").toString();
}
