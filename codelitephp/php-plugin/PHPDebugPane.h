//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPDebugPane.h
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

#ifndef PHPDEBUGPANE_H
#define PHPDEBUGPANE_H

#include "TerminalEmulatorUI.h"
#include "XDebugManager.h"
#include "bitmap_loader.h"
#include "php_event.h"
#include "php_ui.h"
#include "xdebugevent.h"
#include <cl_command_event.h>
#include <macros.h>

class PHPDebugPane : public PHPDebugPaneBase
{
    TerminalEmulatorUI* m_console;
    BitmapLoader::Vec_t m_bitmaps;

public:
    PHPDebugPane(wxWindow* parent);
    virtual ~PHPDebugPane();
    void OnUpdateStackTrace(XDebugEvent& e);
    void OnRefreshBreakpointsView(XDebugEvent& e);
    void OnXDebugSessionEnded(XDebugEvent& e);
    void OnXDebugSessionStarted(XDebugEvent& e);
    void OnXDebugSessionStarting(XDebugEvent& event);
    void OnSettingsChanged(wxCommandEvent& event);
    void SelectTab(const wxString& title);
    void SetTerminal(TerminalEmulator* terminal) { m_console->SetTerminal(terminal); }

protected:
    virtual void OnCallStackMenu(wxDataViewEvent& event);
    virtual void OnBreakpointItemActivated(wxDataViewEvent& event);
    XDebugBreakpoint GetBreakpoint(const wxDataViewItem& item) const;

protected:
    virtual void OnClearAll(wxCommandEvent& event);
    virtual void OnClearAllUI(wxUpdateUIEvent& event);
    virtual void OnDeleteBreakpoint(wxCommandEvent& event);
    virtual void OnDeleteBreakpointUI(wxUpdateUIEvent& event);
    virtual void OnCallStackItemActivated(wxDataViewEvent& event);
};
#endif // PHPDEBUGPANE_H
