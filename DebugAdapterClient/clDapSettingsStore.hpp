#ifndef CLDAPSETTINGSSTORE_HPP
#define CLDAPSETTINGSSTORE_HPP

#include "JSON.h"
#include "dap/dap.hpp"
#include "wxStringHash.h"

#include <map>
#include <wx/filename.h>

enum class DapPathFormat : size_t {
    PATH_NATIVE = 0,
    // relative path: file name only
    PATH_RELATIVE = (1 << 0),
    // forward slash, always
    PATH_FORWARD_SLASH = (1 << 1),
    // MSW: use volume in path
    PATH_NO_VOLUME = (1 << 2),
};

enum class DapLaunchType {
    LAUNCH = 0,
    ATTACH,
};

class DapEntry
{
    wxString m_command;
    wxString m_name;
    wxString m_connection_string;
    wxString m_environment;
    size_t m_flags = (size_t)DapPathFormat::PATH_NATIVE; // bitset of DapPathFormats
    dap::EnvFormat m_envFormat = dap::EnvFormat::LIST;
    DapLaunchType m_launch_type = DapLaunchType::LAUNCH;

private:
    void SetFlag(DapPathFormat flag, bool b)
    {
        if(b) {
            m_flags |= (size_t)flag;
        } else {
            m_flags &= ~(size_t)flag;
        }
    }

    bool HasFlag(DapPathFormat flag) const { return m_flags & (size_t)flag; }

public:
    DapEntry() {}
    ~DapEntry() {}

    void SetEnvFormat(const dap::EnvFormat& envFormat) { this->m_envFormat = envFormat; }
    dap::EnvFormat GetEnvFormat() const { return m_envFormat; }

    void SetCommand(const wxString& command) { this->m_command = command; }
    void SetName(const wxString& name) { this->m_name = name; }
    void SetConnectionString(const wxString& connection_string) { this->m_connection_string = connection_string; }

    const wxString& GetCommand() const { return m_command; }
    const wxString& GetName() const { return m_name; }
    const wxString& GetConnectionString() const { return m_connection_string; }

    void SetEnvironment(const wxString& environment) { this->m_environment = environment; }
    const wxString& GetEnvironment() const { return m_environment; }

    void SetUseVolume(bool b) { SetFlag(DapPathFormat::PATH_NO_VOLUME, !b); }
    bool UseVolume() const { return !HasFlag(DapPathFormat::PATH_NO_VOLUME); }

    void SetUseRelativePath(bool b) { SetFlag(DapPathFormat::PATH_RELATIVE, b); }
    bool UseRelativePath() const { return HasFlag(DapPathFormat::PATH_RELATIVE); }

    void SetUseForwardSlash(bool b) { SetFlag(DapPathFormat::PATH_FORWARD_SLASH, b); }
    bool UseForwardSlash() const { return HasFlag(DapPathFormat::PATH_FORWARD_SLASH); }

    bool IsUsingNativePath() const { return m_flags == 0; }
    void SetUseNativePath() { m_flags = 0; }

    DapLaunchType GetLaunchType() const { return m_launch_type; }
    void SetLaunchType(DapLaunchType type) { m_launch_type = type; }

    JSONItem To() const;
    void From(const JSONItem& json);
};

class clDapSettingsStore
{
    std::map<wxString, DapEntry> m_entries;

public:
    clDapSettingsStore();
    ~clDapSettingsStore();

    void Load(const wxFileName& file);
    void Save(const wxFileName& file);
    bool IsEmpty() const { return m_entries.empty(); }
    bool Get(const wxString& name, DapEntry* entry) const;
    bool Contains(const wxString& name) const;
    bool Set(const DapEntry& entry);

    /**
     * @brief replace all entries with this list
     * @param entries
     */
    void Set(const std::vector<DapEntry>& entries);

    /**
     * @brief update the list. If a dap server already exists with the same name
     * it will be replaced
     */
    void Update(const std::vector<DapEntry>& entries);

    bool Delete(const wxString& name);
    const std::map<wxString, DapEntry>& GetEntries() const { return m_entries; }
    void Clear();
    bool empty() const { return IsEmpty(); }
    size_t size() const { return m_entries.size(); }
};

#endif // CLDAPSETTINGSSTORE_HPP
