
#ifdef __WXMSW__
#   include "winprocess_impl.h"
#else
#   include "unixprocess_impl.h"
#endif

IProcess* CreateAsyncProcess(wxEvtHandler *parent, const wxString& cmd, const wxString &workingDir)
{
#ifdef __WXMSW__
	wxString errMsg;
	return WinProcessImpl::Execute(parent, cmd, errMsg, workingDir);
#else
	return UnixProcessImpl::Execute(parent, cmd, workingDir);
#endif
}
