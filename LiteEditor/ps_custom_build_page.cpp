#include "ps_custom_build_page.h"
#include "project_settings_dlg.h"
#include "envvar_dlg.h"
#include "dirsaver.h"
#include "project.h"
#include "globals.h"
#include "manager.h"
#include <wx/log.h>

static const wxString CUSTOM_TARGET_BUILD   = wxT("Build");
static const wxString CUSTOM_TARGET_CLEAN   = wxT("Clean");
static const wxString CUSTOM_TARGET_REBUILD = wxT("Rebuild");
static const wxString CUSTOM_TARGET_COMPILE_SINGLE_FILE = wxT("Compile Single File");
static const wxString CUSTOM_TARGET_PREPROCESS_FILE = wxT("Preprocess File");

PSCustomBuildPage::PSCustomBuildPage( wxWindow* parent, const wxString &projectName, ProjectSettingsDlg *dlg  )
	: PSCustomBuildBasePage( parent )
	, m_projectName(projectName)
	, m_selecteCustomTaregt(wxNOT_FOUND)
	, m_dlg(dlg)
{
	m_listCtrlTargets->InsertColumn(0, wxT("Target"));
	m_listCtrlTargets->InsertColumn(1, wxT("Command"));
}

void PSCustomBuildPage::OnCustomBuildEnabled( wxCommandEvent& event )
{
	GetDlg()->SetCustomBuildEnabled(event.IsChecked());
}

void PSCustomBuildPage::OnCustomBuildEnabledUI( wxUpdateUIEvent& event )
{
	event.Enable(GetDlg()->IsCustomBuildEnabled());
}

void PSCustomBuildPage::OnCmdEvtVModified( wxCommandEvent& event )
{
	GetDlg()->SetIsDirty(true);
}

void PSCustomBuildPage::OnBrowseCustomBuildWD( wxCommandEvent& event )
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

void PSCustomBuildPage::OnItemActivated( wxListEvent& event )
{
	m_selecteCustomTaregt = event.m_itemIndex;
	DoEditTarget(m_selecteCustomTaregt);
}

void PSCustomBuildPage::OnItemSelected( wxListEvent& event )
{
	m_selecteCustomTaregt = event.m_itemIndex;
}

void PSCustomBuildPage::OnNewTarget( wxCommandEvent& event )
{
	wxUnusedVar(event);

	EnvVarDlg dlg(this);
	dlg.SetTitle(wxT("New target"));
	dlg.SetStaticText1(wxT("Target Name:"));
	dlg.SetStaticText2(wxT("Command:"));
	if (dlg.ShowModal() == wxID_OK) {
		GetDlg()->SetIsDirty(true);
		if (GetTargetCommand(dlg.GetName()).IsEmpty() == false) {
			wxMessageBox(wxString::Format(wxT("Target '%s' already exist!"), dlg.GetName().c_str()), wxT("CodeLite"), wxICON_WARNING|wxCENTER|wxOK, this);
			return;
		}
		long item = AppendListCtrlRow(m_listCtrlTargets);
		DoUpdateTarget(item, dlg.GetName(), dlg.GetValue());
	}
}

void PSCustomBuildPage::OnEditTarget( wxCommandEvent& event )
{
	wxUnusedVar(event);
	if (m_selecteCustomTaregt != wxNOT_FOUND) {
		DoEditTarget(m_selecteCustomTaregt);
	}
}

void PSCustomBuildPage::OnEditTargetUI( wxUpdateUIEvent& event )
{
	event.Enable(m_selecteCustomTaregt != wxNOT_FOUND && m_checkEnableCustomBuild->IsChecked());
}

void PSCustomBuildPage::OnDeleteTarget( wxCommandEvent& event )
{
	wxUnusedVar(event);
	if (m_selecteCustomTaregt != wxNOT_FOUND) {
		m_listCtrlTargets->DeleteItem(m_selecteCustomTaregt);
		m_selecteCustomTaregt = wxNOT_FOUND;

		GetDlg()->SetIsDirty(true);
	}
}

void PSCustomBuildPage::OnDeleteTargetUI( wxUpdateUIEvent& event )
{
	if (m_selecteCustomTaregt != wxNOT_FOUND) {
		wxString name = GetColumnText(m_listCtrlTargets, m_selecteCustomTaregt, 0);
		event.Enable(name != CUSTOM_TARGET_BUILD               &&
		             name != CUSTOM_TARGET_CLEAN               &&
		             name != CUSTOM_TARGET_REBUILD             &&
		             name != CUSTOM_TARGET_COMPILE_SINGLE_FILE &&
		             m_checkEnableCustomBuild->IsChecked());
	} else {
		event.Enable(false);
	}
}

void PSCustomBuildPage::OnChoiceMakefileTool( wxCommandEvent& event )
{
	if (event.GetString() == wxT("None")) {
		m_textCtrlMakefileGenerationCmd->Enable(false);
	} else {
		m_textCtrlMakefileGenerationCmd->Enable(true);
	}
	GetDlg()->SetIsDirty(true);
}

void PSCustomBuildPage::DoEditTarget(long item)
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
		if (target == CUSTOM_TARGET_BUILD              ||
		    target == CUSTOM_TARGET_CLEAN              ||
		    target == CUSTOM_TARGET_REBUILD            ||
		    target == CUSTOM_TARGET_COMPILE_SINGLE_FILE) {
			dlg.DisableName();
		}
		if (dlg.ShowModal() == wxID_OK) {
			DoUpdateTarget(item, dlg.GetName(), dlg.GetValue());
			GetDlg()->SetIsDirty(true);
		}
	}
}

void PSCustomBuildPage::DoUpdateTarget(long item, const wxString& target, const wxString& cmd)
{
	SetColumnText(m_listCtrlTargets, item, 0, target);
	SetColumnText(m_listCtrlTargets, item, 1, cmd);

	m_listCtrlTargets->SetColumnWidth(0, 150);
	m_listCtrlTargets->SetColumnWidth(1, 300);
}

void PSCustomBuildPage::OnCustomBuildCBEnabledUI(wxUpdateUIEvent& event)
{
	GetDlg()->SetCustomBuildEnabled( m_checkEnableCustomBuild->IsChecked() );
}

ProjectSettingsDlg* PSCustomBuildPage::GetDlg()
{
	return m_dlg;
}

wxString PSCustomBuildPage::GetTargetCommand(const wxString& target)
{
	for (int i=0; i<(int)m_listCtrlTargets->GetItemCount(); i++) {
		if (GetColumnText(m_listCtrlTargets, i, 0) == target) {
			return GetColumnText(m_listCtrlTargets, i, 1);
		}
	}
	return wxEmptyString;
}

void PSCustomBuildPage::Load(BuildConfigPtr buildConf)
{
	m_checkEnableCustomBuild->SetValue(buildConf->IsCustomBuild());
	m_textCtrlCustomBuildWD->SetValue(buildConf->GetCustomBuildWorkingDir());
	m_thirdPartyTool->SetStringSelection(buildConf->GetToolName());
	m_textCtrlMakefileGenerationCmd->SetValue(buildConf->GetMakeGenerationCommand());
	
	m_listCtrlTargets->DeleteAllItems();
	long item = AppendListCtrlRow(m_listCtrlTargets);
	SetColumnText(m_listCtrlTargets, item, 0, CUSTOM_TARGET_BUILD);
	SetColumnText(m_listCtrlTargets, item, 1, buildConf->GetCustomBuildCmd());

	item = AppendListCtrlRow(m_listCtrlTargets);
	SetColumnText(m_listCtrlTargets, item, 0, CUSTOM_TARGET_CLEAN);
	SetColumnText(m_listCtrlTargets, item, 1, buildConf->GetCustomCleanCmd());

	item = AppendListCtrlRow(m_listCtrlTargets);
	SetColumnText(m_listCtrlTargets, item, 0, CUSTOM_TARGET_REBUILD);
	SetColumnText(m_listCtrlTargets, item, 1, buildConf->GetCustomRebuildCmd());

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

		if (titer->first == CUSTOM_TARGET_BUILD               ||
			titer->first == CUSTOM_TARGET_CLEAN               ||
			titer->first == CUSTOM_TARGET_REBUILD             ||
			titer->first == CUSTOM_TARGET_COMPILE_SINGLE_FILE ||
			titer->first == CUSTOM_TARGET_PREPROCESS_FILE) {
			continue;
		}

		item = AppendListCtrlRow(m_listCtrlTargets);
		SetColumnText(m_listCtrlTargets, item, 0, titer->first);
		SetColumnText(m_listCtrlTargets, item, 1, titer->second);
	}

	m_listCtrlTargets->SetColumnWidth(0, 150);
	m_listCtrlTargets->SetColumnWidth(1, 300);
	
	m_dlg->SetCustomBuildEnabled( m_checkEnableCustomBuild->IsChecked() );
}

void PSCustomBuildPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
	// loop over the list and create the targets map
	std::map<wxString, wxString> targets;
	for (int i=0; i<(int)m_listCtrlTargets->GetItemCount(); i++) {
		wxString colText = GetColumnText(m_listCtrlTargets, i, 0);
		if (colText == CUSTOM_TARGET_BUILD               ||
			colText == CUSTOM_TARGET_CLEAN               ||
			colText == CUSTOM_TARGET_REBUILD             ||
			colText == CUSTOM_TARGET_COMPILE_SINGLE_FILE ||
			colText == CUSTOM_TARGET_PREPROCESS_FILE) {
			continue;
		}
		targets[GetColumnText(m_listCtrlTargets, i, 0)] = GetColumnText(m_listCtrlTargets, i, 1);
	}
	
	buildConf->SetCustomTargets(targets);
	buildConf->SetCustomBuildCmd(GetTargetCommand(CUSTOM_TARGET_BUILD));
	buildConf->SetCustomCleanCmd(GetTargetCommand(CUSTOM_TARGET_CLEAN));
	buildConf->SetCustomRebuildCmd(GetTargetCommand(CUSTOM_TARGET_REBUILD));
	buildConf->SetSingleFileBuildCommand(GetTargetCommand(CUSTOM_TARGET_COMPILE_SINGLE_FILE));
	buildConf->SetPreprocessFileCommand(GetTargetCommand(CUSTOM_TARGET_PREPROCESS_FILE));
	buildConf->EnableCustomBuild(m_checkEnableCustomBuild->IsChecked());
	buildConf->SetMakeGenerationCommand(m_textCtrlMakefileGenerationCmd->GetValue());
	buildConf->SetToolName(m_thirdPartyTool->GetStringSelection());
	buildConf->SetCustomBuildWorkingDir(m_textCtrlCustomBuildWD->GetValue());
}

void PSCustomBuildPage::Clear()
{
	m_checkEnableCustomBuild->SetValue(false);
	m_listCtrlTargets->DeleteAllItems();
	m_textCtrlCustomBuildWD->SetValue(wxT("$(WorkspacePath)"));
	m_textCtrlMakefileGenerationCmd->Clear();
}
