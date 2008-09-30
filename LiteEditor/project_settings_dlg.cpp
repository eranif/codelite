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

static const wxString CUSTOM_TARGET_BUILD = wxT("Build");
static const wxString CUSTOM_TARGET_CLEAN = wxT("Clean");
static const wxString CUSTOM_TARGET_COMPILE_SINGLE_FILE = wxT("Compile Single File");

ProjectSettingsDlg::ProjectSettingsDlg( wxWindow* parent, const wxString &configName, const wxString &projectName, const wxString &title )
		: ProjectSettingsBaseDlg( parent, wxID_ANY, title, wxDefaultPosition, wxSize( 782,502 ), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
		, m_projectName(projectName)
		, m_configName(configName)
		, m_oldConfigurationName(wxEmptyString)
		, m_selecteCustomTaregt(wxNOT_FOUND)
{
	m_listCtrlTargets->InsertColumn(0, wxT("Target"));
	m_listCtrlTargets->InsertColumn(1, wxT("Command"));

	ConnectEvents();
	m_notebook3->SetSelection(0);
	//fill the dialog with values
	InitDialog(m_configName, wxEmptyString);
	m_oldConfigurationName = m_choiceConfigurationType->GetStringSelection();

	//if this is a custom build project disable the
	//compiler linker pages
	if (m_checkEnableCustomBuild->IsChecked()) {
		DisableLinkerPage(true);
		DisableCompilerPage(true);
		m_postBuildPage->Enable(false);
		m_preBuildPage->Enable(false);
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
	m_buttonApply->Enable(false);

	m_textCtrl1DbgHost->Enable(m_checkBoxDbgRemote->IsChecked());
	m_textCtrlDbgPort->Enable(m_checkBoxDbgRemote->IsChecked());

	//SetSizeHints(780, -1);
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

void ProjectSettingsDlg::InitDialog(const wxString &configName, const wxString &oldConfig)
{
	wxUnusedVar(configName);
	ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);

	UpdateConfigurationTypeChoice(configName);
	if (oldConfig.IsEmpty() == false) {
		// save old values before replacing them
		SaveValues(oldConfig);
	}
	CopyValues(m_choiceConfigurationType->GetStringSelection());
	DoUpdatePages(m_checkEnableCustomBuild->IsChecked());
}

void ProjectSettingsDlg::ClearValues()
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
	long item = AppendListCtrlRow(m_listCtrlTargets);
	SetColumnText(m_listCtrlTargets, item, 0, CUSTOM_TARGET_BUILD);

	item = AppendListCtrlRow(m_listCtrlTargets);
	SetColumnText(m_listCtrlTargets, item, 0, CUSTOM_TARGET_CLEAN);

	item = AppendListCtrlRow(m_listCtrlTargets);
	SetColumnText(m_listCtrlTargets, item, 0, CUSTOM_TARGET_COMPILE_SINGLE_FILE);

	DisableCustomBuildPage(true);
}

void ProjectSettingsDlg::CopyValues(const wxString &confName)
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

	// Initialize the custom build targets
	std::map<wxString, wxString> targets = buildConf->GetCustomTargets();
	std::map<wxString, wxString>::iterator titer = targets.begin();
	for (; titer != targets.end(); titer++) {
		
		if(titer->first == CUSTOM_TARGET_BUILD || titer->first == CUSTOM_TARGET_CLEAN || titer->first == CUSTOM_TARGET_COMPILE_SINGLE_FILE){
			continue;
		}
		
		item = AppendListCtrlRow(m_listCtrlTargets);
		SetColumnText(m_listCtrlTargets, item, 0, titer->first);
		SetColumnText(m_listCtrlTargets, item, 1, titer->second);
	}

	if ( m_listCtrlTargets->GetItemCount() > 3 ) {
		m_listCtrlTargets->SetColumnWidth(0, wxLIST_AUTOSIZE);
		m_listCtrlTargets->SetColumnWidth(1, wxLIST_AUTOSIZE);
	}

	m_checkEnableCustomBuild->SetValue(buildConf->IsCustomBuild());

	m_checkResourceNeeded->SetValue(!buildConf->IsResCompilerRequired());
	m_textAddResCmpOptions->SetValue(buildConf->GetResCompileOptions());
	m_textAddResCmpPath->SetValue(buildConf->GetResCmpIncludePath());
	m_textCtrlCustomBuildWD->SetValue(buildConf->GetCustomBuildWorkingDir());

	m_thirdPartyTool->SetStringSelection(buildConf->GetToolName());
	m_textCtrlMakefileGenerationCmd->SetValue(buildConf->GetMakeGenerationCommand());

	m_textCtrlDbgCmds->SetValue(buildConf->GetDebuggerStartupCmds());
	m_checkBoxDbgRemote->SetValue(buildConf->GetIsDbgRemoteTarget());
	m_textCtrl1DbgHost->SetValue(buildConf->GetDbgHostName());
	m_textCtrlDbgPort->SetValue(buildConf->GetDbgHostPort());

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
}

void ProjectSettingsDlg::SaveValues(const wxString &confName)
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
		if(GetColumnText(m_listCtrlTargets, i, 0) == CUSTOM_TARGET_BUILD || GetColumnText(m_listCtrlTargets, i, 0) == CUSTOM_TARGET_CLEAN || GetColumnText(m_listCtrlTargets, i, 0) == CUSTOM_TARGET_COMPILE_SINGLE_FILE){
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
	buildConf->SetIsDbgRemoteTarget(m_checkBoxDbgRemote->IsChecked());
	buildConf->SetDbgHostName(m_textCtrl1DbgHost->GetValue());
	buildConf->SetDbgHostPort(m_textCtrlDbgPort->GetValue());

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

	//save settings
	ManagerST::Get()->SetProjectSettings(m_projectName, projSettingsPtr);
}

void ProjectSettingsDlg::ConnectEvents()
{
	ConnectChoice(m_choiceConfigurationType, ProjectSettingsDlg::OnConfigurationTypeSelected);
	ConnectCheckBox(m_checkCompilerNeeded, ProjectSettingsDlg::OnCheckCompilerNeeded);
	ConnectCheckBox(m_checkLinkerNeeded, ProjectSettingsDlg::OnCheckLinkerNeeded);
	ConnectButton(m_buttonAddSearchPath, ProjectSettingsDlg::OnAddSearchPath);
	ConnectButton(m_buttonLibraries, ProjectSettingsDlg::OnAddLibrary);
	ConnectButton(m_buttonLibraryPath, ProjectSettingsDlg::OnAddLibraryPath);
	ConnectButton(m_buttonNewPreBuildCmd, ProjectSettingsDlg::OnNewPreBuildCommand);
	ConnectButton(m_buttonEditPreBuildCmd, ProjectSettingsDlg::OnEditPreBuildCommand);
	ConnectButton(m_buttonUpPreBuildCmd, ProjectSettingsDlg::OnUpPreBuildCommand);
	ConnectButton(m_buttonDownPreBuildCmd, ProjectSettingsDlg::OnDownPreBuildCommand);
	ConnectButton(m_buttonDeletePreBuildCmd, ProjectSettingsDlg::OnDeletePreBuildCommand);
	ConnectButton(m_buttonNewPostBuildCmd, ProjectSettingsDlg::OnNewPostBuildCommand);
	ConnectButton(m_buttonEditPostBuildCmd, ProjectSettingsDlg::OnEditPostBuildCommand);
	ConnectButton(m_buttonUpPostBuildCmd, ProjectSettingsDlg::OnUpPostBuildCommand);
	ConnectButton(m_buttonDownPostBuildCmd, ProjectSettingsDlg::OnDownPostBuildCommand);
	ConnectButton(m_buttonDeletePostBuildCmd, ProjectSettingsDlg::OnDeletePostBuildCommand);
	ConnectButton(m_buttonOK, ProjectSettingsDlg::OnButtonOK);
	ConnectButton(m_buttonApply, ProjectSettingsDlg::OnButtonApply);
	ConnectButton(m_buttonConfigManager, ProjectSettingsDlg::OnButtonConfigurationManager);
	ConnectButton(m_buttonAddPreprocessor, ProjectSettingsDlg::OnButtonAddPreprocessor);
	ConnectButton(m_buttonLinkerOptions, ProjectSettingsDlg::OnButtonAddLinkerOptions);
	ConnectButton(m_buttonCompilerOptions, ProjectSettingsDlg::OnButtonAddCompilerOptions);
	ConnectCheckBox(m_checkEnableCustomBuild, ProjectSettingsDlg::OnCustomBuildEnabled);
	ConnectCheckBox(m_checkResourceNeeded, ProjectSettingsDlg::OnResourceCmpNeeded);
	ConnectButton(m_buttonAddResCmpPath, ProjectSettingsDlg::OnResourceCmpAddPath);
	ConnectButton(m_buttonAddResCmpOptions, ProjectSettingsDlg::OnResourceCmpAddOption);
	m_checkListPostBuildCommands->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(ProjectSettingsDlg::OnEditPostBuildCommand), NULL, this);
	m_checkListPreBuildCommands->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(ProjectSettingsDlg::OnEditPreBuildCommand), NULL, this);
}

void ProjectSettingsDlg::OnCustomBuildEnabled(wxCommandEvent &event)
{
	DoUpdatePages(event.IsChecked());
	event.Skip();
}

void ProjectSettingsDlg::DoUpdatePages(bool checked)
{
	DisableCompilerPage(checked);
	DisableLinkerPage(checked);
	DisableGeneralPage(checked);
	DisableCustomBuildPage(!checked);
	DisableCustomMkSteps(checked);

	if (checked) {
		m_checkLinkerNeeded->Enable(false);
		m_checkCompilerNeeded->Enable(false);
		m_postBuildPage->Enable(false);
		m_preBuildPage->Enable(false);
		m_resourceCmpPage->Enable(false);

	} else {

		m_checkLinkerNeeded->Enable(true);
		m_checkCompilerNeeded->Enable(true);
		m_postBuildPage->Enable(true);
		m_preBuildPage->Enable(true);
		m_resourceCmpPage->Enable(true);

	}
}

void ProjectSettingsDlg::DisableCustomBuildPage(bool disable)
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

void ProjectSettingsDlg::OnButtonAddPreprocessor(wxCommandEvent &event)
{
	PopupAddOptionDlg(m_textPreprocessor);
	event.Skip();
}

void ProjectSettingsDlg::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	SaveValues(m_choiceConfigurationType->GetStringSelection());
	EndModal(wxID_OK);
}

void ProjectSettingsDlg::OnButtonApply(wxCommandEvent &event)
{
	wxUnusedVar(event);
	SaveValues(m_choiceConfigurationType->GetStringSelection());
	m_buttonApply->Enable(false);
}

void ProjectSettingsDlg::OnButtonConfigurationManager(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ConfigurationManagerDlg *dlg = new ConfigurationManagerDlg(this);
	dlg->ShowModal();
	dlg->Destroy();

	//update the configuration type choice control
	UpdateConfigurationTypeChoice(m_choiceConfigurationType->GetStringSelection());
	//Load values according to the new selection
	CopyValues(m_choiceConfigurationType->GetStringSelection());
	event.Skip();
}

void ProjectSettingsDlg::OnConfigurationTypeSelected(wxCommandEvent &event)
{
	wxString selection = event.GetString();
	InitDialog(selection, m_oldConfigurationName);
	m_oldConfigurationName = selection;
	event.Skip();
}

void ProjectSettingsDlg::DisableCompilerPage(bool disable)
{
	m_textAdditionalSearchPath->Enable(!disable);
	m_buttonAddSearchPath->Enable(!disable);
	m_textCompilerOptions->Enable(!disable);
	m_textPreprocessor->Enable(!disable);
	m_buttonAddPreprocessor->Enable(!disable);
	m_buttonCompilerOptions->Enable(!disable);
}

void ProjectSettingsDlg::DisableLinkerPage(bool disable)
{
	m_textLibraryPath->Enable(!disable);
	m_textLibraries->Enable(!disable);
	m_textLinkerOptions->Enable(!disable);
	m_buttonLibraries->Enable(!disable);
	m_buttonLibraryPath->Enable(!disable);
	m_buttonLinkerOptions->Enable(!disable);
}

void ProjectSettingsDlg::OnCheckCompilerNeeded(wxCommandEvent &event)
{
	DisableCompilerPage(event.IsChecked());
	event.Skip();
}

void ProjectSettingsDlg::OnCheckLinkerNeeded(wxCommandEvent &event)
{
	DisableLinkerPage(event.IsChecked());
	event.Skip();
}

void ProjectSettingsDlg::PopupAddOptionDlg(wxTextCtrl *ctrl)
{
	AddOptionDlg *dlg = new AddOptionDlg(NULL, ctrl->GetValue());
	if (dlg->ShowModal() == wxID_OK) {
		wxString updatedValue = dlg->GetValue();
		ctrl->SetValue(updatedValue);
	}
	dlg->Destroy();
}

void ProjectSettingsDlg::OnAddSearchPath(wxCommandEvent &event)
{
	PopupAddOptionDlg(m_textAdditionalSearchPath);
	event.Skip();
}

void ProjectSettingsDlg::OnAddLibrary(wxCommandEvent &event)
{
	PopupAddOptionDlg(m_textLibraries);
	event.Skip();
}

void ProjectSettingsDlg::OnAddLibraryPath(wxCommandEvent &event)
{
	PopupAddOptionDlg(m_textLibraryPath);
	event.Skip();
}

void ProjectSettingsDlg::OnNewCommand(wxCheckListBox *list)
{
	FreeTextDialog *dlg = new FreeTextDialog(this);
	if (dlg->ShowModal() == wxID_OK) {
		wxString value = dlg->GetValue();
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
	}
	dlg->Destroy();
}

void ProjectSettingsDlg::OnEditCommand(wxCheckListBox *list)
{
	wxString selectedString  = list->GetStringSelection();
	int sel = list->GetSelection();
	if (sel == wxNOT_FOUND) {
		return;
	}

	FreeTextDialog *dlg = new FreeTextDialog(this, selectedString);
	if (dlg->ShowModal() == wxID_OK) {
		wxString value = dlg->GetValue();
		TrimString(value);
		if (value.IsEmpty() == false) {
			list->SetString(sel, value);
		}
	}
	dlg->Destroy();
}

void ProjectSettingsDlg::OnUpCommand(wxCheckListBox *list)
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
}

void ProjectSettingsDlg::OnDownCommand(wxCheckListBox *list)
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
}

void ProjectSettingsDlg::OnDeleteCommand(wxCheckListBox *list)
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
}

void ProjectSettingsDlg::OnButtonAddCompilerOptions(wxCommandEvent &event)
{
	PopupAddOptionDlg(m_textCompilerOptions);
	event.Skip();
}

void ProjectSettingsDlg::OnButtonAddLinkerOptions(wxCommandEvent &event)
{
	PopupAddOptionDlg(m_textLinkerOptions);
	event.Skip();
}

void ProjectSettingsDlg::OnResourceCmpAddOption(wxCommandEvent &event)
{
	PopupAddOptionDlg(m_textAddResCmpOptions);
	event.Skip();
}

void ProjectSettingsDlg::OnResourceCmpAddPath(wxCommandEvent &event)
{
	PopupAddOptionDlg(m_textAddResCmpPath);
	event.Skip();
}

void ProjectSettingsDlg::OnResourceCmpNeeded(wxCommandEvent &event)
{
	m_textAddResCmpOptions->Enable(!event.IsChecked());
	m_textAddResCmpPath->Enable(!event.IsChecked());
	m_buttonAddResCmpOptions->Enable(!event.IsChecked());
	m_buttonAddResCmpPath->Enable(!event.IsChecked());
	event.Skip();
}

void ProjectSettingsDlg::OnCmdEvtVModified(wxCommandEvent &event)
{
	m_buttonApply->Enable(true);
	event.Skip();
}

void ProjectSettingsDlg::DisableGeneralPage(bool disable)
{
	m_choiceProjectTypes->Enable( !disable );
	m_textOutputFilePicker->Enable( !disable );
	m_textCtrlItermediateDir->Enable( !disable );
	m_buttonBrowseIM_WD->Enable( !disable );
}

void ProjectSettingsDlg::DisableCustomMkSteps(bool disable)
{
	m_textDeps->Enable( !disable );
	m_textPreBuildRule->Enable( !disable );
}

void ProjectSettingsDlg::OnChoiceMakefileTool(wxCommandEvent &e)
{
	if (e.GetString() == wxT("None")) {
		m_textCtrlMakefileGenerationCmd->Enable(false);
	} else {
		m_textCtrlMakefileGenerationCmd->Enable(true);
	}

	OnCmdEvtVModified(e);
}

void ProjectSettingsDlg::OnDebuggingRemoteTarget(wxCommandEvent& e)
{
	if (e.IsChecked()) {
		m_textCtrl1DbgHost->Enable(true);
		m_textCtrlDbgPort->Enable(true);
	} else {
		m_textCtrl1DbgHost->Enable(false);
		m_textCtrlDbgPort->Enable(false);
	}
	OnCmdEvtVModified(e);
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
void ProjectSettingsDlg::OnDeleteTarget(wxCommandEvent& e)
{
	wxUnusedVar(e);
	if (m_selecteCustomTaregt != wxNOT_FOUND) {
		m_listCtrlTargets->DeleteItem(m_selecteCustomTaregt);
		m_selecteCustomTaregt = wxNOT_FOUND;
	}
}

void ProjectSettingsDlg::OnEditTarget(wxCommandEvent& e)
{
	wxUnusedVar(e);
	if (m_selecteCustomTaregt != wxNOT_FOUND) {
		DoEditItem(m_selecteCustomTaregt);
	}
}

void ProjectSettingsDlg::OnNewTarget(wxCommandEvent& e)
{
	wxUnusedVar(e);

	EnvVarDlg dlg(this);
	dlg.SetTitle(wxT("New target"));
	dlg.SetStaticText1(wxT("Target Name:"));
	dlg.SetStaticText2(wxT("Command:"));
	if (dlg.ShowModal() == wxID_OK) {
		if(GetTargetCommand(dlg.GetName()).IsEmpty() == false){
			wxMessageBox(wxString::Format(wxT("Target '%s' already exist!"), dlg.GetName().c_str()), wxT("CodeLite"), wxICON_WARNING|wxCENTER|wxOK, this);
			return;
		}
		long item = AppendListCtrlRow(m_listCtrlTargets);
		DoUpdateItem(item, dlg.GetName(), dlg.GetValue());
	}
}

void ProjectSettingsDlg::OnItemActivated(wxListEvent& e)
{
	m_selecteCustomTaregt = e.m_itemIndex;
	DoEditItem(m_selecteCustomTaregt);
}

void ProjectSettingsDlg::OnItemSelected(wxListEvent& e)
{
	m_selecteCustomTaregt = e.m_itemIndex;
}

void ProjectSettingsDlg::OnDeleteTargetUI(wxUpdateUIEvent& e)
{
	if(m_selecteCustomTaregt != wxNOT_FOUND){
		wxString name = GetColumnText(m_listCtrlTargets, m_selecteCustomTaregt, 0);
		e.Enable(name != CUSTOM_TARGET_BUILD && name != CUSTOM_TARGET_CLEAN && name != CUSTOM_TARGET_COMPILE_SINGLE_FILE && m_checkEnableCustomBuild->IsChecked());
	} else {
		e.Enable(false);
	}
}

void ProjectSettingsDlg::OnEditTargetUI(wxUpdateUIEvent& e)
{
	e.Enable(m_selecteCustomTaregt != wxNOT_FOUND && m_checkEnableCustomBuild->IsChecked());
}

void ProjectSettingsDlg::DoEditItem(long item)
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
		if(target == CUSTOM_TARGET_BUILD || target == CUSTOM_TARGET_CLEAN || target == CUSTOM_TARGET_COMPILE_SINGLE_FILE){
			dlg.DisableName();
		}
		
		if (dlg.ShowModal() == wxID_OK) {
			DoUpdateItem(item, dlg.GetName(), dlg.GetValue());
		}
	}
}

void ProjectSettingsDlg::DoUpdateItem(long item, const wxString& target, const wxString& cmd)
{
	SetColumnText(m_listCtrlTargets, item, 0, target);
	SetColumnText(m_listCtrlTargets, item, 1, cmd);

	if (m_listCtrlTargets->GetItemCount()) {
		m_listCtrlTargets->SetColumnWidth(0, wxLIST_AUTOSIZE);
		m_listCtrlTargets->SetColumnWidth(1, wxLIST_AUTOSIZE);
	}
}

wxString ProjectSettingsDlg::GetTargetCommand(const wxString& target)
{
	for (int i=0; i<(int)m_listCtrlTargets->GetItemCount(); i++) {
		if(GetColumnText(m_listCtrlTargets, i, 0) == target) {
			return GetColumnText(m_listCtrlTargets, i, 1);
		}
	}
	return wxEmptyString;
}

void ProjectSettingsDlg::OnBrowseCustomBuildWD(wxCommandEvent& e)
{
	DirSaver ds;
	
	// Since all paths are relative to the project, set the working directory to the 
	// current project path
	ProjectPtr p = ManagerST::Get()->GetProject(m_projectName);
	if(p){
		wxSetWorkingDirectory(p->GetFileName().GetPath());
	}
	
	wxFileName fn(m_textCtrlCustomBuildWD->GetValue());
	wxString initPath(wxEmptyString);
	
	if(fn.DirExists()) {
		fn.MakeAbsolute();
		initPath = fn.GetFullPath();
	}
	
	wxString new_path = wxDirSelector(wxT("Select working directory:"), initPath, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if(new_path.IsEmpty() == false){
		m_textCtrlCustomBuildWD->SetValue(new_path);
	}
}

void ProjectSettingsDlg::OnBrowseCommandWD(wxCommandEvent& e)
{
	DirSaver ds;
	
	// Since all paths are relative to the project, set the working directory to the 
	// current project path
	ProjectPtr p = ManagerST::Get()->GetProject(m_projectName);
	if(p){
		wxSetWorkingDirectory(p->GetFileName().GetPath());
	}
	
	wxFileName fn(m_textCtrlCommandWD->GetValue());
	wxString initPath(wxEmptyString);
	if(fn.DirExists()) {
		fn.MakeAbsolute();
		initPath = fn.GetFullPath();
	}
	
	wxString new_path = wxDirSelector(wxT("Select working directory:"), initPath, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if(new_path.IsEmpty() == false){
		m_textCtrlCommandWD->SetValue(new_path);
	}
}

void ProjectSettingsDlg::OnBrowseIntermediateDir(wxCommandEvent& e)
{
	DirSaver ds;
	
	// Since all paths are relative to the project, set the working directory to the 
	// current project path
	ProjectPtr p = ManagerST::Get()->GetProject(m_projectName);
	if(p){
		wxSetWorkingDirectory(p->GetFileName().GetPath());
	}

	wxFileName fn(m_textCtrlItermediateDir->GetValue());
	wxString initPath(wxEmptyString);
	
	if(fn.DirExists()) {
		fn.MakeAbsolute();
		initPath = fn.GetFullPath();
	}
	wxString new_path = wxDirSelector(wxT("Select working directory:"), initPath, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if(new_path.IsEmpty() == false){
		m_textCtrlItermediateDir->SetValue(new_path);
	}
}
