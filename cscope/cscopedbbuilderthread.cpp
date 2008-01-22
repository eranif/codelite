#include "dirsaver.h"
#include "wx/filefn.h"
#include "cscopedbbuilderthread.h"
#include "globals.h"
#include "cscope.h"

int wxEVT_CSCOPE_THREAD_DB_BUILD_DONE = wxNewId();

CscopeDbBuilderThread::CscopeDbBuilderThread(wxEvtHandler *owner)
: wxThread(wxTHREAD_JOINABLE) 
, m_owner(owner)
{
}

CscopeDbBuilderThread::~CscopeDbBuilderThread()
{
}

void *CscopeDbBuilderThread::Entry()
{
	//change dir to the workspace directory
	DirSaver ds;
	
	wxSetWorkingDirectory(GetWorkingDir());
	
	//notify the database creation process as completed
	wxArrayString output;
	
	//set environment variables required by cscope
	wxSetEnv(wxT("TMPDIR"), wxT("."));
	SafeExecuteCommand(m_cmd, output);
	
	wxCommandEvent e(wxEVT_CSCOPE_THREAD_DB_BUILD_DONE);
	CscopeResultTable *result = ParseResults( output );
	
	e.SetClientData(result);
	e.SetString(GetCmd());
	m_owner->AddPendingEvent(e);
	return NULL;
}

CscopeResultTable* CscopeDbBuilderThread::ParseResults(const wxArrayString &output)
{
	CscopeResultTable *results = new CscopeResultTable();
	for (size_t i=0; i< output.GetCount(); i++) {
		//parse each line
		wxString line = output.Item(i);
		CscopeEntryData data;

		//first is the file name
		line = line.Trim().Trim(false);
		wxString file = line.BeforeFirst(wxT(' '));
		data.SetFile(file);
		line = line.AfterFirst(wxT(' '));

		//next is the scope
		line = line.Trim().Trim(false);
		wxString scope = line.BeforeFirst(wxT(' '));
		line = line.AfterFirst(wxT(' '));

		//next is the line number
		line = line.Trim().Trim(false);
		long nn;
		wxString line_number = line.BeforeFirst(wxT(' '));
		line_number.ToLong( &nn );
		data.SetLine( nn );
		line = line.AfterFirst(wxT(' '));

		//the rest is the pattern
		wxString pattern = line;
		data.SetPattern(pattern);

		//insert the result
		CscopeResultTable::const_iterator iter = results->find(data.GetFile());
		std::vector< CscopeEntryData > *vec(NULL);
		if (iter != results->end()) {
			//this file already exist, append the result
			vec = iter->second;
		} else {
			vec = new std::vector< CscopeEntryData >();
			//add it to the map
			(*results)[data.GetFile()] = vec;
		}
		vec->push_back( data );
	}
	return results;
}



