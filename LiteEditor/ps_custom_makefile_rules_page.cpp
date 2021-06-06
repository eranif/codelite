//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ps_custom_makefile_rules_page.cpp
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

#include "ps_custom_makefile_rules_page.h"

PSCustomMakefileRulesPage::PSCustomMakefileRulesPage( wxWindow* parent, ProjectSettingsDlg *dlg )
    : PSCustomMakefileBasePage( parent )
    , m_dlg(dlg)
{
    m_textPreBuildRule->SetUseTabs(true); // since this is a Makefile content, we only allow tabs here
    m_textPreBuildRule->SetEOLMode(wxSTC_EOL_LF); // Only "\n"
}

void PSCustomMakefileRulesPage::OnProjectCustumBuildUI( wxUpdateUIEvent& event )
{
    event.Enable( !m_dlg->IsCustomBuildEnabled() );
}

void PSCustomMakefileRulesPage::OnCmdEvtVModified( wxCommandEvent& event )
{
    wxUnusedVar(event);
    m_dlg->SetIsDirty(true);
}

void PSCustomMakefileRulesPage::OnStcEvtVModified( wxStyledTextEvent& event )
{
    event.Skip();
    m_dlg->SetIsDirty(true);
}

void PSCustomMakefileRulesPage::Load(BuildConfigPtr buildConf)
{
    //set the custom pre-prebuild step
    wxString customPreBuild = buildConf->GetPreBuildCustom();

    //extract the dependencies
    wxString deps, rules;
    deps = customPreBuild.BeforeFirst(wxT('\n'));
    rules = customPreBuild.AfterFirst(wxT('\n'));

    rules = rules.Trim();
    rules = rules.Trim(false);

    deps = deps.Trim();
    deps = deps.Trim(false);

    m_textDeps->SetValue(deps);
    m_textPreBuildRule->SetValue(rules);
}

void PSCustomMakefileRulesPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    //set the pre-build step
    wxString rules = m_textPreBuildRule->GetText();
    wxString deps = m_textDeps->GetValue();

    rules.Trim().Trim(false);
    deps.Trim().Trim(false);

    wxString prebuilstep;
    prebuilstep << deps << "\n";
    prebuilstep << rules;
    prebuilstep << "\n";

    // Set the content only if there is real content to add
    wxString tmpPreBuildStep(prebuilstep);
    tmpPreBuildStep.Trim().Trim(false);
    if(tmpPreBuildStep.IsEmpty() == false) {
        buildConf->SetPreBuildCustom(prebuilstep);
    } else {
        buildConf->SetPreBuildCustom(wxT(""));
    }
}

void PSCustomMakefileRulesPage::Clear()
{
    m_textPreBuildRule->Clear();
    m_textDeps->Clear();
}
void PSCustomMakefileRulesPage::OnProjectEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable( m_dlg->IsProjectEnabled() );
}
