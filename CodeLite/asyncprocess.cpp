
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

IProcess* CreateAsyncProcessCB(wxEvtHandler *parent, IProcessCallback* cb, const wxString& cmd, IProcessCreateFlags flags, const wxString &workingDir)
{
#ifdef __WXMSW__
    wxString errMsg;
    return WinProcessImpl::Execute(parent, cmd, errMsg, flags, workingDir, cb);
#else
    return UnixProcessImpl::Execute(parent, cmd, flags, workingDir, cb);
#endif
}

// Static methods:
bool IProcess::GetProcessExitCode(int pid, int &exitCode)
{
    wxUnusedVar(pid);
    wxUnusedVar(exitCode);

    exitCode = 0;
    return true;
}

void IProcess::SetProcessExitCode(int pid, int exitCode)
{
    wxUnusedVar(pid);
    wxUnusedVar(exitCode);
}
