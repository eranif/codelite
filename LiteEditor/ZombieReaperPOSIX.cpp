#include "ZombieReaperPOSIX.h"

#ifndef __WXMSW__

#include <sys/wait.h>
#include "file_logger.h"

ZombieReaperPOSIX::ZombieReaperPOSIX()
    : wxThread(wxTHREAD_JOINABLE)
{
    CL_DEBUG("ZombieReaperPOSIX: started");
}

ZombieReaperPOSIX::~ZombieReaperPOSIX()
{
    
}

void* ZombieReaperPOSIX::Entry()
{
    while ( !TestDestroy() ) {
        int status (0);
        pid_t pid = ::waitpid((pid_t)-1, &status, WNOHANG);
        if ( pid > 0 ) {
            CL_DEBUG("ZombieReaperPOSIX: process %d exited with status code %d", (int)pid, status);
        }
        wxThread::Sleep(100);
    }
    CL_DEBUG("ZombieReaperPOSIX: going down");
    return NULL;
}

void ZombieReaperPOSIX::Stop()
{
    if ( IsAlive() ) {
        Delete(NULL, wxTHREAD_WAIT_BLOCK);
    } else {
        Wait(wxTHREAD_WAIT_BLOCK);
    }
}
#endif
