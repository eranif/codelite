#include "ConnectionString.hpp"
#include "StringUtils.hpp"

namespace dap
{
ConnectionString::ConnectionString(const string& connectionString)
    : m_port(-1)
    , m_isOK(false)
{
    DoParse(connectionString);
}

ConnectionString::~ConnectionString() {}

void ConnectionString::DoParse(const string& connectionString)
{
    m_isOK = false; // default
    // get the protocol part
    string protocol = StringUtils::BeforeFirst(connectionString, ':');
    if(protocol == "tcp") {
        m_protocol = kTcp;
    } else if(protocol == "unix") {
#ifdef _WIN32
        return;
#endif
        m_protocol = kUnixLocalSocket;
    } else {
        return;
    }

    string address = StringUtils::AfterFirst(connectionString, ':');
    address = address.substr(2);
    if(m_protocol == kUnixLocalSocket) {
        // The rest is the file path
        m_path = address;
        m_isOK = !m_path.empty();
    } else {
        // we now expect host[:port]
        m_host = StringUtils::BeforeFirst(address, ':');
        string port = StringUtils::AfterFirst(address, ':');
        if(!port.empty()) { m_port = atol(port.c_str()); }
        m_isOK = !m_host.empty() && (m_port != -1);
    }
}
}; // namespace dap