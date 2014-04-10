#include "LLDBConnector.h"
#include "LLDBNetworkListenerThread.h"

LLDBConnector::LLDBConnector()
    : m_thread(NULL)
{
}

LLDBConnector::~LLDBConnector()
{
    wxDELETE( m_thread );
}

bool LLDBConnector::ConnectToDebugger()
{
    m_socket.SetIp("127.0.0.1");
    m_socket.SetPort(LLDB_PORT);
    if ( !m_socket.Connect() )
        return false;

    // Start the lldb event thread

    // Release the fd into our custody
    // and start a listener thread which will accept replies
    // from codelite-lldb and convert them into LLDBEvent
    int fd = m_socket.Release();
    m_thread = new LLDBNetworkListenerThread(this, fd);
    m_thread->Start();

    return true;
}
