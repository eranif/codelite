//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : svn_console.h
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

#ifndef SVNSHELL_H
#define SVNSHELL_H

#include "svncommandhandler.h"
#include <deque>
#include <wx/stc/stc.h>
#include <wx/event.h>
#include "cl_command_event.h"

class IProcess;
class Subversion2;

struct SvnConsoleCommand {
    SvnCommandHandler* handler;
    wxString cmd;
    wxString workingDirectory;
    bool printProcessOutput;
    bool showConsole;

    SvnConsoleCommand()
        : handler(NULL)
        , printProcessOutput(true)
        , showConsole(false)
    {
    }
    ~SvnConsoleCommand() {}
    void clean()
    {
        this->handler = NULL;
        this->cmd.Clear();
        this->workingDirectory.Clear();
        this->printProcessOutput = true;
    }
};

class SubversionView;
class SvnConsole : public wxEvtHandler
{
    friend class SubversionView;
    wxStyledTextCtrl* m_sci;
    std::deque<SvnConsoleCommand*> m_queue;
    SvnConsoleCommand m_currCmd;
    wxString m_output;
    IProcess* m_process;
    Subversion2* m_plugin;
    wxString m_url;
    int m_inferiorEnd;

protected:
    virtual void OnReadProcessOutput(clProcessEvent& event);
    virtual void OnProcessEnd(clProcessEvent& event);
    virtual void OnCharAdded(wxStyledTextEvent& event);
    virtual void OnUpdateUI(wxStyledTextEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);

    void DoInitializeFontsAndColours();
    void DoExecute(const wxString& cmd,
                   SvnCommandHandler* handler,
                   const wxString& workingDirectory,
                   bool printProcessOutput,
                   bool showConsole = false);
    void DoProcessNextCommand();

public:
    SvnConsole(wxStyledTextCtrl* stc, Subversion2* plugin);
    virtual ~SvnConsole();

    void Execute(const wxString& cmd,
                 const wxString& workingDirectory,
                 SvnCommandHandler* handler,
                 bool printProcessOutput = true,
                 bool showConsole = false);
    void
    ExecuteURL(const wxString& cmd, const wxString& url, SvnCommandHandler* handler, bool printProcessOutput = true);
    void Clear();
    void Stop();
    void AppendText(const wxString& text);
    bool IsRunning();
    bool IsEmpty();
    void EnsureVisible();
};
#endif // SVNSHELL_H
