#ifndef ZOMBIEREAPERPOSIX_H
#define ZOMBIEREAPERPOSIX_H

#ifndef __WXMSW__
#include <wx/thread.h>

class ZombieReaperPOSIX : public wxThread
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

