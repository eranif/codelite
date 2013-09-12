#ifndef SFTPSETTINGS_H
#define SFTPSETTINGS_H

#include "cl_config.h" // Base class: clConfigItem
#include "ssh_account_info.h"

class SFTPSettings : public clConfigItem
{
    SSHAccountInfo::List_t m_accounts;

public:
    SFTPSettings();
    virtual ~SFTPSettings();

    void SetAccounts(const SSHAccountInfo::List_t& accounts) {
        this->m_accounts = accounts;
    }
    const SSHAccountInfo::List_t& GetAccounts() const {
        return m_accounts;
    }
    
    bool GetAccount(const wxString &name, SSHAccountInfo &account) const;
    static void Load(SFTPSettings& settings);
    static void Save(const SFTPSettings& settings);
    
public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;
};

#endif // SFTPSETTINGS_H
