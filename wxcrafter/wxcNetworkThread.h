#ifndef WXCNETWORKTHREAD_H
#define WXCNETWORKTHREAD_H

#include <wx/thread.h>

class wxcNetworkThread : public wxThread
{
protected:
    void ProcessMessage(const wxString& message);

public:
    wxcNetworkThread();
    virtual ~wxcNetworkThread();

    /**
     * @brief start the network thread
     */
    void Start()
    {
        Create();
        Run();
    }

    /**
     * @brief delete and wait for the thread
     */
    void Stop()
    {
        if(IsAlive()) {
            Delete(NULL, wxTHREAD_WAIT_BLOCK);
        } else {
            Wait(wxTHREAD_WAIT_BLOCK);
        }
    }

    // Main entry point
    virtual void* Entry();
};

#endif // WXCNETWORKTHREAD_H
