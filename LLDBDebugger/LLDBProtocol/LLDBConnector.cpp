#include "LLDBConnector.h"

LLDBConnector::LLDBConnector()
{
}

LLDBConnector::~LLDBConnector()
{
}

bool LLDBConnector::ConnectToDebugger()
{
    m_socket.SetIp("127.0.0.1");
    m_socket.SetPort(LLDB_PORT);
    if ( !m_socket.Connect() )
        return false;
    
    // Start the lldb event thread
    
}
