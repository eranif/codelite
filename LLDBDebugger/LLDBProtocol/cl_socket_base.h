#ifndef CLSOCKETBASE_H
#define CLSOCKETBASE_H

#include <string>
#include <wx/sharedptr.h>

#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET socket_t;
typedef int socklen_t;
#else
typedef int socket_t;
#define INVALID_SOCKET -1
#endif

class clSocketException 
{
    std::string m_what;
public:
    clSocketException(const std::string & what ) : m_what(what) {}
    ~clSocketException() {}
    
    const std::string& what() const {
        return m_what;
    }
};

class clSocketBase
{
protected:
    socket_t m_socket;
public:
    typedef wxSharedPtr<clSocketBase> Ptr_t;
    
    enum {
        kSuccess = 1,
        kTimeout = 2,
        kError   = 3,
    };
    
    std::string error() const;
public:
    clSocketBase(int sockfd = INVALID_SOCKET);
    virtual ~clSocketBase();
    
    /**
     * @brief initialize the socket library
     */
    static void Initialize();
    
    /**
     * @brief 
     * @param msg
     */
    void Send(const std::string& msg) throw (clSocketException);
    /**
     * @brief 
     * @return 
     */
    int Read(char* buffer, size_t bufferSize, size_t& bytesRead, long timeout = -1) throw (clSocketException);
    /**
     * @brief 
     * @param milliseconds
     * @return 
     */
    int SelectRead(long seconds = -1) throw (clSocketException);
    
    /**
     * @brief 
     */
    void CloseSocket();
};

#endif // CLSOCKETBASE_H
