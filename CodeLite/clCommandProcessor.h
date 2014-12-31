#ifndef CLCOMMANDPROCESSOR_H
#define CLCOMMANDPROCESSOR_H

#include <wx/event.h>
#include "asyncprocess.h"
#include <wx/string.h>
#include "codelite_exports.h"
#include "cl_command_event.h"

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_COMMAND_PROCESSOR_ENDED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_COMMAND_PROCESSOR_OUTPUT, clCommandEvent);

class WXDLLIMPEXP_CL clCommandProcessor : public wxEvtHandler
{
    clCommandProcessor* m_next;
    clCommandProcessor* m_prev;
    IProcess* m_process;
    wxString m_command;
    wxString m_workingDirectory;
    size_t m_processFlags;

protected:
    void DeleteChain();
    clCommandProcessor* GetFirst();
    
public:
    clCommandProcessor(const wxString& command, const wxString& wd, size_t processFlags = IProcessCreateDefault);
    virtual ~clCommandProcessor();

    void ExecuteCommand();

    /**
     * @brief link two command so they will be executed one after the other
     * @return a pointer to the next command ("next")
     */
    clCommandProcessor* Link(clCommandProcessor* next);

    DECLARE_EVENT_TABLE()
    void OnProcessOutput(wxCommandEvent& event);
    void OnProcessTerminated(wxCommandEvent& event);
};

#endif // CLCOMMANDPROCESSOR_H
