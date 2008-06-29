#include "pipedprocess2.h"
#include <wx/txtstrm.h>
PipedProcessTS::PipedProcessTS(int id, const wxString &cmdLine)
		: PipedProcess(id, cmdLine)
		, m_isRunning(false)
{
}

PipedProcessTS::~PipedProcessTS()
{
}

long PipedProcessTS::Start(bool hide)
{
	wxASSERT_MSG(wxThread::IsMain(), wxT("Start() can only be called from main thread"));
	
	long pid = PipedProcess::Start(hide);
	{
		if (pid == 0) {
			// process terminated
			m_isRunning = false;
		} else {
			m_isRunning = true;
		}
	}
	return pid;
}

bool PipedProcessTS::IsRunning()
{
	return m_isRunning;
}

bool PipedProcessTS::Write(const wxString& msg)
{
	if( !IsRunning() ){
		return false;
	}
	
	if(GetOutputStream()){
		wxTextOutputStream os(*GetOutputStream());
		os.WriteString(msg + wxT("\n"));
		return true;
	}
	return false;
}

bool PipedProcessTS::ReadAll(wxString& msg)
{
	return PipedProcess::ReadAll(msg);
}
