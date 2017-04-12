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

#include "windowattrmanager.h"
#include "quickdebuginfo.h"
#include <wx/tokenzr.h>
#include "editor_config.h"
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/display.h>
#include "debuggermanager.h"
#include "globals.h"
#include "quickdebugdlg.h"

QuickDebugDlg::QuickDebugDlg(wxWindow* parent)
    : QuickDebugBase(parent)
{
    GetSizer()->Fit(this);
    Initialize();
    SetName("QuickDebugDlg");
    WindowAttrManager::Load(this);
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
    m_textCtrlDebuggerExec->ChangeValue(info.GetAlternateDebuggerExec());
}

void QuickDebugDlg::OnButtonBrowseExe(wxCommandEvent& event)
{
    wxUnusedVar(event);

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
    EditorConfigST::Get()->WriteObject(wxT("QuickDebugDlg"), &info);

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
void QuickDebugDlg::OnSelectAlternateDebugger(wxCommandEvent& event)
{
    wxString debuggerPath = ::wxFileSelector(_("Choose debugger:"), wxStandardPaths::Get().GetUserConfigDir());
    if(debuggerPath.IsEmpty()) return;
    m_textCtrlDebuggerExec->ChangeValue(debuggerPath);
}
