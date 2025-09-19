//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : svn_sync_dialog.cpp
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

#include "svn_sync_dialog.h"
#include <wx/dirdlg.h>
#include "subversion2.h"
#include "imanager.h"
#include "windowattrmanager.h"

SvnSyncDialog::SvnSyncDialog(wxWindow* parent,
                             Subversion2* plugin,
                             const wxString& rootDir,
                             bool excludeBin,
                             const wxString& excludeExtensions)
    : SvnSyncDialogBaseClass(parent)
    , m_plugin(plugin)
    , m_rootDir(rootDir)
    , m_excludeExtensions(excludeExtensions)
    , m_excludeBin(excludeBin)
{
    m_dirPickerRootDir->SetInitialDirectory(m_rootDir.IsEmpty() ? wxGetCwd() : m_rootDir);
    m_dirPickerRootDir->SetPath(m_rootDir.IsEmpty() ? wxGetCwd() : m_rootDir);
    m_textCtrlExclude->SetValue(m_excludeExtensions);
    m_checkBoxBin->SetValue(m_excludeBin);

    UpdateUrl(m_rootDir);

    SetName("SvnSyncDialog");
    WindowAttrManager::Load(this);
}

void SvnSyncDialog::UpdateUrl(const wxString& rootDir)
{
    SvnInfo svnInfo;
    m_plugin->DoGetSvnInfoSync(svnInfo, rootDir);

    wxString textLine(_("Root URL:  "));
    if(svnInfo.m_sourceUrl.IsEmpty()) {
        textLine += _("<not applicable>");
    } else {
        textLine += svnInfo.m_sourceUrl;
    }
    m_staticTextSvnInfo->SetLabel(textLine);
}

void SvnSyncDialog::OnButtonOK(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_rootDir = m_dirPickerRootDir->GetPath();
    m_excludeExtensions = m_textCtrlExclude->GetValue();
    m_excludeBin = m_checkBoxBin->IsChecked();
    EndModal(wxID_OK);
}

void SvnSyncDialog::OnOkUI(wxUpdateUIEvent& event) { event.Enable(m_dirPickerRootDir->GetPath().IsEmpty() == false); }
