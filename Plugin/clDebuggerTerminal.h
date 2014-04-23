#ifndef CLDEBUGGERTERMINAL_H
#define CLDEBUGGERTERMINAL_H

#include <wx/string.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clDebuggerTerminalPOSIX
{
    wxString m_title;
    wxString m_tty;
    long     m_pid;

public:
    clDebuggerTerminalPOSIX();
    virtual ~clDebuggerTerminalPOSIX();

    /**
     * @brief launch the terminal. Check the IsValid() after this call
     */
    void Launch(const wxString &title);

    /**
     * @brief is this a valid terminal?
     * @return
     */
    bool IsValid() const;

    /**
     * @brief return the associated pty for this terminal
     */
    const wxString& GetTty() const {
        return m_tty;
    }

    /**
     * @brief terminate the process associated with the terminal and clear the object
     * making it ready for another use
     */
    void Clear();
};

#endif // CLDEBUGGERTERMINAL_H
