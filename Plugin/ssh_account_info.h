#ifndef SSHACCOUNTINFO_H
#define SSHACCOUNTINFO_H

#include "cl_config.h" // Base class: clConfigItem
#include <list>
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK SSHAccountInfo : public clConfigItem
{
    wxString m_accountName;
    wxString m_username;
    wxString m_password;
    int      m_port;
    wxString m_host;
public:
    typedef std::list<SSHAccountInfo> List_t;

public:
    SSHAccountInfo();

    virtual ~SSHAccountInfo();
    SSHAccountInfo& operator=(const SSHAccountInfo& other);

    void SetAccountName(const wxString& accountName) {
        this->m_accountName = accountName;
    }
    const wxString& GetAccountName() const {
        return m_accountName;
    }
    void SetHost(const wxString& host) {
        this->m_host = host;
    }
    void SetPassword(const wxString& password) {
        this->m_password = password;
    }
    void SetPort(int port) {
        this->m_port = port;
    }
    void SetUsername(const wxString& username) {
        this->m_username = username;
    }
    const wxString& GetHost() const {
        return m_host;
    }
    const wxString& GetPassword() const {
        return m_password;
    }
    int GetPort() const {
        return m_port;
    }
    const wxString& GetUsername() const {
        return m_username;
    }

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;
};

#endif // SSHACCOUNTINFO_H
