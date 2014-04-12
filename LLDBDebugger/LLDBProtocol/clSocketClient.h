#ifndef CLSOCKETCLIENT_H
#define CLSOCKETCLIENT_H

#include "cl_socket_base.h"
#include <wx/string.h>

class clSocketClient : public clSocketBase
{
    wxString m_path;

public:
    clSocketClient(const wxString &pipePath);
    clSocketClient() {}
    virtual ~clSocketClient();

    void SetPath(const wxString& path) {
        this->m_path = path;
    }
    const wxString& GetPath() const {
        return m_path;
    }
    bool Connect();
};

#endif // CLSOCKETCLIENT_H
