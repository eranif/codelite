#ifndef LLDBCONNECTOR_H
#define LLDBCONNECTOR_H

#include <wx/event.h>
#include "LLDBCommand.h"
#include "clSocketClient.h"
#include "cl_socket_server.h"

class LLDBConnector : public wxEvtHandler
{
    clSocketClient m_socket;
    
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
