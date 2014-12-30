#ifndef GITCOMMANDPROCESSOR_H
#define GITCOMMANDPROCESSOR_H

#include <wx/event.h>
#include "asyncprocess.h"
#include <wx/string.h>

class GitPlugin;
class GitCommandProcessor : public wxEvtHandler
{
    GitPlugin* m_plugin;
    IProcess* m_process;
    wxString m_output;
    
public:
    GitCommandProcessor(GitPlugin* plugin);
    virtual ~GitCommandProcessor();

    void ExecuteCommand(const wxString& command, const wxString& workingDirectory);
    
    DECLARE_EVENT_TABLE()
    void OnProcessOutput(wxCommandEvent &event);
    void OnProcessTerminated(wxCommandEvent &event);
};

#endif // GITCOMMANDPROCESSOR_H
