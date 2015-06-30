#ifndef NODEJSSOCKET_H
#define NODEJSSOCKET_H

#include <wx/event.h>
#include <wx/socket.h>
#include "smart_ptr.h"

class NodeJSDebugger;
class NodeJSSocket : public wxEvtHandler
{
    wxSocketClient* m_socket;
    wxString m_inBuffer;
    size_t m_sequence;
    NodeJSDebugger* m_debugger;
    
public:
    typedef SmartPtr<NodeJSSocket> Ptr_t;
    
protected:
    void Destroy();
    void OnSocketEvent(wxSocketEvent& event);
    void ReadSocketContent();
    void WriteReply(const wxString& reply);
    void ProcessInputBuffer();
    
public:
    NodeJSSocket(NodeJSDebugger* debugger);
    virtual ~NodeJSSocket();
    
    /**
     * @brief do we have an active to the debuger?
     */
    bool IsConnected() const { return m_socket; }
    
    /**
     * @brief connect to nodejs debugger on a given IP and port
     */
    bool Connect(const wxString& ip, int port);
    
    /**
     * @brief return last error occured
     */
    wxString LastError() const;
    
    /**
     * @brief return last error code
     */
    wxSocketError LastErrorCode() const;
};

#endif // NODEJSSOCKET_H
