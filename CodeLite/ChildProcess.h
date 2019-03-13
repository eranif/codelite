#ifndef CHILD_PROCESS_H
#define CHILD_PROCESS_H

#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <wx/string.h>
#include "codelite_exports.h"
#include <wx/arrstr.h>
#include <wx/event.h>
#include "cl_command_event.h"
#include <atomic>
#include "cl_exception.h"
#include "file_logger.h"

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CHILD_PROCESS_STDOUT, clProcessEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CHILD_PROCESS_STDERR, clProcessEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CHILD_PROCESS_EXIT, clProcessEvent);

class IProcess;
class WXDLLIMPEXP_CL ChildProcess : public wxEvtHandler
{
private:
    IProcess* m_process = nullptr;

protected:
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessStderr(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);

    // wait for process termination
    int Wait();
    void Cleanup();

public:
    typedef wxSharedPtr<ChildProcess> Ptr_t;

    /**
     * @brief construct and start child process. argv[0] should contain the commands + arguments similar to main()
     */
    ChildProcess();
    virtual ~ChildProcess();

    /**
     * @brief return an execution line from an array of argv
     */
    static wxString BuildCommand(const wxArrayString& argv);

    /**
     * @brief start the process
     */
    void Start(const wxArrayString& argv, const wxString& workingDirectory = "");

    // stop the running process
    void Stop();

    /**
     * @brief is the process running?
     */
    bool IsRunning() const;

    void Write(const std::string& message);
    void Write(const wxString& message);
};
#endif // CHILD_PROCESS_H
