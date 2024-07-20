#ifndef LANGUAGESERVERENTRY_H
#define LANGUAGESERVERENTRY_H

#include "AsyncProcess/asyncprocess.h"
#include "LSP/LSPNetwork.h"
#include "cl_config.h"
#include "wxStringHash.h"

#include <map>
#include <vector>
#include <wx/string.h>

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
    wxString m_initOptions;

public:
    // use 'map' to keep the items sorted by name
    typedef std::map<wxString, LanguageServerEntry> Map_t;

    bool IsNull() const;

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    LanguageServerEntry();
    ~LanguageServerEntry();

    void SetCommand(const wxString& command);
    wxString GetCommand(bool pretty = false) const;

    void SetInitOptions(const wxString& initOptions);
    wxString GetInitOptions() const;

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
