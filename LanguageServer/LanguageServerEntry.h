#ifndef LANGUAGESERVERENTRY_H
#define LANGUAGESERVERENTRY_H

#include <vector>
#include <wx/string.h>
#include "cl_config.h"
#include <wxStringHash.h>

class LanguageServerEntry : public clConfigItem
{
    bool m_enabled = true;
    wxString m_name;
    wxString m_exepath;
    wxString m_args;
    wxString m_languageId;
    wxString m_filesPattern;

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
    LanguageServerEntry& SetFilesPattern(const wxString& filesPattern)
    {
        this->m_filesPattern = filesPattern;
        return *this;
    }
    LanguageServerEntry& SetLanguageId(const wxString& languageId)
    {
        this->m_languageId = languageId;
        return *this;
    }
    const wxString& GetArgs() const { return m_args; }
    const wxString& GetExepath() const { return m_exepath; }
    const wxString& GetFilesPattern() const { return m_filesPattern; }
    const wxString& GetLanguageId() const { return m_languageId; }
    LanguageServerEntry& SetEnabled(bool enabled)
    {
        this->m_enabled = enabled;
        return *this;
    }
    bool IsEnabled() const { return m_enabled; }
};

#endif // LANGUAGESERVERENTRY_H
