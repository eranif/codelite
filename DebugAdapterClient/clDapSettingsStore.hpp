#ifndef CLDAPSETTINGSSTORE_HPP
#define CLDAPSETTINGSSTORE_HPP

#include "JSON.h"
#include "wxStringHash.h"

#include <map>
#include <wx/filename.h>

enum class DapEntryFlag : size_t {
    // linux path: no volume, always forward slash
    USE_LINUX_PATHS = (1 << 0),
    // implies: USE_LINUX_PATHS
    REMOTE_OVER_SSH = (1 << 1),
    // relative path: file name only
    USE_RELATIVE_PATH = (1 << 2),
    // forward slash. on windows, we include the volume
    // e.g. C:/path/to/file
    USE_FORWARD_SLASH = (1 << 3),
};

class DapEntry
{
    wxString m_command;
    wxString m_name;
    wxString m_connection_string;
    wxString m_environment;
    wxString m_ssh_account;
    size_t m_flags = (size_t)DapEntryFlag::USE_RELATIVE_PATH; // bitset of DapEntryFlags

private:
    void SetFlag(DapEntryFlag flag, bool b)
    {
        if(b) {
            m_flags |= (size_t)flag;
        } else {
            m_flags &= ~(size_t)flag;
        }
    }

    bool HasFlag(DapEntryFlag flag) const { return m_flags & (size_t)flag; }

public:
    DapEntry() {}
    ~DapEntry() {}

    void SetCommand(const wxString& command) { this->m_command = command; }
    void SetName(const wxString& name) { this->m_name = name; }
    void SetConnectionString(const wxString& connection_string) { this->m_connection_string = connection_string; }
    void SetRemote(bool b) { SetFlag(DapEntryFlag::REMOTE_OVER_SSH, b); }
    void SetSshAccount(const wxString& name) { this->m_ssh_account = name; }

    const wxString& GetCommand() const { return m_command; }
    const wxString& GetName() const { return m_name; }
    const wxString& GetConnectionString() const { return m_connection_string; }
    bool IsRemote() const { return HasFlag(DapEntryFlag::REMOTE_OVER_SSH); }
    const wxString& GetSshAccount() const { return m_ssh_account; }

    void SetEnvironment(const wxString& environment) { this->m_environment = environment; }
    const wxString& GetEnvironment() const { return m_environment; }

    void SetUseUnixPath(bool b) { SetFlag(DapEntryFlag::USE_LINUX_PATHS, b); }
    bool IsUsingUnixPath() const { return HasFlag(DapEntryFlag::USE_LINUX_PATHS); }

    void SetUseRelativePath(bool b) { SetFlag(DapEntryFlag::USE_RELATIVE_PATH, b); }
    bool UseRelativePath() const { return HasFlag(DapEntryFlag::USE_RELATIVE_PATH); }

    void SetUseForwardSlash(bool b) { SetFlag(DapEntryFlag::USE_FORWARD_SLASH, b); }
    bool UseForwardSlash() const { return HasFlag(DapEntryFlag::USE_FORWARD_SLASH); }

    JSONItem To() const;
    void From(const JSONItem& json);
};

class clDapSettingsStore
{
    std::map<wxString, DapEntry> m_entries;
    wxFileName m_filepath;

private:
    void Clear();

public:
    clDapSettingsStore();
    ~clDapSettingsStore();

    void Load(const wxFileName& file);
    void Store();
    bool IsEmpty() const { return m_entries.empty(); }
    bool Get(const wxString& name, DapEntry* entry) const;
    bool Contains(const wxString& name) const;
    bool Set(const DapEntry& entry);
    bool Delete(const wxString& name);
    const std::map<wxString, DapEntry>& GetEntries() const { return m_entries; }
};

#endif // CLDAPSETTINGSSTORE_HPP
