#ifndef LANGUAGESERVERENTRY_H
#define LANGUAGESERVERENTRY_H

#include <vector>
#include <wx/string.h>
#include "cl_config.h"
#include <wxStringHash.h>

class LanguageServerEntry
{
    bool m_enabled = true;
    wxString m_name;
    wxString m_exepath;
    wxString m_args;
    wxString m_workingDirectory;
    wxArrayString m_languages;

public:
    typedef std::unordered_map<wxString, LanguageServerEntry> Map_t;

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;
    LanguageServerEntry();
    virtual ~LanguageServerEntry();
    LanguageServerEntry& SetArgs(const wxString& args)
    {
        this->m_args = args;
        return *this;
    }
    LanguageServerEntry& SetExepath(const wxString& exepath)
    {
        this->m_exepath = exepath;
        return *this;
    }
    const wxString& GetArgs() const { return m_args; }
    const wxString& GetExepath() const { return m_exepath; }
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
};

#endif // LANGUAGESERVERENTRY_H
