#ifndef GITCOMMAND_H
#define GITCOMMAND_H

#include "smart_ptr.h"
#include <wx/event.h>
#include "cl_command_event.h"

class IProcess;
class GitCommand : public wxEvtHandler
{
public:
    typedef SmartPtr<GitCommand> Ptr_t;
    GitCommand::Ptr_t m_next;

protected:
    wxString m_command;
    wxString m_workingDirectory;
    IProcess* m_process;
    
protected:
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);
    bool Execute();
    
public:
    GitCommand();
    virtual ~GitCommand();

    /**
     * @brief set the next command in chain
     */
    void SetNext(GitCommand::Ptr_t next) { m_next = next; }
    GitCommand::Ptr_t GetNext() { return m_next; }

    /**
     * @brief process git output
     */
    virtual void ProcessOutput(const wxString& text) = 0;

    /**
     * @brief git process terminated
     */
    virtual void ProcessTerminated() = 0;
    
    /**
     * @brief is the current command in progress?
     */
    bool IsRunning() const { return m_process != NULL; }
};

#endif // GITCOMMAND_H
