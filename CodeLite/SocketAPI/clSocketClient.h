#ifndef CLSOCKETCLIENT_H
#define CLSOCKETCLIENT_H

#include "clSocketBase.h"
#include <wx/string.h>

class WXDLLIMPEXP_CL clSocketClient : public clSocketBase
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
     * when using non-blocking mode, wouldBlock will be set to true
     * incase the connect fails
     */
    bool ConnectRemote(const wxString &address, int port, bool &wouldBlock, bool nonBlockingMode = false);
};

#endif // CLSOCKETCLIENT_H
