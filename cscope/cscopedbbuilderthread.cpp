#include "dirsaver.h"
#include "wx/filefn.h"
#include "cscopedbbuilderthread.h"
#include "globals.h"

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
	
	wxArrayString *pout = new wxArrayString(output);
	wxCommandEvent e(wxEVT_CSCOPE_THREAD_DB_BUILD_DONE);
	e.SetClientData((void*)pout);
	e.SetString(GetCmd());
	m_owner->AddPendingEvent(e);
	return NULL;
}



