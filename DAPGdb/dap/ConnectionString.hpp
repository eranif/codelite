#ifndef CLCONNECTIONSTRING_H
#define CLCONNECTIONSTRING_H

#include <string>

using namespace std;
namespace dap
{
class ConnectionString
{
public:
    enum eProtocol {
        kTcp,
        kUnixLocalSocket,
    };

protected:
    eProtocol m_protocol;
    string m_host;
    long m_port;
    string m_path;
    bool m_isOK;

protected:
    void DoParse(const string& connectionString);

public:
    ConnectionString(const string& connectionString);
    ~ConnectionString();

    void SetHost(const string& host) { this->m_host = host; }
    void SetIsOK(bool isOK) { this->m_isOK = isOK; }
    void SetPath(const string& path) { this->m_path = path; }
    void SetPort(long port) { this->m_port = port; }
    void SetProtocol(const eProtocol& protocol) { this->m_protocol = protocol; }
    const string& GetHost() const { return m_host; }
    bool IsOK() const { return m_isOK; }
    const string& GetPath() const { return m_path; }
    long GetPort() const { return m_port; }
    const eProtocol& GetProtocol() const { return m_protocol; }
};
};     // namespace dap
#endif // CLCONNECTIONSTRING_H
