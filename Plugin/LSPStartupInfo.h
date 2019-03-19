#ifndef LSP_STARTUPINFO_H
#define LSP_STARTUPINFO_H

#include <wx/string.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK LSPStartupInfo
{
public:
    enum eFlags {
        kShowConsole = (1 << 0),
        kUseTcp = (1 << 1),
    };

protected:
    wxString m_helperCommand;
    wxString m_lspServerCommand;
    wxString m_lspServerCommandWorkingDirectory;
    size_t m_flags = 0;

public:
    void SetHelperCommand(const wxString& helperCommand) { this->m_helperCommand = helperCommand; }
    /**
     * @brief the helper command: In case the connection should be made over TCP/IP it will contain the connection
     * string, else it will contain the actual helper script command
     */
    const wxString& GetHelperCommand() const { return m_helperCommand; }

    void SetLspServerCommand(const wxString& lspServerCommand) { this->m_lspServerCommand = lspServerCommand; }
    const wxString& GetLspServerCommand() const { return m_lspServerCommand; }

    void SetFlags(size_t flags) { this->m_flags = flags; }
    size_t GetFlags() const { return m_flags; }

    void SetLspServerCommandWorkingDirectory(const wxString& lspServerCommandWorkingDirectory)
    {
        this->m_lspServerCommandWorkingDirectory = lspServerCommandWorkingDirectory;
    }
    const wxString& GetLspServerCommandWorkingDirectory() const { return m_lspServerCommandWorkingDirectory; }

public:
    LSPStartupInfo();
    LSPStartupInfo(const LSPStartupInfo& other);
    virtual ~LSPStartupInfo() {}
    LSPStartupInfo& operator=(const LSPStartupInfo& other);
};
#endif // LSP_STARTUPINFO_H
