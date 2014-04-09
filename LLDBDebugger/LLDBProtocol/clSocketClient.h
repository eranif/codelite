#ifndef CLSOCKETCLIENT_H
#define CLSOCKETCLIENT_H

#include "cl_socket_base.h"
#include <wx/string.h>

class clSocketClient : public clSocketBase
{
    wxString m_ip;
    int m_port;

public:
    clSocketClient(const wxString &ip, int port);
    clSocketClient() : m_port(wxNOT_FOUND) {}
    virtual ~clSocketClient();

    void SetIp(const wxString& ip) {
        this->m_ip = ip;
    }
    void SetPort(int port) {
        this->m_port = port;
    }
    const wxString& GetIp() const {
        return m_ip;
    }
    int GetPort() const {
        return m_port;
    }
    bool Connect();
};

#endif // CLSOCKETCLIENT_H
