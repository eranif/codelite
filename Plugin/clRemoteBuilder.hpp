#ifndef CLREMOTEBUILDER_HPP
#define CLREMOTEBUILDER_HPP

#include "codelite_exports.h"
#include <wx/event.h>
#include "asyncprocess.h"
#include <wx/string.h>
#include "cl_command_event.h"
#include <wx/sharedptr.h>

#if USE_SFTP
#include "cl_ssh.h"
#endif

class WXDLLIMPEXP_SDK clRemoteBuilder : public wxEvtHandler
{
    wxString m_sshAccount;
    IProcess* m_remoteProcess = nullptr;
#if USE_SFTP
    clSSH::Ptr_t m_ssh;
#endif
public:
    typedef wxSharedPtr<clRemoteBuilder> Ptr_t;

protected:
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);

public:
    clRemoteBuilder();
    virtual ~clRemoteBuilder();

    bool IsRunning() const;
    void Build(const wxString& sshAccount, const wxString& command, const wxString& workingDirectory);
    void Stop();
};

#endif // CLREMOTEBUILDER_HPP
