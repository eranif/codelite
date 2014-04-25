#ifndef LLDBNETWORKSERVERTHREAD_H
#define LLDBNETWORKSERVERTHREAD_H

#ifndef __WXMSW__

#include <wx/thread.h>
#include "SocketAPI/clSocketBase.h"

class CodeLiteLLDBApp;
class LLDBNetworkServerThread : public wxThread
{
    CodeLiteLLDBApp* m_app;
    clSocketBase::Ptr_t m_socket;
    
public:
    LLDBNetworkServerThread(CodeLiteLLDBApp* app, socket_t fd);
    virtual ~LLDBNetworkServerThread();

    virtual void* Entry();

    void Start() {
        Create();
        Run();
    }
};
#endif // !__WXMSW__
#endif // LLDBNETWORKSERVERTHREAD_H
