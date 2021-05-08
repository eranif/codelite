//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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

#include "gitCommitEditor.h"
#include "gitFileDiffDlg.h"
#include "windowattrmanager.h"

#include "globals.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>

#ifndef __WXMSW__
#include "icons/icon_git.xpm"
#endif

GitFileDiffDlg::GitFileDiffDlg(wxWindow* parent)
    : GitFileDiffDlgBase(parent)
{
    m_editor->InitStyles();
    SetIcon(wxICON(icon_git));
    SetName("GitFileDiffDlg");
    WindowAttrManager::Load(this);
}

/*******************************************************************************/

GitFileDiffDlg::~GitFileDiffDlg() {}

/*******************************************************************************/
void GitFileDiffDlg::SetDiff(const wxString& diff)
{
    m_editor->SetText(diff);
    m_editor->SetReadOnly(true);
}

void GitFileDiffDlg::OnSaveAsPatch(wxCommandEvent& event)
{
    wxString path = ::wxFileSelector(_("Save as"), "", "untitled", "patch", wxFileSelectorDefaultWildcardStr,
                                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if(!path.IsEmpty()) {
        ::WriteFileWithBackup(path, m_editor->GetText(), false);
        ::wxMessageBox("Diff written to:\n" + path, "CodeLite");
        // Close the dialog
        CallAfter(&GitFileDiffDlg::EndModal, wxID_CLOSE);
    }
    // do nothing
}

void GitFileDiffDlg::OnCloseDialog(wxCommandEvent& event) { EndModal(wxID_OK); }
