#ifndef XDEBUG_COMM_THREAD_H
#define XDEBUG_COMM_THREAD_H

#include <wx/thread.h>
#include <wx/msgqueue.h>
#include <wx/string.h>
#include <string>
#include "SocketAPI/clSocketBase.h"
#include "SocketAPI/clSocketServer.h"

#if defined(__WXMSW__)
#include <wx/msw/winundef.h>
#endif

class XDebugManager;
class XDebugComThread : public wxThread
{
    XDebugManager* m_xdebugMgr;
    wxMessageQueue<wxString> m_queue;
    int m_port;
    clSocketServer m_server;
    
protected:
    void DoSendCommand(const wxString &command, clSocketBase::Ptr_t client);
    bool DoReadReply(std::string& reply, clSocketBase::Ptr_t client);
    void Stop();
    
public:
    XDebugComThread(XDebugManager* xdebugMgr, int port) 
        : wxThread(wxTHREAD_JOINABLE)
        , m_xdebugMgr(xdebugMgr)
        , m_port(port)
    {}
    
    virtual ~XDebugComThread();
    
    void SendMsg( const wxString &msg );
    
    void* Entry();
    void Start() {
        Create();
        Run();
    }
    
};

#endif // XDEBUG_COMM_THREAD_H
