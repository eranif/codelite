//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debugcoredump.h
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

#ifndef DEBUGCOREDUMP_H
#define DEBUGCOREDUMP_H

#include "debugcoredumpbase.hpp" // Base class: DebugCoreDumpBase

class DebugCoreDumpDlg : public DebugCoreDumpDlgBase
{

public:
    DebugCoreDumpDlg(wxWindow* parent);
    ~DebugCoreDumpDlg() override = default;

    wxString GetCore() { return m_Core->GetValue(); }
    wxString GetExe() { return m_ExeFilepath->GetValue(); }
    wxString GetWorkingDirectory() { return m_WD->GetValue(); }
    wxString GetDebuggerName() { return m_choiceDebuggers->GetStringSelection(); }

protected:
    void OnButtonBrowseCore(wxCommandEvent& event) override;
    void OnButtonBrowseExe(wxCommandEvent& event) override;
    void OnButtonBrowseWD(wxCommandEvent& event) override;
    void OnButtonCancel(wxCommandEvent& event) override;
    void OnButtonDebug(wxCommandEvent& event) override;
    void OnDebugBtnUpdateUI(wxUpdateUIEvent& event) override;

    void Initialize();
};

#endif // DEBUGCOREDUMP_H
