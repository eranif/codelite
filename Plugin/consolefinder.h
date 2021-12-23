//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : consolefinder.h
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
#ifndef CONSOLEFINDER_H
#define CONSOLEFINDER_H

#include "codelite_exports.h"
#include "precompiled_header.h"

/**
 * \ingroup SDK
 * a misc class that allows user to locate a free pty terminal
 * this is used by the debugger
 * \version 1.0
 * first version
 *
 * \date 10-14-2007
 *
 * \author Eran
 *
 */
class WXDLLIMPEXP_SDK ConsoleFinder
{
    wxString m_ConsoleTty;
    int m_nConsolePid;
    wxString m_consoleCommand;

private:
    int RunConsole(const wxString& title);
    wxString GetConsoleTty(int ConsolePid);

public:
    ConsoleFinder();
    virtual ~ConsoleFinder();

    bool FindConsole(const wxString& title, wxString& consoleName);
    void FreeConsole();

    void SetConsoleCommand(const wxString& consoleCommand) { this->m_consoleCommand = consoleCommand; }
    const wxString& GetConsoleCommand() const { return m_consoleCommand; }
};

#endif
