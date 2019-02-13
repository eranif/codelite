#ifndef CLNODEJS_H
#define CLNODEJS_H

#include <wx/event.h>
#include <wx/string.h>
#include <wx/filename.h>
#include "asyncprocess.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clNodeJS : public wxEvtHandler
{
    bool m_initialised = false;
    wxFileName m_npm;
    wxFileName m_node;
    IProcess* m_process = nullptr;

protected:
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);

private:
    clNodeJS();
    void BindEvents();
    void UnBindEvents();

public:
    static clNodeJS& Get();
    virtual ~clNodeJS();

    /**
     * @brief initialise this class by finding all the binaries required
     */
    bool Initialise();

    /**
     * @brief terminate this instance
     */
    void Shutdown();

    bool IsRunning() const { return m_process != nullptr; }

    /**
     * @brief install an npm package
     */
    bool NpmInstall(const wxString& package, const wxString& workingDirectory, const wxString& args);
    bool IsInitialised() const { return m_initialised; }
    const wxFileName& GetNode() const { return m_node; }
    const wxFileName& GetNpm() const { return m_npm; }
};

#endif // CLNODEJS_H
