//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : GitApplyPatchDlg.cpp
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

#include "GitApplyPatchDlg.h"
#include <wx/filename.h>
#include "windowattrmanager.h"

GitApplyPatchDlg::GitApplyPatchDlg(wxWindow* parent)
    : GitApplyPatchDlgBase(parent)
{
    SetName("GitApplyPatchDlg");
    WindowAttrManager::Load(this);
}

void GitApplyPatchDlg::OnApplyGitPatchUI(wxUpdateUIEvent& event)
{
    wxFileName fnPathFile(m_filePickerPatchFile->GetPath());
    event.Enable(fnPathFile.IsOk() && fnPathFile.Exists());
}
