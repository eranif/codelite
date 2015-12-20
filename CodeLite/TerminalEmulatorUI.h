//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : TerminalEmulatorUI.h
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

#ifndef TERMINALEMULATORUI_H
#define TERMINALEMULATORUI_H

#include "TerminalEmulatorUIBase.h"
#include "TerminalEmulator.h"

class WXDLLIMPEXP_CL TerminalEmulatorUI : public TerminalEmulatorUIBase
{
    TerminalEmulator* m_terminal;
private:
    void DoBindTerminal(TerminalEmulator* terminal);
    void DoUnBindTerminal(TerminalEmulator* terminal);
    
public:
    TerminalEmulatorUI(wxWindow* parent);
    virtual ~TerminalEmulatorUI();
    
    void Clear();
    void SetTerminal(TerminalEmulator* terminal) ;
    TerminalEmulator* GetTerminal() { return m_terminal; }
    wxStyledTextCtrl* GetTerminalOutputWindow() { return m_stc; }
    
protected:
    virtual void OnSendCommand(wxCommandEvent& event);
    void OnProcessExit(clCommandEvent& e);
    void OnProcessOutput(clCommandEvent& e);
};
#endif // TERMINALEMULATORUI_H
