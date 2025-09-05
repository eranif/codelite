//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : GitApplyPatchDlg.h
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

#ifndef GITAPPLYPATCHDLG_H
#define GITAPPLYPATCHDLG_H
#include "gitui.h"

class GitApplyPatchDlg : public GitApplyPatchDlgBase
{
public:
    GitApplyPatchDlg(wxWindow* parent);
    virtual ~GitApplyPatchDlg() = default;

    wxString GetExtraFlags() const { return m_textCtrlExtraFlags->GetValue().Trim(); }
    wxString GetPatchFile() const { return m_filePickerPatchFile->GetPath().Trim(); }

protected:
    virtual void OnApplyGitPatchUI(wxUpdateUIEvent& event);
};
#endif // GITAPPLYPATCHDLG_H
