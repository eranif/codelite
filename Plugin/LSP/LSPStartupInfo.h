#ifndef LSP_STARTUPINFO_H
#define LSP_STARTUPINFO_H

#include "clEnvironment.hpp"
#include "codelite_exports.h"

#include <wx/arrstr.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK LSPStartupInfo
{
public:
    enum eFlags {
        kShowConsole = (1 << 0),
        kUseTcp = (1 << 1),
        kAutoStart = (1 << 2),
        kRemoteLSP = (1 << 3),
    };

protected:
    wxString m_connectionString;
    wxArrayString m_lspServerCommand;
    wxString m_workingDirectory;
    size_t m_flags = 0;
    wxString m_accountName;
    clEnvList_t m_env;

public:
    LSPStartupInfo& SetConnectionString(const wxString& connectionString)
    {
        this->m_connectionString = connectionString;
        return *this;
    }
    const wxString& GetConnectionString() const { return m_connectionString; }
    void SetLspServerCommand(const wxArrayString& lspServerCommand) { this->m_lspServerCommand = lspServerCommand; }
    const wxArrayString& GetLspServerCommand() const { return m_lspServerCommand; }

    void SetFlags(size_t flags) { this->m_flags = flags; }
    size_t GetFlags() const { return m_flags; }

    LSPStartupInfo& SetWorkingDirectory(const wxString& workingDirectory)
    {
        this->m_workingDirectory = workingDirectory;
        return *this;
    }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }
    void SetAccountName(const wxString& accountName) { this->m_accountName = accountName; }
    const wxString& GetAccountName() const { return m_accountName; }

    void SetEnv(const clEnvList_t& env) { this->m_env = env; }
    const clEnvList_t& GetEnv() const { return m_env; }

public:
    LSPStartupInfo() = default;
    LSPStartupInfo(const LSPStartupInfo&) = default;
    virtual ~LSPStartupInfo() = default;
    LSPStartupInfo& operator=(const LSPStartupInfo&) = default;
};
#endif // LSP_STARTUPINFO_H
