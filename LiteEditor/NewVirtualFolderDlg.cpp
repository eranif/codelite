//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : NewVirtualFolderDlg.cpp
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

#include "NewVirtualFolderDlg.h"
#include "workspace.h"
#include "windowattrmanager.h"
#include "cl_config.h"

NewVirtualFolderDlg::NewVirtualFolderDlg(wxWindow* parent, const wxString& currentVD)
    : NewVirtualFolderDlgBase(parent)
{
    m_checkBoxCreateOnDisk->SetValue( clConfig::Get().Read("CreateVirtualFoldersOnDisk", false) );
    wxString project_name = currentVD.BeforeFirst(':');
    wxString vd_path = currentVD.AfterFirst(':');
    vd_path.Replace(":", wxFILE_SEP_PATH);
    wxString errmsg;
    ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(project_name, errmsg);
    wxString projectPath = proj->GetFileName().GetPath();
    m_basePath = wxFileName(projectPath + wxFILE_SEP_PATH + vd_path, "").GetPath();
    WindowAttrManager::Load(this, "NewVirtualFolderDlg");
}

NewVirtualFolderDlg::~NewVirtualFolderDlg()
{
    WindowAttrManager::Save(this, "NewVirtualFolderDlg");
    clConfig::Get().Write("CreateVirtualFoldersOnDisk", m_checkBoxCreateOnDisk->IsChecked());
}

void NewVirtualFolderDlg::OnCreateOnDiskUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxCreateOnDisk->IsChecked());
}

void NewVirtualFolderDlg::OnNameUpdated(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if ( m_checkBoxCreateOnDisk->IsChecked() ) {
        DoUpdatePath();
    }
}

void NewVirtualFolderDlg::OnOkUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_textCtrlName->IsEmpty() );
}

void NewVirtualFolderDlg::OnCreateOnFolderChecked(wxCommandEvent& event)
{
    if ( event.IsChecked() ) {
        DoUpdatePath();
    } else {
        m_textCtrlPath->Clear();
    }
}

void NewVirtualFolderDlg::DoUpdatePath()
{
    wxString curpath;
    curpath << m_basePath << wxFILE_SEP_PATH << m_textCtrlName->GetValue();
    m_textCtrlPath->ChangeValue( wxFileName(curpath, "").GetPath() );
}

