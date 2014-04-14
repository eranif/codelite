#ifndef CLSOCKETCLIENT_H
#define CLSOCKETCLIENT_H

#include "cl_socket_base.h"
#include <wx/string.h>

class clSocketClient : public clSocketBase
{
    wxString m_path;

public:
    clSocketClient();
    virtual ~clSocketClient();

    /**
     * @brief connect to a remote socket, using unix-domain socket
     */
    bool ConnectLocal(const wxString &socketPath);
    /**
     * @brief connect to a remote server using ip/port
     */
    bool ConnectRemote(const wxString &address, int port);
};

#endif // CLSOCKETCLIENT_H
