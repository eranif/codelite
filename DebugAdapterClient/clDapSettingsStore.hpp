#ifndef CLDAPSETTINGSSTORE_HPP
#define CLDAPSETTINGSSTORE_HPP

#include "JSON.h"
#include "wxStringHash.h"

#include <map>
#include <wx/filename.h>

class DapEntry
{
    wxString m_command;
    wxString m_name;
    wxString m_connection_string;
    wxString m_environment;
    bool m_is_remote = false;
    wxString m_ssh_account;

public:
    DapEntry() {}
    ~DapEntry() {}

    void SetCommand(const wxString& command) { this->m_command = command; }
    void SetName(const wxString& name) { this->m_name = name; }
    void SetConnectionString(const wxString& connection_string) { this->m_connection_string = connection_string; }
    void SetRemote(bool b) { this->m_is_remote = b; }
    void SetSshAccount(const wxString& name) { this->m_ssh_account = name; }

    const wxString& GetCommand() const { return m_command; }
    const wxString& GetName() const { return m_name; }
    const wxString& GetConnectionString() const { return m_connection_string; }
    bool IsRemote() const { return m_is_remote; }
    const wxString& GetSshAccount() const { return m_ssh_account; }

    void SetEnvironment(const wxString& environment) { this->m_environment = environment; }
    const wxString& GetEnvironment() const { return m_environment; }

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
