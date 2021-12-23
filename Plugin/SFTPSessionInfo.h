#ifndef SFTPSESSIONINFO_H
#define SFTPSESSIONINFO_H

#if USE_SFTP

#include "cl_config.h"

#include <codelite_exports.h>
#include <vector>
#include <wx/string.h>
#include <wxStringHash.h>

class WXDLLIMPEXP_SDK SFTPSessionInfo
{
    std::vector<wxString> m_files;
    wxString m_rootFolder;
    wxString m_account;

public:
    typedef std::unordered_map<wxString, SFTPSessionInfo> Map_t;

public:
    SFTPSessionInfo();
    ~SFTPSessionInfo();

    void FromJSON(const JSONItem& json);
    JSONItem ToJSON() const;

    void SetFiles(const std::vector<wxString>& files) { this->m_files = files; }
    void SetRootFolder(const wxString& rootFolder) { this->m_rootFolder = rootFolder; }
    const std::vector<wxString>& GetFiles() const { return m_files; }
    std::vector<wxString>& GetFiles() { return m_files; }
    const wxString& GetRootFolder() const { return m_rootFolder; }
    void SetAccount(const wxString& account) { this->m_account = account; }
    const wxString& GetAccount() const { return m_account; }
    void Clear();
    bool IsOk() const;
    void RemoveFile(const wxString& path);
    void AddFile(const wxString& path);
};

class WXDLLIMPEXP_SDK SFTPSessionInfoList : public clConfigItem
{
    SFTPSessionInfo::Map_t m_sessions;

public:
    SFTPSessionInfoList();
    virtual ~SFTPSessionInfoList();
    void FromJSON(const JSONItem& json);
    JSONItem ToJSON() const;

    SFTPSessionInfoList& Load();
    SFTPSessionInfoList& Save();

    /**
     * @brief load session for the given account
     */
    SFTPSessionInfo& GetSession(const wxString& accountName);
    /**
     * @brief add session. If a session with this name already exists, overwrite it
     */
    SFTPSessionInfoList& SetSession(const SFTPSessionInfo& sess);
};
#endif

#endif // SFTPSESSIONINFO_H
