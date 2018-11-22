#include "clNodeProcess.h"
#include <asyncprocess.h>
#include <globals.h>
#include <processreaderthread.h>
#include <wx/crt.h>
#include <wx/event.h>
#include <wx/string.h>
#include <json_node.h>

#if USE_FORK
#include <clJoinableThread.h>
#include <cl_command_event.h>
#include <codelite_exports.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>

char** make_argv(const std::string& cmd, int& _argc);
int StartProcess(const wxString& nodejs, const wxString& scriptPath, int& pid);

class UnixHelperThread : public clJoinableThread
{
    wxEvtHandler* m_owner = nullptr;
    int m_pid = wxNOT_FOUND;
    int m_fd = wxNOT_FOUND;

public:
    UnixHelperThread(wxEvtHandler* owner, int processId, int fd)
        : m_owner(owner)
        , m_pid(processId)
        , m_fd(fd)
    {
    }

    virtual ~UnixHelperThread() {}

    void Terminate() { Stop(); }
    
    void Write(const wxString& command)
    {
        wxString b = command;
        b.Trim().Trim(false);
        b << "\n";
        const char* data = b.mb_str(wxConvUTF8).data();
        write(m_fd, data, strlen(data));
    }
    
    void* Entry()
    {
        wxString buffer;
        while(!TestDestroy()) {
            char input;
            // char output;
            fd_set read_set;
            memset(&read_set, 0, sizeof(read_set));
            FD_SET(m_fd, &read_set);
            timeval tv = { 1, 0 };
            int rc = select(m_fd + 1, &read_set, NULL, NULL, &tv);
            if(rc < 0) {
                break;
            } else if(rc == 0) {
                // timeout
                if(!buffer.empty()) {
                    if(m_owner) {
                        wxString mod_buffer;
                        clStripTerminalColouring(buffer, mod_buffer);
                        clProcessEvent e(wxEVT_ASYNC_PROCESS_OUTPUT);
                        e.SetProcess(nullptr);
                        e.SetOutput(mod_buffer);
                        m_owner->AddPendingEvent(e);
                    }
                    buffer.clear();
                }
            } else {
                if(FD_ISSET(m_fd, &read_set)) {
                    if(read(m_fd, &input, 1) == 1) { buffer << input; }
                }
            }
        }

        kill(m_pid, SIGKILL); // send SIGKILL signal to the child process
        int status = 0;
        waitpid(m_pid, &status, 0);
        if(m_owner) {
            clProcessEvent endEvent(wxEVT_ASYNC_PROCESS_TERMINATED);
            endEvent.SetProcess(nullptr);
            m_owner->AddPendingEvent(endEvent);
        }
        return NULL;
    }
};
#endif

clNodeProcess::clNodeProcess()
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clNodeProcess::OnProcessTerminated, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clNodeProcess::OnProcessOutput, this);
}

clNodeProcess::~clNodeProcess()
{
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clNodeProcess::OnProcessTerminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &clNodeProcess::OnProcessOutput, this);
}

void clNodeProcess::Debug(const wxString& nodejs, const wxString& script)
{
#if USE_FORK
    int pid = wxNOT_FOUND;
    int fd = ::StartProcess(nodejs, script, pid);
    m_helperThread = new UnixHelperThread(this, pid, fd);
    m_helperThread->Start();
#else
    wxString oneliner = nodejs;
    oneliner << " inspect " << script;
    m_helperThread = ::CreateAsyncProcess(this, oneliner, IProcessCreateDefault);
#endif
}

void clNodeProcess::OnProcessOutput(clProcessEvent& event)
{
    wxPrintf("%s\n", event.GetOutput());
    m_buffer << event.GetOutput();
    m_buffer.Trim();
    if(m_buffer.EndsWith("debug>")) {
        // The buffer ends with the prompt, let the user know that he has control
        wxPrintf("{\"response_type\": 1}\n");
    }
}

void clNodeProcess::OnProcessTerminated(clProcessEvent& event)
{
    wxPrintf("Process terminated\n");
    wxDELETE(m_helperThread);
}

void clNodeProcess::Stop() { m_helperThread->Terminate(); }

void clNodeProcess::Write(const wxString& command)
{
    JSONRoot root(command);
    JSONElement element = root.toElement();
    int commandId = element.namedObject("id").toInt();
    wxString commandString = element.namedObject("command").toString();
    m_helperThread->Write(commandString);
}
