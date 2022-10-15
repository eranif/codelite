//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ps_general_page.cpp
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

#include "ps_general_page.h"

#include "build_settings_config.h"
#include "builder.h"
#include "buildmanager.h"
#include "dirsaver.h"
#include "globals.h"
#include "manager.h"
#include "project.h"
#include "project_settings_dlg.h"

#include <algorithm>
#include <vector>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/regex.h>

PSGeneralPage::PSGeneralPage(wxWindow* parent, const wxString& projectName, const wxString& conf,
                             ProjectSettingsDlg* dlg)
    : PSGeneralPageBase(parent)
    , m_dlg(dlg)
    , m_projectName(projectName)
    , m_configName(conf)
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgr136->GetGrid());
}

void PSGeneralPage::OnProjectCustumBuildUI(wxUpdateUIEvent& event)
{
    // do not disable the General page even when in custom build
    event.Enable(m_checkBoxEnabled->IsChecked());
}

void PSGeneralPage::Load(BuildConfigPtr buildConf)
{
    Clear();
    m_configName = buildConf->GetName();
    m_checkBoxEnabled->SetValue(buildConf->IsProjectEnabled());
    m_pgPropArgs->SetValue(buildConf->GetCommandArguments());
    m_pgPropDebugArgs->SetValueFromString(buildConf->GetDebugArgs());
    m_pgPropIntermediateFolder->SetValueFromString(buildConf->GetIntermediateDirectory());
    m_pgPropGUIApp->SetValue(buildConf->IsGUIProgram());
    m_pgPropOutputFile->SetValueFromString(buildConf->GetOutputFileName());
    m_pgPropPause->SetValue(buildConf->GetPauseWhenExecEnds());
    m_pgPropProgram->SetValueFromString(buildConf->GetCommand());
    m_pgPropWorkingDirectory->SetValue(buildConf->GetWorkingDirectory());
    // Project type
    wxPGChoices choices;
    choices.Add(PROJECT_TYPE_STATIC_LIBRARY);
    choices.Add(PROJECT_TYPE_DYNAMIC_LIBRARY);
    choices.Add(PROJECT_TYPE_EXECUTABLE);
    m_pgPropProjectType->SetChoices(choices);

    int sel = choices.Index(buildConf->GetProjectType());
    if(sel != wxNOT_FOUND) {
        m_pgPropProjectType->SetChoiceSelection(sel);
    }

    // Builders
    wxPGChoices builders;
    std::list<wxString> buildersList;
    BuildManagerST::Get()->GetBuilders(buildersList);
    std::for_each(buildersList.begin(), buildersList.end(), [&](const wxString& builder) { builders.Add(builder); });
    m_pgPropMakeGenerator->SetChoices(builders);
    m_pgPropMakeGeneratorArgs->SetValue(buildConf->GetBuildSystemArguments());
    m_pgPropMakeGenerator->SetExpanded(false);

    wxString builderName = buildConf->GetBuildSystem();
    sel = builders.Index(builderName);
    if(sel != wxNOT_FOUND) {
        m_pgPropMakeGenerator->SetChoiceSelection(sel);
    }

    // Compilers
    choices.Clear();
    wxString cmpType = buildConf->GetCompilerType();
    BuildSettingsConfigCookie cookie;
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);

    std::vector<wxString> compiler_names;
    compiler_names.reserve(10);

    // build the compiler list + sort it
    while(cmp) {
        compiler_names.push_back(cmp->GetName());
        cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
    }

    std::sort(compiler_names.begin(), compiler_names.end(),
              [](const wxString& a, const wxString& b) { return a.CmpNoCase(b) < 0; });

    for(const wxString& name: compiler_names) {
        choices.Add(name);
    }

    m_pgPropCompiler->SetChoices(choices);
    sel = choices.Index(buildConf->GetCompiler()->GetName());
    if(sel != wxNOT_FOUND) {
        m_pgPropCompiler->SetChoiceSelection(sel);
    }

    // Debuggers
    choices.Clear();
    wxString dbgType = buildConf->GetDebuggerType();
    wxArrayString dbgs = DebuggerMgr::Get().GetAvailableDebuggers();
    choices.Add(dbgs);
    m_pgPropDebugger->SetChoices(choices);
    sel = choices.Index(buildConf->GetDebuggerType());
    if(sel != wxNOT_FOUND) {
        m_pgPropDebugger->SetChoiceSelection(sel);
    }
    m_pgPropUseSeparateDebuggerArgs->SetValue(buildConf->GetUseSeparateDebugArgs());
    m_dlg->SetIsProjectEnabled(buildConf->IsProjectEnabled());
}

void PSGeneralPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetOutputFileName(GetPropertyAsString(m_pgPropOutputFile));
    buildConf->SetIntermediateDirectory(GetPropertyAsString(m_pgPropIntermediateFolder));
    buildConf->SetCommand(GetPropertyAsString(m_pgPropProgram));
    buildConf->SetCommandArguments(GetPropertyAsString(m_pgPropArgs));
    buildConf->SetWorkingDirectory(GetPropertyAsString(m_pgPropWorkingDirectory));

    // Get the project type selection, unlocalised
    projSettingsPtr->SetProjectType(GetPropertyAsString(m_pgPropProjectType));
    buildConf->SetBuildSystemArguments(GetPropertyAsString(m_pgPropMakeGeneratorArgs));
    buildConf->SetBuildSystem(GetPropertyAsString(m_pgPropMakeGenerator));
    buildConf->SetDebuggerType(GetPropertyAsString(m_pgPropDebugger));
    buildConf->SetPauseWhenExecEnds(GetPropertyAsBool(m_pgPropPause));
    buildConf->SetProjectType(GetPropertyAsString(m_pgPropProjectType));
    buildConf->SetDebugArgs(GetPropertyAsString(m_pgPropDebugArgs));
    buildConf->SetIsGUIProgram(GetPropertyAsBool(m_pgPropGUIApp));
    buildConf->SetIsProjectEnabled(m_checkBoxEnabled->IsChecked());
    buildConf->SetUseSeparateDebugArgs(GetPropertyAsBool(m_pgPropUseSeparateDebuggerArgs));

    // Do not set an empty compiler, this will be prompted later by the CompilersModifiedDlg
    wxString compilerType = GetPropertyAsString(m_pgPropCompiler);
    if(!compilerType.IsEmpty()) {
        buildConf->SetCompilerType(compilerType);
    }
}

void PSGeneralPage::Clear()
{
    wxPropertyGridIterator iter = m_pgMgr136->GetGrid()->GetIterator();
    for(; !iter.AtEnd(); ++iter) {
        if(iter.GetProperty() && !iter.GetProperty()->IsCategory()) {
            iter.GetProperty()->SetValueToUnspecified();
        }
    }
    m_checkBoxEnabled->SetValue(true);
}

void PSGeneralPage::OnValueChanging(wxPropertyGridEvent& event)
{
    event.Skip();
    if(event.GetProperty() == m_pgPropProjectType) {
        BuilderPtr builder = BuildManagerST::Get()->GetBuilder(m_pgPropMakeGenerator->GetValueAsString());
        if(!builder) {
            return;
        }
        // The project type has changed, adjust the output file extension
        wxVariant variant = event.GetPropertyValue();
        wxString oldProjectType = m_pgPropProjectType->GetValueAsString(),
                 newProjectType = m_pgPropProjectType->ValueToString(variant);
        static const wxRegEx reFileExt("^(.*)(\\.[a-zA-Z]+)$"); // libfoobar.a -> (libfoobar) (.a)
        wxString outputFile = m_pgPropOutputFile->GetValueAsString();
        if(outputFile.IsEmpty()) {
            // The output file was blank, simply set a default value
            outputFile = builder->GetOptimalBuildConfig(newProjectType).outputFile;
        } else if(reFileExt.Matches(outputFile) &&
                  reFileExt.GetMatch(outputFile, 2).IsSameAs(builder->GetOutputFileSuffix(oldProjectType), false)) {
            // Update the old file extension
            outputFile = reFileExt.GetMatch(outputFile, 1) + builder->GetOutputFileSuffix(newProjectType);
        } else {
            // Append a file extension because no known extension was set
            outputFile << builder->GetOutputFileSuffix(newProjectType);
        }
        m_pgPropOutputFile->SetValue(outputFile);
    }
}

void PSGeneralPage::OnValueChanged(wxPropertyGridEvent& event)
{
    m_dlg->SetIsDirty(true);

    if(event.GetProperty() == m_pgPropMakeGenerator) {
        BuilderPtr builder = BuildManagerST::Get()->GetBuilder(m_pgPropMakeGenerator->GetValueAsString());
        if(!builder) {
            return;
        }
        wxString dlgmsg;
        dlgmsg = _("Adjust settings to fit this generator?");
        if(::wxMessageBox(dlgmsg, "CodeLite", wxICON_QUESTION | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT,
                          ::wxGetTopLevelParent(this)) != wxYES) {
            return;
        }
        // Update the settings
        Builder::OptimalBuildConfig optimalConf =
            builder->GetOptimalBuildConfig(m_pgPropProjectType->GetValueAsString());
        m_pgPropIntermediateFolder->SetValue(optimalConf.intermediateDirectory);
        m_pgPropOutputFile->SetValue(optimalConf.outputFile);
        m_pgPropProgram->SetValue(optimalConf.command);
        m_pgPropWorkingDirectory->SetValue(optimalConf.workingDirectory);
    }
}

bool PSGeneralPage::GetPropertyAsBool(wxPGProperty* prop) const
{
    wxVariant v = prop->GetValue();
    bool b = v.GetBool();
    return b;
}

wxString PSGeneralPage::GetPropertyAsString(wxPGProperty* prop) const
{
    wxString s = prop->GetValueAsString();
    return s;
}

void PSGeneralPage::OnProjectEnabled(wxCommandEvent& event)
{
    m_dlg->SetIsProjectEnabled(event.IsChecked());
    m_dlg->SetIsDirty(true);
    m_dlg->ShowHideDisabledMessage();
}

void PSGeneralPage::OnCustomEditorClicked(wxCommandEvent& event)
{
    wxPGProperty* prop = m_pgMgr136->GetSelectedProperty();
    CHECK_PTR_RET(prop);
    m_dlg->SetIsDirty(true);

    if(prop == m_pgPropProgram) {
        wxFileName curvalue = prop->GetValueAsString();
        wxString program = ::wxFileSelector(_("Choose a file"), curvalue.GetPath());
        if(!program.IsEmpty()) {
            program.Replace("\\", "/");
            prop->SetValue(program);
        }

    } else if(prop == m_pgPropWorkingDirectory) {
        wxString curpath = prop->GetValueAsString();
        wxFileName fp(curpath, "");
        wxString newPath = ::wxDirSelector(_("Choose a directory"), fp.GetPath());
        if(!newPath.IsEmpty()) {
            newPath.Replace("\\", "/");
            prop->SetValue(newPath);
        }
    }
}
