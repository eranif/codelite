//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : quickdebugdlg.h
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

#ifndef __quickdebugdlg__
#define __quickdebugdlg__

#include "quickdebugbase.hpp"
#include "quickdebuginfo.h"

class QuickDebugDlg : public QuickDebugBase
{
protected:
    void OnRemoteBrowedDebuggee(wxCommandEvent& event) override;
    void OnRemoteBrowseDebugger(wxCommandEvent& event) override;
    void OnRemoteBrowseWD(wxCommandEvent& event) override;
    void OnDebuggerChanged(wxCommandEvent& event) override;
    void OnDebugOverSshUI(wxUpdateUIEvent& event) override;
    void OnSelectAlternateDebugger(wxCommandEvent& event) override;
    void OnButtonBrowseExe(wxCommandEvent& event) override;
    void OnButtonDebug(wxCommandEvent& event) override;
    void OnButtonCancel(wxCommandEvent& event) override;
    void OnButtonBrowseWD(wxCommandEvent& event) override;
    void Initialize();
    void UpdateDebuggerExecutable(const QuickDebugInfo& info);
    wxArrayString GetStartupCmds();
    void SetComboBoxValue(wxComboBox* combo, const wxString& value);

public:
    QuickDebugDlg(wxWindow* parent);
    ~QuickDebugDlg() override = default;
};

#endif // __quickdebugdlg__
