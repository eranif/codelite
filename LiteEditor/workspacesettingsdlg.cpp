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
#include "code_completion_page.h"
#include "localworkspace.h"
#include "environmentconfig.h"
#include "evnvarlist.h"
#include "windowattrmanager.h"
#include <wx/dirdlg.h>
#include <map>
#include <wx/tokenzr.h>
#include "globals.h"

WorkspaceSettingsDlg::WorkspaceSettingsDlg(wxWindow* parent, LocalWorkspace* localWorkspace)
    : WorkspaceSettingsBase(parent)
    , m_localWorkspace(localWorkspace)
{
    m_ccPage = new CodeCompletionPage(m_notebook1, CodeCompletionPage::TypeWorkspace);
    m_notebook1->AddPage(m_ccPage, wxT("Code Completion"), false);

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
    SetName("WorkspaceSettingsDlg");
    WindowAttrManager::Load(this);
}

WorkspaceSettingsDlg::~WorkspaceSettingsDlg() {}

// void WorkspaceSettingsDlg::OnAddIncludePath( wxCommandEvent& event )
//{
//	wxUnusedVar(event);
//	wxString new_path = wxDirSelector(_("Add Parser Search Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition,
//this);
//	if (new_path.IsEmpty() == false) {
//		wxString curpaths = m_textCtrlIncludePaths->GetValue();
//		curpaths.Trim().Trim(false);
//		if(curpaths.IsEmpty()) {
//			curpaths << wxT("\n");
//		}
//		curpaths << new_path;
//		m_textCtrlIncludePaths->SetValue(curpaths);
//	}
//}
//
// void WorkspaceSettingsDlg::OnAddExcludePath( wxCommandEvent& event )
//{
//	wxUnusedVar(event);
//	wxString new_path = wxDirSelector(_("Add Parser Exclude Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition,
//this);
//	if (new_path.IsEmpty() == false) {
//		wxString curpaths = m_textCtrlExcludePaths->GetValue();
//		curpaths.Trim().Trim(false);
//		if(curpaths.IsEmpty()) {
//			curpaths << wxT("\n");
//		}
//		curpaths << new_path;
//		m_textCtrlExcludePaths->SetValue(curpaths);
//	}
//}
//
wxArrayString WorkspaceSettingsDlg::GetExcludePaths() const { return wxArrayString(); }

wxArrayString WorkspaceSettingsDlg::GetIncludePaths() const { return m_ccPage->GetIncludePaths(); }

void WorkspaceSettingsDlg::OnButtonOK(wxCommandEvent& event)
{
    m_localWorkspace->SetActiveEnvironmentSet(m_choiceEnvSets->GetStringSelection());
    clCxxWorkspaceST::Get()->SetEnvironmentVariabels(m_textCtrlWspEnvVars->GetValue());
    m_ccPage->Save();
    event.Skip();
}
