#ifndef CLSOCKETCLIENT_H
#define CLSOCKETCLIENT_H

#include "clSocketBase.h"
#include <wx/string.h>

class WXDLLIMPEXP_CL clSocketClient : public clSocketBase
{
    wxString m_path;
    
protected:
    /**
     * @brief connect with timeout
     * @param num_sesc
     * @return 
     */
    bool WaitForConnect(int num_sesc);
    
public:
    clSocketClient();
    virtual ~clSocketClient();

    /**
     * @brief connect to a remote socket, using unix-domain socket
     */
    bool ConnectLocal(const wxString &socketPath, int num_secs);
    /**
     * @brief connect to a remote server using ip/port
     */
    bool ConnectRemote(const wxString &address, int port, int num_secs);
};

#endif // CLSOCKETCLIENT_H
