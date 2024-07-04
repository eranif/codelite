//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : phpexecutor.h
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

#ifndef PHPEXECUTOR_H
#define PHPEXECUTOR_H

#include "AsyncProcess/TerminalEmulator.h"
#include "php_project.h"

#include <wx/event.h>
#include <wx/process.h>

class PHPExecutor : public wxEvtHandler
{
    TerminalEmulator m_terminal;

protected:
    bool RunRUL(PHPProject::Ptr_t pProject, const wxString& urlToRun, const wxString& xdebugSessionName);
    bool DoRunCLI(const wxString& script, PHPProject::Ptr_t proj, const wxString& xdebugSessionName,
                  bool neverPauseOnExit);
    std::pair<wxString, wxString> DoGetCLICommand(const wxString& script, PHPProject::Ptr_t proj, wxString& errmsg);

public:
    PHPExecutor();
    virtual ~PHPExecutor();

    /**
     * @brief execute project. This function takes into account the project settings
     * @param projectName the project name
     * @param xdebugSessionName if debugging, use this xdebug-session name
     * @param neverPauseOnExit should we display a console with message 'Hit any key?'
     * @return true on success, false otherwise
     */
    bool Exec(const wxString& projectName, const wxString& urlOrFilePath, const wxString& xdebugSessionName,
              bool neverPauseOnExit);
    /**
     * @brief return true if a script is currently being executed using this instance
     */
    bool IsRunning() const;
    /**
     * @brief stop any executed php script
     */
    void Stop();

    /**
     * @brief run simple script using the global settings only and return its output
     */
    bool RunScript(const wxString& script, wxString& php_output);

    TerminalEmulator* GetTerminalEmulator() { return &m_terminal; }
};

#endif // PHPEXECUTOR_H
