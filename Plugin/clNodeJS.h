#ifndef CLNODEJS_H
#define CLNODEJS_H

#include <wx/event.h>
#include <wx/string.h>
#include <wx/filename.h>
#include "asyncprocess.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include <unordered_map>

class WXDLLIMPEXP_SDK clNodeJS : public wxEvtHandler
{
protected:
    struct LintInfo {
        wxFileName filename;
        wxString m_output;
    };

protected:
    bool m_initialised = false;
    wxFileName m_npm;
    wxFileName m_node;

    std::unordered_map<IProcess*, LintInfo> m_processes;

protected:
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);
    void ProcessLintOuput(const wxFileName& fn, const wxString& output);

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
    bool Initialise(const wxArrayString& hints = {});

    /**
     * @brief terminate this instance
     */
    void Shutdown();

    /**
     * @brief install an npm package
     */
    bool NpmInstall(const wxString& package, const wxString& workingDirectory, const wxString& args,
                    wxEvtHandler* sink = nullptr);

    /**
     * @brief run npm init in a directory
     */
    bool NpmInit(const wxString& workingDirectory, wxEvtHandler* sink = nullptr);

    /**
     * @brief lint file. The error are reported in the output view
     */
    void LintFile(const wxFileName& filename);

    bool IsInitialised() const { return m_initialised; }
    const wxFileName& GetNode() const { return m_node; }
    const wxFileName& GetNpm() const { return m_npm; }
};

#endif // CLNODEJS_H
