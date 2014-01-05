#ifndef CMAKEHELPTHREAD_H
#define CMAKEHELPTHREAD_H

#include <wx/thread.h> // Base class: wxThread
#include <wx/string.h>
#include "CMake.h"

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
    
protected:
    /**
     * @brief Loads help of type from command into list.
     *
     * @param command CMake command.
     * @param type    Help type.
     * @param list    Output variable.
     * @return false if the thread was requested to terminate while executing cmake help commands
     */
    bool LoadList(const wxString& command, const wxString& type, CMake::HelpMap& list);

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
