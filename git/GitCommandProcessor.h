#ifndef GITCOMMANDPROCESSOR_H
#define GITCOMMANDPROCESSOR_H

#include <wx/event.h>
#include "asyncprocess.h"
#include <wx/string.h>

class GitPlugin;
class GitCommandProcessor : public wxEvtHandler
{
    GitPlugin* m_plugin;
    GitCommandProcessor* m_next;
    GitCommandProcessor* m_prev;
    IProcess* m_process;
    wxString m_output;
    wxString m_command;
    wxString m_workingDirectory;
    size_t m_processFlags;
    
protected:
    void DeleteChain();
public:
    GitCommandProcessor(GitPlugin* plugin,
                        const wxString& command,
                        const wxString& wd,
                        size_t processFlags = IProcessCreateDefault);
    virtual ~GitCommandProcessor();

    void ExecuteCommand();

    /**
     * @brief link two command so they will be executed one after the other
     * @return a pointer to the next command ("next")
     */
    GitCommandProcessor* Link(GitCommandProcessor* next) ;
    
    DECLARE_EVENT_TABLE()
    void OnProcessOutput(wxCommandEvent& event);
    void OnProcessTerminated(wxCommandEvent& event);
};

#endif // GITCOMMANDPROCESSOR_H
