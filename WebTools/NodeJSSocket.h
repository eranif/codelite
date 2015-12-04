//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSSocket.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef NODEJSSOCKET_H
#define NODEJSSOCKET_H

#include <wx/event.h>
#include "smart_ptr.h"
#include "NodeJSHandlerBase.h"
#include "json_node.h"
#include <map>
#include "SocketAPI/clSocketClientAsync.h"
#include "macros.h"

class NodeJSDebugger;
class NodeJSSocket : public wxEvtHandler
{
    clSocketClientAsync m_socket;
    bool m_connected;
    wxString m_inBuffer;
    size_t m_sequence;
    NodeJSDebugger* m_debugger;
    std::map<size_t, NodeJSHandlerBase::Ptr_t> m_handlers;
    bool m_firstTimeConnected;
    wxString m_errorString;
    wxStringSet_t m_noBacktraceCommands;
    
public:
    typedef SmartPtr<NodeJSSocket> Ptr_t;

protected:
    void Destroy();
    void OnSocketConnected(clCommandEvent& event);
    void OnSocketError(clCommandEvent& event);
    void OnSocketConnectionLost(clCommandEvent& event);
    void OnSocketInput(clCommandEvent& event);
    void OnSocketConnectError(clCommandEvent& event);

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
    bool IsConnected() const { return m_connected; }

    /**
     * @brief connect to nodejs debugger on a given IP and port
     */
    void Connect(const wxString& ip, int port);
    
    /**
     * @brief disconnect the debugger
     */
    void Shutdown();
    
    const wxString& GetErrorString() const { return m_errorString; }
    
    size_t NextSequence() { return ++m_sequence; }
};

#endif // NODEJSSOCKET_H
