#ifndef ZOMBIEREAPERPOSIX_H
#define ZOMBIEREAPERPOSIX_H

#ifndef __WXMSW__

#include <wx/event.h>
#include <wx/thread.h>
#include "codelite_exports.h"
#include <wx/process.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CL_PROCESS_TERMINATED, wxProcessEvent);

class WXDLLIMPEXP_CL ZombieReaperPOSIX : public wxThread
{
public:
    virtual void* Entry();
    ZombieReaperPOSIX();
    virtual ~ZombieReaperPOSIX();

    void Start() {
        Create();
        Run();
    }
    
    void Stop();
};
#endif 
#endif // ZOMBIEREAPERPOSIX_H

