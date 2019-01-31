#ifndef WXCNETWORKREPLYTHREAD_H
#define WXCNETWORKREPLYTHREAD_H

#include <wx/thread.h>
#include "wxcLib/clSocketBase.h"
#include <wx/event.h>

class wxcNetworkReplyThread : public wxThread
{
    clSocketBase::Ptr_t m_socket;
    wxEvtHandler *      m_parent;
public:
    wxcNetworkReplyThread(wxEvtHandler *parent, socket_t sock);
    virtual ~wxcNetworkReplyThread();

public:
    virtual void* Entry();
    
    void Stop() {
#if wxCHECK_VERSION(3, 0, 0)
        if ( IsAlive() ) {
            Delete(NULL, wxTHREAD_WAIT_BLOCK);
        } else {
            Wait( wxTHREAD_WAIT_BLOCK );
        }
#else
        Delete();
#endif
    }
    
    void Start() {
        Create();
        Run();
    }
};

#endif // WXCNETWORKREPLYTHREAD_H
