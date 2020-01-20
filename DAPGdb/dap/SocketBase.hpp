#ifndef CLSOCKETBASE_H
#define CLSOCKETBASE_H

#include <memory>
#include <string>
#include <sys/param.h>
#if defined(__WXOSX__) || defined(BSD)
#include <sys/errno.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#endif

#ifdef _WIN32
typedef SOCKET socket_t;
typedef int socklen_t;
#else
typedef int socket_t;
#define INVALID_SOCKET -1
#endif

using namespace std;
namespace dap
{
class SocketException
{
    string m_what;

public:
    SocketException(const string& what)
        : m_what(what)
    {
        // trim whitespaces
        static string trimString("\r\n\t\v ");
        m_what.erase(0, m_what.find_first_not_of(trimString));
        m_what.erase(m_what.find_last_not_of(trimString) + 1);
    }
    ~SocketException() {}
    const string& what() const { return m_what; }
};

class SocketBase
{
protected:
    socket_t m_socket;
    bool m_closeOnExit;

public:
    typedef shared_ptr<SocketBase> Ptr_t;

    enum {
        kSuccess = 1,
        kTimeout = 2,
        kError = 3,
    };

#ifdef _WIN32
    static const int eWouldBlock = WSAEWOULDBLOCK;
#else
    static const int eWouldBlock = EWOULDBLOCK;
#endif

    static int GetLastError();
    static string error();
    static string error(const int errorCode);

public:
    /**
     * @brief set the socket into blocking/non-blocking mode
     * @param blocking
     */
    void MakeSocketBlocking(bool blocking);

    SocketBase(socket_t sockfd = INVALID_SOCKET);
    virtual ~SocketBase();

    void SetCloseOnExit(bool closeOnExit) { this->m_closeOnExit = closeOnExit; }
    bool IsCloseOnExit() const { return m_closeOnExit; }
    /**
     * @brief return the descriptor and clear this socket.
     */
    socket_t Release();

    /**
     * @brief initialize the socket library
     */
    static void Initialize();

    socket_t GetSocket() const { return m_socket; }

    /**
     * @brief
     * @param msg
     */
    void Send(const string& msg);

    /**
     * @brief
     * @return
     */
    int Read(char* buffer, size_t bufferSize, size_t& bytesRead, long timeout = -1);
    /**
     * @brief read string content from remote server
     * @param content [output]
     * @param timeout seconds to wait
     */
    int Read(string& content, long timeout = -1);
    /**
     * @brief
     * @param seconds
     * @return
     */
    int SelectRead(long seconds = -1);

    /**
     * @brief select for read. Same as above, but use milli seconds instead
     * @param milliSeconds number of _milliseconds_ to wait
     * @return
     */
    int SelectReadMS(long milliSeconds = -1);

    /**
     * @brief select for write
     * @return
     */
    int SelectWrite(long seconds = -1);

    /**
     * @brief select for write (milli seconds version)
     * @return
     */
    int SelectWriteMS(long milliSeconds = -1);

    template <typename T> T* As() const { return dynamic_cast<T*>(const_cast<SocketBase*>(this)); }

protected:
    /**
     * @brief
     */
    void DestroySocket();
};
};     // namespace dap
#endif // CLSOCKETBASE_H
