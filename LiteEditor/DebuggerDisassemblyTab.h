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

#include "cl_command_event.h"
#include "debugger.h"
#include "debuggersettingsbasedlg.h"

//++++++++++----------------------------------
// Our custom model
//++++++++++----------------------------------

class RegistersViewModelClientData : public wxClientData
{
    bool m_firstColModified;
    bool m_secondColModified;

public:
    RegistersViewModelClientData(bool firstModified, bool secondModified)
        : m_firstColModified(firstModified)
        , m_secondColModified(secondModified)
    {
    }

    void SetFirstColModified(bool firstColModified) { this->m_firstColModified = firstColModified; }
    void SetSecondColModified(bool secondColModified) { this->m_secondColModified = secondColModified; }
    bool IsFirstColModified() const { return m_firstColModified; }
    bool IsSecondColModified() const { return m_secondColModified; }
};

//++++++++++----------------------------------
// DebuggerDisassemblyTab
//++++++++++----------------------------------

class DebuggerDisassemblyTab : public DebuggerDisassemblyTabBase
{
    wxString m_title;
    DisassembleEntryVec_t m_lines;
    wxStringMap_t m_oldValues;

protected:
    virtual void OnMarginClicked(wxStyledTextEvent& event);

    void DoClearRegistersView();
    void DoClearDisassembleView();
    void DoCentrLine(int line);

    void OnOutput(clCommandEvent& e);
    void OnCurLine(clCommandEvent& e);
    void OnQueryFileLineDone(clCommandEvent& e);
    void OnRefreshView(clCommandEvent& e);
    void OnShowRegisters(clCommandEvent& e);
    void OnDebuggerStopped(clDebugEvent& e);
    void OnAllBreakpointsDeleted(wxCommandEvent& e);

public:
    DebuggerDisassemblyTab(wxWindow* parent, const wxString& label);
    virtual ~DebuggerDisassemblyTab();

    void SetTitle(const wxString& title) { this->m_title = title; }
    const wxString& GetTitle() const { return m_title; }
    wxArrayString GetRegisterNames() const;
};
#endif // DEBUGGERDISASSEMBLYTAB_H
