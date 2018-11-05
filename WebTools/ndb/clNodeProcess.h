#ifndef CLNODEPROCESS_H
#define CLNODEPROCESS_H

#include <asyncprocess.h>
#include <cl_command_event.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/arrstr.h>

#if defined(__WXGTK__) || defined(__WXOSX__)
#define USE_FORK 1
class UnixHelperThread;
#define HELPER_TYPE UnixHelperThread
#else
#define USE_FORK 0
#define HELPER_TYPE IProcess
#endif

class clNodeProcess : public wxEvtHandler
{
    HELPER_TYPE* m_helperThread = nullptr;
    wxString m_buffer;
    wxArrayString m_commands;

protected:
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);

public:
    clNodeProcess();
    virtual ~clNodeProcess();

    /**
     * @brief start nodejs process for a given script
     */
    void Debug(const wxString& nodejs, const wxString& script);

    void Stop();

    /**
     * @brief tell nodejs to execute a command
     */
    void Write(const wxString& command);
};

#endif // CLNODEPROCESS_H
