#ifndef CLNODEJS_H
#define CLNODEJS_H

#include "asyncprocess.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <unordered_map>
#include <wx/event.h>
#include <wx/filename.h>
#include <wx/process.h>
#include <wx/string.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_NODE_COMMAND_TERMINATED, clProcessEvent);

class WXDLLIMPEXP_SDK clNodeJS : public wxEvtHandler
{
protected:
    class ProcessData
    {
    protected:
        wxFileName filename;
        wxString output;
        wxString uid;
        wxEvtHandler* sink = nullptr;

    public:
        ProcessData() {}
        virtual ~ProcessData() {}
        void SetFilename(const wxFileName& filename) { this->filename = filename; }
        void SetOutput(const wxString& output) { this->output = output; }
        const wxFileName& GetFilename() const { return filename; }
        const wxString& GetOutput() const { return output; }
        wxString& GetOutput() { return output; }
        void SetUid(const wxString& uid) { this->uid = uid; }
        const wxString& GetUid() const { return this->uid; }
        wxString& GetUid() { return this->uid; }
        void SetSink(wxEvtHandler* sink) { this->sink = sink; }
        wxEvtHandler* GetSink() const { return sink; }
    };

protected:
    bool m_initialised = false;
    wxFileName m_npm;
    wxFileName m_node;

    std::unordered_map<IProcess*, ProcessData> m_processes;

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
     * @brief execute NodeJS script. Return the wxProcess executed
     */
    wxProcess* RunScript(const wxArrayString& argv, const wxString& workingDirectory = "",
                         size_t execFlags = wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER | wxEXEC_HIDE_CONSOLE);

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
                    wxEvtHandler* sink = nullptr, const wxString& uid = "");

    /**
     * @brief install package without user interference
     */
    bool NpmSilentInstall(const wxString& package, const wxString& workingDirectory, const wxString& args,
                          wxEvtHandler* sink = nullptr, const wxString& uid = "");

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
