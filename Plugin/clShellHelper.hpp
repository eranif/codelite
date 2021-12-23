#ifndef CLSHELLHELPER_HPP
#define CLSHELLHELPER_HPP

#include "codelite_exports.h"

#include <wx/arrstr.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clShellHelper
{
    wxString m_out_command;
    size_t m_processCreateFlags = 0;
    wxArrayString m_cleanup_scripts;

public:
    clShellHelper();
    ~clShellHelper();
    /**
     * @brief attempt to process an input command
     * return true if the command was modified
     */
    bool ProcessCommand(const wxString& cmd);
    /**
     * @brief clean all temporary files created by this class
     */
    void Cleanup();
    /**
     * @brief return the modified command
     */
    const wxString& GetCommand() const { return m_out_command; }
    /**
     * @brief return true if the input command was modified by this helper
     * @return
     */
    bool IsModified() const { return !m_out_command.empty(); }
    /**
     * @brief return any additional flags that should be added to the ::CreateAsyncProcess() call
     */
    size_t GetProcessCreateFlags() const { return m_processCreateFlags; }
};

#endif // CLSHELLHELPER_HPP
