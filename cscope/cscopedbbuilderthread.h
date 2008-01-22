#ifndef __cscopedbbuilderthread__
#define __cscopedbbuilderthread__


#include "cscopeentrydata.h"
#include "wx/thread.h"
#include "wx/event.h"
#include <map>
#include <vector>

extern int wxEVT_CSCOPE_THREAD_DB_BUILD_DONE;

typedef std::map<wxString, std::vector< CscopeEntryData >* > CscopeResultTable;

class CscopeDbBuilderThread : public wxThread
{
	wxEvtHandler *m_owner;
	wxString m_cmd;
	wxString m_outfile;
	wxString m_workingDir;
	
protected:
	void* Entry();
	CscopeResultTable* ParseResults(const wxArrayString &output);
	
public:
	CscopeDbBuilderThread(wxEvtHandler *owner);
	~CscopeDbBuilderThread();
		
	//Setters
	void SetCmd(const wxString& cmd) {this->m_cmd = cmd;}
	void SetWorkingDir(const wxString& workingDir) {this->m_workingDir = workingDir;}
	
	//Getters
	const wxString& GetCmd() const {return m_cmd;}
	const wxString& GetWorkingDir() const {return m_workingDir;}
};


#endif // __cscopedbbuilderthread__
