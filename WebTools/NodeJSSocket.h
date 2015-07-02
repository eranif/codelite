#ifndef NODEJSSOCKET_H
#define NODEJSSOCKET_H

#include <wx/event.h>
#include <wx/socket.h>
#include "smart_ptr.h"
#include "NodeJSHandlerBase.h"
#include "json_node.h"
#include <map>

class NodeJSDebugger;
class NodeJSSocket : public wxEvtHandler
{
    wxSocketClient* m_socket;
    wxString m_inBuffer;
    size_t m_sequence;
    NodeJSDebugger* m_debugger;
    std::map<size_t, NodeJSHandlerBase::Ptr_t> m_handlers;
    bool m_firstTimeConnected;

public:
    typedef SmartPtr<NodeJSSocket> Ptr_t;

protected:
    void Destroy();
    void OnSocketEvent(wxSocketEvent& event);
    void ReadSocketContent();
    void WriteReply(const wxString& reply);
    void ProcessInputBuffer();
    wxString GetResponse();
    
public:
    void WriteRequest(JSONElement& request, NodeJSHandlerBase::Ptr_t handler);

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

    size_t NextSequence() { return ++m_sequence; }
};

#endif // NODEJSSOCKET_H
