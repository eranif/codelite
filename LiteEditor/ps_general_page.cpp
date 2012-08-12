
#include "ps_general_page.h"
#include "project_settings_dlg.h"
#include "build_settings_config.h"
#include "project.h"
#include "dirsaver.h"
#include "manager.h"

PSGeneralPage::PSGeneralPage( wxWindow* parent, const wxString &projectName, const wxString &conf, ProjectSettingsDlg *dlg )
	: PSGeneralPageBase( parent )
	, m_dlg(dlg)
	, m_projectName(projectName)
    , m_configName(conf)
{
}

void PSGeneralPage::OnProjectCustumBuildUI( wxUpdateUIEvent& event )
{
	event.Enable(!m_dlg->IsCustomBuildEnabled());
}

void PSGeneralPage::OnCmdEvtVModified( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_dlg->SetIsDirty(true);
}

void PSGeneralPage::OnBrowseIntermediateDir( wxCommandEvent& event )
{
	DirSaver ds;

	// Since all paths are relative to the project, set the working directory to the
	// current project path
	ProjectPtr p = ManagerST::Get()->GetProject(m_projectName);
	if (p) {
		::wxSetWorkingDirectory(p->GetFileName().GetPath());
	}
    
	wxString new_path = ::wxDirSelector(_("Select working directory:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if (new_path.IsEmpty() == false) {
        wxFileName fn(new_path, wxT(""));
        fn.MakeRelativeTo(); // Make the path relative to the project
        new_path = fn.GetPath();
        new_path.Replace(wxT("\\"), wxT("/"));
        
		m_textCtrlItermediateDir->SetValue(new_path);
	}
}

void PSGeneralPage::OnBrowseProgram( wxCommandEvent& event )
{
    DirSaver ds;
	wxUnusedVar(event);
    
    wxString workingDir = DoGetWorkingDirectory();
    ProjectPtr p = ManagerST::Get()->GetProject(m_projectName);
	if (p) {
		::wxSetWorkingDirectory(p->GetFileName().GetPath());
	} 
    ::wxSetWorkingDirectory(workingDir);
    
	wxString program = ::wxFileSelector(_("Select Program to Run / Debug:"));
	if (program.IsEmpty() == false) {
        wxFileName fn ( program );
        fn.MakeRelativeTo();
        program = fn.GetFullPath();
        program.Replace(wxT("\\"), wxT("/"));
        if( program == fn.GetFullName() ) {
            program.Prepend(wxT("./"));
        }
		m_textCommand->SetValue( program );
	}
}

void PSGeneralPage::OnBrowseCommandWD( wxCommandEvent& event )
{
	DirSaver ds;
    
    // Since all paths are relative to the project, set the working directory to the
	// current project path
	ProjectPtr p = ManagerST::Get()->GetProject(m_projectName);
	if (p) {
		wxSetWorkingDirectory(p->GetFileName().GetPath());
	}
    
	wxString new_path = ::wxDirSelector(_("Select working directory:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if (new_path.IsEmpty() == false) {
        // make it relative to the current directory
        wxFileName fn ( new_path, wxT("") );
        fn.MakeRelativeTo();
        new_path = fn.GetPath();
        new_path.Replace(wxT("\\"), wxT("/"));
		m_textCtrlCommandWD->SetValue( new_path );
	}
}

void PSGeneralPage::Load(BuildConfigPtr buildConf)
{
    m_configName = buildConf->GetName();
    
	m_textOutputFilePicker->SetValue(buildConf->GetOutputFileName());
	m_textCtrlItermediateDir->SetValue(buildConf->GetIntermediateDirectory());
	m_textCommand->SetValue(buildConf->GetCommand());
	m_textCommandArguments->SetValue(buildConf->GetCommandArguments());
	m_textCtrlCommandWD->SetValue(buildConf->GetWorkingDirectory());

	ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
	wxString projType = projSettingsPtr->GetProjectType(buildConf->GetName());
	const wxString ProjectTypes[] = { wxTRANSLATE("Static Library"), wxTRANSLATE("Dynamic Library"), wxTRANSLATE("Executable") };
	m_stringManager.AddStrings(sizeof(ProjectTypes)/sizeof(wxString), ProjectTypes, projType, m_choiceProjectTypes);

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
	m_checkBoxUseDebugArgs->SetValue(buildConf->GetUseSeparateDebugArgs());
	m_textCtrlDebugArgs->SetValue(buildConf->GetDebugArgs());

}

void PSGeneralPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
	buildConf->SetOutputFileName(m_textOutputFilePicker->GetValue());
	buildConf->SetIntermediateDirectory(m_textCtrlItermediateDir->GetValue());
	buildConf->SetCommand(m_textCommand->GetValue());
	buildConf->SetCommandArguments(m_textCommandArguments->GetValue());
	buildConf->SetWorkingDirectory(m_textCtrlCommandWD->GetValue());
	// Get the project type selection, unlocalised
	wxString ProjectType = m_stringManager.GetStringSelection();
	projSettingsPtr->SetProjectType(ProjectType);
	buildConf->SetCompilerType(m_choiceCompilerType->GetStringSelection());
	buildConf->SetDebuggerType(m_choiceDebugger->GetStringSelection());
	buildConf->SetPauseWhenExecEnds(m_checkBoxPauseWhenExecEnds->IsChecked());
	buildConf->SetProjectType(ProjectType);
	buildConf->SetUseSeparateDebugArgs(m_checkBoxUseDebugArgs->IsChecked());
	buildConf->SetDebugArgs(m_textCtrlDebugArgs->GetValue());
}

void PSGeneralPage::Clear()
{
	m_textOutputFilePicker->Clear();
	m_textCtrlItermediateDir->Clear();
	m_textCommand->Clear();
	m_textCommandArguments->Clear();
	m_textCtrlCommandWD->Clear();
	m_checkBoxPauseWhenExecEnds->SetValue(true);
}

void PSGeneralPage::OnUseDebugArgsUI(wxUpdateUIEvent& event)
{
	event.Enable(m_checkBoxUseDebugArgs->IsChecked());
}

wxString PSGeneralPage::DoGetWorkingDirectory()
{
    wxString expr = m_textCtrlCommandWD->GetValue();
    expr = ::ExpandAllVariables(expr, WorkspaceST::Get(), m_projectName, m_configName, wxEmptyString);
    return expr;
}

