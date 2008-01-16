#include "search_thread.h"
#include "wx/event.h"
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <iostream>
#include <wx/tokenzr.h>
#include <wx/dir.h>
#include <wx/log.h> 
#include "dirtraverser.h"
#include "macros.h"
#include "workspace.h"

#define ADJUST_LINE_AND_CONT(modLine, pos, findString)	\
{														\
	if( !AdjustLine(modLine, pos, findString) ){		\
		break;											\
	} else {											\
		continue;										\
	}													\
}

DEFINE_EVENT_TYPE(wxEVT_SEARCH_THREAD_MATCHFOUND)
DEFINE_EVENT_TYPE(wxEVT_SEARCH_THREAD_SEARCHEND)
DEFINE_EVENT_TYPE(wxEVT_SEARCH_THREAD_SEARCHCANCELED)
DEFINE_EVENT_TYPE(wxEVT_SEARCH_THREAD_SEARCHSTARTED)

//----------------------------------------------------------------
// SearchData
//----------------------------------------------------------------

const wxString& SearchData::GetExtensions() const 
{
	return m_validExt;
}

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

SearchThread::~SearchThread()
{
}

void SearchThread::IndexWordChars()
{
	m_wordCharsMap.clear();
	for(size_t i=0; i<m_wordChars.Length(); i++){
		m_wordCharsMap[m_wordChars.GetChar(i)] = true;
	}
}

void SearchThread::SetWordChars(const wxString &chars)
{
	m_wordChars = chars;
	IndexWordChars();
}

wxRegEx& SearchThread::GetRegex(const wxString &expr, bool matchCase)
{
	if(m_reExpr == expr && matchCase == m_matchCase){
		return m_regex;
	} else {
		m_reExpr = expr;
		m_matchCase = matchCase;

		int flags = wxRE_DEFAULT;
		if( !matchCase ) flags |= wxRE_ICASE;
		m_regex.Compile(m_reExpr, flags);
	}
	return m_regex;
}

void SearchThread::PerformSearch(const SearchData &data)
{
	Add( new SearchData(data) );
}

void SearchThread::ProcessRequest(ThreadRequest *req)
{
	wxStopWatch sw;
	m_summary = SearchSummary();
	DoSearchFiles(req);
	m_summary.SetElapsedTime(sw.Time());

	// Send search end event 
	SendEvent(wxEVT_SEARCH_THREAD_SEARCHEND);
}

void SearchThread::GetFiles(const SearchData *data, wxArrayString &files)
{
	if(data->GetRootDir() == SEARCH_IN_WORKSPACE)
	{
		files = data->GetFiles();
	}
	else if(data->GetRootDir() == SEARCH_IN_PROJECT)
	{
		files = data->GetFiles();
	}
	else
	{
		DirTraverser traverser(data->GetExtensions());
		wxDir dir(data->GetRootDir());
		dir.Traverse(traverser);
		files = traverser.GetFiles();
	}
}

void SearchThread::DoSearchFiles(ThreadRequest *req)
{
	SearchData *data = static_cast<SearchData*>(req);
	// Get all files
	if( data->GetRootDir().IsEmpty())
		return;

	if( data->GetFindString().IsEmpty() )
		return;

	StopSearch(false);
	wxArrayString fileList;
	GetFiles(data, fileList);
	wxStopWatch sw;

	// Send startup message to main thread
	if( m_notifiedWindow ){
		wxString message;
		wxCommandEvent event(wxEVT_SEARCH_THREAD_SEARCHSTARTED, GetId());
		message << wxT("====== Searching for: '") << data->GetFindString();
		message << wxT("';\tMatch case: ");
		if( data->IsMatchCase() ){
			message << wxT("true ");
		} else {
			message << wxT("false ");
		}

		message << wxT(";\tMatch whole word: ");
		if( data->IsMatchWholeWord() ){
			message << wxT("true ");
		} else {
			message << wxT("false ");
		}

		message << wxT(";\tRegular expression: ");
		if( data->IsRegularExpression() ){
			message << wxT("true;");
		} else {
			message << wxT("false; ======");
		}
		event.SetString(message);
		::wxPostEvent(m_notifiedWindow, event);
	}

	for(size_t i=0; i<fileList.Count(); i++){
		m_summary.SetNumFileScanned((int)i+1);

		// give user chance to cancel the search ...
		if( TestStopSearch() ) {
			// Send cancel event
			SendEvent(wxEVT_SEARCH_THREAD_SEARCHCANCELED);
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

void SearchThread::DoSearchFile(const wxString &fileName, const SearchData *data)
{
	// Process single lines
	int lineNumber = 1;
	if(!wxFileName::FileExists(fileName)){
		return;
	}

	wxFFile thefile(fileName, wxT("rb"));
	wxFileOffset size = thefile.Length();
	wxString fileData;
	fileData.Alloc(size);

	thefile.ReadAll(&fileData);
	wxStringTokenizer tkz(fileData, wxT("\n"), wxTOKEN_RET_EMPTY_ALL);
	
	if( data->IsRegularExpression() ){
		// regular expression search
		while(tkz.HasMoreTokens()){
			// Read the next line
			wxString line = tkz.NextToken();
			DoSearchLineRE(line, lineNumber, fileName, data);
			lineNumber++;
		}
	} else {
		// simple search
		while(tkz.HasMoreTokens()){

			// Read the next line
			wxString line = tkz.NextToken();
			DoSearchLine(line, lineNumber, fileName, data);
			lineNumber++;
		}
	}

	if( m_results.empty() == false )
		SendEvent(wxEVT_SEARCH_THREAD_MATCHFOUND);
}
void SearchThread::DoSearchLineRE(const wxString &line, const int lineNum, const wxString &fileName, const SearchData *data)
{
	wxRegEx &re = GetRegex(data->GetFindString(), data->IsMatchCase());
	size_t col = 0;
	wxString modLine = line;
	if( re.IsValid() ){
		while( re.Matches(modLine)) {
			size_t start, len;
			re.GetMatch(&start, &len);
			col += start;

			m_summary.SetNumMatchesFound(m_summary.GetNumMatchesFound() + 1);
			
			// Notify our match
			SearchResult result;
			result.SetColumn((int)col);
			result.SetLineNumber(lineNum);
			result.SetPattern(line);
			result.SetFileName(fileName);
			m_results.push_back(result);
			col += len;

			// adjust the line
			if(line.Length() - col <= 0)
				break;
			modLine = modLine.Right(line.Length() - col);
		}
	}
}

void SearchThread::DoSearchLine(const wxString &line, const int lineNum, const wxString &fileName, const SearchData *data)
{
	wxString findString = data->GetFindString();
	wxString modLine = line;

	if( !data->IsMatchCase() ){
		modLine = modLine.MakeLower();
		findString = findString.MakeLower();
	}

	int pos = 0;
	int col = 0;
	while( pos != wxNOT_FOUND ){
		pos = modLine.Find(findString);
		if(pos != wxNOT_FOUND){
			col += pos;

			// we have a match
			if( data->IsMatchWholeWord() ){

				// make sure that the word before is not in the wordChars map
				if((pos > 0) && (m_wordCharsMap.find(modLine.GetChar(pos-1)) != m_wordCharsMap.end()) )
					ADJUST_LINE_AND_CONT(modLine, pos, findString);

				// if we have more characters to the right, make sure that the first char does not match any
				// in the wordCharsMap
				if(pos + findString.Length() <= modLine.Length()){
					wxChar nextCh = modLine.GetChar(pos+findString.Length());
					if(m_wordCharsMap.find(nextCh) != m_wordCharsMap.end())
						ADJUST_LINE_AND_CONT(modLine, pos, findString);
				}		
			} 

			m_summary.SetNumMatchesFound(m_summary.GetNumMatchesFound() + 1);

			// Notify our match
			SearchResult result;
			result.SetColumn(col);
			result.SetLineNumber(lineNum);
			result.SetPattern(line);
			result.SetFileName(fileName);
			m_results.push_back(result);
			if( !AdjustLine(modLine, pos, findString) ){
				break;									
			}
			col += (int)findString.Length();
		}
	}
}

bool SearchThread::AdjustLine(wxString &line, int &pos, wxString &findString)
{
	// adjust the current line
	if( line.Length() - (pos + findString.Length()) >= findString.Length()){
		line = line.Right(line.Length() - (pos + findString.Length()));
		pos += (int)findString.Length();
		return true;
	} else {
		return false;
	}
}


void SearchThread::SendEvent(wxEventType type)
{
	if( !m_notifiedWindow )
		return;

	wxCommandEvent event(type, GetId());

	if(type == wxEVT_SEARCH_THREAD_MATCHFOUND)
	{
		event.SetClientData( new SearchResultList(m_results) );
		m_results.clear();
	}
	else if(type == wxEVT_SEARCH_THREAD_SEARCHEND)
	{
		// Nothing to do
		event.SetClientData( new SearchSummary(m_summary) );
	}
	else if(type == wxEVT_SEARCH_THREAD_SEARCHCANCELED)
	{
		event.SetString(wxT("Search cancelled by user"));
	}

	if( m_notifiedWindow ){
		::wxPostEvent(m_notifiedWindow, event);
	}
	wxThread::Sleep(5);
}
