#ifndef CLREMOTEBUILDER_HPP
#define CLREMOTEBUILDER_HPP

#include "AsyncProcess/asyncprocess.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <memory>
#include <wx/event.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clRemoteBuilder : public wxEvtHandler
{
    wxString m_sshAccount;
    std::unique_ptr<IProcess> m_remoteProcess;

public:
    using Ptr_t = std::shared_ptr<clRemoteBuilder>;

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
