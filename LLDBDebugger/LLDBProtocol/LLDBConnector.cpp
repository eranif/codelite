#include "LLDBConnector.h"
#include "LLDBNetworkListenerThread.h"

LLDBConnector::LLDBConnector()
    : m_thread(NULL)
{
}

LLDBConnector::~LLDBConnector()
{
    // the order matters here, since both are using the same file descriptor
    // but only m_socket does the actual socket shutdown
    wxDELETE( m_thread );
    m_socket.reset(NULL);
}

bool LLDBConnector::ConnectToDebugger()
{
    clSocketClient *client = new clSocketClient();
    m_socket.reset( client );
    client->SetIp("127.0.0.1");
    client->SetPort( LLDB_PORT );
    if ( !client->Connect() )
        return false;

    // Start the lldb event thread

    // and start a listener thread which will read replies
    // from codelite-lldb and convert them into LLDBEvent
    socket_t fd = m_socket->GetSocket();
    m_thread = new LLDBNetworkListenerThread(this, fd);
    m_thread->Start();
    return true;
}

void LLDBConnector::SendCommand(const LLDBCommand& command)
{
    try {
        m_socket->WriteMessage( command.ToJSON().format() );

    } catch ( clSocketException &e ) {
        wxUnusedVar( e );
    }
}
