#ifndef LSP_STARTUPINFO_H
#define LSP_STARTUPINFO_H

#include <wx/string.h>
#include "codelite_exports.h"
#include <wx/arrstr.h>

class WXDLLIMPEXP_SDK LSPStartupInfo
{
public:
    enum eFlags {
        kShowConsole = (1 << 0),
        kUseTcp = (1 << 1),
    };

protected:
    wxString m_connectioString;
    wxArrayString m_lspServerCommand;
    wxString m_workingDirectory;
    size_t m_flags = 0;

public:
    LSPStartupInfo& SetConnectioString(const wxString& connectioString)
    {
        this->m_connectioString = connectioString;
        return *this;
    }
    const wxString& GetConnectioString() const { return m_connectioString; }
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

public:
    LSPStartupInfo();
    LSPStartupInfo(const LSPStartupInfo& other);
    virtual ~LSPStartupInfo();
    LSPStartupInfo& operator=(const LSPStartupInfo& other);
};
#endif // LSP_STARTUPINFO_H
