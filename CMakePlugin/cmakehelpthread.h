#ifndef CMAKEHELPTHREAD_H
#define CMAKEHELPTHREAD_H

#include <wx/thread.h> // Base class: wxThread
#include <wx/string.h>

class CMake;
class CMakeHelpThread : public wxThread
{
    bool    m_busy;
    CMake*  m_cmake;
    wxString m_command;
    
public:
    CMakeHelpThread(CMake* cmake, const wxString &command);
    virtual ~CMakeHelpThread();
    bool IsBusy() const {
        return m_busy;
    }
    
public:
    virtual void* Entry();
    
    /**
     * @brief start the worker thread
     */
    void Start() {
        Create();
        Run();
    }
    
    /**
     * @brief stop and join the thread
     */
    void Stop() {
        if ( IsAlive() ) {
            Delete(NULL, wxTHREAD_WAIT_BLOCK);
        } else {
            Wait(wxTHREAD_WAIT_BLOCK);
        }
    }
};

#endif // CMAKEHELPTHREAD_H
