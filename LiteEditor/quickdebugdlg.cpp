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

#include "ColoursAndFontsManager.h"
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

    // Let the plugins override the values
    clDebugEvent eventShowing(wxEVT_QUICK_DEBUG_DLG_SHOWING);
    if(EventNotifier::Get()->ProcessEvent(eventShowing)) {
        if(!eventShowing.GetExecutableName().IsEmpty()) {
            SetComboBoxValue(m_ExeFilepath, eventShowing.GetExecutableName());
        }
        if(!eventShowing.GetArguments().IsEmpty()) {
            m_textCtrlArgs->ChangeValue(eventShowing.GetArguments());
        }
        if(!eventShowing.GetWorkingDirectory().IsEmpty()) {
            SetComboBoxValue(m_WD, eventShowing.GetWorkingDirectory());
        }
    }
    ::clSetDialogBestSizeAndPosition(this);
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->Apply(m_stcRemoteStartupCommands);
        lexer->Apply(m_stcStartupCmds);
    }

#if !USE_SFTP
    m_panelSSH->Enable(false);
#endif
}

QuickDebugDlg::~QuickDebugDlg() {}

void QuickDebugDlg::Initialize()
{
    QuickDebugInfo info;
    EditorConfigST::Get()->ReadObject(wxT("QuickDebugDlg"), &info);

    if(info.IsDebugOverSSH()) {
        m_notebook47->SetSelection(1);
    } else {
        m_notebook47->SetSelection(0);
    }

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

    m_stcStartupCmds->SetText(startupCmds);
    UpdateDebuggerExecutable(info);

#if USE_SFTP
    // ssh related
    SFTPSettings settings;
    settings.Load();

    const wxString& selectedAccount = info.GetSshAccount();
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

    m_stcRemoteStartupCommands->SetText(wxJoin(info.GetRemoteStartCmds(), '\n'));
    m_textCtrlRemoteArgs->ChangeValue(info.GetRemoteArgs());
    m_textCtrlRemoteDebuggee->ChangeValue(info.GetRemoteExe());
    m_textCtrlRemoteDebugger->ChangeValue(info.GetRemoteDebugger());
    m_textCtrlRemoteWD->ChangeValue(info.GetRemoteWD());
#endif
}

void QuickDebugDlg::OnButtonBrowseExe(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString path, ans;
    wxFileName fn(m_ExeFilepath->GetValue());
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
    info.SetExeFilepaths(
        ReturnWithStringPrepended(m_ExeFilepath->GetStrings(), m_ExeFilepath->GetValue(), MAX_NO_ITEMS));
    info.SetWDs(ReturnWithStringPrepended(m_WD->GetStrings(), m_WD->GetStringSelection(), MAX_NO_ITEMS));
    info.SetStartCmds(GetStartupCmds());
    info.SetArguments(m_textCtrlArgs->GetValue());
    info.SetAlternateDebuggerExec(m_textCtrlDebuggerExec->GetValue());

    // ssh tab
    info.SetDebugOverSSH(m_notebook47->GetSelection() == 1);
    info.SetSshAccount(m_choiceSshAccounts->GetStringSelection());
    info.SetRemoteDebugger(m_textCtrlRemoteDebugger->GetValue());
    info.SetRemoteExe(m_textCtrlRemoteDebuggee->GetValue());
    info.SetRemoteArgs(m_textCtrlRemoteArgs->GetValue());
    info.SetRemoteWD(m_textCtrlRemoteWD->GetValue());
    info.SetRemoteStartCmds(wxStringTokenize(m_stcRemoteStartupCommands->GetText(), "\n\r", wxTOKEN_STRTOK));
    EditorConfigST::Get()->WriteObject(wxT("QuickDebugDlg"), &info);

    // Let the plugins persist the data
    clDebugEvent eventDismissed(wxEVT_QUICK_DEBUG_DLG_DISMISSED_OK);
    eventDismissed.SetExecutableName(m_ExeFilepath->GetValue());
    eventDismissed.SetArguments(m_textCtrlArgs->GetValue());
    eventDismissed.SetWorkingDirectory(m_WD->GetValue());
    EventNotifier::Get()->ProcessEvent(eventDismissed);

    // Notify that a debug session is starting
    clDebugEvent eventQuickDebug(wxEVT_QUICK_DEBUG);
    if(m_notebook47->GetSelection() == 1) {
        // SSH is selected
        eventQuickDebug.SetIsSSHDebugging(true);
        eventQuickDebug.SetSshAccount(m_choiceSshAccounts->GetStringSelection());
        eventQuickDebug.SetAlternateDebuggerPath(m_textCtrlRemoteDebugger->GetValue());
        eventQuickDebug.SetDebuggerName("GNU gdb debugger");
        eventQuickDebug.SetExecutableName(m_textCtrlRemoteDebuggee->GetValue());
        eventQuickDebug.SetWorkingDirectory(m_textCtrlRemoteWD->GetValue());
        eventQuickDebug.SetArguments(m_textCtrlRemoteArgs->GetValue());
        eventQuickDebug.SetStartupCommands(m_stcRemoteStartupCommands->GetValue());

    } else {
        eventQuickDebug.SetAlternateDebuggerPath(m_textCtrlDebuggerExec->GetValue());
        eventQuickDebug.SetDebuggerName(m_choiceDebuggers->GetStringSelection());
        eventQuickDebug.SetExecutableName(m_ExeFilepath->GetStringSelection());
        eventQuickDebug.SetWorkingDirectory(m_WD->GetStringSelection());
        eventQuickDebug.SetArguments(m_textCtrlArgs->GetValue());
        eventQuickDebug.SetStartupCommands(wxJoin(GetStartupCmds(), '\n'));
    }
    EventNotifier::Get()->AddPendingEvent(eventQuickDebug);
    EndModal(wxID_OK);
}

void QuickDebugDlg::OnButtonCancel(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_CANCEL);
}

wxArrayString QuickDebugDlg::GetStartupCmds()
{
    wxString cmds = m_stcStartupCmds->GetText();
    cmds.Trim().Trim(false);
    return wxStringTokenize(cmds, wxT("\n\r"), wxTOKEN_STRTOK);
}

void QuickDebugDlg::OnButtonBrowseWD(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString ans, path(m_WD->GetValue());
    if(!wxFileName::DirExists(path)) {
        path = wxStandardPaths::Get().GetUserConfigDir();
    }

    ans = wxDirSelector(_("Select working directory:"), path);
    if(!ans.empty()) {
        m_WD->Insert(ans, 0);
        m_WD->SetSelection(0);
    }
}
void QuickDebugDlg::OnSelectAlternateDebugger(wxCommandEvent& event)
{
    wxString debuggerPath = ::wxFileSelector(_("Choose debugger:"), wxStandardPaths::Get().GetUserConfigDir());
    if(debuggerPath.IsEmpty())
        return;
    m_textCtrlDebuggerExec->ChangeValue(debuggerPath);
}

void QuickDebugDlg::OnDebugOverSshUI(wxUpdateUIEvent& event)
{
#if USE_SFTP
    event.Enable(true);
#else
    event.Enable(false);
#endif
}

void QuickDebugDlg::OnDebuggerChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    QuickDebugInfo info;
    EditorConfigST::Get()->ReadObject("QuickDebugDlg", &info);
    UpdateDebuggerExecutable(info);
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

void QuickDebugDlg::OnRemoteBrowedDebuggee(wxCommandEvent& event)
{
    // Open a remote folder browser
    wxUnusedVar(event);
#if USE_SFTP
    SFTPBrowserDlg dlg(this, _("Select executable to debug"), wxEmptyString,
                       clSFTP::SFTP_BROWSE_FOLDERS | clSFTP::SFTP_BROWSE_FILES,
                       m_choiceSshAccounts->GetStringSelection());
    if(dlg.ShowModal() == wxID_OK) {
        m_textCtrlRemoteDebuggee->ChangeValue(dlg.GetPath());
    }
#endif
}

void QuickDebugDlg::OnRemoteBrowseDebugger(wxCommandEvent& event)
{
    wxUnusedVar(event);
#if USE_SFTP
    SFTPBrowserDlg dlg(this, _("Select gdb"), wxEmptyString, clSFTP::SFTP_BROWSE_FOLDERS | clSFTP::SFTP_BROWSE_FILES,
                       m_choiceSshAccounts->GetStringSelection());
    if(dlg.ShowModal() == wxID_OK) {
        m_textCtrlRemoteDebugger->ChangeValue(dlg.GetPath());
    }
#endif
}

void QuickDebugDlg::OnRemoteBrowseWD(wxCommandEvent& event)
{
    wxUnusedVar(event);
#if USE_SFTP
    // Open a remote folder browser
    SFTPBrowserDlg dlg(this, _("Select working directory"), wxEmptyString, clSFTP::SFTP_BROWSE_FOLDERS,
                       m_choiceSshAccounts->GetStringSelection());
    if(dlg.ShowModal() == wxID_OK) {
        m_textCtrlRemoteWD->ChangeValue(dlg.GetPath());
    }
#endif
}

void QuickDebugDlg::SetComboBoxValue(wxComboBox* combo, const wxString& value)
{
    int where = combo->FindString(value);
    if(where != wxNOT_FOUND) {
        combo->SetSelection(where);
    } else {
        // new value
        where = combo->Insert(value, 0);
        combo->SetSelection(where);
    }
}
