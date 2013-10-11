//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : DebuggerDisassemblyTab.h
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

#ifndef DEBUGGERDISASSEMBLYTAB_H
#define DEBUGGERDISASSEMBLYTAB_H

#include "debuggersettingsbasedlg.h"
#include "debugger.h"
#include "cl_command_event.h"

class DebuggerDisassemblyTab : public DebuggerDisassemblyTabBase
{
    wxString m_title;
    DisassembleEntryVec_t m_lines;

protected:
    virtual void OnMarginClicked(wxStyledTextEvent& event);
    void DoClear();
    void DoCentrLine(int line);

    void OnOutput(clCommandEvent &e);
    void OnCurLine(clCommandEvent &e);
    void OnQueryFileLineDone(clCommandEvent &e);
    void OnDebuggerStopped(wxCommandEvent &e);
    void OnAllBreakpointsDeleted(wxCommandEvent &e);
    
public:
    DebuggerDisassemblyTab(wxWindow* parent, const wxString &label);
    virtual ~DebuggerDisassemblyTab();



    void SetTitle(const wxString& title) {
        this->m_title = title;
    }
    const wxString& GetTitle() const {
        return m_title;
    }
};
#endif // DEBUGGERDISASSEMBLYTAB_H
