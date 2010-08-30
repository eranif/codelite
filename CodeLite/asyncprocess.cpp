
class wxEvtHandler;
class IProcess;
#include <wx/string.h>

#ifdef __WXMSW__
#   include "winprocess_impl.h"
#else
#   include "unixprocess_impl.h"
#endif

IProcess* CreateAsyncProcess(wxEvtHandler *parent, const wxString& cmd, IProcessCreateFlags flags, const wxString &workingDir)
{
#ifdef __WXMSW__
	wxString errMsg;
	return WinProcessImpl::Execute(parent, cmd, errMsg, flags, workingDir);
#else
	return UnixProcessImpl::Execute(parent, cmd, flags, workingDir);
#endif
}
