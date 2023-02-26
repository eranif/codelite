#ifndef PROCESS_LAUNCHER_HPP
#define PROCESS_LAUNCHER_HPP

#include "asyncprocess.h"
#include "codelite_exports.h"

#include <wx/event.h>

struct WXDLLIMPEXP_CL ProcessHandle
{
    int pid = wxNOT_FOUND;
};

class WXDLLIMPEXP_CL ProcessLauncher
{
public:
    static ProcessHandle LaunchNonInteractive(wxEvtHandler* parent, const wxString& cmd, size_t flags,
                                              const wxString& workingDir, const clEnvList_t* env,
                                              const wxString& sshAccountName);

    static ProcessHandle LaunchNonInteractive(wxEvtHandler* parent, const wxArrayString& cmd, size_t flags,
                                              const wxString& workingDir, const clEnvList_t* env,
                                              const wxString& sshAccountName);
};
#endif
