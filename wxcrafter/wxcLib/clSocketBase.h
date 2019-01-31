#ifndef CLSOCKETBASE_H
#define CLSOCKETBASE_H

#include <string>
#include <wx/string.h>
#include <wx/version.h>

#if wxCHECK_VERSION(3, 0, 0)
#   include <wx/sharedptr.h>
#else
#   include "wxcLib/smart_ptr.h"
#endif


#ifdef _WIN32
typedef SOCKET socket_t;
typedef int socklen_t;
#else
typedef int socket_t;
#define INVALID_SOCKET -1
#endif

class clSocketException
{
    wxString m_what;
public:
    clSocketException(const wxString & what ) : m_what(what) {}
    ~clSocketException() {}

    const wxString& what() const {
        return m_what;
    }
};

class clSocketBase
{
protected:
    socket_t m_socket;
    bool     m_closeOnExit;

public:
#if wxCHECK_VERSION(3, 0, 0)
    typedef wxSharedPtr<clSocketBase> Ptr_t;
#else
    typedef SmartPtr<clSocketBase> Ptr_t;
#endif

    enum {
        kSuccess = 1,
        kTimeout = 2,
        kError   = 3,
    };

    wxString error() const;
public:
    clSocketBase(socket_t sockfd = INVALID_SOCKET);
    virtual ~clSocketBase();

    void SetCloseOnExit(bool closeOnExit) {
        this->m_closeOnExit = closeOnExit;
    }
    bool IsCloseOnExit() const {
        return m_closeOnExit;
    }
    /**
     * @brief return the descriptor and clear this socket.
     */
    socket_t Release() ;

    /**
     * @brief initialize the socket library
     */
    static void Initialize();

    socket_t GetSocket() const {
        return m_socket;
    }

    /**
     * @brief
     * @param msg
     */
    void Send(const std::string& msg) ;
    /**
     * @brief
     * @return
     */
    int Read(char* buffer, size_t bufferSize, size_t& bytesRead, long timeout = -1) ;
    /**
     * @brief
     * @param milliseconds
     * @return
     */
    int SelectRead(long seconds = -1) ;

    /**
     * @brief read a full message that was sent with 'SendMessage'
     * @param message
     * @param timeout seconds to wait
     * @return kSuccess, kTimeout or kError
     */
    int ReadMessage(wxString &message, int timeout) ;

    /**
     * @brief write a full message
     * @param message
     */
    void WriteMessage(const wxString &message) ;

protected:
    /**
     * @brief
     */
    void DestroySocket();
};

#endif // CLSOCKETBASE_H
