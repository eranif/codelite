#ifndef CLREMOTEBUILDER_HPP
#define CLREMOTEBUILDER_HPP

#include "asyncprocess.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <wx/event.h>
#include <wx/sharedptr.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clRemoteBuilder : public wxEvtHandler
{
    wxString m_sshAccount;
    IProcess* m_remoteProcess = nullptr;

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
