//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clSocketBase.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef CLSOCKETBASE_H
#define CLSOCKETBASE_H

#include <string>
#include <sys/param.h>
#include <wx/msgqueue.h>
#include <wx/sharedptr.h>
#include <wx/string.h>
#if defined(__WXOSX__) || defined(BSD)
#include <sys/errno.h>
#endif
#include "codelite_exports.h"

#include "wx/defs.h"
#ifdef __WINDOWS__ // __WINDOWS__ defined by wx/defs.h
// includes windows.h and if wxUSE_WINSOCK2 is true includes winsock2.h
#include "wx/msw/wrapwin.h"

typedef SOCKET socket_t;
typedef int socklen_t;
#else
typedef int socket_t;
#define INVALID_SOCKET -1
#endif

class WXDLLIMPEXP_CL clSocketException
{
    std::string m_what;

public:
    clSocketException(const std::string& what)
        : m_what(what)
    {
        // trim whitespaces
        static std::string trimString("\r\n\t\v ");
        m_what.erase(0, m_what.find_first_not_of(trimString));
        m_what.erase(m_what.find_last_not_of(trimString) + 1);
    }
    ~clSocketException() {}
    const std::string& what() const { return m_what; }
};

class WXDLLIMPEXP_CL clSocketBase
{
protected:
    socket_t m_socket = INVALID_SOCKET;
    bool m_closeOnExit = true;

public:
    typedef wxSharedPtr<clSocketBase> Ptr_t;
    typedef wxMessageQueue<clSocketBase::Ptr_t> Queue_t;

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
    static std::string error();
    static std::string error(const int errorCode);

public:
    /**
     * @brief set the socket into blocking/non-blocking mode
     * @param blocking
     */
    void MakeSocketBlocking(bool blocking);

    clSocketBase(socket_t sockfd = INVALID_SOCKET);
    virtual ~clSocketBase();

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
     * @brief use user socket handle
     */
    void SetSocket(socket_t socket);

    /**
     * @brief
     * @param msg
     */
    void Send(const std::string& msg);
    void Send(const wxMemoryBuffer& msg);
    void Send(const wxString& msg, const wxMBConv& conv = wxConvUTF8);

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
    int Read(wxString& content, const wxMBConv& conv = wxConvUTF8, long timeout = -1);

    /**
     * @brief read a buffer from the socket
     * @param content [output]
     * @param timeout seconds to wait
     */
    int Read(wxMemoryBuffer& content, long timeout = -1);

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

    /**
     * @brief read a full message that was sent with 'SendMessage'
     * @param message
     * @param timeout seconds to wait
     * @return kSuccess, kTimeout or kError
     */
    int ReadMessage(wxString& message, int timeout);

    /**
     * @brief write a full message
     * @param message
     */
    void WriteMessage(const wxString& message);

protected:
    /**
     * @brief
     */
    void DestroySocket();
};

#endif // CLSOCKETBASE_H
