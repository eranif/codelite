#ifndef LANGUAGESERVERENTRY_H
#define LANGUAGESERVERENTRY_H

#include "LSPNetwork.h"
#include "asyncprocess.h"
#include "cl_config.h"

#include <map>
#include <vector>
#include <wx/string.h>
#include <wxStringHash.h>

class LanguageServerEntry
{
    bool m_enabled = true;
    wxString m_name;
    wxString m_exepath;
    wxString m_args;
    wxString m_workingDirectory;
    wxArrayString m_languages;
    wxString m_connectionString;
    int m_priority = 50;
    bool m_disaplayDiagnostics = true;
    wxString m_command;
    wxString m_remoteCommand;

public:
    // use 'map' to keep the items sorted by name
    typedef std::map<wxString, LanguageServerEntry> Map_t;

    /**
     * @brief try to validate the LSP by checking that all paths do exists
     * @return
     */
    bool IsValid() const;

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;
    LanguageServerEntry();
    virtual ~LanguageServerEntry();

    void SetCommand(const wxString& command);
    wxString GetCommand(bool pretty = false) const;

    LanguageServerEntry& SetDisaplayDiagnostics(bool disaplayDiagnostics)
    {
        this->m_disaplayDiagnostics = disaplayDiagnostics;
        return *this;
    }
    bool IsDisaplayDiagnostics() const { return m_disaplayDiagnostics; }
    LanguageServerEntry& SetConnectionString(const wxString& connectionString)
    {
        this->m_connectionString = connectionString;
        return *this;
    }
    const wxString& GetConnectionString() const { return m_connectionString; }
    LanguageServerEntry& SetPriority(int priority)
    {
        this->m_priority = priority;
        return *this;
    }
    int GetPriority() const { return m_priority; }
    LanguageServerEntry& SetEnabled(bool enabled)
    {
        this->m_enabled = enabled;
        return *this;
    }
    bool IsEnabled() const { return m_enabled; }
    LanguageServerEntry& SetLanguages(const wxArrayString& languages)
    {
        this->m_languages = languages;
        return *this;
    }
    const wxArrayString& GetLanguages() const { return m_languages; }
    LanguageServerEntry& SetWorkingDirectory(const wxString& workingDirectory)
    {
        this->m_workingDirectory = workingDirectory;
        return *this;
    }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }
    LanguageServerEntry& SetName(const wxString& name)
    {
        this->m_name = name;
        return *this;
    }
    const wxString& GetName() const { return m_name; }
    eNetworkType GetNetType() const;
    bool IsAutoRestart() const;
};

#endif // LANGUAGESERVERENTRY_H
