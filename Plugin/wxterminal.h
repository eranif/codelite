//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : wxterminal.h
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

#ifndef __wxterminal__
#define __wxterminal__

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "wxterminalbase.h"
#include <queue>
#include <set>
#include <vector>

class clTerminalHistory;
class IProcess;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_EXECUTE_COMMAND, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_CTRL_C, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_KILL_INFERIOR, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TERMINAL_EXIT_WHEN_DONE, clCommandEvent);

class WXDLLIMPEXP_SDK wxTerminal : public wxTerminalBase
{
protected:
    IProcess* m_process;
    wxString m_workingDir;
    bool m_exitWhenProcessDies;
    bool m_exitOnKey;
#if defined(__WXGTK__) || defined(__WXMAC__)
    wxString m_tty;
    IProcess* m_dummyProcess;
    int m_slave;
#endif
    bool m_interactive;
    wxString m_outputBuffer;
    clTerminalHistory* m_history;

protected:
    void DoProcessCommand(const wxString& command);
    void DoCtrlC();
    void DoFlushOutputBuffer();
    void InsertCommandText(const wxString& command);
    wxString GetCommandText();
    void OnKey(wxKeyEvent& event);
    
protected:
    // Handlers for wxTerminalBase events.
    virtual void OnReadProcessOutput(clProcessEvent& event);
    virtual void OnProcessEnd(clProcessEvent& event);
    virtual void OnIdle(wxIdleEvent& event);

    // Edit events
    void OnCopy(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);

    // Keys
    void OnDown(wxKeyEvent& event);
    void OnUp(wxKeyEvent& event);
    void OnLeft(wxKeyEvent& event);
    void OnRight(wxKeyEvent& event);
    void OnEnter();
    void OnCtrlC(wxKeyEvent& event);

    // Theme
    void OnThemeChanged(wxCommandEvent& event);

public:
    wxTerminal(wxWindow* parent);
    virtual ~wxTerminal();

    void Focus() { m_textCtrl->SetFocus(); }
    void SetInteractive(bool interactive) { this->m_interactive = interactive; }
    bool IsInteractive() const { return m_interactive; }
    void Execute(const wxString& command, bool exitWhenDone = false, const wxString& workingDir = wxT(""));
    void KillInferior();
    bool IsRunning();
    void Clear();
    bool IsEmpty() const { return m_textCtrl->IsEmpty(); }
    // Give access to the text control
    wxStyledTextCtrl* GetCtrl() { return m_textCtrl; }

    // Add text with the EOL at the end
    void AddTextWithEOL(const wxString& text);
    void AddTextRaw(const wxString& text);

#if defined(__WXGTK__) || defined(__WXMAC__)
    wxString StartTTY();
    wxString GetTTY() const { return m_tty; }
    void StopTTY();
#endif
};

#endif // __wxterminal__
