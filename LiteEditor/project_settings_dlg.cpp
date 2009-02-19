//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : project_settings_dlg.cpp
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

#include "dirsaver.h"
#include "windowattrmanager.h"
#include "envvar_dlg.h"
#include <wx/dirdlg.h>
#include "project_settings_dlg.h"
#include "globals.h"
#include "macrosdlg.h"
#include "add_option_dialog.h"
#include "free_text_dialog.h"
#include "manager.h"
#include "configuration_manager_dlg.h"
#include "macros.h"
#include "editor_config.h"
#include "build_settings_config.h"
#include "debuggermanager.h"
#include "wx/tokenzr.h"
#include "addoptioncheckdlg.h"

static const wxString CUSTOM_TARGET_BUILD = wxT("Build");
static const wxString CUSTOM_TARGET_CLEAN = wxT("Clean");
static const wxString CUSTOM_TARGET_COMPILE_SINGLE_FILE = wxT("Compile Single File");
static const wxString CUSTOM_TARGET_PREPROCESS_FILE = wxT("Preprocess File");
static const wxString GLOBAL_SETTINGS_LABEL = wxT("Global settings");
static const wxString APPEND_TO_GLOBAL_SETTINGS = wxT("Append to global settings");
static const wxString OVERWRITE_GLOBAL_SETTINGS = wxT("overwrite global settings");
static const wxString PREPEND_GLOBAL_SETTINGS = wxT("prepend global settings");

static const wxEventType wxEVT_CL_PROJECT_SETTINGS_MODIFIED = wxNewEventType();

static
bool PopupAddOptionDlg(wxTextCtrl *ctrl)
{
	AddOptionDlg dlg(NULL, ctrl->GetValue());
	if (dlg.ShowModal() == wxID_OK) {
		ctrl->SetValue(dlg.GetValue());
		return true;
	}
	return false;
}

static
bool PopupAddOptionCheckDlg(wxTextCtrl *ctrl, const wxString& title, const Compiler::CmpCmdLineOptions& options)
{
	AddOptionCheckDlg dlg(NULL, title, options, ctrl->GetValue());
	if (dlg.ShowModal() == wxID_OK) {
		ctrl->SetValue(dlg.GetValue());
		return true;
	}
	return false;
}

static
bool SelectChoiceWithGlobalSettings(wxChoice* c, const wxString& text)
{
	if (text == BuildConfig::APPEND_TO_GLOBAL_SETTINGS) {
		c->Select(c->FindString(APPEND_TO_GLOBAL_SETTINGS));
	} else if (text == BuildConfig::OVERWRITE_GLOBAL_SETTINGS) {
		c->Select(c->FindString(OVERWRITE_GLOBAL_SETTINGS));
	} else if (text == BuildConfig::PREPEND_GLOBAL_SETTINGS) {
		c->Select(c->FindString(PREPEND_GLOBAL_SETTINGS));
	} else {
		c->Select(c->FindString(APPEND_TO_GLOBAL_SETTINGS));
		return false;
	}
	return true;
}


ProjectConfigurationPanel::ProjectConfigurationPanel(wxWindow* parent, const wxString &configName, const wxString &projectName)
		: ProjectConfigurationBasePanel(parent)
		, m_projectName(projectName)
		, m_configName(configName)
		, m_oldConfigurationName(wxEmptyString)
		, m_selecteCustomTaregt(wxNOT_FOUND)
{
	m_listCtrlTargets->InsertColumn(0, wxT("Target"));
	m_listCtrlTargets->InsertColumn(1, wxT("Command"));

	m_choiceCmpUseWithGlobalSettings->AppendString(APPEND_TO_GLOBAL_SETTINGS);
	m_choiceCmpUseWithGlobalSettings->AppendString(OVERWRITE_GLOBAL_SETTINGS);
	m_choiceCmpUseWithGlobalSettings->AppendString(PREPEND_GLOBAL_SETTINGS);
	m_choiceCmpUseWithGlobalSettings->Select(0);

	m_choiceLnkUseWithGlobalSettings->AppendString(APPEND_TO_GLOBAL_SETTINGS);
	m_choiceLnkUseWithGlobalSettings->AppendString(OVERWRITE_GLOBAL_SETTINGS);
	m_choiceLnkUseWithGlobalSettings->AppendString(PREPEND_GLOBAL_SETTINGS);
	m_choiceLnkUseWithGlobalSettings->Select(0);

	m_choiceResUseWithGlobalSettings->AppendString(APPEND_TO_GLOBAL_SETTINGS);
	m_choiceResUseWithGlobalSettings->AppendString(OVERWRITE_GLOBAL_SETTINGS);
	m_choiceResUseWithGlobalSettings->AppendString(PREPEND_GLOBAL_SETTINGS);
	m_choiceResUseWithGlobalSettings->Select(0);

	ConnectEvents();
	m_notebook->SetSelection(0);
	//fill the dialog with values
	InitDialog(m_configName, wxEmptyString);

	//if this is a custom build project disable the
	//compiler linker pages
	if (m_checkEnableCustomBuild->IsChecked()) {
		DisableLinkerPage(true);
		DisableCompilerPage(true);
		m_checkCompilerNeeded->Enable(false);
		m_checkLinkerNeeded->Enable(false);
		m_resourceCmpPage->Enable(false);
		DisableCustomBuildPage(false);
	} else {
		if (m_checkLinkerNeeded->IsChecked()) {
			DisableLinkerPage(true);
		}
		DisableCustomBuildPage(true);
	}

	m_textAddResCmpOptions->Enable(!m_checkResourceNeeded->IsChecked());
	m_textAddResCmpPath->Enable(!m_checkResourceNeeded->IsChecked());
	m_buttonAddResCmpOptions->Enable(!m_checkResourceNeeded->IsChecked());
	m_buttonAddResCmpPath->Enable(!m_checkResourceNeeded->IsChecked());

	m_textCtrl1DbgHost->Enable(m_checkBoxDbgRemote->IsChecked());
	m_textCtrlDbgPort->Enable(m_checkBoxDbgRemote->IsChecked());

	GetSizer()->Fit(this);
	Centre();
}

void ProjectConfigurationPanel::InitDialog(const wxString &configName, const wxString &oldConfig)
{
	wxUnusedVar(configName);
	ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);

	if (!oldConfig.IsEmpty() && oldConfig != GLOBAL_SETTINGS_LABEL) {
		// save old values before replacing them
		SaveValues(oldConfig);
	}

	ClearValues();
	CopyValues(configName);
	DoUpdatePages(m_checkEnableCustomBuild->IsChecked());
}

void ProjectConfigurationPanel::ClearValues()
{
	BuildCommandList preBuildCmds, postBuildCmds;

	m_textOutputFilePicker->SetValue(wxEmptyString);
	m_textCtrlItermediateDir->SetValue(wxEmptyString);
	m_textCommand->SetValue(wxEmptyString);
	m_textCommandArguments->SetValue(wxEmptyString);
	m_textCtrlCommandWD->SetValue(wxEmptyString);
	m_checkCompilerNeeded->SetValue(false);
	m_textCompilerOptions->SetValue(wxEmptyString);
	DisableCompilerPage(false);
	m_textAdditionalSearchPath->SetValue(wxEmptyString);
	m_checkLinkerNeeded->SetValue(false);
	DisableLinkerPage(m_checkLinkerNeeded->IsChecked());
	m_textLinkerOptions->SetValue(wxEmptyString);
	m_textLibraries->SetValue(wxEmptyString);
	m_textLibraryPath->SetValue(wxEmptyString);
	m_checkListPreBuildCommands->Clear();
	m_checkListPostBuildCommands->Clear();
	m_checkEnableCustomBuild->SetValue(false);

	m_textAddResCmpOptions->Clear();
	m_textAddResCmpPath->Clear();
	m_textPreBuildRule->Clear();
	m_textDeps->Clear();
	m_checkResourceNeeded->SetValue(true);
	m_checkBoxPauseWhenExecEnds->SetValue(true);

	m_listCtrlTargets->DeleteAllItems();
	m_textCtrlDebuggerPath->Clear();
	DisableCustomBuildPage(true);
}

void ProjectConfigurationPanel::CopyValues(const wxString &confName)
{
	BuildConfigPtr buildConf;
	ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
	buildConf =	projSettingsPtr->GetBuildConfiguration(confName);
	if (!buildConf) {
		ClearValues();
		return;
	}

	wxArrayString searchArr, libPath, libs;
	BuildCommandList preBuildCmds, postBuildCmds;

	m_textOutputFilePicker->SetValue(buildConf->GetOutputFileName());
	m_textCtrlItermediateDir->SetValue(buildConf->GetIntermediateDirectory());
	m_textCommand->SetValue(buildConf->GetCommand());
	m_textCommandArguments->SetValue(buildConf->GetCommandArguments());
	m_textCtrlCommandWD->SetValue(buildConf->GetWorkingDirectory());
	m_checkCompilerNeeded->SetValue(!buildConf->IsCompilerRequired());
	m_textCompilerOptions->SetValue(buildConf->GetCompileOptions());
	DisableCompilerPage(m_checkCompilerNeeded->IsChecked());
	m_textAdditionalSearchPath->SetValue(buildConf->GetIncludePath());
	m_checkLinkerNeeded->SetValue(!buildConf->IsLinkerRequired());
	DisableLinkerPage(m_checkLinkerNeeded->IsChecked());
	m_textLinkerOptions->SetValue(buildConf->GetLinkOptions());
	m_textLibraries->SetValue(buildConf->GetLibraries());
	m_textLibraryPath->SetValue(buildConf->GetLibPath());
	m_textPreprocessor->SetValue(buildConf->GetPreprocessor());
	buildConf->GetPreBuildCommands(preBuildCmds);
	buildConf->GetPostBuildCommands(postBuildCmds);
	BuildCommandList::iterator iter = preBuildCmds.begin();

	long item = AppendListCtrlRow(m_listCtrlTargets);
	SetColumnText(m_listCtrlTargets, item, 0, CUSTOM_TARGET_BUILD);
	SetColumnText(m_listCtrlTargets, item, 1, buildConf->GetCustomBuildCmd());

	item = AppendListCtrlRow(m_listCtrlTargets);
	SetColumnText(m_listCtrlTargets, item, 0, CUSTOM_TARGET_CLEAN);
	SetColumnText(m_listCtrlTargets, item, 1, buildConf->GetCustomCleanCmd());

	item = AppendListCtrlRow(m_listCtrlTargets);
	SetColumnText(m_listCtrlTargets, item, 0, CUSTOM_TARGET_COMPILE_SINGLE_FILE);
	SetColumnText(m_listCtrlTargets, item, 1, buildConf->GetSingleFileBuildCommand());

	item = AppendListCtrlRow(m_listCtrlTargets);
	SetColumnText(m_listCtrlTargets, item, 0, CUSTOM_TARGET_PREPROCESS_FILE);
	SetColumnText(m_listCtrlTargets, item, 1, buildConf->GetPreprocessFileCommand());

	// Initialize the custom build targets
	std::map<wxString, wxString> targets = buildConf->GetCustomTargets();
	std::map<wxString, wxString>::iterator titer = targets.begin();
	for (; titer != targets.end(); titer++) {

		if (titer->first == CUSTOM_TARGET_BUILD || titer->first == CUSTOM_TARGET_CLEAN || titer->first == CUSTOM_TARGET_COMPILE_SINGLE_FILE || titer->first == CUSTOM_TARGET_PREPROCESS_FILE) {
			continue;
		}

		item = AppendListCtrlRow(m_listCtrlTargets);
		SetColumnText(m_listCtrlTargets, item, 0, titer->first);
		SetColumnText(m_listCtrlTargets, item, 1, titer->second);
	}

	m_listCtrlTargets->SetColumnWidth(0, 150);
	m_listCtrlTargets->SetColumnWidth(1, 300);

	m_checkEnableCustomBuild->SetValue(buildConf->IsCustomBuild());

	m_checkResourceNeeded->SetValue(!buildConf->IsResCompilerRequired());
	m_textAddResCmpOptions->SetValue(buildConf->GetResCompileOptions());
	m_textAddResCmpPath->SetValue(buildConf->GetResCmpIncludePath());
	m_textCtrlCustomBuildWD->SetValue(buildConf->GetCustomBuildWorkingDir());

	m_thirdPartyTool->SetStringSelection(buildConf->GetToolName());
	m_textCtrlMakefileGenerationCmd->SetValue(buildConf->GetMakeGenerationCommand());

	m_textCtrlDbgCmds->SetValue(buildConf->GetDebuggerStartupCmds());
	m_textCtrlDbgPostConnectCmds->SetValue(buildConf->GetDebuggerPostRemoteConnectCmds());
	m_checkBoxDbgRemote->SetValue(buildConf->GetIsDbgRemoteTarget());
	m_textCtrl1DbgHost->SetValue(buildConf->GetDbgHostName());
	m_textCtrlDbgPort->SetValue(buildConf->GetDbgHostPort());

	m_textCtrlDebuggerPath->SetValue(buildConf->GetDebuggerPath());

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

	m_checkListPreBuildCommands->Clear();
	for (; iter != preBuildCmds.end(); iter ++) {
		int index = m_checkListPreBuildCommands->Append(iter->GetCommand());
		m_checkListPreBuildCommands->Check(index, iter->GetEnabled());
	}

	iter = postBuildCmds.begin();
	m_checkListPostBuildCommands->Clear();
	for (; iter != postBuildCmds.end(); iter ++) {
		int index = m_checkListPostBuildCommands->Append(iter->GetCommand());
		m_checkListPostBuildCommands->Check(index, iter->GetEnabled());
	}

	//set the project type
	wxString projType = projSettingsPtr->GetProjectType(confName);
	int sel = m_choiceProjectTypes->FindString(projType);
	if (sel == wxNOT_FOUND) {
		sel = 0;
	}
	m_choiceProjectTypes->SetSelection(sel);

	m_choiceCompilerType->Clear();
	wxString cmpType = buildConf->GetCompilerType();
	BuildSettingsConfigCookie cookie;
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
	while (cmp) {
		m_choiceCompilerType->Append(cmp->GetName());
		cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
	}

	m_choiceDebugger->Clear();
	wxString dbgType = buildConf->GetDebuggerType();
	wxArrayString dbgs = DebuggerMgr::Get().GetAvailableDebuggers();
	if (dbgs.GetCount() > 0) {
		m_choiceDebugger->Append(dbgs);
	}

	if (m_choiceDebugger->GetCount() > 0) {
		int find = m_choiceDebugger->FindString(dbgType);
		if (find != wxNOT_FOUND) {
			m_choiceDebugger->SetSelection(find);
		} else {
			m_choiceDebugger->SetSelection(0);
		}
	}

	int where = m_choiceCompilerType->FindString(cmpType);
	if (where == wxNOT_FOUND) {
		if (m_choiceCompilerType->GetCount() > 0) {
			m_choiceCompilerType->SetSelection(0);
		}
	} else {
		m_choiceCompilerType->SetSelection(where);
	}

	m_checkBoxPauseWhenExecEnds->SetValue(buildConf->GetPauseWhenExecEnds());

	SelectChoiceWithGlobalSettings(m_choiceCmpUseWithGlobalSettings, buildConf->GetBuildCmpWithGlobalSettings());
	SelectChoiceWithGlobalSettings(m_choiceLnkUseWithGlobalSettings, buildConf->GetBuildLnkWithGlobalSettings());
	SelectChoiceWithGlobalSettings(m_choiceResUseWithGlobalSettings, buildConf->GetBuildResWithGlobalSettings());
}

void ProjectConfigurationPanel::SaveValues(const wxString &confName)
{
	BuildConfigPtr buildConf;
	ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
	buildConf =	projSettingsPtr->GetBuildConfiguration(confName);
	if (!buildConf) {
		return;
	}
	wxArrayString searchArr, libPath, libs;
	BuildCommandList preBuildCmds, postBuildCmds;

	// loop over the list and create the targets map
	std::map<wxString, wxString> targets;
	for (int i=0; i<(int)m_listCtrlTargets->GetItemCount(); i++) {
		wxString colText = GetColumnText(m_listCtrlTargets, i, 0);
		if (colText == CUSTOM_TARGET_BUILD || colText == CUSTOM_TARGET_CLEAN || colText == CUSTOM_TARGET_COMPILE_SINGLE_FILE || colText == CUSTOM_TARGET_PREPROCESS_FILE) {
			continue;
		}
		targets[GetColumnText(m_listCtrlTargets, i, 0)] = GetColumnText(m_listCtrlTargets, i, 1);
	}
	buildConf->SetCustomTargets(targets);
	buildConf->SetOutputFileName(m_textOutputFilePicker->GetValue());
	buildConf->SetIntermediateDirectory(m_textCtrlItermediateDir->GetValue());
	buildConf->SetCommand(m_textCommand->GetValue());
	buildConf->SetCommandArguments(m_textCommandArguments->GetValue());
	buildConf->SetWorkingDirectory(m_textCtrlCommandWD->GetValue());
	buildConf->SetCompilerRequired(!m_checkCompilerNeeded->IsChecked());
	buildConf->SetCompileOptions(m_textCompilerOptions->GetValue());
	buildConf->SetIncludePath(m_textAdditionalSearchPath->GetValue());
	buildConf->SetLibPath(m_textLibraryPath->GetValue());
	buildConf->SetLibraries(m_textLibraries->GetValue());
	buildConf->SetLinkerRequired(!m_checkLinkerNeeded->IsChecked());
	buildConf->SetLinkOptions(m_textLinkerOptions->GetValue());
	projSettingsPtr->SetProjectType(m_choiceProjectTypes->GetStringSelection());
	buildConf->SetCompilerType(m_choiceCompilerType->GetStringSelection());
	buildConf->SetDebuggerType(m_choiceDebugger->GetStringSelection());
	buildConf->SetPreprocessor(m_textPreprocessor->GetValue());

	buildConf->SetCustomBuildCmd(GetTargetCommand(CUSTOM_TARGET_BUILD));
	buildConf->SetCustomCleanCmd(GetTargetCommand(CUSTOM_TARGET_CLEAN));
	buildConf->SetSingleFileBuildCommand(GetTargetCommand(CUSTOM_TARGET_COMPILE_SINGLE_FILE));
	buildConf->SetPreprocessFileCommand(GetTargetCommand(CUSTOM_TARGET_PREPROCESS_FILE));

	buildConf->EnableCustomBuild(m_checkEnableCustomBuild->IsChecked());
	buildConf->SetMakeGenerationCommand(m_textCtrlMakefileGenerationCmd->GetValue());
	buildConf->SetToolName(m_thirdPartyTool->GetStringSelection());
	buildConf->SetResCompilerRequired(!m_checkResourceNeeded->IsChecked());
	buildConf->SetResCmpIncludePath(m_textAddResCmpPath->GetValue());
	buildConf->SetResCmpOptions(m_textAddResCmpOptions->GetValue());
	buildConf->SetCustomBuildWorkingDir(m_textCtrlCustomBuildWD->GetValue());
	buildConf->SetPauseWhenExecEnds(m_checkBoxPauseWhenExecEnds->IsChecked());
	buildConf->SetProjectType(m_choiceProjectTypes->GetStringSelection());
	buildConf->SetDebuggerStartupCmds(m_textCtrlDbgCmds->GetValue());
	buildConf->SetDebuggerPostRemoteConnectCmds(m_textCtrlDbgPostConnectCmds->GetValue());
	buildConf->SetIsDbgRemoteTarget(m_checkBoxDbgRemote->IsChecked());
	buildConf->SetDbgHostName(m_textCtrl1DbgHost->GetValue());
	buildConf->SetDbgHostPort(m_textCtrlDbgPort->GetValue());
	buildConf->SetDebuggerPath(m_textCtrlDebuggerPath->GetValue());

	//set the pre-build step
	wxString rules = m_textPreBuildRule->GetValue();
	wxString deps = m_textDeps->GetValue();

	rules = rules.Trim();
	rules = rules.Trim(false);
	deps = deps.Trim();
	deps = deps.Trim(false);

	wxString prebuilstep;
	prebuilstep << deps << wxT("\n");
	prebuilstep << rules;
	prebuilstep << wxT("\n");
	buildConf->SetPreBuildCustom(prebuilstep);

	BuildCommandList cmds;
	cmds.clear();
	for (size_t i=0; i<m_checkListPreBuildCommands->GetCount(); i++) {
		wxString cmdLine = m_checkListPreBuildCommands->GetString((unsigned int)i);
		bool enabled = m_checkListPreBuildCommands->IsChecked((unsigned int)i);
		BuildCommand cmd(cmdLine, enabled);
		cmds.push_back(cmd);
	}
	buildConf->SetPreBuildCommands(cmds);

	cmds.clear();
	for (size_t i=0; i<m_checkListPostBuildCommands->GetCount(); i++) {
		wxString cmdLine = m_checkListPostBuildCommands->GetString((unsigned int)i);
		bool enabled = m_checkListPostBuildCommands->IsChecked((unsigned int)i);
		BuildCommand cmd(cmdLine, enabled);
		cmds.push_back(cmd);
	}
	buildConf->SetPostBuildCommands(cmds);

	wxString useWithGlobalSettings = m_choiceCmpUseWithGlobalSettings->GetStringSelection();
	if (useWithGlobalSettings == APPEND_TO_GLOBAL_SETTINGS) {
		buildConf->SetBuildCmpWithGlobalSettings(BuildConfig::APPEND_TO_GLOBAL_SETTINGS);
	} else if (useWithGlobalSettings == OVERWRITE_GLOBAL_SETTINGS) {
		buildConf->SetBuildCmpWithGlobalSettings(BuildConfig::OVERWRITE_GLOBAL_SETTINGS);
	} else if (useWithGlobalSettings == PREPEND_GLOBAL_SETTINGS) {
		buildConf->SetBuildCmpWithGlobalSettings(BuildConfig::PREPEND_GLOBAL_SETTINGS);
	}
	useWithGlobalSettings = m_choiceLnkUseWithGlobalSettings->GetStringSelection();
	if (useWithGlobalSettings == APPEND_TO_GLOBAL_SETTINGS) {
		buildConf->SetBuildLnkWithGlobalSettings(BuildConfig::APPEND_TO_GLOBAL_SETTINGS);
	} else if (useWithGlobalSettings == OVERWRITE_GLOBAL_SETTINGS) {
		buildConf->SetBuildLnkWithGlobalSettings(BuildConfig::OVERWRITE_GLOBAL_SETTINGS);
	} else if (useWithGlobalSettings == PREPEND_GLOBAL_SETTINGS) {
		buildConf->SetBuildLnkWithGlobalSettings(BuildConfig::PREPEND_GLOBAL_SETTINGS);
	}
	useWithGlobalSettings = m_choiceResUseWithGlobalSettings->GetStringSelection();
	if (useWithGlobalSettings == APPEND_TO_GLOBAL_SETTINGS) {
		buildConf->SetBuildResWithGlobalSettings(BuildConfig::APPEND_TO_GLOBAL_SETTINGS);
	} else if (useWithGlobalSettings == OVERWRITE_GLOBAL_SETTINGS) {
		buildConf->SetBuildResWithGlobalSettings(BuildConfig::OVERWRITE_GLOBAL_SETTINGS);
	} else if (useWithGlobalSettings == PREPEND_GLOBAL_SETTINGS) {
		buildConf->SetBuildResWithGlobalSettings(BuildConfig::PREPEND_GLOBAL_SETTINGS);
	}

	//save settings
	ManagerST::Get()->SetProjectSettings(m_projectName, projSettingsPtr);
}

void ProjectConfigurationPanel::ConnectEvents()
{
	m_checkListPreBuildCommands->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(ProjectConfigurationPanel::OnEditPreBuildCommand), NULL, this);
}

void ProjectConfigurationPanel::OnCustomBuildEnabled(wxCommandEvent &event)
{
	DoUpdatePages(event.IsChecked());
	SetSettingsModified();
	event.Skip();
}

void ProjectConfigurationPanel::DoUpdatePages(bool checked)
{
	DisableCompilerPage(checked);
	DisableLinkerPage(checked);
	DisableGeneralPage(checked);
	DisableCustomBuildPage(!checked);
	DisableCustomMkSteps(checked);

	if (checked) {
		m_checkLinkerNeeded->Enable(false);
		m_checkCompilerNeeded->Enable(false);
		m_resourceCmpPage->Enable(false);
	} else {
		m_checkLinkerNeeded->Enable(true);
		m_checkCompilerNeeded->Enable(true);
		m_resourceCmpPage->Enable(true);
	}
}

void ProjectConfigurationPanel::SetSettingsModified()
{
	wxCommandEvent event(wxEVT_CL_PROJECT_SETTINGS_MODIFIED, GetId());
	event.SetEventObject(this);
	GetParent()->ProcessEvent(event);
}

void ProjectConfigurationPanel::DisableCustomBuildPage(bool disable)
{
	m_listCtrlTargets->Enable(!disable);
	m_textCtrlCustomBuildWD->Enable(!disable);
	m_buttonBrowseCustomBuildWD->Enable(!disable);
	m_thirdPartyTool->Enable(!disable);
	m_buttonNewCustomTarget->Enable(!disable);
	m_buttonEditCustomTarget->Enable(!disable);
	m_buttonDeleteCustomTarget->Enable(!disable);

	if (!disable) {
		if (m_thirdPartyTool->GetStringSelection() == wxT("None")) {
			m_textCtrlMakefileGenerationCmd->Enable(false);
		} else {
			m_textCtrlMakefileGenerationCmd->Enable(true);
		}
	} else {
		m_textCtrlMakefileGenerationCmd->Enable(!disable);
	}
}

void ProjectConfigurationPanel::OnButtonAddPreprocessor(wxCommandEvent &event)
{
	if (PopupAddOptionDlg(m_textPreprocessor)) {
		SetSettingsModified();
	}
	event.Skip();
}

void ProjectSettingsDlg::OnButtonOK(wxCommandEvent &event)
{
	OnButtonApply(event);
	EndModal(wxID_OK);
}

void ProjectSettingsDlg::OnButtonApply(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxString sel = m_choiceConfigurationType->GetStringSelection();
	if (sel != GLOBAL_SETTINGS_LABEL) {
		m_configurationPanel->SaveValues(sel);
	}
	m_globalSettingsPanel->SaveValues();
	m_buttonApply->Enable(false);
}

void ProjectSettingsDlg::OnConfigurationTypeSelected(wxCommandEvent &event)
{
	wxString selection = event.GetString();
	if (selection == GLOBAL_SETTINGS_LABEL) {

		m_sizerSettings->Hide(m_configurationPanel);
		m_sizerSettings->Show(m_globalSettingsPanel);
	} else {

		m_sizerSettings->Hide(m_globalSettingsPanel);
		m_sizerSettings->Show(m_configurationPanel);

		m_configurationPanel->InitDialog(selection, m_oldConfigurationName);
		m_oldConfigurationName = selection;
	}

	m_sizerSettings->Layout();
	GetSizer()->Layout();

	//Layout(); // JFO : doesn't work, I don't kow why...
				// EI  : calling each sizer for layout seems to work. Looks like a bug in WX
	event.Skip();
}

void ProjectConfigurationPanel::DisableCompilerPage(bool disable)
{
	m_textAdditionalSearchPath->Enable(!disable);
	m_buttonAddSearchPath->Enable(!disable);
	m_textCompilerOptions->Enable(!disable);
	m_textPreprocessor->Enable(!disable);
	m_buttonAddPreprocessor->Enable(!disable);
	m_buttonCompilerOptions->Enable(!disable);
}

void ProjectConfigurationPanel::DisableLinkerPage(bool disable)
{
	m_textLibraryPath->Enable(!disable);
	m_textLibraries->Enable(!disable);
	m_textLinkerOptions->Enable(!disable);
	m_buttonLibraries->Enable(!disable);
	m_buttonLibraryPath->Enable(!disable);
	m_buttonLinkerOptions->Enable(!disable);
}

void ProjectConfigurationPanel::OnCheckCompilerNeeded(wxCommandEvent &event)
{
	DisableCompilerPage(event.IsChecked());
	SetSettingsModified();
	event.Skip();
}

void ProjectConfigurationPanel::OnCheckLinkerNeeded(wxCommandEvent &event)
{
	DisableLinkerPage(event.IsChecked());
	SetSettingsModified();
	event.Skip();
}

void ProjectConfigurationPanel::OnAddSearchPath(wxCommandEvent &event)
{
	if (PopupAddOptionDlg(m_textAdditionalSearchPath)) {
		SetSettingsModified();
	}
	event.Skip();
}

void ProjectConfigurationPanel::OnAddLibrary(wxCommandEvent &event)
{
	if (PopupAddOptionDlg(m_textLibraries)) {
		SetSettingsModified();
	}
	event.Skip();
}

void ProjectConfigurationPanel::OnAddLibraryPath(wxCommandEvent &event)
{
	if (PopupAddOptionDlg(m_textLibraryPath)) {
		SetSettingsModified();
	}
	event.Skip();
}

void ProjectConfigurationPanel::OnNewCommand(wxCheckListBox *list)
{
	FreeTextDialog dlg(this);
	if (dlg.ShowModal() == wxID_OK) {
		wxString value = dlg.GetValue();
		TrimString(value);
		//incase several commands were entered, split them
		wxStringTokenizer tkz(value, wxT("\n"), wxTOKEN_STRTOK);
		while (tkz.HasMoreTokens()) {
			wxString command = tkz.NextToken();
			if (command.IsEmpty() == false) {
				list->Append(command);
				list->Check(list->GetCount()-1);
			}
		}
		SetSettingsModified();
	}
}

void ProjectConfigurationPanel::OnEditCommand(wxCheckListBox *list)
{
	wxString selectedString  = list->GetStringSelection();
	int sel = list->GetSelection();
	if (sel == wxNOT_FOUND) {
		return;
	}

	// on GTK it looks like that the state of the item in the list
	// is changed after calling 'SetString'
	bool selectIt = list->IsChecked((unsigned int) sel);

	FreeTextDialog dlg(this, selectedString);
	if (dlg.ShowModal() == wxID_OK) {
		wxString value = dlg.GetValue();
		TrimString(value);
		if (value.IsEmpty() == false) {
			list->SetString((unsigned int)sel, value);
			list->Check((unsigned int)sel, selectIt);
		}
		SetSettingsModified();
	}
}

void ProjectConfigurationPanel::OnUpCommand(wxCheckListBox *list)
{
	wxString selectedString  = list->GetStringSelection();

	int sel = list->GetSelection();
	if (sel == wxNOT_FOUND) {
		return;
	}

	bool isSelected = list->IsChecked(sel);
	sel --;
	if (sel < 0) {
		return;
	}

	// sel contains the new position we want to place the selection string
	list->Delete(sel + 1);
	list->Insert(selectedString, sel);
	list->Select(sel);
	list->Check(sel, isSelected);

	SetSettingsModified();
}

void ProjectConfigurationPanel::OnDownCommand(wxCheckListBox *list)
{
	int sel = list->GetSelection();
	if (sel == wxNOT_FOUND) {
		return;
	}

	sel ++;
	if (sel >= (int)list->GetCount()) {
		return;
	}

	// sel contains the new position we want to place the selection string
	wxString oldStr = list->GetString(sel);
	bool oldStringIsSelected = list->IsChecked(sel);

	list->Delete(sel);
	list->Insert(oldStr, sel - 1);
	list->Select(sel);
	list->Check(sel - 1, oldStringIsSelected);

	SetSettingsModified();
}

void ProjectConfigurationPanel::OnDeleteCommand(wxCheckListBox *list)
{
	int sel = list->GetSelection();
	if (sel == wxNOT_FOUND) {
		return;
	}
	list->Delete(sel);
	if (sel < (int)list->GetCount()) {
		list->Select(sel);
	} else if (sel - 1 < (int)list->GetCount()) {
		list->Select(sel -1);
	}
	SetSettingsModified();
}

void ProjectConfigurationPanel::OnButtonAddCompilerOptions(wxCommandEvent &event)
{
	wxString cmpName = m_choiceCompilerType->GetStringSelection();
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpName);
	if (PopupAddOptionCheckDlg(m_textCompilerOptions, _("Compiler options"), cmp->GetCompilerOptions())) {
		SetSettingsModified();
	}
	event.Skip();
}

void ProjectConfigurationPanel::OnButtonAddLinkerOptions(wxCommandEvent &event)
{
	wxString cmpName = m_choiceCompilerType->GetStringSelection();
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpName);
	if (PopupAddOptionCheckDlg(m_textLinkerOptions, _("Linker options"), cmp->GetLinkerOptions())) {
		SetSettingsModified();
	}
	event.Skip();
}

void ProjectConfigurationPanel::OnResourceCmpAddOption(wxCommandEvent &event)
{
	if (PopupAddOptionDlg(m_textAddResCmpOptions)) {
		SetSettingsModified();
	}
	event.Skip();
}

void ProjectConfigurationPanel::OnResourceCmpAddPath(wxCommandEvent &event)
{
	if (PopupAddOptionDlg(m_textAddResCmpPath)) {
		SetSettingsModified();
	}
	event.Skip();
}

void ProjectConfigurationPanel::OnResourceCmpNeeded(wxCommandEvent &event)
{
	m_textAddResCmpOptions->Enable(!event.IsChecked());
	m_textAddResCmpPath->Enable(!event.IsChecked());
	m_buttonAddResCmpOptions->Enable(!event.IsChecked());
	m_buttonAddResCmpPath->Enable(!event.IsChecked());
	event.Skip();
}

void ProjectConfigurationPanel::DisableGeneralPage(bool disable)
{
	m_choiceProjectTypes->Enable( !disable );
	m_buttonBrowseIM_WD->Enable( !disable );
}

void ProjectConfigurationPanel::DisableCustomMkSteps(bool disable)
{
	m_textDeps->Enable( !disable );
	m_textPreBuildRule->Enable( !disable );
}

void ProjectConfigurationPanel::OnChoiceMakefileTool(wxCommandEvent &e)
{
	if (e.GetString() == wxT("None")) {
		m_textCtrlMakefileGenerationCmd->Enable(false);
	} else {
		m_textCtrlMakefileGenerationCmd->Enable(true);
	}
	SetSettingsModified();
}

void ProjectConfigurationPanel::OnDebuggingRemoteTarget(wxCommandEvent& e)
{
	if (e.IsChecked()) {
		m_textCtrl1DbgHost->Enable(true);
		m_textCtrlDbgPort->Enable(true);
	} else {
		m_textCtrl1DbgHost->Enable(false);
		m_textCtrlDbgPort->Enable(false);
	}
	SetSettingsModified();
}

void ProjectSettingsDlg::OnButtonHelp(wxCommandEvent& e)
{
	wxUnusedVar(e);
	MacrosDlg dlg(this, MacrosDlg::MacrosProject);
	dlg.ShowModal();
}

/**
 * Custom Build event handling
 */
void ProjectConfigurationPanel::OnDeleteTarget(wxCommandEvent& e)
{
	wxUnusedVar(e);
	if (m_selecteCustomTaregt != wxNOT_FOUND) {
		m_listCtrlTargets->DeleteItem(m_selecteCustomTaregt);
		m_selecteCustomTaregt = wxNOT_FOUND;

		SetSettingsModified();
	}
}

void ProjectConfigurationPanel::OnEditTarget(wxCommandEvent& e)
{
	wxUnusedVar(e);
	if (m_selecteCustomTaregt != wxNOT_FOUND) {
		DoEditItem(m_selecteCustomTaregt);
	}
}

void ProjectConfigurationPanel::OnNewTarget(wxCommandEvent& e)
{
	wxUnusedVar(e);

	EnvVarDlg dlg(this);
	dlg.SetTitle(wxT("New target"));
	dlg.SetStaticText1(wxT("Target Name:"));
	dlg.SetStaticText2(wxT("Command:"));
	if (dlg.ShowModal() == wxID_OK) {
		SetSettingsModified();
		if (GetTargetCommand(dlg.GetName()).IsEmpty() == false) {
			wxMessageBox(wxString::Format(wxT("Target '%s' already exist!"), dlg.GetName().c_str()), wxT("CodeLite"), wxICON_WARNING|wxCENTER|wxOK, this);
			return;
		}
		long item = AppendListCtrlRow(m_listCtrlTargets);
		DoUpdateItem(item, dlg.GetName(), dlg.GetValue());
	}
}

void ProjectConfigurationPanel::OnItemActivated(wxListEvent& e)
{
	m_selecteCustomTaregt = e.m_itemIndex;
	DoEditItem(m_selecteCustomTaregt);
}

void ProjectConfigurationPanel::OnItemSelected(wxListEvent& e)
{
	m_selecteCustomTaregt = e.m_itemIndex;
}

void ProjectConfigurationPanel::OnDeleteTargetUI(wxUpdateUIEvent& e)
{
	if (m_selecteCustomTaregt != wxNOT_FOUND) {
		wxString name = GetColumnText(m_listCtrlTargets, m_selecteCustomTaregt, 0);
		e.Enable(name != CUSTOM_TARGET_BUILD && name != CUSTOM_TARGET_CLEAN && name != CUSTOM_TARGET_COMPILE_SINGLE_FILE && m_checkEnableCustomBuild->IsChecked());
	} else {
		e.Enable(false);
	}
}

void ProjectConfigurationPanel::OnEditTargetUI(wxUpdateUIEvent& e)
{
	e.Enable(m_selecteCustomTaregt != wxNOT_FOUND && m_checkEnableCustomBuild->IsChecked());
}

void ProjectConfigurationPanel::DoEditItem(long item)
{
	if (item != wxNOT_FOUND) {
		wxString target = GetColumnText(m_listCtrlTargets, item, 0);
		wxString cmd    = GetColumnText(m_listCtrlTargets, item, 1);
		EnvVarDlg dlg(this);
		dlg.SetTitle(wxT("Edit target"));
		dlg.SetStaticText1(wxT("Target Name:"));
		dlg.SetStaticText2(wxT("Command:"));
		dlg.SetName(target);
		dlg.SetValue(cmd);

		// dont allow user to modify the common targets
		if (target == CUSTOM_TARGET_BUILD || target == CUSTOM_TARGET_CLEAN || target == CUSTOM_TARGET_COMPILE_SINGLE_FILE) {
			dlg.DisableName();
		}

		if (dlg.ShowModal() == wxID_OK) {
			DoUpdateItem(item, dlg.GetName(), dlg.GetValue());
			SetSettingsModified();
		}
	}
}

void ProjectConfigurationPanel::DoUpdateItem(long item, const wxString& target, const wxString& cmd)
{
	SetColumnText(m_listCtrlTargets, item, 0, target);
	SetColumnText(m_listCtrlTargets, item, 1, cmd);

	m_listCtrlTargets->SetColumnWidth(0, 150);
	m_listCtrlTargets->SetColumnWidth(1, 300);
}

wxString ProjectConfigurationPanel::GetTargetCommand(const wxString& target)
{
	for (int i=0; i<(int)m_listCtrlTargets->GetItemCount(); i++) {
		if (GetColumnText(m_listCtrlTargets, i, 0) == target) {
			return GetColumnText(m_listCtrlTargets, i, 1);
		}
	}
	return wxEmptyString;
}

void ProjectConfigurationPanel::OnBrowseCustomBuildWD(wxCommandEvent& e)
{
	DirSaver ds;

	// Since all paths are relative to the project, set the working directory to the
	// current project path
	ProjectPtr p = ManagerST::Get()->GetProject(m_projectName);
	if (p) {
		wxSetWorkingDirectory(p->GetFileName().GetPath());
	}

	wxFileName fn(m_textCtrlCustomBuildWD->GetValue());
	wxString initPath(wxEmptyString);

	if (fn.DirExists()) {
		fn.MakeAbsolute();
		initPath = fn.GetFullPath();
	}

	wxString new_path = wxDirSelector(wxT("Select working directory:"), initPath, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if (new_path.IsEmpty() == false) {
		m_textCtrlCustomBuildWD->SetValue(new_path);
	}
}

void ProjectConfigurationPanel::OnBrowseCommandWD(wxCommandEvent& e)
{
	DirSaver ds;

	// Since all paths are relative to the project, set the working directory to the
	// current project path
	ProjectPtr p = ManagerST::Get()->GetProject(m_projectName);
	if (p) {
		wxSetWorkingDirectory(p->GetFileName().GetPath());
	}

	wxFileName fn(m_textCtrlCommandWD->GetValue());
	wxString initPath(wxEmptyString);
	if (fn.DirExists()) {
		fn.MakeAbsolute();
		initPath = fn.GetFullPath();
	}

	wxString new_path = wxDirSelector(wxT("Select working directory:"), initPath, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if (new_path.IsEmpty() == false) {
		m_textCtrlCommandWD->SetValue(new_path);
	}
}

void ProjectConfigurationPanel::OnBrowseIntermediateDir(wxCommandEvent& e)
{
	DirSaver ds;

	// Since all paths are relative to the project, set the working directory to the
	// current project path
	ProjectPtr p = ManagerST::Get()->GetProject(m_projectName);
	if (p) {
		wxSetWorkingDirectory(p->GetFileName().GetPath());
	}

	wxFileName fn(m_textCtrlItermediateDir->GetValue());
	wxString initPath(wxEmptyString);

	if (fn.DirExists()) {
		fn.MakeAbsolute();
		initPath = fn.GetFullPath();
	}
	wxString new_path = wxDirSelector(wxT("Select working directory:"), initPath, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if (new_path.IsEmpty() == false) {
		m_textCtrlItermediateDir->SetValue(new_path);
	}
}

void ProjectConfigurationPanel::OnSelectDebuggerPath(wxCommandEvent& e)
{
	wxUnusedVar(e);
	wxString path(m_textCtrlDebuggerPath->GetValue());
	if (wxFileName::FileExists(path)) {
		m_textCtrlDebuggerPath->SetValue(wxFileSelector(wxT("Select file:"), path.c_str()));
	} else {
		m_textCtrlDebuggerPath->SetValue(wxFileSelector(wxT("Select file:")));
	}
}

ProjectSettingsDlg::ProjectSettingsDlg( wxWindow* parent, const wxString &configName, const wxString &projectName, const wxString &title )
		: ProjectSettingsBaseDlg( parent, wxID_ANY, title, wxDefaultPosition, wxSize( 782,502 ), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
		, m_projectName(projectName)
		, m_configName(configName)
		, m_oldConfigurationName(wxEmptyString)
		, m_selecteCustomTarget(wxNOT_FOUND)
{
	m_configurationPanel  = new ProjectConfigurationPanel(m_panelSettings, configName, m_projectName);
	m_globalSettingsPanel = new GlobalSettingsPanel(m_panelSettings, m_projectName);
	m_sizerSettings->Add(m_configurationPanel, 1, wxEXPAND | wxALL, 0);
	m_sizerSettings->Add(m_globalSettingsPanel, 1, wxEXPAND | wxALL, 0);
	SetAutoLayout(true);
	ConnectEvents();

	//fill the dialog with values
	UpdateConfigurationTypeChoice(configName);
	m_configurationPanel->InitDialog(m_configName, wxEmptyString);
	m_oldConfigurationName = m_choiceConfigurationType->GetStringSelection();

	m_buttonApply->Enable(false);
	m_sizerSettings->Hide(m_globalSettingsPanel);

	GetSizer()->Fit(this);
	Centre();

	WindowAttrManager::Load(this, wxT("ProjectSettingsDlg"), NULL);
}

ProjectSettingsDlg::~ProjectSettingsDlg()
{
	WindowAttrManager::Save(this, wxT("ProjectSettingsDlg"), NULL);
}

void ProjectSettingsDlg::UpdateConfigurationTypeChoice(const wxString &itemToSelect)
{
	ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
	ProjectSettingsCookie cookie;
	m_choiceConfigurationType->Clear();
	m_choiceConfigurationType->Append(GLOBAL_SETTINGS_LABEL);
	BuildConfigPtr conf = projSettingsPtr->GetFirstBuildConfiguration(cookie);
	while (conf) {
		m_choiceConfigurationType->Append(conf->GetName());
		conf = projSettingsPtr->GetNextBuildConfiguration(cookie);
	}

	if (itemToSelect.IsEmpty() || m_choiceConfigurationType->FindString(itemToSelect) == wxNOT_FOUND) {
		if (m_choiceConfigurationType->GetCount() > 0)
			m_choiceConfigurationType->SetSelection(0);
	} else {
		m_choiceConfigurationType->SetStringSelection(itemToSelect);
	}
}

void ProjectSettingsDlg::OnButtonConfigurationManager(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ConfigurationManagerDlg dlg(this);
	dlg.ShowModal();

	//update the configuration type choice control
	UpdateConfigurationTypeChoice(m_choiceConfigurationType->GetStringSelection());
	//Load values according to the new selection
	m_configurationPanel->CopyValues(m_choiceConfigurationType->GetStringSelection());
	event.Skip();
}

void ProjectSettingsDlg::ConnectEvents()
{
	Connect(wxEVT_CL_PROJECT_SETTINGS_MODIFIED, wxCommandEventHandler(ProjectSettingsDlg::OnSettingsModified));
}

void ProjectSettingsDlg::OnSettingsModified(wxCommandEvent &event)
{
	m_buttonApply->Enable(true);
	event.Skip();
}


GlobalSettingsPanel::GlobalSettingsPanel(wxWindow* parent, const wxString &projectName)
		: GlobalSettingsBasePanel(parent)
		, m_projectName(projectName)
{
	CopyValues();

	GetSizer()->Fit(this);
	Centre();
}

void GlobalSettingsPanel::SetSettingsModified()
{
	wxCommandEvent event(wxEVT_CL_PROJECT_SETTINGS_MODIFIED, GetId());
	event.SetEventObject(this);
	GetParent()->ProcessEvent(event);
}

void GlobalSettingsPanel::ClearValues()
{
	m_textCompilerOptions->SetValue(wxEmptyString);
	m_textPreprocessor->SetValue(wxEmptyString);
	m_textAdditionalSearchPath->SetValue(wxEmptyString);

	m_textLinkerOptions->SetValue(wxEmptyString);
	m_textLibraries->SetValue(wxEmptyString);
	m_textLibraryPath->SetValue(wxEmptyString);

	m_textAddResCmpOptions->SetValue(wxEmptyString);
	m_textAddResCmpPath->SetValue(wxEmptyString);
}

void GlobalSettingsPanel::CopyValues()
{
	ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
	BuildConfigCommonPtr globalSettings = projSettingsPtr->GetGlobalSettings();
	if (!globalSettings) {
		ClearValues();
		return;
	}

	m_textCompilerOptions->SetValue(globalSettings->GetCompileOptions());
	m_textPreprocessor->SetValue(globalSettings->GetPreprocessor());
	m_textAdditionalSearchPath->SetValue(globalSettings->GetIncludePath());

	m_textLinkerOptions->SetValue(globalSettings->GetLinkOptions());
	m_textLibraries->SetValue(globalSettings->GetLibraries());
	m_textLibraryPath->SetValue(globalSettings->GetLibPath());

	m_textAddResCmpOptions->SetValue(globalSettings->GetResCompileOptions());
	m_textAddResCmpPath->SetValue(globalSettings->GetResCmpIncludePath());
}

void GlobalSettingsPanel::SaveValues()
{
	ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
	BuildConfigCommonPtr globalSettings = projSettingsPtr->GetGlobalSettings();
	if (!globalSettings) {
		return;
	}

	globalSettings->SetCompileOptions(m_textCompilerOptions->GetValue());
	globalSettings->SetIncludePath(m_textAdditionalSearchPath->GetValue());
	globalSettings->SetPreprocessor(m_textPreprocessor->GetValue());

	globalSettings->SetLibPath(m_textLibraryPath->GetValue());
	globalSettings->SetLibraries(m_textLibraries->GetValue());
	globalSettings->SetLinkOptions(m_textLinkerOptions->GetValue());

	globalSettings->SetResCmpIncludePath(m_textAddResCmpPath->GetValue());
	globalSettings->SetResCmpOptions(m_textAddResCmpOptions->GetValue());

	//save settings
	ManagerST::Get()->SetProjectGlobalSettings(m_projectName, globalSettings);
}

void GlobalSettingsPanel::OnButtonAddCompilerOptions(wxCommandEvent &event)
{
	// This is not perfect : I just take the first compiler to find options
	BuildSettingsConfigCookie cookie;
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
	if (PopupAddOptionCheckDlg(m_textCompilerOptions, _("Compiler options"), cmp->GetCompilerOptions())) {
		SetSettingsModified();
	}
	event.Skip();
}

void GlobalSettingsPanel::OnAddSearchPath(wxCommandEvent &event)
{
	if (PopupAddOptionDlg(m_textAdditionalSearchPath)) {
		SetSettingsModified();
	}
	event.Skip();
}

void GlobalSettingsPanel::OnButtonAddPreprocessor(wxCommandEvent &event)
{
	if (PopupAddOptionDlg(m_textPreprocessor)) {
		SetSettingsModified();
	}
	event.Skip();
}

void GlobalSettingsPanel::OnAddLibrary(wxCommandEvent &event)
{
	if (PopupAddOptionDlg(m_textLibraries)) {
		SetSettingsModified();
	}
	event.Skip();
}

void GlobalSettingsPanel::OnAddLibraryPath(wxCommandEvent &event)
{
	if (PopupAddOptionDlg(m_textLibraryPath)) {
		SetSettingsModified();
	}
	event.Skip();
}

void GlobalSettingsPanel::OnButtonAddLinkerOptions(wxCommandEvent &event)
{
	// This is not perfect : I just take the first compiler to find options
	BuildSettingsConfigCookie cookie;
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
	if (PopupAddOptionCheckDlg(m_textLinkerOptions, _("Linker options"), cmp->GetLinkerOptions())) {
		SetSettingsModified();
	}
	event.Skip();
}

void GlobalSettingsPanel::OnResourceCmpAddOption(wxCommandEvent &event)
{
	if (PopupAddOptionDlg(m_textAddResCmpOptions)) {
		SetSettingsModified();
	}
	event.Skip();
}

void GlobalSettingsPanel::OnResourceCmpAddPath(wxCommandEvent &event)
{
	if (PopupAddOptionDlg(m_textAddResCmpPath)) {
		SetSettingsModified();
	}
	event.Skip();
}
