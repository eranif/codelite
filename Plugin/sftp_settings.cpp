#include "sftp_settings.h"
#if USE_SFTP

SFTPSettings::SFTPSettings()
    : clConfigItem("sftp-settings")
{
}

SFTPSettings::~SFTPSettings()
{
}

void SFTPSettings::FromJSON(const JSONElement& json)
{
    m_accounts.clear();
    JSONElement arrAccounts = json.namedObject("accounts");
    int size = arrAccounts.arraySize();
    for(int i=0; i<size; ++i) {
        SSHAccountInfo account;
        account.FromJSON(arrAccounts.arrayItem(i));
        m_accounts.push_back( account );
    }
}

JSONElement SFTPSettings::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    JSONElement arrAccounts = JSONElement::createArray("accounts");
    element.append(arrAccounts);
    for(size_t i=0; i<m_accounts.size(); ++i) {
        arrAccounts.append( m_accounts.at(i).ToJSON() );
    }
    return element;
}

SFTPSettings& SFTPSettings::Load()
{
    clConfig config("sftp-settings.conf");
    config.ReadItem( this );
    return *this;
}

SFTPSettings& SFTPSettings::Save()
{
    clConfig config("sftp-settings.conf");
    config.WriteItem( this );
    return *this;
}

bool SFTPSettings::GetAccount(const wxString& name, SSHAccountInfo &account) const
{
    for( size_t i=0; i<m_accounts.size(); ++i ) {
        const SSHAccountInfo& currentAccount = m_accounts.at(i);
        if ( name == currentAccount.GetAccountName() ) {
            account = currentAccount;
            return true;
        }
    }
    return false;
}

bool SFTPSettings::UpdateAccount(const SSHAccountInfo& account)
{
    for( size_t i=0; i<m_accounts.size(); ++i ) {
        SSHAccountInfo& currentAccount = m_accounts.at(i);
        if ( account.GetAccountName() == currentAccount.GetAccountName() ) {
            currentAccount = account;
            return true;
        }
    }
    return false;
}
#endif // USE_SFTP
