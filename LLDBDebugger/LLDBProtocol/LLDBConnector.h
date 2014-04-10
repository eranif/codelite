#ifndef LLDBCONNECTOR_H
#define LLDBCONNECTOR_H

#include <wx/event.h>
#include "LLDBCommand.h"
#include "clSocketClient.h"
#include "cl_socket_server.h"
#include "LLDBEnums.h"

class LLDBNetworkListenerThread;
class LLDBConnector : public wxEvtHandler
{
    clSocketClient::Ptr_t m_socket;
    LLDBNetworkListenerThread *m_thread;
    
public:
    LLDBConnector();
    virtual ~LLDBConnector();
    
    bool ConnectToDebugger();
    
    /**
     * @brief send command to codelite-lldb
     */
    void SendCommand(const LLDBCommand& command);
};

#endif // LLDBCONNECTOR_H
