#ifndef DAPPROCESS_HPP
#define DAPPROCESS_HPP

#include "cl_command_event.h"

#include <wx/process.h>

/// helper class that notifies when a process was terminated
class DapProcess : public wxProcess
{
    wxEvtHandler* m_notifier = nullptr;

public:
    DapProcess(wxEvtHandler* notifier);
    virtual ~DapProcess();

    void OnTerminate(int pid, int status) override;
};

#endif // DAPPROCESS_HPP
