#ifndef NODEJSDEVTOOLSPROTOCOL_H
#define NODEJSDEVTOOLSPROTOCOL_H

#include "MessageManager.h"
#include "SocketAPI/clWebSocketClient.h"
#include <unordered_map>
#include <wx/arrstr.h>

typedef std::function<void(const wxString&)> CommandHandlerFunc_t;
class CommandHandler
{
public:
    CommandHandlerFunc_t action = nullptr;
    long m_commandID = wxNOT_FOUND;

    CommandHandler(const wxString& command, long commandID, const CommandHandlerFunc_t& func)
        : action(func)
        , m_commandID(commandID)
    {
    }
};

class NodeJSDevToolsProtocol
{
    long message_id = 0;
    MessageManager m_handlers;
    std::unordered_map<long, CommandHandler> m_waitingReplyCommands;

protected:
    void SendSimpleCommand(clWebSocketClient& socket, const wxString& command);

public:
    NodeJSDevToolsProtocol();
    virtual ~NodeJSDevToolsProtocol();
    void Clear();
    
    void SendStartCommands(clWebSocketClient& socket);
    void ProcessMessage(const wxString& msg, clWebSocketClient& socket);
    void Next(clWebSocketClient& socket);
    void StepIn(clWebSocketClient& socket);
    void StepOut(clWebSocketClient& socket);
    void Continue(clWebSocketClient& socket);
};

#endif // NODEJSDEVTOOLSPROTOCOL_H
