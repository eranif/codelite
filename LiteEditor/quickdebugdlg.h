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

#include "quickdebugbase.h"
#include "quickdebuginfo.h"

class QuickDebugDlg : public QuickDebugBase
{
protected:
    virtual void OnRemoteBrowedDebuggee(wxCommandEvent& event);
    virtual void OnRemoteBrowseDebugger(wxCommandEvent& event);
    virtual void OnRemoteBrowseWD(wxCommandEvent& event);
    virtual void OnDebuggerChanged(wxCommandEvent& event);
    virtual void OnDebugOverSshUI(wxUpdateUIEvent& event);
    virtual void OnSelectAlternateDebugger(wxCommandEvent& event);
    void OnButtonBrowseExe(wxCommandEvent& event);
    void OnButtonDebug(wxCommandEvent& event);
    void OnButtonCancel(wxCommandEvent& event);
    void OnButtonBrowseWD(wxCommandEvent& event);
    void Initialize();
    void UpdateDebuggerExecutable(const QuickDebugInfo& info);
    wxArrayString GetStartupCmds();
    void SetComboBoxValue(wxComboBox* combo, const wxString& value);

public:
    QuickDebugDlg(wxWindow* parent);
    virtual ~QuickDebugDlg();
};

#endif // __quickdebugdlg__
