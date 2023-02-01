//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : cl_ssh.h
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

#ifndef CLSSH_H
#define CLSSH_H

#if USE_SFTP
#include "clSSHAgent.hpp"
#include "cl_command_event.h"
#include "cl_exception.h"
#include "codelite_exports.h"

#include <errno.h>
#include <memory>
#include <wx/event.h>
#include <wx/sharedptr.h>
#include <wx/string.h>
#include <wx/timer.h>

// We do it this way to avoid exposing the include to ssh/libssh.h to files including this header
struct ssh_session_struct;
struct ssh_channel_struct;
typedef struct ssh_session_struct* SSHSession_t;
typedef struct ssh_channel_struct* SSHChannel_t;

// Sent when a remote command over ssh has an output
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_COMMAND_OUTPUT, clCommandEvent);
// Sent when a remote command over ssh has completed
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_COMMAND_COMPLETED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_COMMAND_ERROR, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SSH_CONNECTED, clCommandEvent);

class WXDLLIMPEXP_CL clSSH : public wxEvtHandler
{
protected:
    wxString m_host;
    wxString m_username;
    wxString m_password;
    int m_port;
    bool m_connected;
    SSHSession_t m_session;
    SSHChannel_t m_channel;
    wxTimer* m_timer;
    wxEvtHandler* m_owner;
    clSSHAgent::Ptr_t m_sshAgent;

public:
    typedef std::shared_ptr<clSSH> Ptr_t;

protected:
    void OnCheckRemoteOutut(wxTimerEvent& event);
    void DoCloseChannel();
    void DoOpenChannel();
    void DoConnectWithRetries(int retries);

public:
    clSSH(const wxString& host, const wxString& user, const wxString& pass, int port = 22);
    clSSH();
    virtual ~clSSH();

    bool IsConnected() const { return m_connected; }
    bool IsCommandRunning() const { return m_channel != NULL; }

    /**
     * @brief send a message that should be ignored. This is useful for implemeting keep-alive
     */
    void SendIgnore();

    /**
     * @brief connect to the remote server
     */
    void Connect(int seconds = 10);

    /**
     * @brief authenticate the server
     * @param [output] message in case the authentication failed, prompt the user with the message
     * @return true if the server could be authenticated, otherwise return false.
     * In case an error occurs, throw a clException
     */
    bool AuthenticateServer(wxString& message);

    /**
     * @brief accepts the server authentication and add it to the "known_hosts"
     */
    void AcceptServerAuthentication();

    /**
     * @brief login to the server with the user credentials
     * @return true if we managed to login
     * @throw clException incase something really bad happened
     */
    bool LoginPassword(bool throwExc = true);

    /**
     * @brief login using ssh_userauth_none()
     */
    bool LoginAuthNone(bool throwExc = true);

    /**
     * @brief login using public key
     * @return true if we managed to login
     * @throw clException incase something really bad happened
     */
    bool LoginPublicKey(bool throwExc = true);

    /**
     * @brief login using interactive-keyboard method
     * @return true if we managed to login
     * @throw clException incase something really bad happened
     */
    bool LoginInteractiveKBD(bool throwExc = true);

    /**
     * @brief try to login using all the methods we support (interactive-kbd, user/pass and public key)
     */
    void Login();

    /**
     * @brief close the SSH session
     * IMPORTANT: this will invalidate all other channels (like: scp)
     */
    void Close();

    /**
     * @brief execute a remote command and return the output. open the shell if no is opened
     */
    void ExecuteShellCommand(wxEvtHandler* owner, const wxString& command);

    SSHSession_t GetSession() { return m_session; }

    void SetPassword(const wxString& password) { this->m_password = password; }
    void SetPort(int port) { this->m_port = port; }

    const wxString& GetPassword() const { return m_password; }
    int GetPort() const { return m_port; }
    void SetHost(const wxString& host) { this->m_host = host; }
    void SetUsername(const wxString& username) { this->m_username = username; }
    const wxString& GetHost() const { return m_host; }
    const wxString& GetUsername() const { return m_username; }
};
#endif // USE_SFTP
#endif // CLSSH_H
