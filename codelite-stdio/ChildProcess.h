#ifndef CHILDPROCESS_H
#define CHILDPROCESS_H

#include <wx/arrstr.h>
#include <wx/event.h>
#include "cl_command_event.h"
#include <asyncprocess.h>

class ChildProcess : public wxEvtHandler
{
    IProcess* m_process = nullptr;
    
protected:
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessStderr(clProcessEvent& event);

public:
    ChildProcess();
    virtual ~ChildProcess();

    void Start(const wxArrayString& args);
};

#endif // CHILDPROCESS_H
