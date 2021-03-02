//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ps_compiler_page.cpp
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

#include "CopyCompilerSettingsDlg.h"
#include "addoptioncheckdlg.h"
#include "build_settings_config.h"
#include "compiler.h"
#include "globals.h"
#include "manager.h"
#include "project.h"
#include "project_settings_dlg.h"
#include "ps_compiler_page.h"
#include "ps_general_page.h"
#include <wx/filedlg.h>

PSCompilerPage::PSCompilerPage(wxWindow* parent, const wxString& projectName, ProjectSettingsDlg* dlg,
                               PSGeneralPage* gp)
    : PSCompilerPageBase(parent)
    , m_dlg(dlg)
    , m_projectName(projectName)
    , m_gp(gp)
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgr->GetGrid());
}

void PSCompilerPage::Load(BuildConfigPtr buildConf)
{
    m_checkCompilerNeeded->SetValue(!buildConf->IsCompilerRequired());
    m_pgPropCppOpts->SetValueFromString(buildConf->GetCompileOptions());
    m_pgPropIncludePaths->SetValueFromString(buildConf->GetIncludePath());
    m_pgPropPreProcessors->SetValueFromString(buildConf->GetPreprocessor());
    m_pgPropPreCmpHeaderFile->SetValue(buildConf->GetPrecompiledHeader());
    m_pgPropCOpts->SetValue(buildConf->GetCCompileOptions());
    SelectChoiceWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings, buildConf->GetBuildCmpWithGlobalSettings());
    m_pgPropIncludePCH->SetValue(buildConf->GetPchInCommandLine());
    m_pgPropPCHCompileLine->SetValue(buildConf->GetPchCompileFlags());
    m_pgPropAssembler->SetValue(buildConf->GetAssmeblerOptions());
    m_pgPropPCHPolicy->SetValue((int)buildConf->GetPCHFlagsPolicy());
}

void PSCompilerPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetCompilerRequired(!m_checkCompilerNeeded->IsChecked());
    buildConf->SetCompileOptions(m_pgPropCppOpts->GetValueAsString());
    buildConf->SetIncludePath(m_pgPropIncludePaths->GetValueAsString());
    buildConf->SetPreprocessor(m_pgPropPreProcessors->GetValueAsString());
    buildConf->SetPrecompiledHeader(m_pgPropPreCmpHeaderFile->GetValueAsString());
    buildConf->SetCCompileOptions(m_pgPropCOpts->GetValueAsString());
    buildConf->SetPchInCommandLine(m_pgPropIncludePCH->GetValue().GetBool());
    buildConf->SetPchCompileFlags(m_pgPropPCHCompileLine->GetValueAsString());
    buildConf->SetAssmeblerOptions(m_pgPropAssembler->GetValueAsString());
    buildConf->SetBuildCmpWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings->GetValueAsString());
    buildConf->SetPCHFlagsPolicy((BuildConfig::ePCHPolicy)m_pgPropPCHPolicy->GetValue().GetInteger());
}

void PSCompilerPage::Clear()
{
    wxPropertyGridIterator iter = m_pgMgr->GetGrid()->GetIterator();
    for(; !iter.AtEnd(); ++iter) {
        if(iter.GetProperty() && !iter.GetProperty()->IsCategory()) {
            iter.GetProperty()->SetValueToUnspecified();
        }
    }
    m_checkCompilerNeeded->SetValue(false);
}

void PSCompilerPage::OnProjectEnabledUI(wxUpdateUIEvent& event) { event.Enable(m_dlg->IsProjectEnabled()); }

void PSCompilerPage::OnPropertyChanged(wxPropertyGridEvent& event) { m_dlg->SetIsDirty(true); }

void PSCompilerPage::OnUpdateUI(wxUpdateUIEvent& event) { event.Enable(true); }

void PSCompilerPage::OnCustomEditorClicked(wxCommandEvent& event)
{
    wxPGProperty* prop = m_pgMgr->GetSelectedProperty();
    CHECK_PTR_RET(prop);
    m_dlg->SetIsDirty(true);

    if(prop == m_pgPropPreProcessors || prop == m_pgPropIncludePaths || prop == m_pgPropAssembler) {
        wxString value = prop->GetValueAsString();
        if(PopupAddOptionDlg(value)) {
            prop->SetValueFromString(value);
        }

    } else if(prop == m_pgPropCppOpts || prop == m_pgPropCOpts) {
        wxString value = prop->GetValueAsString();
        wxString cmpName = m_gp->GetCompiler();
        CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpName);
        if(PopupAddOptionCheckDlg(value, _("Compiler options"), cmp->GetCompilerOptions())) {
            prop->SetValueFromString(value);
        }
    } else if(prop == m_pgPropPreCmpHeaderFile) {
        wxFileName curvalue = prop->GetValueAsString();
        wxString program = ::wxFileSelector(_("Choose a file"), curvalue.GetPath());
        if(!program.IsEmpty()) {
            program.Replace("\\", "/");
            prop->SetValue(program);
        }
    }
}

void PSCompilerPage::OnCompilerNeeded(wxCommandEvent& event) { m_dlg->SetIsDirty(true); }
void PSCompilerPage::OnCopyCompilerSettings(wxCommandEvent& event)
{
    CopyCompilerSettingsDlg dlg(this);
    if(dlg.ShowModal() != wxID_OK) {
        return;
    }

    wxString project, config;
    dlg.GetCopyFrom(project, config);

    auto p = clCxxWorkspaceST::Get()->GetProject(project);
    CHECK_PTR_RET(p);

    auto buildConf = p->GetBuildConfiguration(config);
    CHECK_PTR_RET(buildConf);

    m_checkCompilerNeeded->SetValue(!buildConf->IsCompilerRequired());
    m_pgPropCppOpts->SetValueFromString(buildConf->GetCompileOptions());
    m_pgPropIncludePaths->SetValueFromString(buildConf->GetIncludePath());
    m_pgPropPreProcessors->SetValueFromString(buildConf->GetPreprocessor());
    m_pgPropPreCmpHeaderFile->SetValue(buildConf->GetPrecompiledHeader());
    m_pgPropCOpts->SetValue(buildConf->GetCCompileOptions());
    SelectChoiceWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings, buildConf->GetBuildCmpWithGlobalSettings());
    m_pgPropIncludePCH->SetValue(buildConf->GetPchInCommandLine());
    m_pgPropPCHCompileLine->SetValue(buildConf->GetPchCompileFlags());
    m_pgPropAssembler->SetValue(buildConf->GetAssmeblerOptions());
    m_pgPropPCHPolicy->SetValue((int)buildConf->GetPCHFlagsPolicy());
    m_dlg->SetIsDirty(true);
}
