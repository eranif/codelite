//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clDebuggerTerminal.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

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

    //! Make a console title for debugging an exe.
    static wxString MakeExeTitle(const wxString &exePath, const wxString &args);

    //! Make a console title for debugging a core file.
    static wxString MakeCoreTitle(const wxString &coreFile);

    //! Make a console title for debugging a running process.
    static wxString MakePidTitle(const int pid);
};

#endif // CLDEBUGGERTERMINAL_H
