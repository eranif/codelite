//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : gitSettingsDlg.cpp
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

#include "gitSettingsDlg.h"

#include "cl_config.h"
#include "event_notifier.h"
#include "gitentry.h"
#include "windowattrmanager.h"

GitSettingsDlg::GitSettingsDlg(wxWindow* parent, const wxString& localRepoPath, const wxString& userEnteredRepoPath,
                               const wxString& projectNameHash)
    : GitSettingsDlgBase(parent)
    , m_userEnteredRepoPath(userEnteredRepoPath)
    , m_projectNameHash(projectNameHash)
{
    GitEntry data;
    data.Load();

    m_pathGIT->SetPath(data.GetGITExecutablePath());
    m_pathGITK->SetPath(data.GetGITKExecutablePath());
    m_textCtrlGitShell->ChangeValue(data.GetGitShellCommand());

    m_checkBoxLog->SetValue(data.GetFlags() & GitEntry::Git_Verbose_Log);
    m_checkBoxTerminal->SetValue(data.GetFlags() & GitEntry::Git_Show_Terminal);
    m_checkBoxShowBlameInStatusBar->SetValue(data.IsShowBlameInfoInStatusBar());
    GitEntry::GitProperties props = GitEntry::ReadGitProperties(localRepoPath);

    m_textCtrlGlobalEmail->ChangeValue(props.global_email);
    m_textCtrlGlobalName->ChangeValue(props.global_username);
    m_textCtrlLocalEmail->ChangeValue(props.local_email);
    m_textCtrlLocalName->ChangeValue(props.local_username);
    m_dirPickerGitRepoPath->SetPath(m_userEnteredRepoPath);

    SetName("GitSettingsDlg");
    WindowAttrManager::Load(this);
}

GitSettingsDlg::~GitSettingsDlg() {}

void GitSettingsDlg::OnOK(wxCommandEvent& event)
{
    GitEntry data;
    data.Load();

    wxString repopath = m_dirPickerGitRepoPath->GetPath();
    // Sanity-check the entered path: we don't want /foo/bar/.git/, just /foo/bar
    if(repopath.Right(1) == "/") {
        repopath.RemoveLast();
    }
    if(repopath.Right(5) == "/.git") {
        repopath.RemoveLast(5);
    }

    if(!m_projectNameHash.empty() && (repopath != m_userEnteredRepoPath)) {
        m_userEnteredRepoPath = repopath;
        data.SetProjectUserEnteredRepoPath(repopath, m_projectNameHash);
        data.Save();

        // The git repo path was changed, so we need to  flag a reload
        EndModal(wxID_REFRESH);
        return;
    };

    data.SetGITExecutablePath(m_pathGIT->GetPath());
    data.SetGITKExecutablePath(m_pathGITK->GetPath());
    data.SetGitShellCommand(m_textCtrlGitShell->GetValue());

    size_t flags = 0;
    if(m_checkBoxLog->IsChecked())
        flags |= GitEntry::Git_Verbose_Log;

    if(m_checkBoxTerminal->IsChecked())
        flags |= GitEntry::Git_Show_Terminal;

    if(m_checkBoxShowBlameInStatusBar->IsChecked())
        flags |= GitEntry::Git_Show_Commit_Info;

    data.SetFlags(flags);
    data.Save();

    GitEntry::GitProperties props;
    props.global_email = m_textCtrlGlobalEmail->GetValue();
    props.global_username = m_textCtrlGlobalName->GetValue();
    props.local_email = m_textCtrlLocalEmail->GetValue();
    props.local_username = m_textCtrlLocalName->GetValue();
    GitEntry::WriteGitProperties(m_userEnteredRepoPath, props);

    // Notify about configuration changed
    wxCommandEvent evt(wxEVT_GIT_CONFIG_CHANGED);
    EventNotifier::Get()->AddPendingEvent(evt);

    EndModal(wxID_OK);
}

void GitSettingsDlg::OnLocalRepoUI(wxUpdateUIEvent& event) { event.Enable(!m_userEnteredRepoPath.IsEmpty()); }
