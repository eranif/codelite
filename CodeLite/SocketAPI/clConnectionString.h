#ifndef CLCONNECTIONSTRING_H
#define CLCONNECTIONSTRING_H

#include <codelite_exports.h>
#include <wx/string.h>

class WXDLLIMPEXP_CL clConnectionString
{
public:
    enum eProtocol {
        kTcp,
        kUnixLocalSocket,
    };

protected:
    eProtocol m_protocol;
    wxString m_host;
    long m_port;
    wxString m_path;
    bool m_isOK;

protected:
    void DoParse(const wxString& connectionString);

public:
    clConnectionString(const wxString& connectionString);
    ~clConnectionString();

    void SetHost(const wxString& host) { this->m_host = host; }
    void SetIsOK(bool isOK) { this->m_isOK = isOK; }
    void SetPath(const wxString& path) { this->m_path = path; }
    void SetPort(long port) { this->m_port = port; }
    void SetProtocol(const eProtocol& protocol) { this->m_protocol = protocol; }
    const wxString& GetHost() const { return m_host; }
    bool IsOK() const { return m_isOK; }
    const wxString& GetPath() const { return m_path; }
    long GetPort() const { return m_port; }
    const eProtocol& GetProtocol() const { return m_protocol; }
};

#endif // CLCONNECTIONSTRING_H
