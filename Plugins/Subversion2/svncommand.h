//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : svncommand.h
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

#ifndef SVNCOMMAND_H
#define SVNCOMMAND_H

#include "AsyncProcess/asyncprocess.h"
#include "AsyncProcess/processreaderthread.h"
#include "svncommandhandler.h"

#include <wx/event.h> // Base class

class Subversion2;
class SvnCommand : public wxEvtHandler
{
    IProcess* m_process;
    wxString m_command;
    wxString m_workingDirectory;
    SvnCommandHandler* m_handler;
    wxString m_output;
    Subversion2* m_plugin;

protected:
    // Event handlers
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);

public:
    SvnCommand(Subversion2* plugin);
    virtual ~SvnCommand();

    bool
    Execute(const wxString& command, const wxString& workingDirectory, SvnCommandHandler* handler, Subversion2* plugin);

    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }

    const wxString& GetCommand() const { return m_command; }

    IProcess* GetProcess() { return m_process; }

    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }

    void ClearAll();
    bool IsBusy() { return m_process != NULL; }
};

#endif // SVNCOMMAND_H
