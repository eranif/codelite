#ifndef SFTPBOOKMARK_H
#define SFTPBOOKMARK_H

#include <wx/string.h>
#include "json_node.h"
#include "ssh_account_info.h"

class SFTPBookmark
{
    wxString        m_name;
    SSHAccountInfo  m_account;
    wxString        m_folder;

public:
    SFTPBookmark();
    ~SFTPBookmark();

    void SetAccount(const SSHAccountInfo& account) {
        this->m_account = account;
    }
    const SSHAccountInfo& GetAccount() const {
        return m_account;
    }
    void SetFolder(const wxString& folder) {
        this->m_folder = folder;
    }
    const wxString& GetFolder() const {
        return m_folder;
    }

    void SetName(const wxString& name) {
        this->m_name = name;
    }
    const wxString& GetName() const {
        return m_name;
    }

    //-----------------------
    // Seerialization API
    //-----------------------
    JSONElement ToJSON() const;
    void FromJSON(const JSONElement &json);
};

#endif // SFTPBOOKMARK_H
