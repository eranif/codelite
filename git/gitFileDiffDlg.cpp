//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : gitFileDiffDlg.cpp
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

#include "gitFileDiffDlg.h"
#include "gitCommitEditor.h"
#include "windowattrmanager.h"

#include "icons/icon_git.xpm"
#include "globals.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>

GitFileDiffDlg::GitFileDiffDlg(wxWindow* parent)
    : GitFileDiffDlgBase(parent)
{
    m_editor->InitStyles();
    SetIcon(wxICON(icon_git));
    WindowAttrManager::Load(this, wxT("GitFileDiffDlg"), NULL);
}

/*******************************************************************************/

GitFileDiffDlg::~GitFileDiffDlg() { WindowAttrManager::Save(this, wxT("GitFileDiffDlg"), NULL); }

/*******************************************************************************/
void GitFileDiffDlg::SetDiff(const wxString& diff)
{
    m_editor->SetText(diff);
    m_editor->SetReadOnly(true);
}

void GitFileDiffDlg::OnSaveAsPatch(wxCommandEvent& event)
{
    wxString path = ::wxFileSelector(
        _("Save as"), "", "untitled", "patch", wxFileSelectorDefaultWildcardStr, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if(!path.IsEmpty()) {
        ::WriteFileWithBackup(path, m_editor->GetText(), false);
        ::wxMessageBox("Diff written to:\n" + path, "CodeLite");
        // Close the dialog
        CallAfter(&GitFileDiffDlg::EndModal, wxID_CLOSE);
    }
    // do nothing
}

void GitFileDiffDlg::OnCloseDialog(wxCommandEvent& event) { EndModal(wxID_OK); }
