//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : quickdebugdlg.cpp
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

#include "SFTPBrowserDlg.h"
#include "cl_sftp.h"
#include "codelite_events.h"
#include "debuggermanager.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "globals.h"
#include "quickdebugdlg.h"
#include "quickdebuginfo.h"
#include "sftp_settings.h"
#include "windowattrmanager.h"
#include <wx/dirdlg.h>
#include <wx/display.h>
#include <wx/filedlg.h>
#include <wx/tokenzr.h>

QuickDebugDlg::QuickDebugDlg(wxWindow* parent)
    : QuickDebugBase(parent)
{
    GetSizer()->Fit(this);
    Initialize();
    SetName("QuickDebugDlg");

#if !USE_SFTP
    m_checkBoxDebugOverSSH->SetValue(false);
    m_checkBoxDebugOverSSH->Enable(false);
#endif

    // Let the plugins override the values
    clDebugEvent eventShowing(wxEVT_QUICK_DEBUG_DLG_SHOWING);
    if(EventNotifier::Get()->ProcessEvent(eventShowing)) {
        if(!eventShowing.GetExecutableName().IsEmpty()) {
            m_ExeFilepath->ChangeValue(eventShowing.GetExecutableName());
        }
        if(!eventShowing.GetArguments().IsEmpty()) {
            m_textCtrlArgs->ChangeValue(eventShowing.GetArguments());
        }
        if(!eventShowing.GetWorkingDirectory().IsEmpty()) {
            m_WD->SetValue(eventShowing.GetWorkingDirectory());
        }
    }

    ::clSetDialogBestSizeAndPosition(this);
}

QuickDebugDlg::~QuickDebugDlg() {}

void QuickDebugDlg::Initialize()
{
    QuickDebugInfo info;
    EditorConfigST::Get()->ReadObject(wxT("QuickDebugDlg"), &info);

    m_choiceDebuggers->Append(DebuggerMgr::Get().GetAvailableDebuggers());
    if(m_choiceDebuggers->GetCount()) {
        m_choiceDebuggers->SetSelection(0);
    }
    if(m_choiceDebuggers->GetCount() > (unsigned int)info.GetSelectedDbg()) {
        m_choiceDebuggers->SetSelection(info.GetSelectedDbg());
    }

    m_ExeFilepath->Append(info.GetExeFilepaths());
    if(m_ExeFilepath->GetCount() > 0) {
        m_ExeFilepath->SetSelection(0);
    }

    wxArrayString wds = info.GetWds();
    wxString homeDir = wxStandardPaths::Get().GetUserConfigDir();
    if(wds.Index(homeDir) == wxNOT_FOUND) {
        wds.Add(homeDir);
    }

    m_WD->Append(wds);
    if(m_WD->GetCount() > 0) {
        m_WD->SetSelection(0);
    }
    m_textCtrlArgs->ChangeValue(info.GetArguments());

    wxString startupCmds;
    for(size_t i = 0; i < info.GetStartCmds().GetCount(); i++) {
        startupCmds << info.GetStartCmds().Item(i) << wxT("\n");
    }

    m_textCtrlCmds->ChangeValue(startupCmds);
    UpdateDebuggerExecutable(info);

#if USE_SFTP
    SFTPSettings settings;
    settings.Load();

    wxString selectedAccount = info.GetSshAccount();
    const SSHAccountInfo::Vect_t& accounts = settings.GetAccounts();
    int selection = wxNOT_FOUND;
    for(const auto& account : accounts) {
        int index = m_choiceSshAccounts->Append(account.GetAccountName());
        if(account.GetAccountName() == selectedAccount) {
            selection = index;
        }
    }

    if(selection != wxNOT_FOUND) {
        m_choiceSshAccounts->SetSelection(selection);
    } else if(!m_choiceSshAccounts->IsEmpty()) {
        m_choiceSshAccounts->SetSelection(0);
    }

    m_checkBoxDebugOverSSH->SetValue(info.IsDebugOverSSH());
#endif
}

void QuickDebugDlg::OnButtonBrowseExe(wxCommandEvent& event)
{
    wxUnusedVar(event);
#if USE_SFTP
    if(m_checkBoxDebugOverSSH->IsChecked()) {
        // Open a remote folder browser
        SFTPBrowserDlg dlg(this, _("Select executable to debug"), wxEmptyString,
                           clSFTP::SFTP_BROWSE_FOLDERS | clSFTP::SFTP_BROWSE_FILES,
                           m_choiceSshAccounts->GetStringSelection());
        if(dlg.ShowModal() == wxID_OK) {
            m_ExeFilepath->Insert(dlg.GetPath(), 0);
            m_ExeFilepath->SetSelection(0);
        }
        return;
    }
#endif
    wxString path, ans;
    wxFileName fn(GetExe());
    if(fn.FileExists()) {
        // Use the serialised path as the wxFileSelector default path
        path = fn.GetPath();
    } else {
        // Otherwise use any working dir entry, which might just have been altered
        path = wxStandardPaths::Get().GetUserConfigDir();
    }

    ans = wxFileSelector(_("Select file:"), path);
    if(!ans.empty()) {
        m_ExeFilepath->Insert(ans, 0);
        m_ExeFilepath->SetSelection(0);
    }
}

void QuickDebugDlg::OnButtonDebug(wxCommandEvent& event)
{
    wxUnusedVar(event);

    // save values
    const size_t MAX_NO_ITEMS = 10;
    QuickDebugInfo info;
    info.SetSelectedDbg(m_choiceDebuggers->GetSelection());
    info.SetExeFilepaths(ReturnWithStringPrepended(m_ExeFilepath->GetStrings(), GetExe(), MAX_NO_ITEMS));
    info.SetWDs(ReturnWithStringPrepended(m_WD->GetStrings(), GetWorkingDirectory(), MAX_NO_ITEMS));
    info.SetStartCmds(GetStartupCmds());
    info.SetArguments(m_textCtrlArgs->GetValue());
    info.SetAlternateDebuggerExec(m_textCtrlDebuggerExec->GetValue());
    info.SetDebugOverSSH(m_checkBoxDebugOverSSH->IsChecked());
    info.SetSshAccount(m_choiceSshAccounts->GetStringSelection());
    EditorConfigST::Get()->WriteObject(wxT("QuickDebugDlg"), &info);

    // Let the plugins persist the data
    clDebugEvent eventDismissed(wxEVT_QUICK_DEBUG_DLG_DISMISSED_OK);
    eventDismissed.SetExecutableName(m_ExeFilepath->GetValue());
    eventDismissed.SetArguments(m_textCtrlArgs->GetValue());
    eventDismissed.SetWorkingDirectory(m_WD->GetValue());
    EventNotifier::Get()->ProcessEvent(eventDismissed);

    // Notify that a debug session is starting
    clDebugEvent eventQuickDebug(wxEVT_QUICK_DEBUG);
    eventQuickDebug.SetAlternateDebuggerPath(m_textCtrlDebuggerExec->GetValue());
    eventQuickDebug.SetDebuggerName(m_choiceDebuggers->GetStringSelection());
    eventQuickDebug.SetExecutableName(m_ExeFilepath->GetStringSelection());
    eventQuickDebug.SetWorkingDirectory(m_WD->GetStringSelection());
    eventQuickDebug.SetArguments(m_textCtrlArgs->GetValue());
    eventQuickDebug.SetStartupCommands(wxJoin(GetStartupCmds(), '\n'));
#if USE_SFTP
    SFTPSettings settings;
    settings.Load();
    SSHAccountInfo sshAccount;
    settings.GetAccount(m_choiceSshAccounts->GetStringSelection(), sshAccount);
    eventQuickDebug.SetIsSSHDebugging(m_checkBoxDebugOverSSH->IsChecked());
    eventQuickDebug.SetSshAccount(sshAccount);
#endif
    EventNotifier::Get()->AddPendingEvent(eventQuickDebug);
    EndModal(wxID_OK);
}

void QuickDebugDlg::OnButtonCancel(wxCommandEvent& event)
{
    wxUnusedVar(event);

    EndModal(wxID_CANCEL);
}

wxString QuickDebugDlg::GetArguments() { return m_textCtrlArgs->GetValue(); }

wxString QuickDebugDlg::GetDebuggerName() { return m_choiceDebuggers->GetStringSelection(); }

wxString QuickDebugDlg::GetExe() { return m_ExeFilepath->GetValue(); }

wxArrayString QuickDebugDlg::GetStartupCmds()
{
    wxString cmds = m_textCtrlCmds->GetValue();
    cmds.Trim().Trim(false);

    return wxStringTokenize(cmds, wxT("\n\r"), wxTOKEN_STRTOK);
}

wxString QuickDebugDlg::GetWorkingDirectory() { return m_WD->GetValue(); }

void QuickDebugDlg::OnButtonBrowseWD(wxCommandEvent& event)
{
    wxUnusedVar(event);

    if(m_checkBoxDebugOverSSH->IsChecked()) {
#if USE_SFTP
        // Open a remote folder browser
        SFTPBrowserDlg dlg(this, _("Select working directory"), wxEmptyString, clSFTP::SFTP_BROWSE_FOLDERS,
                           m_choiceSshAccounts->GetStringSelection());
        if(dlg.ShowModal() == wxID_OK) {
            m_WD->Insert(dlg.GetPath(), 0);
            m_WD->SetSelection(0);
        }
#endif
    } else {
        wxString ans, path(GetWorkingDirectory());
        if(!wxFileName::DirExists(path)) {
            path = wxStandardPaths::Get().GetUserConfigDir();
        }

        ans = wxDirSelector(_("Select working directory:"), path);
        if(!ans.empty()) {
            m_WD->Insert(ans, 0);
            m_WD->SetSelection(0);
        }
    }
}
void QuickDebugDlg::OnSelectAlternateDebugger(wxCommandEvent& event)
{
#if USE_SFTP
    if(m_checkBoxDebugOverSSH->IsChecked()) {
        SFTPBrowserDlg dlg(this, _("Select gdb"), wxEmptyString,
                           clSFTP::SFTP_BROWSE_FOLDERS | clSFTP::SFTP_BROWSE_FILES,
                           m_choiceSshAccounts->GetStringSelection());
        if(dlg.ShowModal() == wxID_OK) {
            m_textCtrlDebuggerExec->ChangeValue(dlg.GetPath());
        }
        return;
    }
#endif
    wxString debuggerPath = ::wxFileSelector(_("Choose debugger:"), wxStandardPaths::Get().GetUserConfigDir());
    if(debuggerPath.IsEmpty())
        return;
    m_textCtrlDebuggerExec->ChangeValue(debuggerPath);
}

void QuickDebugDlg::OnDebugOverSshUI(wxUpdateUIEvent& event) { event.Enable(m_checkBoxDebugOverSSH->IsChecked()); }

void QuickDebugDlg::OnDebuggerChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(!m_checkBoxDebugOverSSH->IsChecked()) {
        QuickDebugInfo info;
        EditorConfigST::Get()->ReadObject("QuickDebugDlg", &info);
        UpdateDebuggerExecutable(info);
    }
}

void QuickDebugDlg::UpdateDebuggerExecutable(const QuickDebugInfo& info)
{
    // set the default debugger executable
    DebuggerInformation debuggerInfo;
    if(DebuggerMgr::Get().GetDebuggerInformation(m_choiceDebuggers->GetStringSelection(), debuggerInfo)) {
        m_textCtrlDebuggerExec->ChangeValue(debuggerInfo.path);
    }

    // Check to see if got alternate value stored
    if(!info.GetAlternateDebuggerExec().empty()) {
        m_textCtrlDebuggerExec->ChangeValue(info.GetAlternateDebuggerExec());
    }
}
