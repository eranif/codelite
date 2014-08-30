#ifndef PHPEXECUTOR_H
#define PHPEXECUTOR_H

#include <wx/event.h>
#include <wx/process.h>
#include "php_project.h"

class PHPExecutor : public wxEvtHandler
{
    wxProcess *m_process;
    
protected:
    void OnProcessTerminated(wxProcessEvent &e);
    bool RunRUL(PHPProject::Ptr_t pProject, const wxString& xdebugSessionName);
    
public:
    PHPExecutor();
    virtual ~PHPExecutor();

    bool Exec(const wxString& projectName, const wxString& xdebugSessionName, bool neverPauseOnExit);
    bool IsRunning() const;
    void Stop();
};

#endif // PHPEXECUTOR_H
