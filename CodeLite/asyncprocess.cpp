
class wxEvtHandler;
class IProcess;
#include <wx/string.h>

#ifdef __WXMSW__
#   include "winprocess_impl.h"
#else
#   include "unixprocess_impl.h"
#endif

// statics
wxCriticalSection  IProcess::m_cs;
std::map<int, int> IProcess::m_exitCodeMap;

IProcess* CreateAsyncProcess(wxEvtHandler *parent, const wxString& cmd, IProcessCreateFlags flags, const wxString &workingDir)
{
#ifdef __WXMSW__
	wxString errMsg;
	return WinProcessImpl::Execute(parent, cmd, errMsg, flags, workingDir);
#else
	return UnixProcessImpl::Execute(parent, cmd, flags, workingDir);
#endif
}

// Static methods:
bool IProcess::GetProcessExitCode(int pid, int &exitCode)
{
	wxCriticalSectionLocker locker(m_cs);

	std::map<int, int>::iterator iter = m_exitCodeMap.find(pid);
	if(iter == m_exitCodeMap.end()) {
		return false;
	}
	exitCode = iter->second;
	return true;
}

void IProcess::SetProcessExitCode(int pid, int exitCode)
{
	wxCriticalSectionLocker locker(m_cs);
	m_exitCodeMap[pid] = exitCode;
}

