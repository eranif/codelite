#ifndef LLDBCONNECTOR_H
#define LLDBCONNECTOR_H

#include <wx/event.h>
#include "LLDBCommand.h"
#include "clSocketClient.h"
#include "cl_socket_server.h"
#include "LLDBEnums.h"
#include "LLDBBreakpoint.h"

class LLDBNetworkListenerThread;
class LLDBConnector : public wxEvtHandler
{
    clSocketClient::Ptr_t m_socket;
    LLDBNetworkListenerThread *m_thread;
    LLDBBreakpoint::Vec_t m_breakpoints;
    
protected:
    bool IsBreakpointExists(LLDBBreakpoint::Ptr_t bp) const;
    LLDBBreakpoint::Vec_t::const_iterator FindBreakpoint(LLDBBreakpoint::Ptr_t bp) const;
    LLDBBreakpoint::Vec_t::iterator FindBreakpoint(LLDBBreakpoint::Ptr_t bp);
    
public:
    LLDBConnector();
    virtual ~LLDBConnector();
    
    /**
     * @brief invalidate all breakpoints (remove their internal id)
     */
    void InvalidateBreakpoints();

    /**
     * @brief establish connection to codelite-lldb server
     * @return 
     */
    bool ConnectToDebugger();

    /**
     * @brief apply the breakpoints
     * Note this does not assure that the breakpoints will actually be added
     * codelite-lldb will might need to interrupt the process in order to apply them
     * so a LLDB_STOPPED event will be sent with interrupt-reason set to 
     */
    void ApplyBreakpoints();
    
    /**
     * @brief add a breakpoint to the list (do not apply them yet)
     */
    void AddBreakpoint(LLDBBreakpoint::Ptr_t breakpoint);
    
    // aliases
    void AddBreakpoint(const wxString &location) {
        LLDBBreakpoint::Ptr_t bp( new LLDBBreakpoint(location) );
        AddBreakpoint( bp );
    }
    void AddBreakpoint(const wxFileName& filename, int line) {
        LLDBBreakpoint::Ptr_t bp( new LLDBBreakpoint(filename, line) );
        AddBreakpoint( bp );
    }
    
    /**
     * @brief send command to codelite-lldb
     */
    void SendCommand(const LLDBCommand& command);
    
    /**
     * @brief issue a 'Continue' command
     */
    void Continue();
    
    /**
     * @brief stop the debugger
     */
    void Stop();
};

#endif // LLDBCONNECTOR_H
