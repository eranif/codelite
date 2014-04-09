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
    return m_socket.Connect();
}
