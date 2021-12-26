//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : shelltab.h
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
#ifndef SHELLTAB_H
#define SHELLTAB_H

#include "OutputDebugStringThread.h"
#include "clWorkspaceEvent.hpp"
#include "outputtabwindow.h"

#include <wx/combobox.h>

class wxTerminal;
class AsyncExeCmd;

class ShellTab : public OutputTabWindow
{
protected:
    wxSizer* m_inputSizer;
    wxSizer* m_vertSizer;
    wxComboBox* m_input;
    AsyncExeCmd* m_cmd;

protected:
    static void InitStyle(wxStyledTextCtrl* sci);

    virtual bool DoSendInput(const wxString& line);

    virtual void OnProcStarted(wxCommandEvent& e);
    virtual void OnProcOutput(wxCommandEvent& e);
    virtual void OnProcError(wxCommandEvent& e);
    virtual void OnProcEnded(wxCommandEvent& e);

    virtual void OnSendInput(wxCommandEvent& e);
    virtual void OnStopProc(wxCommandEvent& e);
    virtual void OnKeyDown(wxKeyEvent& e);
    virtual void OnEnter(wxCommandEvent& e);
    virtual void OnUpdateUI(wxUpdateUIEvent& e);
    virtual void OnHoldOpenUpdateUI(wxUpdateUIEvent& e);
    void OnThemeChanged(wxCommandEvent& e);

    DECLARE_EVENT_TABLE()

public:
    ShellTab(wxWindow* parent, wxWindowID id, const wxString& name);
    virtual ~ShellTab();
};

class OutputTab : public ShellTab
{
    OutputDebugStringThread* m_thread;
    bool m_outputDebugStringActive;

protected:
    void DoSetCollecting(bool b);

public:
    OutputTab(wxWindow* parent, wxWindowID id, const wxString& name);
    virtual ~OutputTab();

    /**
     * @brief Windows only. Report a string from a debuggee process which used the "OuptutDebugString" method
     */
    void OnOutputDebugString(clCommandEvent& event);

    void OnDebugStarted(clDebugEvent& event);
    void OnDebugStopped(clDebugEvent& event);
    virtual void OnProcStarted(wxCommandEvent& e);
    virtual void OnProcEnded(wxCommandEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
};

class DebugTab : public wxPanel
{
    wxTerminal* m_terminal;
    clToolBar* m_toolbar;

protected:
    void OnUpdateUI(wxUpdateUIEvent& e);
    void OnHoldOpenUpdateUI(wxUpdateUIEvent& e);
    void OnEnableDbgLog(wxCommandEvent& event);
    void OnEnableDbgLogUI(wxUpdateUIEvent& event);
    void OnCtrlC(clCommandEvent& event);
    void OnExecuteCommand(clCommandEvent& event);

public:
    DebugTab(wxWindow* parent, wxWindowID id, const wxString& name);
    virtual ~DebugTab();
    clToolBar* GetToolBar() { return m_toolbar; }
    void AppendLine(const wxString& line);
};

#endif // SHELLTAB_H
