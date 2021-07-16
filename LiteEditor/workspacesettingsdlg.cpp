//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : workspacesettingsdlg.cpp
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

#include "workspacesettingsdlg.h"
#include "ColoursAndFontsManager.h"
#include "code_completion_page.h"
#include "environmentconfig.h"
#include "evnvarlist.h"
#include "globals.h"
#include "localworkspace.h"
#include "windowattrmanager.h"
#include <map>
#include <wx/dirdlg.h>
#include <wx/tokenzr.h>

WorkspaceSettingsDlg::WorkspaceSettingsDlg(wxWindow* parent, LocalWorkspace* localWorkspace)
    : WorkspaceSettingsBase(parent)
    , m_localWorkspace(localWorkspace)
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    lexer->Apply(m_textCtrlWspEnvVars);

    m_ccPage = new CodeCompletionPage(m_notebook1, CodeCompletionPage::TypeWorkspace);
    m_notebook1->AddPage(m_ccPage, _("Code Completion"), false);

    EvnVarList vars;
    EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);
    const wxStringMap_t& envSets = vars.GetEnvVarSets();
    wxString activePage = vars.GetActiveSet();
    m_choiceEnvSets->Clear();

    wxStringMap_t::const_iterator iter = envSets.begin();
    int useActiveSetIndex = m_choiceEnvSets->Append(wxGetTranslation(USE_GLOBAL_SETTINGS));

    for(; iter != envSets.end(); iter++) {
        m_choiceEnvSets->Append(iter->first);
    }

    // select the current workspace active set name
    wxString activeEnvSet;
    wxString tmpSet = localWorkspace->GetActiveEnvironmentSet();

    if(tmpSet == _("<Use Active Set>")) {
        tmpSet = wxGetTranslation(USE_GLOBAL_SETTINGS);
    }

    int where = m_choiceEnvSets->FindString(tmpSet);
    if(where == wxNOT_FOUND) {
        activeEnvSet = activePage;
        m_choiceEnvSets->SetSelection(useActiveSetIndex);

    } else {
        activeEnvSet = tmpSet;
        m_choiceEnvSets->SetSelection(where);
    }

    if(activeEnvSet.IsEmpty() == false) {
        vars.SetActiveSet(activeEnvSet);
        EnvironmentConfig::Instance()->SetSettings(vars);
    }

    wxString envvars = clCxxWorkspaceST::Get()->GetEnvironmentVariabels();
    envvars.Trim().Trim(false);

    m_textCtrlWspEnvVars->SetValue(envvars);
    ::clSetDialogBestSizeAndPosition(this);
}

WorkspaceSettingsDlg::~WorkspaceSettingsDlg() {}

wxArrayString WorkspaceSettingsDlg::GetExcludePaths() const { return wxArrayString(); }

wxArrayString WorkspaceSettingsDlg::GetIncludePaths() const { return m_ccPage->GetIncludePaths(); }

void WorkspaceSettingsDlg::OnButtonOK(wxCommandEvent& event)
{
    m_localWorkspace->SetActiveEnvironmentSet(m_choiceEnvSets->GetStringSelection());
    clCxxWorkspaceST::Get()->SetEnvironmentVariabels(m_textCtrlWspEnvVars->GetValue());
    m_ccPage->Save();
    event.Skip();
}
