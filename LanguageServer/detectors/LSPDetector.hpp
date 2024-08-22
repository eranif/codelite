#ifndef LSPDETECTOR_HPP
#define LSPDETECTOR_HPP

#include "AsyncProcess/asyncprocess.h"
#include "LanguageServerEntry.h"

#include <wx/arrstr.h>
#include <wx/sharedptr.h>
#include <wx/string.h>

class LSPDetector
{
protected:
    wxString m_name;
    wxString m_command;
    wxArrayString m_languages;
    wxString m_connectionString;
    int m_priority = 50;
    clEnvList_t m_env;
    bool m_enabled = true;
    wxString m_initialiseOptions;

public:
    typedef wxSharedPtr<LSPDetector> Ptr_t;

protected:
    virtual bool DoLocate() = 0;
    virtual void DoClear();

public:
    LSPDetector(const wxString& name);
    virtual ~LSPDetector();
    bool IsOk() const { return !m_command.IsEmpty(); }
    void SetCommand(const wxString& command) { this->m_command = command; }
    const wxString& GetCommand() const { return m_command; }
    void SetLanguages(const wxArrayString& languages) { this->m_languages = languages; }
    void SetName(const wxString& name) { this->m_name = name; }
    const wxArrayString& GetLanguages() const { return m_languages; }
    wxArrayString& GetLanguages() { return m_languages; }
    const wxString& GetName() const { return m_name; }
    void SetConnectionString(const wxString& connectionString) { this->m_connectionString = connectionString; }
    const wxString& GetConnectionString() const { return m_connectionString; }
    void SetPriority(int priority) { this->m_priority = priority; }
    int GetPriority() const { return m_priority; }
    bool Locate();
    virtual void GetLanguageServerEntry(LanguageServerEntry& entry);
    void SetEnv(const clEnvList_t& env) { this->m_env = env; }
    const clEnvList_t& GetEnv() const { return m_env; }
    void SetEnabled(bool enabled) { this->m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }
    LSPDetector& SetInitialiseOptions(const wxString& initialiseOptions)
    {
        this->m_initialiseOptions = initialiseOptions;
        return *this;
    }
    const wxString& GetInitialiseOptions() const { return m_initialiseOptions; }
};

#endif // LSPDETECTOR_HPP
