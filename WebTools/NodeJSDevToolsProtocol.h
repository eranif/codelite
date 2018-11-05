#ifndef NODEJSDEVTOOLSPROTOCOL_H
#define NODEJSDEVTOOLSPROTOCOL_H

#include "MessageManager.h"
#include "SocketAPI/clWebSocketClient.h"
#include <wx/arrstr.h>

class NodeJSDevToolsProtocol
{
    long message_id = 0;
    MessageManager m_handlers;

public:
    NodeJSDevToolsProtocol();
    virtual ~NodeJSDevToolsProtocol();

    void SendStartCommands(clWebSocketClient& socket);
    void ProcessMessage(const wxString& msg, clWebSocketClient& socket);
};

#endif // NODEJSDEVTOOLSPROTOCOL_H
